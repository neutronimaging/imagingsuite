import pymuhrec as pm
import pytest
import numpy as np

class TestReconstructor:

    @pytest.fixture(autouse=True)
    def reconstructor(self):
        self.reconstructor = pm.Reconstructor(pm.bpMultiProjParallel)

    def test_reconstructor_fixture(self):
        # When Then Expect
        assert self.reconstructor is not None

    def test_reconstructor_init(self):
        # When
        Nproj = 10
        proj = np.ones([Nproj,256,256])
        args = {"angles" : np.linspace(0,180,num=Nproj), 
                "weights" : np.ones(Nproj)}
        
        # Then
        self.reconstructor.configure({"center" : 50, 
                                      "resolution" : 0.135})
        self.reconstructor.process(proj,args)
        vol = self.reconstructor.volume()

        # Expect
        assert type(vol) is np.ndarray
        assert vol.shape == (256,256,256)
