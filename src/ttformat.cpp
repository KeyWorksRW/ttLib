/////////////////////////////////////////////////////////////////////////////
// Name:      ttprintf.cpp
// Purpose:
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <cstdarg>
#include <sstream>
#include <iomanip>
#include <ios>

#include "../include/ttstring.h"
#include "../include/utf8unchecked.h"

enum WIDTH : size_t
{
    WIDTH_NONE,
    WIDTH_CHAR,
    WIDTH_SHORT,
    WIDTH_LONG,
    WIDTH_LONG_LONG,
    WIDTH_INT_MAX,
    WIDTH_SIZE_T,
    WIDTH_PTRDIFF,
    WIDTH_LONG_DOUBLE
};

ttString& cdecl ttString::Format(std::string_view format, ...)
{
    std::va_list args;
    va_start(args, format);

    std::stringstream buffer;

    // Place this outside the try block so that we can refer to it in the catch block
    size_t pos = 0;

    try
    {
        for (pos = 0; pos < format.length(); ++pos)
        {
            if (format[pos] != '%')
            {
                buffer << format[pos];
                continue;
            }
            ++pos;

            // From here on out we use at() rather than [] so that an exception is thrown if pos exceeds range.

            if (format.at(pos) == '%')
            {
                buffer << format[++pos];
                continue;
            }

            bool kflag = false;
            if (format.at(pos) == 'k')
            {
                kflag = true;
                ++pos;
            }

            WIDTH width = WIDTH_NONE;

            if (format.at(pos) == 'h')
            {
                ++pos;
                if (format.at(pos) == 'h')
                {
                    ++pos;
                    width = WIDTH_SHORT;
                }
                else
                {
                    width = WIDTH_CHAR;
                }
            }

            else if (format.at(pos) == 'l')
            {
                ++pos;
                if (format.at(pos) == 'l')
                {
                    ++pos;
                    width = WIDTH_LONG_LONG;
                }
                else
                {
                    width = WIDTH_LONG;
                }
            }

            else if (format.at(pos) == 'j')
            {
                ++pos;
                width = WIDTH_INT_MAX;
            }
            else if (format.at(pos) == 'z')
            {
                ++pos;
                width = WIDTH_SIZE_T;
            }
            else if (format.at(pos) == 't')
            {
                ++pos;
                width = WIDTH_PTRDIFF;
            }
            else if (format.at(pos) == 'L')
            {
                ++pos;
                width = WIDTH_LONG_DOUBLE;
            }

            bool LeftFieldWidth = false;
            if (format[pos] == '-')
            {
                buffer << std::left;
                LeftFieldWidth = true;
                ++pos;
            }

            if (tt::isdigit(format[pos]))
            {
                auto fieldWidth = tt::atoi(format.substr(pos));
                buffer << std::setw(fieldWidth);
                do
                {
                    ++pos;
                } while (pos < format.length() && tt::isdigit(format[pos]));
            }

            // For both %lc and %ls we assume a UTF16 string and convert it to UTF8.

            if (format.at(pos) == 'c')
            {
                if (width != WIDTH_LONG)
                    buffer << va_arg(args, char);
                else
                {
                    std::wstring str16;
                    str16 += va_arg(args, wchar_t);
                    std::string str8;
                    utf8::unchecked::utf16to8(str16.begin(), str16.end(), back_inserter(str8));
                    buffer << str8;
                }
            }
            else if (format.at(pos) == 's')
            {
                if (width != WIDTH_LONG)
                {
                    if (kflag)
                        buffer << std::quoted(va_arg(args, const char*));
                    else
                        buffer << va_arg(args, const char*);
                }
                else
                {
                    std::wstring str16;
                    str16 += va_arg(args, const wchar_t*);
                    std::string str8;
                    utf8::unchecked::utf16to8(str16.begin(), str16.end(), back_inserter(str8));
                    if (kflag)
                        buffer << std::quoted(str8);
                    else
                        buffer << str8;
                }
            }
            else if (format.at(pos) == 'v')
            {
                if (width != WIDTH_LONG)
                {
                    if (kflag)
                        buffer << std::quoted(va_arg(args, std::string_view));
                    else
                        buffer << va_arg(args, std::string_view);
                }
                else
                {
                    std::wstring str16;
                    str16 += va_arg(args, std::wstring_view);
                    std::string str8;
                    utf8::unchecked::utf16to8(str16.begin(), str16.end(), back_inserter(str8));
                    if (kflag)
                        buffer << std::quoted(str8);
                    else
                        buffer << str8;
                }
            }
            else if (format.at(pos) == 'd' || format.at(pos) == 'i')
            {
                std::locale previous;
                if (kflag)
                    previous = buffer.imbue(std::locale(""));

                switch (width)
                {
                    default:
                    case WIDTH_NONE:
                        buffer << va_arg(args, int);
                        break;

                    case WIDTH_CHAR:
                        buffer << va_arg(args, signed char);
                        break;

                    case WIDTH_SHORT:
                        buffer << va_arg(args, short);
                        break;

                    case WIDTH_LONG:
                        buffer << va_arg(args, long);
                        break;

                    case WIDTH_LONG_LONG:
                        buffer << va_arg(args, long long);
                        break;

                    case WIDTH_INT_MAX:
                        buffer << va_arg(args, intmax_t);
                        break;

                    case WIDTH_SIZE_T:
                        // REVIEW: [KeyWorks - 02-14-2020] The spec says this should be signed. Knowing that a
                        // size_t is set to -1 is valuable, but any other value is likely to be unsigned. We break
                        // from the spec and special case a value of -1, otherwise we format the value as unsigned.
                        {
                            size_t val = va_arg(args, size_t);
                            if (val == static_cast<size_t>(-1))
                                buffer << "-1";
                            else
                                buffer << val;
                        }
                        break;

                    case WIDTH_PTRDIFF:
                        buffer << va_arg(args, ptrdiff_t);
                        break;
                }
                if (kflag)
                    previous = buffer.imbue(previous);
            }
            else if (format.at(pos) == 'u' || format.at(pos) == 'o' || format.at(pos) == 'x' ||
                     format.at(pos) == 'X')
            {
                std::locale previous;
                if (kflag)
                    previous = buffer.imbue(std::locale(""));

                if (format.at(pos) == 'o')
                    buffer << std::oct;
                else if (format.at(pos) == 'x')
                {
                    buffer << std::hex;
                }
                else if (format.at(pos) == 'X')
                {
                    buffer << std::uppercase;
                    buffer << std::hex;
                }

                switch (width)
                {
                    default:
                    case WIDTH_NONE:
                        buffer << va_arg(args, unsigned int);
                        break;

                    case WIDTH_CHAR:
                        buffer << va_arg(args, unsigned char);
                        break;

                    case WIDTH_SHORT:
                        buffer << va_arg(args, unsigned short);
                        break;

                    case WIDTH_LONG:
                        buffer << va_arg(args, unsigned long);
                        break;

                    case WIDTH_LONG_LONG:
                        buffer << va_arg(args, unsigned long long);
                        break;

                    case WIDTH_INT_MAX:
                        buffer << va_arg(args, uintmax_t);
                        break;

                    case WIDTH_SIZE_T:
                        // REVIEW: [KeyWorks - 02-14-2020] The spec says this should be signed. Knowing that a
                        // size_t is set to -1 is valuable, but any other value is likely to be unsigned. We
                        // break from the spec and special case a value of -1, otherwise we format the value as
                        // unsigned.
                        {
                            size_t val = va_arg(args, size_t);
                            if (val == static_cast<size_t>(-1))
                                buffer << "-1";
                            else
                                buffer << val;
                        }
                        break;

                    case WIDTH_PTRDIFF:
                        // REVIEW: [KeyWorks - 02-14-2020] There is no unsigned ptrdiff_t, so we use size_t
                        buffer << va_arg(args, size_t);
                        break;
                }

                if (kflag)
                    previous = buffer.imbue(previous);
                buffer << std::dec;
                buffer << std::nouppercase;
            }

            if (LeftFieldWidth)
                buffer << std::right;
        }
    }
    catch (const std::exception& /* e */)
    {
        assert(!"exception in ttString.Format()");
    }

    va_end(args);

    this->assign(buffer.str());

    return *this;
}
