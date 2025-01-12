import numpy as np
import tifffile as tiff

def makeSample(N,mu) :
    """
        Creates projection of a sphere made of a material with a specified attenuation coefficient.
        
        Parameters
        ----------
        N : integer
            Projection size NxN pixels
        mu : float
            Attenuation coefficient of the material
    """
    
    x,y = np.meshgrid(np.linspace(-1,1,N),np.linspace(-1,1,N))
    R = np.sqrt(x**2+y**2)
    R=1-(1.25*R)**2
    R[R<0]=0
    
    return np.exp(-mu*R**2)

def makeOpenBeam(N) :
    """
        Creates projection of the beam profile. It is a slightly inhomogeneous profile with max intensity in the center.
        
        Parameters
        ----------
        N : integer
            Projection size NxN pixels
    """
        
    x,y = np.meshgrid(np.linspace(-1,1,N),np.linspace(-1,1,N))
    R = np.sqrt(x**2+2*y**2)
    R=1-(0.9*R)**2
    
    return 0.05*R+np.ones([N,N])
    
def makeDarkCurrent(N,level,sigma) :
    """
        Creates projection of the dark current signal.
        
        Parameters
        ----------
        N : integer
            Projection size NxN pixels
        level : float
            bias value of the dc
        sigma : float
            scales the noise as fraction of the level
    """
        
    return np.ones([N,N])*level+np.random.normal(0,level*sigma,size=[N,N])
    
def addSpots(img,fraction, amplitude, width) :
    """
        Adds spots to the an image.
        
        Parameters
        ----------
        img : float 
            Projection that will get the spots
        fraction : float
            amount of spots as a fraction of the number of pixels
        amplitude : float
            spot amplitude
        width :
            spot width
            
        Returns
        -------
        An image with spots.
            
        Note
        ----
        Not implemented
    """
    
    return img

def makeProjection(img,ob,dc,SNR=100) :
    """
        Adds spots to the an image.
        
        Parameters
        ----------
        img : float 
            Projection that will get the spots
        fraction : float
            amount of spots as a fraction of the number of pixels
        amplitude : float
            spot amplitude
        width :
            spot width
            
        Returns
        -------
        An image with spots.
    """
    
    n=np.random.normal(0,1/SNR,size=img.shape)
    p=(img+n)*ob+dc
        
    return p

def makeTomodata(size,ampl,SNR,path,filemask,Nsample,Nrefs) :
    """
        Builds a projection data set and saves on disk.
        
        Parameters
        ----------
        size : int 
            Size of the images size x size pixels
        ampl : float
            the amplitude of the open beam image
        SNR : float
            signal to noise ratio of the projections
        path : string
            the path to the folder where the projections are stored
        filemask : string
            file name mask of the projections. It must contain a string formatting place holder like {0:05}.
        Nsample : int
            number of projections
        Nrefs : int
            number reference images (ob and dc)
            
        Output
        -------
        A collection of projections written as tiff files.
    """
    
    dclevel=300
    sigma = 0.05
    for i in range(Nrefs) :
        dc = np.ones([size,size])*dclevel+np.random.normal(0,dclevel*sigma,size=[size,size])
        ob = ampl*(makeOpenBeam(size)+np.random.normal(0,1/SNR,size=[size,size]))
        tiff.imwrite(path+'/ob_{0:05}.tif'.format(i),(ob+dc).astype('uint16'))
        tiff.imwrite(path+'/dc_{0:05}.tif'.format(i),dc.astype('uint16'))
        
    sample = makeSample(size,1.0)
    
    for i in range(Nsample) :
        img = makeProjection(sample,ob,dc,SNR=SNR)
        tiff.imwrite(path+"/"+filemask.format(i),img.astype('uint16'))
        
    