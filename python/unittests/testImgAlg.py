import pytest
import numpy as np
import sys
#sys.path.append('../../../build-imagingsuite/Release/MuhRec.app/Contents/Frameworks/')
sys.path.append('../../../build-imagingsuite/Release/lib')

import imgalg as ia

def test_average_image():
    avg = ia.AverageImage()

    img = np.random.rand(5, 100, 100).astype(np.float32)

    res = avg.process(img,method=ia.AverageImage.Average)

    assert res.shape == (100, 100)
    assert np.allclose(res, np.mean(img, axis=0)) 

def test_average_image_median():

    avg = ia.AverageImage()

    img = np.random.rand(5, 100, 100).astype(np.float32)

    res = avg.process(img,method=ia.AverageImage.Median)

    assert res.shape == (100, 100)
    assert np.allclose(res, np.median(img, axis=0))

def test_average_image_sum():
    avg = ia.AverageImage()

    img = np.random.rand(5, 100, 100).astype(np.float32)

    res = avg.process(img,method=ia.AverageImage.Sum)

    assert res.shape == (100, 100)
    assert np.allclose(res, np.sum(img, axis=0))

def test_average_image_weighted_average():
    avg = ia.AverageImage()

    img = np.random.rand(5, 100, 100).astype(np.float32)
    weights = np.random.rand(5).astype(np.float32)

    res = avg.process(img,method=ia.AverageImage.WeightedAverage)

    assert res.shape == (100, 100)
    # assert np.allclose(res, np.mean(img, axis=0))

def test_average_image_mad_weighted_average():
    avg = ia.AverageImage()

    img = np.random.rand(5, 100, 100).astype(np.float32)

    res = avg.process(img,method=ia.AverageImage.MADWeightedAverage)

    assert res.shape == (100, 100)
    # assert np.allclose(res, np.mean(img, axis=0))

def test_average_image_min():
    avg = ia.AverageImage()

    img = np.random.rand(5, 100, 100).astype(np.float32)

    res = avg.process(img,method=ia.AverageImage.Min)

    assert res.shape == (100, 100)
    assert np.allclose(res, np.min(img, axis=0))

def test_average_image_max():
    avg = ia.AverageImage()

    img = np.random.rand(5, 100, 100).astype(np.float32)

    res = avg.process(img,method=ia.AverageImage.Max)

    assert res.shape == (100, 100)
    assert np.allclose(res, np.max(img, axis=0))

