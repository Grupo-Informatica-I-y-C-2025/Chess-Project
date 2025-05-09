#include "fileUtils.h"
#include <sys/stat.h>  // UNIX


#ifdef _WIN32
#include <windows.h>
#include <stringapiset.h> // WideCharToMultiByte
#else
#include <dirent.h>
#endif

#ifdef _WIN32
#define MKDIR(dir) _mkdir(dir)
#else
#define MKDIR(dir) mkdir(dir, 0733)
#endif

namespace FileUtils {

    bool directoryExists(const std::string& path) {
        struct stat info;
        if (stat(path.c_str(), &info) != 0) {
            return false;
        }
        else if (info.st_mode & S_IFDIR) {
            return true;
        }
        else {
            return false;
        }
    }

    bool createDirectory(const std::string& path) {
        if (directoryExists(path)) {
            return true;
        }
        return MKDIR(path.c_str()) == 0;
    }

    std::vector<std::string> listDirectory(const std::string& path) {
        std::vector<std::string> entries;

#ifdef _WIN32
        WIN32_FIND_DATAW findData;
        HANDLE hFind = FindFirstFileW((std::wstring(path.begin(), path.end()) + L"\\*").c_str(), &findData);

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                std::wstring wname = findData.cFileName;
                if (wname != L"." && wname != L"..") {
                    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wname.c_str(), (int)wname.length(), NULL, 0, NULL, NULL);
                    std::string name(size_needed, 0);
                    WideCharToMultiByte(CP_UTF8, 0, wname.c_str(), (int)wname.length(), &name[0], size_needed, NULL, NULL);
                    entries.push_back(name);
                }
            } while (FindNextFileW(hFind, &findData) != 0);
            FindClose(hFind);
        }
#else
        DIR* dir = opendir(path.c_str());
        if (dir != nullptr) {
            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                std::string name = entry->d_name;
                if (name != "." && name != "..") {
                    entries.push_back(name);
                }
            }
            closedir(dir);
        }
#endif
        return entries;
    }

    std::string getProjectPath() {
        return "./";
    }
}
