# Building imagingsuite in a Vanilla computer
1. Install ubuntu 20.04 (on hardware or virtual machine)
2. Install needed packages for building the libraries
```
sudo apt-get update
sudo apt-get build-essential git python-is-python3
sudo snap install cmake --classic
sudo apt install python3.8-distutils
sudo apt-get install libarmadillo9 libarmadillo-dev libtiff-dev libcfitsio-dev libfftw3-3 libfftw3-dev libxml2-dev libnexus-dev

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
git clone https://anderskaestner@github.com/neutronimaging/imagingsuite.git
```

## Start the build 

Prepare for build 
```
mkdir build install 
cd build
```

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
