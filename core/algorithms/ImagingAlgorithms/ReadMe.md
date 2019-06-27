# Instructions to compile and run python bindings on Imaging algorithms
1. INstall support for pybind in your python environment
```bash
<python home>/bin$./pip install pybind11
```
2. Build 'kipl' as this is a required dependency
3. Build the library using the following commands from the '<git root>/imagingsuite/core/algorithms/' folder: 
```bash
$ mkdir - p build
$ cd build
$ cmake ..
$ cmake --build .
```
4. Run the script 'copydeps.sh' to copy needed libs 
5. Use the module with the python version you compiled with.  
6. A test script would be 
  ```python
  import imagalg
  
  a=imagalg.AverageImage
  b=a.ImageWeightedAverage
  a.windowSize=7
  ```
