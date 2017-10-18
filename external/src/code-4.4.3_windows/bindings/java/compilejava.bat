@echo off
REM Compile java files under Win32
REM
javac -g ncsa/hdf/hdflib/*.java
javac -g neutron/nexus/*.java
jar -cvf jnexus.jar ncsa/hdf/hdflib/*.class neutron/nexus/*.class
