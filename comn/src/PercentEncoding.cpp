/*
 * PercentEncoding.cpp
 *
 * Created on: 2011-3-18
 *     Author: terry
 */

///////////////////////////////////////////////////////////////////
#include "PercentEncoding.h"
#include <ctype.h>


namespace comn
{

PercentEncoding::PercentEncoding()
{
}

PercentEncoding::~PercentEncoding()
{
}


char PercentEncoding::hexToAscii(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    return '0';
}

char PercentEncoding::asciiToHex(char ch)
{
    if (ch <= 9)
    {
        return '0' + ch;
    }
    else if (ch <= 15)
    {
        return 'A' + ch - 10;
    }
    else
    {
        return '0';
    }
}

void PercentEncoding::charToHex(char ch, char& highPart, char& lowPart)
{
    highPart = asciiToHex((ch >> 4) & 0x0f);
    lowPart = asciiToHex(ch & 0x0f);
}

char PercentEncoding::hexToChar(char highPart, char lowPart)
{
    char ch = hexToAscii(highPart) << 4;
    ch += hexToAscii(lowPart);
    return ch;
}

std::string PercentEncoding::encode(const std::string& src)
{
    std::string dest;
    dest.reserve(src.size() * 3 / 2);
    for (size_t i = 0; i < src.size(); ++ i)
    {
        unsigned char ch = src[i];
        switch (ch)
        {
        //unreserved
        case '-' :
        case '.' :
        case '_' :
        case '~' :
            dest += src[i];
            break;
        default:
            {
                if (isalnum(ch))
                {
                    dest += src[i];
                }
                else if (isspace(ch))
                {
                    //dest += '+';
                    dest += "%20";
                }
                else
                {
                    dest += '%';

                    char highPart = 0;
                    char lowPart = 0;
                    charToHex(src[i], highPart, lowPart);

                    dest += highPart;
                    dest += lowPart;
                }
            }
            break;
        }
    }
    return dest;
}

std::string PercentEncoding::decode(const std::string& src)
{
    std::string dest;
    dest.reserve(src.size());
    size_t i = 0;
    while (i < src.size())
    {
        if (src[i] == '%')
        {
            if (i + 2 < src.size())
            {
                dest += hexToChar(src[i+1], src[i+2]);
            }

            i +=3;
        }
        else if (src[i] == '+')
        {
            dest += ' ';
            ++ i;
        }
        else
        {
            dest += src[i];
            ++ i;
        }
    }
    return dest;
}



}

///////////////////////////////////////////////////////////////////

