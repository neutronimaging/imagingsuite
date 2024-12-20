import pymuhrec as pm 
import pytest
import numpy as np
import pymuhrec.utils.readers as rd
from pathlib import Path

class TestWorkflow:

    def test_full_reconstruction_workflow(self):
        # Read in images and set paths
        self.output_dirs  = Path(__file__).parent / "output" / "workflow"
        self.output_dirs.mkdir(parents=True, exist_ok=True)
        self.data_path = Path(__file__).parents[4] / "TestData" / "2D" / "tiff" / "tomo" / "04_ct5s375_128lines"

        imgs = rd.read_images(str(self.data_path / "ct5s_{0:05d}.tif"), first=1, last=376)
        dc   = rd.read_image(str(self.data_path / "dc_00001.tif"))    
        ob   = rd.read_image(str(self.data_path / "ob_00001.tif"))

        # Normalize projections with dose correction        
        normalize = pm.NormalizeImage(True)
        doseROI = [10,10,50,50]
        normalize.setDoseROI(doseROI)
        normalize.setReferences(ob,dc)
        normalize.process(imgs)

        # Apply spot cleaning
        # Clean 1% of dark and bright spots with a fuzzyness sigma of 0.01
        spotclean = pm.MorphSpotClean()
        spotclean.setCleanMethod(detectionMethod=pm.MorphDetectAllSpots, cleanMethod=pm.MorphCleanReplace)
        spotclean.process(imgs,th=[0.99, 0.99],sigma=[0.01, 0.01])

        # Apply ring cleaning
        ringclean = pm.StripeFilter([imgs.shape[2],imgs.shape[0]],"daub7",4,0.05)
        ringclean.process(imgs,pm.eStripeFilterOperation.VerticalComponentFFT)
        
        # Setup the reconstructor
        Nproj = imgs.shape[0]
        
        args = {"angles" : np.linspace(0,360,num=Nproj), 
                "weights" : np.ones(Nproj)}
        
        center = pm.TomoCenter()
        print(imgs.shape)
        c_est= center.estimate(imgs[0], imgs[imgs.shape[0]//2], pm.centerLeastSquare, False)

        reconstructor = pm.Reconstructor(pm.bpMultiProjParallel)

        reconstructor.configure({"center" : c_est[0], 
                                #  "tilt"   : 0.0,
                                #  ""
                                 "resolution" : 0.135})
        
        # run the reconstruction
        reconstructor.process(imgs,args)
        vol = reconstructor.volume()

        # assert type(vol) == np.ndarray
        # assert vol.shape == (256,256,256)
        rd.save_TIFF(str(self.output_dirs / "recon_{0:05d}.tif"),vol)