/////////////////////////////////////////////////////////////////////////////
// Name:      ttmultistr.cpp
// Purpose:   Breaks a single string into multiple strings
// Author:    Ralph Walden
// Copyright: Copyright (c) 2018-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include "ttmultistr.h"

using namespace ttlib;

void multistr::SetString(std::string_view str, char separator, tt::TRIM trim)
{
    clear();
    size_t start = 0;
    size_t end = str.find_first_of(separator);

    // The last string will not have a separator, so end == tt::npos, but we still need to add that final string
    for (;;)
    {
        emplace_back();
        if (trim == tt::TRIM::both || trim == tt::TRIM::left)
        {
            auto begin = str.find_first_not_of(" \t\n\r\f\v", start);
            if (begin != tt::npos)
            {
                start = begin;
            }
            else
            {
                if (end != tt::npos)
                {
                    start = end;
                }
                else
                {
                    // We're at the end, there's nothing here but whitespace, so we're done
                    break;
                }
            }
        }

        if (trim == tt::TRIM::both || trim == tt::TRIM::right)
        {
            auto temp_end = end;
            if (end == tt::npos)
                temp_end = str.length();
            while (temp_end > start && ttlib::is_whitespace(str.at(temp_end - 1)))
            {
                --temp_end;
            }
            back().assign(str.substr(start, temp_end - start));
        }

        else if (end == tt::npos)
        {
            back().assign(str.substr(start, str.length() - start));
        }
        else
        {
            back().assign(str.substr(start, end - start));
        }

        // The last string will not have a separator after it so end will already be set to tt::npos
        if (end == tt::npos)
            break;

        start = end + sizeof(char);
        if (start >= str.length())
            break;

        end = str.find_first_of(separator, start);
    }
}

void multistr::SetString(std::string_view str, std::string_view separator, tt::TRIM trim)
{
    clear();
    size_t start = 0;
    size_t end = str.find_first_of(separator);

    // The last string will not have a separator, so end == tt::npos, but we still need to add that final string
    for (;;)
    {
        emplace_back();
        if (trim == tt::TRIM::both || trim == tt::TRIM::left)
        {
            auto begin = str.find_first_not_of(" \t\n\r\f\v", start);
            if (begin != tt::npos)
            {
                start = begin;
            }
            else
            {
                if (end != tt::npos)
                {
                    start = end;
                }
                else
                {
                    // We're at the end, there's nothing here but whitespace, so we're done
                    break;
                }
            }
        }

        if (trim == tt::TRIM::both || trim == tt::TRIM::right)
        {
            auto temp_end = end;
            if (end == tt::npos)
                temp_end = str.length();
            while (temp_end > start && ttlib::is_whitespace(str.at(temp_end - 1)))
            {
                --temp_end;
            }
            back().assign(str.substr(start, temp_end - start));
        }

        else if (end == tt::npos)
        {
            back().assign(str.substr(start, str.length() - start));
        }
        else
        {
            back().assign(str.substr(start, end - start));
        }

        // The last string will not have a separator after it so end will already be set to tt::npos
        if (end == tt::npos)
            break;

        start = end + separator.length();
        if (start >= str.length())
            break;

        end = str.find_first_of(separator, start);
    }
}

/////////////////////////////////////// multiview ///////////////////////////////////////

void multiview::SetString(std::string_view str, char separator, tt::TRIM trim)
{
    clear();
    size_t start = 0;
    size_t end = str.find_first_of(separator);

    // The last string will not have a separator, so end == tt::npos, but we still need to add that final string
    for (;;)
    {
        emplace_back(ttlib::emptystring);
        if (trim == tt::TRIM::both || trim == tt::TRIM::left)
        {
            auto begin = str.find_first_not_of(" \t\n\r\f\v", start);
            if (begin != tt::npos)
            {
                start = begin;
            }
            else
            {
                if (end != tt::npos)
                {
                    start = end;
                }
                else
                {
                    // We're at the end, there's nothing here but whitespace, so we're done
                    break;
                }
            }
        }

        if (trim == tt::TRIM::both || trim == tt::TRIM::right)
        {
            auto temp_end = end;
            if (end == tt::npos)
                temp_end = str.length();
            while (temp_end > start && ttlib::is_whitespace(str.at(temp_end - 1)))
            {
                --temp_end;
            }
            back() = str.substr(start, temp_end - start);
        }

        else if (end == tt::npos)
        {
            back() = str.substr(start, str.length() - start);
        }
        else
        {
            back() = str.substr(start, end - start);
        }

        // The last string will not have a separator after it so end will already be set to tt::npos
        if (end == tt::npos)
            break;

        start = end + sizeof(char);
        if (start >= str.length())
            break;

        end = str.find_first_of(separator, start);
    }
}

void multiview::SetString(std::string_view str, std::string_view separator, tt::TRIM trim)
{
    clear();
    size_t start = 0;
    size_t end = str.find_first_of(separator);

    // The last string will not have a separator, so end == tt::npos, but we still need to add that final string
    for (;;)
    {
        emplace_back(ttlib::emptystring);
        if (trim == tt::TRIM::both || trim == tt::TRIM::left)
        {
            auto begin = str.find_first_not_of(" \t\n\r\f\v", start);
            if (begin != tt::npos)
            {
                start = begin;
            }
            else
            {
                if (end != tt::npos)
                {
                    start = end;
                }
                else
                {
                    // We're at the end, there's nothing here but whitespace, so we're done
                    break;
                }
            }
        }

        if (trim == tt::TRIM::both || trim == tt::TRIM::right)
        {
            auto temp_end = end;
            if (end == tt::npos)
                temp_end = str.length();
            while (temp_end > start && ttlib::is_whitespace(str.at(temp_end - 1)))
            {
                --temp_end;
            }
            back() = str.substr(start, temp_end - start);
        }

        else if (end == tt::npos)
        {
            back() = str.substr(start, str.length() - start);
        }
        else
        {
            back() = str.substr(start, end - start);
        }

        // The last string will not have a separator after it so end will already be set to tt::npos
        if (end == tt::npos)
            break;

        start = end + separator.length();
        if (start >= str.length())
            break;

        end = str.find_first_of(separator, start);
    }
}
