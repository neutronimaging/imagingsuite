import os
import sys
import numpy as np
import skimage.io as io
import math
sys.path.append(os.getenv('IMAGALG_MODULE_PATH'))

import imagalg as ia 

msc=ia.MorphSpotClean()
# Constructor values
assert msc.cleanMethod()     == ia.eMorphCleanMethod.MorphCleanReplace
assert msc.detectionMethod() == ia.eMorphDetectionMethod.MorphDetectHoles

limits=msc.clampLimits()
assert math.isclose(limits[0],-0.1,rel_tol=1e-07)
assert limits[1]==7.0
assert msc.clampActive() == False
assert msc.maxArea()     == 100

assert msc.edgeConditionLength() == 9

# Tests with parameter changes
msc.setCleanMethod(ia.eMorphDetectionMethod.MorphDetectPeaks, ia.eMorphCleanMethod.MorphCleanFill)
assert msc.cleanMethod()     == ia.eMorphCleanMethod.MorphCleanFill
assert msc.detectionMethod() == ia.eMorphDetectionMethod.MorphDetectPeaks



msc.setLimits(True, -2, 10, 1000);
limits=msc.clampLimits()
assert math.isclose(limits[0],-2,rel_tol=1e-07)
assert limits[1]==10
assert msc.clampActive() == True
assert msc.maxArea()     == 1000


# Tests with image

msc.setCleanMethod(ia.eMorphDetectionMethod.MorphDetectHoles, ia.eMorphCleanMethod.MorphCleanReplace)
msc.setLimits(False,-0.1,7.0,100)

fimg=io.imread('../../UnitTests/data/spotprojection_0001.tif').astype('float32')

di=msc.detectionImage(fimg)
assert di.shape == fimg.shape
ci=fimg.copy()
msc.process(ci,0.3,0.01)
assert ci.shape == fimg.shape

assert math.isclose(ci.mean(),5.89322,rel_tol=1e-4)
assert math.isclose(ci.std(),22.368868,rel_tol=1e-4)

ci=fimg.copy()
msc.process(ci,[0.3,0.2],[0.01,0.0005])
assert ci.shape == fimg.shape

assert math.isclose(ci.mean(),5.89322,rel_tol=1e-4)
assert math.isclose(ci.std(),22.368868,rel_tol=1e-4)

dimg=io.imread('../../UnitTests/data/spotprojection_0001.tif').astype('float64')

di=msc.detectionImage(dimg)
assert di.shape == dimg.shape
ci=dimg.copy()
msc.process(ci,0.3,0.01)
assert ci.shape == dimg.shape

assert math.isclose(ci.mean(),5.89322,rel_tol=1e-4)
assert math.isclose(ci.std(),22.368868,rel_tol=1e-4)


ci=dimg.copy()
msc.process(ci,[0.3,0.2],[0.01,0.0005])
assert ci.shape == dimg.shape

assert math.isclose(ci.mean(),5.89322,rel_tol=1e-4)
assert math.isclose(ci.std(),22.368868,rel_tol=1e-4)

print("MorphSpotClean tests done")
