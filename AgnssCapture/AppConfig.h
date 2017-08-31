/*
 * AppConfig.h
 *
 *  Created on: 2016年9月20日
 *      Author: zhengboyuan
 */

#ifndef APPCONFIG_H_
#define APPCONFIG_H_

#include "BasicType.h"
#include <string>
#include "SharedPtr.h"
#include <vector>
#include "TStringCast.h"

namespace comn
{
	class IniFile;
}

/**
 * 应用配置
 */
class AppConfig
{
public:
	/// 获取单实例
    static AppConfig& instance();

	static const char SEPERATOR = '.';

public:
    AppConfig();
    virtual ~AppConfig();

    /**
     * 从文件加载
     * @param filename
     * @return
     */
    bool load(const std::string& filename);

	bool save(const std::string& filename);

    /**
     * 获取指定路径的值, 路径以,分割
     * @param path
     * @param value
     * @return true 表示找到该路径
     */
    bool get(const std::string& path, std::string& value);

    /**
     * 获取指定路径的值, 不存在时返回默认值
     * @param path
     * @param defValue
     * @return
     */
    std::string get(const std::string& path, const std::string& defValue=std::string());
    std::string get(const char* path, const char* defValue="");

    int getInt(const std::string& path, int defValue = 0);
    bool getBool(const std::string& path, bool defValue = false);
    double getDouble(const std::string& path, double defValue = 0.0);
    std::string getString(const std::string& path, const std::string& defValue=std::string());

    bool reload();

    template < class T >
    bool get(const std::string& path, T& t)
    {
        bool found = false;
        std::string value;
        if (get(path, value))
        {
            comn::StringCast::toValue(value, t);
            found = true;
        }
        return found;
    }

    std::string getFileName() const;

    void set(const std::string& path, const std::string& value);

    template < class T >
    void set(const std::string& path, const T& t)
    {
        std::string value = comn::StringCast::toString(t);
        set(path, value);
    }

	std::vector<std::string> getKeys(const std::string& path);

protected:
    std::string m_filename;
    std::shared_ptr< comn::IniFile >    m_iniFile;

};



#endif /* APPCONFIG_H_ */
