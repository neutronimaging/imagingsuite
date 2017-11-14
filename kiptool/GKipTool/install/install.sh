#!/bin/bash
repos_path=~/repos/src
base_path=/opt/kiptool
app_path=$repos_path/apps/kiptool/trunk
lib_path=$repos_path/libs

if  [ ! -d $base_path ]; then
	mkdir $base_path
	mkdir $base_path/lib
	cd $base_path
	ln -s $app_path/kiptool/resources .
	ln -s $app_path/kiptool/Release/kiptool .
	ln -s $lib_path/gtk_addons/trunk/gtkmm_addons/Release/libgtkmm_addons.so .
	ln -s $lib_path/modules/trunk/ModuleConfig/Release/libModuleConfig.so .
	ln -s $lib_path/modules/trunk/ModuleConfigurator/Release/libModuleConfigurator.so .
	ln -s $lib_path/kipl/trunk/ProcessFramework/Release/libProcessFramework.so .
	ln -s $app_path/AdvancedFilterModules/Release/libAdvancedFilterModules.so .
	ln -s $app_path/BaseModules/Release/libBaseModules.so .
	ln -s $app_path/ClassificationModules/Release/libClassificationModules.so .

	ln -s $app_path/kiptool/install .
fi



