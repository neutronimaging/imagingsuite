import pymuhrec as pm 
import pytest
import numpy as np
import utils.readers as rd
import os

class TestNormalization:

    @pytest.fixture(autouse=True)
    def normalize(self):
        # print("Setup")
        self.normalize = pm.NormalizeImage(True)

        self.path  = os.path.dirname(os.path.abspath(__file__))+'/'
        os.makedirs(self.path + 'output', exist_ok=True)
        self.data_path = self.path + "../../../../../TestData/"
        # print(self.path)
        self.img = rd.read_image(self.data_path + "2D/tiff/tomo/04_ct5s375_128lines/ct5s_00001.tif")
        self.ob  = rd.read_image(self.data_path + "2D/tiff/tomo/04_ct5s375_128lines/ob_00001.tif")
        self.dc  = rd.read_image(self.data_path + "2D/tiff/tomo/04_ct5s375_128lines/dc_00001.tif")


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
        rd.save_TIFF(self.path+"output/pynorm.tif",n)
        rd.save_TIFF(self.path+"output/bindnorm.tif",cproj)
        s = np.sum(np.fabs(n-cproj)>0.00001)
        assert s == 0
