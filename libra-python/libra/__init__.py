"""
LibRA Python Package

This package provides Python bindings for the LibRA radio astronomy toolkit.
It imports the compiled C++ extensions and provides a clean Python interface.
"""

import sys
import os
import glob
from pathlib import Path

# Add the install/lib directory to sys.path to find the .so files
# This assumes the package is run from the project root
install_lib_path = Path(__file__).parent.parent.parent / "install" / "lib"
if install_lib_path.exists():
    sys.path.insert(0, str(install_lib_path))

# Dynamically import all *2py*.so modules
imported_modules = {}
failed_imports = []

if install_lib_path.exists():
    # Find all *2py*.so files
    so_files = glob.glob(str(install_lib_path / "*2py*.so"))
    
    for so_file in so_files:
        # Extract module name (e.g., asp2py from asp2py.cpython-313-x86_64-linux-gnu.so)
        module_name = Path(so_file).name.split('.')[0]
        
        try:
            # Import the module dynamically
            module = __import__(module_name)
            imported_modules[module_name] = module
            
            # Add to global namespace
            globals()[module_name] = module
            
        except ImportError as e:
            failed_imports.append(f"{module_name}: {e}")

# Report import status
if imported_modules:
    print(f"✅ Successfully imported {len(imported_modules)} LibRA modules:")
    for name in sorted(imported_modules.keys()):
        print(f"   - {name}")

if failed_imports:
    print(f"⚠️  Failed to import {len(failed_imports)} modules:")
    for failure in failed_imports:
        print(f"   - {failure}")

# Re-export all successfully imported modules
__all__ = list(imported_modules.keys())