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
#ifndef LIBRA_KOKKOSINIT_H
#define LIBRA_KOKKOSINIT_H
#include <Kokkos_Core.hpp>
#include <stdlib.h>

  namespace KokkosInterop {
    //
    // ENUM for the available Kokkos backends.
    //
    enum K_BACKEND {
      K_CUDA=0,
      K_OPENMP,
      K_SERIAL,
      K_HIP
    };

    std::string kokkos_backend_to_str(const KokkosInterop::K_BACKEND& dev);

    std::vector<KokkosInterop::K_BACKEND>
    kokkos_initialize(const std::vector<KokkosInterop::K_BACKEND>& K_Device);
 
    void kokkos_finalize();

#ifdef KOKKOS_ENABLE_CUDA
    inline bool is_kokkos_cuda_initialized(void)
    {
	return Kokkos::Cuda::impl_is_initialized();
    };
#endif
#ifdef KOKKOS_ENABLE_HIP
    inline bool is_kokkos_hip_initialized(void)
    {
	return Kokkos::HIP::impl_is_initialized();
    };
#endif
#ifdef KOKKOS_ENABLE_OPENMP
    inline bool is_kokkos_openmp_initialized(void)
    {
	return Kokkos::OpenMP::impl_is_initialized();
    };
#endif

#ifdef KOKKOS_ENABLE_SERIAL
    inline bool is_kokkos_serial_initialized(void)
    {
	return Kokkos::Serial::impl_is_initialized();
    };
#endif

    inline bool is_kokkos_cuda_enabled(void)
    {
#ifdef KOKKOS_ENABLE_CUDA
      return true;
#else
      return false;
#endif
    }

    inline bool is_kokkos_hip_enabled(void)
    {
#ifdef KOKKOS_ENABLE_HIP
      return true;
#else
      return false;
#endif
    }

    inline bool is_kokkos_openmp_enabled(void)
    {
#ifdef KOKKOS_ENABLE_OPENMP
      return true;
#else
      return false;
#endif
    }
    //
    //------------------------------------------------------------------
    // Keeping this as a baseclass for Kokkos*{Initializer,Finalizer}
    // class below.
    class KokkosInternalTask
    {
    public:
      KokkosInternalTask() {};
      void mark_done() {};
      void wait_done() {};
    };
    //
    //------------------------------------------------------------------
    //
    class KokkosSerialInitializer : public KokkosInternalTask
    {
    public:
      virtual bool execute_on_processor()
      {
	//cerr << "doing serial init on proc " << endl;

        Kokkos::InitializationSettings init_settings;
        Kokkos::Serial::impl_initialize(init_settings);
	mark_done();
	return is_kokkos_serial_initialized();
      }
    };
    //
    //------------------------------------------------------------------
    //
#ifdef KOKKOS_ENABLE_OPENMP
    class KokkosOpenMPInitializer : public KokkosInternalTask
    {
    public:
      virtual bool execute_on_processor()
      {
        Kokkos::InitializationSettings init_settings;
	init_settings.set_num_threads(-1);
        Kokkos::OpenMP::impl_initialize(init_settings);

	mark_done();
	return is_kokkos_openmp_initialized();
      }
    };
    
    class KokkosOpenMPFinalizer : public KokkosInternalTask
    {
    public:
      virtual void execute_on_processor()
      {
	Kokkos::OpenMP::impl_finalize();
	mark_done();
      }
    };
#endif
    //
    //------------------------------------------------------------------
    //
#ifdef KOKKOS_ENABLE_CUDA
    class KokkosCudaInitializer : public KokkosInternalTask
    {
    public:
      virtual bool execute_on_processor()
      {
        Kokkos::InitializationSettings init_settings;
        // init_settings.set_device_id(gpu->gpu->info->index);
        // init_settings.set_num_devices(1);
        Kokkos::Cuda::impl_initialize(init_settings);

	mark_done();
	return is_kokkos_cuda_initialized();
      }
    };
    
    class KokkosCudaFinalizer : public KokkosInternalTask
    {
    public:
      virtual void execute_on_processor()
      {
	Kokkos::Cuda::impl_finalize();
	mark_done();
      }
    };
#endif
    //
    //------------------------------------------------------------------
    // CAUTION: HIP initialization/finalization has not been tested.
    //
#ifdef KOKKOS_ENABLE_HIP
    class KokkosHIPInitializer : public KokkosInternalTask
    {
    public:
      virtual bool execute_on_processor()
      {
        Kokkos::InitializationSettings init_settings;
        // init_settings.set_device_id(gpu->gpu->info->index);
        // init_settings.set_num_devices(1);
        Kokkos::HIP::impl_initialize(init_settings);

	mark_done();
	return is_kokkos_hip_initialized();
      }
    };

    class KokkosHipFinalizer : public KokkosInternalTask
    {
    public:
      virtual void execute_on_processor()
      {
	Kokkos::HIP::impl_finalize();
	mark_done();
      }
    };
#endif
  }; //namespace KokkosInterop 
#endif
