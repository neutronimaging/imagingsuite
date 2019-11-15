#!/bin/sh

mkdir /tmp/NIQA
ln -s /Applications /tmp/NIQA/Applications
cp -r ~/Applications/NIQA.app /tmp/NIQA

hdiutil create -volname NIQA -srcfolder /tmp/NIQA -ov -format UDZO ~/NIQA_`date +%Y%m%d`.dmg

