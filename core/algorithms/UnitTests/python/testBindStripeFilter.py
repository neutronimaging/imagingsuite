import sys
from os import makedirs
import numpy as np
import skimage.io as io
import math

sys.path.append('../../../../../deployed')

from nitools.imgalg import imgalg as ia 
import nitools.utils.readers as rd

import unittest


class TestStripeFilter(unittest.TestCase):
    data_path = "../../../../../TestData/"
    def test_constructor(self):
        sf=ia.StripeFilter([100,110],"daub7",2,0.01)
        self.assertAlmostEqual(sf.sigma(),0.01) #, rel_tol=1e-5)
        self.assertEqual(sf.decompositionLevels(),2)
        self.assertEqual(sf.dims()[0],100)
        self.assertEqual(sf.dims()[1],110)
        self.assertEqual(sf.waveletName(),"daub7")

    def test_configure(self):
        sino = io.imread('../../UnitTests/data/woodsino_0200.tif')

        sf=ia.StripeFilter([100,110],"daub7",2,0.01)
        sf.configure([sino.shape[1],sino.shape[0]],"daub9", 3, 0.02)
        self.assertAlmostEqual(sf.sigma(),0.02) #, rel_tol=1e-5)
        self.assertEqual(sf.decompositionLevels(), 3)
        self.assertEqual(sf.dims()[0],             sino.shape[1])
        self.assertEqual(sf.dims()[1],             sino.shape[0])
        self.assertEqual(sf.waveletName(),         "daub9")

    def test_basic_process(self) :
        sino = io.imread('../../UnitTests/data/woodsino_0200.tif')

        sf2=ia.StripeFilter([sino.shape[1],sino.shape[0]],"daub7",2,0.1)

        s2=sino.copy().astype("float32")
        sf2.process(s2,ia.eStripeFilterOperation.VerticalComponentFFT)
        mse=((sino-s2)**2).mean()

        self.assertAlmostEqual(mse,1.8129678e-05) #,rel_tol=1e-6)

        makedirs('result',exist_ok=True)
        rd.save_TIFF('result/cleanedsino.tif',s2)
        rd.save_TIFF('result/originalsino.tif',sino)

    def test_process_stack(self) :
        proj = rd.read_images(self.data_path+"2D/tiff/tomo/04_ct5s375_128lines/ct5s_{0:05d}.tif",first=1, last=376).astype('float32')

        print("proj.shape=",proj.shape)

        sf2=ia.StripeFilter([proj.shape[2],proj.shape[0]],"daub7",2,0.1)

        p2=proj.copy().astype("float32")
        sf2.process(p2,ia.eStripeFilterOperation.VerticalComponentFFT)
        mse=((proj-p2)**2).mean()

        # self.assertAlmostEqual(mse,627.64905) #,rel_tol=1e-6)

        makedirs('result',exist_ok=True)
        rd.save_TIFF('result/cleanedsinos_{0:05d}.tif',p2)
        rd.save_TIFF('result/originalsinos_{0:05d}.tif',proj)       


if __name__ == '__main__':
    unittest.main()
    print("StripeFilter tests done")
    