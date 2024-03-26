import os

from skbuild import setup

# Get the current working directory
cwd = os.getcwd()

# Define the install directory
build_dir = os.path.join(cwd, '_skbuild/linux-x86_64-3.12/cmake-build/')
install_dir = os.path.join(cwd, '_skbuild/linux-x86_64-3.12/cmake-install/libra/')


setup(
    name="libra",
    version="0.1.0",
    description="Libra is a library for imaging radio interferometry data",
    author='Sanjay Bhatnagar, Preshanth Jagannathan, Genie Hsieh, and others',
    author_email='pjaganna@nrao.edu ',
    license='GPL3',
    include_package_data=True,
    packages=['libra'],
    cmake_args=['-DCMAKE_BUILD_TYPE=Release',  f'-DBUILD_DIR={build_dir}', f'-DINSTALL_DIR={install_dir}'],
    cmake_source_dir='.',
    cmake_install_dir='_skbuild/linux-x86_64-3.12/cmake-install/libra/',
    cmake_install_target='Apps'
)