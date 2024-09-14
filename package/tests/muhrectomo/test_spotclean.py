import pymuhrec as pm 
import pytest
import numpy as np
import pymuhrec.utils.readers as rd
from pathlib import Path

class TestSpotClean:

    @pytest.fixture(autouse=True)
    def spotclean(self):
        # print("Setup")
        self.spotclean = pm.MorphSpotClean()

        self.output_dirs  = Path(__file__).parent / "output" / "spotclean"
        self.output_dirs.mkdir(parents=True, exist_ok=True)
        self.data_path = Path(__file__).parents[4] / "TestData" / "2D" / "tiff" / "tomo" / "04_ct5s375_128lines"

        self.img = rd.read_image(str(self.data_path.parents[1] / "normalized_projection.tif"))
        self.imgs = rd.read_images(str(self.data_path / "ct5s_{0:05d}.tif"), first=1, last=5)
        self.ob  = rd.read_image(str(self.data_path / "ob_00001.tif"))
        self.dc  = rd.read_image(str(self.data_path / "dc_00001.tif"))
        
        normalize = pm.NormalizeImage(True)
        normalize.setReferences(self.ob, self.dc)
        normalize.process(self.imgs)

        

    def test_spotclean_fixture(self):
        # When Then Expect
        assert self.spotclean is not None

    def test_spotclean_basic_run(self):
        rd.save_TIFF(str(self.output_dirs / "spotty.tif"),self.img)
        self.spotclean.setCleanMethod(detectionMethod=pm.MorphDetectAllSpots, cleanMethod=pm.MorphCleanReplace)
        assert self.spotclean.detectionMethod() == pm.MorphDetectAllSpots
        assert self.spotclean.cleanMethod() == pm.MorphCleanReplace
        strip = self.img.copy() 
        self.spotclean.process(strip,th=[0.99, 0.99],sigma=[0.01, 0.01])

        assert strip is not None
        assert np.array_equal(strip.shape,self.img.shape)
        rd.save_TIFF(str(self.output_dirs / "spotcleaned.tif"),strip)

    def test_spotclean_stack(self):
        rd.save_TIFF(str(self.output_dirs / "spotty_{0:05d}.tif"),self.imgs)
        cproj = self.imgs.copy()
        self.spotclean.setCleanMethod(detectionMethod=pm.MorphDetectAllSpots, cleanMethod=pm.MorphCleanReplace)
        self.spotclean.process(cproj,th=[0.99, 0.99],sigma=[0.01, 0.01])

        assert cproj is not None
        assert np.array_equal(cproj.shape,self.imgs.shape)

        rd.save_TIFF(str(self.output_dirs / "spotcleaned_{0:05d}.tif"),cproj)