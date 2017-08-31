/*    file: CStr.cpp
 *    desc:
 * 
 * created: 2013-08-14 10:53:18
 *  author: zhengchuanjiang
 * company: 
 */

#include "CStr.h"

#undef min

#include <algorithm>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>


namespace comn
{



const size_t CStr::npos = -1;



int icompareChar(char ch, char ch2)
{
    if (ch == ch2)
    {
        return 0;
    }
    int ret = tolower(ch) > tolower(ch2);
    if (ret == 0)
    {
        return 0;
    }
    return (ret < 0) ? -1 : 1;
}

size_t findStr(const char* str, size_t length, const char* sp, size_t spLen)
{
    if (spLen > length)
    {
        return -1;
    }

    for (size_t i = 0; i < (length - spLen); ++ i)
    {
        if (strncmp(str + i, sp, spLen) == 0)
        {
            return i;
        }
    }

    return -1;
}

size_t rfindStr(const char* str, size_t length, const char* sp, size_t spLen)
{
    if (spLen > length)
    {
        return -1;
    }
    size_t pos = length - spLen;
    for (size_t i = pos; i > 0; -- i)
    {
        if (strncmp(str + i - 1, sp, spLen) == 0)
        {
            return (i - 1);
        }
    }
    return -1;
}






CStr::CStr():
m_ptr(),
m_length()
{
}

CStr::CStr(const char* str):
    m_ptr(str),
    m_length(strlen(str))
{
}

CStr::CStr(const char* str, size_t length):
    m_ptr(str),
    m_length(length)
{

}

CStr::CStr(const CStr& str):
    m_ptr(str.m_ptr),
    m_length(str.m_length)
{
    
}

void CStr::assign(const CStr& str)
{
    m_ptr = str.m_ptr;
    m_length = str.m_length;
}

void CStr::assign(const char* str)
{
    m_ptr = str;
    m_length = strlen(str);
}

const CStr& CStr::operator = (const CStr& str)
{
    assign(str);
    return *this;
}

const CStr& CStr::operator = (const char* str)
{
    assign(str);
    return *this;
}

bool CStr::operator < (const CStr& str) const
{
    return compare(str) < 0;
}

bool CStr::operator == (const CStr& str) const
{
    return compare(str) == 0;
}




bool CStr::empty() const
{
    return (m_ptr == NULL) || (m_length == 0);
}

void CStr::reset(const char* str, size_t length)
{
    m_ptr = str;
    m_length = length;
}

void CStr::reset(const CStr& str)
{
    assign(str);
}

void CStr::reset()
{
    m_ptr = NULL;
    m_length = 0;
}

void CStr::clear()
{
    m_ptr = NULL;
    m_length = 0;
}

bool CStr::advance(size_t step)
{
    if (step > m_length)
    {
        return false;
    }

    m_ptr += step;
    m_length -= step;
    return true;
}

const char* CStr::c_str() const
{
    return m_ptr;
}

size_t CStr::size() const
{
    return m_length;
}

size_t CStr::length() const
{
    return m_length;
}

const char* CStr::begin() const
{
    return m_ptr;
}

const char* CStr::end() const
{
    return (m_ptr == NULL) ? NULL : (m_ptr + m_length);
}

char CStr::operator [](size_t idx) const
{
    return m_ptr[idx];
}

int CStr::compare(const char* str, size_t length) const
{
    if (empty())
    {
        return (length == 0) ? 0 : -1;
    }

    size_t minLength = std::min(m_length, length);
    int ret = strncmp(m_ptr, str, minLength);
    if (ret == 0)
    {
        if (m_length < length)
        {
            ret = -1;
        }
        else if (m_length > length)
        {
            ret = 1;
        }
    }
    return ret;
}

int CStr::icompare(const char* str, size_t length) const
{
    if (empty())
    {
        return (length == 0) ? 0 : -1;
    }

    size_t minLength = std::min(m_length, length);
    for (size_t i = 0; i < minLength; ++ i)
    {
        int ret = icompareChar(m_ptr[i], str[i]);
        if (ret != 0)
        {
            return (ret < 0) ? -1 : 1;
        }
    }

    if (m_length == length)
    {
        return 0;
    }
    return (m_length < length) ? -1 : 1;
}

int CStr::compare(const char* str) const
{
    return compare(str, strlen(str));
}

int CStr::icompare(const char* str) const
{
    return icompare(str, strlen(str));
}

int CStr::compare(const CStr& str) const
{
    return compare(str.m_ptr, str.m_length);
}

int CStr::icompare(const CStr& str) const
{
    return icompare(str.m_ptr, str.m_length);
}

bool CStr::equal(const CStr& str) const
{
    return compare(str) == 0;
}

bool CStr::equal(const char* str) const
{
	return compare(str) == 0;
}

bool CStr::iequal(const CStr& str) const
{
    return icompare(str) == 0;
}



size_t CStr::find(char ch, size_t pos) const
{
    for (size_t i = pos; i < m_length; ++ i)
    {
        if (m_ptr[i] == ch)
        {
            return i;
        }
    }
    return npos;
}

size_t CStr::find(const char* str, size_t length, size_t pos) const
{
    if (pos >= m_length)
    {
        return npos;
    }

    if ((pos + length) > m_length)
    {
        return npos;
    }

    size_t idx = findStr(m_ptr + pos, m_length - pos, str, length);

    return (idx == npos) ? idx : (idx + pos);
}

size_t CStr::find(const char* str, size_t pos) const
{
    return find(str, strlen(str), pos);
}

size_t CStr::rfind(char ch, size_t pos) const
{
    if (empty())
    {
        return npos;
    }

    size_t idx = (pos == npos) ? m_length : (pos + 1);
    for (size_t i = idx; i > 0; -- i)
    {
        if (m_ptr[i-1] == ch)
        {
            return (i-1);
        }
    }
    return npos;
}

size_t CStr::rfind(const char* str, size_t length, size_t pos) const
{
    if (pos >= m_length)
    {
        return npos;
    }

    if ((pos + length) > m_length)
    {
        return npos;
    }

    return rfindStr(m_ptr, m_length - pos, str, length);
}

size_t CStr::rfind(const char* str, size_t pos) const
{
    return rfind(str, strlen(str), pos);
}


CStr CStr::substr(size_t pos, size_t n) const
{
    CStr cstr;
    if (pos < m_length)
    {
        size_t length = m_length - pos;
        if (n < length)
        {
            length = n;
        }
        cstr.reset(m_ptr + pos, length);
    }
    return cstr;
}

CStr CStr::mid(size_t pos, size_t endPos) const
{
    CStr cstr;
    if (pos < m_length)
    {
        if (endPos > m_length)
        {
            endPos = m_length - 1;
        }
        cstr.reset(m_ptr + pos, (endPos - pos));
    }
    return cstr;
}


bool CStr::isNumber() const
{
    if (empty())
    {
        return false;
    }

    for (size_t i = 0; i < m_length; ++ i)
    {
        if (!::isdigit(m_ptr[i]))
        {
            return false;
        }
    }
    return true;
}

bool CStr::startWith(const char* str, size_t length) const
{
    if (m_length < length)
    {
        return false;
    }
    return compare(str, length) == 0;
}

bool CStr::startWith(const char* str) const
{
    return startWith(str, strlen(str));
}

bool CStr::endWith(const char* str, size_t length) const
{
    if (m_length < length)
    {
        return false;
    }

    size_t offset = m_length - length;
    CStr cstr(m_ptr + offset, length);
    return cstr.compare(str, length) == 0;
}

bool CStr::endWith(const char* str) const
{
    return endWith(str, strlen(str));
}


size_t CStr::split(char sp, CStr& head, CStr& tail) const
{
    if (empty())
    {
        return 0;
    }

    size_t count = 0;
    size_t idx = find(sp);
    if (idx == npos)
    {
        head.reset(m_ptr, m_length);
        tail.reset();
        count = 1;
    }
    else
    {
        head.reset(m_ptr, idx);

        idx ++;
        tail.reset(m_ptr + idx, m_length - idx);

        count = 2;
    }
    return count;
}

size_t CStr::split(const char* sp, CStr& head, CStr& tail) const
{
    if (empty())
    {
        return 0;
    }

    size_t count = 0;
    size_t idx = find(sp);
    if (idx == npos)
    {
        head.reset(m_ptr, m_length);
        tail.reset();
        count = 1;
    }
    else
    {
        head.reset(m_ptr, idx);

        idx ++;
        tail.reset(m_ptr + idx, m_length - idx);

        count = 2;
    }
    return count;
}

bool CStr::extract(char spBegin, char spEnd, CStr& str) const
{
    size_t beginPos = find(spBegin);
    if (beginPos == npos)
    {
        return false;
    }

    beginPos ++;
    size_t endPos = find(spEnd, beginPos);
    if (endPos == npos)
    {
        return false;
    }

    str.reset(m_ptr + beginPos, (endPos - beginPos));
    return true;
}

bool CStr::extract(const char* spBegin, const char* spEnd, CStr& str) const
{
    size_t beginLength = strlen(spBegin);
    size_t beginPos = find(spBegin, beginLength, 0);
    if (beginPos == npos)
    {
        return false;
    }

    beginPos += beginLength;
    size_t endPos = find(spEnd, beginPos);
    if (endPos == npos)
    {
        return false;
    }

    str.reset(m_ptr + beginPos, (endPos - beginPos));
    return true;
}


void CStr::toValue(short& number) const
{
    number = (short)atoi(m_ptr);
}

void CStr::toValue(unsigned short& number) const
{
    number = (unsigned short)atoi(m_ptr);
}

void CStr::toValue(int& number) const
{
    number = atoi(m_ptr);
}

void CStr::toValue(unsigned int& number) const
{
    number = atoi(m_ptr);
}

void CStr::toValue(long& number) const
{
    number = atol(m_ptr);
}

void CStr::toValue(unsigned long& number) const
{
    number = atol(m_ptr);
}

void CStr::toValue(float& number) const
{
    number = (float)atof(m_ptr);
}

void CStr::toValue(double& number) const
{
    number = atof(m_ptr);
}


//////////////////////////////////////////////////////////////////////////


CharToken::CharToken(const char* str, size_t length, char sp):
m_src(str, length),
m_sp(sp)
{
}

bool CharToken::next(CStr& rawstr)
{
	if (m_src.length() == 0)
	{
		return false;
	}

	size_t idx = m_src.find(m_sp, 0);
	if (idx == CStr::npos)
	{
		rawstr = m_src;
		m_src.reset();
	}
	else
	{
		rawstr.reset(m_src.c_str(), idx);
		m_src.advance(idx + 1);
	}
	return true;
}

///////////////////////////////////////////////////////////////////
StrToken::StrToken(const char* str, size_t length, const char* sp):
m_src(str, length),
m_sp(sp)
{
}

bool StrToken::next(CStr& rawstr)
{
	if (m_src.length() == 0)
	{
		return false;
	}

	size_t idx = m_src.find(m_sp.c_str(), m_sp.length());
	if (idx == CStr::npos)
	{
		rawstr = m_src;
		m_src.reset();
	}
	else
	{
		rawstr.reset(m_src.c_str(), idx);
		m_src.advance(idx + m_sp.length());
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

}
