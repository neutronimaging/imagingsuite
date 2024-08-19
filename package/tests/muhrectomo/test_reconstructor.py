import pymuhrec.muhrectomo as mt
import pytest

class TestReconstructor:

    @pytest.fixture(autouse=True)
    def setup(self):
        print("Setup")
        reconstructor = mt.Reconstructor(mt.bpMultiProj)

    def test_reconstructor_fixture(self):
        assert reconstructor is not None