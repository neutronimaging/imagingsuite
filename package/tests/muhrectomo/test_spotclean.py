import pymuhrec as pm 
import pytest
import numpy as np
import utils.readers as rd
import os

class TestSpotClean:

    @pytest.fixture(autouse=True)
    def spotclean(self):
        # print("Setup")
        self.spotclean = pm.MorphSpotClean()

        self.path  = os.path.dirname(os.path.abspath(__file__))+'/'
        os.makedirs(self.path + 'output/spotclean', exist_ok=True)
        self.data_path = self.path + "../../../../../TestData/"
        self.img = rd.read_image(self.data_path + "2D/tiff/normalized_projection.tif")

        self.imgs = rd.read_images(self.data_path + "2D/tiff/tomo/04_ct5s375_128lines/ct5s_{0:05d}.tif", first=1, last=5)
        dc   = rd.read_image(self.data_path + "2D/tiff/tomo/04_ct5s375_128lines/dc_00001.tif")
        ob   = rd.read_image(self.data_path + "2D/tiff/tomo/04_ct5s375_128lines/ob_00001.tif")
        
        normalize = pm.NormalizeImage(True)
        normalize.setReferences(ob,dc)
        normalize.process(self.imgs)

        

    def test_spotclean_fixture(self):
        # When Then Expect
        assert self.spotclean is not None

    def test_spotclean_basic_run(self):
        rd.save_TIFF(self.path+"output/spotclean/spotty.tif",self.img)
        self.spotclean.setCleanMethod(detectionMethod=pm.MorphDetectAllSpots, cleanMethod=pm.MorphCleanReplace)
        assert self.spotclean.detectionMethod() == pm.MorphDetectAllSpots
        assert self.spotclean.cleanMethod() == pm.MorphCleanReplace
        strip = self.img.copy() 
        self.spotclean.process(strip,th=[0.99, 0.99],sigma=[0.01, 0.01])

        assert strip is not None
        assert np.array_equal(strip.shape,self.img.shape)
        rd.save_TIFF(self.path+"output/spotclean/spotcleaned.tif",strip)

    def test_spotclean_stack(self):
        rd.save_TIFF(self.path+"output/spotclean/spotty_{0:05d}.tif",self.imgs)
        cproj = self.imgs.copy()
        self.spotclean.setCleanMethod(detectionMethod=pm.MorphDetectAllSpots, cleanMethod=pm.MorphCleanReplace)
        self.spotclean.process(cproj,th=[0.99, 0.99],sigma=[0.01, 0.01])

        assert cproj is not None
        assert np.array_equal(cproj.shape,self.imgs.shape)

        rd.save_TIFF(self.path+"output/spotclean/spotcleaned_{0:05d}.tif",cproj)