#!/bin/sh

if test "$1" = "-?"; then
    cat <<EOF
Usage:

  VERSION=major.minor RELEASE=.patch mkpkg.sh Config


=== Config file ===

The Package configuration file is a set of bash script defines 

Required Keys
  PACKAGE=""           Name of package file
  BUNDLE=""            Bundle identifier such as org.nexusformat.napi
  VERSION=""           Major.minor version number
                       Use shell commands to extract the VERSION and RELEASE 
                       from your source

Optional Keys
  ROOT="Root"          See Installed Files below
  RESOURCES="Resources" See Resource Files below
  RELEASE=""           Patch level such as ".2" or "rc1" or "a1"
  COPYRIGHT=""         Name of copyright holder (ignored if not present)
  YEAR="start-stop"    Use "2001-`date +%Y`" to go from 2001 to this year
  InfoString="$VERSION$RELEASE, (C) $YEAR $COPYRIGHT"
                       Package info
  MajorVersion="VERSION up to the first dot"
  MinorVersion="VERSION after the last dot"
                       Major.Minor version number integers
  BackgroundAlignment="center"
                       Alignment of background picture, which should be one of
                       center left right top bottom (top|bottom)(left|right)
  BackgroundScaling="tofit"
                       Scaling of background picture, which should be one of
                       none proportional tofit
  AuthorizationAction="NoAuthorization"
                       Required authorization, which should be one of
                       (Admin|Root|No)Authorization
  RestartAction="NoRestart"
                       Require restart?  Should be one of NoRestart Shutdown
                       RecommendedRestart RequiredRestart RequiredLogout
  DefaultLocation="/"  Default location
  Relocatable=false    true if package is relocatable
  FollowLinks=true     false if links should be replaced with file content
  InstallFat=false     true if distributing development libraries
  IsRequired=false     true if package is not optional
  OverwritePermissions=false   
                       true if you want to mess with system permissions
  RootVolumeOnly=false
                       true if you need to install on the root volume
  UpdateInstalledLanguages=false
                       true if only install resources for the current languages
  AllowBackRev=false   true if old versions can replace new

=== Installed Files ===

The Root directory contains the complete package layout as expected from
the install location.

The ownership of all files will be changed to root and permissions to 
world readable.  Make sure executables already have the executable bit set.

=== Resource Files ===

Resources contains install scripts and texts.  All resources are optional.

Scripts can be run at various stages in the install process.  They should
be named as follows:

    preflight preinstall/preupgrade postinstall/postupgrade postflight

Within the script files, the following are defined:
     
   $1: package source directory
       e.g., /Volumes/Users/michelle/Desktop/Levon.pkg
   $2: installation destination  
       e.g., /Applications
   $3: destination volume  
       e.g., /Volumes/Tools  or  /
   $4: system root directory
       e.g., /
   $PACKAGE_PATH: package source directory
       = $1
   $SCRIPT_NAME: currently running script
       e.g., preflight
   $INSTALLER_TEMP: working directory
       e.g., /private/tmp/.Levon.pkg.897.install
   $RECEIPT_PATH: directory containing the script
       e.g., $INSTALLER_TEMP/Receipts/Levon.pkg/Contents/Resources
    
Be sure to mark the scripts as executables.

Texts can be in the Resources directory for the package, or in a language
specific subdirectory such as English.proj. The texts include:

   Welcome.{rtf,rtfd,html,txt} - first page of the installer
   Readme.{rtf,rtfd,html,txt}  - second page of the installer
   License.{rtf,rtfd,html,txt} - license description
   Conclusion.{rtf,rtfd,html,txt} - final page 
   Description.plist  - package description (see below)
   Background.{tiff,jpg,gif,pict,eps,pdf} - installer image

Description.plist is a property list which should contain keys for
IFPkgDescriptionTitle and IFPkgDescriptionVersion

<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd ">
<plist version="1.0">
<dict>
   <key>IFPkgDescriptionTitle</key>
   <string> insert title here </string>
   <key>IFPkgDescriptionDescription</key>
   <string> insert description here </string>
</dict>
</plist>

=== References ===

  Software Delivery Guide - Managed Installs
  http://developer.apple.com/documentation/DeveloperTools/Conceptual/SoftwareDistribution/Managed_Installs/Managed_Installs.htm

  PackageMaker How-to
  http://s.sudre.free.fr/Stuff/PackageMaker_Howto.html

  Installer Keys
  http://developer.apple.com/documentation/DeveloperTools/Conceptual/SoftwareDistribution4/Concepts/sd_pkg_flags.html

EOF
    exit
fi


function err { echo $* && exit; }
test -n "$1" || err "usage: makepkg [-? | pkg.config]"
source $1
test -n "$PACKAGE" -a -n "$VERSION" -a -n "$BUNDLE" || err "required keywords missing. use 'makepkg -?' for help"

# Check that sources exist
: ${ROOT:=Root}
: ${RESOURCES:=Resources}

test -d "$ROOT" -a -d "$RESOURCES" || err "need to create $ROOT and $RESOURCES"

# Prepare package directory, removing existing package directory first
test -d "$PACKAGE.pkg" && rm -rf "$PACKAGE.pkg"
target="$PACKAGE.pkg/Contents"
echo "creating $target"
mkdir -p "$target"
cp -Rpv $RESOURCES "$target/Resources"
echo "Purging .svn directories from $target"
find -d "$target" -type d -name ".svn" -exec rm -fr "{}" \; -print

# Purge .DS_STORE; protect against spaces if filename
echo "Purging .DS_Store files from install files (requires root) ..."
find "$ROOT" -name ".DS_Store" | sed '-es/^/sudo rm -f "/;s/$/"/' | sh

# Reset permissions on the distribution files
echo "Setting permissions on install files (requires root) ..."
set -x
# Files readable by everybody, writable by ownder
sudo chmod -R a+r,go-w $ROOT/*
# Extend executable bit to all users; protect against spaces in filename
find "$ROOT" -perm +u=x | sed '-es/^/sudo chmod a+x "/;s/$/"/' | sh
# Most directories are owned by root:admin
sudo chown -R root:admin $ROOT/*
# /usr directory is owned by root:wheel
test -d $ROOT/usr && sudo chown -R root:wheel $ROOT/usr
# Turn on sticky bit for /Library directory
test -d $ROOT/Library && sudo chmod u+t $ROOT/Library
set +x

# Build the archives and bill of materials
echo "building archives"
( cd $ROOT; pax -w . | gzip -9 -c ) > "$target/Archive.pax.gz"  || err "Could not create pax"
mkbom $ROOT "$target/Archive.bom" || err "Could not create bom"

SIZE=`du -sk $ROOT | sed -e"s,^\([0-9]*\)[^0-9].*$,\1,"`
# Build the Info.plist file
: ${RELEASE:=}
: ${BundleIdentifier:=$BUNDLE}
: ${BackgroundAlignment:=left}
: ${BackgroundScaling:=none}
: ${AllowBackRev:=false}
: ${AuthorizationAction:=NoAuthorization}
: ${DefaultLocation:=/}
: ${Relocatable:=false}
: ${FollowLinks:=true}
: ${InstallFat:=false}
: ${IsRequired:=false}
: ${RestartAction:=NoRestart}
: ${OverwritePermissions:=false}
: ${RootVolumeOnly:=false}
: ${UpdateInstalledLanguages:=false}
if test -z "$InfoString"; then
    : ${YEAR:=`date +%Y`}
    InfoString="$VERSION$RELEASE, (c) $YEAR $COPYRIGHT"
    test -z "$YEAR" -a -z "$COPYRIGHT" && InfoString="$VERSION$RELEASE"
fi
: ${InstalledSize:=${SIZE}}
: ${MajorVersion:=${VERSION%.*}}
: ${MinorVersion:=${VERSION#*.}}
: ${ShortVersionString:=$VERSION$RELEASE}

cat <<EOF >"$target/Info.plist"
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleGetInfoString</key>
	<string>$InfoString</string>
	<key>CFBundleIdentifier</key>
	<string>$BundleIdentifier</string>
	<key>CFBundleShortVersionString</key>
	<string>$ShortVersionString</string>
	<key>IFMajorVersion</key>
	<integer>$MajorVersion</integer>
	<key>IFMinorVersion</key>
	<integer>$MinorVersion</integer>
	<key>IFPkgFlagAllowBackRev</key>
	<$AllowBackRev/>
	<key>IFPkgFlagAuthorizationAction</key>
	<string>$AuthorizationAction</string>
	<key>IFPkgFlagBackgroundAlignment</key>
	<string>$BackgroundAlignment</string>
	<key>IFPkgFlagBackgroundScaling</key>
	<string>$BackgroundScaling</string>
	<key>IFPkgFlagDefaultLocation</key>
	<string>$DefaultLocation</string>
	<key>IFPkgFlagFollowLinks</key>
	<$FollowLinks/>
	<key>IFPkgFlagInstallFat</key>
	<$InstallFat/>
	<key>IFPkgFlagInstalledSize</key>
	<integer>$InstalledSize</integer>
	<key>IFPkgFlagIsRequired</key>
	<$IsRequired/>
	<key>IFPkgFlagOverwritePermissions</key>
	<$OverwritePermissions/>
	<key>IFPkgFlagRelocatable</key>
	<$Relocatable/>
	<key>IFPkgFlagRestartAction</key>
	<string>$RestartAction</string>
	<key>IFPkgFlagRootVolumeOnly</key>
	<$RootVolumeOnly/>
	<key>IFPkgFlagUpdateInstalledLanguages</key>
	<$UpdateInstalledLanguages/>
	<key>IFPkgFormatVersion</key>
	<real>0.10000000149011612</real>
</dict>
</plist>
EOF

# Create PkgInfo file, etc
echo pmkrpkg1 > "$target/PkgInfo"
cat <<EOF > "$target/Resources/package_version"
major: $MajorVersion
minor: $MinorVersion
EOF
cat <<EOF > "$target/Resources/BundleVersions.plist"
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict/>
</plist>
EOF

# Set permissions on the package files
chmod -R a+r,a-w "$target"
(cd "$target" && find . -perm +u=x | xargs chmod a+x)
(cd "$target" && find . -type d | xargs chmod u+w)
