#!/bin/sh
# at the moment hdf4 only support -arch i386 -arch ppc 
# and nexus tests fail if hdf5 is build with -arch x86_64 -arch ppc64
# so leave as just "-arch i386 -arch ppc" for the moment


echo "Removing old package root (require root privileges)"
test -d package_root && sudo rm -fr package_root

cd ..
echo "Building NeXus"
make distclean # needed as we disable dependency tracking
arch_flags="-arch i386 -arch ppc"
export MACOSX_DEPLOYMENT_TARGET=10.4
env CFLAGS="-isysroot /Developer/SDKs/MacOSX10.4u.sdk $arch_flags" \
    CXXFLAGS="-isysroot /Developer/SDKs/MacOSX10.4u.sdk $arch_flags" \
    LDFLAGS="-Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk $arch_flags" \
    ./configure --with-hdf4=/usr/local/hdf4.2r4 \
    --with-hdf5=/usr/local/hdf5-1.8.2 --with-python \
    --disable-dependency-tracking
make
make install DESTDIR=`pwd`/macosx_install_kit/package_root
cp InstallerBits/Licences/*.txt macosx_install_kit/package_root/usr/local/share/nexus

cd macosx_install_kit

# Use pkg.config to fetch the package name and version number
. pkg.config
rm -rf "$PACKAGE.pkg" "$PACKAGE-$VERSION$RELEASE"

cp ../NEWS Resources/English.lproj/ReadMe.txt

echo "Building the package (requires root to set permissions)"
ROOT=package_root bin/makepkg.sh pkg.config

echo "Building the disk image"
bin/dmgpack.sh "$PACKAGE-$VERSION$RELEASE" ReadMe.txt "$PACKAGE.pkg"

echo "Enabling internet install"
hdiutil internet-enable -yes "$PACKAGE-$VERSION$RELEASE.dmg"

echo "Cleaning NeXus build (as we configured without dependency tracing)"
( cd ..; make distclean; ) > /dev/null

echo "Removing temporary files (requires root to clear)"
#sudo rm -fr package_root
#rm -rf "$PACKAGE.pkg"
