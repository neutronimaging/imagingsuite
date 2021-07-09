# Building imagingsuite in a Vanilla computer
1. Install ubuntu 20.04 (on hardware or virtual machine)
2. Install needed packages for building the libraries
```bash
sudo apt-get update
sudo apt-get install build-essential git python-is-python3
sudo snap install cmake --classic
sudo apt install python3.8-distutils
sudo apt-get install libarmadillo9 libarmadillo-dev libtiff-dev libcfitsio-dev libfftw3-3 libfftw3-dev libxml2-dev libnexus-dev
```
3. Anaconda, it is easier to install anaconda than trying to install all needed dependencies. 
Install packages for anaconda
```
apt-get install libgl1-mesa-glx libegl1-mesa libxrandr2 libxrandr2 libxss1 libxcursor1 libxcomposite1 libasound2 libxi6 libxtst6
```
4. Download anaconda
```
wget https://repo.anaconda.com/archive/Anaconda3-2021.05-Linux-x86_64.sh
chmod +x Anaconda3-2021.05-Linux-x86_64.sh
./Anaconda3-2021.05-Linux-x86_64.sh
```
and follow the instructions. You may need to reboot your system to get all env variables right for the build.
## Download repository
### Create a folder for repositories

```
mkdir ~/git  
cd ~/git
```

###  Clone the repository 

```
git clone https://github.com/neutronimaging/imagingsuite.git
```

## Start the build 

Prepare for build by creating the folders ```build``` and ```install```:
```
mkdir build install 
cd build
```
Configure and start the build:
```bash
cmake ../imagingsuite -DCMAKE_INSTALL_PREFIX=../install -DDYNAMIC_LIB=ON
cmake --build . --target install
```

alternatively if the cmake doesn't find anaconda python 
```bash
cmake ../imagingsuite -DCMAKE_INSTALL_PREFIX=../install 
-DDYNAMIC_LIB=ON 
-DPYTHON_LIBRARY=$CONDA_PREFIX/lib/libpython3.8m.dylib 
-DPYTHON_INCLUDE_DIR=$CONDA_PREFIX/include/python3.8m/ 
-DPYTHON_EXECUTABLE=$CONDA_PREFIX/bin/python
```

The environment variable ```CONDA_PREFIX``` should be set automatically when you install Anaconda.

## Test the bindings
Once your build is successfull you can try out if it works. First change to the install folder
```bash
cd ../install/lib
```
Then you may have to set the library load path
```bash
LD_LIBRARY_PATH=<the absolute path to install/lib>:$LD_LIBRARY_PATH
```
Start python
```
python
```
Check in the start up banner that it is the anaconda python that started.

Try to import the binder modules
```python
import muhrectomo as mt
import imgalg 
```
Both should load without error. Once you are done, you can stop python by pressing ctrl-D.

