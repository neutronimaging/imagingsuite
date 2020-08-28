#!/bin/sh
name=`basename $1 .png`
convert $1 -resize 256x256 "$name-256.png"
convert $1 -resize 128x128 "$name-128.png"
convert $1 -resize 96x96 "$name-96.png"
convert $1 -resize 64x64 "$name-64.png"
convert $1 -resize 48x48 "$name-48.png"
convert $1 -resize 32x32 "$name-32.png"
convert $1 -resize 16x16 "$name-16.png"

convert "$name-128.png" "$name-96.png" "$name-64.png" "$name-48.png" "$name-32.png" "$name-16.png" "$name.ico"
