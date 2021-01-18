import os
import sys
import numpy as np
import pytest 
sys.path.append(os.getenv('IMAGALG_MODULE_PATH'))

from imagalg import PolynomialCorrection
from imagalg import ImagingException

pc=PolynomialCorrection()

pc.polynomialOrder()
pc.coefficients()

c=np.array([1,2,3,4])
pc.setup(c)
assert pc.polynomialOrder()==3
assert np.array(pc.coefficients()).all()==c.all()

# List tests
arr=np.array([1,2,3,4,5,6])
res=pc.process(arr)

assert  np.array(res).all()==(c[0]+c[1]*arr+c[2]*arr**2+c[3]*arr**3).all()

# 2D array test
arr2D=np.array([[1.0,2,3],[4,5,6]])
res2D=arr2D.copy()

pc.processInplace(res2D)

ver2D=c[0]+c[1]*arr2D+c[2]*arr2D**2+c[3]*arr2D**3

assert ver2D.all()==res2D.all()

# exception test
with pytest.raises(ImagingException) :
    assert pc.setup([1])

with pytest.raises(ImagingException) :
    assert pc.setup([0,1,2,3,4,5,6,7,8,9,10,11])