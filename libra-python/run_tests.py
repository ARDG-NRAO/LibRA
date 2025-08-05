#!/usr/bin/env python3
"""
LibRA Test Runner

Runs all tests in the LibRA test suite.
"""

import sys
import unittest
import os
from pathlib import Path

def main():
    # Add the parent directory to the path so we can import libra
    test_dir = Path(__file__).parent
    sys.path.insert(0, str(test_dir))
    
    # Change to the test directory
    os.chdir(test_dir / "tests")
    
    # Discover and run all tests
    loader = unittest.TestLoader()
    suite = loader.discover('.', pattern='test_*.py')
    
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Return exit code based on results
    return 0 if result.wasSuccessful() else 1

if __name__ == '__main__':
    sys.exit(main())