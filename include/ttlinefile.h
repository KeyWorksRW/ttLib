/////////////////////////////////////////////////////////////////////////////
// Name:      ttCLineFile
// Purpose:   Line-oriented file class
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

// Note that lines are zero-based. The first line is accessed as 0.

// It's fine to call AddLine() before calling ReadFile(), however you can only call ReadFile() once.

#include "../include/ttfile.h"  // ttCFile
#include "../include/ttstr.h"   // ttCStr

class ttCLineFile : public ttCHeap
{
public:
    ttCLineFile();
    ttCLineFile(HANDLE hHeap);

    // Public functions

    void AddLine(const char* pszLine);  // Adds to the end of the file
    void DeleteLine(int line);
    void InsertLine(int line, const char* pszLine);  // Insert before line
    void ReplaceLine(int line, const char* pszLine);

    int  GetLineNumber() { return m_curLine; }  // Get the current line number used by ReadLine() and GetCurLine()
    int  GetMaxLine() { return m_cLines - 1; }  // Largest line number you can read
    int  GetCount() const { return m_cLines; }  // Total number of lines
    bool InRange(int pos) const { return (pos < m_cLines && m_cLines > 0); }

    void Sort(int firstLine, int lastLine, int column = 0);  // sort lines into alphabetical order

    bool ReadFile(const char* pszFile);
    bool WriteFile(const char* pszFile = nullptr);  // nullptr will write to the file that was read

    void SetCurLine(int line)  // sets the line to be read by ReadLine() or GetCurLine()
    {
        ttASSERT(InRange(line));
        if (line < m_cLines)
            m_curLine = line;
    }

    // Call ReadLine() to get the current line and increment the line number.

    char* ReadLine()
    {
        if (m_curLine >= m_cLines)
            return nullptr;
        return m_aptrs[m_curLine++].pszLine;
    }

    operator char*() const
    {
        if (m_curLine >= m_cLines)
            return nullptr;
        return m_aptrs[m_curLine].pszLine;
    }

    char* operator[](int line) { return (line >= 0 && line < m_cLines) ? m_aptrs[line].pszLine : nullptr; }

protected:
    // Protected functions

    void Initialize();

    inline void swap(int pos1, int pos2)
    {
        LINE_PTRS pszTmp;
        memcpy(&pszTmp, m_aptrs + pos1, sizeof(LINE_PTRS));
        memcpy(m_aptrs + pos1, m_aptrs + pos2, sizeof(LINE_PTRS));
        memcpy(m_aptrs + pos2, &pszTmp, sizeof(LINE_PTRS));
    }
    void qsortCol(int low, int high);

private:
    typedef struct
    {
        char* pszLine;
        bool  bAllocated;  // true means string was allocated, false it's part of m_pbuf
    } LINE_PTRS;

    // Class members

    ttCStr  m_cszReadFile;
    ttCFile m_file;

    int m_cLines;
    int m_curLine;
    int m_SortColumn;

    int m_cAllocPtrs;

    LINE_PTRS* m_aptrs;
};
