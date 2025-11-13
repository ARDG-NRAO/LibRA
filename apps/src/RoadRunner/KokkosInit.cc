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
};

