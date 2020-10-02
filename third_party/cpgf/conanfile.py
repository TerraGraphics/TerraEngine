import os
from conans import ConanFile, CMake, tools


class Cpgf(ConanFile):
    name = "cpgf"
    version = "1.6.1"
    license = "Apache License 2.0"
    url = "https://github.com/cpgf/cpgf"
    description = "cpgf library "
    topics = ("reflection")
    settings = "os", "arch", "compiler", "build_type"
    options: dict = {}
    default_options: dict = {}
    generators = "cmake", "cmake_find_package"

    def source(self):
        tools.download(f"{self.url}/archive/v{self.version}.tar.gz", "src.tar.gz")
        tools.untargz("src.tar.gz")

    def _create_cmake(self):
        cmake = CMake(self)
        cmake.configure(source_folder=os.path.join(f"{self.name}-{self.version}", "build"))
        return cmake

    def build(self):
        cmake = self._create_cmake()
        cmake.build()

    def package(self):
        self.copy("*.a", dst="lib", keep_path=False)
        self.copy("*.h*", dst="include", src=os.path.join(f"{self.name}-{self.version}", "include"))

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
