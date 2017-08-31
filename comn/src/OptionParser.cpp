/*
 * OptionParser.cpp
 *
 *  Created on: 2015年7月31日
 *      Author: chuanjiang.zh@qq.com
 */

#include "OptionParser.h"
#include <iostream>



namespace detail
{

bool startWith(const std::string& src, const std::string& pattern)
{
    if (src.size() < pattern.size())
        return false;
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        if (pattern[i] != src[i])
            return false;
    }
    return true;
}

std::vector< std::string > split(const std::string& cmdLine)
{
	std::vector< std::string > arr;
	size_t prev = 0;
	bool inQuote = false;
	bool isSeperator = false;
	for (size_t i = 0; i < cmdLine.size(); ++i)
	{
		if (cmdLine[i] == '"')
		{
			if (inQuote)
			{
				arr.push_back(cmdLine.substr(prev, i - prev - 1));
			}
			else
			{
				if (i > prev && !isSeperator)
					arr.push_back(cmdLine.substr(prev, i - prev));
			}
			prev = i + 1;
			inQuote = !inQuote;
			isSeperator = false;
		}
		else if (cmdLine[i] == ' ' || cmdLine[i] == '\t')
		{
			if (!isSeperator)
			{
				if (i > prev)
					arr.push_back(cmdLine.substr(prev, i - prev));
			}

			prev = i + 1;
			isSeperator = true;
		}
		else
		{
			isSeperator = false;
		}
	}

	if (cmdLine.size() > 0 && prev < cmdLine.size())
	{
		arr.push_back(cmdLine.substr(prev));
	}
	return arr;
}

int split(const std::string& str, char sp, std::string& head, std::string& tail)
{
	int count = 0;
	size_t idx = str.find(sp);
	if (idx != std::string::npos)
	{
		head = str.substr(0, idx);
		tail = str.substr(idx + 1);
		count = 2;
	}
	else
	{
		head = str;
		tail.clear();
		count = 1;
	}
	return count;
}

std::string getFileTitle(const std::string& name)
{
    size_t idx = name.rfind('/');
    if (idx == std::string::npos)
        idx = name.rfind('\\');

    std::string title(name);
    if (idx != std::string::npos)
    {
        title = name.substr(idx + 1);
    }
    return title;
}


}








OptionParser::OptionParser()
{
}

OptionParser::~OptionParser()
{
}

OptionParser::OptionParser(const std::string& usage, const std::string& version):
        m_usage(usage),
        m_version(version)
{
}

void OptionParser::setVersion(const std::string& version)
{
    m_version = version;
}

void OptionParser::setUsage(const std::string& usage)
{
    m_usage = usage;
}

bool OptionParser::addOption(char shortOpt, const std::string& longOpt, bool needArg,
		const std::string& def, const std::string& help)
{
    if (!isalpha(shortOpt) && longOpt.empty())
    {
        return false;
    }

    Option option;
    option.shortOpt = shortOpt;
    option.longOpt = longOpt;
    option.needArg = needArg;
    option.defValue = def;
    option.help = help;
    option.present = false;
    option.value = def;

    if (option.longOpt.empty())
    {
        option.longOpt = std::string(1, shortOpt);
    }

    if (existOption(option))
    {
        return false;
    }

    m_optionMap[option.longOpt] = option;
    return true;
}

/// parse command line
bool OptionParser::parse(int argc, char** argv)
{
	std::vector< std::string > arrStr;
    for (int i = 0; i < argc; ++i)
    {
        arrStr.push_back(argv[i]);
    }
    return parse(arrStr);
}

/// parse command line
bool OptionParser::parse(const std::string& cmdLine)
{
	std::vector< std::string > arrStr = detail::split(cmdLine);
	return parse(arrStr);
}

bool OptionParser::parse(const std::vector< std::string >& arrStr)
{
	clearResult();

	if (arrStr.empty())
	{
		return false;
	}

	m_program = detail::getFileTitle(arrStr[0]);

	bool working = true;
	Option* option = NULL;
	bool needArg = false;
	std::string name;
	std::string value;

	for (size_t i= 1; i < arrStr.size(); ++ i)
	{
		const std::string& item = arrStr[i];

		if (detail::startWith(item, "--"))
		{
			if (needArg)
			{
				std::cout << "--" << option->longOpt << " need arg." << std::endl;
				return false;
			}

			int count = detail::split(item.substr(2), '=', name, value);
			option = findOption(name);
			if (!option)
			{
				std::cout << "invalid option: " << name << std::endl;
				return false;
			}

			if (count > 1)
			{
				option->setValue(value);
			}
			else
			{
                option->present = true;
				needArg = option->needArg && option->defValue.empty();
			}
		}
		else if (detail::startWith(item, "-") || detail::startWith(item, "/"))
		{
			if (needArg)
			{
				std::cout << "--" << option->longOpt << " need arg." << std::endl;
				return false;
			}

			int count = detail::split(item.substr(1), '=', name, value);
			option = findOption(name);
			if (!option)
			{
				std::cout << "invalid option: " << name << std::endl;
				return false;
			}

			if (count > 1)
			{
				option->setValue(value);
			}
			else
			{
                option->present = true;
				needArg = option->needArg;
			}
		}
		else
		{
			if (option && needArg)
			{
				option->setValue(item);
			}
			else
			{
				m_args.push_back(item);
			}

			needArg = false;
		}

	}

	return working;
}

/// print usage
void OptionParser::usage() const
{
    std::cout << getUsage() << std::endl;
}

/// get usage
std::string OptionParser::getUsage() const
{
    std::string szUsage = m_version;
    szUsage += "\n";

    szUsage = getUsageHeader();
    szUsage += "\n";

    OptionMap::const_iterator it = m_optionMap.begin();
    for (; it != m_optionMap.end(); ++ it)
    {
        szUsage += it->second.toString();
        szUsage += "\n";
    }

    return szUsage;
}

/// print version info
void OptionParser::version() const
{
    std::cout << m_version << std::endl;
}

/// get version info
std::string OptionParser::getVersion() const
{
    return m_version;
}


bool OptionParser::hasOption(char shortOpt) const
{
	OptionMap::const_iterator it = m_optionMap.begin();
	for (; it != m_optionMap.end(); ++ it)
	{
		if (it->second.shortOpt == shortOpt)
		{
			return it->second.present;
		}
	}
	return false;
}

bool OptionParser::hasOption(const std::string& name) const
{
	bool found = false;
	OptionMap::const_iterator it = find(name);
	if (it != m_optionMap.end())
	{
		const Option& option = it->second;
		found = option.present;
	}
	return found;
}

bool OptionParser::getOption(const std::string& name, std::string& value) const
{
	bool found = false;
	OptionMap::const_iterator it = find(name);
	if (it != m_optionMap.end())
	{
		const Option& option = it->second;
		if (option.present)
		{
			value = option.value;
			found = true;
		}
	}
	return found;
}

size_t OptionParser::getArgCount() const
{
    return m_args.size();
}

std::string OptionParser::getArg(size_t idx) const
{
    return m_args[idx];
}

bool OptionParser::hasArg() const
{
    return getArgCount() > 0;
}


/// enable help(-?,-h,--help)
void OptionParser::enableHelp()
{
    addOption('h', "help", false, std::string(), "print help info");
}

/// enable version( --version )
void OptionParser::enableVersion()
{
    addOption('v', "version", false, std::string(), "print version info");
}

/// check whether help option exist
bool OptionParser::hasHelpOption() const
{
    return hasOption("help");
}

/// check whether version option exist
bool OptionParser::hasVersionOption() const
{
    return hasOption("version");
}

bool OptionParser::Option::equal(const Option& opt) const
{
    if ((shortOpt == opt.shortOpt) && (isalpha(shortOpt)))
    {
        return true;
    }
    return longOpt == opt.longOpt;
}

bool OptionParser::existOption(const Option& option) const
{
    OptionMap::const_iterator it = m_optionMap.begin();
    for (; it != m_optionMap.end(); ++ it)
    {
        if (option.equal(it->second))
        {
            return true;
        }
    }
    return false;
}

OptionParser::OptionMap::iterator OptionParser::find(const std::string& name)
{
	OptionMap::iterator it = m_optionMap.find(name);
	if (it == m_optionMap.end())
	{
		if (name.size() == 1)
		{
			char shortOpt = name[0];
			it = m_optionMap.begin();
			for (; it != m_optionMap.end(); ++ it)
			{
				if (shortOpt == it->second.shortOpt)
				{
					break;
				}
			}
		}
	}
	return it;
}

OptionParser::OptionMap::const_iterator OptionParser::find(const std::string& name) const
{
	OptionMap::const_iterator it = m_optionMap.find(name);
	if (it == m_optionMap.end())
	{
		if (name.size() == 1)
		{
			char shortOpt = name[0];
			it = m_optionMap.begin();
			for (; it != m_optionMap.end(); ++ it)
			{
				if (shortOpt == it->second.shortOpt)
				{
					break;
				}
			}
		}
	}
	return it;
}

OptionParser::Option* OptionParser::findOption(const std::string& name)
{
	Option* option = NULL;
	OptionMap::iterator it = find(name);
	if (it != m_optionMap.end())
	{
		option = &(it->second);
	}
	return option;
}

std::string OptionParser::Option::toString() const
{
	std::ostringstream oss;
	oss << "  ";
	if (isalpha(shortOpt))
	{
		oss << '-' << shortOpt;
		if (!longOpt.empty())
		{
			oss << ", --" << longOpt;
		}
	}
	else
	{
		oss << "    --" << longOpt;
	}

	if (hasDefault())
	{
		oss << '=' << defValue;
	}

	int blankCount = 24 - (int) oss.str().size();
	if (blankCount <= 0)
		blankCount = 1;
	oss << std::string(blankCount, ' ');
	oss << help;

	if (hasDefault())
	{
		oss << ". default is " << defValue;
	}

	return oss.str();
}


std::string OptionParser::getUsageHeader() const
{
    std::string prog = "%prog";
    std::string line(m_usage);
    size_t pos = line.find(prog);
    if (pos != std::string::npos)
    {
        line.replace(pos, prog.size(), m_program);
    }
    return line;
}

void OptionParser::clearResult()
{
	OptionMap::iterator it = m_optionMap.begin();
	for (; it != m_optionMap.end(); ++ it)
	{
		it->second.present = false;
	}

    m_args.clear();
    m_program.clear();
}

