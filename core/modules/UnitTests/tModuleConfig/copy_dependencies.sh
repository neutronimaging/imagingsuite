#!/bin/sh

if [[ $# -eq 0 ]] ; then
	build="Release"   
else
	build="#1"
fi

echo $build
repospath="/Users/kaestner/repos"
destpath="../build-testModuleConfig-Desktop_Qt_5_3_clang_64bit-$build"
pushd .

cd $destpath

`cp $repospath/modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-$build/*.dylib .`
`cp $repospath/kipl/trunk/kipl/build-kipl-Qt5-$build/*.dylib .`
`cp $repospath/src/libs/recon2/trunk/ReconFramework/build-ReconFramework-Qt5-$build/*.dylib .`

popd 