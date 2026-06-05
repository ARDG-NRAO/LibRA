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


