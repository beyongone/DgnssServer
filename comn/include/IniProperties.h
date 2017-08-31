/*
 * IniProperties.h
 *
 *  Created on: 2015年12月22日
 *      Author: terry
 */

#ifndef SRC_INIPROPERTIES_H_
#define SRC_INIPROPERTIES_H_

#include "BasicType.h"
#include <stddef.h>
#include <string>
#include <sstream>
#include <cstring>


namespace comn
{

class IniFile;

class IniProperties
{
public:
	IniProperties();
	virtual ~IniProperties();

	virtual bool load(const char* filename);

	virtual bool store(const char* filename);

	virtual std::string getFilename();

	virtual bool get(const std::string& key, std::string& value);

	virtual bool set(const std::string& key, const std::string& value);



	template < class T >
	bool getValue(const std::string& key, T& t)
	{
		std::string value;
		if (!get(key, value))
		{
			return false;
		}

		std::istringstream iss(value);
		iss >> t;
		return true;
	}

	template < bool >
	bool getValue(const std::string& key, bool& t)
	{
		std::string value;
		if (!get(key, value))
		{
			return false;
		}

		t = true;
		if (value.empty())
		{
			t = false;
		}
		else if (value == "false")
		{
			t = false;
		}
		else if (value == "0")
		{
			t = false;
		}
		return true;
	}


	template < class T >
	bool setValue(const std::string& key, const T& t)
	{
		std::ostringstream oss;
		oss << t;
		return set(key, oss.str());
	}

	virtual int getInt(const std::string& key, int defValue=0)
	{
		int value = defValue;
		getValue(key, value);
		return value;
	}

	virtual bool getBool(const std::string& key, bool defValue=false)
	{
		bool value = defValue;
		getValue(key, value);
		return value;
	}

	virtual double getDouble(const std::string& key, double defValue=0)
	{
		double value = defValue;
		getValue(key, value);
		return value;
	}

	virtual int64_t getInt64(const std::string& key, int64_t defValue=0)
	{
		int64_t value = defValue;
		getValue(key, value);
		return value;
	}

	virtual std::string getString(const std::string& key, const std::string& defValue=std::string())
	{
		std::string value(defValue);
		get(key, value);
		return value;
	}

	virtual bool setInt(const std::string& key, int value)
	{
		return setValue(key, value);
	}

	virtual bool setBool(const std::string& key, bool value)
	{
		return setValue(key, value);
	}

	virtual bool setDouble(const std::string& key, double value)
	{
		return setValue(key, value);
	}

	virtual bool setInt64(const std::string& key, int64_t value)
	{
		return setValue(key, value);
	}


	bool split(const std::string& path, std::string& sec, std::string& name);

protected:
	std::string	m_filename;
	comn::IniFile*	m_iniFile;

};


} /* namespace util */

#endif /* SRC_INIPROPERTIES_H_ */
