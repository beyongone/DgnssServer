/*
 File:        TStringUtil.h
 Date:        2007-9-14
 Author:      terry
 Description:
 History:
 <author>        <time>      <desc>

 */

#ifndef TSTRINGUTIL_TERRY_
#define TSTRINGUTIL_TERRY_

#include <string>
#include <algorithm>
#include <vector>

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef max
#undef max
#endif //max

#include "TStringCast.h"

namespace comn
{


class StringUtil
{
public:
	/**
	 * 大小写无关比较
	 * @param src
	 * @param dest
	 * @return
	 */
    static int icompare(const std::string& src, const std::string& dest)
    {
#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
        return _stricmp( src.c_str(), dest.c_str() );
#else
        return strcasecmp(src.c_str(), dest.c_str());
#endif

    }

    static std::string mid(const std::string& src, size_t pos, size_t count =
            std::string::npos)
    {
        return src.substr(pos, (std::string::npos == count) ? src.size() : count);
    }

    static std::string left(const std::string& src, size_t count)
    {
        return src.substr(0, count);
    }

    static std::string right(const std::string& src, size_t count)
    {
        size_t start = std::max((size_t) 0, src.size() - count);
        return src.substr(start, count);
    }

    static void toUpper(std::string& src)
    {
        for (size_t idx = 0; idx < src.size(); ++idx)
        {
            src[idx] = toupper(src[idx]);
        }
    }

    static void toLower(std::string& src)
    {
        for (size_t idx = 0; idx < src.size(); ++idx)
        {
            src[idx] = tolower(src[idx]);
        }
    }

    /**
     * 反转
     * @param src
     */
    static void reverse(std::string& src)
    {
        std::string strTmp(src);
        std::copy(strTmp.rbegin(), strTmp.rend(), src.begin());
    }

    static size_t remove(std::string& src, char ch)
    {
        size_t len = src.size();
        src.erase(std::remove(src.begin(), src.end(), ch), src.end());
        return (len - src.size());
    }

    static std::string::iterator getIterator(std::string& src, size_t idx)
	{
		std::string::iterator pos = src.begin();
		size_t len = src.size();
		if ((size_t) idx >= len)
		{
			pos = src.end();
		}
		else
		{
			pos += idx;
		}
		return pos;
	}

    static size_t insert(std::string& src, size_t idx, char ch)
    {
        std::string::iterator pos = getIterator(src, idx);
        src.insert(pos, ch);
        return src.size();
    }

    static size_t insert(std::string& src, size_t idx, const std::string& str)
    {
        std::string::iterator pos = getIterator(src, idx);
        src.insert(pos, str.begin(), str.end());
        return src.size();
    }

    //
    static size_t remove(std::string& src, size_t idx, size_t count = 1)
    {
        size_t nCountNew = std::max((size_t) 0, count);
        std::string::iterator posBegin = getIterator(src, idx);
        std::string::iterator posEnd = getIterator(src, idx + nCountNew);
        src.erase(posBegin, posEnd);
        return src.size();
    }



    static bool isTrimChar(char ch)
	{
		return ((ch == ' ') || (ch == '\t') || (ch == '\r') || (ch == '\n'));
	}

    static size_t trimLeft(std::string& src)
    {
    	size_t count = 0;
        size_t i = 0;
        for (i = 0; i < src.size(); i++)
        {
            if (!isTrimChar(src[i]))
            {
            	count = i;
                break;
            }
        }
        //
        if (i < src.size())
        {
            src.erase(0, i);
        }
        else
        {
            src.clear();
        }
        return count;
    }

    static size_t trimLeft(std::string& src, char ch)
    {
    	size_t count = 0;
        size_t i = 0;
        for (i = 0; i < src.size(); i++)
        {
            if (src[i] != ch)
            {
            	count = i;
                break;
            }
        }
        src.erase(0, i);
        return count;
    }

    static size_t trimRight(std::string& src, char ch)
    {
    	size_t count = 0;
        size_t len = src.size();
        if (len > 0)
        {
            size_t i = len - 1;
            for (; i != std::string::npos; --i)
            {
                if (src[i] != ch)
                {
                    break;
                }
                else
                {
                	count ++;
                }
            }
            src.erase(i + 1, len);
        }
        return count;
    }

    static size_t trimRight(std::string& src)
    {
    	size_t count = 0;
        size_t len = src.size();
        if (len > 0)
        {
            size_t i = len - 1;
            for (; i >= 0; --i)
            {
                if (!isTrimChar(src[i]))
                {
                    break;
                }
                else
                {
                	count ++;
                }
            }
            src.erase(i + 1, len);
        }
        return count;
    }

    static size_t trim(std::string& src)
    {
    	size_t count = trimLeft(src);
    	count += trimRight(src);
    	return count;
    }

    /**
     * 删除开头连续出现在模式集中的字符
     * @param src
     * @param pattern	模式字符集
     * @return	删除字符的个数
     */
    static size_t trimLeft(std::string& src, const std::string& pattern)
    {
    	size_t count = 0;
        size_t i = 0;
        for (i = 0; i < src.size(); i++)
        {
            if (pattern.find(src[i]) == std::string::npos)
            {
            	count = i;
                break;
            }
        }
        //
        if (i < src.size())
        {
            src.erase(0, i);
        }
        else
        {
            src.clear();
        }
        return count;
    }

    /**
     * 删除尾部连续出现在模式集中的字符
     * @param src		源字符串
     * @param pattern	模式字符集
     * @return 删除字符的个数
     */
    static size_t trimRight(std::string& src, const std::string& pattern)
    {
    	size_t count = 0;
        size_t len = src.size();
        if (len > 0)
        {
            size_t i = len - 1;
            for (; i >= 0; --i)
            {
                if (pattern.find(src[i]) == std::string::npos)
                {
                    break;
                }
                else
                {
                	count ++;
                }
            }
            src.erase(i + 1, len);
        }
        return count;
    }

    /**
     * 删除头尾连续出现在模式集中的字符
     * @param src
     * @param pattern	模式字符集
     * @return 删除字符的个数
     */
    static size_t trim(std::string& src, const std::string& pattern)
    {
    	size_t count = trimLeft(src, pattern);
    	count += trimRight(src, pattern);
    	return count;
    }


    /**
     * 替换所有的字符
     * @param src
     * @param chOld
     * @param chNew
     * @return 替换次数
     */
    static size_t replace(std::string& src, char chOld, char chNew)
    {
        size_t count = 0;
        for (size_t i = 0; i < src.size(); ++i)
        {
            if (src[i] == chOld)
            {
                src[i] = chNew;
                count++;
            }
        }
        return count;
    }

    /**
     * 替换字符串
     * @param src	源字符串
     * @param strOld 	要替换的子串
     * @param strNew	新子串
     * @return 替换次数
     */
    static size_t replace(std::string& src, const std::string& strOld,
            const std::string& strNew)
    {
        size_t count = 0;
        size_t idx = src.find(strOld, 0);
        while (idx != std::string::npos)
        {
            src.replace(idx, strOld.size(), strNew);
            count++;
            idx += strNew.size();
            idx = src.find(strOld, idx);
        }
        return count;
    }

    /**
     * 检测是否以指定的子串开头
     * @param src
     * @param target
     * @return
     */
    static bool startsWith(const std::string& src, const std::string& target)
    {
        bool ret = false;
        if (src.size() >= target.size())
        {
            std::string tmp = src.substr(0, target.size());
            ret = (tmp.compare(target) == 0);
        }
        return ret;
    }

    /**
     * 检测是否以指定的子串结尾
     * @param src
     * @param target
     * @return
     */
    static bool endsWith(const std::string& src, const std::string& target)
    {
        bool ret = false;
        if (src.size() >= target.size())
        {
            std::string tmp = src.substr(src.size() - target.size());
            ret = (tmp.compare(target) == 0);
        }
        return ret;
    }

    static bool ensureStartsWith(std::string& src, char ch)
    {
        bool touched = false;
        if (src.empty())
        {
            src.insert(0, 1, ch);
            touched = true;
        }
        else if (src[0] != ch)
        {
            src.insert(0, 1, ch);
            touched = true;
        }
        return touched;
    }

    static bool ensureEndsWith(std::string& src, char ch)
    {
        bool touched = false;
        if (src.empty())
        {
            src.append(1, ch);
            touched = true;
        }
        else if (src[src.length() - 1] != ch)
        {
            src.append(1, ch);
            touched = true;
        }
        return touched;
    }

    static bool ensureStartsWith(std::string& src, const std::string& prefix)
    {
        bool touched = false;
        if (src.empty())
        {
            src = prefix;
            touched = true;
        }
        else if (!startsWith(src, prefix))
        {
            src.insert(0, prefix);
            touched = true;
        }
        return touched;
    }

    static bool ensureEndsWith(std::string& src, const std::string& postfix)
    {
        bool touched = false;
        if (src.empty())
        {
            src = postfix;
            touched = true;
        }
        else if (!endsWith(src, postfix))
        {
            src.append(postfix);
            touched = true;
        }
        return touched;
    }



    static std::string getHead(const std::string& src, char ch)
    {
        size_t pos = src.find(ch);
        if (pos != std::string::npos)
        {
            return src.substr(0, pos);
        }
        return src;
    }

    static std::string getTail(const std::string& src, char ch)
    {
        size_t pos = src.find(ch);
        if (pos != std::string::npos)
        {
            return src.substr(pos+1);
        }
        return std::string();
    }

    static std::string getHead(const std::string& src, const std::string& seperator)
    {
        size_t pos = src.find(seperator);
        if (pos != std::string::npos)
        {
            return src.substr(0, pos);
        }
        return src;
    }

    static std::string getTail(const std::string& src, const std::string& seperator)
    {
        size_t pos = src.find(seperator);
        if (pos != std::string::npos)
        {
            return src.substr(pos+seperator.size());
        }
        return std::string();
    }



    static bool split(const std::string& src, char ch, std::string& head, std::string& tail)
    {
    	bool found = false;
        size_t pos = src.find(ch);
        if (pos != std::string::npos)
        {
        	head = src.substr(0, pos);
        	tail = src.substr(pos+1);
        	found = true;
        }
        else
        {
        	head = src;
        	tail.clear();
        }
        return found;
    }

    template < class Head, class Tail >
    static bool split(const std::string& src, char ch, Head& h, Tail& t)
    {
        std::string head;
        std::string tail;
        if (!split(src, ch, head, tail))
        {
            return false;
        }

        StringCast::toValue(head, h);
        StringCast::toValue(tail, t);

        return true;
    }

    static bool split(const std::string& src, const std::string& seperator, std::string& head, std::string& tail)
    {
    	bool found = false;
        size_t pos = src.find(seperator);
        if (pos != std::string::npos)
        {
        	head = src.substr(0, pos);
        	tail = src.substr(pos+1);
        	found = true;
        }
        else
        {
        	head = src;
        	tail.clear();
        }
        return found;
    }

    template < class Head, class Tail >
    static bool split(const std::string& src, const std::string& seperator, Head& h, Tail& t)
    {
        std::string head;
        std::string tail;
        if (!split(src, seperator, head, tail))
        {
            return false;
        }

        StringCast::toValue(head, h);
        StringCast::toValue(tail, t);

        return true;
    }

    static bool rsplit(const std::string& src, char ch, std::string& head, std::string& tail)
    {
    	bool found = false;
        size_t pos = src.rfind(ch);
        if (pos != std::string::npos)
        {
        	head = src.substr(0, pos);
        	tail = src.substr(pos+1);
        	found = true;
        }
        else
        {
        	head = src;
        	tail.clear();
        }
        return found;
    }

    static bool rsplit(const std::string& src, const std::string& seperator, std::string& head, std::string& tail)
    {
    	bool found = false;
        size_t pos = src.rfind(seperator);
        if (pos != std::string::npos)
        {
        	head = src.substr(0, pos);
        	tail = src.substr(pos+1);
        	found = true;
        }
        else
        {
        	head = src;
        	tail.clear();
        }
        return found;
    }



    template<class Container>
    static size_t split(const std::string& src, char ch, Container& vect)
    {
        if (src.empty())
            return 0;

        size_t count = 0;

        size_t begin = 0;
        size_t pos = src.find(ch, begin);
        while (std::string::npos != pos)
        {
            std::string substr;
            if (pos - begin > 0)
            {
                substr = src.substr(begin, pos - begin);
            }
            vect.push_back(substr);
            count++;

            begin = pos + 1;
            pos = src.find(ch, begin);
        }

        if (begin < src.size())
        {
            vect.push_back(src.substr(begin, std::string::npos));
            count++;
        }
        return count;
    }

    template <class Container>
    static size_t split(const std::string& src, const std::string& sp, Container& cont)
    {
        if (src.empty())
            return 0;

        size_t count = 0;

        size_t begin = 0;
        size_t pos = src.find(sp, begin);
        while (std::string::npos != pos)
        {
            std::string substr;
            if (pos - begin > 0)
            {
                substr = src.substr(begin, pos - begin);
            }
            cont.push_back(substr);
            count++;

            begin = pos + sp.size();
            pos = src.find(sp, begin);
        }

        if (begin < src.size())
        {
            cont.push_back(src.substr(begin, std::string::npos));
            count++;
        }
        return count;
    }


    static std::string extract(const std::string& src, size_t start, char chBegin, char chEnd, bool fullMatch)
    {
        std::string substr;
        size_t posBegin = src.find(chBegin, start);
        if (posBegin == std::string::npos)
        {
            return substr;
        }

        size_t posEnd = src.find(chEnd, posBegin + 1);
        if (posEnd == std::string::npos)
        {
            if (!fullMatch)
            {
                substr = src.substr(posBegin+1);
            }
        }
        else
        {
            substr = src.substr(posBegin+1, posEnd - posBegin - 1);
        }
        return substr;
    }

    static std::string extract(const std::string& src, size_t start, const char* strBegin, 
        const char* strEnd, bool fullMatch)
    {
        std::string substr;
        size_t posBegin = src.find(strBegin, start);
        if (posBegin == std::string::npos)
        {
            return substr;
        }

        size_t beginSize = strlen(strBegin);

        size_t posEnd = src.find(strEnd, posBegin + beginSize);
        if (posEnd == std::string::npos)
        {
            if (!fullMatch)
            {
                substr = src.substr(posBegin+beginSize);
            }
        }
        else
        {
            substr = src.substr(posBegin+beginSize, posEnd - posBegin - beginSize);
        }
        return substr;
    }


    static std::string format(const char* lpszFormat, ...)
    {
        const size_t MAX_SIZE = 1024 * 2;
        char buffer[MAX_SIZE] = {0};
        memset(buffer, '\0', MAX_SIZE);

        va_list args;
        va_start(args, lpszFormat);

        char* buf = buffer;
        vsprint(&buf, MAX_SIZE, lpszFormat, args);

        va_end(args);

        std::string str(buf);

        if (buf != buffer)
        {
            free(buf);
        }

        return str;
    }



public:
    #ifndef va_copy
    #define va_copy(x,y) x = y
    #endif //


    static int vsprint(char **buf, size_t size, const char *fmt, va_list ap)
    {
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4996)
#endif //
        va_list ap_copy;
        int len;

        va_copy(ap_copy, ap);
        len = vsnprintf(*buf, size, fmt, ap_copy);
        va_end(ap_copy);

        if (len < 0)
        {
            // eCos and Windows are not standard-compliant and return -1 when
            // the buffer is too small. Keep allocating larger buffers until we
            // succeed or out of memory.
            *buf = NULL;

            while (len < 0)
            {
                if (*buf)
                {
                    free(*buf);
                }
                size *= 2;
                if ((*buf = (char *)malloc(size)) == NULL)
                    break;
                va_copy(ap_copy, ap);
                len = vsnprintf(*buf, size, fmt, ap_copy);
                va_end(ap_copy);
            }
        }
        else if (len > (int) size)
        {
            // Standard-compliant code path. Allocate a buffer that is large enough.
            if ((*buf = (char *) malloc(len + 1)) == NULL)
            {
                len = -1;
            }
            else
            {
                va_copy(ap_copy, ap);
                len = vsnprintf(*buf, len + 1, fmt, ap_copy);
                va_end(ap_copy);
            }
        }

#if defined(_MSC_VER)
#pragma warning(pop)
#endif //
        return len;
    }




};


} // end of namespace

#endif //TSTRINGUTIL_TERRY_
