#nexus_related
#install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libkipl.1.0.0.dylib
#install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libkipl.1.0.0.dylib
#install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib libNexus.1.dylib
#install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib libNexusCPP.1.dylib
#install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libNexusCPP.1.dylib

install_name_tool -change @executable_path/libhdf5.10.dylib libhdf5.10.dylib libhdf5_cpp.11.dylib
install_name_tool -change @executable_path/libhdf5.10.dylib libhdf5.10.dylib libhdf5_hl.10.dylib

install_name_tool -change  @executable_path/libhdf5_cpp.11.dylib libhdf5_cpp.11.dylib libNeXus.1.0.0.dylib
install_name_tool -change  @executable_path/libhdf5.10.dylib libhdf5.10.dylib libNeXus.1.0.0.dylib
install_name_tool -change  @executable_path/libhdf5_hl.10.dylib libhdf5_hl.10.dylib libNeXus.1.0.0.dylib
install_name_tool -change  @executable_path/libsz.2.dylib libsz.2.dylib libNeXus.1.0.0.dylib

install_name_tool -change  @executable_path/libhdf5_cpp.11.dylib libhdf5_cpp.11.dylib libNeXusCPP.1.0.0.dylib
install_name_tool -change  @executable_path/libhdf5.10.dylib libhdf5.10.dylib libNeXusCPP.1.0.0.dylib
install_name_tool -change  @executable_path/libhdf5_hl.10.dylib libhdf5_hl.10.dylib libNeXusCPP.1.0.0.dylib
install_name_tool -change  @executable_path/libsz.2.dylib libsz.2.dylib libNeXusCPP.1.0.0.dylib

install_name_tool -change @executable_path/libhdf5.10.dylib libhdf5.10.dylib libhdf5.10.dylib
install_name_tool -change @executable_path/libsz.2.dylib libsz.2.dylib libhdf5.10.dylib
install_name_tool -change /usr/lib/libz.1.dylib libz.1.dylib libhdf5.10.dylib

install_name_tool -change @executable_path/libhdf5_cpp.11.dylib libhdf5_cpp.11.dylib libhdf5_cpp.11.dylib
install_name_tool -change @executable_path/libsz.2.dylib libsz.2.dylib libhdf5_cpp.11.dylib
install_name_tool -change /usr/lib/libz.1.dylib libz.1.dylib libhdf5_cpp.11.dylib

install_name_tool -change @executable_path/libhdf5_hl.10.dylib  libhdf5_hl.10.dylib libhdf5_hl.10.dylib
install_name_tool -change @executable_path/libsz.2.dylib libsz.2.dylib libhdf5_hl.10.dylib
install_name_tool -change /usr/lib/libz.1.dylib libz.1.dylib libhdf5_hl.10.dylib