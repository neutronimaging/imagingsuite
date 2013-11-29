CPCMD="cp" 

DESTPATH="qt/QtKipTool-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Release/QtKipTool.app/Contents/MacOS/"

`$CPCMD ../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/libProcessFramework.1.0.0.dylib $DESTPATH`
`$CPCMD ../AdvancedFilterModules/qt/AdvancedFilterModules-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Release/libAdvancedFilterModules.1.0.0.dylib $DESTPATH`
`$CPCMD ../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libkipl.1.0.0.dylib $DESTPATH`
`$CPCMD ../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libModuleConfig.1.0.0.dylib $DESTPATH`
`$CPCMD ../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libQtAddons.1.0.0.dylib $DESTPATH`
`$CPCMD ../../../gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/libQtModuleConfigure.1.0.0.dylib $DESTPATH`

cd $DESTPATH

for f in `ls *.1.0.0.dylib`; do
	ln -s $f "`basename $f .1.0.0.dylib`.1.0.dylib"
	ln -s $f "`basename $f .1.0.0.dylib`.1.dylib"
done