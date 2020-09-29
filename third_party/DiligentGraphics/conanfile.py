import os
from conans import ConanFile, CMake, tools


class DiligentGraphics(ConanFile):
    name = "DiligentGraphics"
    commit_core_sha = "cfaf74c"
    commit_tools_sha = "6b7ae76"
    version = f"2.4.{commit_core_sha}.{commit_tools_sha}"
    license = "Apache License 2.0"
    url = "https://github.com/DiligentGraphics"
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
        self.run(f"git clone --recursive {self.url}/DiligentCore.git DiligentGraphics/DiligentCore")
        with tools.chdir("DiligentGraphics/DiligentCore"):
            self.run(f"git reset --hard {self.commit_core_sha}")
            self.run("echo '#!/bin/bash' > ./BuildTools/FormatValidation/validate_format_linux.sh")

        self.run(f"git clone --recursive {self.url}/DiligentTools.git DiligentGraphics/DiligentTools")
        with tools.chdir("DiligentGraphics/DiligentTools"):
            self.run(f"git reset --hard {self.commit_tools_sha}")
            self.run("echo '#!/bin/bash' > ./BuildTools/FormatValidation/validate_format_linux.sh")

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
        self.cpp_info.libdirs = [os.path.join("lib", "DiligentCore", build_type),
                                 os.path.join("lib", "DiligentTools", build_type)]
        self.cpp_info.libs = tools.collect_libs(self)
