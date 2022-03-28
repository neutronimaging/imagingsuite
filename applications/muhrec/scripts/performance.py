import sys, os
import pathlib
from tkinter import CENTER
from subprocess import call
from tqdm import tqdm
import makeprojections as mp

from os.path import expanduser
home = expanduser("~")

def build_data_set(image_size,image_sizes,data_path) :
    if not os.path.exists(data_path) :
        os.mkdirs(data_path,exist_ok=True)

    mp.makeTomodata(image_size,100,100,data_path,"proj_{0:05}.tif",image_size,10)


def run_muhrec(image_size, n_projections, n_reconstructions, n_threads,data_path) :
    # Create data
    recon_path = "{0}/../recon/recon_{1}".format(data_path,image_size)

    pathlib.Path(recon_path).mkdir(parents=True,exist_ok=True)

    callargs = []
    callargs.append('./muhrec')
    callargs.append('-f')
    callargs.append('PerformanceRecon.xml')
    callargs.append('system:maxthreads={}'.format(n_threads))
    callargs.append('projections:firstindex={0}'.format(0))
    callargs.append('projections:lastindex={0}'.format(n_projections-1))
    callargs.append('projections:center={0}'.format(image_size/2))
    callargs.append('projections:roi=0,0,{0},{0}'.format(image_size-1))
    callargs.append("projections:filemask={0}/proj_#####.tif".format(data_path))
    callargs.append("projections:obfilemask={0}/ob_#####.tif".format(data_path))
    callargs.append("projections:dcfilemask={0}/dc_#####.tif".format(data_path))
    callargs.append('matrix:path={0}/'.format(recon_path))
    callargs.append("matrix:matrixname=recon{0}_run{1:02}_#####.tif".format(image_size,0))

    # run muhrec several times
    for i in tqdm(range(n_reconstructions)) :
        callargs[-1]="matrix:matrixname=recon{0}_run{1:02}_#####.tif".format(image_size,i)
        call(callargs)

def performance_runner() :
    n_threads   = [1] #,2,4,8,16,32,64]
    image_sizes = [128] #,256,378,512,768,1024,2048] 
    n_reconstructions = 10
    data_home=home+'/projections'
    if not os.path.exists(data_home) :
        os.mkdir(data_home)
    for image_size in image_sizes :
        data_path = "{0}/proj_{1}".format(data_home,image_size)
        build_data_set(image_size,image_sizes,data_path)

        for threads in n_threads :
            run_muhrec(image_size,image_size,n_reconstructions,threads,data_path)

def main(argv) :
    for arg in argv[1:]:
        print(arg)

    performance_runner()


if __name__ == "__main__":
    sys.exit(main(sys.argv))
