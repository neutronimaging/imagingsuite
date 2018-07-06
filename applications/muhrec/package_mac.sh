#!/bin/sh

mkdir /tmp/MuhRec
ln -s /Applications /tmp/muhrec3/Applications
cp -r ~/Applications/MuhRec.app /tmp/MuhRec

hdiutil create -volname MuhRec -srcfolder /tmp/muhrec -ov -format UDZO ~/MuhRec_`date +%Y%m%d`.dmg

