/*
 * TAutoString.h
 *
 *  Created on: 2012-3-24
 *      Author: terry
 */

#ifndef TAUTOSTRING_H_
#define TAUTOSTRING_H_

#include <stdio.h>
#include <stdarg.h>
#include <string.h>


namespace comn
{

/// 可变格式化类
template < int MAX_LENGTH >
class AutoString
{
public:
    AutoString():
        m_ptr(),
        m_capacity(MAX_LENGTH),
        m_length()
    {
        memset(m_data, 0, MAX_LENGTH);
    }

    ~AutoString()
    {
        destroyBuffer();
    }

    /// 获取当前缓冲最大长度
    size_t max_size() const
    {
        return m_capacity;
    }

    /// 获取当前缓冲最大长度
    size_t capacity() const
    {
        return m_capacity;
    }

    /// 获取当前缓冲长度
    size_t size() const
    {
        return m_length;
    }

    /**
     * 设置缓冲长度
     * @param length
     * @return false 如果长度超出缓冲最大值
     */
    bool setSize(size_t length)
    {
        if (length >= m_capacity)
        {
            return false;
        }

        m_length = length;
        return true;
    }

    /// 获取字符串指针
    char* data()
    {
        return (m_ptr) ? m_ptr : m_data;
    }

    /// 获取字符串常量指针
    const char* c_str() const
    {
        return (m_ptr) ? m_ptr : m_data;
    }

    /**
     * 确保缓冲最大长度
     * @param length
     * @return false 如果分配内存失败
     */
    bool ensure(size_t length)
    {
        if (length < m_capacity)
        {
            m_length = length;
            return true;
        }

        destroyBuffer();

        if (createBuffer(length + MAX_LENGTH/2))
        {
            m_length = length;
        }
        return (m_ptr != NULL);
    }

    /**
     * 格式化，与printf一致
     * @param fmt
     * @return 格式化后缓冲长度
     */
    size_t format(const char* fmt, ...)
    {
        va_list valist;
        va_start(valist, fmt);

    #if defined(_MSC_VER) && (_MSC_VER >= 1400 )
        size_t length = _vscprintf(fmt, valist);
        ensure(length);
        length = vsprintf_s(data(), capacity(), fmt, valist);

    #else
        size_t length = vsprintf(NULL, fmt, valist);
        ensure(length);
        length = vsprintf(data(), fmt, valist);
    #endif

        m_length = length;
        data()[m_length] = '\0';

        va_end(valist);

        return length;
    }

    /// 字符串指针转换操作符
    operator const char* () const
    {
        return c_str();
    }

    void assign(const char* str, size_t length)
    {
        ensure(length);

        memcpy(data(), str, length);
        m_length = length;
    }

    void assign(const char* str)
    {
        return assign(str, strlen(str));
    }

    AutoString< MAX_LENGTH >& operator = (const char* str)
    {
        assign(str);
        return (*this);
    }

protected:
    bool createBuffer(size_t capacity)
    {
        m_ptr = new char[capacity];
        if (m_ptr)
        {
            memset(m_ptr, 0, capacity);
            m_capacity = capacity;
        }
        return (m_ptr != NULL);
    }

    void destroyBuffer()
    {
        delete[] m_ptr;
        m_ptr = NULL;

        m_capacity = MAX_LENGTH;
    }

protected:
    char    m_data[MAX_LENGTH];
    char*   m_ptr;
    size_t  m_capacity;
    size_t  m_length;

};










}

#endif /* TAUTOSTRING_H_ */
