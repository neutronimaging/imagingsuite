import numpy as np
import muhrectomo as mt

recon = mt.Reconstructor(mt.bpMultiProj)

print("Created reconstructor :",recon.name())
Nproj = 10
proj = np.ones([Nproj,256,256])

recon.configure({   "center" : 128, 
                    "resolution" : 0.135
                })

args = {"angles" : np.linspace(0,180,num=Nproj), "weights" : np.ones(Nproj)}
recon.process(proj,args)

vol = recon.volume()

print(vol.shape)
