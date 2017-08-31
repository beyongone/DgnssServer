/*
 * TMap.h
 *
 *  Created on: 2015年7月23日
 *      Author: chuanjiang.zh@qq.com
 */

#ifndef TEST_TMAP_H_
#define TEST_TMAP_H_

#include "TCriticalSection.h"
#include <map>
#include <functional>

namespace comn
{


/**
 * 线程安全Map
 */
template < typename _Key, typename _Tp, typename _Compare = std::less<_Key>,
            typename _Alloc = std::allocator<std::pair<const _Key, _Tp> > >
class Map
{
public:
    typedef typename std::map< _Key, _Tp, _Compare, _Alloc >    BasicMap;

    typedef typename BasicMap::key_type     key_type;
    typedef typename BasicMap::mapped_type  mapped_type;
    typedef typename BasicMap::value_type   value_type;

    typedef typename BasicMap::iterator iterator;

protected:
    comn::CriticalSection   m_cs;
    BasicMap    m_map;


public:
    Map()
    {

    }

    ~Map()
    {

    }

    bool empty()
    {
        comn::AutoCritSec lock(m_cs);
        return m_map.empty();
    }

    size_t size()
    {
        comn::AutoCritSec lock(m_cs);
        return m_map.size();
    }

    bool exist(const key_type& key)
    {
        comn::AutoCritSec lock(m_cs);
        iterator it = m_map.find(key);
        return (it != m_map.end());
    }

    mapped_type& get(const key_type& key)
    {
        comn::AutoCritSec lock(m_cs);
        return m_map[key];
    }

    bool find(const key_type& key, mapped_type& value)
    {
        comn::AutoCritSec lock(m_cs);
        bool found = false;
        iterator it = m_map.find(key);
        if (it != m_map.end())
        {
            value = it->second;
            found = true;
        }
        return found;
    }

    bool findByValue(const mapped_type& value, key_type& key)
    {
        comn::AutoCritSec lock(m_cs);
        bool found = false;
        iterator it = m_map.begin();
        for (; it != m_map.end(); ++ it)
        {
            if (value == it->second)
            {
                key = it->first;
                found = true;
                break;
            }
        }
        return found;
    }

    bool existValue(const mapped_type& value)
    {
        comn::AutoCritSec lock(m_cs);
        bool found = false;
        iterator it = m_map.begin();
        for (; it != m_map.end(); ++ it)
        {
            if (value == it->second)
            {
                found = true;
                break;
            }
        }
        return found;
    }


    /**
     * 不存在就添加到映射表中
     * @param key
     * @param value
     * @return 是否已经插入
     */
    bool insert(const key_type& key, const mapped_type& value)
    {
        comn::AutoCritSec lock(m_cs);
        std::pair<iterator, bool> result = m_map.insert(BasicMap::value_type(key, value));
        return result.second;
    }

    /**
     * 添加到映射表中, 如果key已经存在,对应的value会被替换
     * @param key
     * @param value
     */
    void set(const key_type& key, const mapped_type& value)
    {
        comn::AutoCritSec lock(m_cs);
        m_map[key] = value;
    }

    /**
     * 添加到映射表中, 并返回原来的值
     * @param key
     * @param value
     * @return
     */
    mapped_type put(const key_type& key, const mapped_type& value)
    {
        comn::AutoCritSec lock(m_cs);
        mapped_type ret(value);
        std::pair<iterator, bool> result = m_map.insert(typename BasicMap::value_type(key, value));
        if (result.second)
        {
            // pass
        }
        else
        {
            ret = result.first->second;
            result.first->second = value;
        }
        return ret;
    }

    bool remove(const key_type& key)
    {
        comn::AutoCritSec lock(m_cs);
        bool found = false;
        iterator it = m_map.find(key);
        if (it != m_map.end())
        {
            m_map.erase(it);
            found = true;
        }
        return found;
    }

    bool remove(const key_type& key, mapped_type& value)
    {
        comn::AutoCritSec lock(m_cs);
        bool found = false;
        iterator it = m_map.find(key);
        if (it != m_map.end())
        {
            value = it->second;
            m_map.erase(it);

            found = true;
        }
        return found;
    }

    bool removeValue(const mapped_type& value, key_type& key)
	{
		comn::AutoCritSec lock(m_cs);
		bool found = false;
		iterator it = m_map.begin();
		for (; it != m_map.end(); ++ it)
		{
			if (value == it->second)
			{
				key = it->first;
				m_map.erase(it);
				found = true;
				break;
			}
		}
		return found;
	}

    /**
     * 移除头部元素
     * @param key
     * @param value
     * @return true 表示已移除并返回对应的key-value
     */
    bool pop_front(key_type& key, mapped_type& value)
    {
        comn::AutoCritSec lock(m_cs);
        bool found = false;
        iterator it = m_map.begin();
        if (it != m_map.end())
        {
            key = it->first;
            value = it->second;

            m_map.erase(it);
            found = true;
        }
        return found;
    }


    void clear()
    {
        comn::AutoCritSec lock(m_cs);
        m_map.clear();
    }

    bool getAt(size_t idx, key_type& key, mapped_type& value)
    {
    	bool found = false;
    	comn::AutoCritSec lock(m_cs);
    	if (idx < m_map.size())
    	{
    		iterator it = m_map.begin();
    		std::advance(it, idx);
    		if (it != m_map.end())
    		{
    			key = it->first;
    			value = it->second;
    			found = true;
    		}
    	}
    	return found;
    }

    template < class Functor >
    bool findIf(Functor& func, key_type& key, mapped_type& value)
    {
        comn::AutoCritSec lock(m_cs);
        bool found = false;
        iterator it = m_map.begin();
        for (; it != m_map.end(); ++ it)
        {
            if (func(it->first, it->second))
            {
                key = it->first;
                value = it->second;
                found = true;
                break;
            }
        }
        return found;
    }

    template < class Functor >
    bool removeIf(Functor& func, key_type& key, mapped_type& value)
    {
        comn::AutoCritSec lock(m_cs);
        bool found = false;
        iterator it = m_map.begin();
        for (; it != m_map.end(); ++ it)
        {
            if (func(it->first, it->second))
            {
                key = it->first;
                value = it->second;
                m_map.erase(it);

                found = true;
                break;
            }
        }
        return found;
    }

    template < class Functor >
    void forEach(Functor& func)
    {
        comn::AutoCritSec lock(m_cs);
        iterator it = m_map.begin();
        for (; it != m_map.end(); ++ it)
        {
            func(it->first, it->second);
        }
    }

    template < class Functor >
    size_t removeAll(Functor& func)
    {
        comn::AutoCritSec lock(m_cs);
        size_t count = 0;
        iterator it = m_map.begin();
        while (it != m_map.end())
        {
            if (func(it->first, it->second))
            {
                count ++;
                m_map.erase(it ++);
            }
            else
            {
                it ++;
            }
        }
        return count;
    }



};






}

#endif /* TEST_TMAP_H_ */
