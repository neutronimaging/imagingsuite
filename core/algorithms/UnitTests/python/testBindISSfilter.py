import os
import sys
import numpy as np
import skimage.io as io
import math
sys.path.append(os.getenv('ADVANCEDFILTERS_MODULE_PATH'))

import advancedfilters as af

iss=af.ISSfilter3D()

assert iss.initialImageType() == af.InitialImageOriginal

iss.setInitialImageType(af.InitialImageZero)
assert iss.initialImageType() == af.InitialImageZero

img=io.imread('../../UnitTests/data/frame0000_100x100x100.tif').astype('float32')

m=img.mean()
s=img.std()
img=(img-m)/s

assert len(iss.errors()) == 0

imgp=img.copy() # making a copy to allow comparison (the filter operates in-place)
iss.setInitialImageType(af.InitialImageOriginal)
N=10
iss.process(imgp, tau=0.05,N=N,plambda=0.04, palpha=0.01)

assert len(iss.errors()) == N

# plt.subplot(2,2,1)
# plt.imshow(img[50])
# plt.subplot(2,2,2)
# plt.imshow(imgp[50])
# plt.subplot(2,2,3)
# plt.imshow(img[50]-imgp[50])
# plt.subplot(2,2,4)
# plt.plot(iss.errors()) 


print("ISS filter tests done")
