#!/usr/bin/env python

print("This line will be printed.")

projpath="/Volumes/DataDisk/P20140142"
destpath=projpath+"/04_evaluation/20141126"
muhrec="/Users/kaestner/Applications/muhrec3.app/Contents/MacOS/muhrec3"
cfgpath=projpath+"/04_evaluation/20141126/recon_roots.xml"

from subprocess import call
from math import fmod
firstproj=1;
firstframe=0
lastframe=3
for i in range(firstframe,lastframe) :
	# select projection sub set
	firstindex="projections:firstindex="+str(firstproj+i*180)
	lastindex="projections:lastindex="+str(firstproj+(i+1)*180)
	# set file mask for the slices
	matrixname="matrix:matrixname=frame_"+("%04d" % i)+"-slice_####.tif"
	# adjust the reconstruction angles to alternating between 0-180 and 180-360
	angle=fmod(i,2)*180 
	scanarc="'projections:scanarc="+str(angle)+" "+str(angle+180)+"'"
	# call the reconstruction
	call([muhrec, "-f", cfgpath, firstindex, lastindex, matrixname, scanarc])
