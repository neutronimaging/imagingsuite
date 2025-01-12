from pymuhrec.utils.readers import read_image
from unittest.mock import MagicMock
import numpy as np

def test_read_image(monkeypatch):
    # When
    monkeypatch.setattr("tifffile.imread", MagicMock(return_value=np.zeros((10,10))))
    monkeypatch.setattr("astropy.io.fits.getdata", MagicMock(return_value=np.zeros((10,10))))
    # Then
    img_tif = read_image("test.tif")
    img_fits = read_image("test.fits")
    # Expect
    assert img_tif.shape == (10,10)
    assert img_fits.shape == (10,10)
    assert img_fits.dtype == np.float32
    assert img_tif.dtype == np.float32