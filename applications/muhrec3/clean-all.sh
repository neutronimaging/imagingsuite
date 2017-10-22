#!/bin/sh

for d in `ls -d build-*`; do
	make -f $d/Makefile clean
done

rm -r build-muhrec3-Qt5-Release/muhrec3.app
rm -r build-muhrec3-Qt5-Debug/muhrec3.app