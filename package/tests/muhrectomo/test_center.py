import imgalg 
import pytest
import numpy as np
class TestCentering:

    @pytest.fixture(autouse=True)
    def center(self):
        print("Setup")
        self.center = imgalg.Center()

    def test_center_fixture(self):
        # When Then Expect
        assert self.center is not None

    def test_center_init(self):
        assert True