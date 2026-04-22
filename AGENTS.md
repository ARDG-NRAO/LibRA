# AGENTS.md

## Purpose

This repository builds LibRA, a radio-astronomy algorithms library plus command-line apps.
The top-level build drives a set of external dependencies with `ExternalProject_Add`, then
builds the LibRA libraries and apps on top of those dependencies.

This file documents repository-specific facts that were learned while working in this tree,
especially the current state of building on macOS.

## Repository Layout

- `src/`: algorithm and library code derived from CASA/casacore-related sources.
- `apps/src/`: command-line apps such as `roadrunner`, `dale`, `chip`, `hummbee`, `coyote`.
- `apps/src/tests/`: C++ unit/integration tests for the apps.
- `cmake/`: top-level configuration and dependency recipes.
- `cmake/dependencies/`: `ExternalProject` definitions for casacore, FFTW, Kokkos, HPG, parafeed, pybind11, etc.
- `dependencies/`: checked-out or unpacked external dependencies produced by the build.
- `build/`: external dependency build trees. This is repo-root, not per-build-dir.
- `install/`: install prefix for external dependencies and built artifacts. This is repo-root, not per-build-dir.

## Important Build-System Quirks

- `cmake/paths.cmake` hardcodes:
  - `BUILD_DIR=${CMAKE_CURRENT_SOURCE_DIR}/build`
  - `INSTALL_DIR=${CMAKE_CURRENT_SOURCE_DIR}/install`
- That means even if you configure into `build-mac-gcc/` or another out-of-tree build directory,
  external dependency builds still spill into repo-root `build/` and `install/`.
- The top-level build expects to own `dependencies/`, `build/`, and `install/`.
- Several external projects need the chosen compiler explicitly propagated; this repo now does that
  via `LIBRA_EXTERNAL_CMAKE_COMPILER_ARGS` and `LIBRA_EXTERNAL_AUTOTOOLS_COMPILER_ENV` in
  `cmake/LibraConfig.cmake`.

## Test Data

- App tests depend on the `apps/src/tests/gold_standard` submodule.
- The submodule is declared in `.gitmodules` and points to `../libra-data.git`.
- If tests are needed, initialize it explicitly:

```bash
git submodule update --init --recursive apps/src/tests/gold_standard
```

- Tests are off by default (`Apps_BUILD_TESTS=OFF`).

## macOS Status

The upstream `README.md` only says macOS might work and is untested. In practice:

- a CPU-only build is the practical macOS baseline
- Apple Clang now works end-to-end on macOS
- Homebrew GCC also works and remains useful for comparison/debugging

### macOS Prerequisites

Assumed tools/libraries:

- Xcode Command Line Tools
- Homebrew
- Homebrew packages:

```bash
brew install cmake gcc bison libomp cfitsio wcslib gsl
```

Notes:

- Homebrew `bison` is required. Apple `/usr/bin/bison` is too old for `parafeed`.
- Homebrew `libomp` is required for Apple Clang builds.
- `flex` from the Apple toolchain has been sufficient so far.
- BLAS/LAPACK were picked up from Accelerate on macOS during the casacore build.

## Recommended macOS Apple Clang Configure Command

Use a clean CPU-only configuration first:

```bash
PATH="/opt/homebrew/opt/bison/bin:$PATH" \
cmake -S . -B build-mac-clang \
  -DCMAKE_PREFIX_PATH=/opt/homebrew \
  -DLIBRA_ENABLE_CUDA_BACKEND=OFF \
  -DApps_BUILD_TESTS=OFF \
  -DCASACORE_DATA_DOWNLOAD=OFF
```

Then build:

```bash
PATH="/opt/homebrew/opt/bison/bin:$PATH" \
cmake --build build-mac-clang -- -j1
```

As of 2026-04-09, this Apple Clang path completes successfully end to end and installs into the
repo-root `install/` prefix.

## Recommended macOS GCC Configure Command

Use a clean CPU-only configuration first:

```bash
PATH="/opt/homebrew/opt/bison/bin:$PATH" \
cmake -S . -B build-mac-gcc \
  -DCMAKE_PREFIX_PATH=/opt/homebrew \
  -DCMAKE_C_COMPILER=/opt/homebrew/bin/gcc-15 \
  -DCMAKE_CXX_COMPILER=/opt/homebrew/bin/g++-15 \
  -DCMAKE_Fortran_COMPILER=/opt/homebrew/bin/gfortran-15 \
  -DLIBRA_ENABLE_CUDA_BACKEND=OFF \
  -DApps_BUILD_TESTS=OFF \
  -DCASACORE_DATA_DOWNLOAD=OFF
```

Then build:

```bash
PATH="/opt/homebrew/opt/bison/bin:$PATH" \
cmake --build build-mac-gcc -- -j1
```

Serial `-j1` is the most useful mode while bringing up macOS because the first real failure stays
visible. Once the tree is stable, parallelism can be increased.

For isolating dependency failures, building `Casacore` serially is useful:

```bash
PATH="/opt/homebrew/opt/bison/bin:$PATH" \
cmake --build build-mac-gcc --target Casacore -- -j1
```

## Local casacore Source Cache

The casacore dependency recipe prefers an existing local checkout before falling back to GitHub.
Current preference order in `cmake/dependencies/Casacore.cmake`:

1. `$HOME/SoftwareProjects/casa/casatools/casacore`
2. `$HOME/SoftwareProjects/casacore`
3. upstream GitHub repository

This is useful on macOS because direct network cloning of casacore was unreliable and slow in
earlier runs.

## Current macOS GCC Findings

As of 2026-04-09, the following are known:

- `parafeed` builds under Homebrew GCC once Homebrew `bison` is on `PATH`.
- `pybind11` needs a CMake 4.x compatibility workaround; that is already in this tree.
- `FFTW`, `Kokkos`, and `HPG` can be driven successfully far enough under GCC to stop being the
  first blocker.
- `HPG` previously tried to build bundled googletest even with tests disabled; this tree contains a
  patch step to suppress that.
- the full top-level GCC/macOS build now completes successfully, including:
  - dependencies
  - `Casacpp`
  - app libraries and CLI binaries
  - pybind11 Python modules
  - `Frameworks`
  - `Scripts`

### casacore-specific macOS fixes already present

The tree contains macOS/GCC-specific adjustments for casacore:

- A local clone helper is used instead of the default CMake git download step.
- `HostInfo.cc` is patched so `_XOPEN_SOURCE 600` is not forced on Apple builds.
- `HostInfoDarwin.h` is patched for Darwin compatibility.
- GCC builds on macOS add:
  - `-D_Static_assert=static_assert`
  - `-U_GLIBCXX_HAVE_AT_QUICK_EXIT`
  - `-U_GLIBCXX_HAVE_QUICK_EXIT`
- casacore C files are forced to `-std=gnu17` to avoid newer-dialect breakage in legacy C code.

### macOS/GCC fixes in this tree beyond casacore

The tree also contains non-casacore fixes that were needed for a successful macOS/GCC build:

- external project compiler propagation into `Casacpp` and `Apps`
- Apple-specific handling for GNU `ld` flags such as:
  - `--push-state --as-needed --pop-state`
  - `--disable-new-dtags`
  - `-z,lazy`
- explicit FFTW linkage for `src/synthesis` on macOS
- missing direct library links in:
  - `src/libracore`
  - `src/librautils`
  - `src/components`
  - `apps/src/translation`
- a GCC-visible move-assignment issue in `src/msvis/MSVis/statistics/Vi2DataProvider.h`
- Darwin-safe hostname handling in `apps/src/Coyote/coyote.cc`
- Darwin-safe memory accounting fallback in
  `src/synthesis/ImagerObjects/SynthesisUtilMethods.cc`

### Current state of the GCC build

As of 2026-04-09, the repository builds successfully end-to-end on macOS with Homebrew GCC using:

```bash
PATH="/opt/homebrew/opt/bison/bin:$PATH" \
cmake --build build-mac-gcc -- -j1
```

Installed artifacts land under repo-root `install/`, including:

- shared libraries in `install/lib`
- CLI apps in `install/bin`
- pybind11 modules in `install/lib`

## Clang Status

As of 2026-04-09, the repository also builds successfully end-to-end on macOS with default Apple
Clang using:

```bash
PATH="/opt/homebrew/opt/bison/bin:$PATH" \
cmake --build build-mac-clang -- -j1
```

Important Apple Clang-specific findings from the working path:

- `libomp` from Homebrew must be installed and propagated into external projects.
- `Kokkos` needs explicit OpenMP hints under Apple Clang.
- `FFTW` must not use its autotools OpenMP flavor under Apple Clang; the pthread/threaded libs are
  the working path.
- `HPG` must be pointed at those Apple-Clang-built FFTW thread libraries explicitly.
- `casacore` needs Apple-specific OpenMP include/link flags and local source patching for:
  - `HostInfo`
  - `HostInfoDarwin.h`
  - `Array2Math.cc`
- `parafeed` needs Homebrew `bison` plus a small patch to `clgetBaseCode.h`.
- the main tree still needs libc++ compatibility fixes for removed standard adaptors such as:
  - `std::unary_function`
  - `std::binary_function`
  - `std::bind2nd`
  - `std::mem_fun`, `std::mem_fun_ref`, `std::not1`

The current Clang build uses Apple Clang for C/C++ and Homebrew `gfortran` for Fortran.

## Practical Tips

- Prefer `rg` for search.
- If an external dependency seems to fail in parallel, retry that dependency serially before
  assuming the source tree is broken.
- If tests are enabled, make sure `apps/src/tests/gold_standard` is initialized first.
- `CASACORE_DATA_DOWNLOAD=OFF` avoids pulling ephemerides data during initial macOS build work.
- The install location for built binaries is under repo-root `install/`, not inside the
  per-build-dir tree.
