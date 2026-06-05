// # Copyright (C) 2021
// # Associated Universities, Inc. Washington DC, USA.
// #
// # This library is free software; you can redistribute it and/or modify it
// # under the terms of the GNU Library General Public License as published by
// # the Free Software Foundation; either version 2 of the License, or (at your
// # option) any later version.
// #
// # This library is distributed in the hope that it will be useful, but WITHOUT
// # ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// # FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// # License for more details.is
// #
// # You should have received a copy of the GNU Library General Public License
// # along with this library; if not, write to the Free Software Foundation,
// # Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
// #
// # Correspondence concerning this should be addressed as follows:
// #        Postal address: National Radio Astronomy Observatory
// #                        1003 Lopezville Road,
// #                        Socorro, NM - 87801, USA
// #
// # $Id$
import unittest
import os
import shutil
from pathlib import Path
import sys

sys.path.append('../../../install/lib/')

import hummbee2py
from helper_functions import compare_images_with_tolerance
#from casatools import image

# Define the tolerance and the expected gold value
tol = 0.1
goldPeakRes = 4.98845

# Test class for testing PeakRes calculations
class TestHummbee(unittest.TestCase):

    def setUp(self):
        # Get the test name
        self.testName = "runPythonTests"
        
        # Create a unique directory for this test case
        self.testDir = Path.cwd() / self.testName
        self.testDir.mkdir(parents=True, exist_ok=True)

        self.goldDir = Path.cwd() / "gold_standard/"
        # Copy files to the test directory
        shutil.copytree(self.goldDir / "unittest_hummbee.pb", self.testDir / "unittest_hummbee.pb")
        shutil.copytree(self.goldDir / "unittest_hummbee.psf", self.testDir / "unittest_hummbee.psf")
        shutil.copytree(self.goldDir / "unittest_hummbee.residual", self.testDir / "unittest_hummbee.residual")
        shutil.copytree(self.goldDir / "unittest_hummbee.sumwt", self.testDir / "unittest_hummbee.sumwt")

        # Set the current working directory to the test directory
        os.chdir(self.testDir)

        # Define parameters
        self.imageName = "unittest_hummbee"
        self.modelImageName = "unittest_hummbee.image"
        self.deconvolver = "asp"
        self.specmode = "cube"
        self.scales = []
        self.largestscale = -1
        self.fusedthreshold = 0
        self.nterms = 1
        self.gain = 0.1
        self.threshold = 1e-4
        self.nsigma = 1.5
        self.cycleniter = 10
        self.cyclefactor = 1.0
        self.mask = ["circle[[256pix,290pix],140pix]"]
        self.doPBCorr = False
        self.imagingMode = "deconvolve"

        # Call the function equivalent to Hummbee in Python
        self.PeakRes = hummbee2py.Hummbee(self.imageName, self.modelImageName,
                                          self.deconvolver,
                                          self.scales,
                                          self.largestscale, self.fusedthreshold,
                                          self.nterms,
                                          self.gain, self.threshold,
                                          self.nsigma,
                                          self.cycleniter, self.cyclefactor,
                                          self.mask, self.specmode,
                                          self.doPBCorr,
                                          self.imagingMode)

    def test_peak_res(self):
        self.assertAlmostEqual(self.PeakRes, goldPeakRes, delta=tol)


    def test_residual_value(self):
        self.assertTrue(compare_images_with_tolerance(self.goldDir/"unittest_hummbee_gold2.residual", "unittest_hummbee.residual"), tol)

    # can't do the following because hummbee2py internally 
    # has interaction with casa that conflict when both 
    # are loaded simultaneously.
    '''def test_residual_value(self):
        ia = image()
        ia.open("unittest_hummbee.model")
        model_result = ia.getchunk()

        print("Value at location ", model_result[275,330,0,0])
        ia.close()'''


    def tearDown(self):
        # Set the current working directory back to the parent dir
        os.chdir(self.testDir.parent)

        # Remove the test directory
        shutil.rmtree(self.testDir)


# Main entry point to run the tests
if __name__ == '__main__':
    unittest.main()
