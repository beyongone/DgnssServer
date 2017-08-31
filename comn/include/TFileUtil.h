/*
 * TFileUtil.h
 *
 *  Created on: 2014年3月3日
 *      Author: zhengchuanjiang
 */

#ifndef TFILEUTIL_H_
#define TFILEUTIL_H_


#include <string>
#include <stdio.h>


namespace comn
{

class FileUtil
{
public:
    static bool load(std::string& str, const char* filepath)
    {
        FILE* pFile = NULL;
        #if defined(_MSC_VER) && (_MSC_VER >= 1400 )
        pFile = _fsopen(filepath, "rb", _SH_DENYNO);
        #else
        pFile = fopen(filepath, "rb");
        #endif //

        if (!pFile)
        {
            return false;
        }

        str.reserve(1024 * 1024 * 1);

        static const size_t BUFFER_SIZE = 1024 * 128;
        char buffer[BUFFER_SIZE];
        while (!feof(pFile))
        {
            size_t count = fread(buffer, 1, BUFFER_SIZE, pFile);
            if (count == 0)
            {
                break;
            }

            str.append(buffer, count);
        }

        fclose(pFile);
        return true;
    }

    static bool write(const char* data, size_t length, const char* filepath, bool append)
    {
        return write((const unsigned char*)data, length, filepath, append);
    }

    static bool write(const unsigned char* data, size_t length, const char* filepath, bool append)
    {
        FILE* pFile = NULL;
        if (append)
        {
            #if defined(_MSC_VER) && (_MSC_VER >= 1400 )
            pFile = _fsopen(filepath, "ab", _SH_DENYWR);
            #else
            pFile = fopen(filepath, "ab");
            #endif //
        }
        else
        {
            #if defined(_MSC_VER) && (_MSC_VER >= 1400 )
            pFile = _fsopen(filepath, "wb", _SH_DENYWR);
            #else
            pFile = fopen(filepath, "wb");
            #endif //
        }

        if (!pFile)
        {
            return false;
        }

        fwrite(data, 1, length, pFile);


        fclose(pFile);
        return true;
    }

    static bool dump(const std::string& data, const char* filepath)
    {
        return write(data.c_str(), data.length(), filepath, false);
    }

    static bool append(const std::string& data, const char* filepath)
    {
        return write(data.c_str(), data.length(), filepath, true);
    }

};



}


#endif /* TFILEUTIL_H_ */
