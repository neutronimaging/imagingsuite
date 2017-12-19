#!/bin/bash
./build_core_kipl.sh
./build_GUI.sh # needs to be here due to GUI components in modules
./build_core_modules.sh
./build_core_algorithms.sh

./build_frameworks_tomography.sh
./build_frameworks_imageprocessing.sh

./build_applications_NIQA.sh
./build_applications_imageviewer.sh
./build_applications_kipltool.sh
./build_applications_muhrec.sh

