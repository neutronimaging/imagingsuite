import imgalg 
import pytest
import numpy as np
class TestNormalization:

    @pytest.fixture(autouse=True)
    def normalize(self):
        print("Setup")
        self.normalize = imgalg.NormalizeImage(True)



    def test_normalize_fixture(self):
        # When Then Expect
        assert self.normalize is not None

    def test_normalize_init(self):
        assert True
        norm.setReferences(ob,dc)
        cproj = proj.copy()
        norm.process(cproj)