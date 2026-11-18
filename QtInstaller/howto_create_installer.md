Step 0: Ensure you have installed the Qt Installer Framework tool from the Qt Installer (otherwise, open the Qt maintenance tool and add it, it is found under build)

Step 1: Build the application as normal (The steps are outlined on GitHub)

Step 2: Copy the files from build-imagingsuite to the QtInstaller/packages/core/data folder:

# Windows
from imagingsuite\QtInstaller in a bash terminal:
    cp -r ../../build-imagingsuite/bin/Release/* packages/core/data/
    rm packages/core/data/*.pyd
    rm packages/core/data/t*.exe

# Linux
from build-imagingsuite/Release in a bash terminal:
    cp -r bin lib resources ../../imagingsuite/QtInstaller/packages/core/data/
    cd ../../imagingsuite/QtInstaller/packages/core/data/
    rm bin/t* lib/*cpython*

# Macos
from imagingsuite\QtInstaller in a bash terminal:
    cp -r ../../build-imagingsuite/Release/*.app packages/core/data/

Step 3: Build the installer

# Windows
from imagingsuite/QtInstaller in a bash terminal
    ${QTPATH}/../../Tools/QtInstallerFramework/4.8/bin/binarycreator.exe -c config/config.xml -p packages Windows_MuhRec_Installer.exe

# Linux/MacOS
from imagingsuite/QtInstaller in a bash terminal
    ${QTPATH}/../../Tools/QtInstallerFramework/4.8/bin/binarycreator -c config/config.xml -p packages OS_Arch_MuhRec_Installer