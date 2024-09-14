import pymuhrec as pm 
import pytest
import numpy as np
import pymuhrec.utils.readers as rd
from pathlib import Path # Automatically gives correct paths independent of OS, more user friendly / less verbose than os

class TestNormalization:

    @pytest.fixture(autouse=True)
    def normalize(self):
        # print("Setup")
        self.normalize = pm.NormalizeImage(True)

        self.output_dirs  = Path(__file__).parent / "output"
        self.output_dirs.mkdir(parents=True, exist_ok=True)
        self.data_path = Path(__file__).parents[4] / "TestData" / "2D" / "tiff" / "tomo" / "04_ct5s375_128lines"
        self.img = rd.read_image(str(self.data_path / "ct5s_00001.tif"))
        self.ob  = rd.read_image(str(self.data_path / "ob_00001.tif"))
        self.dc  = rd.read_image(str(self.data_path / "dc_00001.tif"))


    def test_normalize_fixture(self):
        # When Then Expect
        assert self.normalize is not None
        assert self.img is not None
        assert self.ob is not None
        assert self.dc is not None
        assert np.array_equal(self.img.shape,self.ob.shape)
        assert np.array_equal(self.img.shape,self.dc.shape)
        assert np.array_equal(self.dc.shape,self.ob.shape)


    def test_lognorm(self):
        self.normalize.setReferences(self.ob,self.dc)
        cproj = self.img.copy()
        self.normalize.process(cproj)
        assert cproj is not None
        assert np.array_equal(cproj.shape,self.img.shape)
        i = (self.img-self.dc)
        i[i<1] = 1
        o = (self.ob-self.dc)
        o[o<1] = 1
        n = i/o
        n = -np.log(n)
        #assert np.allclose(n,cproj,1e-3)    
        rd.save_TIFF(str(self.output_dirs / "pynorm.tif"),n)
        rd.save_TIFF(str(self.output_dirs / "bindnorm.tif"),cproj)
        s = np.sum(np.fabs(n-cproj)>0.00001)
        assert s == 0
        assert np.isclose(n,cproj,atol=1e-3).all()

    def test_normstack(self) :
        self.normalize.setReferences(self.ob,self.dc)

        imgs = rd.read_images(str(self.data_path / "ct5s_{0:05d}.tif"), first=1, last=5)

        cproj = imgs.copy()
        self.normalize.process(cproj)
        assert cproj is not None
        assert np.array_equal(cproj.shape,imgs.shape)
        n = np.zeros(cproj.shape)
        for idx in range(0,imgs.shape[0]):
            i = (imgs[idx]-self.dc)
            i[i<1] = 1
            o = (self.ob-self.dc)
            o[o<1] = 1
            n[idx] = i/o

        n = -np.log(n)

        assert np.isclose(n,cproj,atol=1e-3).all()

    def test_dosecorrection(self) :
        doseROI = [10,10,50,50]
        self.normalize.setDoseROI(doseROI)
        self.normalize.setReferences(self.ob,self.dc)

        cproj = self.img.copy()
        self.normalize.process(cproj)
        assert cproj is not None
        assert np.array_equal(cproj.shape,self.img.shape)
        
        i = (self.img-self.dc)
        i[i<1] = 1
        o = (self.ob-self.dc)
        o[o<1] = 1
        n = i/o
        n = -np.log(n)
        # rd.save_TIFF(self.path+"output/pynorm.tif",n)
        rd.save_TIFF(str(self.output_dirs / "bindnorm_dose.tif"),cproj)


        # assert np.isclose(n,cproj,atol=1e-3).all()
        
    