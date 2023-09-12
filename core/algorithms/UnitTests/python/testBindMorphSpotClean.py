import sys
from os import makedirs
import numpy as np
import skimage.io as io
import math

sys.path.append('../../../../../deployed')

from nitools.imgalg import imgalg as ia 
import nitools.utils.readers as rd

import unittest


class TestMorphSotClean(unittest.TestCase):
    data_path = "../../../../../TestData/"
    def test_constructor(self):
        msc=ia.MorphSpotClean()
        self.assertEqual(msc.cleanMethod(),     ia.eMorphCleanMethod.MorphCleanReplace, "Should be replace")
        self.assertEqual(msc.detectionMethod(), ia.eMorphDetectionMethod.MorphDetectHoles, "Should be DetectoHoles")
        limits=msc.clampLimits()
        self.assertTrue(math.isclose(limits[0],-0.1,rel_tol=1e-07))
        self.assertTrue(msc.isThreaded())
        self.assertEqual(limits[1],7.0, "Should be 7")
        self.assertFalse(msc.clampActive())
        self.assertEqual(msc.maxArea(),100, "Should be 100")
        self.assertEqual(msc.edgeConditionLength(),9)

    def test_set_parameters(self):
        # Tests with parameter changes
        msc=ia.MorphSpotClean()
        msc.setCleanMethod(ia.eMorphDetectionMethod.MorphDetectPeaks, ia.eMorphCleanMethod.MorphCleanFill)
        assert msc.cleanMethod()     == ia.eMorphCleanMethod.MorphCleanFill
        assert msc.detectionMethod() == ia.eMorphDetectionMethod.MorphDetectPeaks



        msc.setLimits(True, -2, 10, 1000)
        limits=msc.clampLimits()
        assert math.isclose(limits[0],-2,rel_tol=1e-07)
        assert limits[1]==10
        assert msc.clampActive() == True
        assert msc.maxArea()     == 1000


    def test_detection_image_float(self) :
        msc=ia.MorphSpotClean()
        msc.setCleanMethod(ia.eMorphDetectionMethod.MorphDetectHoles, ia.eMorphCleanMethod.MorphCleanReplace)
        msc.setLimits(False,-0.1,7.0,100)

        fimg=io.imread('../../UnitTests/data/spotprojection_0001.tif').astype('float32')

        di=msc.detectionImage(fimg,True)
        self.assertEqual(len(di),2, "The output dict shall have two entries")
        self.assertTrue("dark" in di)
        self.assertTrue("bright" in di)

    def test_detection_image_double(self) :
        msc=ia.MorphSpotClean()
        msc.setCleanMethod(ia.eMorphDetectionMethod.MorphDetectHoles, ia.eMorphCleanMethod.MorphCleanReplace)
        msc.setLimits(False,-0.1,7.0,100)

        dimg=io.imread('../../UnitTests/data/spotprojection_0001.tif').astype('float64')

        di=msc.detectionImage(dimg,True)
        self.assertEqual(len(di),2, "The output dict shall have two entries")
        self.assertTrue("dark" in di)
        self.assertTrue("bright" in di)
        

    # def test_process_image_double_2D(self) :
    #     msc=ia.MorphSpotClean()
    #     dimg=io.imread('../../UnitTests/data/spotprojection_0001.tif').astype('float64')

    #     ci=dimg.copy()
    #     msc.process(ci,[0.95,0.95],[0.01,0.01])
    #     assert ci.shape == dimg.shape

    # def test_process_image_float_2D(self) :
    #     msc=ia.MorphSpotClean()
    #     dimg=io.imread('../../UnitTests/data/spotprojection_0001.tif').astype('float32')

    #     ci=dimg.copy()
    #     msc.process(ci,[0.3,0.2],[0.01,0.0005])
    #     assert ci.shape == dimg.shape

    # def test_process_image_double_3D(self) :
    #     proj = rd.read_images(self.data_path+"2D/tiff/tomo/04_ct5s375_128lines/ct5s_{0:05d}.tif",first=1, last=10).astype('float64')
    #     cleaned = proj.copy()

    #     msc=ia.MorphSpotClean()
    #     print(cleaned.shape)
    #     msc.setCleanMethod(ia.eMorphDetectionMethod.MorphDetectBoth, ia.eMorphCleanMethod.MorphCleanReplace)
    #     msc.process(cleaned,th=[0.95,0.95],sigma=[0.01,0.01]) 
    #     makedirs('result',exist_ok=True)
    #     rd.save_TIFF('result/spot_double_{0:05d}.tif',cleaned)

    def test_process_image_float_3D(self) :
        proj = rd.read_images(self.data_path+"2D/tiff/tomo/04_ct5s375_128lines/ct5s_{0:05d}.tif",first=1, last=10).astype('float32')
        cleaned = proj.copy()

        # <clampdata>false</clampdata>
        # <cleanmethod>morphcleanfill</cleanmethod>
        # <connectivity>conn4</connectivity>
        # <detectionmethod>morphdetectallspots</detectionmethod>
        # <edgesmooth>5</edgesmooth>
        # <maxarea>30</maxarea>
        # <maxlevel>12</maxlevel>
        # <minlevel>-0.1</minlevel>
        # <removeinfnan>false</removeinfnan>
        # <sigma>0.01 0.01</sigma>
        # <threading>true</threading>
        # <threshold>0.9 0.9</threshold>
        # <thresholdbyfraction>true</thresholdbyfraction>
        # <transpose>false</transpose>
        msc=ia.MorphSpotClean()
        msc.useThreading(True)
        msc.setThresholdByFraction(True)
        msc.setLimits(applyClamp=False,vmin=-0.1,vmax=12.0,maxarea=30)

        msc.setCleanMethod(ia.eMorphDetectionMethod.MorphDetectAllSpots, ia.eMorphCleanMethod.MorphCleanFill)
        msc.process(cleaned,th=[0.95,0.95],sigma=[0.01,0.01]) 
        makedirs('result',exist_ok=True)
        rd.save_TIFF('result/spot_float_{0:05d}.tif',cleaned)

if __name__ == '__main__':
    unittest.main()
    
    print("MorphSpotClean tests done")
