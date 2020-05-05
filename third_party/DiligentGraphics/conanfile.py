import os
from conans import ConanFile, CMake, tools


class DiligentGraphics(ConanFile):
    name = "DiligentGraphics"
    version = "2.4.37e6c04.c1d83d1"
    license = "Apache License 2.0"
    url = "https://github.com/DiligentGraphics/DiligentGraphics"
    description = " A modern cross-platform low-level graphics library and rendering framework"
    topics = ("gamedev", "graphics-engine", "renderer", "rendering", "graphics-library", "3d-engine")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "development": [True, False],
    }
    default_options = {
        "shared": False,
        "development": False,
    }
    generators = "cmake"
    exports_sources = "DiligentGraphics/*"

    def source(self):
        core_version = "37e6c04"
        self.run("git clone --recursive https://github.com/DiligentGraphics/DiligentCore.git DiligentGraphics/DiligentCore")
        with tools.chdir("DiligentGraphics/DiligentCore"):
            self.run("git reset --hard %s" % core_version)
            self.run('echo "#!/bin/bash" > ./BuildTools/FormatValidation/validate_format_linux.sh')

        tools_version = "c1d83d1"
        self.run("git clone --recursive https://github.com/DiligentGraphics/DiligentTools.git DiligentGraphics/DiligentTools")
        with tools.chdir("DiligentGraphics/DiligentTools"):
            self.run("git reset --hard %s" % tools_version)
            self.run('echo "#!/bin/bash" > ./BuildTools/FormatValidation/validate_format_linux.sh')

    def _create_cmake(self):
        cmake = CMake(self)
        if self.options.development:
            cmake.definitions["DEVELOPMENT"] = 1
        cmake.configure(source_folder="DiligentGraphics")
        return cmake

    def build(self):
        cmake = self._create_cmake()
        cmake.build()

    def package(self):
        cmake = self._create_cmake()
        cmake.install()

    def package_info(self):
        build_type = str(self.settings.build_type)
        self.cpp_info.libdirs = [os.path.join('lib', 'DiligentCore', build_type), os.path.join('lib', 'DiligentTools', build_type)]
        self.cpp_info.libs = tools.collect_libs(self)
