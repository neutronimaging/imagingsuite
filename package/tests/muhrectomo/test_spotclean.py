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
        os.makedirs(self.path + 'output', exist_ok=True)
        self.data_path = self.path + "../../../../../TestData/"
        self.img = rd.read_image(self.data_path + "2D/tiff/normalized_projection.tif")
        

    def test_spotclean_fixture(self):
        # When Then Expect
        assert self.spotclean is not None

    def test_spotclean_basic_run(self):
        rd.save_TIFF(self.path+"output/spotty.tif",self.img)
        self.spotclean.setCleanMethod(detectionMethod=pm.MorphDetectAllSpots, cleanMethod=pm.MorphCleanReplace)
        assert self.spotclean.detectionMethod() == pm.MorphDetectAllSpots
        assert self.spotclean.cleanMethod() == pm.MorphCleanReplace
        strip = self.img.copy() 
        self.spotclean.process(strip,th=[0.99, 0.99],sigma=[0.01, 0.01])

        assert strip is not None
        assert np.array_equal(strip.shape,self.img.shape)
        rd.save_TIFF(self.path+"output/spotcleaned.tif",strip)
        
