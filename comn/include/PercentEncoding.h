/*
 * PercentEncoding.h
 *
 * Created on: 2011-3-18
 *     Author: terry
 */

#ifndef PERCENTENCODING_H_
#define PERCENTENCODING_H_

#include <string>
///////////////////////////////////////////////////////////////////
namespace comn
{

class  PercentEncoding
{
public:
    PercentEncoding();
    ~PercentEncoding();

    static char hexToAscii(char ch);
    static char asciiToHex(char ch);

    static void charToHex(char ch, char& highPart, char& lowPart);
    static char hexToChar(char highPart, char lowPart);

    static std::string encode(const std::string& src);

    static std::string decode(const std::string& src);

};


}

///////////////////////////////////////////////////////////////////


#endif /* PERCENTENCODING_H_ */
