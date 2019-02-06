/////////////////////////////////////////////////////////////////////////////
// Name:		ttFile
// Purpose:		class for reading and writing
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <stdlib.h>	// for wcstombs_s

#ifdef _WX_WX_H_
	#include <wx/file.h>
#endif

#include "../include/ttfile.h"
#include "../include/ttstr.h"	// ttStr

#ifdef _WINDOWS_
	#define CHECK_URL_PTR(str)	{ ttASSERT(str); if (!str || !str[0] || tt::strlen(str) >= INTERNET_MAX_URL_LENGTH) { m_ioResult = ERROR_BAD_NAME; return false; } }

	#pragma comment(lib, "Wininet.lib")
#endif

// Not unsafe in the way that we use it (first call it to get buffer size needed, then call again with correctly sized buffer)
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable: 4996)	// 'wcstombs': This function or variable may be unsafe.

// CB_END_PAD must be sufficient to allow for a CR/LF, and beginning/ending quotes without overflowing buffer

#define CB_END_PAD 4			// amount of extra bytes before buffer overflow
#define CB_MAX_FMT_WIDTH 20		// Largest formatted width we allow

#ifndef FILENAME_MAX
	#define FILENAME_MAX 260
#endif

#ifndef ERROR_INVALID_NAME
	#define ERROR_INVALID_NAME 123
#endif

#define CHECK_FILE_PTR(str) { ttASSERT(str); if (!str || !str[0] || tt::strlen(str) >= FILENAME_MAX) { m_ioResult = ERROR_BAD_NAME; return false; } }

ttFile::ttFile()
{
	m_cbAllocated = 0;	// nothing is allocated until a file is read, or the first output (e.g., WriteStr()) is called
	m_pbuf = m_pCopy = nullptr;
	m_pCurrent = nullptr;
	m_pszLine = nullptr;
	m_hInternetSession = nullptr;
	m_bReadlineReady = false;
	m_fUnixLF = true;
}

ttFile::ttFile(ptrdiff_t cb)
{
	// It's likely we're being give the exact file size, and AllocateBuffer() will round up (to nearest 256 byte boundary)
	// The upside to calling AllocateBuffer() and AllocateMoreMemory() is keeping all memory allocation in just two places

	AllocateBuffer(cb);
}

ttFile::~ttFile()
{
#ifdef _WINDOWS_
	if (m_hInternetSession)
		InternetCloseHandle(m_hInternetSession);
#endif
	if (m_pbuf)
		tt::free(m_pbuf);
}

// Memory allocation is always rounded up to the nearest 4K boundary. I.e., if you request 1 byte or 4095 bytes, what will actually
// be allocated is 4096 bytes.

void ttFile::AllocateBuffer(size_t cbInitial)
{
	ttASSERT_MSG(!m_pbuf, "Buffer already allocated!");
	cbInitial >>= 12;		// remove 1-4095 no matter what bit-width cbInitial is
	cbInitial <<= 12;
	cbInitial += 0x1000;	// round up to nearest 4K byte boundary (1-4095 becomes 4096, 4096-8191 become 8192)
	m_cbAllocated = cbInitial;
	m_pbuf = (char*) tt::malloc(m_cbAllocated);	// won't return on failure
	m_pszLine = m_pCurrent = m_pbuf;
	m_pEnd = m_pbuf + (m_cbAllocated - CB_END_PAD);
}

void ttFile::AllocateMoreMemory(size_t cbMore)
{
	size_t cOffset = m_pCurrent - m_pbuf;
	cbMore >>= 12;
	cbMore <<= 12;
	cbMore += 0x1000;	// round up to nearest 4k byte boundary
	m_cbAllocated += (cbMore);
	m_pbuf = (char*) tt::realloc(m_pbuf, m_cbAllocated);
	m_pszLine = m_pbuf;
	m_pCurrent = m_pbuf + cOffset;
	m_pEnd = m_pbuf + (m_cbAllocated - CB_END_PAD);
}

bool ttFile::WriteFile(const char* pszFile)
{
	CHECK_FILE_PTR(pszFile);	// returns false on failure
#ifdef _DEBUG
	m_pszFile = tt::fndFilename(pszFile);	// set this so Debugger will see it
#endif
	ttASSERT_MSG(m_pCurrent > m_pbuf, "Trying to write an empty file!");
	if (m_pCurrent == m_pbuf) {
		m_ioResult = ERROR_EMPTY_BUFFER;
		return false;	// we refuse to write an empty file
	}

#ifdef _WX_WX_H_
	wxFile file(pszFile, wxFile::write);

	if (!file.IsOpened()) {
		m_ioResult = ERROR_CANT_OPEN;
		return false;
	}

	ptrdiff_t cbWritten = file.Write(m_pbuf, m_pCurrent - m_pbuf);
	m_ioResult = cbWritten == (m_pCurrent - m_pbuf) ? ERROR_NONE : ERROR_CANT_WRITE;
	return m_ioResult == ERROR_NONE ? true : false;
#else
	HANDLE hf = CreateFile(pszFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hf == INVALID_HANDLE_VALUE)	{
		m_ioResult = ERROR_CANT_OPEN;
		return false;
	}

	DWORD cbWritten;
	BOOL bResult = ::WriteFile(hf, m_pbuf, (DWORD) (m_pCurrent - m_pbuf), &cbWritten, NULL);
	CloseHandle(hf);
	m_ioResult = bResult ? ERROR_NONE : ERROR_CANT_WRITE;
	return bResult ? true : false;
#endif
}

bool ttFile::ReadFile(const char* pszFile)
{
	Delete();
	CHECK_FILE_PTR(pszFile);	// returns on failure
#ifdef _DEBUG
	m_pszFile = tt::fndFilename(pszFile);	// set this so Debugger will see it
#endif

#ifdef _WX_WX_H_
	wxFile file(pszFile, wxFile::read);
	if (!file.IsOpened()) {
		m_ioResult = ERROR_CANT_OPEN;
		return false;
	}

	wxFileOffset cbFile = file.Length();
	if (cbFile == wxInvalidOffset) {
		m_ioResult = ERROR_SEEK_FAILURE;
		return false;
	}
#else	// not _WX_WX_H_
	HANDLE hf = CreateFile(pszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hf == INVALID_HANDLE_VALUE) {
		m_ioResult = ERROR_CANT_OPEN;
		return false;
	}
	DWORD cbFile = GetFileSize(hf, NULL);
	if (cbFile == INVALID_FILE_SIZE) {
		m_ioResult = ERROR_SEEK_FAILURE;
		return false;
	}
#endif	// _WX_WX_H_

	if (m_pbuf)
		AllocateMoreMemory((size_t) cbFile + CB_END_PAD);
	else {
		AllocateBuffer((size_t) cbFile + CB_END_PAD);
	}

#ifdef _WX_WX_H_
	wxFileOffset cbRead = file.Read((void*) m_pCurrent, (size_t) cbFile);
	if (cbRead == wxInvalidOffset) {
		m_ioResult = ERROR_CANT_READ;
		return false;
	}
#else	// not _WX_WX_H_
	DWORD cbRead;
	if (!::ReadFile(hf, (void*) m_pCurrent, cbFile, &cbRead, NULL)) {
		CloseHandle(hf);
		m_ioResult = ERROR_CANT_READ;
		return false;
	}
	CloseHandle(hf);
#endif	// _WX_WX_H_

	ttASSERT_MSG((size_t) cbRead < m_cbAllocated, "Read more bytes than buffer size!");
	m_pCurrent[(size_t) cbRead] = 0;	// null-terminate the file
	m_ioResult = ERROR_NONE;
	m_pCurrent += (size_t) cbRead;	// note that we do NOT change m_pszLine
	return true;
}

#ifdef _WINDOWS_
bool ttFile::ReadURL(const char* pszURL, HINTERNET hInternet)	// _WINDOWS_ only
{
	Delete();
	m_cbUrlFile = 0;

	const DWORD cbBuffer = (4 * 1024);

	CHECK_URL_PTR(pszURL);	// returns on failure
	if (!hInternet && !m_hInternetSession) {
		char szModule[MAX_PATH];
		GetModuleFileNameA(NULL, szModule, sizeof(szModule));
		m_hInternetSession = InternetOpenA(szModule, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (!m_hInternetSession) {
			m_ioResult = ERROR_CANT_OPEN;
			return false;
		}
	}
	HINTERNET hURL = ::InternetOpenUrlA(hInternet ? hInternet : m_hInternetSession, pszURL, NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if (!hURL) {
		m_ioResult = ERROR_CANT_OPEN;
		return false;
	}

	// We allocate an additional 2 bytes so that we can NULL terminate the file (in case of ANSI or UNICODE file)

	if (m_pbuf)
		AllocateMoreMemory(cbBuffer + CB_END_PAD);
	else {
		AllocateBuffer(cbBuffer + CB_END_PAD);
	}
	DWORD dwBytesRead = 0;

	if (!InternetReadFile(hURL, m_pCurrent, cbBuffer, &dwBytesRead)) {
		m_ioResult = ERROR_CANT_READ;
		InternetCloseHandle(hURL);
		return false;
	}
	while (dwBytesRead != 0) {
		m_cbUrlFile += dwBytesRead;
		m_pCurrent += dwBytesRead;
		AllocateMoreMemory(cbBuffer);
		if (!InternetReadFile(hURL, m_pCurrent, cbBuffer, &dwBytesRead)) {
			m_ioResult = ERROR_CANT_READ;
			InternetCloseHandle(hURL);
			return false;
		}
	}

	m_pCurrent[0] = 0;	// NULL terminate in case it is a string
	m_pCurrent[1] = 0;	// NULL terminate in case it is a UNICODE string
	InternetCloseHandle(hURL);
	return true;
}

__inline DWORD GetFileSize(IStream* pStream)	// _WINDOWS_ only
{
	LARGE_INTEGER liOffset;
	liOffset.LowPart = 0;
	liOffset.HighPart = 0;
	ULARGE_INTEGER liNewPos;
	HRESULT hr = pStream->Seek(liOffset, FILE_END, &liNewPos);
	if (FAILED(hr)) {
		ttASSERT_MSG(!FAILED(hr), "Seek failed");
		return 0;
	}
	pStream->Seek(liOffset, FILE_BEGIN, NULL);
	return liNewPos.LowPart;
}

HRESULT ttFile::ReadFile(IStream* pStream)	// _WINDOWS_ only
{
	Delete();
	if (!pStream)
		return ERROR_INVALID_PARAMETER;

	DWORD cbFile = GetFileSize(pStream);
	if (!cbFile) {
		m_ioResult = ERROR_SEEK_FAILURE;
		return E_FAIL;
	}

	if (m_pbuf)
		AllocateMoreMemory(cbFile + sizeof(char) + CB_END_PAD);
	else {
		AllocateBuffer(cbFile + CB_END_PAD);
	}
	ULONG cbRead;
	HRESULT hr = pStream->Read(m_pCurrent, cbFile, &cbRead);
	if (FAILED(hr)) {
		ttASSERT_MSG(!FAILED(hr), "Read failed");
		m_ioResult = ERROR_CANT_READ;
		return E_FAIL;
	}
	m_pCurrent[cbRead] = 0;
	m_pCurrent += cbRead;
	m_ioResult = ERROR_NONE;
	return hr;
}

bool ttFile::ReadResource(DWORD idResource)	// _WINDOWS_ only
{
	Delete();
	HRSRC hrsrc	 = FindResource(tt::hinstResources, MAKEINTRESOURCE(idResource), RT_RCDATA);
	ttASSERT(hrsrc);
	if (!hrsrc) {
		m_ioResult = ERROR_CANT_OPEN;
		return false;
	}
	uint32_t cbFile = SizeofResource(tt::hinstResources, hrsrc);
	HGLOBAL hglb = LoadResource(tt::hinstResources, hrsrc);
	ttASSERT(hglb);
	if (!hglb) {
		m_ioResult = ERROR_CANT_READ;
		return false;
	}
	if (m_pbuf)
		Delete();
	AllocateBuffer(cbFile);
	memcpy(m_pbuf, LockResource(hglb), cbFile);
	m_pCurrent[cbFile] = 0;	// null-terminate the file
	m_ioResult = ERROR_NONE;
	m_pCurrent += cbFile;	// note that we do NOT change m_pszLine
	return true;
}

#endif	// _WINDOWS_

void ttFile::WriteChar(char ch)
{
	ttASSERT(!m_bReadlineReady);
	if (!m_pbuf)
		AllocateBuffer();
	*m_pCurrent++ = ch;
	if (m_pCurrent > m_pEnd)
		AllocateMoreMemory();
}

void ttFile::WriteEol()	// Write only \n if m_fUnixLF, else \r\n
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

void ttFile::WriteEol(const char* psz)
{
	ttASSERT(!m_bReadlineReady);
	ttASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return;

	if (!m_pbuf)
		AllocateBuffer(4097);

	size_t cb = tt::strlen(psz) + 2;	// include room for cr/lf (even if set for Unix -- the extra byte won't hurt)
	if (m_pCurrent + cb > m_pEnd)
		AllocateMoreMemory(max(cb + 1024, 16 * 1024));
	tt::strcpy(m_pCurrent, psz);
	m_pCurrent += (cb - 2);

	if (!m_fUnixLF)
		*m_pCurrent++ = '\r';
	*m_pCurrent++ = '\n';
	*m_pCurrent = 0;
}

void ttFile::WriteStr(const char* psz)
{
	ttASSERT(!m_bReadlineReady);
	ttASSERT_NONEMPTY(psz);
	if (!psz || !*psz)
		return;
	if (!m_pbuf)
		AllocateBuffer(strlen(psz) + 4);

	size_t cb = tt::strlen(psz);
	if (m_pCurrent + cb + 2 > m_pEnd)
		AllocateMoreMemory(max(cb + 1024, 16 * 1024));
	tt::strcpy(m_pCurrent, psz);
	m_pCurrent += cb;
}

void cdecl ttFile::printf(const char* pszFormat, ...)
{
	ttASSERT(!m_bReadlineReady);
	ttASSERT_NONEMPTY(pszFormat);
	if (!pszFormat || !*pszFormat)
		return;

	ttStr csz;
	va_list argList;
	va_start(argList, pszFormat);
	tt::vprintf(&csz.m_psz, pszFormat, argList);
	va_end(argList);

	WriteStr(csz);
}

bool ttFile::readline(char** ppszLine)
{
	ttASSERT_MSG(m_pbuf, "Attempting to read a line from an empty ttFile!");
	if (!m_pbuf)
		return false;

	if (!m_bReadlineReady) {
		m_pszLine = m_pCurrent = m_pbuf;
		m_bReadlineReady = true;
	}

	if (!m_pCurrent || !*m_pCurrent)
		return false;

	m_pszLine = m_pCurrent;
	if (ppszLine)
		*ppszLine = m_pCurrent;

	char* pszEndLine = m_pCurrent;
	while (*pszEndLine) {
		if (*pszEndLine == '\r') {
			m_pCurrent = pszEndLine + (pszEndLine[1] == '\n' ? 2 : 1);	// if line ends with \r\n, skip over both characters
			*pszEndLine-- = 0;
			while (pszEndLine >= m_pszLine && (*pszEndLine == ' ' || *pszEndLine == ' '))	// remove any trailing whitespace
				*pszEndLine-- = 0;
			return true;
		}
		else if (*pszEndLine == '\n') {
			m_pCurrent = pszEndLine + 1;
			*pszEndLine-- = 0;
			while (pszEndLine >= m_pszLine && (*pszEndLine == ' ' || *pszEndLine == ' '))	// remove any trailing whitespace
				*pszEndLine-- = 0;
			return true;
		}
		++pszEndLine;
	}
	m_pCurrent = pszEndLine;	// We are now at the end
	return true;
}

void ttFile::Delete()
{
	if (m_pbuf)
		tt::free(m_pbuf);
	if (m_pCopy)
		tt::free(m_pCopy);

	m_pbuf = m_pCopy = nullptr;
	m_pCurrent = nullptr;
	m_pszLine = nullptr;
	m_cbAllocated = 0;
	m_bReadlineReady = false;
}

void ttFile::InsertStr(const char* pszText, char* pszPosition)
{
	ttASSERT(pszText);
	ttASSERT(*pszText);
	ttASSERT(pszPosition);
	ttASSERT(pszPosition >= m_pbuf);
	ttASSERT(pszPosition <= m_pbuf + m_cbAllocated);

	if (!pszText || !*pszText || !pszPosition || pszPosition < m_pbuf || pszPosition > m_pbuf + m_cbAllocated)
		return;

	size_t cb = tt::strlen(pszText);
	ptrdiff_t offset = pszPosition - m_pbuf;
	while ((m_pCurrent - m_pbuf) + cb >= m_cbAllocated)
		AllocateMoreMemory();
	pszPosition = m_pbuf + offset;	// because AllocateMoreMemory() may change location
	memmove(pszPosition + cb, pszPosition, strlen(pszPosition) + 1);
	while (*pszText)	// can't use strcpy() because we don't want the NULL terminator
		*pszPosition++ = *pszText++;
	m_pCurrent += cb;
}

bool ttFile::ReplaceStr(const char* pszOldText, const char* pszNewText, bool fCaseSensitive)
{
	ttASSERT_MSG(pszOldText, "NULL pointer!");
	ttASSERT(*pszOldText);
	ttASSERT(pszNewText);
	ttASSERT_MSG(!*m_pCurrent, "m_pCurrent does not appear to be pointing to the end of the buffer. Did you call readline?");

	if (!pszOldText || !*pszOldText || !pszNewText)
		return false;

	char* pszPos = fCaseSensitive ? tt::strstr(m_pbuf, pszOldText) : tt::stristr(m_pbuf, pszOldText);
	if (!pszPos)
		return false;

	ttASSERT_MSG(pszPos < m_pCurrent, "m_pCurrent does not appear to be pointing to the end of the buffer. Did you call readline?");
	if (pszPos >= m_pCurrent)
		return false;

	size_t cbOld = tt::strlen(pszOldText);
	size_t cbNew = tt::strlen(pszNewText);

	if (cbNew == 0) {	// delete the old text since new text is empty
		ptrdiff_t cb = m_pCurrent - pszPos;
		ttASSERT_MSG(cb > 0, "m_pCurrent does not appear to be pointing to the end of the buffer. Did you call readline()? You can't use ReplaceStr() if you called readline()");
		memmove(pszPos, pszPos + cbOld, cb);
		m_pCurrent -= cbOld;
		ttASSERT_MSG(!*m_pCurrent, "m_pCurrent did not get changed correctly");
	}

	else if (cbNew == cbOld) {
		while (*pszNewText) {	// copy and return
			*pszPos++ = *pszNewText++;
		}
		return true;
	}
	else if (cbNew > cbOld) {
		while (cbOld--) {	// replace the old, insert what's left
			*pszPos++ = *pszNewText++;
		}
		InsertStr(pszNewText, pszPos);
		ttASSERT_MSG(!*m_pCurrent, "m_pCurrent did not get changed correctly");
	}
	else {	// new text is shorter
		cbOld -= cbNew;
		while (cbNew--) {
			*pszPos++ = *pszNewText++;
		}
		ptrdiff_t cb = m_pCurrent - pszPos;
		ttASSERT_MSG(cb > 0, "m_pCurrent does not appear to be pointing to the end of the buffer. Did you call readline()? You can't use ReplaceStr() if you called readline()");
		memmove(pszPos, pszPos + cbOld, cb);
		m_pCurrent -= cbOld;
		ttASSERT_MSG(!*m_pCurrent, "m_pCurrent did not get changed correctly");
	}
	return true;
}

size_t ttFile::GetCurLineLength()
{
	if (!m_pCurrent || !m_pbuf)
		return 0;

	if (m_bReadlineReady && m_pszLine)
		return tt::strlen(m_pszLine);

	const char* pszBeginLine = m_pCurrent;
	while (pszBeginLine > m_pbuf && *pszBeginLine != '\n')
		pszBeginLine--;

	return tt::strlen(pszBeginLine) - 1;
}

bool ttFile::isThisPreviousString(const char* pszPrev)
{
	ttASSERT_NONEMPTY(pszPrev);
	ttASSERT(m_pCurrent);

	if (!pszPrev || !*pszPrev || !m_pCurrent)
		return false;

	size_t cb = tt::strlen(pszPrev);

	if (m_pCurrent - cb < m_pbuf)
		return false;
	return tt::samestr(m_pCurrent - cb, pszPrev);
}

bool ttFile::UnicodeToAnsi()
{
	if (!m_pbuf || m_pEnd < m_pbuf + 2 || (uint8_t) m_pbuf[0] != 0xFF || (uint8_t) m_pbuf[1] != 0xFE)
		return false;

	size_t cb = tt::strlen((wchar_t*) (m_pbuf + 2)) * sizeof(wchar_t);
	size_t cbLen;
	int err = wcstombs_s(&cbLen, nullptr, cb, (wchar_t*) (m_pbuf + 2), cb);
	// cb = wcstombs(nullptr, (wchar_t*) (m_pbuf + 2), cb);
	ttASSERT(err != -1);
	if (err == -1)
		return false;

	char* psz = (char*) tt::malloc(cbLen + 1);
	err = wcstombs_s(&cbLen, psz, cbLen, (wchar_t*) (m_pbuf + 2), cb);
	psz[cb] = '\0';

	tt::free(m_pbuf);
	m_pbuf = psz;
	m_pszLine = m_pCurrent = m_pbuf;
	m_pEnd = m_pbuf + cb;
	m_pCurrent = m_pEnd;
	return true;
}

// Add a single EOL to the current buffer. Do this by first backing up over any trailing whitespace. Then see if we already
// have a EOL, and if not, add one.

void ttFile::AddSingleLF()
{
	if (!m_pCurrent)
		return;
	if (m_pCurrent == m_pbuf) {
		WriteEol();
		return;
	}
	m_pCurrent--;
	while (m_pCurrent > m_pbuf && (*m_pCurrent == ' ' || *m_pCurrent == '\t'))
		m_pCurrent--;
	if (m_pCurrent == m_pbuf)
		WriteEol();
	else if (*m_pCurrent != '\n') {
		m_pCurrent++;
		WriteEol();
	}
	else
		m_pCurrent++;
}

void ttFile::ReCalcSize()
{
	if (m_pbuf)
		m_pCurrent = m_pbuf + tt::strlen(m_pbuf);
}

void ttFile::Backup(size_t cch)
{
	ttASSERT(m_pCurrent);
	if (!m_pCurrent)
		return;
	if (m_pCurrent - cch > m_pbuf)
		m_pCurrent -= cch;
	memset(m_pCurrent, 0, cch);
}

char* ttFile::GetParsedYamlLine()
{
	if (!m_bReadlineReady)
		readline();
	ttASSERT_MSG(m_bReadlineReady, "Attempting to call GetParsedYamlLine() without a properly read file!");

	const char* pszLine = tt::nextnonspace(m_pszLine);	// ignore any leading spaces
	if (tt::samesubstri(pszLine, "%YAML"))
		return nullptr;

	if (tt::isempty(pszLine) || pszLine[0] == '#' || (pszLine[0] == '-' && pszLine[1] == '-' && pszLine[2] == '-'))	// ignore empty, comment or divider lines
		return nullptr;

	char* pszComment = tt::strchr(pszLine, '#');	// strip off any comments
	if (pszComment)
		*pszComment = 0;

	tt::trim_right((char*) pszLine);		// remove any trailing white space

	return (char*) pszLine;
}

void ttFile::MakeCopy()
{
	ttASSERT_MSG(m_pbuf, "You must read a file before calling MakeCopy()!");
	ttASSERT_MSG(!m_pCopy, "You have already created a copy and not called Delete() or RestoreCopy()");
	if (!m_pCopy && m_pbuf)
		m_pCopy = tt::strdup(m_pbuf);
}

void ttFile::RestoreCopy()
{
	ttASSERT_MSG(m_pCopy, "No copy available -- either MakeCopy() wasn't called, or RestoreCopy() has been called.");

	if (!m_pCopy)
		return;

	if (m_pbuf)
		tt::free(m_pbuf);
	m_pbuf = m_pCopy;
	m_pCopy = nullptr;

	m_bReadlineReady = false;

	// We allocated the buffer using strdup(), so we can't be sure tt::size(m_pbuf) will be the exact size, and
	// m_pCurrent has to point to the null-terminating character. We're stuck with tt::strbyte()

	m_cbAllocated = tt::strbyte(m_pbuf);
	m_pCurrent = m_pbuf + m_cbAllocated - sizeof(char);
	m_pszLine = m_pbuf;
}
