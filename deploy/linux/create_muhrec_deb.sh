#!/bin/bash

# Define variables
PACKAGE_NAME="muhrec"
VERSION="1.0"  # Update this version as needed
ARCHITECTURE="amd64"
MAINTAINER="Anders Kaestner <anders.kaestner@psi.ch>"  # Replace with your details
DESCRIPTION="muhrec application package with plugins and resources"
BUILD_DIR="$HOME/git/build-imagingsuite/Release"
SCRIPT_DIR="$HOME/git/imagingsuite/applications/muhrec/scripts"
OUTPUT_DIR="$HOME/$PACKAGE_NAME-deb"
PACKAGE_ROOT="$OUTPUT_DIR/$PACKAGE_NAME"

# Clean up any previous build
rm -rf "$OUTPUT_DIR"
mkdir -p "$PACKAGE_ROOT/DEBIAN"

# Create control file
cat <<EOF > "$PACKAGE_ROOT/DEBIAN/control"
Package: $PACKAGE_NAME
Version: $VERSION
Section: utils
Priority: optional
Architecture: $ARCHITECTURE
Maintainer: $MAINTAINER
Description: $DESCRIPTION
EOF

# Create postinst script for PATH modification
cat <<'EOF' > "$PACKAGE_ROOT/DEBIAN/postinst"
#!/bin/bash
set -e

# Add /opt/muhrec/scripts to PATH in .profile
if ! grep -q 'export PATH=$PATH:/opt/muhrec/scripts' "$HOME/.profile"; then
  echo 'export PATH=$PATH:/opt/muhrec/scripts' >> "$HOME/.profile"
  echo "PATH updated in .profile. Please restart your shell or source .profile to apply changes."
fi

exit 0
EOF

chmod 755 "$PACKAGE_ROOT/DEBIAN/postinst"

# Create the /opt/muhrec directory structure
mkdir -p "$PACKAGE_ROOT/opt/$PACKAGE_NAME"

# Copy the required directories to /opt/muhrec
cp -r "$BUILD_DIR/bin" "$PACKAGE_ROOT/opt/$PACKAGE_NAME/"
cp -r "$BUILD_DIR/lib" "$PACKAGE_ROOT/opt/$PACKAGE_NAME/"
cp -r "$BUILD_DIR/plugins" "$PACKAGE_ROOT/opt/$PACKAGE_NAME/"
cp -r "$BUILD_DIR/resources" "$PACKAGE_ROOT/opt/$PACKAGE_NAME/"

# Copy the script and make it executable
#mkdir -p "$PACKAGE_ROOT/opt/$PACKAGE_NAME/scripts"
cp "$SCRIPT_DIR/muhrec" "$PACKAGE_ROOT/opt/$PACKAGE_NAME/"
chmod +x "$PACKAGE_ROOT/opt/$PACKAGE_NAME/muhrec"

# Create a symlink in /usr/bin for easy execution
mkdir -p "$PACKAGE_ROOT/usr/bin"
ln -s /opt/$PACKAGE_NAME/muhrec "$PACKAGE_ROOT/usr/bin/muhrec"

# Set ownership and permissions
sudo chown -R root:root "$PACKAGE_ROOT"
sudo chmod -R 755 "$PACKAGE_ROOT"

# Build the .deb package
mkdir -p "$OUTPUT_DIR/output"
dpkg-deb --build "$PACKAGE_ROOT" "$OUTPUT_DIR/output"

echo "Package created successfully at $OUTPUT_DIR/output/${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb"

