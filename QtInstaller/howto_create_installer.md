Step 1: Copy the files from build-imagingsuite to the QtInstaller/packages/core/data folder:

# Windows
from imagingsuite\QtInstaller in a bash terminal:
    cp -r ../../build-imagingsuite/bin/Release/* packages/core/data/
    rm packages/core/data/*.pyd
    rm packages/core/data/t*.exe

# Linux
from imagingsuite\QtInstaller in a bash terminal:
    cp -r ../../build-imagingsuite/bin/Release/* packages/core/data/
    rm packages/core/data/*.pyd
    rm packages/core/data/t*.exe

Step 2: Build the installer

# Windows
from imagingsuite\QtInstaller in a terminal
    %QTPATH%\..\..\Tools\QtInstallerFramework\4.8\bin\binarycreator.exe -c config\config.xml -p packages Windows_MuhRec_Installer.exe
