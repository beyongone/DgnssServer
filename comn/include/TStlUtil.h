/*
 * TStlUtil.h
 *
 *  Created on: 2010-8-30
 *      Author: Administrator
 */

#ifndef TSTLUTIL_H_
#define TSTLUTIL_H_


#include <algorithm>
////////////////////////////////////////////////////////////////////////////
namespace comn
{

class StlUtil
{
public:
    /// get iterator from index
    template < typename Cont >
    static typename Cont::iterator getIterator(Cont& cont, size_t index)
    {
        typename Cont::iterator pos = cont.begin();
        std::advance(pos, index);
        return pos;
    }

    /// get cont_iterator from index
    template < typename Cont >
    static typename Cont::const_iterator getIterator(const Cont& cont, size_t index)
    {
        typename Cont::const_iterator pos = cont.begin();
        std::advance(pos, index);
        return pos;
    }

    /// get index from iterator
    template < typename Cont >
    static size_t getIndex(Cont& cont, typename Cont::iterator pos)
    {
        size_t index = -1;
        if (pos != cont.end())
        {
            index = std::distance(pos, cont.end());
        }
        return index;
    }

    /// get index from cont_iterator
    template < typename Cont >
    static size_t getIndex(const Cont& cont, typename Cont::const_iterator pos)
    {
        size_t index = -1;
        if (pos != cont.end())
        {
            index = std::distance(pos, cont.end());
        }
        return index;
    }


    /**
     * check whether value exist
     * @return true if value exists
     */
    template < typename Cont, typename Value >
    static bool exists(const Cont& cont, const Value& value)
    {
        return (std::find(cont.begin(), cont.end(), value) != cont.end());
    }

    /**
     * remove element with specified index
     * @return true if element was found and removed
     */
    template<typename Cont>
    static bool removeAt(Cont& cont, size_t index)
    {
        bool done = false;
        typename Cont::iterator it = getIterator(cont, index);
        if (it != cont.end())
        {
            cont.erase(it);
            done = true;
        }
        return done;
    }


    /**
     * delete element with specified index
     * @return true if element was found and deleted
     */
    template < typename Cont >
    static bool deleteAt(Cont& cont, size_t index)
    {
        bool done = false;
        typename Cont::iterator it = getIterator(cont, index);
        if (it != cont.end())
        {
            delete (*it);
            cont.erase(it);
            done = true;
        }
        return done;
    }


    /// delete all elements of list,vector,deque
    template<typename SequenceCont>
    static void deleteAllElement(SequenceCont& seq)
    {
        typename SequenceCont::iterator pos = seq.begin();
        for (; pos != seq.end(); ++pos)
        {
            if ((*pos) != NULL)
            {
                delete (*pos);
                (*pos) = NULL;
            }
        }
        seq.clear();
    }


    /**
     * check whether key exist in map
     */
    template < typename MapCont >
    static bool existKey(const MapCont& cont, const typename MapCont::key_type& key)
    {
        return (cont.find(key) != cont.end());
    }

    /**
     * check whether value exist in map
     */
    template < typename MapCont >
    static bool existValue(const MapCont& cont, const typename MapCont::value_type& value)
    {
        typename MapCont::const_iterator it = cont.begin();
        for (; it != cont.end(); ++ it)
        {
            if (it->second == value)
            {
                return true;
            }
        }
        return false;
    }

    /**
     * find the value in map
     */
    template < typename MapCont >
    static typename MapCont::iterator findByValue(MapCont& cont, const typename MapCont::mapped_type& value)
    {
        typename MapCont::iterator it = cont.begin();
        for (; it != cont.end(); ++ it)
        {
            if (it->second == value)
            {
                return it;
            }
        }
        return cont.end();
    }

    template < typename MapCont >
    static typename MapCont::mapped_type getValue(MapCont& cont,
            const typename MapCont::key_type& key,
            const typename MapCont::mapped_type& value)
    {
        typename MapCont::const_iterator it = cont.find(key);
        if (it != cont.end())
        {
            return it->second;
        }
        return value;
    }

    template < typename MapCont >
    static typename MapCont::mapped_type getValue(MapCont& cont,
            const typename MapCont::key_type& key)
    {
        typename MapCont::const_iterator it = cont.find(key);
        if (it != cont.end())
        {
            return it->second;
        }

        typedef typename MapCont::mapped_type   mapped_type;
        return mapped_type();
    }




};















}

////////////////////////////////////////////////////////////////////////////
#endif /* TSTLUTIL_H_ */
