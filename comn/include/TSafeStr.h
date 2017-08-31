/*    file: TSafeStr.h
 *    desc:
 *   
 * created: 2013-08-30 10:37:32
 *  author: zhengchuanjiang
 * company: 
 */ 


#if !defined TSAFESTR_ZHENGCHUANJIANG_
#define TSAFESTR_ZHENGCHUANJIANG_

#include <string>
#include <string.h>
#include <errno.h>
////////////////////////////////////////////////////////////////////////////
namespace comn
{


inline int copyStr(char* strDest, size_t length, const char* strSource, size_t count)
{
    int err = 0;
    size_t size = count;
    if (length <= count)
    {
        size = (length-1);
        err = ERANGE;
    }

#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
    err = strncpy_s(strDest, length, strSource, size);
#else
    strncpy(strDest, strSource, size);
#endif //
    return err;
}


inline int copyStr(char* strDest, size_t length, const char* strSource)
{
	return copyStr(strDest, length, strSource, strlen(strSource));
}

inline int copyStr(char* strDest, size_t length, const std::string& src)
{
    return copyStr(strDest, length, src.c_str(), src.length());
}


template < class T >
int copyStr(T& t, const char* strSource)
{
    return copyStr(t, sizeof(t), strSource);
}

template < class T >
int copyStr(T& t, const char* strSource, size_t count)
{
    return copyStr(t, sizeof(t), strSource, count);
}

template < class T >
int copyStr(T& t, const std::string& strSource)
{
    return copyStr(t, sizeof(t), strSource.c_str(), strSource.length());
}



}
////////////////////////////////////////////////////////////////////////////
#endif //TSAFESTR_ZHENGCHUANJIANG_

