import os

from conan import ConanFile

class MuhrecRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

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
        self.requires("qt/[6.6.1]")

    def build_requirements(self):
        self.tool_requires("cmake/3.28.1") # Only used if conanbuild.bat environment is used

    def layout(self):
        # We make the assumption that if the compiler is msvc the
        # CMake generator is multi-config
        # System independent way of setting the correct build path
        multi = True if self.settings.get_safe("compiler") == "msvc" else False
        if multi:
            self.folders.generators = os.path.join(os.pardir, "build-imagingsuite", "generators")
            self.folders.build = os.path.join(os.pardir, "build-imagingsuite")
        else:
            self.folders.generators = os.path.join(os.pardir, "build-imagingsuite", str(self.settings.build_type), "generators")
            self.folders.build = os.path.join(os.pardir, "build-imagingsuite", str(self.settings.build_type))
