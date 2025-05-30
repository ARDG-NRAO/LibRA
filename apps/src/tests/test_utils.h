#pragma once

#include <filesystem>
#include <gtest/gtest.h>
#include <unistd.h>  // for gethostname()



inline  bool directoryExists(const std::string& path) {
    std::filesystem::path dir(path);
    return std::filesystem::is_directory(dir);
}
inline  bool create_directory(const std::filesystem::path& path) {
    return std::filesystem::create_directory(path);
}
inline  bool remove_directory(const std::filesystem::path& path) {
    return std::filesystem::remove_all(path);
}

inline  bool IsGpuHost003() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        return false;  
    }
    return std::string(hostname) == "gpuhost003";
}


