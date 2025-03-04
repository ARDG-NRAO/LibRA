#include <filesystem>
#include <gtest/gtest.h>

using namespace std;
using namespace std::filesystem;

namespace test{
  bool directoryExists(const std::string& path) {
    std::filesystem::path dir(path);
    return std::filesystem::is_directory(dir);
  }
  bool create_directory(const std::filesystem::path& path) {
    return std::filesystem::create_directory(path);
  }
  bool remove_directory(const std::filesystem::path& path) {
    return std::filesystem::remove_all(path);
  }

}