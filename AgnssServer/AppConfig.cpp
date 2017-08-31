/*
 * AppConfig.cpp
 *
 *  Created on: 2016年9月20日
 *      Author: zhengboyuan
 */

#include "AppConfig.h"
#include "IniFile.h"
#include "TSingleton.h"
#include "TStringUtil.h"

using namespace comn;


AppConfig& AppConfig::instance()
{
    return comn::Singleton< AppConfig >::instance();
}



AppConfig::AppConfig():
        m_iniFile(new IniFile())
{
}

AppConfig::~AppConfig()
{
}

bool AppConfig::load(const std::string& filename)
{
    if (!m_iniFile->load(filename.c_str()))
    {
        return false;
    }

    m_filename = filename;

    return true;
}

bool AppConfig::save(const std::string& filename)
{
    return m_iniFile->save(filename.c_str());
}


bool AppConfig::get(const std::string& path, std::string& value)
{
    std::string sec("App");
    std::string key(path);

    size_t pos = path.find('.');
    if (pos != std::string::npos)
    {
        sec = path.substr(0, pos);
        key = path.substr(pos + 1);
    }
    return m_iniFile->queryValue(sec.c_str(), key.c_str(), value);
}

std::string AppConfig::get(const std::string& path, const std::string& defValue)
{
    std::string value(defValue);
    get(path, value);
    return value;
}

std::string AppConfig::get(const char* path, const char* defValue)
{
    std::string value(defValue);
    get(path, value);
    return value;
}

int AppConfig::getInt(const std::string& path, int defValue)
{
    int ret = defValue;
    std::string str;
    if (get(path, str))
    {
        comn::StringCast::toValue(str, ret);
    }
    return ret;
}

bool AppConfig::getBool(const std::string& path, bool defValue)
{
    bool ret = defValue;
    std::string str;
    if (get(path, str))
    {
        if (str.empty())
        {
            ret = false;
        }
        else if (str == "0")
        {
            ret = false;
        }
        else if (comn::StringUtil::icompare(str, "false") == 0)
        {
            ret = false;
        }
        else
        {
            ret = true;
        }
    }
    return ret;
}

double AppConfig::getDouble(const std::string& path, double defValue)
{
    double ret = defValue;
    std::string str;
    if (get(path, str))
    {
        comn::StringCast::toValue(str, ret);
    }
    return ret;
}

std::string AppConfig::getString(const std::string& path, const std::string& defValue)
{
	return get(path, defValue);
}

bool AppConfig::reload()
{
    return m_iniFile->load(m_filename.c_str());
}

std::string AppConfig::getFileName() const
{
    return m_filename;
}

void AppConfig::set(const std::string& path, const std::string& value)
{
    std::string sec("App");
    std::string key(path);

    size_t pos = path.find('.');
    if (pos != std::string::npos)
    {
        sec = path.substr(0, pos);
        key = path.substr(pos + 1);
    }
    return m_iniFile->setValue(sec.c_str(), key.c_str(), value);   
}

std::vector<std::string> AppConfig::getKeys(const std::string& path)
{
	std::vector<std::string> keys;
	if (m_iniFile->existSection(path.c_str()))
	{
		IniFile::IniSection& section = m_iniFile->ensure(path.c_str());
		for (size_t i = 0; i < section.entryArray.elements.size(); ++i)
		{
			keys.push_back(section.entryArray.elements[i].key);
		}
	}
	return keys;
}
