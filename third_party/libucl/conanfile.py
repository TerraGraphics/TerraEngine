import os
from conans import ConanFile, CMake, tools


class Libucl(ConanFile):
    name = "libucl"
    version = "0.8.1"
    license = 'BSD 2-Clause "Simplified" License'
    url = "https://github.com/vstakhov/libucl"
    description = "Universal configuration library parser"
    topics = ("config")
    settings = "os", "arch", "compiler", "build_type"
    options: dict = {
        "shared": [True, False],
    }
    default_options: dict = {
        "shared": False,
    }
    generators = "cmake", "cmake_find_package"

    def source(self):
        tools.download(f"{self.url}/archive/{self.version}.tar.gz", "src.tar.gz")
        tools.untargz("src.tar.gz")

    def _create_cmake(self):
        cmake = CMake(self)
        cmake.configure(source_folder=f"{self.name}-{self.version}")
        return cmake

    def build(self):
        cmake = self._create_cmake()
        cmake.build()

    def package(self):
        self.copy("*.a", dst="lib", keep_path=False)
        self.copy("*.h*", dst="include", src=os.path.join(f"{self.name}-{self.version}", "include"))

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
