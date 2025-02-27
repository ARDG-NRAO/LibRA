#include <regex>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <casacore/images/Images/PagedImage.h>
#include <utils.h>


using namespace std;
using namespace casacore;
namespace utils
{
    bool imageExists(const std::string &imagename)
    {
        Directory image(imagename);
        return image.exists();
    }

    void printImageMax(const std::string &name, LogIO &logio)
    {
        PagedImage<float> target(name);
        checkImageMax(name, target, logio);
    }

    void printImageMax(const vector<string> &nameList, LogIO &logio)
    {
        for (auto name : nameList)
        {
            PagedImage<float> target(name);
            checkImageMax(name, target, logio);
        }
    }
    std::string removeExtension(const std::string& path) {
        if (path == "." || path == "..")
          return path;
      
        size_t pos = path.find_last_of("\\/.");
        if (pos != std::string::npos && path[pos] == '.')
          return path.substr(0, pos);
      
        return path;
      }
}; // namespace utils
