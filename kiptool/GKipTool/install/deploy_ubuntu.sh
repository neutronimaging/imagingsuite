#!/bin/bash
# CPCMD = "ln -s"
CPCMD="cp -r "
repos_path=~/repos/src
base_path=~/bin/kiptool
app_path=$repos_path/apps/kiptool/trunk
lib_path=$repos_path/libs

if  [ ! -d $base_path ]; then
	mkdir $base_path
	mkdir $base_path/lib
	mkdir $base_path/bin
	mkdir $base_path/resources
fi

`$CPCMD $app_path/kiptool/resources/* $base_path/resources`
`$CPCMD $app_path/kiptool/Release/kiptool $base_path/bin`
`$CPCMD $lib_path/gtk_addons/trunk/gtkmm_addons/Release/libgtkmm_addons.so $base_path/lib`
`$CPCMD $lib_path/modules/trunk/ModuleConfig/Release/libModuleConfig.so $base_path/lib`
`$CPCMD $lib_path/modules/trunk/ModuleConfigurator/Release/libModuleConfigurator.so $base_path/lib`
`$CPCMD $lib_path/kipl/trunk/ProcessFramework/Release/libProcessFramework.so $base_path/lib`
`$CPCMD $app_path/AdvancedFilterModules/Release/libAdvancedFilterModules.so $base_path/lib`
`$CPCMD $app_path/BaseModules/Release/libBaseModules.so $base_path/lib`
`$CPCMD $app_path/ClassificationModules/Release/libClassificationModules.so $base_path/lib`

`$CPCMD $app_path/kiptool/install/kiptool $base_path`


