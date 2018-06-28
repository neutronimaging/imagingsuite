#!/bin/sh

fname=~/muhrec_`uname -s`_`uname -m`_`date +%Y%m%d`_rev`svn info |grep Revision | sed -e 's/Revision: //'`.tar.bz2

echo $fname
pushd .
cd 
#tar -jcvhf $fname muhrec
tar -jcvf $fname muhrec
popd
 
