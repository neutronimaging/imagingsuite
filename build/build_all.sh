#!/bin/bash

REPOSPATH=$WORKSPACE/imagingsuite

$REPOSPATH/build/build_core_kipl.sh
$REPOSPATH/build/build_core_algorithms.sh

#if [ -d "$WORKSPACE/ToFImaging" ] 
#then
#    echo "Directory $WORKSPACE/ToFImaging exists."
#    ./build_lmfit.sh
#    cd $WORKSPACE/ToFImaging/build/
#    ./build_tofimagingalgorithms.sh
#    cd $WORKSPACE/imagingsuite/build/
#else
#    echo "Warning: Directory $WORKSPACE/ToFImaging does not exists."
#fi

$REPOSPATH/build/build_core_modules.sh
$REPOSPATH/build/build_GUI.sh # needs to be here due to GUI components in modules

$REPOSPATH/build/build_frameworks_tomography.sh

$REPOSPATH/build/build_applications_imageviewer.sh
$REPOSPATH/build/build_applications_muhrec.sh
# $REPOSPATH/build_CLI_framesplitter.sh
#./build_CLI_muhrec.sh
#../../ImagingQuality/build/build_applications_NIQA.sh
