This is a collection of libraries required by kipl and recon on windows machines.
Some are installers and others are source-code 

Look in:
http://pecl2.php.net/downloads/php-windows-builds/php-libs/VC8/x64/
http://ftp.gnome.org/pub/gnome/binaries/win64/dependencies/

binaries for
-zlib
-libxml2
-libjpeg
-libpng
-libtiff

  fftw3
=========
Download from
http://www.fftw.org/install/windows.html
There are further instructions

Make a lib file by using
lib /machine:x64 /def:libfftw3-3.def

Compile on Mac/linux
./configure --enable-float --enable-shared
make ; sudo make install
./configure --enable-shared
make ; sudo make install

  cppunit
===========
Compile the solution in the example folder

  Cfitsio
===========
Compile the solution in the cfitsio/cfitsio folder

GSL
==============
version 1.8
http://gnuwin32.sourceforge.net/packages/gsl.htm

Create a lib file
Open a MSVC command prompt
The lib folder and execute
lib /def:libgsl.def
