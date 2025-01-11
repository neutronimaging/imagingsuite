# Explaining the script: `create_muhrec_deb.sh`**

## The script code
```bash
#!/bin/bash

# Define variables
PACKAGE_NAME="muhrec"
VERSION="1.0"  # Update this version as needed
ARCHITECTURE="amd64"
MAINTAINER="Your Name <your.email@example.com>"  # Replace with your details
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
cp -r "$BUILD_DIR/Plugins" "$PACKAGE_ROOT/opt/$PACKAGE_NAME/"
cp -r "$BUILD_DIR/Resources" "$PACKAGE_ROOT/opt/$PACKAGE_NAME/"

# Copy the script and make it executable
mkdir -p "$PACKAGE_ROOT/opt/$PACKAGE_NAME/scripts"
cp "$SCRIPT_DIR/muhrec" "$PACKAGE_ROOT/opt/$PACKAGE_NAME/scripts/"
chmod +x "$PACKAGE_ROOT/opt/$PACKAGE_NAME/scripts/muhrec"

# Create a symlink in /usr/bin for easy execution
mkdir -p "$PACKAGE_ROOT/usr/bin"
ln -s /opt/$PACKAGE_NAME/scripts/muhrec "$PACKAGE_ROOT/usr/bin/muhrec"

# Set ownership and permissions
sudo chown -R root:root "$PACKAGE_ROOT"
sudo chmod -R 755 "$PACKAGE_ROOT"

# Build the .deb package
mkdir -p "$OUTPUT_DIR/output"
dpkg-deb --build "$PACKAGE_ROOT" "$OUTPUT_DIR/output"

echo "Package created successfully at $OUTPUT_DIR/output/${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb"
```

---

### **Key Changes**
1. **`postinst` Script**:
   - This script adds `/opt/muhrec/scripts` to the `PATH` by appending it to `~/.profile` if it’s not already there.
   - It informs the user to restart their shell or source `~/.profile` to apply the change.

2. **File Permissions**:
   - The `postinst` script is set to executable (`chmod 755`).

3. **PATH Adjustment**:
   - Ensures that the `muhrec` script can be called without specifying its full path, even if the user’s shell session doesn’t include `/usr/bin/muhrec`.

---

### **Usage**
1. Save the script as `create_muhrec_deb.sh`.
2. Make it executable:
   ```bash
   chmod +x create_muhrec_deb.sh
   ```
3. Run the script:
   ```bash
   ./create_muhrec_deb.sh
   ```
4. Install the package:
   ```bash
   sudo dpkg -i ~/muhrec-deb/output/muhrec_1.0_amd64.deb
   ```
5. Restart the shell or source `~/.profile`:
   ```bash
   source ~/.profile
   ```

### **Effect**
After installation:
- The `muhrec` script will be directly executable without specifying the full path.
- The package files will be located in `/opt/muhrec/`.

