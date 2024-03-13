from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.env import VirtualRunEnv
from conan.tools.files import copy
import os
import shutil

class MuhrecRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    default_options = {"*:shared": True}

    def requirements(self):
        self.requires("zlib/1.3.1")
        self.requires("openblas/0.3.25")
        self.requires("pybind11/[2.11.1]")
        #self.requires("pthreads4w/[>2.11.0]")
        self.requires("libxml2/[2.12.4]")
        self.requires("armadillo/[12.6.4]")
        self.requires("libtiff/[4.6.0]")
        self.requires("fftw/[3.3.10]")
        self.requires("cfitsio/[4.3.1]")
        self.requires("hdf5/1.14.3")
        if self.settings.os == "Windows":
            self.requires("dirent/1.24") # Header files only
        #self.requires("qt/[6.6.1]") Does work but QtCharts is not included

#    def build_requirements(self):
#        self.tool_requires("cmake/3.28.1") # Only used if conanbuild.bat environment is used

    def layout(self):
        cmake_layout(
            self,
            build_folder="../build-imagingsuite",
            )

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()
        ms = VirtualRunEnv(self)
        ms.generate()
        dst = os.path.abspath(os.path.join(self.build_folder, "applications", self.cpp.build.bindir))
        # Copy dynamic libraries from conan
        for dep in self.dependencies.values():
            if len(dep.cpp_info.bindirs)>0: # Avoid errors when using header-only files such as dirent. Can probably be done neater
                copy(self, "*.dll", dep.cpp_info.bindirs[0], dst)
                copy(self, "*.dylib", dep.cpp_info.bindirs[0], dst)
            if len(dep.cpp_info.libdirs)>0:
                copy(self, "*.so*", dep.cpp_info.libdirs[0], dst)
        # Copy dynamic libraries from qt
        qtpath = os.environ["QTPATH"]
        Qt_dynamic_library_list = ["Qt6PrintSupport", "Qt6Charts", "Qt6OpenGLWidgets", "Qt6OpenGl", "Qt6Test"]
        Qt_linux_library_list = ["Qt6Core","Qt6Gui","Qt6Widgets","Qt6DBus","Qt6XcbQpa","icui18n","icudata","icuuc"]
        print(os.listdir(qtpath))
        for library in Qt_dynamic_library_list:
            copy(self, library+".dll", os.path.join(qtpath, "bin"), dst)
            copy(self, library+".dylib", os.path.join(qtpath, "bin"), dst)
            copy(self, "lib"+library+".so*", os.path.join(qtpath, "lib"), dst)
        if self.settings.os == "Linux":
            for library in Qt_linux_library_list:
                copy(self, "lib"+library+".so*", os.path.join(qtpath, "lib"), dst)
        shutil.copytree(os.path.join(self.source_folder,"applications","muhrec","Resources"), os.path.join(dst,"resources"))
        print(os.listdir(dst))
        #if not os.path.exists(os.path.join(dst,"resources")):
        #    os.mkdir(os.path.join(dst,"resources"))
        #copy(self, "*.*", os.path.join(self.source_folder,"applications","muhrec","Resources"), os.path.join(dst,"resources"))
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
