import os
import sys
import numpy as np
import skimage.io as io
import math
sys.path.append(os.getenv('IMAGALG_MODULE_PATH'))

import imagalg as ia 

sf=ia.StripeFilter()

print("StripeFilter tests done")
