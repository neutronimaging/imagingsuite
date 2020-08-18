#!/bin/bash
./build_core_kipl.sh
./build_core_algorithms.sh


if [ -d "$WORKSPACE/ToFImaging" ] 
then
    echo "Directory $WORKSPACE/ToFImaging exists."
    ./build_lmfit.sh
    cd $WORKSPACE/ToFImaging/build/
    ./build_tofimagingalgorithms.sh
    cd $WORKSPACE/imagingsuite/build/
else
    echo "Warning: Directory $WORKSPACE/ToFImaging does not exists."
fi


./build_core_modules.sh
./build_GUI.sh # needs to be here due to GUI components in modules

./build_frameworks_tomography.sh
#./build_frameworks_imageprocessing.sh

./build_applications_imageviewer.sh
./build_applications_muhrec.sh
#./build_CLI_framesplitter.sh
#./build_CLI_muhrec.sh
../../ImagingQuality/build/build_applications_NIQA.sh
