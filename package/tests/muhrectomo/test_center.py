import pymuhrec as pm 
import pytest
import numpy as np
import matplotlib.pyplot as plt
import pymuhrec.utils.readers as rd
from pathlib import Path

class TestCentering:

    @pytest.fixture(autouse=True)
    def center(self):
        self.center = pm.TomoCenter()

        self.output_dirs  = Path(__file__).parent / "output" / "center"
        self.output_dirs.mkdir(parents=True, exist_ok=True)
        self.data_path = Path(__file__).parents[4] / "TestData" / "2D" / "tiff" / "tomo" / "04_ct5s375_128lines"

        self.img0   = rd.read_image(str(self.data_path / "ct5s_00001.tif"))
        self.img180 = rd.read_image(str(self.data_path / "ct5s_00187.tif"))
        dc          = rd.read_image(str(self.data_path / "dc_00001.tif"))
        ob          = rd.read_image(str(self.data_path / "ob_00001.tif"))
        
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
        ax.set_title('Center at x={0:0.2f}'.format(est[0]))
        fig.savefig(str(self.output_dirs / 'center_basic.png'))
        print(est)
        assert True
