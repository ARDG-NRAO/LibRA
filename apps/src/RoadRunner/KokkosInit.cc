/* Copyright 2024 Stanford University, NVIDIA Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <KokkosInit.h>
namespace KokkosInterop
{
#if KOKKOS_VERSION < 50000
  //#ifdef LIBRA_NONSTD_KOKKOS_INITIALIZE
  // This code allows initializing only the requested Kokkos backends.
  //
  // The default kokkos::initialize() function initializes *all*
  // enabled backends, even if only one of them is used at runtime.
  // This requires that, e.g., if CUDA backend is enabled at compile
  // time, but only SERIAL backend is used it run time, a CUDA-enabled
  // actual device be available at runtime (even if it is not used),
  // or else kokkos::initialize() fails.
  //
  // However, the per-backend Kokkos::*::impl_initialize() methods are
  // not part of the Kokkos public API and therefore not guaranteed to
  // be supported.  Indeed, with Kokkos 5.x, these are even more
  // difficult to access, if not impossible.
  //
  // Explaination (from the General channel on Kokkkos SLACK) of the
  // pitfalls of not initializing (at run time) the backends that are
  // not used:
  //
  // "Like if you don't enable cuda in a cuda build all our
  // default stuff doesn't work. You can't call kokkos_malloc(n) you
  // have to specify the memory space. Cuda space initialization might
  // rely on certain host capabikities being available already Certain
  // ops which you might not recognize as explicitly involving an
  // execution space may expect certain backends to be initialized,
  // and we assume they are as soon as kokkos globally is initialized
  // Kokkos::View<int*> a(ptr, n) would potentially not work if you
  // don't enable cuda because we use cuda functions to check whether
  // the ptr is compatible with the defaulted memory space type of a
  // And there are no seatbelts from our side since we only know a
  // "kokkos is initialized or not" state ". @crtrott, Nov. 13, 2025
  //
    std::string kokkos_backend_to_str(const KokkosInterop::K_BACKEND& dev)
    {
      std::string str;
      if      (dev == KokkosInterop::K_BACKEND::K_CUDA)   str="K::Cuda";
      else if (dev == KokkosInterop::K_BACKEND::K_SERIAL) str="K::Serial";
      else if (dev == KokkosInterop::K_BACKEND::K_OPENMP) str="K::OpenMP";
      else if (dev == KokkosInterop::K_BACKEND::K_HIP)    str="K::HIP";

      return str;
    }

    //
    //------------------------------------------------------------------
    // Input: A list of Kokkos backends to initialize.
    // Output: A list of Kokkos backends that were successfully initialized.
    //
    std::vector<KokkosInterop::K_BACKEND>
    kokkos_initialize(const std::vector<KokkosInterop::K_BACKEND>& K_Device)
    {
      std::vector<KokkosInterop::K_BACKEND> list_of_backends;

      // use Kokkos::Impl::{pre,post}_initialize to allow us to do our own
      //  execution space initialization
      Kokkos::InitializationSettings kokkos_init_args;
      //os << "doing general pre-initialization" << endl;
      Kokkos::Impl::pre_initialize(kokkos_init_args);

      for(auto BE : K_Device)
	switch(BE)
	  {
#ifdef KOKKOS_ENABLE_SERIAL
	  case K_SERIAL:
	    {
	      Kokkos::Serial::impl_initialize(kokkos_init_args);
	      if (Kokkos::Serial::impl_is_initialized())
		list_of_backends.push_back(K_SERIAL);
	      break;
	    };
#endif
#ifdef KOKKOS_ENABLE_OPENMP
	  case K_OPENMP:
	    {
	      KokkosOpenMPInitializer ompinit;
	      if (ompinit.execute_on_processor())
		list_of_backends.push_back(K_OPENMP);
	      ompinit.wait_done();
	      break;
	    };
#endif
#ifdef KOKKOS_ENABLE_CUDA
	  case K_CUDA:
	    {
	      KokkosCudaInitializer cudainit;
	      if (cudainit.execute_on_processor())
		list_of_backends.push_back(K_CUDA);
	      cudainit.wait_done();
	      break;
	    };
#endif
#ifdef KOKKOS_ENABLE_HIP
	  case K_HIP:
	    {
	      KokkosHIPInitializer hipinit;
	      if (hipinit.execute_on_processor())
		list_of_backends.push_back(K_HIP);
	      hipinit.wait_done();
	      break;
	    };
#endif
	  };
      // TODO: warn if Kokkos has other execution spaces enabled that we're not
      //  willing/able to initialize?

      //os << "doing general post-initialization" << endl;
      Kokkos::Impl::post_initialize(kokkos_init_args);
      //global_initialized_kokkos_backends = list_of_backends;
      return list_of_backends;
    }
    //
    //------------------------------------------------------------------
    //
    void kokkos_finalize()
    {
#if KOKKOS_VERSION >= 40000
      Kokkos::Impl::pre_finalize();
#endif

      // !!!! Does the Serial backend also need finalization?

#ifdef KOKKOS_ENABLE_OPENMP
      if (Kokkos::OpenMP::impl_is_initialized())
	{
	  KokkosOpenMPFinalizer ompfinal;
	  ompfinal.execute_on_processor();
	  ompfinal.wait_done();
	}
#endif
      
#ifdef KOKKOS_ENABLE_CUDA
      if (Kokkos::Cuda::impl_is_initialized())
	{
	  KokkosCudaFinalizer cudafinal;
	  cudafinal.execute_on_processor();
	  cudafinal.wait_done();
	}
#endif
      
#ifdef KOKKOS_ENABLE_HIP
      if (Kokkos::HIP::impl_is_initialized())
	{
	  KokkosHIPFinalizer hipfinal;
	  hipfinal.execute_on_processor();
	  hipfinal.wait_done();
	}
#endif

      //      cerr << "doing general finalization" << endl;
// #if KOKKOS_VERSION >= 40000
//       Kokkos::Impl::post_finalize();
// #else
//       Kokkos::finalize();
// #endif
    }
#endif
};

