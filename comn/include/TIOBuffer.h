/*
 * TIOBuffer.h
 *
 * Created on: 2011-5-16
 *     Author: terry
 */

#ifndef TIOBUFFER_H_
#define TIOBUFFER_H_

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
////////////////////////////////////////////////////////////////////////////
namespace comn
{

class IOBuffer
{

public:
    enum
    {
        MEMORY_INCREASE_STEP = 1024
    };

    typedef unsigned char   uint8_t;

protected:
    uint8_t*   m_ptr;
    size_t  m_length;
    size_t  m_readPos;
    size_t  m_writePos;
    size_t  m_increaseStep;

public:
    IOBuffer():
        m_ptr(NULL),
        m_length(0),
        m_readPos(0),
        m_writePos(0),
        m_increaseStep(MEMORY_INCREASE_STEP)
    {
    }

    ~IOBuffer()
    {
        cleanup();
    }

    explicit IOBuffer(size_t size):
        m_ptr(NULL),
        m_length(0),
        m_readPos(0),
        m_writePos(0),
        m_increaseStep(MEMORY_INCREASE_STEP)
    {
        expect(size);
    }


    IOBuffer(const IOBuffer& ref):
        m_ptr(NULL),
        m_length(0),
        m_readPos(0),
        m_writePos(0),
        m_increaseStep(MEMORY_INCREASE_STEP)
    {
        assign(ref);
    }

    const IOBuffer& operator = (const IOBuffer& ref)
    {
        assign(ref);
        return (*this);
    }

    bool expect(size_t expected)
    {
        if (expected == 0)
        {
            return true;
        }

        if ((m_writePos + expected) <= m_length)
        {
            return true;
        }

        if (!m_ptr)
        {
            m_ptr = new uint8_t[expected];
            memset(m_ptr, 0, expected);
            m_length = expected;
            m_writePos = 0;
            m_readPos = 0;

            return (m_ptr != NULL);
        }

        if ((m_writePos - m_readPos + expected) <= m_length)
        {
            moveData();
            return true;
        }

        bool done = false;
        size_t newSize = m_writePos + expected + m_increaseStep;

        uint8_t* pTmp = new uint8_t[newSize];
        if (pTmp)
        {
            memset(pTmp, 0, newSize);
            memcpy(pTmp, m_ptr + m_readPos, m_writePos - m_readPos);

            delete [] m_ptr;
            m_ptr = pTmp;

            m_length = newSize;
            m_writePos -= m_readPos;
            m_readPos = 0;

            done = true;
        }

        return done;
    }

    bool ensure(size_t size)
    {
        if (m_length >= size)
        {
            return true;
        }

        return expect(size - m_writePos);
    }

    void clear()
    {
        m_readPos = 0;
        m_writePos = 0;
    }

    size_t read(unsigned char* buffer, size_t length)
    {
        if (!buffer || length == 0)
        {
            return 0;
        }

        size_t left = readable();
        if (left < length)
        {
            length = left;
        }
        memcpy(buffer, m_ptr + m_readPos, length);
        m_readPos += length;

        return length;
    }

    size_t write(const unsigned char* buffer, size_t length)
    {
        if (!buffer || length == 0)
        {
            return 0;
        }

        if (!expect(length))
        {
            return 0;
        }

        memcpy(m_ptr + m_writePos, buffer, length);
        m_writePos += length;
        return length;
    }

    size_t readable() const
    {
        return (m_writePos - m_readPos);
    }

    size_t writable() const
    {
        return (m_length - m_writePos);
    }

    const uint8_t* getReadPtr() const
    {
        return m_ptr + m_readPos;
    }

    const uint8_t* getWritePtr() const
    {
        return m_ptr + m_writePos;
    }

    uint8_t* getReadPtr()
    {
        return m_ptr + m_readPos;
    }

    uint8_t* getWritePtr()
    {
        return m_ptr + m_writePos;
    }

    uint8_t* getPtr()
    {
        return m_ptr;
    }

    size_t getReadPos() const
    {
        return m_writePos;
    }

    size_t getWritePos() const
    {
        return m_readPos;
    }

    bool checkReadable(size_t length) const
    {
        return ((m_writePos - m_readPos) >= length);
    }

    bool checkWritable(size_t length) const
    {
        return (writable() >= length);
    }

    size_t capacity() const
    {
        return m_length;
    }

    size_t size() const
    {
        return readable();
    }

    bool empty() const
    {
        return size() == 0;
    }

    std::string toString() const
    {
        std::stringstream   ss;
        ss << "size: " << m_length;
        ss << ";";
        ss << "out: " << m_readPos;
        ss << ";";
        ss << "in: " << m_writePos;
        ss << std::endl;
        return ss.str();
    }

    template < class T >
    size_t writeValue(T value)
    {
        return write((uint8_t*)&value, sizeof(value));
    }

    template < class T >
    size_t readValue(T& value)
    {
        return read((uint8_t*)&value, sizeof(value));
    }

    size_t writeString(const char* str, size_t length)
    {
        return write((const uint8_t*)str, length);
    }

    size_t writeString(const std::string& str)
    {
        return write((const uint8_t*)str.c_str(), str.size());
    }

    size_t readString(char* str, size_t length)
    {
        return read((uint8_t*)str, length);
    }

    size_t readString(std::string& value, size_t length = (size_t)-1)
    {
        size_t left = readable();
        if (left < length)
        {
            length = left;
        }
        else if (length == size_t(-1))
        {
            length = left;
        }

        if (length > 0)
        {
            value.assign((char*)getReadPtr(), length);
            m_readPos += length;

            recycle();
        }
        return length;
    }

    uint8_t& operator [] (size_t idx)
    {
        return (m_ptr + m_readPos)[idx];
    }

    size_t writeFrom(const IOBuffer& buf, size_t length)
    {
        size_t left = buf.readable();
        if (left < length)
        {
            length = left;
        }

        return write(buf.getReadPtr(), length);
    }

    size_t writeFrom(const IOBuffer& buf)
    {
        return write(buf.getReadPtr(), buf.readable());
    }

    size_t moveFrom(IOBuffer& buf, size_t length)
    {
        size_t ret = writeFrom(buf, length);
        buf.skip(ret);
        return ret;
    }

    bool skip(size_t size)
    {
        bool done = false;
        if (checkReadable(size))
        {
            m_readPos += size;
            recycle();
            done = true;
        }
        return done;
    }

    void skipAll()
    {
        m_readPos = 0;
        m_writePos = 0;
    }

    bool advance(size_t size)
    {
        bool done = false;
        if ((m_length - m_writePos) >= size)
        {
            m_writePos += size;
            done = true;
        }
        return done;
    }

    size_t fromStream(std::istream& input, size_t length)
    {
        expect(length);

        input.read((char*)getWritePtr(), length);
        size_t ret = (size_t)input.gcount();
        m_writePos += ret;

        return ret;
    }

    size_t toStream(std::ostream& os, size_t length)
    {
        size_t left = readable();
        if (left < length)
        {
            length = left;
        }
        os.write((char*)getReadPtr(), length);
        m_readPos += length;

        recycle();

        return length;
    }

    bool fromFile(const char* filename)
    {
        std::ifstream ifs;
        ifs.open(filename, std::ios::binary);
        if (!ifs.is_open())
        {
            return false;
        }

        ifs.seekg(0, std::ios_base::end);
        size_t length = (size_t)ifs.tellg();
        ifs.seekg(0);

        expect(length);

        ifs.read((char*)getWritePtr(), length);

        m_writePos += length;

        ifs.close();
        return true;
    }

    bool toFile(const char* filename) const
    {
        std::ofstream ofs;
        ofs.open(filename, std::ios::binary);
        if (!ofs.is_open())
        {
            return false;
        }

        ofs.write((char*)getReadPtr(), readable());
        ofs.close();
        return true;
    }

    void setIncreaseStep(size_t length)
    {
        if (length > 0)
        {
            m_increaseStep = length;
        }
    }

    void moveData()
    {
        if (m_readPos == 0)
        {
            return;
        }

        if ((m_writePos - m_readPos) <= m_readPos)
        {
            memcpy(m_ptr, m_ptr + m_readPos, (m_writePos - m_readPos));
            m_writePos -= m_readPos;
            m_readPos = 0;
        }
        else
        {
            uint8_t* pDest = m_ptr;
            uint8_t* pSrc = m_ptr + m_readPos;
            for (size_t i = 0; i < ((m_writePos - m_readPos)/m_readPos); ++ i)
            {
                pDest = m_ptr + (i * m_readPos);
                pSrc = pDest + m_readPos;
                memcpy(pDest, pSrc, m_readPos);
            }

            size_t left = (m_writePos - m_readPos) % m_readPos;
            if (left != 0)
            {
                pDest += m_readPos;
                pSrc += m_readPos;
                memcpy(pDest, pSrc, left);
            }

            m_writePos -= m_readPos;
            m_readPos = 0;
        }
    }

protected:
    void cleanup()
    {
        if (m_ptr)
        {
            delete[] m_ptr;
            m_ptr = NULL;

            m_writePos = 0;
            m_readPos = 0;
            m_length = 0;
        }
    }

    void recycle()
    {
        if (m_readPos == m_writePos)
        {
            m_readPos = 0;
            m_writePos = 0;
        }
    }

    void assign(const IOBuffer& ref)
    {
        expect(ref.m_length);

        if (m_length > 0)
        {
            memcpy(m_ptr, ref.m_ptr, m_length);
            m_readPos = ref.m_readPos;
            m_writePos = ref.m_writePos;
        }
    }


};



}

////////////////////////////////////////////////////////////////////////////

#endif /* TIOBUFFER_H_ */
