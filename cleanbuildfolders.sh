#!/bin/sh

for d in `find . -type d | grep bit-Release`; 
do
	echo $d
	rm -rf $d
done

for d in `find . -type d | grep bit-Debug`;
do
        echo $d
	rm -rf $d
done

for f in `find . -type f | grep .qmake.stash`;
do 
	echo $f
	rm $f
done

for d in `find . -type d | grep macOS-Debug`;
do
        echo $d
	rm -rf $d
done

for d in `find . -type d | grep macOS-Release`;
do
        echo $d
	rm -rf $d
done

rm -rf ../builds/build*
