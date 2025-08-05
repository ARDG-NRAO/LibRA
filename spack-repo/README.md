# LibRA Spack Repository

This is a local Spack repository containing package definitions for LibRA and its dependencies.

## Repository Structure

```
spack-repo/
├── repo.yaml                    # Repository configuration
├── README.md                    # This file
└── packages/
    ├── libra/package.py         # Main LibRA package
    ├── hpg/package.py           # HPG dependency
    ├── parafeed/package.py      # Parafeed dependency
    └── libsakura/package.py     # LibSakura dependency (optional)
```

## Setup Instructions

### 1. Add the repository to Spack

```bash
# Navigate to your LibRA directory
cd /home/pjaganna/Software/libra

# Add the local repository to Spack
spack repo add spack-repo
```

### 2. Verify the repository is added

```bash
# List all repositories
spack repo list

# You should see the libra repository listed
```

### 3. Install dependencies before casacore

Install the dependencies in order:

```bash
# Install Kokkos (if not already available in spack)
spack install kokkos@4.6.01 +openmp+serial

# Install FFTW (if not already available in spack)
spack install fftw@3.3.10 +openmp

# Install our custom packages
spack install parafeed@1.1.4
spack install hpg@3.0.3

# Install pybind11 (available in spack)
spack install py-pybind11@2.10.2

# Install googletest (available in spack)
spack install googletest@1.14.0
```

### 4. Install casacore

```bash
# Install casacore (available in spack)
spack install casacore@3.7.1
```

### 5. Install LibRA

```bash
# Install LibRA with default options
spack install libra

# Or with specific variants:
spack install libra+sakura+openmp~cuda+tests
```

## Available Variants

### LibRA variants:
- `+sakura`: Enable libsakura support (default: False)
- `+exodus`: Enable exodus support (default: False)
- `+cuda`: Enable CUDA support (default: False)
- `+openmp`: Enable OpenMP support (default: True)
- `+tests`: Build tests (default: False)

### HPG variants:
- `+cuda`: Enable CUDA device (default: False)
- `+openmp`: Enable OpenMP device (default: False)
- `+serial`: Enable serial device (default: True)
- And many others for precision and array configurations

### Parafeed variants:
- `cxxstd`: C++ standard to use (default: "11")

## Example Installation Commands

```bash
# Basic installation
spack install libra

# Installation with CUDA support
spack install libra+cuda ^kokkos+cuda ^hpg+cuda

# Installation with all features
spack install libra+sakura+openmp+tests

# Installation for development
spack install libra+tests ^googletest
```

## Using in Spack Environment

You can also create a Spack environment for LibRA development:

```bash
# Create a new environment
spack env create libra-dev

# Activate the environment
spack env activate libra-dev

# Add packages to the environment
spack add libra+tests+openmp
spack add casacore@3.7.1

# Install all packages in the environment
spack install
```

## Troubleshooting

### If packages are not found:
1. Ensure the repository is properly added: `spack repo list`
2. Try refreshing the package index: `spack reindex`

### If builds fail:
1. Check dependencies are installed: `spack find`
2. Look at build logs: `spack install --verbose libra`
3. Check for missing system dependencies

### To remove the repository:
```bash
spack repo remove /home/pjaganna/Software/libra/spack-repo
```

## Notes

- The libsakura package uses placeholder SHA256 values - you'll need to update these with actual checksums
- Some system dependencies (cfitsio, blas, lapack, etc.) should be available via your system package manager or Spack
- CUDA support requires proper CUDA installation and compatible GPU