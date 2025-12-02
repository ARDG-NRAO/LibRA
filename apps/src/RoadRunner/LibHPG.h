//# LibHPG.h: Definition of the LibHPG class
//# Copyright (C) 2025
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning this should be addressed as follows:
//#        Postal address: National Radio Astronomy Observatory
//#                        1003 Lopezville Road,
//#                        Socorro, NM - 87801, USA
//#
//# $Id$

// This has direct Kokkos dependence, and provides the KokkosIterop
// namespace.
#ifndef LIBRA_LIBHPG_H
#define LIBRA_LIBHPG_H
#include <KokkosInit.h>

#include <casacore/casa/Logging/LogIO.h>
#include <stdexcept>
#include <synthesis/TransformMachines2/Utils.h>
#include <iomanip>

#ifdef LIBRA_USE_HPG
#include <hpg/hpg.hpp>
#endif

using namespace casacore;
using namespace std;
//
//-------------------------------------------------------------------------
// Class to manage the HPG initialize/finalize scope.
// hpg::finalize() in the destructor.  This also reports, via the
// destructor, the time elapsed between contruction and destruction.
//
/**
 * @class LibHPG
 * @brief Manages the HPG initialize/finalize scope.
 *
 * This class is responsible for initializing and finalizing the HPG library. 
 * It also reports the time elapsed between construction and destruction.
 */
class LibHPG
{
public:
  /**
   * @brief Constructor for the LibHPG class.
   * @param usingHPG A boolean indicating whether to use HPG. Default is true.
   */
  static std::vector<KokkosInterop::K_BACKEND> global_initialized_kokkos_backends;

  LibHPG(const bool usingHPG=true, std::ostream* os=nullptr):
    init_hpg(usingHPG),hpg_initialized(false),os_p(os)
  {
    if (!initialize())
      {
	LogIO log_l(LogOrigin("LibHPG","LibHPG"));
	log_l << "Failed to initialized HPG" << LogIO::EXCEPTION << LogIO::POST;
      };
  }
  /**
   * @brief Initializes the HPG library.
   * @return A boolean indicating whether the initialization was successful.
   */
  bool initialize();

  //
  // @brief Return the internal state variable to indicate if the
  // hpg::initialize() method was called.
  //
  bool hpg_intialize_called() {return hpg_initialized;}
  
  //
  // @brief Return the total time elasped since the class constructor was called.
  //
  double runtime()
  {
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> totalRuntime = endTime - startTime;
    return totalRuntime.count();
  }
  /**
   * @brief Destructor for the LibHPG class.
   *
   * If an output stream is give, this also prints the total runtime
   * on that stream.
   */
  ~LibHPG()
  {
    if (os_p != nullptr)
      (*os_p) << "LibHPG: Total runtime: "
	      << std::fixed << std::setprecision(2)
	      << runtime() << " sec" << std::endl;
  };

  std::chrono::time_point<std::chrono::steady_clock> startTime;
  bool init_hpg;
  bool hpg_initialized;
  std::ostream *os_p;
};
#endif
