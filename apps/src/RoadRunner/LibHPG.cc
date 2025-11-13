//# LibHPG.h: Definition of the LibHPG class and the getHPGDevice() function
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
#define LIBRA_USE_HPG
#include <LibHPG.h>
std::vector<KokkosInterop::K_BACKEND> LibHPG::global_initialized_kokkos_backends;
//
//-------------------------------------------------------------------------
//
std::vector<KokkosInterop::K_BACKEND> getHPGDevice()
{
  std::vector<KokkosInterop::K_BACKEND> hpgDevice;

#ifdef LIBRA_USE_HPG
  // LibRA uses OpenMP backend internally with CUDA or Seiral
  // backends.  So always initialize it.
  hpgDevice.push_back(KokkosInterop::K_BACKEND::K_OPENMP);

  string hpgDeviceStr=casa::refim::SynthesisUtils::getenv("HPGDEVICE",String("libra_cuda"));

  if ((hpgDeviceStr == "libra_omp")   || (hpgDeviceStr == "opium"))
    {
      // Already pushed on hpgDevice
    }
  else if ((hpgDeviceStr == "libra_cuda")   || (hpgDeviceStr == "curd") || (hpgDeviceStr == ""))
    {
      hpgDevice.push_back(KokkosInterop::K_BACKEND::K_CUDA);
    }
  else if ((hpgDeviceStr == "libra_serial") || (hpgDeviceStr == "cereal"))
    {
      hpgDevice.push_back(KokkosInterop::K_BACKEND::K_SERIAL);
    }
  else
    {
      std::string msg="Supported backend names: \"curd or libra_cuda\" for CUDA\n"
                      "                         \"cereal or libra_serial\" for Serial\n"
                      "                         \"opium or libra_omp\" for OpenMP";
      throw(
	    std::runtime_error("Incorrect setting for the HPGDEVICE env. var.: " +
			       hpgDeviceStr + "\n" + msg)
	    );
    }
#endif

  return hpgDevice;
}
//
// Initialize the public list of Kokkos backends initialized by LibHPG.
//
bool LibHPG::initialize()
  {
    startTime = std::chrono::steady_clock::now();

#ifdef LIBRA_USE_HPG
    if (init_hpg)
      {
	std::vector<KokkosInterop::K_BACKEND> hpgDevices = getHPGDevice();

	static bool init_once = [&]
			   {
			     // Save the list of initialized backends.
			     // This list is also accessible from the
			     // global function
			     // KokkosInterop::initialized_kokkos_backends().
			     //
			     std::vector<KokkosInterop::K_BACKEND> initialized_backends = KokkosInterop::kokkos_initialize(hpgDevices);
			     // Were all Kokkos backends initialized, in the order they were requested?
			     bool kokkos_initialized=(initialized_backends == hpgDevices);

			     // Were all Kokkos backends initialized, in the order they were requested?
			     // bool kokkos_initialized=(LibHPG::global_initialized_kokkos_backends == hpgDevices);

			     LogIO log_l(LogOrigin("LibHPG","initialize()"));
			     if (kokkos_initialized)
			       {
				 LibHPG::global_initialized_kokkos_backends= initialized_backends;

				 log_l << "Intialized the following Kokkos backends: ";
				 for(auto dev : initialized_backends)
				   log_l << KokkosInterop::kokkos_backend_to_str(dev) << " ";
				 log_l << LogIO::POST;

#ifdef LIBRA_USE_HPG
				 // Do HPG initialization without (re-)initializing Kokkos (which is initialized above).
				 if (!hpg::is_initialized()) hpg::initialize(false);
				 hpg_initialized = (kokkos_initialized && hpg::is_initialized());
#endif
				 if (kokkos_initialized) std::atexit(KokkosInterop::kokkos_finalize);
			       }
			     else
			       log_l << "Kokkos initialization failed" << LogIO::EXCEPTION << LogIO::POST;
			     return hpg_initialized;
			   }();
	// To be re-enterant in a single process, derive the status of
	// Kokkos and HPG initilization from the static variable.
	// This variable is assured to be initialized only once in a
	// process.
	hpg_initialized = init_once;
	return hpg_initialized;
      }
    //     if (init_hpg)
    //       {
    // 	static bool once = [&]
    // 			   {
    // 			     assert(!hpg::is_finalized());
    // 			     if (!hpg::is_initialized())
    // 			       {
    // 				 hpg_initialized=hpg::initialize();
    // 				 // Kokkos::push_finalize_hook([]() {
    // 				 // 	 cerr << "Kokkos::finalize() called" << endl;
    // 				 // });
    // 				 std::atexit(hpg::finalize);
    // 			       }
    // 			     return hpg_initialized;
    // 			   }();
    //       } 
    //    assert(hpg::is_intialized);
#else //LIBRA_USE_HPG
    hpg_initialized=true;
#endif //LIBRA_USE_HPG
    return hpg_initialized;
  };

