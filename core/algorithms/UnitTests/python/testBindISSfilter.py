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

img=io.imread('/data/P20170229/04_evaluation/filtered_0000/frame0000_vol.tif').astype('float32')

img2=img[300:400,300:400,300:400]
m=img2.mean()
s=img2.std()
img2=(img2-m)/s

assert len(iss.errors()) == 0

imgp=img2.copy()
iss.setInitialImageType(af.InitialImageOriginal)
iss.process(imgp, tau=0.05,N=10,plambda=0.04, palpha=0.01)

assert len(iss.errors()) == 10

# plt.subplot(2,2,1)
# plt.imshow(img2[50])
# plt.subplot(2,2,2)
# plt.imshow(imgp[50])
# plt.subplot(2,2,3)
# plt.imshow(img2[50]-imgp[50])
# plt.subplot(2,2,4)
# plt.plot(iss.errors()) 


