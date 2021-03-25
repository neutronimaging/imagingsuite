#!/usr/bin/env python
# coding: utf-8

# # MuhRec in python demonstration

# In[74]:


import sys, os
sys.path.insert(0, "/Users/kaestner/git/imagingsuite/frameworks/install/lib")
sys.path.insert(0, "/Users/kaestner/git/scripts/python/")
if 'LD_LIBRARY_PATH' not in os.environ:
    os.environ['LD_LIBRARY_PATH'] = '/Users/kaestner/git/imagingsuite/frameworks/install/lib'
   # os.execv(sys.argv[0], sys.argv)



# In[75]:


import numpy as np
import muhrectomo as mt
import matplotlib.pyplot as plt
import amglib.readers as io
import amglib.imageutils as amg


# In[76]:


import importlib
importlib.reload(mt)
importlib.reload(io)
importlib.reload(amg)


# ## Create a reconstructor object

# In[77]:


recon = mt.Reconstructor(mt.bpMultiProj)

print("Created reconstructor :",recon.name())


# ## Reconstruction workflow

# ### Numerical dummy
# This dummy is only to test the connection and that the reconstructor starts at all. Only nonsense data.

# In[78]:


Nproj = 10
proj = np.ones([Nproj,256,256])


# In[79]:


args = {"angles" : np.linspace(0,180,num=Nproj), 
        "weights" : np.ones(Nproj)}


# In[80]:


recon.configure({   "center" : 50, 
                    "resolution" : 0.135
                })


# In[81]:


recon.process(proj,args)

vol = recon.volume()


# In[82]:


plt.imshow(vol[0])


# ### The wood data
# The wood data is the data I use a lot for demos, tests, and tutorials. It a piece of petrified wood [DOI](http://dx.doi.org/10.17632/g5snr785xy.2). 

# #### Load projection data

# In[83]:


ob = io.readImages('/Users/Shared/data/wooddata/projections/ob_{0:04d}.tif',1,5,averageStack=True).mean(axis=0)
dc = io.readImages('/Users/Shared//data/wooddata/projections/dc_{0:04d}.tif',1,5,averageStack=True).mean(axis=0)


# In[84]:


proj = io.readImages('/Users/Shared/data/wooddata/projections/wood_{0:04d}.tif',1,626) # This takes a while


# In[85]:


fig,ax = plt.subplots(1,3,figsize=[15,7])
ax[0].imshow(dc)
ax[1].imshow(ob)
ax[2].imshow(proj[0])


# ## Preprocessing
# Here the projections needs to be prepared for reconstruction. Typical operations are 
# - Cropping
# - Normalization (possibly with scattering correction)
# - Spot cleaning
# - Ring cleaning

# ### Normalize - the kipl way

# In[86]:


import imgalg 


# In[87]:


norm = imgalg.NormalizeImage(True) # True for use logarithm


# In[88]:


norm.usingLog()


# In[89]:


norm = imgalg.NormalizeImage(True) # True for use logarithm
norm.setReferences(ob,dc)
cproj = proj.copy()
norm.process(cproj)


# ## Normalize - the python way

# In[90]:


nproj = amg.normalizeImage(img=proj, ob=ob, dc=dc, neglog=True)


# In[68]:


cproj = nproj
plt.imshow(cproj[0])
cproj.shape


# In[91]:


fig,axes=plt.subplots(1,5,figsize=(15,4))

for idx,ax in enumerate(axes) :
    ax.imshow(cproj[idx*20]-nproj[idx*20])


# In[92]:


print(nproj.shape,cproj.shape)


# In[97]:


fig,axes=plt.subplots(1,3,figsize=(15,4))
a0=axes[0].imshow(cproj[:,400,:], vmin=-0.1,vmax=1); axes[0].set_title('cproj')
fig.colorbar(a0,ax=axes[0])
a1=axes[1].imshow(nproj[:,400,:], vmin=-0.1,vmax=1); axes[1].set_title('nproj')
fig.colorbar(a1,ax=axes[1])
a2=axes[2].imshow(cproj[:,400,:]-nproj[:,400,:]); axes[2].set_title('diff')
fig.colorbar(a2,ax=axes[2])

cproj.shape


# #### Prepare and run the back-projection

# In[98]:


Nproj = cproj.shape[0]
# Information per projection
args = {"angles"  : np.linspace(0,360,num=Nproj), 
        "weights" : np.ones(Nproj)/Nproj
       }

# Geometry information
recon.configure({   "center" : 305, 
                    "resolution" : 0.1
                })
print("have",Nproj,"projections")


# In[101]:


fig,ax=plt.subplots(1,2,figsize=(12,5))
ax[0].plot(args["angles"],'o')
ax[1].plot(args["weights"],'o')


# In[119]:


recon.process(cproj[:,500:510,:],args) # Reconstruct a part of the slices (32 slices here)
del vol
vol = recon.volume() # Retrieve the reconstructed volume


# In[120]:


plt.imshow(vol[0])
plt.colorbar()


# In[115]:


import ipyvolume as ipv


# In[116]:


ipv.widgets.quickvolshow(vol)


# In[104]:


print(nproj.flags)
print(cproj.flags)


# In[ ]:




