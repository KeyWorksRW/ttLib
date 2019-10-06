/////////////////////////////////////////////////////////////////////////////
// Name:      ttCLineFile
// Purpose:   Line-oriented file class
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../include/ttfile.h"  // ttCFile
#include "../include/ttstr.h"   // ttCStr

// Line-oriented file class
//
// Note that lines are zero-based. The first line is accessed as 0.
//
// It's fine to call AddLine() before calling ReadFile(), however you can only call ReadFile() once.
class ttCLineFile : public ttCHeap
{
public:
    ttCLineFile();
    ttCLineFile(HANDLE hHeap);

    // Public functions

    // Adds to the end of the file
    void AddLine(const char* pszLine);
    void DeleteLine(int line);
    // Insert before line
    void InsertLine(int line, const char* pszLine);
    void ReplaceLine(int line, const char* pszLine);

    // Get the current line number used by ReadLine() and GetCurLine()
    int GetLineNumber() { return m_curLine; }
    // Largest line number you can read
    int GetMaxLine() { return m_cLines - 1; }
    // Total number of lines
    int  GetCount() const { return m_cLines; }
    bool InRange(int pos) const { return (pos < m_cLines && m_cLines > 0); }

    // sort lines into alphabetical order
    void Sort(int firstLine, int lastLine, int column = 0);

    bool ReadFile(const char* pszFile);
    // nullptr will write to the file that was read
    bool WriteFile(const char* pszFile = nullptr);

    // sets the line to be read by ReadLine() or GetCurLine()
    void SetCurLine(int line)
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
        // true means string was allocated, false it's part of m_pbuf
        bool bAllocated;
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
