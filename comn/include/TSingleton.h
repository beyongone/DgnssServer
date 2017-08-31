/*
 * TSingleton.h
 *
 *  Created on: 2009-1-19
 *      Author: terry
 */

#ifndef TSINGLETON_H_
#define TSINGLETON_H_

namespace comn
{

template<class T>
class Singleton: private T
{
private:
    Singleton();
    ~Singleton();

public:
    static T &instance();
};

template<class T>
inline Singleton<T>::Singleton()
{
    /* no-op */
}

template<class T>
inline Singleton<T>::~Singleton()
{
    /* no-op */
}

template<class T>
/*static*/T &Singleton<T>::instance()
{
    // function-local static to force this to work correctly at static
    // initialization time.
    static Singleton<T> s_oT;
    return (s_oT);
}

}
#endif /* TSINGLETON_H_ */
