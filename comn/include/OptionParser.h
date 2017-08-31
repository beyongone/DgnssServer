/*
 * OptionParser.h
 *
 *  Created on: 2015年7月31日
 *      Author: chuanjiang.zh@qq.com
 */

#ifndef TEST_OPTIONPARSER_H_
#define TEST_OPTIONPARSER_H_

#include <string>
#include <sstream>
#include <vector>
#include <map>



class OptionParser
{
public:
    OptionParser();
    ~OptionParser();

    OptionParser(const std::string& usage, const std::string& version);

    void setVersion(const std::string& version);

    void setUsage(const std::string& usage);

    bool addOption(char shortOpt, const std::string& longOpt, const std::string& help)
    {
        return addOption(shortOpt, longOpt, false, std::string(), help);
    }

    bool addOption(char shortOpt, const std::string& longOpt, bool needArg,
    		const std::string& def, const std::string& help);

    template < class T >
    bool addOption(char shortOpt, const std::string& longOpt, bool needArg, T def, const std::string& help)
    {
        std::ostringstream ss;
        ss << def;
        return addOption(shortOpt, longOpt, needArg, ss.str(), help);
    }

    /// parse command line
    bool parse(int argc, char** argv);

    /// parse command line
    bool parse(const std::string& cmdLine);

    bool parse(const std::vector< std::string >& arrStr);

    /// print usage
    void usage() const;

    /// get usage
    std::string getUsage() const;

    /// print version info
    void version() const;

    /// get version info
    std::string getVersion() const;


    bool hasOption(char shortOpt) const;

    bool hasOption(const std::string& name) const;

    bool getOption(const std::string& name, std::string& value) const;

    template < class T >
    bool getOption(const std::string& name, T& value) const
    {
        std::string str;
        if (!getOption(name, str))
        {
            return false;
        }

        std::istringstream iss(str);
        iss >> value;
        return true;
    }


    size_t getArgCount() const;

    std::string getArg(size_t idx) const;

    bool hasArg() const;


    /// enable help(-?,-h,--help)
    void enableHelp();

    /// enable version( --version )
    void enableVersion();

    /// check whether help option exist
    bool hasHelpOption() const;

    /// check whether version option exist
    bool hasVersionOption() const;


private:

private:
    struct Option
    {
        char    shortOpt;
        std::string longOpt;
        bool needArg;
        std::string defValue;
        std::string help;

        bool present;
        std::string value;

        Option()
        {
            shortOpt = 0;
            needArg = false;
            present = false;
        }

        bool equal(const Option& opt) const;

        std::string toString() const;

        bool hasDefault() const
        {
        	return !defValue.empty();
        }

        void setValue(const std::string& v)
        {
        	present = true;
        	value = v;
        }
    };

    typedef std::map< std::string, Option >     OptionMap;

    bool existOption(const Option& option) const;
    OptionMap::iterator find(const std::string& name);
    OptionMap::const_iterator find(const std::string& name) const;
    Option* findOption(const std::string& name);

    std::string getUsageHeader() const;

    void clearResult();


private:
    std::string m_usage;
    std::string m_version;
    std::vector< std::string >  m_args;

    OptionMap   m_optionMap;

    std::string	m_program;

};

#endif /* TEST_OPTIONPARSER_H_ */
