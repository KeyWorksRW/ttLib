/////////////////////////////////////////////////////////////////////////////
// Name:      ttlib.h
// Purpose:   Master header file for ttLib
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if defined(_TTLIBWIN_H_GUARD_)
    #error Do not #include ttLib.h if you have already #included ttlibWin.h
#endif

// clang-format off
#ifndef _TTLIB_H_GUARD_
#define _TTLIB_H_GUARD_

#include <stdint.h>  // needed for standard types
#include <cstdlib>   // needed for std::abs

#include <string_view>
#include <filesystem>

#if defined(_WIN32)
    #include <wtypes.h>
#endif

// clang-format on

namespace tt
{
    static constexpr size_t npos = size_t(-1);

    // strings limited to 16,777,215 bytes (16 megabytes)
    static constexpr size_t MAX_STRING_LEN = 0x00FFFFFF;

    // title for message boxes
    extern const char* pszMsgTitle;
    // title for message boxes
    extern const wchar_t* pwszMsgTitle;

    /// Equivalent to Windows RECT structure -- this makes it available on non-Windows
    /// platforms.
    struct RECT
    {
        int32_t left;
        int32_t top;
        int32_t right;
        int32_t bottom;

        inline int32_t GetWidth() { return std::abs(right - left); }
        inline int32_t GetHeight() { return std::abs(bottom - top); }
    };

    // clang-format off
#if defined(_WIN32)
    // Handle to use to load resources
    extern HINSTANCE hinstResources;
#endif  // _WINDOWS_
    // clang-format on

    /// Only valid for ANSI or UTF8 characters
    inline bool isdigit(char ch) { return ((ch >= '0' && ch <= '9') || ch == '-'); }

    /// Is ch the start of a utf8 sequence?
    inline bool isutf8(char ch) { return ((ch & 0xC0) != 0x80); }

    /// Returns true if character is a space, tab, eol or form feed character.
    inline bool iswhitespace(char ch) { return (ch != 0 && std::strchr(" \t\r\n\f", ch)) ? true : false; };

    /// Returns true if strings are identical
    bool issameas(std::string_view str1, std::string_view str2, bool CaseSensitive = true);

    /// Same as compare only it returns a boolean instead of the difference.
    inline bool issamestr(std::string_view str1, std::string_view str2) { return (str1.compare(str2) == 0); }

    /// Same as issamestr only case insensitive comparison of ASCII characters
    bool issamestri(std::string_view str1, std::string_view str2);

    /// Returns true if the sub-string is identical to the first part of the main string
    bool issamesubstr(std::string_view strMain, std::string_view strSub);

    /// Case-insensitive sub string comparison
    bool issamesubstri(std::string_view strMain, std::string_view strSub);

    /// Return a view to the portion of the string beginning with the sub string.
    ///
    /// Return view is empty if substring is not found.
    std::string_view findstr(std::string_view main, std::string_view sub, bool CaseSensitive = true);

    /// Returns the position of sub within main, or tt::npos if not found.
    size_t findstr_pos(std::string_view main, std::string_view sub, bool CaseSensitive = true);

    /// Returns true if the sub string exists withing the main string.
    ///
    /// Same as findstr but with a boolean return instead of a string_view.
    bool contains(std::string_view main, std::string_view sub, bool CaseSensitive = true);

    /// Return a view to the portion of the string beginning with the sub string.
    ///
    /// Return view is empty if substring is not found.
    std::string_view strstr(std::string_view strMain, std::string_view strSub);

    /// Case-insensitive search for a sub string.
    ///
    /// Return view is empty if substring is not found.
    std::string_view strstri(std::string_view strMain, std::string_view strSub);

    /// Returns a pointer to the next character in a UTF8 string.
    const char* nextchar(const char* psz);

    /// Returns view to the next whitespace character. View is empty if there are no more
    /// whitespaces.
    std::string_view findspace(std::string_view str);

    /// Returns position of next whitespace character or tt::npos if not found.
    size_t findspace_pos(std::string_view str);

    /// Returns view to the next non-whitespace character. View is empty if there are no
    /// non-whitespace characters.
    std::string_view findnonspace(std::string_view str);

    /// Returns position of next non-whitespace character or tt::npos if not found.
    size_t findnonspace_pos(std::string_view str);

    /// Equivalent to findnonspace(findspace(str)).
    std::string_view stepover(std::string_view str);

    /// Equivalent to findnonspace(findspace(str)) returning the position or tt::npos.
    size_t stepover_pos(std::string_view str);

    /// Generates hash of string using djb2 hash algorithm
    size_t gethash(std::string_view str);

    /// Converts a string into an integer.
    ///
    /// If string begins with '0x' it is assumed to be hexadecimal and is converted.
    /// String may begin with a '-' or '+' to indicate the sign of the integer.
    int atoi(std::string_view str);

    /// Return a view to a filename's extension. View is empty if there is no extension.
    std::string_view findext(std::string_view str);

    /// Determines whether the character at pos is part of a filename. This will
    /// differentiate between '.' being used as part of a path (. for current directory, or ..
    /// for relative directory) versus being the leading character in a file.
    bool isvalidfilechar(std::string_view str, size_t pos);

    /// Converts all backslashes in a filename to forward slashes.
    ///
    /// Note: Windows handles paths that use forward slashes, so backslashes are normally
    /// unnecessary.
    void backslashestoforward(std::string& str);

    /// Performs a case-insensitive check to see if a directory entry is a filename and
    /// contains the specified extension.
    bool hasextension(std::filesystem::directory_entry name, std::string_view extension);

    /// Confirms newdir exists and is a directory and then changes to that directory.
    ///
    /// Returns false only if newdir is not an existing directory. Throws filesystem_error
    /// if the directory is valid but could not be changed to.
    bool ChangeDir(std::string_view newdir);

    bool dirExists(std::string_view dir);
    bool fileExists(std::string_view filename);

}  // namespace tt

// clang-format off

namespace ttch {
    const char CH_OPEN_PAREN =    '(';
    const char CH_CLOSE_PAREN =   ')';
    const char CH_COLON =         ':';
    const char CH_SEMICOLON =     ';';
    const char CH_START_QUOTE =   '`';
    const char CH_SQUOTE =        '\'';
    const char CH_END_QUOTE =     '\'';
    const char CH_QUOTE =         '"';
    const char CH_EQUAL =         '=';
    const char CH_SPACE =         ' ';
    const char CH_COMMA =         ',';
    const char CH_LEFT_BRACKET =  '[';
    const char CH_RIGHT_BRACKET = ']';
    const char CH_TAB =           '\t';
    const char CH_BACKSLASH =     '\\';
    const char CH_FORWARDSLASH =  '/';

    const char CHW_OPEN_PAREN =    L'(';
    const char CHW_CLOSE_PAREN =   L')';
    const char CHW_COLON =         L':';
    const char CHW_SEMICOLON =     L';';
    const char CHW_START_QUOTE =   L'`';
    const char CHW_SQUOTE =        L'\'';
    const char CHW_END_QUOTE =     L'\'';
    const char CHW_QUOTE =         L'"';
    const char CHW_EQUAL =         L'=';
    const char CHW_SPACE =         L' ';
    const char CHW_COMMA =         L',';
    const char CHW_LEFT_BRACKET =  L'[';
    const char CHW_RIGHT_BRACKET = L']';
    const char CHW_TAB =           L'\t';
    const char CHW_BACKSLASH =     L'\\';
    const char CHW_FORWARDSLASH =  L'/';
} // end of ttch namespace

#if !defined(_XGET)
    // This macro can be placed around static text that you want xgettext.exe to extract for
    // translation using the "xgettext.exe -k_XGET" keyword option.
    #define _XGET(txt) txt
#endif

#if !defined(TRANSLATE)
    #if defined(_WX_DEFS_H_)
        // If using wxWidgets, then translate the string into a UTF8 string. Currently there
        // is no mechanism for translating without wxWidgets, but the macro makes it possible
        // to add that functionality later.
        #define TRANSLATE(str) wxGetTranslation(str).utf8_str()
    #else
        #define TRANSLATE(str) str
    #endif
#endif

// clang-format on

class ttCStr;  // forward definition

extern bool(_cdecl* pttAssertHandlerA)(const char* pszMsg, const char* pszFile, const char* pszFunction, int line);
extern bool(_cdecl* pttAssertHandlerW)(const wchar_t* pszMsg, const char* pszFile, const char* pszFunction,
                                       int line);

typedef bool(_cdecl* TTASSERTHANDLERA)(const char* pszMsg, const char* pszFile, const char* pszFunction, int line);
typedef bool(_cdecl* TTASSERTHANDLERW)(const wchar_t* pszMsg, const char* pszFile, const char* pszFunction,
                                       int line);

int    ttStrCat(char* pszDst, const char* pszSrc);
int    ttStrCat(char* pszDst, size_t cbDest, const char* pszSrc);
char*  ttStrChr(const char* psz, char ch);
char*  ttStrChrR(const char* psz, char ch);
int    ttStrCpy(char* pszDst, const char* pszSrc);
int    ttStrCpy(char* pszDst, size_t cbDest, const char* pszSrc);
size_t ttStrLen(const char* psz);
char*  ttStrStr(const char* pszMain, const char* pszSub);
char*  ttStrStrI(const char* pszMain, const char* pszSub);

// Use ttStrByteLen() to get the number of bytes including the terminating zero.
//
// Use ttStrLen() to get the number of characters NOT includingt the trailing zero.
inline size_t ttStrByteLen(const char* psz)
{
    return ttStrLen(psz) * sizeof(char) + sizeof(char);
}

void*  ttCalloc(size_t cb);
void*  ttCalloc(size_t num, size_t cb);
void   ttFree(void* pv);
void*  ttMalloc(size_t cb);
void*  ttReAlloc(void* pv, size_t cbNew);
void*  ttReCalloc(void* pv, size_t cbNew);
char*  ttStrDup(const char* psz);
char*  ttStrDup(const char* psz, char** ppszDst);
size_t ttSize(const void* pv);
bool   ttValidate(const void* pv);

bool ttIsSamePath(const char* pszFile1, const char* pszFile2);
// Same as strcmp, but returns true/false.
bool ttIsSameStr(const char* psz1, const char* psz2);
// Case-insensitive comparison
bool ttIsSameStrI(const char* psz1, const char* psz2);
// Returns true if sub string matches first part of main string.
bool ttIsSameSubStr(const char* pszMain, const char* pszSub);
// Case-insensitive comparison
bool ttIsSameSubStrI(const char* pszMain, const char* pszSub);
// Returns true if the character at the specified position is a valid filename character.
bool ttIsValidFileChar(const char* psz, size_t pos);

void ttAddTrailingSlash(char* pszPath);

// Converts all backslashes in the string to forward slashes
void ttBackslashToForwardslash(char* pszPath);

// Converts pszFile into a path relative to pszRoot, stores result in cszResult.
void ttConvertToRelative(const char* pszRoot, const char* pszFile, ttCStr& cszResult);
bool ttCreateDir(const char* pszDir);

// Returns pointer to the next non-whitespace character.
char* ttFindNonSpace(const char* psz);

// Returns pointer to the next whitespace character or to the end of the string if there is
// no whitespace.
char* ttFindSpace(const char* psz);

void ttForwardslashToBackslash(char* pszPath);

// Creates a hash number from an string
size_t ttHashFromSz(const char* psz);

// Creates a hash number from a url or filename ('/' and '\' are considered the same, letter case doesn't matter).
size_t ttHashFromURL(const char* pszURL);

// Handles UTF8 strings
char* ttNextChar(const char* psz);

// Equivalent to FindNonSpace(FindSpace(psz)).
char* ttStepOver(const char* psz);

void ttTrimRight(char* psz);

inline bool ttIsAlpha(char ch)
{
    return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}
inline bool ttIsDigit(char ch)
{
    return ((ch >= '0' && ch <= '9') || ch == '-');
}
inline bool ttIsEmpty(const char* psz)
{
    return (bool) ((psz == nullptr) || (!psz[0]));
}
inline bool ttIsNonEmpty(const char* psz)
{
    return (psz != nullptr && psz[0]);
}
inline bool ttIsPunct(char ch)
{
    return (ch == '.' || ch == ',' || ch == ';' || ch == ':' || ch == '?' || ch == '!');
}
// Is ch the start of a utf8 sequence?
inline bool ttIsUTF8(char ch)
{
    return ((ch & 0xC0) != 0x80);
}
inline bool ttIsWhitespace(char ch)
{
    return (ch != 0 && strchr(" \t\r\n\f", ch)) ? true : false;
};

    // clang-format off
#if defined(_WIN32)
    inline bool ttChDir(const char* pszDir) { return (SetCurrentDirectoryA(pszDir) != FALSE); }
#else
    // #include <wx/filefn.h>
    inline bool ttChDir(const wchar_t* pwszDir) { return (wxSetWorkingDirectory(pwszDir); }
#endif
// clang-format on

ptrdiff_t ttAtoi(const char* psz);
char*     ttHextoa(size_t val, char* pszDst, bool bUpperCase);
char*     ttItoa(int32_t val, char* pszDst, size_t cbDst);
char*     ttItoa(int64_t val, char* pszDst, size_t cbDst);
char*     ttUtoa(uint32_t val, char* pszDst, size_t cbDst);
char*     ttUtoa(uint64_t val, char* pszDst, size_t cbDst);

// The Exists() functions return true/false base on whether the file or directory actually exists
bool ttDirExists(const char* pszFolder);
bool ttFileExists(const char* pszFile);

// ttFindExtPortion and ttFindFilePortion properly handle directories and filenames that start with '.' -- i.e.,
// ttFindExtPortion(".gitignore") will return nullptr

// Find a case-insensitive extension in a path string.
char* ttFindExt(const char* pszPath, const char* pszExt);

// Returns pointer to the '.' that begins a file name extension, or nullptr.
char* ttFindExtPortion(const char* pszPath);
// Returns pointer to the filename portion of a path.
char* ttFindFilePortion(const char* pszPath);
// Handles both forward and back slashes.
char* ttFindLastSlash(const char* pszPath);

// Replace the ttLib assertion handler with your own.
inline void ttSetAssertHandlerA(TTASSERTHANDLERA pFunc)
{
    pttAssertHandlerA = pFunc;
}
inline void ttSetAssertHandlerW(TTASSERTHANDLERW pFunc)
{
    pttAssertHandlerW = pFunc;
}

void ttInitCaller(const char* pszTitle);

void ttSetMsgBoxTitle(const char* pszMsgTitle);

// ttPrintf/ttVPrintf provides a sub-set of the standard sprintf format codes, with automatic allocation of
// sufficient memory to hold the result, along with some special format specifiers.

// standard: c, C, d, i, u, x, X, s, S     (no floating point, precision or padding)
//
// %kd -  formats an integer with commas. I.e., 54321 would be formatted as 54,321
// %kq -  outputs quotation marks around the string
//
// %ks -  adds a 's' to the current buffer if the integer is zero or greater then 1, e.g.,
//
//     printf("item%ks", cItems);
//
//
// %kS -  adds a 's' to the current buffer if the __int64 is zero or greater then 1
//
// %kls - adds a 's' to the current buffer if the last numeric argument is zero or greater
// then 1
//
//     printf("%d item%kls", cItems);
//
// %kt - formats a size_t value with commas
//
// %ku -  formats an unsigned integer with commas
//
// %kI64d -- handles int64_t, adding commas if needed
// %kI64u -- handles uint64_t, adding commas if needed
//
// The following are only valid when compiling for _WIN32
//
// %ke - formats a system message assuming the argument is an error number
// %kr - argument is a resource identifier to a string
char* cdecl ttPrintf(char** ppszDst, const char* pszFormat,
                     ...);  // CAUTION! The memory ppszDst points to will be modified by ttHeap functions
void        ttVPrintf(char** ppszDst, const char* pszFormat, va_list argList);

/////////////////////////// wide character versions /////////////////////////////////////

int      ttStrCat(wchar_t* pszDst, const wchar_t* pszSrc);
int      ttStrCat(wchar_t* pszDst, size_t cbDest, const wchar_t* pszSrc);
int      ttStrCpy(wchar_t* pszDst, const wchar_t* pszSrc);
int      ttStrCpy(wchar_t* pszDst, size_t cbDest, const wchar_t* pszSrc);
size_t   ttStrLen(const wchar_t* pwsz);
wchar_t* ttStrChr(const wchar_t* psz, wchar_t ch);
wchar_t* ttStrChrR(const wchar_t* psz, wchar_t ch);
wchar_t* ttStrStr(const wchar_t* pszMain, const wchar_t* pszSub);

wchar_t* ttStrChr(const wchar_t* psz, wchar_t ch);
wchar_t* ttStrChrR(const wchar_t* psz, wchar_t ch);  // Returns nullptr if not found
wchar_t* ttStrStr(const wchar_t* pszMain, const wchar_t* pszSub);
wchar_t* ttStrStrI(const wchar_t* pszMain, const wchar_t* pszSub);
wchar_t* ttFindNonSpace(const wchar_t* psz);  // returns pointer to the next non-space character
wchar_t* ttFindSpace(const wchar_t* psz);     // returns pointer to the next space character

wchar_t* ttStrDup(const wchar_t* pwsz);
wchar_t* ttStrDup(const wchar_t* psz, wchar_t** ppszDst);

bool ttIsSamePath(const wchar_t* pszFile1, const wchar_t* pszFile2);
bool ttIsSameStr(const wchar_t* psz1, const wchar_t* psz2);   // same as strcmp, but returns true/false
bool ttIsSameStrI(const wchar_t* psz1, const wchar_t* psz2);  // case-insensitive comparison
bool ttIsSameSubStr(const wchar_t* pszMain,
                    const wchar_t* pszSub);  // true if sub string matches first part of main string
bool ttIsSameSubStrI(const wchar_t* pszMain, const wchar_t* pszSub);  // case-insensitive comparison

ptrdiff_t ttAtoi(const wchar_t* psz);
wchar_t*  ttHextoa(size_t val, wchar_t* pszDst, bool bUpperCase);
wchar_t*  ttItoa(int32_t val, wchar_t* pszDst, size_t cbDst);
wchar_t*  ttItoa(int64_t val, wchar_t* pszDst, size_t cbDst);
wchar_t*  ttUtoa(uint32_t val, wchar_t* pszDst, size_t cbDst);
wchar_t*  ttUtoa(uint64_t val, wchar_t* pszDst, size_t cbDst);

inline bool ttIsAlpha(wchar_t ch)
{
    return ((ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z'));
}
inline bool ttIsDigit(wchar_t ch)
{
    return ((ch >= L'0' && ch <= L'9') || ch == L'-');
}
inline bool ttIsEmpty(const wchar_t* psz)
{
    return (bool) ((psz == nullptr) || (!psz[0]));
}
inline bool ttIsNonEmpty(const wchar_t* psz)
{
    return (psz != nullptr && psz[0]);
}
inline bool ttIsPunct(wchar_t ch)
{
    return (ch == L'.' || ch == L',' || ch == L';' || ch == L':' || ch == L'?' || ch == L'!');
}
inline bool ttIsWhitespace(wchar_t ch)
{
    return (ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n' || ch == L'\f') ? true : false;
}
inline size_t ttStrByteLen(const wchar_t* pwsz)
{
    return ttStrLen(pwsz) * sizeof(wchar_t) + sizeof(wchar_t);
}

    // clang-format off
#if defined(_WIN32)
    inline  bool   ttChDir(const wchar_t* pwszDir) { return (SetCurrentDirectoryW(pwszDir) != FALSE); }
#else
    // #include <wx/filefn.h>
    inline  bool   ttChDir(const wchar_t* pwszDir) { return (wxSetWorkingDirectory(pwszDir); }
#endif
// clang-format on

bool     ttCreateDir(const wchar_t* pszDir);
bool     ttDirExists(const wchar_t* pszFolder);
bool     ttFileExists(const wchar_t* pszFile);
wchar_t* ttFindExt(const wchar_t* pszPath, const wchar_t* pszExt);
size_t   ttHashFromSz(const wchar_t* psz);
size_t   ttHashFromURL(const wchar_t* pszURL);
wchar_t* ttStepOver(const wchar_t* pwsz);
void     ttTrimRight(wchar_t* psz);

    #if defined(_WIN32)
// ttInitCaller is equivalent to calling setResInst(hinstRes), setMsgBoxParent(hwndParent)
// and setMsgBoxTitle(pszMsgTitle)
[[deprecated]] void ttInitCaller(HINSTANCE hinstRes, HWND hwndParent, const char* pszMsgTitle);

inline HINSTANCE ttGetResInst()
{
    return tt::hinstResources;
}
inline void ttSetResInst(HINSTANCE hinst)
{
    tt::hinstResources = hinst;
}

const char* ttGetResString(size_t idString);
const char* ttLoadTxtResource(int idRes, uint32_t* pcbFile = nullptr, HINSTANCE hinst = tt::hinstResources);

int       ttMsgBox(UINT idResource, UINT uType = MB_OK | MB_ICONWARNING);
int       ttMsgBox(const char* pszMsg, UINT uType = MB_OK | MB_ICONWARNING);
int cdecl ttMsgBoxFmt(const char* pszFormat, UINT uType, ...);
int cdecl ttMsgBoxFmt(int idResource, UINT uType, ...);

ptrdiff_t ttCompareFileTime(FILETIME* pftSrc, FILETIME* pftDst);
HFONT     ttCreateLogFont(const char* pszTypeFace, size_t cPt, bool fBold = false, bool fItalics = false);

inline int ttRC_HEIGHT(const RECT* prc)
{
    return prc->bottom - prc->top;
};
inline int ttRC_HEIGHT(const RECT rc)
{
    return rc.bottom - rc.top;
};
inline int ttRC_WIDTH(const RECT* prc)
{
    return prc->right - prc->left;
};
inline int ttRC_WIDTH(const RECT rc)
{
    return rc.right - rc.left;
};

inline bool ttIsPosInRect(const RECT* prc, int xPos, int yPos)
{
    return (xPos >= prc->left && xPos <= prc->right && yPos >= prc->top && yPos <= prc->bottom);
}
inline bool ttIsValidWindow(HWND hwnd)
{
    return (bool) (hwnd && IsWindow(hwnd));
};
    #endif  // defined(_WIN32)

    // clang-format off
#if defined(_WX_DEFS_H_)
    inline void  ttInitCaller(const wxString& str) { ttInitCaller((const char*) str.utf8_str()); }
    inline void  ttSetMsgBoxTitle(const wxString& str) { ttSetMsgBoxTitle((const char*) str.utf8_str()); }
    inline char* ttStrDup(const wxString& str) { return ttStrDup((const char*) str.utf8_str()); }
    inline int   ttStrCat(char* pszDst, const wxString& str) { return ttStrCat(pszDst, (const char*) str.utf8_str()); };

#if defined(_WIN32)
    inline int ttMsgBox(const wxString& str, UINT uType = MB_OK | MB_ICONWARNING)
    {
        return ttMsgBox((const char*) str.utf8_str(), uType);
    };
#endif  // defined (_WIN32)

#endif  // defined(_WX_DEFS_H_)
    // clang-format on

#endif  // _TTLIB_H_GUARD_
