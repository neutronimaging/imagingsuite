import os
import sys
import numpy as np
import skimage.io as io
import math
sys.path.append(os.getenv('IMAGALG_MODULE_PATH'))

import imagalg as ia 

sf=ia.StripeFilter([100,110],"daub7",2,0.01)

assert math.isclose(sf.sigma(),0.01, rel_tol=1e-5)

assert sf.decompositionLevels() == 2
assert sf.dims()[0]             == 100
assert sf.dims()[1]             == 110
assert sf.waveletName()         == "daub7"

sino = io.imread('../../UnitTests/data/woodsino_0200.tif')

sf.configure([sino.shape[1],sino.shape[0]],"daub9", 3, 0.02)

assert math.isclose(sf.sigma(),0.02, rel_tol=1e-5)
assert sf.decompositionLevels() == 3
assert sf.dims()[0]             == sino.shape[1]
assert sf.dims()[1]             == sino.shape[0]
assert sf.waveletName()         == "daub9"

sf2=ia.StripeFilter([sino.shape[1],sino.shape[0]],"daub7",2,0.1)

s2=sino.copy().astype("float32")
sf2.process(s2,ia.eStripeFilterOperation.VerticalComponentFFT)
mse=((sino-s2)**2).mean()

assert math.isclose(mse,1.8129678e-05,rel_tol=1e-6)

print("StripeFilter tests done")
