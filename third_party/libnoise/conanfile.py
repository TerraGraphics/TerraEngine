import os
from conans import ConanFile, CMake, tools


class Libnoise(ConanFile):
    name = "libnoise"
    version = "1.0.032caf6"
    license = 'GNU Lesser General Public License v2.1'
    url = "https://github.com/qknight/libnoise"
    description = "coherent noise-generating library for C++"
    topics = ("noise")
    settings = "os", "compiler", "build_type", "arch"
    options = { }
    default_options = { }
    generators = "cmake"

    def source(self):
        self.run("git clone %s ." % Libnoise.url)
        commit_version = "032caf6"
        self.run("git reset --hard %s" % commit_version)

    def _create_cmake(self):
        cmake = CMake(self)
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
