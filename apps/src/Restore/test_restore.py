import unittest
import os
import shutil
import numpy as np
from pathlib import Path
import sys

sys.path.append('../../../install/lib/')

import restore2py
import utilities2py


class TestAsp(unittest.TestCase):
    def setUp(self):
        self.test_name = 'restore2py_testdir'
        self.test_dir = Path.cwd() / self.test_name
        self.test_dir.mkdir(parents=True, exist_ok=True) 
        
        # Copy files 
        self.goldDir = Path.cwd() / "gold_standard/"
        shutil.copytree(self.goldDir /'unittest_hummbee_mfs_revE_restore.psf', self.test_dir / 'unittest_hummbee_mfs_revE_restore.psf')
        shutil.copytree(self.goldDir /'unittest_hummbee_mfs_revE_restore.residual', self.test_dir / 'unittest_hummbee_mfs_revE_restore.residual')
        shutil.copytree(self.goldDir /'unittest_hummbee_mfs_revE_restore.sumwt', self.test_dir / 'unittest_hummbee_mfs_revE_restore.sumwt')
        shutil.copytree(self.goldDir /'unittest_hummbee_mfs_revE_restore.weight', self.test_dir / 'unittest_hummbee_mfs_revE_restore.weight')
        shutil.copytree(self.goldDir /'unittest_hummbee_mfs_revE_restore.model', self.test_dir / 'unittest_hummbee_mfs_revE_restore.model')
        shutil.copytree(self.goldDir /'unittest_hummbee_mfs_revE_restore_gold.image', self.test_dir / 'unittest_hummbee_mfs_revE_restore_gold.image')
        shutil.copytree(self.goldDir /'unittest_hummbee_mfs_revE_restore.pb', self.test_dir / 'unittest_hummbee_mfs_revE_restore.pb')
        shutil.copytree(self.goldDir /'unittest_hummbee_mfs_revE_restore_gold.image.pbcor', self.test_dir / 'unittest_hummbee_mfs_revE_restore_gold.image.pbcor')
         # Change current working directory to the test directory
        os.chdir(self.test_dir)


    def test_restore2py_func_level(self):
        nx = 2
        ny = 5

        model = np.array([
            [1, 2, 3, 4, 5],
            [6, 7, 8, 9, 10]
            ], dtype=np.float32)

        residual = np.array([
            [1, -1, 1, -1, 1],
            [-1, 1, -1, 1, -1]
            ], dtype=np.float32)

        image = np.zeros((nx, ny), dtype=np.float32)
        pb = np.zeros((nx, ny), dtype=np.float32)
        image_pbcor = np.zeros((nx, ny), dtype=np.float32)

        refi = 1.0
        refj = 1.0
        inci = 0.5
        incj = 0.5
        majaxis = 2.0
        minaxis = 1.0
        pa = 0.0
        pbcor = False

        restore2py.Restore2py(
            model,
            residual,
            image,
            size_x=nx,
            size_y=ny,
            refi=refi,
            refj=refj,
            inci=inci,
            incj=incj,
            majaxis=majaxis,
            minaxis=minaxis,
            pa=pa,
            pbcor=pbcor,
            pb=pb,
            image_pbcor=image_pbcor
        )

        # Define tolerance and expected values
        tol = 0.01
        gold_val_loc1 = 19.6208
        gold_val_loc2 = 28.8573

        # Assertions
        assert np.isclose(image[0][1], gold_val_loc1, atol=tol), f"Value at image[1][0] is {image[1][0]}, expected {gold_val_loc1}"
        assert np.isclose(image[0][2], gold_val_loc2, atol=tol), f"Value at image[0][1] is {image[0][1]}, expected {gold_val_loc2}"


    def test_restore2py_mfs_pbcor(self):        
        nx = 4000
        ny = 4000
        refi = 2000
        refj = 2000
        inci = 1.21203e-07
        incj = 1.21203e-07
        majaxis = 4.90554e-06
        minaxis = 4.67228e-06
        pa = 2.76764
        pbcor = True

        image_name = "unittest_hummbee_mfs_revE_restore"
        residual = utilities2py.getchunk(image_name, utilities2py.ImageType.RESIDUAL)
        model = utilities2py.getchunk(image_name, utilities2py.ImageType.MODEL)
        image = np.zeros((nx, ny), dtype=np.float32)
        pb = utilities2py.getchunk(image_name, utilities2py.ImageType.PB)
        image_pbcor = np.zeros((nx, ny), dtype=np.float32)


        restore2py.Restore2py(
            model[:, :, 0, 0],
            residual[:, :, 0, 0],
            image,
            size_x=nx,
            size_y=ny,
            refi=refi,
            refj=refj,
            inci=inci,
            incj=incj,
            majaxis=majaxis,
            minaxis=minaxis,
            pa=pa,
            pbcor=pbcor,
            pb=pb,
            image_pbcor=image_pbcor
        )

        tol = 0.01
        im_gold_val_loc1 = 0.2126
        im_gold_val_loc2 = 0.07974
        self.assertAlmostEqual(im_gold_val_loc1, image[1072][1639], delta=tol);
        self.assertAlmostEqual(im_gold_val_loc2, image[3072][2406], delta=tol);
        
        impbcor_gold_val_loc1 = 0.580613
        impbcor_gold_val_loc2 = 0.30845
        self.assertAlmostEqual(impbcor_gold_val_loc1, image_pbcor[1072][1639], delta=tol);
        self.assertAlmostEqual(impbcor_gold_val_loc2, image_pbcor[3072][2406], delta=tol);
        
         
    def tearDown(self):
        # Move to the parent directory and clean up
        os.chdir(os.path.dirname(self.test_dir))
        shutil.rmtree(self.test_dir)
    

        

if __name__ == '__main__':
    unittest.main()
