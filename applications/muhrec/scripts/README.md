# Perfomance tests for MuhRec
## Introduction
The performance testing is used to determine how much resources it is needed to allocate to perform a reconstruction of 

## Prerequisites

### Compile MuhRec and dependencies
If haven't compiled muhrec already, please follow the instuctions on the documentation [wiki](https://github.com/neutronimaging/imagingsuite/wiki)

### Install a python distribution
Python is mostly already installed on linux and MacOS. If you don't have a python installation it probably best to use [Anaconda](https://www.anaconda.com/products/individual). 

### Provide test data generator
The performance test will need projection data to run. The best way is to generate this data using the projection generator provided in the [TestData repository](https://github.com/neutronimaging/TestData).

```git clone https://github.com/neutronimaging/TestData.git```

The test data generator can be imported by
```
import sys
sys.path.append('<relative or absolute path to you repository collection>/TestData/generators')
import makeprojections as mp
```

## Run the tests
The test are set up to test different configurations of 
- Image size. The projection images are quadratic.
- Number of projections. The number of projections are typically chosen to be the same as the image width.
- Number of threads used.
- Number of repetitions. In a standard run we use 10 repetitions of the same data set.

## Collect timing information
MuhRec save the timing information for each reconstruction in the file ```<User home>/.imagingtools/recontiming.json```. 
The entries in the timing file look like
```json
"2022-02-15T16:19:50" : {
        "data":{    "projections":625, 
                    "sizeu":555, 
                    "sizev":100, 
                    "sizex":555, 
                    "sizey":555, 
                    "sizez":100},
        "timing":{  "FullLogNorm":213.735, 
                    "MorphSpotClean":1432.24, 
                    "Multi projection BP parallel":8995.26, 
                    "WaveletRingClean":977.15, 
                    "total":24378.8}
    }
```

The file needs to be edited before loading it in the evaluation notebook. The last line ends with a comma ```,```, please replace by a linefeed and closing curly bracket ```}```.


