#!/bin/bash

# Variables
APP_NAME="MuhRec"
APP_BUNDLE="MuhRec.app"
DMG_VOLUME_NAME="MuhRec Installer"
SOURCE_DIR="$HOME/git/build-imagingsuite/Release"  # Path to the app
BACKGROUND_IMG="$HOME/git/imagingsuite/applications/muhrec/doc/icon/muh4_install.png"
ICON_FILE="$HOME/git/imagingsuite/applications/muhrec/src/muh4_icon.icns"  # Path to the custom .icns file
DEST_DIR="$HOME/git/build-imagingsuite"  # Destination directory for building and result
BUILD_DIR="$DEST_DIR/dmg-build"  # Temporary build directory

# Generate the date and Git commit hash
CURRENT_DATE=$(date +%Y-%m-%d)
GIT_COMMIT=$(cd "$HOME/git/imagingsuite" && git rev-parse --short HEAD)

# Construct the .dmg file name
DMG_NAME="${APP_NAME}_${CURRENT_DATE}_${GIT_COMMIT}.dmg"

# Ensure the destination directory exists
mkdir -p "$DEST_DIR"
rm -rf "$BUILD_DIR"  # Clean up any previous build directory
mkdir -p "$BUILD_DIR/.background"

# Copy the app bundle
cp -R "$SOURCE_DIR/$APP_BUNDLE" "$BUILD_DIR"

# Copy the background image
cp "$BACKGROUND_IMG" "$BUILD_DIR/.background/background.png"

# Create a symbolic link to /Applications
ln -s /Applications "$BUILD_DIR/Applications"

# Create the DMG file
hdiutil create -volname "$DMG_VOLUME_NAME" \
  -srcfolder "$BUILD_DIR" \
  -ov \
  -format UDZO \
  "$DEST_DIR/$DMG_NAME"

# Mount the DMG to customize it
MOUNT_DIR=$(hdiutil attach "$DEST_DIR/$DMG_NAME" -mountpoint /Volumes/"$DMG_VOLUME_NAME" | grep "/Volumes/" | awk '{print $3}')
sleep 2  # Wait for the DMG to mount properly

# Customize the DMG layout
echo '
tell application "Finder"
    tell disk "'$DMG_VOLUME_NAME'"
        open
        set current view of container window to icon view
        set toolbar visible of container window to false
        set statusbar visible of container window to false
        set the bounds of container window to {100, 100, 600, 400}
        set theViewOptions to the icon view options of container window
        set arrangement of theViewOptions to not arranged
        set icon size of theViewOptions to 128
        set background picture of theViewOptions to file ".background:background.png"
        make new alias file at container window to POSIX file "/Applications" with properties {name:"Applications"}
        set position of item "'$APP_BUNDLE'" of container window to {150, 200}
        set position of item "Applications" of container window to {450, 200}
        close
        open
        update without registering applications
    end tell
end tell
' | osascript

# Detach the DMG
hdiutil detach "$MOUNT_DIR"

# Add the custom icon to the DMG
if [ -f "$ICON_FILE" ]; then
  cp "$ICON_FILE" "$BUILD_DIR/.VolumeIcon.icns"
  SetFile -a C "$BUILD_DIR/.VolumeIcon.icns"  # Enable the custom icon for the DMG
  hdiutil create -ov -srcfolder "$BUILD_DIR" -volname "$DMG_VOLUME_NAME" -format UDZO "$DEST_DIR/$DMG_NAME"
else
  echo "Icon file not found: $ICON_FILE"
fi

# Clean up
rm -rf "$BUILD_DIR"

echo "DMG creation completed: $DEST_DIR/$DMG_NAME"
