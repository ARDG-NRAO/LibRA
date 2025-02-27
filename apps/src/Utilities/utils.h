// # Copyright (C) 2021
// # Associated Universities, Inc. Washington DC, USA.
// #
// # This library is free software; you can redistribute it and/or modify it
// # under the terms of the GNU Library General Public License as published by
// # the Free Software Foundation; either version 2 of the License, or (at your
// # option) any later version.
// #
// # This library is distributed in the hope that it will be useful, but WITHOUT
// # ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// # FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// # License for more details.is
// #
// # You should have received a copy of the GNU Library General Public License
// # along with this library; if not, write to the Free Software Foundation,
// # Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
// #
// # Correspondence concerning this should be addressed as follows:
// #        Postal address: National Radio Astronomy Observatory
// #                        1003 Lopezville Road,
// #                        Socorro, NM - 87801, USA
// #
// # $Id$
#ifndef UTILS_H
#define UTILS_H

#include <casacore/images/Images/PagedImage.h>
#include <regex>
#include <string>
#include <iostream>
#include <casacore/casa/OS/Directory.h>


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


#endif