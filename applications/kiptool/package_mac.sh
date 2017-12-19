#!/bin/sh

mkdir /tmp/kiptool
ln -s /Applications /tmp/kiptool/Applications
cp -r ~/Applications/QtKipTool.app /tmp/kiptool

hdiutil create -volname KipTool -srcfolder /tmp/kiptool -ov -format UDZO ~/KipTool_`date +%Y%m%d`.dmg

