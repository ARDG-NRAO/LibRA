#include <regex>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <casacore/images/Images/PagedImage.h>
#include <Utilities/utils.h>


using namespace std;
using namespace casacore;
namespace utils
{
    template <class T>
    void getImageType(PagedImage<T> &im,
                      std::string &type, std::string &subType)
    {
        type = im.table().tableInfo().type();
        subType = im.table().tableInfo().subType();
    }
    //
    //-------------------------------------------------------------------------
    //
    template <class T>
    bool isNormalized(PagedImage<T> &im)
    {
        // Regular expression to match the token "normalized" as a word.
        std::regex rx(R"(\bnormalized\b)");
        string type, subType;
        getImageType(im, type, subType);

        std::smatch m;
        return std::regex_search(subType, m, rx);
    }
    //
    //-------------------------------------------------------------------------
    //
    template <class T>
    void setNormalized(PagedImage<T> &im)
    {
        string type, subType;
        getImageType(im, type, subType);
        im.table().tableInfo().setSubType(subType + " normalized");
        im.table().flushTableInfo();
    }

} // namespace utils
