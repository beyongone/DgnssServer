/*
 * IniFile.h
 *
 *  Created on: 2010-3-23
 *      Author: terry
 */

#ifndef INIFILE_H_
#define INIFILE_H_

#include <string>
#include <vector>
#include <sstream>
///////////////////////////////////////////////////////////////////


namespace comn
{


/// ini file reader and writer
class  IniFile
{
public:
	IniFile();
	~IniFile();

	bool load(const char* filename);
	bool save(const char* filename);

	size_t getSectionCount();

	bool existSection(const char* name);

	/// return true if insertion happened
	bool insertSection(const char* name);

	void removeSection(const char* name);

	/// remove comment in the section
	void removeComment(const char* sectionName);

	/// remove all comment in the file
	void removeComment();

	/// insert comment before key
	void setComment(const char* sectionName, const char* keyName,
			const char* comment);

	/// return empty if comment does not exist
	std::string getComment(const char* sectionName, const char* keyName);

	size_t getKeyCount(const char* sectionName);
	bool existKey(const char* sectionName, const char* keyName);
	void removeKey(const char* sectionName, const char* keyName);

	std::string	getValue(const char* sectionName, const char* keyName,
			const std::string& defaultValue);
	std::string	getValue(const char* sectionName, const char* keyName,
			const char* defaultValue = "");

	void setValue(const char* sectionName, const char* keyName,
			const char* value);
	void setValue(const char* sectionName, const char* keyName,
			const std::string& value);

    template < class T >
    bool queryValue(const char* sectionName, const char* keyName, T& value)
    {
        bool found = false;
        iterator it = findIterator(sectionName);
        if (it != m_sections.elements.end() )
        {
            IniSection& sec = *it;
            IniEntryIterator pos = sec.findIterator(keyName);
            if (pos != sec.entryArray.elements.end() )
            {
                std::istringstream iss(pos->value);
                iss >> value;
                found = true;
            }
        }
        return found;
    }

    template < class T >
    bool getValue(const char* sectionName, const char* keyName, T& value)
    {
        bool found = queryValue(sectionName, keyName, value);
        if (!found)
        {
            value = T();
        }
        return found;
    }

	template < class T >
	T getValue(const char* sectionName, const char* keyName, T defaultValue )
	{
		iterator it = findIterator(sectionName);
		if (it != m_sections.elements.end() )
		{
			IniSection& sec = *it;
			IniEntryIterator pos = sec.findIterator(keyName);
			if (pos != sec.entryArray.elements.end() )
			{
				std::istringstream iss(pos->value);
				T t(defaultValue);
				iss >> t;
				return t;
			}
		}
		return defaultValue;
	}

	template < class T >
	void setValue(const char* sectionName, const char* keyName,
				T value )
	{
		IniSection& sec = ensure(sectionName);
		std::ostringstream ss;
		ss << value;
		return sec.set(keyName, ss.str());
	}


	std::string	getFileComment();
	void setFileComment(const std::string& comment);

	void clear();



public:
	class  IniEntry
	{
	public:
		std::string	key;
		std::string	value;
		std::string	comment;

		IniEntry()
		{
		}

		explicit IniEntry(const std::string& keyName )
		:key(keyName)
		{
		}

		void clear()
		{
			key.clear();
			value.clear();
			comment.clear();
		}
	};

	struct IniEntryArray
	{
		std::vector< IniEntry >	elements;
		typedef std::vector< IniEntry >::iterator	iterator;
	};
	
	typedef IniEntryArray::iterator		IniEntryIterator;

	class  IniSection
	{
	public:
		std::string		name;
		IniEntryArray	entryArray;

		IniSection();
		explicit IniSection(const std::string& name);

		void set(const char* keyName, const std::string& value);
		bool get(const char* keyName, std::string& value);
		bool get(const char* keyName, int& value);
		bool get(const char* keyName, double& value);
		bool exist(const char* keyName);

		IniEntryIterator findIterator(const char* keyName);
		IniEntry& ensure(const char* keyName);
		void remove(const char* keyName);
		void removeComment();
		void clear();
		bool empty();



	};

	struct IniSectionArray
	{
		std::vector< IniSection >	elements;
		typedef std::vector< IniSection >::iterator	iterator;
	};

	typedef IniSectionArray::iterator	iterator;

	IniSection& getSection(size_t idx);

	IniSection& ensure(const char* name);

private:

	iterator	findIterator(const char* sectionName);

	enum LineFlag
	{
		LINE_UNKNOWN,
		LINE_EMPTY,
		LINE_COMMENT,
		LINE_SECTION,
		LINE_ENTRY
	};

	LineFlag parseLine(const std::string& line);
	char getFirstVisibleChar(const std::string& line);
	std::string parseSectionName(const std::string& line);
	void parseEntry(const std::string& line, IniEntry& entry);




private:
	IniSectionArray		m_sections;
	std::string			m_fileComment;

};

}

#endif /* INIFILE_H_ */
