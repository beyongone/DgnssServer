/*
 * BasicType.h
 *
 *  Created on: 2015-5-26
 *      Author: zhengchuanjiang
 */

#ifndef BASICTYPE_H_
#define BASICTYPE_H_


///////////////////////////////////////////////////////////////////
#ifdef WIN32

    #ifndef NOMINMAX
    #define NOMINMAX
    #endif //NOMINMAX

    //#pragma warning( disable : 4786 )
    #include <winsock2.h>
    //#include <windows.h>

#else


#endif //WIN32


///////////////////////////////////////////////////////////////////
#include <stddef.h>
#include <string.h>
///////////////////////////////////////////////////////////////////

#ifndef SOCKET_T
    #ifdef WIN32
        typedef SOCKET  socket_t;
    #else
        typedef int     socket_t;
    #endif // WIN32

    #define SOCKET_T

#endif //SOCKET_T



#ifdef _MSC_VER
    typedef signed char     int8_t;
    typedef unsigned char   uint8_t;
    typedef short           int16_t;
    typedef unsigned short  uint16_t;
    typedef int             int32_t;
    typedef unsigned        uint32_t;
    typedef long long       int64_t;
    typedef unsigned long long   uint64_t;
#else
    #include <stdint.h>
#endif //_MSC_VER


///////////////////////////////////////////////////////////////////
#ifdef WIN32
    #ifndef DLLEXPORT
    #define DLLEXPORT __declspec(dllexport)
    #endif //DLLEXPORT
#else
    #define DLLEXPORT __attribute__ ((visibility ("default"))) 
#endif //WIN32

///////////////////////////////////////////////////////////////////

#endif /* BASICTYPE_H_ */
