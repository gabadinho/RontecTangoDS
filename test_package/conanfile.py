import os
from conan import ConanFile
from conan.tools.build import can_run


class RontecTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        self.requires(self.tested_reference_str)

    def test(self):
        if can_run(self):
            if self.settings.os == "Windows":
                self.run("ds_Rontec 2>&1 | findstr \"usage\"", env="conanrun")
            else:
                self.run("ds_Rontec 2>&1 | grep \"^usage\"", env="conanrun")
                