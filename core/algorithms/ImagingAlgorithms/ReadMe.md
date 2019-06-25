# Instructions to compile and run python bindings on Imaging algorithms
1. Build 'kipl' as this is a required dependency
2. Build the library using the following commands from the '<git root>/imagingsuite/core/algorithms/' folder: 
```bash
$ mkdir - p build
$ cd build
$ cmake ..
$ cmake --build .
```
3. Run the script 'copydeps.sh' to copy needed libs 
4. Use the module with the python version you compiled with.  
5. A test script would be 
  ```python
  import imagalg
  
  a=imagalg.AverageImage
  b=a.ImageWeightedAverage
  a.windowSize=7
  ```
