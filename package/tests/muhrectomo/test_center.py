import pymuhrec as pm 
import pytest
import numpy as np
import matplotlib.pyplot as plt
import utils.readers as rd
import os

class TestCentering:

    @pytest.fixture(autouse=True)
    def center(self):
        print("Setup")
        self.center = pm.TomoCenter()

        self.path  = os.path.dirname(os.path.abspath(__file__))+'/'
        os.makedirs(self.path + 'output/center', exist_ok=True)
        self.data_path = self.path + "../../../../../TestData/"
        
        self.img0   = rd.read_image(self.data_path + "2D/tiff/tomo/04_ct5s375_128lines/ct5s_00001.tif")
        self.img180 = rd.read_image(self.data_path + "2D/tiff/tomo/04_ct5s375_128lines/ct5s_00187.tif")
        dc          = rd.read_image(self.data_path + "2D/tiff/tomo/04_ct5s375_128lines/dc_00001.tif")
        ob          = rd.read_image(self.data_path + "2D/tiff/tomo/04_ct5s375_128lines/ob_00001.tif")
        
        normalize = pm.NormalizeImage(True)
        normalize.setReferences(ob,dc)
        normalize.process(self.img0)
        normalize.process(self.img180)

    def test_center_fixture(self):
        # When Then Expect
        assert self.center is not None

    def test_center_basic(self):
        est = self.center.estimate(self.img0, self.img180, pm.centerLeastSquare, False)
        centers = self.center.centers()

        fig, ax = plt.subplots(1)
        img = np.zeros([self.img0.shape[0],self.img0.shape[1],3])
        img[:,:,1] = self.img0
        img[:,:,2] = self.img180
        img[:,:,0] = (self.img0 + self.img180)/2
        ax.imshow(img)

        ax.plot(centers, np.arange(len(centers)), 'r')
        ax.set_title('Center at x={0:0.2f}'.format(est))
        fig.savefig(self.path + 'output/center/center_basic.png')
        print(est)
        assert True
