# ARDG Spack Repository

Radio astronomy optimized Spack packages from the Atacama Radio Data Group (ARDG).

## What We Provide

**ARDG Namespace Packages:**
- `ardg.libra` - LibRA radio astronomy processing library  
- `ardg.hpg` - High Performance Gridding library (not in mainline Spack)
- `ardg.casacore` - Enhanced casacore with v3.7.1 and dysco improvements
- `ardg.wcslib` - WCS library with proper compiler dependencies

**From Mainline Spack:**  
- `kokkos` - Parallel computing framework (comprehensive architecture support)
- `fftw` - Fast Fourier Transform library
- `parafeed` - Parallel feed simulation  
- `cfitsio`, `boost`, `blas`, `lapack`, `pybind11`, etc.

## Installation Guide

### 1. Environment Setup

```bash
# Create dedicated environment
spack env create libra-dev
spack env activate libra-dev

# Add the ARDG repository  
spack repo add /path/to/ardg/spack_repo/ardg
```

### 2. General Installation Workflow

```bash
# 1. Create environment and add repo
spack env create libra-dev
spack env activate libra-dev
spack repo add /path/to/ardg/spack_repo/ardg

# 2. Test with one-liner spec first
spack spec "ardg.libra +cuda" "^ardg.casacore@3.7.1" "^ardg.wcslib@8.4+cfitsio" "^kokkos+cuda+cuda_lambda+wrapper cuda_arch=61"

# 3a. If spec works - install directly
spack install "ardg.libra +cuda" "^ardg.casacore@3.7.1" "^ardg.wcslib@8.4+cfitsio" "^kokkos+cuda+cuda_lambda+wrapper cuda_arch=61"

# 3b. If spec fails - use individual adds approach
spack add "ardg.libra +cuda"
spack add "ardg.casacore@3.7.1"
spack add "ardg.wcslib@8.4+cfitsio"
spack add "kokkos+cuda+cuda_lambda+wrapper cuda_arch=61"
spack concretize
# If concretize passes:
spack install
```

**Optional: Specify compiler version if needed**
```bash
# Add %gcc@13.3 to any command if you need a specific compiler version
spack spec "ardg.libra +cuda" "^ardg.casacore@3.7.1" "^ardg.wcslib@8.4+cfitsio" "^kokkos+cuda+cuda_lambda+wrapper cuda_arch=61" "%gcc@13.3"
```

### 3. Installation Examples by Architecture

#### Intel/AMD x64 with NVIDIA Pascal (GTX 1060, GTX 1080, etc.)
```bash
# Use cuda_arch=61 and add CUDA version constraint for Pascal
spack spec "ardg.libra +cuda" "^ardg.casacore@3.7.1" "^ardg.wcslib@8.4+cfitsio" "^kokkos+cuda+cuda_lambda+wrapper cuda_arch=61" "^cuda@:12.9"
```

#### Intel/AMD x64 with NVIDIA Ampere (RTX 3080, A100, etc.)
```bash  
spack spec "ardg.libra +cuda" "^ardg.casacore@3.7.1" "^ardg.wcslib@8.4+cfitsio" "^kokkos+cuda+cuda_lambda+wrapper cuda_arch=80"
```

#### Intel/AMD x64 with NVIDIA Hopper (H100, etc.)
```bash
spack spec "ardg.libra +cuda" "^ardg.casacore@3.7.1" "^ardg.wcslib@8.4+cfitsio" "^kokkos+cuda+cuda_lambda+wrapper cuda_arch=90"
```

#### ARM64 Grace CPU + Hopper GPU (Grace Hopper Superchip)
```bash
spack install ardg.libra +cuda ^ardg.casacore@3.7.1 ^ardg.wcslib+cfitsio ^kokkos+cuda+openmp+cuda_lambda cuda_arch=90 target=neoverse_v2
```

#### ARM64 CPU Only (Graviton, ThunderX2, etc.)
```bash
# ThunderX2
spack install ardg.libra +openmp ~cuda ^ardg.casacore@3.7.1 ^ardg.wcslib+cfitsio ^kokkos+openmp~cuda target=thunderx2

# Graviton3  
spack install ardg.libra +openmp ~cuda ^ardg.casacore@3.7.1 ^ardg.wcslib+cfitsio ^kokkos+openmp~cuda target=neoverse_v1
```

#### CPU-Only (Any Architecture)
```bash
spack install ardg.libra +openmp ~cuda ^ardg.casacore@3.7.1 ^ardg.wcslib+cfitsio ^kokkos+openmp~cuda
```

### 3. Available LibRA Variants

- `+cuda` - Enable CUDA GPU acceleration  
- `+openmp` - Enable OpenMP threading (default: true)
- `+all` - Enable all Kokkos backends (CUDA + OpenMP + Serial)
- `+tests` - Build test suite
- `+exodus` - Enable Exodus bundle support

### 4. Architecture-Specific Notes

**CUDA Architecture Selection:**
- `60` - GTX 10 series, Titan X Pascal
- `61` - GTX 1050, GTX 1060, GTX 1070, GTX 1080, Tesla P40
- `70` - Tesla V100  
- `75` - RTX 20 series
- `80` - RTX 30 series, A100
- `86` - RTX 3060, RTX 3050
- `90` - H100, H200

**ARM CPU Targets:**
- `thunderx2` - Marvell ThunderX2
- `neoverse_v1` - AWS Graviton3, Ampere Altra
- `neoverse_v2` - NVIDIA Grace CPU  
- `a64fx` - Fujitsu A64FX

**CUDA Version Constraints:**
- Pascal architectures: CUDA ≤ 12.9 (hardware limitation)
- Newer architectures: No upper CUDA limit

## Advanced Examples

### Development Environment with Tests
```bash
spack env create libra-test
spack env activate libra-test
spack install ardg.libra +tests +openmp ~cuda ^ardg.casacore@3.7.1 ^ardg.wcslib+cfitsio ^kokkos+openmp~cuda
```

### Multi-GPU Grace Hopper System  
```bash
spack install ardg.libra +cuda +openmp ^ardg.casacore@3.7.1 ^ardg.wcslib+cfitsio ^kokkos+cuda+openmp+cuda_lambda cuda_arch=90 target=neoverse_v2 ^cuda@12.0:
```

### Debugging/Profiling Build
```bash  
spack install ardg.libra +cuda +tests ^ardg.casacore@3.7.1 ^ardg.wcslib+cfitsio ^kokkos+cuda+cuda_lambda+debug cuda_arch=80 build_type=Debug
```

## Verification

```bash
# Check installation
spack find ardg.libra

# Load and test
spack load ardg.libra  
# Run your LibRA applications

# Check Kokkos configuration
spack load kokkos
python -c "import sys; print('Kokkos loaded successfully')"
```

## Troubleshooting

**CUDA Version Conflicts:**
- Ensure CUDA version matches architecture capabilities
- Pascal cards require `^cuda@:12.9`

**ARM Build Issues:**
- Verify target architecture: `spack arch -t`
- Some ARM systems need explicit `target=` specification

**Missing Dependencies:**
```bash
spack graph ardg.libra  # Visualize dependency tree
spack spec -I ardg.libra +cuda  # Check what will be installed
```

**Repository Issues:**
```bash
spack repo list  # Verify ardg repo is listed
spack reindex   # Refresh package cache if needed
```