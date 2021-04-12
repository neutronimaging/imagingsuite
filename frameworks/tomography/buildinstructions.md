# Build instructions

## Create directories for the build
```bash 
mkdir build
mkdir install
```

## Build the module
```bash
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../install -DDYNAMIC_LIB=ON
cmake --build . --target install
cd ..
```

## Post build actions
```bash 
cd install/lib
../../nexuslinkage.sh
for f in `ls *.1.0.0.*`; do ln -s $f `basename $f .1.0.0.dylib`.1.dylib; done
```
