import pymuhrec as pm 
import pytest
import numpy as np
import pymuhrec.utils.readers as rd
from pathlib import Path

class TestRingClean:

    @pytest.fixture(autouse=True)
    def ringclean(self):

        self.output_dirs  = Path(__file__).parent / "output" / "ringclean"
        self.output_dirs.mkdir(exist_ok=True)
        self.data_path = Path(__file__).parents[4] / "TestData" / "2D" / "tiff"
        self.img = rd.read_image(str(self.data_path / "woodsino_0200.tif"))

        self.ringclean = pm.StripeFilter([self.img.shape[1],self.img.shape[0]],"daub7",4,0.05)

    def test_ringclean_fixture(self):
        # When Then Expect
        assert self.ringclean is not None

    def test_ringclean_basic(self):
        rd.save_TIFF(str(self.output_dirs / "ring_rawsino.tif"),self.img)

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

        rd.save_TIFF(str(self.output_dirs / "ringcleaned.tif"),sino)
        
    def test_multiple_sinograms(self):
        imgs = rd.read_images(str(self.data_path / "tomo" / "04_ct5s375_128lines" / "ct5s_{0:05d}.tif"), first=1, last=376)
        dc   = rd.read_image(str(self.data_path / "tomo" / "04_ct5s375_128lines" / "dc_00001.tif"))
        ob   = rd.read_image(str(self.data_path / "tomo" / "04_ct5s375_128lines" / "ob_00001.tif"))
        
        normalize = pm.NormalizeImage(True)
        normalize.setReferences(ob,dc)
        normalize.process(imgs)

        sinos = imgs.copy().astype("float32")
        rc= pm.StripeFilter([sinos.shape[2],sinos.shape[0]],"daub7",4,0.05)
        
        rc.process(sinos,pm.eStripeFilterOperation.VerticalComponentFFT)

        assert sinos is not None
        

        rd.save_TIFF(str(self.output_dirs / "rc_{0:05d}.tif"),sinos)
        rd.save_TIFF(str(self.output_dirs / "raw_sino_50.tif"),imgs[:,50,:])
        rd.save_TIFF(str(self.output_dirs / "raw_sino_100.tif"),imgs[:,100,:])
        rd.save_TIFF(str(self.output_dirs / "rcsino_50.tif"),sinos[:,50,:])
        rd.save_TIFF(str(self.output_dirs / "rcsino_100.tif"),sinos[:,100,:])