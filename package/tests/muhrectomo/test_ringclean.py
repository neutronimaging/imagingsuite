import pymuhrec as pm
import pytest
import numpy as np

class TestRingClean:

    @pytest.fixture(autouse=True)
    def ringclean(self):
        print("Setup")
        self.ringclean = pm.StripeFilter([100,100],"daub7",4,0.05)

    def test_ringclean_fixture(self):
        # When Then Expect
        assert self.ringclean is not None

    def test_ringclean_init(self):
        assert True
