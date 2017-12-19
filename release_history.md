# Release history

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
