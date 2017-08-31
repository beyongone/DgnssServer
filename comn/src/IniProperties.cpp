/*
 * IniProperties.cpp
 *
 *  Created on: 2015年12月22日
 *      Author: terry
 */

#include "IniProperties.h"
#include "IniFile.h"


namespace comn
{

IniProperties::IniProperties():
		m_iniFile(new IniFile())
{
}

IniProperties::~IniProperties()
{
}

bool IniProperties::load(const char* filename)
{
	m_filename = filename;
	return m_iniFile->load(filename);
}

bool IniProperties::store(const char* filename)
{
	if (filename == NULL)
	{
		filename = m_filename.c_str();
	}
	return m_iniFile->save(filename);
}

std::string IniProperties::getFilename()
{
	return m_filename;
}

bool IniProperties::get(const std::string& path, std::string& value)
{
    std::string sec;
    std::string key;
    split(path, sec, key);
    return m_iniFile->queryValue(sec.c_str(), key.c_str(), value);
}

bool IniProperties::set(const std::string& path, const std::string& value)
{
    std::string sec;
    std::string key;
    split(path, sec, key);

    m_iniFile->setValue(sec.c_str(), key.c_str(), value);
    return true;
}

bool IniProperties::split(const std::string& path, std::string& sec, std::string& name)
{
	if (path.empty())
	{
		return false;
	}

	sec = "App";
	name = path;

	size_t pos = path.find('.');
	if (pos != std::string::npos)
	{
		sec = path.substr(0, pos);
		name = path.substr(pos + 1);
	}
	return true;
}



} /* namespace util */
