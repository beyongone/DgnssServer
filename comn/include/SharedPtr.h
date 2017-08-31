/*
 * SharedPtr.h
 *
 *  Created on: 2015-5-7
 *      Author: terry
 */

#ifndef SHAREDPTR_H_
#define SHAREDPTR_H_


///////////////////////////////////////////////////////////////////
// MSVC
#if defined(_MSC_VER) && (_MSC_VER >= 1500)
    #include <memory>
    namespace std
    {
        using std::tr1::shared_ptr;
        using std::tr1::enable_shared_from_this;
    }
    #define FOUND_SHARED_PTR

#endif //_MSC_VER



// g++. test c++0x
// use compiler options: -std=gnu++0x
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    #include <memory>
    //namespace std
    //{
        //using std::shared_ptr;
        //using std::enable_shared_from_this;
    //}
    #define FOUND_SHARED_PTR
#else
     // test tr1
    #if defined(__GNUC__) && (__GNUC__ >= 4) && (__GNUC_MINOR__ >= 1)
    #include <tr1/memory>
    namespace std
    {
        using std::tr1::shared_ptr;
        using std::tr1::enable_shared_from_this;
    }
    #define FOUND_SHARED_PTR
    #endif //
#endif //__GXX_EXPERIMENTAL_CXX0X__
///////////////////////////////////////////////////////////////////

// boost
#ifndef FOUND_SHARED_PTR
#include <boost/shared_ptr.hpp>

namespace std
{
    using boost::shared_ptr;
    using boost::enable_shared_from_this;
}

#endif //FOUND_SHARED_PTR

///////////////////////////////////////////////////////////////////

#endif /* SHAREDPTR_H_ */
