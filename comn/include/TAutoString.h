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

/// �ɱ��ʽ����
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

    /// ��ȡ��ǰ������󳤶�
    size_t max_size() const
    {
        return m_capacity;
    }

    /// ��ȡ��ǰ������󳤶�
    size_t capacity() const
    {
        return m_capacity;
    }

    /// ��ȡ��ǰ���峤��
    size_t size() const
    {
        return m_length;
    }

    /**
     * ���û��峤��
     * @param length
     * @return false ������ȳ����������ֵ
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

    /// ��ȡ�ַ���ָ��
    char* data()
    {
        return (m_ptr) ? m_ptr : m_data;
    }

    /// ��ȡ�ַ�������ָ��
    const char* c_str() const
    {
        return (m_ptr) ? m_ptr : m_data;
    }

    /**
     * ȷ��������󳤶�
     * @param length
     * @return false ��������ڴ�ʧ��
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
     * ��ʽ������printfһ��
     * @param fmt
     * @return ��ʽ���󻺳峤��
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

    /// �ַ���ָ��ת��������
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
