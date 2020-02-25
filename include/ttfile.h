/////////////////////////////////////////////////////////////////////////////
// Name:      ttCFile
// Purpose:   class for reading and writing files, strings, data, etc.
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// This class can be used to read from or write to a file and under Windows, a stream, resource or URL
// After reading, the caller can get the data one line at a time, or a pointer to the entire buffer

// If a file is not read, the caller can write lines or strings and when done, write the entire file

// Note that this class only supports ANSI/UTF8 -- it will fail miserably if passed UNICODE strings. Call
// UnicodeToAnsi() if you read a Unicode file.

#pragma once

#include <stdint.h>

#include <string_view>
#include <sstream>

#include "ttstr.h"  // ttCStr

#if defined(_WIN32)
    #include <Wininet.h>
    #include <objidl.h>  // for IStream interface
#endif

// Class for reading and writing files, strings, data, etc.
class ttCFile
{
public:
#if !defined(TTLIB_INTERNAL_BUILD)
    [[deprecated("Use ttlib::textfile instead of this class")]]
#endif
    ttCFile();
    ttCFile(ptrdiff_t cb);
    ~ttCFile();

    typedef enum
    {
        ERROR_NONE,
        ERROR_EMPTY_BUFFER,
        ERROR_CANT_OPEN,
        ERROR_CANT_WRITE,
        ERROR_CANT_READ,
        ERROR_SEEK_FAILURE,
        ERROR_BAD_NAME
    } FILEIO_RESULT;

    // Class functions

    // Default is LF-only EOL. Call SetUnixLF(false) to get CR/LF EOL
    //
    // Only affects writeEol() functions
    void SetUnixLF(bool bUnix = true) { m_fUnixLF = bUnix; }

    // For ReadFile, ReadURL and WriteFile() call GetErrorResult() for ERROR_ info

    bool ReadFile(const ttCStr& cszFilename);
    bool Read(const std::string& filename);
    bool WriteFile(const char* pszFile);

    // Reads a string as if it was a file (makes a copy of the string).
    bool ReadStrFile(const char* pszText);

#if defined(_WIN32)
    bool ReadURL(const char* pszURL,
                 HINTERNET hInternet = NULL);  // ERROR_INVALID_NAME, ERROR_SERVICE_DOES_NOT_EXIST
                                               // if cannot access, ERROR_CANTOPEN if URL not found
    HRESULT ReadFile(IStream* pStream);        // ERROR_INVALID_PARAMETER, ERROR_SEEK_FAILURE, ERROR_CANTREAD
    bool ReadResource(DWORD idResource);
    size_t GetURLFileSize() { return m_cbUrlFile; }
    HRESULT GetErrorResult() { return m_ioResult; }
#endif  // defined(_WIN32)

    // Converts loaded file from Unicode to Ansi. Will return false if file not read.
    bool UnicodeToAnsi();

    // note that this converts \r and/or \n into 0, so you can only read lines once.
    //
    // trim(pszLine) is called before returning
    bool ReadLine(char** ppszLine = nullptr);

    // only needed if you aren't going to call ReadLine
    void PrepForReadLine()
    {
        m_pCurrent = m_pbuf;
        m_pszLine = m_pCurrent;
        m_bReadlineReady = true;
    }
    char* GetLnPtr() { return m_pszLine; }
    bool IsEndOfFile() const { return (!m_pCurrent || !*m_pCurrent) ? true : false; }

    // Returns zero 0 if no line has been successfully read. The first call to ReadLine will
    // set this value to 1, and the value will be incremented with each additional call.
    int GetLastReadLine() { return m_curReadLine; }
    void SetLastReadLine(int line) { m_curReadLine = line; }

    // Returns nullptr if blank, comment, section diveder, or %YAML line. Otherwises returns
    // pointer to first non-space character, stripped of comment and trailing space.
    char* GetParsedYamlLine();

    void WriteStr(const char* psz);
    void WriteChar(char ch);
    void WriteEol(void);
    void WriteEol(const char* psz);

    void WriteText(std::string_view text);
    void WriteText(const std::stringstream& text);

    // Adds a CR/LF only if there isn't one already.
    void AddSingleLF();

    // Use when writing data
    size_t GetCurLineLength();
    char GetPrevChar()
    {
        if (m_pCurrent && m_pCurrent > m_pbuf)
            return m_pCurrent[-1];
        else
            return 0;
    }
    void Backup(size_t cch);
    bool IsThisPreviousString(const char* pszPrev);

    // Resets the current position based on string length of entire buffer.
    void ReCalcSize();

    void cdecl printf(const char* pszFormat, ...);

    // pszPosition derived from previous call to GetCurPosition()
    void InsertStr(const char* pszText, char* pszPosition);
    bool ReplaceStr(const char* pszOldText, const char* pszNewText, bool fCaseSensitive = false);

    // Frees memory, resets pointers.
    void Delete();

    size_t GetCurSize() const { return m_cbAllocated; }
    char* GetBeginPosition() const { return m_pbuf; }
    char* GetEndPosition() const { return m_pEnd; }

    // Used for InsertStr().
    char* GetCurPosition() { return m_pCurrent; }
    bool IsUnicode()
    {
        return (m_pbuf && m_pEnd > m_pbuf + 2 && (BYTE) m_pbuf[0] == 0xFF && (BYTE) m_pbuf[1] == 0xFE);
    }

    void SetCurPosition(char* psz)
    {
        ttASSERT(psz);
        ttASSERT(psz >= m_pbuf);
        ttASSERT(psz <= m_pEnd);
        if (psz >= m_pbuf && psz <= m_pEnd)
            m_pCurrent = psz;
    }

    // Calling readLine() will modify the contents -- which means you can't compare two ttCFile objects if you
    // parsed one with readLine(). To allow for this, call MakeCopy() after you have read the file into memory, and
    // RestoreCopy() if you need to reset the file contents to they way they were before readLine() was called.
    void MakeCopy();

    // Restores the file to the state saved by a previous call to MakeCopy().
    void RestoreCopy();

    // Returns a pointer to the buffer previously saved by a call to MakeCopy().
    char* GetCopy() { return m_pCopy; }

    void AllocateMoreMemory(size_t cbMore = 16 * 1024);

    operator void*() { return (void*) m_pszLine; };
    operator uint8_t*() { return (uint8_t*) m_pszLine; };
    operator char*() const { return m_pszLine; }
    operator const char*() const { return m_pszLine; }

    void operator+=(const char* psz) { WriteStr(psz); }
    void operator=(const char* psz)
    {
        ttASSERT_MSG(psz, "NULL pointer!");
        if (m_pbuf)
            Delete();
        WriteStr(psz);
    }
    char operator[](int pos) { return m_pszLine[pos]; }

    // Use with great caution! Only affects the above operators, and is changed by the next ReadLine() call.
    void SetLnPtr(char* pszLine) { m_pszLine = pszLine; }

    char* m_pszLine;  // Default line pointer when calling ReadLine().

protected:
    void AllocateBuffer(size_t cbInitial = 16 * 1024);

    // Class members

    char* m_pCurrent;
#if !defined(NDEBUG)  // Starts debug section.
    char* m_pszFile;
#endif

    size_t m_cbAllocated;
    size_t m_cbUrlFile;  // Actual file size after call to ReadURL().

    char* m_pbuf;
    char* m_pEnd;

    FILEIO_RESULT m_ioResult;
#if defined(_WIN32)
    HINTERNET m_hInternetSession;
#endif

    char* m_pCopy;

    bool m_bReadlineReady;
    bool m_fUnixLF;

private:
    // 0 means no line has been successfully read, 1 is the first line. Value is incremented
    // with every call to ReadLine.
    int m_curReadLine;
};
