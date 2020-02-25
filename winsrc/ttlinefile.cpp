/////////////////////////////////////////////////////////////////////////////
// Name:      ttCLineFile
// Purpose:   Line-oriented file class
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#if !defined(TTALL_LIB)
    #if defined(NDEBUG)
        #pragma comment(lib, "ttLibwin.lib")
    #else
        #pragma comment(lib, "ttLibwinD.lib")
    #endif
#endif

#include "../include/ttlibwin.h"    // Master header file for ttLibwin.lib
#include "../include/ttfile.h"      // ttCFile
#include "../include/ttlinefile.h"  // ttCLineFile

ttCLineFile::ttCLineFile()
    : ttCHeap(true)
{
    Initialize();
}

ttCLineFile::ttCLineFile(HANDLE hHeap)
    : ttCHeap(hHeap)
{
    Initialize();
}
void ttCLineFile::Initialize()
{
    m_curLine = 0;
    m_cLines = 0;

    m_cAllocPtrs = 256;
    m_aptrs = (LINE_PTRS*) this->ttCalloc(m_cAllocPtrs * sizeof(LINE_PTRS));
}

void ttCLineFile::AddLine(const char* pszLine)
{
    if (m_cLines + 1 > m_cAllocPtrs)
    {
        m_cAllocPtrs += 256;
        m_aptrs = (LINE_PTRS*) this->ttReCalloc(m_aptrs, m_cAllocPtrs * sizeof(LINE_PTRS));
    }

    m_aptrs[m_cLines].pszLine = this->ttStrDup(pszLine);
    m_aptrs[m_cLines++].bAllocated = true;
}

void ttCLineFile::DeleteLine(int line)
{
    ttASSERT(line <= m_cLines);
    if (line > m_cLines)
        return;
    if (m_aptrs[line].bAllocated)
    {
        this->ttFree(m_aptrs[line].pszLine);
    }
    memmove((void*) (m_aptrs + line), (void*) (m_aptrs + line + 1), (m_cLines - (line + 1)) * sizeof(LINE_PTRS));
    --m_cLines;
    if (m_curLine > m_cLines)
        m_curLine = m_cLines;
}

bool ttCLineFile::ReadFile(const char* pszFile)
{
    if (m_file.GetBeginPosition())
    {
        // REVIEW: [KeyWorks - 09-03-2019] If we need to support this, then first we have to allocate memory for
        // every string that is currently stored that hasn't been allocated yet.

        ttMsgBox("You've already read a file into ttCLineFile");
        return false;
    }

    if (m_file.ReadFile(pszFile))
    {
        while (m_file.ReadLine())
        {
            if (m_cLines + 1 > m_cAllocPtrs)
            {
                m_cAllocPtrs += 256;
                m_aptrs = (LINE_PTRS*) this->ttReCalloc(m_aptrs, m_cAllocPtrs * sizeof(LINE_PTRS));
            }
            m_aptrs[m_cLines++].pszLine = (char*) m_file;
        }
        m_cszReadFile = pszFile;
        return true;
    }
    else
        return false;
}

bool ttCLineFile::WriteFile(const char* pszFile)
{
    if (!pszFile)
        pszFile = m_cszReadFile;

    ttCFile file;

    for (int line = 0; line < m_cLines; ++line)
        file.WriteEol(m_aptrs[line].pszLine);

    return file.WriteFile(pszFile);
}

void ttCLineFile::InsertLine(int line, const char* pszLine)
{
    ttASSERT(line <= m_cLines);
    if (line > m_cLines)
        throw;

    if (m_cLines + 1 >= m_cAllocPtrs)
    {
        m_cAllocPtrs += 256;
        m_aptrs = (LINE_PTRS*) this->ttReCalloc(m_aptrs, m_cAllocPtrs * sizeof(LINE_PTRS));
    }

    memmove((void*) (m_aptrs + line + 1), (void*) (m_aptrs + line), (m_cLines - line + 1) * sizeof(LINE_PTRS));
    m_aptrs[line].pszLine = this->ttStrDup(pszLine);
    m_aptrs[line].bAllocated = true;
    m_cLines++;
}

void ttCLineFile::ReplaceLine(int line, const char* pszLine)
{
    ttASSERT(line < m_cLines);
    if (line >= m_cLines)
        throw;

    if (m_aptrs[line].bAllocated)
    {
        this->ttFree(m_aptrs[line].pszLine);
        m_aptrs[line].pszLine = this->ttStrDup(pszLine);
    }
    else
    {
        // If the new line is the same or shorter then the old line, then copy over it, otherwise duplicate it

        auto cbOld = ttStrByteLen(m_aptrs[line].pszLine);
        auto cbNew = ttStrByteLen(pszLine);
        if (cbNew <= cbOld)
            ttStrCpy(m_aptrs[line].pszLine, cbOld, pszLine);
        else
        {
            m_aptrs[line].pszLine = this->ttStrDup(pszLine);
            m_aptrs[line].bAllocated = true;
        }
    }
}

void ttCLineFile::Sort(int firstLine, int lastLine, int column)
{
#if !defined(NDEBUG)  // Starts debug section.
    for (int itmp = firstLine; itmp <= lastLine; ++itmp)
    {
        if (!m_aptrs[itmp].pszLine)
        {
            ttFAIL_MSG("Attempt to sort a line that doesn't exist");
            return;
        }
        if ((int) ttStrLen(m_aptrs[itmp].pszLine) < column)
        {
            ttFAIL_MSG("Column number is beyond the end of the line");
            return;
        }
    }
#endif

    m_SortColumn = column;
    qsortCol(firstLine, lastLine);
}

// Original caller must have confirmed that m_SortColumn is within the range of every string being sorted.

void ttCLineFile::qsortCol(int low, int high)
{
    if (low >= high)
        return;

    // Do we need to sort?

    int pos = high - 1;
    while (pos >= low)
    {
        if (strcmp(m_aptrs[pos].pszLine + m_SortColumn, m_aptrs[pos + 1].pszLine + m_SortColumn) > 0)
            break;
        else
            pos--;
    }
    if (pos < low)
        return;  // it's already sorted

    swap(low, (low + high) / 2);

    int end = low;
    for (pos = low + 1; pos <= high; pos++)
    {
        if (strcmp(m_aptrs[pos].pszLine + m_SortColumn, m_aptrs[low].pszLine + m_SortColumn) < 0)
            swap(++end, pos);
    }
    swap(low, end);

    if (low < end - 1)
        qsortCol(low, end - 1);
    if (end + 1 < high)
        qsortCol(end + 1, high);
}
