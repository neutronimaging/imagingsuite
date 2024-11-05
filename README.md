[![Build MuhRec on multiple platforms](https://github.com/neutronimaging/imagingsuite/actions/workflows/Build-multi-OS.yml/badge.svg)](https://github.com/neutronimaging/imagingsuite/actions/workflows/Build-multi-OS.yml)
[![Build pymuhrec for multiple python versions](https://github.com/neutronimaging/imagingsuite/actions/workflows/Build-wheels.yml/badge.svg)](https://github.com/neutronimaging/imagingsuite/actions/workflows/Build-wheels.yml)
# The imaging suite core
The imaging suite repository contains the core functionality sources for and documentation related to MuhRec, KipTool, and nGITool.
The project has a [public webpage](https://neutronimaging.github.io/) where you get some more information.

## Documentation
- [MuhRec](https://github.com/neutronimaging/imagingsuite/wiki/User-manual-MuhRec)
- [KipTool](https://github.com/neutronimaging/imagingsuite/wiki/User-manual-KipTool)

# Contributing
-[How to contribute](https://github.com/neutronimaging/imagingsuite/blob/master/CONTRIBUTING.md)

# Build instructions
- [Prerequisities](https://github.com/neutronimaging/imagingsuite/wiki/Prerequisites-for-building)
- [How to build](https://github.com/neutronimaging/imagingsuite/wiki/Build-instructions)

## Building python bindings
Python bindings are implemented to allow the use of the modules in python. The bindings are implemented using PYBIND11 and you need to use cmake to build them.

### First step create directories for the build
Create build and install folders in the ```path``` where you want them
```bash 
mkdir <path>/build
mkdir <path>/install
```

#### MuhRec back-projectors
The cmake file to build the backprojector bindings located located in the folder ````imagingsuite/framework/tomography````.

Steps to build:
```bash
cmake ../imagingsuite -DCMAKE_INSTALL_PREFIX=../install -DDYNAMIC_LIB=ON -DCMAKE_PREFIX_PATH=<path to Qt version>/macos
```
##### Build the module
```bash
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../install -DDYNAMIC_LIB=ON
cmake --build . --target install
cd ..
```

##### Post build actions
```bash 
cd install/lib
for f in `ls *.1.0.0.*`; do ln -s $f `basename $f .1.0.0.dylib`.1.dylib; done

```
### Testing the build
cd ../python
PYTHONPATH=$PYTHONPATH:../install python3

```python
import imgalg
import muhrectomo
```




