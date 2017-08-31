/*
 * TMapUtil.h
 *
 *  Created on: 2013Äê8ÔÂ16ÈÕ
 *      Author: zhengchuanjiang
 */

#ifndef TMAPUTIL_H_
#define TMAPUTIL_H_

#include <map>


namespace comn
{


namespace MapUtil
{

    /**
     * check whether key exist in map
     */
    template < class Map >
    bool existKey(const Map& theMap, const typename Map::key_type& key)
    {
        return (theMap.find(key) != theMap.end());
    }

    /**
     * check whether value exist in map
     */
    template < class Map >
    bool existValue(const Map& theMap, const typename Map::value_type& value)
    {
        typename Map::const_iterator it = theMap.begin();
        for (; it != theMap.end(); ++ it)
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
    template < class Map >
    typename Map::iterator findIterator(Map& theMap, const typename Map::mapped_type& value)
    {
        typename Map::iterator it = theMap.begin();
        for (; it != theMap.end(); ++ it)
        {
            if (it->second == value)
            {
                return it;
            }
        }
        return theMap.end();
    }

    template < class Map >
    typename Map::const_iterator findIterator(const Map& theMap, const typename Map::mapped_type& value)
    {
        typename Map::const_iterator it = theMap.begin();
        for (; it != theMap.end(); ++ it)
        {
            if (it->second == value)
            {
                return it;
            }
        }
        return theMap.end();
    }

    template < class Map >
    bool find(const Map& theMap, const typename Map::key_type& key,
            typename Map::mapped_type& value)
    {
        bool found = false;
        typename Map::const_iterator it = theMap.find(key);
        if (it != theMap.end())
        {
            value = it->second;
            found = true;
        }
        return found;
    }


    template < class Map >
    typename Map::mapped_type getValue(const Map& theMap,
            const typename Map::key_type& key,
            const typename Map::mapped_type& value)
    {
        typename Map::const_iterator it = theMap.find(key);
        if (it != theMap.end())
        {
            return it->second;
        }
        return value;
    }

    template < class Map >
    typename Map::mapped_type getValue(Map& theMap,
            const typename Map::key_type& key)
    {
        typename Map::const_iterator it = theMap.find(key);
        if (it != theMap.end())
        {
            return it->second;
        }

        typedef typename Map::mapped_type   mapped_type;
        return mapped_type();
    }


    template < class Map, class Pred >
    typename Map::mapped_type findIf(Map& theMap, const Pred& pred)
    {
        typename Map::const_iterator it = theMap.begin();
        for (; it != theMap.end(); ++ it)
        {
            if (pred(it->second))
            {
                return it->second;
            }
        }
        return Map::mapped_type();
    }

    template < class Map, class Pred, class Seq >
    size_t findIf(Map& theMap, const Pred& pred, Seq& seq)
    {
        size_t count = 0;
        typename Map::const_iterator it = theMap.begin();
        for (; it != theMap.end(); ++ it)
        {
            if (pred(it->second))
            {
                seq.push_back(it->second);
                count ++;
            }
        }
        return count;
    }

    template < class Map >
    bool remove(Map& theMap, const typename Map::key_type& key,
        typename Map::mapped_type& value)
    {
        bool found = false;
        typename Map::iterator it = theMap.find(key);
        if (it != theMap.end())
        {
            value = it->second;
            theMap.erase(it);
            found = true;
        }
        return found;
    }


    template < class Map, class Pred >
    typename Map::mapped_type removeIf(Map& theMap, const Pred& pred)
    {
        typename Map::iterator it = theMap.begin();
        for (; it != theMap.end(); ++ it)
        {
            if (pred(it->second))
            {
                Map::mapped_type ret = it->second;
                theMap.erase(it);
                return ret;
            }
        }
        return Map::mapped_type();
    }


    template < class Map, class Pred, class Seq >
    size_t removeIf(Map& theMap, const Pred& pred, Seq& seq)
    {
        size_t count = 0;
        typename Map::iterator it = theMap.begin();
        while(it != theMap.end())
        {
            if (pred(it->second))
            {
                seq.push_back(it->second);
                theMap.erase(it ++);
                count ++;
            }
            else
            {
                ++ it;
            }
        }
        return count;
    }



}


}


#endif /* TMAPUTIL_H_ */
