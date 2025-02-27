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

/**
 * @file utils.h
 * @brief Utility functions for image processing.
 * 
 * This file contains various utility functions for handling and processing images.
 * 
 * @namespace utils
 * @brief Namespace for utility functions.
 * 
 * @tparam T The data type of the image.
 * 
 * @function getImageType
 * @brief Retrieves the type and subtype of a PagedImage.
 * @param im The PagedImage object.
 * @param type The type of the image.
 * @param subType The subtype of the image.
 * 
 * @function isNormalized
 * @brief Checks if a PagedImage is normalized.
 * @param im The PagedImage object.
 * @return True if the image is normalized, false otherwise.
 * 
 * @function setNormalized
 * @brief Sets the subtype of a PagedImage to "normalized".
 * @param im The PagedImage object.
 * 
 * @function setImType
 * @brief Sets the type and subtype of a PagedImage.
 * @param im The PagedImage object.
 * @param type The type to set.
 * @param subType The subtype to set.
 * 
 * @function imageExists
 * @brief Checks if an image exists.
 * @param imagename The name of the image.
 * @return True if the image exists, false otherwise.
 * 
 * @function printImageMax
 * @brief Prints the maximum value of an image.
 * @param name The name of the image.
 * @param logio The LogIO object for logging.
 * 
 * @function printImageMax
 * @brief Prints the maximum value of a list of images.
 * @param nameList The list of image names.
 * @param logio The LogIO object for logging.
 * 
 * @function removeExtension
 * @brief Removes the extension from a file path.
 * @param path The file path.
 * @return The file path without the extension.
 * 
 * @function checkImageMax
 * @brief Checks the maximum value of an image and logs it.
 * @param name The name of the image.
 * @param target The ImageInterface object.
 * @param logio The LogIO object for logging.
 * @throws AipsError if the image amplitude is out of range.
 * 
 * @function printImageMax
 * @brief Prints the maximum value of a PagedImage.
 * @param name The name of the image.
 * @param image The PagedImage object.
 * @param logio The LogIO object for logging.
 * 
 * @function resetImage
 * @brief Resets an image to zero.
 * @param target The ImageInterface object.
 * 
 * @function addImages
 * @brief Adds multiple images to a target image.
 * @param target The target ImageInterface object.
 * @param partImageNames The list of part image names.
 * @param imExt The image extension.
 * @param reset_target Flag to reset the target image before adding.
 * @param logio The LogIO object for logging.
 */

#ifndef UTILS_H
#define UTILS_H

#include <casacore/images/Images/PagedImage.h>
#include <regex>
#include <string>
#include <iostream>
#include <casacore/casa/OS/Directory.h>
#include <iomanip>

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

    template <class T>
    void setImType(PagedImage<T> &im, const std::string &type, const std::string &subType)
    {
        im.table().tableInfo().setType(type);
        im.table().tableInfo().setSubType(subType);
        im.table().flushTableInfo();
    }

    bool imageExists(const std::string &imagename);
    void printImageMax(const std::string &name, LogIO &logio);
    void printImageMax(const std::vector<std::string> &nameList, LogIO &logio);
    std::string removeExtension(const std::string &path);
    std::string getExtension(const std::string &path);

    template <class T>
    void checkImageMax(const std::string &name, const ImageInterface<T> &target, LogIO &logio)
    {
        T mim = max(target.get());

        {
            std::stringstream os;
            os << std::fixed << std::setprecision(std::numeric_limits<T>::max_digits10)
               << "Image stats: "
               << " max(" << name << ") = " << mim;
            logio << os.str() << LogIO::POST;
        }

        T threshold = static_cast<T>(1e20);
        if (mim >= threshold)
            throw AipsError("Image amplitude is out of range.");
    }


    template <class T>
    void printImageMax(const std::string &name, const PagedImage<T> &image, LogIO &logio)
    {
        checkImageMax(name, image, logio);
    }

    template <class T>
    void resetImage(ImageInterface<T> &target)
    {
        target.set(0.0);
    }

    template <class T>
    void addImages(ImageInterface<T> &target, const vector<string> &partImageNames, const string &imExt, const bool &reset_target, LogIO &logio)
    {
        if (reset_target)
            resetImage(target);

        for (auto partName : partImageNames)
        {
            if (imageExists(partName + imExt))
                target += PagedImage<T>(partName + imExt);
            else
                logio << "Image " << partName + imExt << " does not exist." << LogIO::EXCEPTION;
        }
    }
} // namespace utils

#endif