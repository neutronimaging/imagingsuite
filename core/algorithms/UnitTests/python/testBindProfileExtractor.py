import os
import sys
import numpy as np
import skimage.io as io
import math
sys.path.append(os.getenv('IMAGINGQA_MODULE_PATH'))

import imagingqa as iqa

pe=iqa.ProfileExtractor()

assert pe.precision() == 1.0

pe.setPrecision(0.1)
assert math.isclose(pe.precision(),0.1,rel_tol=1e-6)


print("ProfileExtractor tests done")