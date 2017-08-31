/*
 * CharArray.h
 *
 *  Created on: 2015年5月28日
 *      Author: chuanjiang.zh@qq.com
 */

#ifndef INCLUDE_CHARARRAY_H_
#define INCLUDE_CHARARRAY_H_

#include <string>
#include <stdarg.h>


namespace comn
{


template < int MAX_SIZE >
class CharArrayT
{
public:
    CharArrayT():
        m_ptr(),
        m_capacity(MAX_SIZE),
        m_length()
    {
        memset(m_data, 0, MAX_SIZE);
    }

    ~CharArrayT()
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
    bool resize(size_t length)
    {
        ensure(length);

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
     * 格式化，与printf一致
     * @param fmt
     * @return 格式化后缓冲长度
     */
    size_t format(const char* fmt, ...)
    {
        va_list argList;
        va_start(argList, fmt);

        char* buf = data();
        size_t length = vsprint(&buf, m_length, fmt, argList);
        va_end(valist);
        if (buf != data())
        {
            destroyBuffer();

            m_ptr = buf;
            m_length = length;
            m_capacity = m_length;
        }
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

    CharArrayT< MAX_SIZE >& operator = (const char* str)
    {
        assign(str);
        return (*this);
    }

protected:
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

        if (createBuffer(length + MAX_SIZE/2))
        {
            m_length = length;
        }
        return (m_ptr != NULL);
    }

    bool createBuffer(size_t capacity)
    {
        m_ptr = (char *)malloc(capacity);
        if (m_ptr)
        {
            memset(m_ptr, 0, capacity);
            m_capacity = capacity;
        }
        return (m_ptr != NULL);
    }

    void destroyBuffer()
    {
        if (m_ptr)
        {
            free(m_ptr);
            m_ptr = NULL;
        }

        m_capacity = MAX_SIZE;
    }

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4996)
#endif //

    int vsprint(char **buf, size_t size, const char *fmt, va_list ap)
    {
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

        return len;
    }
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //

protected:
    char    m_data[MAX_SIZE];
    char*   m_ptr;
    size_t  m_capacity;
    size_t  m_length;

};


typedef CharArrayT< 1024 >  CharArray;





} /* namespace comn */

#endif /* INCLUDE_CHARARRAY_H_ */
