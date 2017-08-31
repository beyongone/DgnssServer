/*
 * Path.h
 *
 *  Created on: 2012-5-22
 *      Author: terry
 */

#ifndef PATH_H_
#define PATH_H_

#include "BasicType.h"
#include <string>
#include <vector>


namespace comn
{

class  Path
{
public:
    Path();
    ~Path();

    enum Constants
    {
#ifdef WIN32
        PATH_SPERATOR    = '\\',
#else
        PATH_SPERATOR    = '/',
#endif //

        FORWARD_SLASH = '/',
        BACKWARD_SLASH = '\\'

    };

    //typedef std::string     std::string;

#ifdef WIN32
    typedef __int64 int64_t;

#endif //

    static const size_t npos = size_t(-1);


    static bool exist(const char* path);
    static bool exist(const std::string& path) { return exist(path.c_str()); }

    static bool createDirectory(const char* path);
    static bool createDirectories(const char* path);
    static bool createDirectories(const std::string& path) { return createDirectories(path.c_str()); }

    static bool isFile(const char* path);
    static bool isFile(const std::string& path) { return isFile(path.c_str()); }
    static bool isDir(const char* path);
    static bool isDir(const std::string& path) {return isDir(path.c_str()); }
    static bool isLink(const char* path);

    static bool deleteFile(const char* path);
    static bool deleteFile(const std::string& path) {return deleteFile(path.c_str()); }

    static bool removeDir(const char* path);
    static bool removeDir(const std::string& path) {return removeDir(path.c_str()); }

    static time_t getatime(const char* path);
    static time_t getmtime(const char* path);
    static time_t getctime(const char* path);

	static time_t getatime(const std::string& path) { return getatime(path.c_str());}
	static time_t getmtime(const std::string& path) { return getmtime(path.c_str());}
	static time_t getctime(const std::string& path) { return getctime(path.c_str());}

    static int64_t getSize(const char* path);
    static int64_t getSize(const std::string& path) { return getSize(path.c_str()); }

    static std::string getCurDir();
    static std::string getHomeDir();


    static bool isValidName(const char* path);
    static bool isabs(const std::string& path);

    static std::string abspath(const char* path);
    static void abspath(std::string& path);

    /**
     * Normalize a pathname.
     * A//B, A/./B and A/foo/../B all become A/B.
     */
    static std::string normpath(const char* path);
    static void normpath(std::string& path);

    static std::string basename(const char* path);
    static std::string basename(const std::string& path);

    static std::string dirname(const char* path);
    static std::string dirname(const std::string& path);

    static std::string join(const char* dir, const char* name);
    static std::string join(const std::string& dir, const char* name);
    static std::string join(const std::string& dir, const std::string& name);

    static std::string getFileName(const char* path);
    static std::string getFileName(const std::string& path);

    /// get file extension, such as ".txt"
    static std::string getFileExt(const char* filename);
    static std::string getFileExt(const std::string& filename);

    static std::string getFileTitle(const char* filename);
    static std::string getFileTitle(const std::string& filename);


    static bool addSperator(std::string& path);
    static void removeSperator(std::string& path);
    static bool endWithSperator(const char* path);
    static bool endWithSperator(std::string& path);

    static bool split(const char* path, char sp, std::string& dir, std::string& name);
    static bool rsplit(const char* path, char sp, std::string& dir, std::string& name);

    static void toWindowsSlash(std::string& path);
    static void toUnixSlash(std::string& path);

	static void convertSlash(std::string& path);


    bool listDir(const std::string& dir, std::vector< std::string >& array);

    bool listFile(const std::string& dir, std::vector< std::string >& array);

    

#ifdef WIN32
    static std::string getWindowDir();
    static std::string getWindowDisk();
    static std::string getSystemDir();
    static std::string getSystem32Dir();

    static std::string getWorkDir(HMODULE handle = NULL);
#else
    static std::string getWorkDir();
#endif //WIN32


    static std::string findFile(const std::string paths[], int size, const char* filename);


};



}

#endif /* PATH_H_ */
