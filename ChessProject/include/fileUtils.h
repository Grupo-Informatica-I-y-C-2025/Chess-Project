#pragma once
#include <string>
#include <vector>


namespace FileUtils {
    bool directoryExists(const std::string& path);

    bool createDirectory(const std::string& path);

    std::vector<std::string> listDirectory(const std::string& path);


    std::string getProjectPath();
}
