import unittest
import os
import shutil
import numpy as np
from pathlib import Path
import sys

# Add the libra package to the path
sys.path.insert(0, str(Path(__file__).parent.parent))

import libra

# Get the functions from the dynamically imported modules
Asp2py = libra.asp2py.Asp2py
getchunk = libra.utilities2py.getchunk
ImageType = libra.utilities2py.ImageType


class TestAsp(unittest.TestCase):
    def setUp(self):
        self.test_name = 'casacore_asp_mfs'
        self.test_dir = Path.cwd() / self.test_name
        self.test_dir.mkdir(parents=True, exist_ok=True) 
        
        # Copy files
        self.goldDir = Path.cwd() / "gold_standard/"
        shutil.copytree(self.goldDir /'unittest_hummbee_mfs_revE.psf', self.test_dir / 'unittest_hummbee_mfs_revE.psf')
        shutil.copytree(self.goldDir /'unittest_hummbee_mfs_revE.mask', self.test_dir / 'unittest_hummbee_mfs_revE.mask')
        shutil.copytree(self.goldDir /'unittest_hummbee_mfs_revE.residual', self.test_dir / 'unittest_hummbee_mfs_revE.residual')
        
         # Change current working directory to the test directory
        os.chdir(self.test_dir)
        

    def test_asp_func_level(self):
        specmode = "cube"
        largestscale = -1
        fusedthreshold = 0
        nterms = 2
        gain = 0.1
        threshold = 1e-4
        nsigma = 1.5
        cycleniter = 10
        cyclefactor = 1.0
        psfwidth = 5.0
        nsigmathreshold = 1

        nx = 2
        ny = 5

        psfb = np.array([
            [1, 8, 12, 20, 25],
            [5, 9, 13, 24, 26]
        ])

        modelb = np.zeros((nx, ny))
        residualb = np.array([
            [1, 8, 12, -20, -25],
            [5, 9, -13, 24, 26]
        ])
        maskb = np.array([
            [1, 8, 0, 20, 25],
            [5, 9, 13, 24, 0]
        ])

        # Call the Asp function
        Asp2py(modelb, psfb, residualb, maskb,
            nx, ny,
            psfwidth,
            largestscale, fusedthreshold,
            nterms,
            gain, 
            threshold, nsigmathreshold,
            nsigma,
            cycleniter, cyclefactor,
            specmode
            )

        self.assertEqual(psfb[1, 0], 5.0)
        self.assertEqual(residualb[0, 3], -20.0)

    def test_getchunk(self):
        r1_result = getchunk("unittest_hummbee_mfs_revE", ImageType.RESIDUAL)
        self.assertAlmostEqual(r1_result[1072,1639,0,0], 12.110947, delta=0.01)


    def test_asp2py_mfs(self):
        specmode = "mfs"
        largestscale = -1
        fusedthreshold = 0.007
        nterms = 2
        gain = 0.2
        threshold = 2.6e-07
        nsigma = 0.0
        cycleniter = 3
        cyclefactor = 1.0

        psfwidth = 40.0 
        nsigmathreshold = 0
        nx = 4000
        ny = 4000
        
        # retrieve the input images
        image_name = "unittest_hummbee_mfs_revE"
        residual = getchunk(image_name, ImageType.RESIDUAL)
        psf = getchunk(image_name, ImageType.PSF)
        mask = getchunk(image_name, ImageType.MASK)
        model = np.zeros((nx, ny))


        Asp2py(model, psf[:, :, 0, 0], residual[:, :, 0, 0], mask[:, :, 0, 0],
            nx, ny,
            psfwidth,
            largestscale, fusedthreshold,
            nterms,
            gain, 
            threshold, nsigmathreshold,
            nsigma,
            cycleniter, cyclefactor,
            specmode
            )

        tol = 0.1
        res_gold_val_loc = 9.44497
        self.assertAlmostEqual(residual[1072, 1639, 0, 0], res_gold_val_loc, delta=tol)


    def tearDown(self):
        # Move to the parent directory and clean up
        os.chdir(os.path.dirname(self.test_dir))
        shutil.rmtree(self.test_dir)
    

        

if __name__ == '__main__':
    unittest.main()