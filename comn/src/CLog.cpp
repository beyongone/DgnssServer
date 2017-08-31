/*
 * CLog.cpp
 *
 *  Created on: 2015年4月22日
 *      Author: chuanjiang.zh@qq.com
 */
//#include "stdafx.h"
#include "CLog.h"
#include <errno.h>
#include <iostream>
#include <memory>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <vector>

#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>


#else
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <syslog.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#endif //


#ifndef MAX_PATH
#define MAX_PATH    256
#endif //MAX_PATH

///////////////////////////////////////////////////////////////////
namespace detail
{

enum Constant
{
    kMaxPath = 256,
    kMaxFileSize = 10,
    kMaxFileCount = 2
};


#ifndef va_copy
#define va_copy(x,y) x = y
#endif //


#ifdef WIN32

class CriticalSection
{
public:
    CriticalSection()
    {
        InitializeCriticalSection( &m_cs );
    }

    ~CriticalSection()
    {
        DeleteCriticalSection( &m_cs );
    }

    void lock()
    {
        EnterCriticalSection( &m_cs );
    }

    void unlock()
    {
        LeaveCriticalSection( &m_cs );
    }

    CRITICAL_SECTION &getLock()
    {
        return m_cs;
    }

#if(_WIN32_WINNT >= 0x0400)
    bool trylock()
    {
        return ( TryEnterCriticalSection( &m_cs ) != 0 );
    }
#endif /* _WIN32_WINNT >= 0x0400 */

private:
    CRITICAL_SECTION m_cs;
};

#else

/// cross platform critical section
class CriticalSection
{
public:
    CriticalSection()
    {
        pthread_mutexattr_t attr;
        ::pthread_mutexattr_init(&attr);
        ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        ::pthread_mutex_init(&m_mutex, &attr);
        ::pthread_mutexattr_destroy(&attr);
    }

    ~CriticalSection()
    {
        ::pthread_mutex_destroy(&m_mutex);
    }

    void lock()
    {
        ::pthread_mutex_lock(&m_mutex);
    }

    void unlock()
    {
        ::pthread_mutex_unlock(&m_mutex);
    }

    bool trylock()
    {
        return (0 == ::pthread_mutex_trylock(&m_mutex));
    }

    pthread_mutex_t &getLock()
    {
        return m_mutex;
    }

private:
    pthread_mutex_t m_mutex;

};

#endif //WIN32



class AutoLock
{
public:
	explicit AutoLock(CriticalSection& cs):
		m_cs(cs)
    {
        m_cs.lock();
    }

    ~AutoLock()
    {
        m_cs.unlock();
    }

private:
    CriticalSection& m_cs;
};




#ifdef WIN32
/*
* Number of micro-seconds between the beginning of the Windows epoch
* (Jan. 1, 1601) and the Unix epoch (Jan. 1, 1970).
*
* This assumes all Win32 compilers have 64-bit support.
*/
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS) || defined(__WATCOMC__)
  #define DELTA_EPOCH_IN_USEC  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_USEC  11644473600000000ULL
#endif
typedef unsigned __int64 u_int64_t;

static u_int64_t filetime_to_unix_epoch (const FILETIME *ft)
{
    u_int64_t res = (u_int64_t) ft->dwHighDateTime << 32;

    res |= ft->dwLowDateTime;
    res /= 10;                   /* from 100 nano-sec periods to usec */
    res -= DELTA_EPOCH_IN_USEC;  /* from Win epoch to Unix epoch */
    return (res);
}

int gettimeofday (struct timeval *tv, void *tz)
{
    FILETIME  ft;
    u_int64_t tim;

    if (!tv) {
        //errno = EINVAL;
        return (-1);
    }
    GetSystemTimeAsFileTime (&ft);
    tim = filetime_to_unix_epoch (&ft);
    tv->tv_sec  = (long) (tim / 1000000L);
    tv->tv_usec = (long) (tim % 1000000L);
    return (0);
}
#else

int gettimeofday (struct timeval *tv, void *tz)
{
    return ::gettimeofday(tv, (struct timezone*)tz);
}
#endif //win32



int getcurtime(struct tm& t, int& ms)
{
	struct timeval tv;
	void* tz = NULL;
	int rc = gettimeofday(&tv, tz);
	time_t sec = tv.tv_sec;

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
    localtime_s(&t, &sec);
#else
    t = *localtime(&sec);
#endif //

    ms = tv.tv_usec/1000;
    return rc;
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


int sprint(char **buf, size_t size, const char *fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	return vsprint(buf, size, fmt, argList);
}


///////////////////////////////////////////////////////////////////

/// ini file reader and writer
class IniFile
{
public:
    IniFile();
    ~IniFile();

    explicit IniFile(bool ignoreCase);

    bool load( const char* filename );
    bool save( const char* filename );

    size_t getSectionCount();
    bool existSection( const char* name );
    /// return true if insertion happened
    bool insertSection( const char* name );
    void removeSection( const char* name );

    /// remove comment in the section
    void removeComment( const char* sectionName );
    /// remove all comment in the file
    void removeComment();
    /// insert comment before key
    void setComment( const char* sectionName, const char* keyName,
            const char* comment );
    /// return empty if comment does not exist
    std::string getComment( const char* sectionName, const char* keyName );

    size_t getKeyCount( const char* sectionName );
    bool existKey( const char* sectionName, const char* keyName );
    void removeKey( const char* sectionName, const char* keyName );

    std::string getValue( const char* sectionName, const char* keyName,
            const std::string& defaultValue );
    std::string getValue( const char* sectionName, const char* keyName,
            const char* defaultValue = "");

    void setValue( const char* sectionName, const char* keyName,
            const char* value );
    void setValue( const char* sectionName, const char* keyName,
            const std::string& value );

    template < class T >
    bool queryValue(const char* sectionName, const char* keyName, T& value)
    {
        bool found = false;
        iterator it = findIterator( sectionName );
        if ( it != m_sections.end() )
        {
            IniSection& sec = *it;
            IniEntryIterator pos = sec.findIterator( keyName );
            if ( pos != sec.entryArray.end() )
            {
                std::istringstream iss(pos->value);
                iss >> value;
                found = true;
            }
        }
        return found;
    }

    bool queryValue(const char* sectionName, const char* keyName, bool& value)
    {
        std::string str;
        if (!queryValue(sectionName, keyName, str))
        {
            return false;
        }

        value = (str == "true") || (str == "1");
        return true;
    }

    template < class T >
    T getValue( const char* sectionName, const char* keyName, T defaultValue )
    {
        iterator it = findIterator( sectionName );
        if ( it != m_sections.end() )
        {
            IniSection& sec = *it;
            IniEntryIterator pos = sec.findIterator( keyName );
            if ( pos != sec.entryArray.end() )
            {
                std::istringstream iss( pos->value );
                T t(defaultValue);
                iss >> t;
                return t;
            }
        }
        return defaultValue;
    }

    template < class T >
    void setValue( const char* sectionName, const char* keyName,
                T value )
    {
        IniSection& sec = ensure( sectionName );
        std::ostringstream ss;
        ss << value;
        return sec.set( keyName, ss.str() );
    }


    std::string getFileComment();
    void setFileComment( const std::string& comment );

    void clear();



public:
    class IniEntry
    {
    public:
        std::string key;
        std::string value;
        std::string comment;

        IniEntry()
        {
        }

        explicit IniEntry( const std::string& keyName )
        :key(keyName)
        {
        }

        void clear()
        {
            key.clear();
            value.clear();
            comment.clear();
        }
    };

    typedef std::vector< IniEntry >     IniEntryArray;
    typedef IniEntryArray::iterator     IniEntryIterator;

    class IniSection
    {
    public:
        std::string     name;
        IniEntryArray   entryArray;
        bool m_ignoreCase;

        IniSection();
        IniSection( const std::string& name, bool ignoreCase);

        void set( const char* keyName, const std::string& value );
        bool get(const char* keyName, std::string& value);
        bool get(const char* keyName, int& value);
        bool get(const char* keyName, double& value);
        bool exist(const char* keyName);

        IniEntryIterator findIterator( const char* keyName );
        IniEntry& ensure( const char* keyName );
        void remove( const char* keyName );
        void removeComment();
        void clear();
        bool empty();

    };

    typedef std::vector< IniSection >   IniSectionArray;
    typedef IniSectionArray::iterator   iterator;


    IniSection& getSection(size_t idx);

    IniSection& ensure( const char* name );

    class StringPredicator
    {
    public:
        explicit StringPredicator(bool ignoreCase):
            m_ignoreCase(ignoreCase)
        {

        }

        bool equals(const std::string& left, const std::string& right) const
        {
            if (m_ignoreCase)
            {
                if (left.size() != right.size())
                {
                    return false;
                }

                for (size_t i = 0; i < left.size(); ++ i)
                {
                    if (tolower(left[i]) != tolower(right[i]))
                    {
                        return false;
                    }
                }

                return true;
            }
            else
            {
                return (left == right);
            }
        }

        bool    m_ignoreCase;
    };

private:

    iterator    findIterator( const char* sectionName );

    enum LineFlag
    {
        LINE_UNKNOWN,
        LINE_EMPTY,
        LINE_COMMENT,
        LINE_SECTION,
        LINE_ENTRY
    };

    LineFlag parseLine( const std::string& line );
    char getFirstVisibleChar( const std::string& line );
    std::string parseSectionName( const std::string& line );
    void parseEntry( const std::string& line, IniEntry& entry );




private:
    IniSectionArray     m_sections;
    std::string         m_fileComment;
    bool    m_ignoreCase;


};


using namespace std;

IniFile::IniFile():
    m_ignoreCase(false)
{
}

IniFile::~IniFile()
{
}

IniFile::IniFile(bool ignoreCase):
    m_ignoreCase(ignoreCase)
{

}

bool IniFile::load( const char* filename )
{
    bool done = false;
    std::ifstream ifs;
    ifs.open( filename );
    if ( ifs.is_open() )
    {
        clear();

        /// parse header
        LineFlag flag = LINE_UNKNOWN;
        std::string line;
        while ( std::getline( ifs, line ) )
        {
            flag = parseLine( line );
            if ( flag == LINE_COMMENT )
            {
                m_fileComment += line;
                m_fileComment += "\n";
            }
            else
            {
                break;
            }
        }

        IniSection sec;
        sec.m_ignoreCase = m_ignoreCase;

        IniEntry entry;
        std::string comment;

        do
        {
            switch ( flag )
            {
            case LINE_SECTION:
                {
                    if ( !sec.empty() )
                    {
                        m_sections.push_back( sec );
                        sec.clear();
                    }

                    sec.name = parseSectionName( line );
                }
                break;
            case LINE_COMMENT:
                {
                    comment += line;
                    comment += "\n";
                }
                break;
            case LINE_ENTRY:
                {
                    entry.comment = comment;
                    parseEntry( line, entry );

                    sec.entryArray.push_back( entry );
                    comment.clear();
                    entry.clear();
                }
                break;
            case LINE_EMPTY:
                break;
            default:
                {
                    comment += line;
                    comment += "\n";
                }
                break;
            }

            std::getline( ifs, line );
            flag = parseLine( line );
        } while ( ifs );

        if ( !sec.empty() )
        {
            m_sections.push_back( sec );
        }

        ifs.close();
        done = true;
    }
    return done;
}

bool IniFile::save( const char* filename )
{
    bool success = false;
    std::ofstream ofs;
    ofs.open( filename );
    if ( ofs.is_open() )
    {
        ofs << m_fileComment;

        iterator it = m_sections.begin();
        for ( ; it != m_sections.end(); ++ it )
        {
            IniSection& sec = *it;
            ofs << endl << "[" << sec.name << "]" << endl;

            IniEntryIterator pos = sec.entryArray.begin();
            for (; pos != sec.entryArray.end(); ++ pos )
            {
                if ( !pos->comment.empty() )
                {
                    ofs << pos->comment;
                }
                ofs << pos->key << "=" << pos->value << endl;
            }
        }

        ofs.close();
        success = true;
    }
    return success;
}


size_t IniFile::getSectionCount()
{
    return m_sections.size();
}

bool IniFile::existSection( const char* sectionName )
{
    iterator it = findIterator( sectionName );
    return ( it != m_sections.end() );
}


bool IniFile::insertSection( const char* sectionName )
{
    bool done = false;
    iterator it = findIterator( sectionName );
    if ( it == m_sections.end() )
    {
        m_sections.push_back( IniSection( sectionName, m_ignoreCase ) );
        done = true;
    }
    return done;
}

void IniFile::removeSection( const char* sectionName )
{
    iterator it = findIterator( sectionName );
    if ( it != m_sections.end() )
    {
        m_sections.erase( it );
    }
}



void IniFile::removeComment( const char* sectionName )
{
    iterator it = findIterator( sectionName );
    if ( it != m_sections.end() )
    {
        it->removeComment();
    }
}


void IniFile::removeComment()
{
    iterator it = m_sections.begin();
    for ( ; it != m_sections.end(); ++ it )
    {
        it->removeComment();
    }
}


void IniFile::setComment( const char* sectionName, const char* keyName, const char* comment )
{
    IniSection& sec = ensure( sectionName );
    IniEntry& entry = sec.ensure( keyName );
    entry.comment = comment;
}

std::string IniFile::getComment( const char* sectionName, const char* keyName )
{
    iterator it = findIterator( sectionName );
    if ( it != m_sections.end() )
    {
        IniSection& sec = *it;
        IniEntryIterator pos = sec.findIterator( keyName );
        if ( pos != sec.entryArray.end() )
        {
            return pos->comment;
        }
    }
    return std::string();
}


size_t IniFile::getKeyCount( const char* sectionName )
{
    iterator it = findIterator( sectionName );
    if ( it != m_sections.end() )
    {
        return it->entryArray.size();
    }
    return 0;
}

bool IniFile::existKey( const char* sectionName, const char* keyName )
{
    iterator it = findIterator( sectionName );
    if ( it != m_sections.end() )
    {
        IniSection& sec = *it;
        return ( sec.findIterator( keyName ) != sec.entryArray.end() );
    }
    return false;
}

void IniFile::removeKey( const char* sectionName, const char* keyName )
{
    iterator it = findIterator( sectionName );
    if ( it != m_sections.end() )
    {
        IniSection& sec = *it;
        sec.remove( keyName );
    }
}


std::string IniFile::getValue( const char* sectionName, const char* keyName,
        const std::string& defaultValue )
{
    iterator it = findIterator( sectionName );
    if ( it != m_sections.end() )
    {
        IniSection& sec = *it;
        IniEntryIterator pos = sec.findIterator( keyName );
        if ( pos != sec.entryArray.end() )
        {
            return pos->value;
        }
    }
    return defaultValue;
}

std::string IniFile::getValue( const char* sectionName, const char* keyName,
            const char* defaultValue )
{
    return getValue( sectionName, keyName, std::string(defaultValue) );
}

void IniFile::setValue( const char* sectionName, const char* keyName,
        const char* value )
{
    IniSection& sec = ensure( sectionName );
    sec.set( keyName, value );
}

void IniFile::setValue( const char* sectionName, const char* keyName,
        const std::string& value )
{
    IniSection& sec = ensure( sectionName );
    sec.set( keyName, value );
}


std::string IniFile::getFileComment()
{
    return m_fileComment;
}

void IniFile::setFileComment( const std::string& comment )
{
    m_fileComment = comment;
}

void IniFile::clear()
{
    m_fileComment.clear();
    m_sections.clear();
}


IniFile::IniSection& IniFile::getSection(size_t idx)
{
    assert(idx < m_sections.size());
    return m_sections[idx];
}

IniFile::IniSection& IniFile::ensure( const char* name )
{
    iterator it = findIterator( name );
    if ( it == m_sections.end() )
    {
        m_sections.push_back( IniSection(name, m_ignoreCase) );
        return m_sections.back();
    }
    return *it;
}

IniFile::iterator   IniFile::findIterator( const char* sectionName )
{
    StringPredicator pred(m_ignoreCase);

    iterator it = m_sections.begin();
    for ( ; it != m_sections.end(); ++ it )
    {
        if (pred.equals(it->name, sectionName))
        {
            return it;
        }
    }
    return m_sections.end();
}

IniFile::LineFlag IniFile::parseLine( const std::string& line )
{
    if ( line.empty() )
        return LINE_EMPTY;

    char firstChar = getFirstVisibleChar( line );
    if ( firstChar == ';' || firstChar == '#' )
    {
        return LINE_COMMENT;
    }
    else if ( firstChar == '[' )
    {
        return LINE_SECTION;
    }

    size_t idx = line.find_first_of( '=' );
    if ( idx != std::string::npos )
    {
        return LINE_ENTRY;
    }
    return LINE_UNKNOWN;
}

char IniFile::getFirstVisibleChar( const std::string& line )
{
    for ( size_t i = 0; i < line.size(); ++ i )
    {
        if ( line[i] != ' ' && line[i] != '\t' )
        {
            return line[i];
        }
    }
    return 0;
}

std::string IniFile::parseSectionName( const std::string& line )
{
    size_t start = line.find_first_of( '[' );
    size_t stop = line.find_last_of( ']' );
    if ( stop == std::string::npos )
    {
        return line.substr( start );
    }
    return line.substr( start + 1, stop - start-1 );
}

void IniFile::parseEntry( const std::string& line, IniEntry& entry )
{
    size_t idx = line.find_first_of( '=' );
    entry.key = line.substr( 0, idx );
    if (line[line.size() - 1] == '\r')
    {
        entry.value = line.substr( idx + 1, line.size() -1 - (idx+1) );
    }
    else
    {
        entry.value = line.substr( idx + 1 );
    }
}



IniFile::IniSection::IniSection()
:m_ignoreCase(false)
{
}

IniFile::IniSection::IniSection( const std::string& aName, bool ignoreCase)
:name( aName )
,m_ignoreCase(ignoreCase)
{
}

void IniFile::IniSection::set( const char* keyName, const std::string& value )
{
    IniEntry& entry = ensure( keyName );
    entry.value = value;
}

bool IniFile::IniSection::get(const char* keyName, std::string& value)
{
    bool found = false;
    IniEntryIterator it = findIterator(keyName);
    if (it != entryArray.end())
    {
        value = it->value;
        found = true;
    }
    return found;
}

bool IniFile::IniSection::get(const char* keyName, int& value)
{
    std::string strValue;
    if (!get(keyName, strValue))
    {
        return false;
    }

    std::istringstream iss(strValue);
    iss >> value;

    return true;
}

bool IniFile::IniSection::get(const char* keyName, double& value)
{
    std::string strValue;
    if (!get(keyName, strValue))
    {
        return false;
    }

    std::istringstream iss(strValue);
    iss >> value;

    return true;
}

bool IniFile::IniSection::exist(const char* keyName)
{
    return (findIterator(keyName) != entryArray.end());
}


IniFile::IniEntryIterator IniFile::IniSection::findIterator( const char* keyName )
{
    StringPredicator pred(m_ignoreCase);

    IniEntryIterator it = entryArray.begin();
    for ( ; it != entryArray.end(); ++ it )
    {
        if (pred.equals(it->key, keyName))
        {
            return it;
        }
    }
    return entryArray.end();
}

IniFile::IniEntry& IniFile::IniSection::ensure( const char* keyName )
{
    IniEntryIterator it = findIterator( keyName );
    if ( it == entryArray.end() )
    {
        entryArray.push_back( IniEntry(keyName));
        return entryArray.back();
    }
    return *it;
}

void IniFile::IniSection::remove( const char* keyName )
{
    IniEntryIterator it = findIterator( keyName );
    if ( it != entryArray.end() )
    {
        entryArray.erase( it );
    }
}

void IniFile::IniSection::removeComment()
{
    IniEntryIterator pos = entryArray.begin();
    for ( ; pos != entryArray.end(); ++ pos )
    {
        pos->comment.clear();
    }
}

void IniFile::IniSection::clear()
{
    name.clear();
    entryArray.clear();
}

bool IniFile::IniSection::empty()
{
    return name.empty() && entryArray.empty();
}

///////////////////////////////////////////////////////////////////


int rsplit(const std::string& str, const std::string& sp, std::string& head, std::string& tail)
{
    int ch = -1;
    for (size_t i = str.size(); i > 0; i --)
    {
        size_t idx = i - 1;
        size_t pos = sp.find(str[idx]);
        if (pos != std::string::npos)
        {
            head = str.substr(0, idx);
            tail = str.substr(idx+1);
            ch = sp[pos];
            break;
        }
    }
    return ch;
}


#ifdef WIN32
bool existDir(const std::string& dirpath)
{
	return (GetFileAttributes(dirpath.c_str()) != 0xFFFFFFFF);
}

bool createDirectory(const std::string& dirpath)
{
    return (::CreateDirectory(dirpath.c_str(), NULL) != FALSE);
}

bool createIfNotExist(const char* path)
{
    BOOL done = TRUE;
    DWORD attr = ::GetFileAttributes(path);
    if (attr == 0xFFFFFFFF)
    {
        done = CreateDirectory(path, NULL);
    }
    return (done != 0);
}

#else

bool existDir(const std::string& path)
{
    struct stat buf;
    memset(&buf, 0, sizeof(buf));
    int ret = stat(path.c_str(), &buf);
    return (ret == 0) && S_ISDIR(buf.st_mode);
}


bool createDirectory(const std::string& path)
{
    int ret = mkdir(path.c_str(), S_IRWXU);
    return (ret == 0 || errno == EEXIST);
}


bool createIfNotExist(const char* path)
{
    if (strlen(path) == 0)
    {
        return true;
    }
    
    bool done = true;
    struct stat buf;
    memset(&buf, 0, sizeof(buf));
    int ret = stat(path, &buf);
    if ((ret == 0) && S_ISDIR(buf.st_mode))
    {
        done = true;
    }
    else
    {
        ret = mkdir(path, S_IRWXU);
        done = (ret == 0 || errno == EEXIST);
    }
    return done;
}

#endif //

bool createDirectories(const char* path)
{
    size_t length = strlen(path);
    char buffer[MAX_PATH] = {0};
    #if defined(_MSC_VER) && (_MSC_VER >= 1400 )
    strcpy_s(buffer, MAX_PATH, path);
    #else
    strcpy(buffer, path);
    #endif //_MSC_VER

    char sp = '\\';

    for (size_t i = 0; i < length; ++ i)
    {
        if ((buffer[i] == '\\') || (buffer[i] == '/'))
        {
            sp = buffer[i];
            buffer[i] = '\0';
            if (!createIfNotExist(buffer))
            {
                return false;
            }

            buffer[i] = sp;
        }
    }

    if ((path[length - 1] == '\\') || (path[length - 1] == '/'))
    {
        return true;
    }

    return createIfNotExist(buffer);
}


bool ensureParentDir(const std::string& filepath)
{
	std::string sp("/\\");
	std::string dirpath;
	std::string name;

	rsplit(filepath, sp, dirpath, name);
	if (dirpath.empty() || existDir(dirpath))
	{
		return true;
	}
	return createDirectories(dirpath.c_str());
}





void writeCout(int level, const char* data, int length, int flags)
{
	if (flags & CLog::DATETIME)
	{
		printf("%s %s | %s", CLog::getDateTime(), CLog::getName(level), data);
	}
	else if (flags & CLog::TIME)
	{
		printf("%s %s | %s", CLog::getShortTime(), CLog::getName(level), data);
	}
	else
	{
		printf("%s", data);
	}
}

void writeDebug(int level, const char* data, int length, int flags)
{
#ifdef WIN32
	char buffer[1024];
	char* buf = buffer;

	if (flags & CLog::DATETIME)
	{
		detail::sprint(&buf, 1024, "%s %s | %s",
				CLog::getDateTime(), CLog::getName(level), data);
		OutputDebugStringA(buf);
	}
	else if (flags & CLog::TIME)
	{
		detail::sprint(&buf, 1024, "%s %s | %s",
				CLog::getShortTime(), CLog::getName(level), data);
		OutputDebugStringA(buf);
	}
	else
	{
		OutputDebugStringA(data);
	}

	if (buf != buffer)
	{
		free(buf);
	}

#else
    /// pass
#endif //WIN32
}



class LogWriter : public CLog::Writer
{
public:
    WriteLog m_logger;

    explicit LogWriter(WriteLog logger):
        m_logger(logger)
    {
    }

    virtual void write(int level, const char* msg, int length, int flags)
    {
        if (m_logger)
        {
            (m_logger)(level, msg, length, flags);
        }
    }

};


#ifndef WIN32

class SyslogWriter : public CLog::Writer
{
public:
    SyslogWriter()
    {
        openlog(0, LOG_NDELAY | LOG_PID, LOG_LOCAL6);
    }

    ~SyslogWriter()
    {
        closelog();
    }

    struct LevelEntry
    {
        int level;
        int sysLevel;
    };

    static int transform(int level)
    {
        static LevelEntry s_levels[] =
        {
            {CLog::kNone, LOG_DEBUG},
            {CLog::kDebug, LOG_DEBUG},
            {CLog::kInfo, LOG_INFO},
            {CLog::kWarning, LOG_WARNING},
            {CLog::kError, LOG_ERR},
            {CLog::kFatal, LOG_CRIT},
            {CLog::kDebug, LOG_DEBUG},
            {CLog::kDebug, LOG_DEBUG},
        };

        if ((level < 0) || (level >= (int)sizeof(s_levels)))
        {
            return s_levels[0].sysLevel;
        }
        return s_levels[level].sysLevel;
    }

    virtual void write(int level, const char* msg, int length, int flags)
    {
        int sysLevel = transform(level);
        syslog(LOG_LOCAL6 | sysLevel, "%s", msg);
    }

};

#endif //WIN32



#ifdef ANDROID
#include <android/log.h>

struct LevelPriority
{
    int level;
    int prio;
};

static LevelPriority s_levelPriority[] =
{
    { CLog::kDebug, ANDROID_LOG_DEBUG },
    { CLog::kInfo, ANDROID_LOG_INFO },
    { CLog::kWarning, ANDROID_LOG_WARN },
    { CLog::kError, ANDROID_LOG_ERROR },
    { CLog::kFatal, ANDROID_LOG_FATAL }
};

static int getPriority(int level)
{
    size_t count = sizeof(s_levelPriority)/sizeof(s_levelPriority[0]);
    for (size_t i = 0; i < count; ++ i)
    {
        if (level == s_levelPriority[i].level)
        {
            return s_levelPriority[i].prio;
        }
    }
    return ANDROID_LOG_DEFAULT;
}

class AndroidLogWriter: public CLog::Writer
{
public:
    virtual void write(int level, const char* msg, int length, int flags)
    {
        int prio = getPriority(level);
        __android_log_write(prio, "jni", msg);
    }
};

#endif //ANDROID

class FileLogWriter : public CLog::Writer
{
public:
    explicit FileLogWriter(const char* filepath = ""):
		CLog::Writer(),
	    m_filePath(filepath),
		m_maxFileSize(DEFAULT_MAX_SIZE),
		m_rollCount(DEFAULT_ROLL_COUNT),
		m_file(NULL),
		m_curFileSize()
	{
    	ensureFileDir(m_filePath);

    	openFile();
	}

    ~FileLogWriter()
    {
    	closeFile();
    }

    virtual void write(int level, const char* msg, int length, int flags)
    {
    	if (m_file == NULL)
		{
			if (!openFile())
			{
				return;
			}
		}

		size_t written = 0;
		if (flags & CLog::DATETIME)
		{
			written = fprintf(m_file, "%s %s | %s", CLog::getDateTime(), CLog::getName(level), msg);
		}
		else if (flags & CLog::TIME)
		{
			written = fprintf(m_file, "%s %s | %s", CLog::getShortTime(), CLog::getName(level), msg);
		}
		else
		{
			written = fwrite(msg, 1, length, m_file);
		}

		fflush(m_file);

		m_curFileSize += written;
		if (m_curFileSize >= m_maxFileSize)
		{
			// switch
			closeFile();

			doRollOver();
		}
    }

    bool setFile(const char* filepath)
    {
    	if (filepath == NULL)
		{
			return false;
		}

		closeFile();

		m_filePath = filepath;

		ensureFileDir(m_filePath);

		return openFile();
    }

    void setMaxSize(size_t fileSize)
    {
    	if (fileSize < 1)
    	{
    		return;
    	}
   		m_maxFileSize = fileSize;
    }

    void setRollCount(size_t count)
    {
    	if (count < 1)
    	{
    		return;
    	}
   		m_rollCount = count;
    }

    static const size_t DEFAULT_MAX_SIZE = 10 * 1024 * 1024;
    static const size_t DEFAULT_ROLL_COUNT = 2;

protected:
    bool ensureFileDir(const std::string& filepath)
    {
    	return detail::ensureParentDir(filepath);
    }

    bool openFile()
    {
    	if (m_file == NULL)
		{

	#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
			m_file = _fsopen(m_filePath.c_str(), "a+", _SH_DENYNO);
	#else
			m_file = fopen(m_filePath.c_str(), "a+");
	#endif
			if (m_file)
			{
				fseek(m_file, 0, SEEK_END);
				m_curFileSize = ftell(m_file);
			}
		}
		return (m_file != NULL);
    }

    void closeFile()
    {
    	if (m_file != NULL)
		{
			fclose(m_file);
			m_file = NULL;
		}
    }

    bool isOpen()
    {
    	return (m_file != NULL);
    }

    std::string getBackupFile(const char* path, size_t idx)
    {
        std::ostringstream ss;
        ss << path;
        ss << '.';
        ss << idx;
        return ss.str();
    }

    void doRollOver()
    {
    	if (m_rollCount == 0)
		{
			::remove(m_filePath.c_str());
		}
		else
		{
			// remove last file
			size_t idx = m_rollCount;
			std::string lastFile = getBackupFile(m_filePath.c_str(), idx);
			::remove(lastFile.c_str());

			// rename
			for (idx = m_rollCount - 1; idx > 0; -- idx)
			{
				std::string filepath = getBackupFile(m_filePath.c_str(), idx);
				std::string nextFile = getBackupFile(m_filePath.c_str(), idx + 1);
				::rename(filepath.c_str(), nextFile.c_str());
			}

			std::string nextFile = getBackupFile(m_filePath.c_str(), 1);
			::rename(m_filePath.c_str(), nextFile.c_str());
		}
    }

    std::string m_filePath;
    size_t  m_maxFileSize;
    size_t  m_rollCount;

    FILE*   m_file;
    size_t  m_curFileSize;


};






}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////

struct CLogContext
{
    CLog::Writer*   logger[CLog::MAXLOGGER];
    CLog::Level     levels[CLog::MAXLOGGER];
    int             flags[CLog::MAXLOGGER];

    char    fileName[detail::kMaxPath];
    int     fileSize;
    int     fileCount;

    detail::CriticalSection	cs;

    CLogContext():
        fileSize(detail::kMaxFileSize),
        fileCount(detail::kMaxFileCount)
    {
        reset();
    }

    void lock()
    {
    	cs.lock();
    }

    void unlock()
    {
    	cs.unlock();
    }

    void reset()
    {
        fileSize = detail::kMaxFileSize;
        fileCount = detail::kMaxFileCount;

        memset(logger, 0, sizeof(logger));
        memset(levels, 0, sizeof(levels));
        memset(flags, 0, sizeof(flags));
        memset(fileName, 0, sizeof(fileName));

    }

    void setLogger(CLog::LoggerType type, CLog::Writer* logger, CLog::Level level, int flags)
    {
        this->logger[type] = logger;
        this->levels[type] = level;
        this->flags[type] = flags;
    }

    void resetLogger()
    {
        for (int i = 0; i < CLog::MAXLOGGER; i ++)
        {
            if (logger[i])
            {
                delete logger[i];
                logger[i] = NULL;
            }
        }
    }
};

static CLogContext s_context;

class AutoLock
{
public:
	AutoLock()
    {
        s_context.lock();
    }

    ~AutoLock()
    {
        s_context.unlock();
    }
};

///////////////////////////////////////////////////////////////////
struct LogLevelEntry
{
    CLog::Level   level;
    const char* name;
};

static LogLevelEntry s_levelEntry[] = {
    { CLog::kNone, "all" },
    { CLog::kDebug, "debug" },
    { CLog::kInfo, "info" },
    { CLog::kWarning, "warning" },
    { CLog::kError, "error" },
    { CLog::kFatal, "fatal" }
};

CLog::CLog()
{

}

CLog::~CLog()
{
}


int CLog::startup(const char* configPath)
{
    return reload(configPath);
}

void CLog::cleanup()
{
    resetLogger();

    s_context.reset();
}



void CLog::log(int level, const char* format, ...)
{
    va_list argList;
    va_start(argList, format);

    logv(level, format, argList);

    va_end(argList);
}


void CLog::logv(int level, const char* format, va_list args)
{
    char buffer[1024];
    char* buf = buffer;
    int length = detail::vsprint(&buf, 1024, format, args);

    writeLog(level, buf, length);

    if (buf != buffer)
    {
    	free(buf);
    }
}

void CLog::writeLog(int level, const char* data, int length)
{
    detail::AutoLock lock(s_context.cs);

    for (int i = 0; i < MAXLOGGER; i ++)
    {
        if (s_context.logger[i])
        {
        	int flags  = s_context.flags[i];
        	Level outLevel = s_context.levels[i];
        	if (outLevel <= level)
        	{
        	    s_context.logger[i]->write(level, data, length, flags);
        	}
        }
    }
}


void CLog::debug(const char* format, ...)
{
    va_list argList;
    va_start(argList, format);

    logv(kDebug, format, argList);

    va_end(argList);
}


void CLog::info(const char* format, ...)
{
    va_list argList;
    va_start(argList, format);

    logv(kInfo, format, argList);

    va_end(argList);
}


void CLog::warning(const char* format, ...)
{
    va_list argList;
    va_start(argList, format);

    logv(kWarning, format, argList);

    va_end(argList);
}


void CLog::error(const char* format, ...)
{
    va_list argList;
    va_start(argList, format);

    logv(kError, format, argList);

    va_end(argList);
}


void CLog::fatal(const char* format, ...)
{
    va_list argList;
    va_start(argList, format);

    logv(kFatal, format, argList);

    va_end(argList);
}




int CLog::setLogger(LoggerType type, Level level, int flags)
{
    int rc = 0;

    detail::AutoLock lock(s_context.cs);

    if (type == COUT)
    {
#ifdef ANDROID
        s_context.setLogger(type, new detail::AndroidLogWriter(), level, flags);
#else
        s_context.setLogger(type, new detail::LogWriter(detail::writeCout), level, flags);
#endif //
    }
    else if (type == SYSLOG)
    {
#ifndef WIN32
        s_context.setLogger(type, new detail::SyslogWriter(), level, flags);
#endif //WIN32
    }
    else if (type == DEBUGWINDOW)
    {
        s_context.setLogger(type, new detail::LogWriter(detail::writeDebug), level, flags);
    }
    else if (type == FILE)
    {
        s_context.setLogger(type, new detail::FileLogWriter(s_context.fileName), level, flags);
    }
    else
    {
        rc = EINVAL;
    }
    return rc;
}


int CLog::setFileParam(const char* filename, int fileSize, int rollCount)
{
    if ((filename == NULL) || (strlen(filename) == 0))
    {
        filename = "app.log";
    }

    if (fileSize <= 0)
    {
        fileSize = detail::kMaxFileSize;
    }

    if (rollCount <= 0)
    {
        rollCount = detail::kMaxFileCount;
    }

    int rc = 0;

    detail::AutoLock lock(s_context.cs);

    #if defined(_MSC_VER) && (_MSC_VER >= 1400 )
    strcpy_s(s_context.fileName, filename);
    #else
    strcpy(s_context.fileName, filename);
    #endif//
    s_context.fileSize = fileSize;
    s_context.fileCount = rollCount;

    detail::FileLogWriter* writer = (detail::FileLogWriter*)s_context.logger[FILE];
    if (writer)
    {
    	writer->setFile(filename);
    	writer->setMaxSize(fileSize * 1024 * 1024);
    	writer->setRollCount(rollCount);
    }

    return rc;
}


void CLog::setExtLogger(WriteLog logger, Level level)
{
    detail::AutoLock lock(s_context.cs);

    s_context.logger[EXTERNAL] = new detail::LogWriter(logger);
}


void CLog::resetLogger()
{
    detail::AutoLock lock(s_context.cs);

    s_context.resetLogger();
}


static CLog::Level parseLevel(const std::string& strLevel)
{
    size_t count = sizeof(s_levelEntry)/sizeof(s_levelEntry[0]);
    for (size_t i = 0; i < count; i ++)
    {
        if (strLevel == s_levelEntry[i].name)
        {
            return s_levelEntry[i].level;
        }
    }
    return CLog::kNone;
}

static int parseFormat(const std::string& strFormat)
{
	if (strFormat == "datetime")
	{
		return CLog::DATETIME;
	}
	else if (strFormat == "time")
	{
		return CLog::TIME;
	}
	return 0;
}

static int readFormat(detail::IniFile& iniFile, const char* section, const char* key)
{
	int flags = 0;
	std::string strFormat;
	iniFile.queryValue(section, key, strFormat);
	while (!strFormat.empty())
	{
		size_t pos = strFormat.find(',');
		if (pos != std::string::npos)
		{
			flags |= parseFormat(strFormat.substr(0, pos));
			strFormat = strFormat.substr(pos + 1);
		}
		else
		{
			flags |= parseFormat(strFormat);
            break;
		}
	}
	return flags;
}

static CLog::Level readLevel(detail::IniFile& iniFile, const char* section, const char* key)
{
    std::string strLevel;
    iniFile.queryValue(section, key, strLevel);
    CLog::Level level = parseLevel(strLevel);
    return level;
}

int CLog::reload(const char* configPath)
{
    if ((configPath == NULL) || strlen(configPath) == 0)
    {
        return 0;
    }

    detail::IniFile iniFile;
    if (!iniFile.load(configPath))
    {
        return ENOENT;
    }

    resetLogger();

    Level level = kNone;

    bool enabled = false;
    int flags = 0;
    iniFile.queryValue("Console", "Enabled", enabled);
    level = readLevel(iniFile, "Console", "Level");
    flags = readFormat(iniFile, "Console", "Format");
    if (enabled)
    {
        setLogger(CLog::COUT, level, flags);
    }

    iniFile.queryValue("Debug", "Enabled", enabled);
    level = readLevel(iniFile, "Debug", "Level");
    flags = readFormat(iniFile, "Debug", "Format");
    if (enabled)
    {
        setLogger(CLog::DEBUGWINDOW, level, flags);
    }

    std::string filename = s_context.fileName;
    int fileSize = s_context.fileSize;
    int fileCount = s_context.fileCount;
    iniFile.queryValue("File", "FileName", filename);
    iniFile.queryValue("File", "MaxSize", fileSize);
    iniFile.queryValue("File", "RollCount", fileCount);

    setFileParam(filename.c_str(), fileSize, fileCount);
    
    iniFile.queryValue("File", "Enabled", enabled);
    level = readLevel(iniFile, "File", "Level");
    flags = readFormat(iniFile, "File", "Format");
    if (enabled)
    {
        setLogger(CLog::FILE, level, flags);
    }

    
    iniFile.queryValue("Syslog", "Enabled", enabled);
    level = readLevel(iniFile, "Syslog", "Level");
    flags = readFormat(iniFile, "Syslog", "Format");
    if (enabled)
    {
        setLogger(CLog::SYSLOG, level, flags);
    }

    return 0;
}


const char* CLog::getDateTime()
{
	static char time_buffer[128];

	struct tm t;
	int ms = 0;
	detail::getcurtime(t, ms);

	char* buf = time_buffer;
	detail::sprint(&buf, 128, "%04d-%02d-%02d %02d:%02d:%02d.%d",
			(t.tm_year + 1900), (t.tm_mon + 1), t.tm_mday,
			t.tm_hour, t.tm_min, t.tm_sec, ms);

    return time_buffer;
}

const char* CLog::getShortTime()
{
	static char short_time_buffer[128];

	struct tm t;
	int ms = 0;
	detail::getcurtime(t, ms);

	char* buf = short_time_buffer;
	detail::sprint(&buf, 128, "%02d:%02d:%02d.%d",
			t.tm_hour, t.tm_min, t.tm_sec, ms);

    return short_time_buffer;
}

int CLog::getCurTime(int* ms)
{
    struct timeval tv;
    detail::gettimeofday(&tv, NULL);
    if (ms)
    {
        *ms = tv.tv_usec / 1000;
    }
    return tv.tv_sec;
}


const char* CLog::getName(int level)
{
	if (level < 0)
	{
		level = 0;
	}
	else if (level > kFatal)
	{
		level = kFatal;
	}
	return s_levelEntry[level].name;
}

