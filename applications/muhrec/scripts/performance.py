import sys, os
from tkinter import CENTER
from subprocess import call
from tqdm import tqdm

sys.path.append('../../../../TestData/generators')
import makeprojections as mp

def build_data_set(image_size,image_sizes,data_path) :
    if not os.path.exists(data_path) :
        os.mkdir(data_path)
    mp.makeTomodata(image_size,100,100,data_path,"proj_{0:05}.tif",image_size,10)


def run_muhrec(image_size, n_projections, n_reconstructions, n_threads,data_path) :
    # Create data

    callargs = []
    callargs.append('<absolute path to muhrec>')
    callargs.append('-f')
    callargs.append('<absolute path to config.xml>')
    callargs.append('system::maxthreads={}'.format(n_threads))
    callargs.append('projections::lastindex={0}'.format(0))
    callargs.append('projections::firstindex={0}'.format(n_projections))
    callargs.append('projections::center={0}'.format(image_size/2))
    callargs.append("projections::filemask={0}/proj_{1}/proj_#####.tif".format(data_path,image_size))
    callargs.append('projections::path={0}/proj_{1}/'.format(data_path))
    callargs.append("projections::obfilemask={0}/proj_{1}/ob_#####.tif".format(data_path,image_size))
    callargs.append("projections::dcfilemask={0}/proj_{1}/dc_#####.tif".format(data_path,image_size))
    callargs.append('matrix::path={0}/proj_{1}/'.format(data_path,image_size))
    callargs.append("matrix::matrixname={0}/proj_{1}/recon_#####.tif".format())

    # run muhrec several times
    for i in tqdm(range(n_reconstructions)) :
        callargs.append("matrix::matrixname={0}/proj_{1}/recon_run{2:02}_#####.tif".format(data_path,image_size,i))
        call(callargs)

def performance_runner() :
    n_threads   = [1,2,4,8,16,32,64]
    image_sizes = [128,256] #,378,512,768,1024,2048] 
    n_reconstructions = 10
    data_home='projections'
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