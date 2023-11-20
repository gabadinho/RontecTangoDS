from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class RontecRecipe(ConanFile):
    name = "rontec"
    executable = "ds_Rontec"
    version = "1.1.1"
    package_type = "application"
    user = "soleil"

    license = "GPL-3.0-or-later"
    author = "Jean Coquet"
    url = "https://gitlab.synchrotron-soleil.fr/software-control-system/tango-devices/acquisition/1d/rontec.git"
    description = "Rontec device"
    topics = ("control-system", "tango", "device")

    settings = "os", "compiler", "build_type", "arch"

    exports_sources = "CMakeLists.txt", "src/*"
    
    def requirements(self):
        self.requires("cpptango/9.2.5@soleil/stable")
        if self.settings.os == "Linux":
            self.requires("crashreporting2/[>=1.0]@soleil/stable")
    
    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.variables["PROJECT_NAME"] = self.name
        tc.variables["PROJECT_VERSION"] = self.version
        tc.variables["EXECUTABLE_NAME"] = self.executable
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
