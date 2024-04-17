import numpy as np
import astropy.io.fits as fits
import tifffile as tiff
import pandas as pd
import argparse

def read_fits_meta_data2(fmask,first,last,step,parlist = []) :
    '''Reads selected meta data from a series of fits files
    
    Arguments:
        fmask   - file name mask of the fits file the index template is using the format {0:0N} where N is the number of digits in the index
        first   - first file index in the series
        last    - last file index in the series
        parlist - a list of strings to select the meta data tags.
        
        
    Returns:
        A dict containing lists of the requested values from each file
    '''
    data = {}
    
    for par in parlist :
        data[par]= []

    data['file'] = []
        
    for idx in np.arange(first,last+1,step=step) :
        data['file'].append(fmask.format(idx))
        hdul = fits.open(data['file'][-1])
        hdul.verify('silentfix')
    
        for par in parlist :
            value = hdul[0].header[par]
            # if isinstance(value,str) and value[0]=='b' :
            #     value=value[2:-1]
            if isinstance(value,str):
                value=value.split(' ')[0].split('\'')[1]
            data[par].append(value)
            
        hdul.close()
        
    return data

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Create a projection list file with angles and file names for MuhRec.")
    parser.add_argument('-m','--mask', help="Full path mask for the projection files.", default="proj_{0:04d}.fits")
    parser.add_argument('-f','--first', help="First projection index.",default=1)
    parser.add_argument('-l','--last', help="Last projection index.",default=100)
    parser.add_argument('-s','--step', help="Step between projections.",default=1)
    parser.add_argument('-o','--output', help="Output file name.",default="projlist.txt")

    args = parser.parse_args()

    data = read_fits_meta_data2(args.mask,int(args.first),int(args.last),int(args.step),['sample_position_2/sp2_ry'])

    projlist = pd.DataFrame(data)
    projlist['sample_position_2/sp2_ry'] = projlist['sample_position_2/sp2_ry'].astype(float)
    projlist.to_csv(args.output,sep='\t',index=False,header=False)