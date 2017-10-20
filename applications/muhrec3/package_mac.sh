#!/bin/sh

mkdir /tmp/muhrec3
ln -s /Applications /tmp/muhrec3/Applications
cp -r ~/Applications/muhrec3.app /tmp/muhrec3

hdiutil create -volname MuhRec3 -srcfolder /tmp/muhrec3 -ov -format UDZO ~/MuhRec3_`date +%Y%m%d`.dmg

