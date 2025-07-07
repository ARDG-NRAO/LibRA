# LibRA Python Package

This directory contains a clean Python package structure for the LibRA radio astronomy toolkit.

## Directory Structure

```
python/
├── libra/                      # Main LibRA Python package
│   ├── __init__.py            # Package initialization and .so imports
│   ├── asp.py                 # ASP deconvolution wrapper
│   ├── hummbee.py             # Hummbee deconvolution wrapper  
│   ├── restore.py             # Image restoration wrapper
│   ├── utilities.py           # Utilities (getchunk, ImageType)
│   └── helper_functions.py    # Test helper functions
├── tests/                      # Test suite
│   ├── __init__.py            # Test package initialization
│   ├── test_asp.py            # ASP tests
│   ├── test_hummbee.py        # Hummbee tests
│   ├── test_restore.py        # Restore tests
│   └── gold_standard/         # Test data (copied from apps/src/tests)
├── reports/                    # Test reports (created by CI)
├── run_tests.py               # Test runner script
└── README.md                  # This file
```

## Module Mapping

The package wraps the compiled C++ extensions:

- `libra.asp` → `asp2py.Asp2py`
- `libra.hummbee` → `hummbee2py.Hummbee`  
- `libra.restore` → `restore2py.Restore2py`
- `libra.utilities` → `utilities2py.getchunk`, `utilities2py.ImageType`

## Usage

### Running Tests

From the python directory:
```bash
python3 run_tests.py
```

Or using pytest directly:
```bash
cd tests
python3 -m pytest test_*.py -v
```

### Importing Modules

```python
from libra.asp import Asp2py
from libra.hummbee import Hummbee  
from libra.restore import Restore2py
from libra.utilities import getchunk, ImageType
```

## CI Integration

The GitLab CI now:
1. Builds the C++ extensions to `install/lib/*.so`
2. Runs C++ tests and saves results to `python/reports/report.xml`
3. Sets up PYTHONPATH to find the .so files
4. Copies test data from `apps/src/tests/gold_standard`
5. Runs Python tests and saves results to `python/reports/pytest_report.xml`

## Requirements

- The .so files must be built and available in `install/lib/`
- Test data must be available in `gold_standard/` directory
- Python 3.x with numpy and pytest