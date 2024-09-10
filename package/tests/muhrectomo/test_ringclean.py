import pymuhrec as pm 
import pytest
import numpy as np
import utils.readers as rd
import os

class TestRingClean:

    @pytest.fixture(autouse=True)
    def ringclean(self):
        print("Setup")
        self.path  = os.path.dirname(os.path.abspath(__file__))+'/'
        os.makedirs(self.path + 'output/ringclean', exist_ok=True)
        self.data_path = self.path + "../../../../../TestData/"
        self.img = rd.read_image(self.data_path + "2D/tiff/woodsino_0200.tif")
        self.ringclean = pm.StripeFilter([self.img.shape[1],self.img.shape[0]],"daub7",4,0.05)

    def test_ringclean_fixture(self):
        # When Then Expect
        assert self.ringclean is not None

    def test_ringclean_basic(self):
        rd.save_TIFF(self.path+"output/ringclean/ring_rawsino.tif",self.img)

        # checking configured values
        assert self.ringclean.dims() == [self.img.shape[1],self.img.shape[0]] # self.img.shape
        assert self.ringclean.waveletName() == "daub7"
        assert self.ringclean.decompositionLevels() == 4
        assert np.isclose(self.ringclean.sigma(),0.05, atol=1e-4)

        # basic processing of single sinogram
        sino=self.img.copy().astype("float32")
        self.ringclean.process(sino,pm.eStripeFilterOperation.VerticalComponentFFT)

        assert sino is not None
        assert np.array_equal(sino.shape,self.img.shape)

        rd.save_TIFF(self.path+"output/ringclean/ringcleaned.tif",sino)
        
    def test_multiple_sinograms(self):
        imgs = rd.read_images(self.data_path + "2D/tiff/tomo/04_ct5s375_128lines/ct5s_{0:05d}.tif", first=1, last=376)
        dc   = rd.read_image(self.data_path + "2D/tiff/tomo/04_ct5s375_128lines/dc_00001.tif")
        ob   = rd.read_image(self.data_path + "2D/tiff/tomo/04_ct5s375_128lines/ob_00001.tif")
        
        normalize = pm.NormalizeImage(True)
        normalize.setReferences(ob,dc)
        normalize.process(imgs)

        sinos = imgs.copy().astype("float32")
        rc= pm.StripeFilter([sinos.shape[2],sinos.shape[0]],"daub7",4,0.05)
        
        rc.process(sinos,pm.eStripeFilterOperation.VerticalComponentFFT)

        assert sinos is not None
        

        rd.save_TIFF(self.path+"output/ringclean/rc_{0:05d}.tif",sinos)
        rd.save_TIFF(self.path+"output/ringclean/raw_sino_50.tif",imgs[:,50,:])
        rd.save_TIFF(self.path+"output/ringclean/raw_sino_100.tif",imgs[:,100,:])
        rd.save_TIFF(self.path+"output/ringclean/rcsino_50.tif",sinos[:,50,:])
        rd.save_TIFF(self.path+"output/ringclean/rcsino_100.tif",sinos[:,100,:])