# Release history
## Release note MuhRec 4.0.1 - September 2018
Bug fix release.

### Fixed issues
1. Issue 100: MuhRec - configure geometry dialog first and last slices
2. Issue 123: Add BB instruction in the manual wiki
3. Issue 125: Problems with ROI handling in MuhRec bug
4. Issue 144: CBCT crashes when running from CLI bug
5. Issue 150: Add support for multi-frame tiff in the core library
6. Issue 157: MuhRec - improvements on the BB plugin enhancement

## Release note MuhRec 4.0 - June 2018
In this release we have revised the graphical user interface and added a new module for scattering correction.

### New features:
1. Revised GUI, 
	1. Rearranged the views for a more logic workflow. 
	2. Changed color scheme and started to use colors actively to guide the user.
	3. Introduced an new widget to select ROIs.
2. Added a normalization module that provides more accurate attenuation coefficients and reduces cupping radically.
3. Improved CBCT quality and speed.
4. Transferred the user documentation to the github wiki.
5. The piercing point estimation dialog has been added for the CBCT paramterization.
6. Added several automated code tests for quality assessment.
7. The application now suggests reference images when a new projection is chosen.
8. Logging now appears as a modeless dialog on the side of the main window.
9. We have tried to reduce the height of the main window to allow smaller screens.
10. The application is now compiled on windows 10. There may still be some widget scaling issues.
11. A lot of stability fixes.

### Fixed issues:
1. Issue 8: The piercing point dialog crashes and was deactivated in the previous release.
2. Issue 21: It is now possible open a config file.
3. Issue 19: GUI size doesn't allow very small screens.
4. Issue 42: The statistics calculations (in particular std dev didn't deliver correct values)
5. Issue 59: Tool tips are added on all widgets.
6. Issue 88: It is now possible to run muhrec on the command line with windows.
7. Issue 92: The geometry dialog doesn't crash then the OB is missing.

### Repository:
1. Updated some build scripts.

## Release note MuhRec3 3.14 - December 2017
In the current release we focussed on stability and the open source release including making automated build scripts.

### New features:
1. Improved the performance of the CBCT reconstruction. Including tidying the user interface.
2. Added menu items for bug reporting and to sign up for the news letter.
3. Now using Qt 5.9.3 as base API for the development.
4. Added bound checking to avoid out of bound related crashes.
5. NeXuS reading (BOA style nexus) is supported on all platforms. 

### Fixed issues:
1. GUI clean-up
2. All platforms now have a progress dialog while processing.
3. Many issues with crashes as consequence have been fixed. E.g fixed an issue that made the logging display to crash.

### Repository:
1. MuhRec and its dependencies are now uploaded on GitHub under GPL 3.0 License. Location https://github.com/neutronimaging/imagingsuite
2. Build scripts for automated build are implemented.
Unsolved issues are listed on https://github.com/neutronimaging/imagingsuite/issues
