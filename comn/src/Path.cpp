/*
 * Path.cpp
 *
 *  Created on: 2012-5-22
 *      Author: terry
 */

#include "Path.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef WIN32
    #include <windows.h>
    #include <shlobj.h>

#if defined(_MSC_VER)
    #pragma comment(lib,"Shell32.lib")
#endif //

#else
    #include <sys/stat.h>
    #include <dirent.h>
    #include <unistd.h>

    #include <stdlib.h>
    #include <pwd.h>
#endif //WIN32

#ifndef MAX_PATH
#define MAX_PATH    256
#endif //MAX_PATH


namespace comn
{


Path::Path()
{
}

Path::~Path()
{
}


size_t rfind(const char* p, size_t length, char sp)
{
    for (size_t pos = length; pos != 0; -- pos)
    {
        if (p[pos - 1] == sp)
        {
            return pos -1;
        }
    }
    return (size_t)-1;
}



#ifdef WIN32

class AutoHandle
{
public:
    explicit AutoHandle(const char* path):
        m_handle(INVALID_HANDLE_VALUE)
    {
        m_handle = CreateFile(path, GENERIC_READ,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    explicit AutoHandle(HANDLE handle):
            m_handle(handle)
    {
    }

    ~AutoHandle()
    {
        if (m_handle != INVALID_HANDLE_VALUE)
        {
            ::CloseHandle(m_handle);
        }
    }

    bool operator ! () const
    {
        return (m_handle == INVALID_HANDLE_VALUE);
    }

    operator bool () const
    {
        return (m_handle != INVALID_HANDLE_VALUE);
    }

    HANDLE  m_handle;

};


time_t filetime_to_time(const FILETIME& ft)
{
    ULARGE_INTEGER ull;
    ull.LowPart = ft.dwLowDateTime;
    ull.HighPart = ft.dwHighDateTime;
    return ull.QuadPart / 10000000ULL - 11644473600ULL;
}

bool createIfNotExist(const char* path)
{
    BOOL done = TRUE;
    DWORD attr = ::GetFileAttributes(path);
    if (attr == 0xFFFFFFFF)
    {
        done = CreateDirectory(path, NULL);
    }
    return (done != 0);
}












bool Path::exist(const char* path)
{
    return (GetFileAttributes(path) != 0xFFFFFFFF);
}

bool Path::createDirectory(const char* path)
{
    return (::CreateDirectory(path, NULL) != FALSE);
}

bool Path::createDirectories(const char* path)
{
    size_t length = strlen(path);
    char buffer[MAX_PATH] = {0};
    #if defined(_MSC_VER) && (_MSC_VER >= 1400 )
    strcpy_s(buffer, MAX_PATH, path);
    #else
    strcpy(buffer, path);
    #endif //_MSC_VER

    for (size_t i = 0; i < length; ++ i)
    {
        if (buffer[i] == PATH_SPERATOR)
        {
            buffer[i] = '\0';

            if (!createIfNotExist(buffer))
            {
                    return false;
            }

            buffer[i] = PATH_SPERATOR;
        }
    }

    if (path[length - 1] == PATH_SPERATOR)
    {
        return true;
    }

    return createIfNotExist(buffer);
}

bool Path::isFile(const char* path)
{
    DWORD attr = GetFileAttributes(path);
    return (attr != 0xFFFFFFFF) && ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool Path::isDir(const char* path)
{
    DWORD attr = GetFileAttributes(path);
    return (attr != 0xFFFFFFFF) && ((attr & FILE_ATTRIBUTE_DIRECTORY) > 0);
}

bool Path::isLink(const char* path)
{
    size_t length = strlen(path);
    if (length <= 4)
    {
        return false;
    }
    int ret = strcmp((path + length - 4), ".lnk");
    return (ret == 0);
}

bool Path::deleteFile(const char* path)
{
    BOOL ret = DeleteFile(path);
    return (ret != FALSE);
}

bool Path::removeDir(const char* path)
{
    BOOL ret = RemoveDirectory(path);
    return (ret != FALSE);
}

time_t Path::getatime(const char* path)
{
    AutoHandle handle(path);
    FILETIME ft;
    memset(&ft, 0, sizeof(ft));
    GetFileTime(handle.m_handle, NULL, &ft, NULL);
    return filetime_to_time(ft);
}

time_t Path::getmtime(const char* path)
{
    AutoHandle handle(path);
    FILETIME ft;
    memset(&ft, 0, sizeof(ft));
    GetFileTime(handle.m_handle, NULL, NULL, &ft);
    if ((ft.dwHighDateTime == 0) && (ft.dwLowDateTime == 0))
    {
        return 0;
    }
    return filetime_to_time(ft);
}

time_t Path::getctime(const char* path)
{
    AutoHandle handle(path);
    FILETIME ft;
    memset(&ft, 0, sizeof(ft));
    GetFileTime(handle.m_handle, &ft, NULL, NULL);
    return filetime_to_time(ft);
}

int64_t Path::getSize(const char* path)
{
    AutoHandle handle(path);
    LARGE_INTEGER li;
    li.LowPart = GetFileSize(handle.m_handle, (DWORD*)&li.HighPart);

    return li.QuadPart;
}

std::string Path::getCurDir()
{
    std::string path;
    char buffer[MAX_PATH] = {0};
    DWORD count = ::GetCurrentDirectory(MAX_PATH, buffer);
    if (count != 0)
    {
        path.assign(buffer, count);
    }
    return path;
}

std::string Path::getHomeDir()
{
    std::string path;
    char buffer[MAX_PATH] = {0};
    HRESULT ret = SHGetFolderPath(NULL, CSIDL_PERSONAL|CSIDL_FLAG_CREATE,
            NULL,
            0,
            buffer);
    if (ret == S_OK)
    {
        path.assign(buffer);
    }
    return path;
}

std::string Path::getWindowDir()
{
    std::string path;
    char buffer[MAX_PATH] = {0};
    UINT count = ::GetWindowsDirectory(buffer, MAX_PATH);
    if (count != 0)
    {
        path.assign(buffer);
    }
    return path;
}

std::string Path::getWindowDisk()
{
    std::string path;
    char buffer[MAX_PATH] = {0};
    UINT count = ::GetWindowsDirectory(buffer, MAX_PATH);
    if (count != 0)
    {
        path.assign(buffer);
    }
    return path;
}

std::string Path::getSystemDir()
{
    std::string path;
    char buffer[MAX_PATH] = {0};
    UINT count = ::GetSystemDirectory(buffer, MAX_PATH);
    if (count != 0)
    {
        path.assign(buffer);
    }
    return path;
}

std::string Path::getSystem32Dir()
{
    std::string path = getWindowDir();
    path += PATH_SPERATOR;
    path += "system32";
    return path;
}

std::string Path::getWorkDir(HMODULE handle)
{
    std::string path;
    char buffer[MAX_PATH] = {0};
    DWORD count = ::GetModuleFileName(handle, buffer, MAX_PATH);
    if (count != 0)
    {
        size_t idx = rfind(buffer, count, PATH_SPERATOR);
        if (idx != std::string::npos)
        {
            buffer[idx] = 0;
        }

        path.assign(buffer);
    }
    return path;
}


bool Path::isabs(const std::string& path)
{
	if (path.empty())
	{
		return false;
	}

	if (path[0] == '.' || path[0] == '~')
	{
		return false;
	}

	if (path.find("..") != std::string::npos)
	{
		return false;
	}

	return true;
}


bool Path::isValidName(const char* path)
{
    size_t length = strlen(path);
    if (length == 0)
    {
        return false;
    }

    char excludes[] = {'/', '\\', '|', '?', '<', '>', '*', '"', ':' };
    size_t idx = strcspn(path, excludes);

    return (idx != (size_t)-1);
}

std::string Path::abspath(const char* path)
{
    std::string fullpath;
    char buffer[MAX_PATH] = {0};
    DWORD ret = GetFullPathName(path, MAX_PATH, buffer, NULL);
    if (ret != 0)
    {
        fullpath.assign(buffer, ret);
    }
    return fullpath;
}


std::string Path::normpath(const char* path)
{
    std::string fullpath = abspath(path);
    toWindowsSlash(fullpath);
    return fullpath;
}

void Path::normpath(std::string& path)
{
    path = normpath(path.c_str());
}

struct FileNamePredicate
{
	virtual ~FileNamePredicate() {}

    virtual bool predicate(const char* filename) =0;
};

template < class T >
struct FileNameContainer : public FileNamePredicate
{
    explicit FileNameContainer(T& sequence):
    m_sequence(sequence)
    {
    }

    virtual bool predicate(const char* filename)
    {
        m_sequence.push_back(filename);
        return true;
    }

    T& m_sequence;
};

static bool walkDir(const std::string& dir, FileNamePredicate& pred)
{
    std::string filename = Path::join(dir, TEXT("*"));

    WIN32_FIND_DATA filedata;
    HANDLE hSearch = FindFirstFile(filename.c_str(), &filedata);
    if (hSearch == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    std::string strDot(".");
    std::string strDot2("..");

    BOOL working = true;
    while (working)
    {
        if ((filedata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
            if (strDot.compare(filedata.cFileName) == 0)
            {
                // pass
            }
            else if (strDot2.compare(filedata.cFileName) == 0)
            {
                // pass
            }
            else
            {
                if (!pred.predicate(filedata.cFileName))
                {
                    break;
                }
            }
        }

        working = FindNextFile(hSearch, &filedata);
    }

    FindClose(hSearch);

    return true;
}

static bool walkFile(const std::string& dir, FileNamePredicate& pred)
{
    std::string filename = Path::join(dir, TEXT("*"));

    WIN32_FIND_DATA filedata;
    HANDLE hSearch = FindFirstFile(filename.c_str(), &filedata);
    if (hSearch == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    std::string strDot(".");
    std::string strDot2("..");

    BOOL working = true;
    while (working)
    {
        if ((filedata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
            // pass
        }
        else
        {
            if (!pred.predicate(filedata.cFileName))
            {
                break;
            }
        }

        working = FindNextFile(hSearch, &filedata);
    }

    FindClose(hSearch);

    return true;
}

bool listDir(const std::string& dir, std::vector< std::string >& array)
{
    typedef FileNameContainer< std::vector< std::string > >	Pred;
    Pred pred(array);
    return walkDir(dir, pred);
}

bool listFile(const std::string& dir, std::vector< std::string >& array)
{
    typedef FileNameContainer< std::vector< std::string > >	Pred;
    Pred pred(array);
    return walkFile(dir, pred);
}

#else


bool Path::exist(const char* path)
{
    struct stat buf;
    memset(&buf, 0, sizeof(buf));
    int ret = stat(path, &buf);
    return (ret == 0);
}


bool Path::createDirectory(const char* path)
{
    int ret = mkdir(path, S_IRWXU);
    return (ret == 0 || ret == EEXIST);
}


static bool createIfNotExist(const char* path)
{
    bool done = true;
	
	if (strlen(path) == 0)
	{
		return true;
	}
	
    struct stat buf;
    memset(&buf, 0, sizeof(buf));
    int ret = stat(path, &buf);
	//printf("stat ret:%d, %s\n", ret, path);
	if (ret != 0)
	{
		ret = mkdir(path, S_IRWXU);
		//printf("mkdir ret:%d, %s\n", ret, path);
        done = (0 == ret);
	}

    return done;
}

bool Path::createDirectories(const char* path)
{
    size_t length = strlen(path);
    if (length == 0)
    {
        return false;
    }

    char buffer[MAX_PATH] = {0};
    strncpy(buffer, path, length);

    for (size_t i = 0; i < length; ++ i)
    {
        if (buffer[i] == PATH_SPERATOR)
        {
			if (i == 0)
			{
				continue;
			}
			
            buffer[i] = '\0';

            if (!createIfNotExist(buffer))
            {
                return false;
            }

            buffer[i] = PATH_SPERATOR;
        }
    }

    if (path[length - 1] == PATH_SPERATOR)
    {
        return true;
    }

    return createIfNotExist(buffer);
}

bool Path::isDir(const char* path)
{
    struct stat buf;
    memset(&buf, 0, sizeof(buf));
    int ret = stat(path, &buf);
    if (ret == 0)
    {
        return S_ISDIR(buf.st_mode);
    }
    return false;
}

bool Path::isFile(const char* path)
{
    struct stat buf;
    memset(&buf, 0, sizeof(buf));
    int ret = stat(path, &buf);
    if (ret == 0)
    {
        return S_ISREG(buf.st_mode);
    }
    return false;
}

bool Path::isLink(const char* path)
{
    struct stat buf;
    memset(&buf, 0, sizeof(buf));
    int ret = stat(path, &buf);
    if (ret == 0)
    {
        return S_ISLNK(buf.st_mode);
    }
    return false;
}

bool Path::deleteFile(const char* path)
{
	int ret = remove(path);
	return (ret == 0);
}

time_t Path::getatime(const char* path)
{
    struct stat buf;
    memset(&buf, 0, sizeof(buf));
    int ret = stat(path, &buf);
    if (ret == 0)
    {
        return buf.st_atime;
    }
    return 0;
}

time_t Path::getmtime(const char* path)
{
    struct stat buf;
    memset(&buf, 0, sizeof(buf));
    int ret = stat(path, &buf);
    if (ret == 0)
    {
        return buf.st_mtime;
    }
    return 0;
}

time_t Path::getctime(const char* path)
{
    struct stat buf;
    memset(&buf, 0, sizeof(buf));
    int ret = stat(path, &buf);
    if (ret == 0)
    {
        return buf.st_ctime;
    }
    return 0;
}


int64_t Path::getSize(const char* path)
{
    struct stat buf;
    memset(&buf, 0, sizeof(buf));
    int ret = stat(path, &buf);
    if (ret == 0)
    {
        return buf.st_size;
    }
    return 0;
}



bool Path::isValidName(const char* path)
{
    size_t length = strlen(path);
    if (length == 0)
    {
        return false;
    }

    char excludes[] = {'/', '\\', '|', '?', '<', '>', '*', '"', ':' };
    size_t idx = strcspn(path, excludes);

    return (idx != (size_t)-1);
}

std::string Path::abspath(const char* path)
{
	try
	{
		char buffer[MAX_PATH] = {0};
		return realpath(path, buffer);
	}
	catch (...)
	{
		return path;
	}
}

bool Path::isabs(const std::string& path)
{
    if (path.empty())
    {
        return false;
    }

    if (path[0] == '.' || path[0] == '~')
    {
        return false;
    }

    if (path.find("..") != std::string::npos)
    {
        return false;
    }

    return true;
}


std::string Path::normpath(const char* path)
{
    std::string result(path);
    toUnixSlash(result);

    abspath(result);
    return result;
}

void Path::normpath(std::string& path)
{
    toUnixSlash(path);

    abspath(path);
}


std::string Path::getCurDir()
{
    char buffer[MAX_PATH] = {0};
    return getcwd(buffer, MAX_PATH);
}

std::string Path::getWorkDir()
{
    char buffer[MAX_PATH] = {0};
    char szTmp[32];
    sprintf(szTmp, "/proc/%d/exe", getpid());
    int bytes = readlink(szTmp, buffer, MAX_PATH);
    if(bytes > 0)
    {
        return dirname(buffer);
    }
    return std::string();
}


bool Path::listDir(const std::string& dir, std::vector< std::string >& array)
{
    bool done = false;
    DIR* dp = opendir(dir.c_str());
    if (dp)
    {
        struct dirent* ep = readdir(dp);
        while (ep)
        {
            if ((ep->d_type & DT_DIR) != 0)
            {
                std::string name(ep->d_name);
                if (name == "." || name == "..")
                {
                    // pass
                }
                else
                {
                    array.push_back(name);
                }
            }
            ep = readdir(dp);
        }
        closedir(dp);
        done = true;
    }
    return done;
}

bool Path::listFile(const std::string& dir, std::vector< std::string >& array)
{
    bool done = false;
    DIR* dp = opendir(dir.c_str());
    if (dp)
    {
        struct dirent* ep = readdir(dp);
        while (ep)
        {
            if ((ep->d_type & DT_REG) != 0)
            {
                array.push_back(ep->d_name);
            }
            ep = readdir(dp);
        }
        closedir(dp);
        done = true;
    }
    return done;
}


std::string Path::getHomeDir()
{
    struct passwd *pw = getpwuid(getuid());

    return pw->pw_dir;
}



#endif //WIN32
///////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////


void Path::abspath(std::string& path)
{
    path = abspath(path.c_str());
}

std::string Path::basename(const char* path)
{
    std::string name;
    size_t length = strlen(path);
    size_t idx = rfind(path, length, PATH_SPERATOR);
    if (idx == npos)
    {
        // pass
    }
    else
    {
        name.assign(path + idx + 1);
    }
    return name;
}

std::string Path::basename(const std::string& path)
{
    return basename(path.c_str());
}

std::string Path::dirname(const char* path)
{
    std::string name;
    size_t length = strlen(path);
    size_t idx = rfind(path, length, PATH_SPERATOR);
    if (idx == npos)
    {
        name.assign(path, length);
    }
    else
    {
        name.assign(path, idx);
    }
    return name;
}

std::string Path::dirname(const std::string& path)
{
    return dirname(path.c_str());
}

std::string Path::join(const char* dir, const char* name)
{
    std::string path(dir);
    path.reserve(strlen(dir) + 1 + strlen(name));
    addSperator(path);
    path += name;
    return path;
}

std::string Path::join(const std::string& dir, const char* name)
{
    std::string path(dir);
    path.reserve(dir.size() + 1 + strlen(name));
    addSperator(path);
    path += name;
    return path;
}

std::string Path::join(const std::string& dir, const std::string& name)
{
    std::string path(dir);
    path.reserve(dir.size() + 1 + name.size());
    addSperator(path);
    path += name;
    return path;
}

std::string Path::getFileName(const char* path)
{
    return basename(path);
}

std::string Path::getFileName(const std::string& path)
{
    return basename(path);
}

/// get file extension, such as ".txt"
std::string Path::getFileExt(const char* filename)
{
    std::string result;
    size_t length = strlen(filename);
    size_t pos = rfind(filename, length, '.');
    if (pos == npos)
    {
        //
    }
    else
    {
        result.assign(filename + pos);
    }
    return result;
}

std::string Path::getFileExt(const std::string& filename)
{
    return getFileExt(filename.c_str());
}

std::string Path::getFileTitle(const char* filename)
{
    std::string result;
    size_t length = strlen(filename);
    size_t pos = rfind(filename, length, '.');
    if (pos == npos)
    {
        //
    }
    else
    {
        result.assign(filename, pos);
    }
    return result;
}

std::string Path::getFileTitle(const std::string& filename)
{
    return getFileTitle(filename.c_str());
}


bool Path::addSperator(std::string& path)
{
    if (path.empty())
    {
        return false;
    }

    bool done = false;
    if (path[path.length() - 1] != PATH_SPERATOR)
    {
        path += PATH_SPERATOR;
        done = true;
    }
    return done;
}

void Path::removeSperator(std::string& path)
{
    if (path.empty())
    {
        return;
    }

    if (path[path.length() - 1] != PATH_SPERATOR)
    {
        path.erase(path.length() - 1, 1);
    }
}

bool Path::endWithSperator(const char* path)
{
    size_t length = strlen(path);
    if (length == 0)
    {
        return false;
    }
    return (path[length - 1] == PATH_SPERATOR);
}

bool Path::endWithSperator(std::string& path)
{
    if (path.empty())
    {
        return false;
    }
    return (path[path.size() - 1] == PATH_SPERATOR);
}

bool Path::split(const char* path, char sp, std::string& dir, std::string& name)
{
    bool found = false;
    const char* p = strchr(path, sp);
    if (p == NULL)
    {
        dir.assign(path);
        name.clear();
    }
    else
    {
        dir.assign(path, p - path - 1);
        name.assign(p+1);
        found = true;
    }
    return  found;
}

bool Path::rsplit(const char* path, char sp, std::string& dir, std::string& name)
{
    bool found = false;
    size_t length = strlen(path);
    size_t pos = rfind(path, length, sp);
    if (pos == npos)
    {
        dir.assign(path);
        name.clear();
    }
    else
    {
        dir.assign(path, pos);
        name.assign(path + pos + 1);
        found = true;
    }
    return found;
}

void Path::toWindowsSlash(std::string& path)
{
    for (size_t i = 0; i < path.size(); ++ i)
    {
        if (path[i] == FORWARD_SLASH)
        {
            path[i] = BACKWARD_SLASH;
        }
    }
}

void Path::toUnixSlash(std::string& path)
{
    for (size_t i = 0; i < path.size(); ++ i)
    {
        if (path[i] == BACKWARD_SLASH)
        {
            path[i] = FORWARD_SLASH;
        }
    }
}


void Path::convertSlash(std::string& path)
{
#ifdef WIN32
	toWindowsSlash(path);
#else
	toUnixSlash(path);
#endif //WIN32
}


std::string Path::findFile(const std::string paths[], int size, const char* filename)
{
    std::string filepath;
    for (int i = 0; i < size; i ++)
    {
        filepath = join(paths[i], filename);
        abspath(filepath);
        if (filepath.empty())
        {
            continue;
        }

        if (exist(filepath))
        {
            return filepath;
        }
    }
    return std::string(filename);
}













}
