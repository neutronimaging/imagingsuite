import os
import sys
import numpy as np
sys.path.append(os.getenv('IMAGALG_MODULE_PATH'))

from imagalg import AverageImage

N=10
a=np.linspace(0,N-1,N)
b=np.linspace(0,N,N+1)
c=np.linspace(0,N+1,N+2)
x,y,z=np.meshgrid(a,b,c)
img=x+y*N+z*N*N

avg=AverageImage()

assert avg.windowSize() == 5

# Average combination
res  = avg.process(img,AverageImage.ImageAverage,[])
ximg = img.mean(axis=0)

mse=((res-ximg)**2).mean()

assert mse==0.0

# Min combiantion
res=avg.process(img,AverageImage.ImageMin,[])
ximg=img.min(axis=0)

mse=((res-ximg)**2).mean()

assert mse==0.0

# Max combination
res=avg.process(img,AverageImage.ImageMax,[])
ximg=img.max(axis=0)

mse=((res-ximg)**2).mean()

assert mse==0.0

# Median combination
res=avg.process(img,AverageImage.ImageMedian,[])
ximg=np.median(img,axis=0)

mse=((res-ximg)**2).mean()

assert mse==0.0

print('AverageImage tests done')