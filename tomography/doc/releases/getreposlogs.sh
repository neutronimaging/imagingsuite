#!/bin/bash

for d in `ls ~/repos`; do
	dir=~/repos/$d

	if [ -d "$dir/.svn" ]; then 
		echo $dir	
		svn log $dir -v -r {2016-08-01}:{2017-06-07} > $d.log
	fi
done