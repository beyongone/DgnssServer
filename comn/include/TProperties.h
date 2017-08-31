/*
 * Properties.h
 *
 *  Created on: 2015年1月15日
 *      Author: zhengchuanjiang
 */

#ifndef TPROPERTIES_H_
#define TPROPERTIES_H_

#include "BasicType.h"
#include "TStringCast.h"
#include <map>


namespace comn
{

class Properties
{
public:
    typedef std::map< std::string, std::string >    StringMap;

public:
	Properties()
	{
	}

	virtual ~Properties()
	{
	}

	void setString(const std::string& name, const std::string& value)
	{
		m_map[name] = value;
	}

	bool getString(const std::string& name, std::string& value) const
	{
		bool found = false;
		StringMap::const_iterator it =  m_map.find(name);
		if (it != m_map.end())
		{
			value = it->second;
			found = true;
		}
		return found;
	}

	size_t size() const
	{
		return m_map.size();
	}

	bool empty() const
	{
		return m_map.empty();
	}

	void clear()
	{
		m_map.clear();
	}

	bool getAt(size_t idx, std::string& name, std::string& value) const
	{
		bool found = false;
		StringMap::const_iterator it = m_map.begin();
		std::advance(it, idx);
		if (it != m_map.end())
		{
			name = it->first;
			value = it->second;
			found = true;
		}
		return found;
	}

	bool remove(const std::string& name)
	{
		size_t count = m_map.erase(name);
		return count > 0;
	}

	bool contains(const std::string& name) const
	{
		return (m_map.find(name) != m_map.end());
	}


	template < class T >
	void set(const std::string& name, T value)
	{
		std::string str = StringCast::toString(value);
		setString(name, str);
	}

	template < class T >
	bool get(const std::string& name, T& value)
	{
		std::string str;
		if (!getString(name, str))
		{
			return false;
		}

		StringCast::toValue(str, value);
		return true;
	}

	std::string toString(char eq, char sp) const
	{
	    std::string str;
	    int idx = 0;
	    StringMap::const_iterator it = m_map.begin();
	    for (; it != m_map.end(); ++ it)
	    {
	        if (idx != 0)
            {
                str += sp;
            }

	        str += it->first;
	        str += eq;
	        str += it->second;

	        idx ++;
	    }
	    return str;
	}

	/**
	 * parse from string
	 * @param str
	 * @param length
	 * @param eq
	 * @param sp
	 * @return property item count
	 */
	size_t parse(const char* str, size_t length, char eq, char sp)
	{
	    size_t count = 0;
	    m_map.clear();

	    size_t begin = 0;
	    for (size_t i = 0; i < length; i ++)
	    {
	        if (str[i] == sp)
	        {
	            count += parseItem(str+begin, str+i, eq);
	            begin = i + 1;
	        }
	    }

	    if (begin < length)
	    {
	        count += parseItem(str+begin, str+length, eq);
	    }
	    return count;
	}

	size_t parseItem(const char* str, const char* strEnd, char eq)
	{
	    bool found = false;
	    size_t length = strEnd - str;
	    for (size_t i = 0; i < length; i ++)
	    {
	        if (str[i] == eq)
	        {
	            std::string name(str, i);
	            std::string value(str+i+1, strEnd - (str+i+1));
	            m_map[name] = value;
	            found = true;
	            break;
	        }
	    }
	    return (size_t)found;
	}

protected:
	StringMap	m_map;


};




} /* namespace comn */

#endif /* TPROPERTIES_H_ */
