/*
 * IniFile.cpp
 *
 *  Created on: 2010-3-23
 *      Author: terry
 */

#include "IniFile.h"
#include <fstream>
#include <assert.h>

using namespace std;



namespace comn
{


IniFile::IniFile()
{
}

IniFile::~IniFile()
{
}

bool IniFile::load( const char* filename )
{
	bool done = false;
	std::ifstream ifs;
	ifs.open( filename );
	if ( ifs.is_open() )
	{
		clear();

		/// parse header
		LineFlag flag = LINE_UNKNOWN;
		std::string line;
		while ( std::getline( ifs, line ) )
		{
			flag = parseLine( line );
			if ( flag == LINE_COMMENT )
			{
				m_fileComment += line;
				m_fileComment += "\n";
			}
			else
			{
				break;
			}
		}

		IniSection sec;
		IniEntry entry;
		std::string	comment;

		do
		{
			switch ( flag )
			{
			case LINE_SECTION:
				{
					if ( !sec.empty() )
					{
						m_sections.elements.push_back( sec );
						sec.clear();
					}

					sec.name = parseSectionName( line );
				}
				break;
			case LINE_COMMENT:
				{
					comment += line;
					comment += "\n";
				}
				break;
			case LINE_ENTRY:
				{
					entry.comment = comment;
					parseEntry( line, entry );

					sec.entryArray.elements.push_back( entry );
					comment.clear();
					entry.clear();
				}
				break;
			case LINE_EMPTY:
				break;
			default:
				{
					comment += line;
					comment += "\n";
				}
				break;
			}

			std::getline( ifs, line );
			flag = parseLine( line );
		} while ( ifs );

		if ( !sec.empty() )
		{
			m_sections.elements.push_back( sec );
		}

		ifs.close();
		done = true;
	}
	return done;
}

bool IniFile::save( const char* filename )
{
	bool success = false;
	std::ofstream ofs;
	ofs.open( filename );
	if ( ofs.is_open() )
	{
		ofs << m_fileComment;

		iterator it = m_sections.elements.begin();
		for ( ; it != m_sections.elements.end(); ++ it )
		{
			IniSection& sec = *it;
			ofs << endl << "[" << sec.name << "]" << endl;

			IniEntryIterator pos = sec.entryArray.elements.begin();
			for (; pos != sec.entryArray.elements.end(); ++ pos )
			{
				if ( !pos->comment.empty() )
				{
					ofs << pos->comment;
				}
				ofs << pos->key << "=" << pos->value << endl;
			}
		}

		ofs.close();
		success = true;
	}
	return success;
}


size_t IniFile::getSectionCount()
{
	return m_sections.elements.size();
}

bool IniFile::existSection( const char* sectionName )
{
	iterator it = findIterator( sectionName );
	return ( it != m_sections.elements.end() );
}


bool IniFile::insertSection( const char* sectionName )
{
	bool done = false;
	iterator it = findIterator( sectionName );
	if ( it == m_sections.elements.end() )
	{
		m_sections.elements.push_back( IniSection( sectionName ) );
		done = true;
	}
	return done;
}

void IniFile::removeSection( const char* sectionName )
{
	iterator it = findIterator( sectionName );
	if ( it != m_sections.elements.end() )
	{
		m_sections.elements.erase( it );
	}
}



void IniFile::removeComment( const char* sectionName )
{
	iterator it = findIterator( sectionName );
	if ( it != m_sections.elements.end() )
	{
		it->removeComment();
	}
}


void IniFile::removeComment()
{
	iterator it = m_sections.elements.begin();
	for ( ; it != m_sections.elements.end(); ++ it )
	{
		it->removeComment();
	}
}


void IniFile::setComment( const char* sectionName, const char* keyName, const char* comment )
{
	IniSection& sec = ensure( sectionName );
	IniEntry& entry = sec.ensure( keyName );
	entry.comment = comment;
}

std::string IniFile::getComment( const char* sectionName, const char* keyName )
{
	iterator it = findIterator( sectionName );
	if ( it != m_sections.elements.end() )
	{
		IniSection& sec = *it;
		IniEntryIterator pos = sec.findIterator( keyName );
		if ( pos != sec.entryArray.elements.end() )
		{
			return pos->comment;
		}
	}
	return std::string();
}


size_t IniFile::getKeyCount( const char* sectionName )
{
	iterator it = findIterator( sectionName );
	if ( it != m_sections.elements.end() )
	{
		return it->entryArray.elements.size();
	}
	return 0;
}

bool IniFile::existKey( const char* sectionName, const char* keyName )
{
	iterator it = findIterator( sectionName );
	if ( it != m_sections.elements.end() )
	{
		IniSection& sec = *it;
		return ( sec.findIterator( keyName ) != sec.entryArray.elements.end() );
	}
	return false;
}

void IniFile::removeKey( const char* sectionName, const char* keyName )
{
	iterator it = findIterator( sectionName );
	if ( it != m_sections.elements.end() )
	{
		IniSection& sec = *it;
		sec.remove( keyName );
	}
}


std::string	IniFile::getValue( const char* sectionName, const char* keyName,
		const std::string& defaultValue )
{
	iterator it = findIterator( sectionName );
	if ( it != m_sections.elements.end() )
	{
		IniSection& sec = *it;
		IniEntryIterator pos = sec.findIterator( keyName );
		if ( pos != sec.entryArray.elements.end() )
		{
			return pos->value;
		}
	}
	return defaultValue;
}

std::string	IniFile::getValue( const char* sectionName, const char* keyName,
			const char* defaultValue )
{
	return getValue( sectionName, keyName, std::string(defaultValue) );
}

void IniFile::setValue( const char* sectionName, const char* keyName,
		const char* value )
{
	IniSection& sec = ensure( sectionName );
	sec.set( keyName, value );
}

void IniFile::setValue( const char* sectionName, const char* keyName,
		const std::string& value )
{
	IniSection& sec = ensure( sectionName );
	sec.set( keyName, value );
}


std::string	IniFile::getFileComment()
{
	return m_fileComment;
}

void IniFile::setFileComment( const std::string& comment )
{
	m_fileComment = comment;
}

void IniFile::clear()
{
	m_fileComment.clear();
	m_sections.elements.clear();
}


IniFile::IniSection& IniFile::getSection(size_t idx)
{
    assert(idx < m_sections.elements.size());
    return m_sections.elements[idx];
}

IniFile::IniSection& IniFile::ensure( const char* name )
{
	iterator it = findIterator( name );
	if ( it == m_sections.elements.end() )
	{
		m_sections.elements.push_back( IniSection( name ) );
		return m_sections.elements.back();
	}
	return *it;
}

IniFile::iterator	IniFile::findIterator( const char* sectionName )
{
	iterator it = m_sections.elements.begin();
	for ( ; it != m_sections.elements.end(); ++ it )
	{
		if ( it->name == sectionName )
		{
			return it;
		}
	}
	return m_sections.elements.end();
}

IniFile::LineFlag IniFile::parseLine( const std::string& line )
{
	if ( line.empty() )
		return LINE_EMPTY;

	char firstChar = getFirstVisibleChar( line );
	if ( firstChar == ';' || firstChar == '#' )
	{
		return LINE_COMMENT;
	}
	else if ( firstChar == '[' )
	{
		return LINE_SECTION;
	}

	size_t idx = line.find_first_of( '=' );
	if ( idx != std::string::npos )
	{
		return LINE_ENTRY;
	}
	return LINE_UNKNOWN;
}

char IniFile::getFirstVisibleChar( const std::string& line )
{
	for ( size_t i = 0; i < line.size(); ++ i )
	{
		if ( line[i] != ' ' && line[i] != '\t' )
		{
			return line[i];
		}
	}
	return 0;
}

std::string IniFile::parseSectionName( const std::string& line )
{
	size_t start = line.find_first_of( '[' );
	size_t stop = line.find_last_of( ']' );
	if ( stop == std::string::npos )
	{
		return line.substr( start );
	}
	return line.substr( start + 1, stop - start-1 );
}

void IniFile::parseEntry( const std::string& line, IniEntry& entry )
{
	size_t idx = line.find_first_of( '=' );
	entry.key = line.substr( 0, idx );
	entry.value = line.substr( idx + 1 );
}



IniFile::IniSection::IniSection()
{
}

IniFile::IniSection::IniSection( const std::string& aName )
:name( aName )
{
}

void IniFile::IniSection::set( const char* keyName, const std::string& value )
{
	IniEntry& entry = ensure( keyName );
	entry.value = value;
}

bool IniFile::IniSection::get(const char* keyName, std::string& value)
{
    bool found = false;
    IniEntryIterator it = findIterator(keyName);
    if (it != entryArray.elements.end())
    {
        value = it->value;
        found = true;
    }
    return found;
}

bool IniFile::IniSection::get(const char* keyName, int& value)
{
    std::string strValue;
    if (!get(keyName, strValue))
    {
        return false;
    }

    std::istringstream iss(strValue);
    iss >> value;

    return true;
}

bool IniFile::IniSection::get(const char* keyName, double& value)
{
    std::string strValue;
    if (!get(keyName, strValue))
    {
        return false;
    }

    std::istringstream iss(strValue);
    iss >> value;

    return true;
}

bool IniFile::IniSection::exist(const char* keyName)
{
    return (findIterator(keyName) != entryArray.elements.end());
}


IniFile::IniEntryIterator IniFile::IniSection::findIterator( const char* keyName )
{
	IniEntryIterator it = entryArray.elements.begin();
	for ( ; it != entryArray.elements.end(); ++ it )
	{
		if ( it->key == keyName )
		{
			return it;
		}
	}
	return entryArray.elements.end();
}

IniFile::IniEntry& IniFile::IniSection::ensure( const char* keyName )
{
	IniEntryIterator it = findIterator( keyName );
	if ( it == entryArray.elements.end() )
	{
		entryArray.elements.push_back( IniEntry(keyName));
		return entryArray.elements.back();
	}
	return *it;
}

void IniFile::IniSection::remove( const char* keyName )
{
	IniEntryIterator it = findIterator( keyName );
	if ( it != entryArray.elements.end() )
	{
		entryArray.elements.erase( it );
	}
}

void IniFile::IniSection::removeComment()
{
	IniEntryIterator pos = entryArray.elements.begin();
	for ( ; pos != entryArray.elements.end(); ++ pos )
	{
		pos->comment.clear();
	}
}

void IniFile::IniSection::clear()
{
	name.clear();
	entryArray.elements.clear();
}

bool IniFile::IniSection::empty()
{
	return name.empty() && entryArray.elements.empty();
}


}