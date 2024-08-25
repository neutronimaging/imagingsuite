import pymuhrec as pm
import pytest
import numpy as np
class TestSpotClean:

    @pytest.fixture(autouse=True)
    def spotclean(self):
        print("Setup")
        self.spotclean = pm.MorphSpotClean()

    def test_spotclean_fixture(self):
        # When Then Expect
        assert self.spotclean is not None

    def test_spotclean_init(self):
        assert True
