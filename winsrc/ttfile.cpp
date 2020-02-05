/////////////////////////////////////////////////////////////////////////////
// Name:      ttCFile
// Purpose:   class for reading and writing
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <cassert>

#include <stdlib.h>  // for wcstombs_s

#include "../include/ttfile.h"

#if defined(_WIN32)
    #define CHECK_URL_PTR(str)                                               \
        {                                                                    \
            ttASSERT(str);                                                   \
            if (!str || !str[0] || ttStrLen(str) >= INTERNET_MAX_URL_LENGTH) \
            {                                                                \
                m_ioResult = ERROR_BAD_NAME;                                 \
                return false;                                                \
            }                                                                \
        }

    #pragma comment(lib, "Wininet.lib")
#endif  // defined(_WIN32)

// Not unsafe in the way that we use it (first call it to get buffer size needed, then call again with correctly
// sized buffer) #define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable : 4996)  // 'wcstombs': This function or variable may be unsafe.

// CB_END_PAD must be sufficient to allow for a CR/LF, and beginning/ending quotes without overflowing buffer

#define CB_END_PAD       4   // amount of extra bytes before buffer overflow
#define CB_MAX_FMT_WIDTH 20  // Largest formatted width we allow

#ifndef FILENAME_MAX
    #define FILENAME_MAX 260
#endif

#ifndef ERROR_INVALID_NAME
    #define ERROR_INVALID_NAME 123
#endif

#define CHECK_FILE_PTR(str)                                   \
    {                                                         \
        ttASSERT(str);                                        \
        if (!str || !str[0] || ttStrLen(str) >= FILENAME_MAX) \
        {                                                     \
            m_ioResult = ERROR_BAD_NAME;                      \
            return false;                                     \
        }                                                     \
    }

ttCFile::ttCFile()
{
    m_cbAllocated =
        0;  // nothing is allocated until a file is read, or the first output (e.g., writeStr()) is called
    m_pbuf = m_pCopy = nullptr;
    m_pCurrent = nullptr;
    m_pszLine = nullptr;
    m_hInternetSession = nullptr;
    m_bReadlineReady = false;
    m_fUnixLF = true;
    m_curReadLine = 0;
}

ttCFile::ttCFile(ptrdiff_t cb)
{
    // It's likely we're being give the exact file size, and AllocateBuffer() will round up (to nearest 256 byte
    // boundary) The upside to calling AllocateBuffer() and AllocateMoreMemory() is keeping all memory allocation
    // in just two places

    AllocateBuffer(cb);
    m_curReadLine = 0;
}

ttCFile::~ttCFile()
{
#if defined(_WIN32)
    if (m_hInternetSession)
        InternetCloseHandle(m_hInternetSession);
#endif  // defined(_WIN32)
    if (m_pbuf)
        ttFree(m_pbuf);
}

// Memory allocation is always rounded up to the nearest 4K boundary. I.e., if you request 1 byte or 4095 bytes,
// what will actually be allocated is 4096 bytes.

void ttCFile::AllocateBuffer(size_t cbInitial)
{
    ttASSERT_MSG(!m_pbuf, "Buffer already allocated!");
    cbInitial >>= 12;  // remove 1-4095 no matter what bit-width cbInitial is
    cbInitial <<= 12;
    cbInitial += 0x1000;  // round up to nearest 4K byte boundary (1-4095 becomes 4096, 4096-8191 become 8192)
    m_cbAllocated = cbInitial;
    m_pbuf = (char*) ttMalloc(m_cbAllocated);  // won't return on failure
    m_pszLine = m_pCurrent = m_pbuf;
    m_pEnd = m_pbuf + (m_cbAllocated - CB_END_PAD);
}

void ttCFile::AllocateMoreMemory(size_t cbMore)
{
    size_t cOffset = m_pCurrent - m_pbuf;
    cbMore >>= 12;
    cbMore <<= 12;
    cbMore += 0x1000;  // round up to nearest 4k byte boundary
    m_cbAllocated += (cbMore);
    m_pbuf = (char*) ttReAlloc(m_pbuf, m_cbAllocated);
    m_pszLine = m_pbuf;
    m_pCurrent = m_pbuf + cOffset;
    m_pEnd = m_pbuf + (m_cbAllocated - CB_END_PAD);
}

bool ttCFile::WriteFile(const char* pszFile)
{
    CHECK_FILE_PTR(pszFile);                 // returns false on failure
#if !defined(NDEBUG)                         // Starts debug section.
    m_pszFile = ttFindFilePortion(pszFile);  // set this so Debugger will see it
#endif
    ttASSERT_MSG(m_pCurrent > m_pbuf, "Trying to write an empty file!");
    if (m_pCurrent == m_pbuf)
    {
        m_ioResult = ERROR_EMPTY_BUFFER;
        return false;  // we refuse to write an empty file
    }

    HANDLE hf = CreateFileA(pszFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hf == INVALID_HANDLE_VALUE)
    {
        m_ioResult = ERROR_CANT_OPEN;
        return false;
    }

    DWORD cbWritten;
    BOOL  bResult = ::WriteFile(hf, m_pbuf, (DWORD)(m_pCurrent - m_pbuf), &cbWritten, NULL);
    CloseHandle(hf);
    m_ioResult = bResult ? ERROR_NONE : ERROR_CANT_WRITE;
    return bResult ? true : false;
}

bool ttCFile::ReadFile(const ttCStr& cszFilename)
{
    assert(!cszFilename.empty());
    std::string temp(cszFilename);
    return Read(temp);
}

bool ttCFile::Read(const std::string& filename)
{
    Delete();
    if (filename.empty())
    {
        m_ioResult = ERROR_BAD_NAME;
        return false;
    }

    HANDLE hf = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                            FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hf == INVALID_HANDLE_VALUE)
    {
        m_ioResult = ERROR_CANT_OPEN;
        return false;
    }
    DWORD cbFile = GetFileSize(hf, NULL);
    if (cbFile == INVALID_FILE_SIZE)
    {
        m_ioResult = ERROR_SEEK_FAILURE;
        return false;
    }

    if (m_pbuf)
        AllocateMoreMemory((size_t) cbFile + CB_END_PAD);
    else
    {
        AllocateBuffer((size_t) cbFile + CB_END_PAD);
    }

    DWORD cbRead;
    if (!::ReadFile(hf, (void*) m_pCurrent, cbFile, &cbRead, NULL))
    {
        CloseHandle(hf);
        m_ioResult = ERROR_CANT_READ;
        return false;
    }
    CloseHandle(hf);

    ttASSERT_MSG((size_t) cbRead < m_cbAllocated, "Read more bytes than buffer size!");
    m_pCurrent[(size_t) cbRead] = 0;  // null-terminate the file
    m_ioResult = ERROR_NONE;
    m_pCurrent += (size_t) cbRead;  // note that we do NOT change m_pszLine
    return true;
}

bool ttCFile::ReadStrFile(const char* pszText)
{
    Delete();
    ttASSERT_MSG(pszText, "NULL pointer!");
    if (!pszText)
        return false;

    auto cb = ttStrByteLen(pszText);
    AllocateBuffer(cb + CB_END_PAD);
    memcpy(m_pbuf, pszText, cb);
    m_pCurrent = m_pbuf + (cb - 1);
    return true;
}

#if defined(_WIN32)

bool ttCFile::ReadURL(const char* pszURL, HINTERNET hInternet)
{
    Delete();
    m_cbUrlFile = 0;

    const DWORD cbBuffer = (4 * 1024);

    CHECK_URL_PTR(pszURL);  // returns on failure
    if (!hInternet && !m_hInternetSession)
    {
        char szModule[MAX_PATH];
        GetModuleFileNameA(NULL, szModule, sizeof(szModule));
        m_hInternetSession = InternetOpenA(szModule, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
        if (!m_hInternetSession)
        {
            m_ioResult = ERROR_CANT_OPEN;
            return false;
        }
    }
    HINTERNET hURL =
        ::InternetOpenUrlA(hInternet ? hInternet : m_hInternetSession, pszURL, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hURL)
    {
        m_ioResult = ERROR_CANT_OPEN;
        return false;
    }

    // We allocate an additional 2 bytes so that we can NULL terminate the file (in case of ANSI or UNICODE file)

    if (m_pbuf)
        AllocateMoreMemory(cbBuffer + CB_END_PAD);
    else
        AllocateBuffer(cbBuffer + CB_END_PAD);

    DWORD dwBytesRead = 0;
    if (!InternetReadFile(hURL, m_pCurrent, cbBuffer, &dwBytesRead))
    {
        m_ioResult = ERROR_CANT_READ;
        InternetCloseHandle(hURL);
        return false;
    }
    while (dwBytesRead != 0)
    {
        m_cbUrlFile += dwBytesRead;
        m_pCurrent += dwBytesRead;
        AllocateMoreMemory(cbBuffer);
        if (!InternetReadFile(hURL, m_pCurrent, cbBuffer, &dwBytesRead))
        {
            m_ioResult = ERROR_CANT_READ;
            InternetCloseHandle(hURL);
            return false;
        }
    }

    m_pCurrent[0] = 0;  // NULL terminate in case it is a string
    m_pCurrent[1] = 0;  // NULL terminate in case it is a UNICODE string
    InternetCloseHandle(hURL);
    return true;
}

DWORD GetFileSize(IStream* pStream)
{
    LARGE_INTEGER liOffset;
    liOffset.LowPart = 0;
    liOffset.HighPart = 0;
    ULARGE_INTEGER liNewPos;
    HRESULT        hr = pStream->Seek(liOffset, FILE_END, &liNewPos);
    if (FAILED(hr))
    {
        ttASSERT_MSG(!FAILED(hr), "Seek failed");
        return 0;
    }
    pStream->Seek(liOffset, FILE_BEGIN, NULL);
    return liNewPos.LowPart;
}

HRESULT ttCFile::ReadFile(IStream* pStream)  // _WINDOWS_ only
{
    Delete();
    if (!pStream)
        return ERROR_INVALID_PARAMETER;

    DWORD cbFile = GetFileSize(pStream);
    if (!cbFile)
    {
        m_ioResult = ERROR_SEEK_FAILURE;
        return E_FAIL;
    }

    if (m_pbuf)
        AllocateMoreMemory(cbFile + sizeof(char) + CB_END_PAD);
    else
        AllocateBuffer(cbFile + CB_END_PAD);

    ULONG   cbRead;
    HRESULT hr = pStream->Read(m_pCurrent, cbFile, &cbRead);
    if (FAILED(hr))
    {
        ttASSERT_MSG(!FAILED(hr), "Read failed");
        m_ioResult = ERROR_CANT_READ;
        return E_FAIL;
    }
    m_pCurrent[cbRead] = 0;
    m_pCurrent += cbRead;
    m_ioResult = ERROR_NONE;
    return hr;
}

bool ttCFile::ReadResource(DWORD idResource)
{
    Delete();
    HRSRC hrsrc = FindResourceA(GetModuleHandle(NULL), MAKEINTRESOURCEA(idResource), (char*) RT_RCDATA);
    ttASSERT(hrsrc);
    if (!hrsrc)
    {
        m_ioResult = ERROR_CANT_OPEN;
        return false;
    }
    uint32_t cbFile = SizeofResource(GetModuleHandle(NULL), hrsrc);
    HGLOBAL  hglb = LoadResource(GetModuleHandle(NULL), hrsrc);
    ttASSERT(hglb);
    if (!hglb)
    {
        m_ioResult = ERROR_CANT_READ;
        return false;
    }
    if (m_pbuf)
        Delete();
    AllocateBuffer(cbFile);
    memcpy(m_pbuf, LockResource(hglb), cbFile);
    m_pCurrent[cbFile] = 0;  // null-terminate the file
    m_ioResult = ERROR_NONE;
    m_pCurrent += cbFile;  // note that we do NOT change m_pszLine
    return true;
}

#endif  // defined(_WIN32)

void ttCFile::WriteChar(char ch)
{
    ttASSERT(!m_bReadlineReady);
    if (!m_pbuf)
        AllocateBuffer();
    *m_pCurrent++ = ch;
    if (m_pCurrent > m_pEnd)
        AllocateMoreMemory();
}

void ttCFile::WriteEol()  // Write only \n if m_fUnixLF, else \r\n
{
    if (!m_pbuf)
        AllocateBuffer();
    if (!m_fUnixLF)
        *m_pCurrent++ = '\r';
    *m_pCurrent++ = '\n';
    if (m_pCurrent > m_pEnd)
        AllocateMoreMemory();
    *m_pCurrent = 0;
}

void ttCFile::WriteEol(const char* psz)
{
    ttASSERT(!m_bReadlineReady);
    ttASSERT_MSG(psz, "NULL pointer!");
    if (!psz)
        return;

    if (!m_pbuf)
        AllocateBuffer(4097);

    size_t cb = ttStrLen(psz) + 2;  // include room for cr/lf (even if set for Unix -- the extra byte won't hurt)
    if (m_pCurrent + cb > m_pEnd)
        AllocateMoreMemory(max(cb + 1024, 16 * 1024));
    ttStrCpy(m_pCurrent, psz);
    m_pCurrent += (cb - 2);

    if (!m_fUnixLF)
        *m_pCurrent++ = '\r';
    *m_pCurrent++ = '\n';
    *m_pCurrent = 0;
}

void ttCFile::WriteStr(const char* psz)
{
    ttASSERT(!m_bReadlineReady);
    ttASSERT_NONEMPTY(psz);
    if (!psz || !*psz)
        return;
    if (!m_pbuf)
        AllocateBuffer(ttStrLen(psz) + 4);

    size_t cb = ttStrLen(psz);
    if (m_pCurrent + cb + 2 > m_pEnd)
        AllocateMoreMemory(max(cb + 1024, 16 * 1024));
    ttStrCpy(m_pCurrent, psz);
    m_pCurrent += cb;
}

void ttCFile::WriteText(std::string_view text)
{
    if (text.empty())
        return;

    if (m_pCurrent + text.size() + 2 > m_pEnd)
        AllocateMoreMemory(max(text.size() + 1024, 16 * 1024));
    memcpy(m_pCurrent, text.data(), text.size());
    m_pCurrent[text.size()] = 0;
    m_pCurrent += text.size();
}

void ttCFile::WriteText(const std::stringstream& text)
{
    if (text.str().empty())
        return;

    WriteText(text.str());
}

void cdecl ttCFile::printf(const char* pszFormat, ...)
{
    ttASSERT(!m_bReadlineReady);
    ttASSERT_NONEMPTY(pszFormat);
    if (!pszFormat || !*pszFormat)
        return;

    ttCStr  csz;
    va_list argList;
    va_start(argList, pszFormat);
    ttVPrintf(csz.GetPPtr(), pszFormat, argList);
    va_end(argList);

    WriteStr(csz);
}

bool ttCFile::ReadLine(char** ppszLine)
{
    ttASSERT_MSG(m_pbuf, "Attempting to read a line from an empty ttCFile!");
    if (!m_pbuf)
        return false;

    if (!m_bReadlineReady)
    {
        m_pszLine = m_pCurrent = m_pbuf;
        m_bReadlineReady = true;
        m_curReadLine = 0;
    }

    if (!m_pCurrent || !*m_pCurrent)
        return false;

    ++m_curReadLine;

    m_pszLine = m_pCurrent;
    if (ppszLine)
        *ppszLine = m_pCurrent;

    char* pszEndLine = m_pCurrent;
    while (*pszEndLine)
    {
        if (*pszEndLine == '\r')
        {
            m_pCurrent =
                pszEndLine + (pszEndLine[1] == '\n' ? 2 : 1);  // if line ends with \r\n, skip over both characters
            *pszEndLine-- = 0;
            while (pszEndLine >= m_pszLine &&
                   (*pszEndLine == ' ' || *pszEndLine == ' '))  // remove any trailing whitespace
                *pszEndLine-- = 0;
            return true;
        }
        else if (*pszEndLine == '\n')
        {
            m_pCurrent = pszEndLine + 1;
            *pszEndLine-- = 0;
            while (pszEndLine >= m_pszLine &&
                   (*pszEndLine == ' ' || *pszEndLine == ' '))  // remove any trailing whitespace
                *pszEndLine-- = 0;
            return true;
        }
        ++pszEndLine;
    }
    m_pCurrent = pszEndLine;  // We are now at the end
    return true;
}

void ttCFile::Delete()
{
    if (m_pbuf)
        ttFree(m_pbuf);
    if (m_pCopy)
        ttFree(m_pCopy);

    m_pbuf = m_pCopy = nullptr;
    m_pCurrent = nullptr;
    m_pszLine = nullptr;
    m_cbAllocated = 0;
    m_bReadlineReady = false;
}

void ttCFile::InsertStr(const char* pszText, char* pszPosition)
{
    ttASSERT(pszText);
    ttASSERT(*pszText);
    ttASSERT(pszPosition);
    ttASSERT(pszPosition >= m_pbuf);
    ttASSERT(pszPosition <= m_pbuf + m_cbAllocated);

    if (!pszText || !*pszText || !pszPosition || pszPosition < m_pbuf || pszPosition > m_pbuf + m_cbAllocated)
        return;

    size_t    cb = ttStrLen(pszText);
    ptrdiff_t offset = pszPosition - m_pbuf;
    while ((m_pCurrent - m_pbuf) + cb >= m_cbAllocated)
        AllocateMoreMemory();
    pszPosition = m_pbuf + offset;  // because AllocateMoreMemory() may change location
    memmove(pszPosition + cb, pszPosition, ttStrByteLen(pszPosition));
    while (*pszText)  // can't use strCopy() because we don't want the NULL terminator
        *pszPosition++ = *pszText++;
    m_pCurrent += cb;
}

bool ttCFile::ReplaceStr(const char* pszOldText, const char* pszNewText, bool fCaseSensitive)
{
    ttASSERT_MSG(pszOldText, "NULL pointer!");
    ttASSERT(*pszOldText);
    ttASSERT_MSG(!*m_pCurrent,
                 "m_pCurrent does not appear to be pointing to the end of the buffer. Did you call ReadLine?");

    if (!pszNewText)
        pszNewText = "";

    if (!pszOldText || !*pszOldText)
        return false;

    char* pszPos = fCaseSensitive ? ttStrStr(m_pbuf, pszOldText) : ttStrStrI(m_pbuf, pszOldText);
    if (!pszPos)
        return false;

    ttASSERT_MSG(pszPos < m_pCurrent,
                 "m_pCurrent does not appear to be pointing to the end of the buffer. Did you call ReadLine?");
    if (pszPos >= m_pCurrent)
        return false;

    size_t cbOld = ttStrLen(pszOldText);
    size_t cbNew = ttStrLen(pszNewText);

    if (cbNew == 0)  // delete the old text since new text is empty
    {
        ptrdiff_t cb = m_pCurrent - pszPos;
        ttASSERT_MSG(cb > 0, "m_pCurrent does not appear to be pointing to the end of the buffer."
                             " Did you call ReadLine()? You can't use ReplaceStr() if you called ReadLine()");
        memmove(pszPos, pszPos + cbOld, cb);
        m_pCurrent -= cbOld;
        ttASSERT_MSG(!*m_pCurrent, "m_pCurrent did not get changed correctly");
    }

    else if (cbNew == cbOld)
    {
        while (*pszNewText)
        {  // copy and return
            *pszPos++ = *pszNewText++;
        }
        return true;
    }
    else if (cbNew > cbOld)
    {
        while (cbOld--)
        {  // replace the old, insert what's left
            *pszPos++ = *pszNewText++;
        }
        InsertStr(pszNewText, pszPos);
        ttASSERT_MSG(!*m_pCurrent, "m_pCurrent did not get changed correctly");
    }
    else  // new text is shorter
    {
        cbOld -= cbNew;
        while (cbNew--)
        {
            *pszPos++ = *pszNewText++;
        }
        ptrdiff_t cb = m_pCurrent - pszPos;
        ttASSERT_MSG(cb > 0, "m_pCurrent does not appear to be pointing to the end of the buffer."
                             " Did you call ReadLine()? You can't use ReplaceStr() if you called ReadLine()");
        memmove(pszPos, pszPos + cbOld, cb);
        m_pCurrent -= cbOld;
        ttASSERT_MSG(!*m_pCurrent, "m_pCurrent did not get changed correctly");
    }
    return true;
}

size_t ttCFile::GetCurLineLength()
{
    if (!m_pCurrent || !m_pbuf)
        return 0;

    if (m_bReadlineReady && m_pszLine)
        return ttStrLen(m_pszLine);

    const char* pszBeginLine = m_pCurrent;
    while (pszBeginLine > m_pbuf && *pszBeginLine != '\n')
        pszBeginLine--;

    return ttStrLen(pszBeginLine) - 1;
}

bool ttCFile::IsThisPreviousString(const char* pszPrev)
{
    ttASSERT_NONEMPTY(pszPrev);
    ttASSERT(m_pCurrent);

    if (!pszPrev || !*pszPrev || !m_pCurrent)
        return false;

    size_t cb = ttStrLen(pszPrev);

    if (m_pCurrent - cb < m_pbuf)
        return false;
    return ttIsSameStr(m_pCurrent - cb, pszPrev);
}

bool ttCFile::UnicodeToAnsi()
{
    if (!m_pbuf || m_pEnd < m_pbuf + 2 || (uint8_t) m_pbuf[0] != 0xFF || (uint8_t) m_pbuf[1] != 0xFE)
        return false;

    size_t cb = ttStrLen((wchar_t*) (m_pbuf + 2)) * sizeof(wchar_t);
    size_t cbLen;
    int    err = wcstombs_s(&cbLen, nullptr, cb, (wchar_t*) (m_pbuf + 2), cb);
    // cb = wcstombs(nullptr, (wchar_t*) (m_pbuf + 2), cb);
    ttASSERT(err != -1);
    if (err == -1)
        return false;

    char* psz = (char*) ttMalloc(cbLen + 1);
    err = wcstombs_s(&cbLen, psz, cbLen, (wchar_t*) (m_pbuf + 2), cb);
    psz[cb] = '\0';

    ttFree(m_pbuf);
    m_pbuf = psz;
    m_pszLine = m_pCurrent = m_pbuf;
    m_pEnd = m_pbuf + cb;
    m_pCurrent = m_pEnd;
    return true;
}

// Add a single EOL to the current buffer. Do this by first backing up over any trailing whitespace. Then see if we
// already have a EOL, and if not, add one.

void ttCFile::AddSingleLF()
{
    if (!m_pCurrent)
        return;
    if (m_pCurrent == m_pbuf)
    {
        WriteEol();
        return;
    }
    m_pCurrent--;
    while (m_pCurrent > m_pbuf && (*m_pCurrent == ' ' || *m_pCurrent == '\t'))
        m_pCurrent--;
    if (m_pCurrent == m_pbuf)
        WriteEol();
    else if (*m_pCurrent != '\n')
    {
        m_pCurrent++;
        WriteEol();
    }
    else
        m_pCurrent++;
}

void ttCFile::ReCalcSize()
{
    if (m_pbuf)
        m_pCurrent = m_pbuf + ttStrLen(m_pbuf);
}

void ttCFile::Backup(size_t cch)
{
    ttASSERT(m_pCurrent);
    if (!m_pCurrent)
        return;
    if (m_pCurrent - cch > m_pbuf)
        m_pCurrent -= cch;
    memset(m_pCurrent, 0, cch);
}

char* ttCFile::GetParsedYamlLine()
{
    if (!m_bReadlineReady)
        ReadLine();
    ttASSERT_MSG(m_bReadlineReady, "Attempting to call GetParsedYamlLine() without a properly read file!");

    const char* pszLine = ttFindNonSpace(m_pszLine);  // ignore any leading spaces
    if (ttIsSameSubStrI(pszLine, "%YAML"))
        return nullptr;

    // ignore empty, comment or divider lines
    if (ttIsEmpty(pszLine) || pszLine[0] == '#' || (pszLine[0] == '-' && pszLine[1] == '-' && pszLine[2] == '-'))
        return nullptr;

    char* pszComment = ttStrChr(pszLine, '#');  // strip off any comments
    if (pszComment)
        *pszComment = 0;

    ttTrimRight((char*) pszLine);  // remove any trailing white space

    return (char*) pszLine;
}

void ttCFile::MakeCopy()
{
    ttASSERT_MSG(m_pbuf, "You must read a file before calling MakeCopy()!");
    ttASSERT_MSG(!m_pCopy, "You have already created a copy and not called Delete() or RestoreCopy()");
    if (!m_pCopy && m_pbuf)
        m_pCopy = ttStrDup(m_pbuf);
}

void ttCFile::RestoreCopy()
{
    ttASSERT_MSG(m_pCopy,
                 "No copy available -- either MakeCopy() wasn't called, or RestoreCopy() has been called.");

    if (!m_pCopy)
        return;

    if (m_pbuf)
        ttFree(m_pbuf);
    m_pbuf = m_pCopy;
    m_pCopy = nullptr;

    m_bReadlineReady = false;

    // We allocated the buffer using StrDup(), so we can't be sure ttSize(m_pbuf) will be the exact size, and
    // m_pCurrent has to point to the null-terminating character. We're stuck with ttStrByteLen()

    m_cbAllocated = ttStrByteLen(m_pbuf);
    m_pCurrent = m_pbuf + m_cbAllocated - sizeof(char);
    m_pszLine = m_pbuf;
}
