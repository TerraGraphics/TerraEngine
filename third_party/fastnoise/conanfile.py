import os
from conans import ConanFile, CMake, tools


class FastNoise(ConanFile):
    name = "fastnoise"
    version = "0.4.faba444"
    license = "MIT License"
    url = "https://github.com/Auburns/FastNoise"
    description = "coherent noise-generating library for C++"
    topics = ("noise")
    settings = "os", "compiler", "build_type", "arch"
    options = { }
    default_options = { }
    generators = "cmake"
    exports_sources = "CMakeLists.txt"

    def source(self):
        self.run("git clone %s" % FastNoise.url)
        commit_version = "faba444"
        with tools.chdir("FastNoise"):
            self.run("git reset --hard %s" % commit_version)

    def _create_cmake(self):
        cmake = CMake(self)
        cmake.definitions["FN_DECIMAL"] = 1
        cmake.configure()
        return cmake

    def build(self):
        cmake = self._create_cmake()
        cmake.build()

    def package(self):
        cmake = self._create_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
