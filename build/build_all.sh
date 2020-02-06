#!/bin/bash
./build_core_kipl.sh
./build_core_algorithms.sh
./build_core_modules.sh
./build_GUI.sh # needs to be here due to GUI components in modules

if [ -d "$WORKSPACE/TOFImaging" ] 
then
    echo "Directory $WORKSPACE/TOFImaging exists."
    ./$WORKSPACE/TOFImaging/build/build_tofimagingalgorithms.sh
else
    echo "Warning: Directory $WORKSPACE/TOFImaging does not exists."
fi

./build_frameworks_tomography.sh
./build_frameworks_imageprocessing.sh

#../../ImagingQuality/build/build_applications_NIQA.sh
./build_applications_imageviewer.sh
./build_applications_kiptool.sh
./build_applications_muhrec.sh
#./build_CLI_framesplitter.sh
#./build_CLI_muhrec.sh

