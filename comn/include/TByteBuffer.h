/*
 * TByteBuffer.h
 *
 *  Created on: 2014年3月3日
 *      Author: zhengchuanjiang
 */

#ifndef TBYTEBUFFER_H_
#define TBYTEBUFFER_H_


#include <string.h>
#include <algorithm>

namespace comn
{

typedef unsigned char uint8_t;

class ByteBuffer
{
public:
    static const size_t RESERVED_SIZE = 16;

public:
    ByteBuffer():
        m_ptr(NULL),
        m_capacity(0),
        m_length(0)
    {

    }

    virtual ~ByteBuffer()
    {
        cleanup();
    }

    explicit ByteBuffer(size_t capacity):
        m_ptr(NULL),
        m_capacity(0),
        m_length(0)
    {
        internalEnsure(capacity);
    }

    ByteBuffer(const ByteBuffer& other):
        m_ptr(NULL),
        m_capacity(0),
        m_length(0)
    {
        assign(other);
    }

    ByteBuffer& operator = (const ByteBuffer& other)
    {
        assign(other);
        return (*this);
    }

    void reserve(size_t capacity)
    {
        ensure(capacity);
    }

    bool expect(size_t length)
    {
        return ensure(m_length + length);
    }

    const char* c_str() const
    {
        return (const char*)m_ptr;
    }

    const uint8_t* data() const
    {
        return m_ptr;
    }

    uint8_t* data()
    {
        return m_ptr;
    }

    uint8_t& operator [] (size_t idx)
    {
        return m_ptr[idx];
    }

    const uint8_t& operator [] (size_t idx) const
    {
        return m_ptr[idx];
    }

    size_t size() const
    {
        return m_length;
    }

    size_t  length() const
    {
        return m_length;
    }

    size_t capacity() const
    {
        return m_capacity - RESERVED_SIZE;
    }

    bool empty() const
    {
        return (m_length == 0);
    }

    bool isValid() const
    {
        return (m_ptr != NULL);
    }

    void clear()
    {
        m_length = 0;
    }

    bool resize(size_t length)
    {
        ensure(length);

        m_length = length;
        return true;
    }

    bool write(const uint8_t* data, size_t length)
    {
        if ((data == NULL) || (length == 0))
        {
            return true;
        }

        if (!expect(length))
        {
            return false;
        }

        memcpy(m_ptr + m_length, data, length);
        m_length += length;
        m_ptr[m_length] = '\0';

        return true;
    }

    bool write(const char* data, size_t length)
    {
        return write((const uint8_t*)data, length);
    }

    bool write(uint8_t ch)
    {
        if (!expect(sizeof(uint8_t)))
        {
            return false;
        }

        m_ptr[m_length] = ch;
        m_length += sizeof(uint8_t);

        m_ptr[m_length] = '\0';
        return true;
    }

    bool write(char ch)
    {
        return write((uint8_t)ch);
    }

    bool ensure(size_t capacity)
    {
        return internalEnsure(capacity);
    }

    void cleanup()
    {
        if (m_ptr)
        {
            delete[] m_ptr;
            m_ptr = NULL;

            m_length = 0;
            m_capacity = 0;
        }
    }

    void trim(size_t length)
    {
        if (!m_ptr)
        {
            return;
        }

        if (length >= m_length)
        {
            m_length = 0;
        }
        else
        {
            memmove(m_ptr, m_ptr + length, m_length - length);
            m_length -= length;
        }
    }

private:
    void assign(const ByteBuffer& other)
    {
        internalEnsure(other.m_length);

        memcpy(m_ptr, other.data(), other.length());
        m_length = other.length();
        m_ptr[m_length] = '\0';
    }

    bool internalEnsure(size_t capacity)
    {
        capacity += RESERVED_SIZE;

        if (capacity <= m_capacity)
        {
            return true;
        }

        size_t newCapacity = (m_capacity + capacity) * 2;

        uint8_t* buffer = new uint8_t[newCapacity];
        if (!buffer)
        {
            return false;
        }

        if (m_length > 0)
        {
            memcpy(buffer, m_ptr, m_length);
        }

        std::swap(m_ptr, buffer);
        m_capacity = newCapacity;
        m_ptr[m_length] = '\0';

        delete[] buffer;

        return true;
    }

protected:
    uint8_t*  m_ptr;
    size_t  m_capacity;
    size_t  m_length;
};



}

#endif //TBYTEBUFFER_H_
