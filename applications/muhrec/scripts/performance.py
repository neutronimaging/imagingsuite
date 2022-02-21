import sys, os
sys.path.append('../../../../TestData/generators')
import makeprojections as mp

def build_data_set(image_size,image_sizes,data_path) :
    if not os.path.exists(data_path) :
        os.mkdir(data_path)
    mp.makeTomodata(image_size,100,100,data_path,"proj_{0:04}.tif",image_size,10)


def run_muhrec(image_size, n_projections, n_reconstructions, n_threads,data_path) :
    # Create data

    # run muhrec several times
    #for i in range(n_reconstructions) :
    print('hepp')

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