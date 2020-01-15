import os
from conans import ConanFile, CMake, tools


class ImGui(ConanFile):
    name = "imgui"
    version = "1.74.docking.44174b1"
    license = "MIT"
    url = "https://github.com/ocornut/imgui"
    description = "Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies"
    topics = ("gamedev", "imgui", "gui", "graphical")
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True
    }
    generators = "cmake"
    exports_sources = "CMakeLists.txt"

    def source(self):
        self.run("git clone %s" % ImGui.url)

        commit_version = "44174b1"
        with tools.chdir("imgui"):
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

        self.copy(pattern="examples/imgui_impl_*", dst="misc/bindings", src="imgui", keep_path=False)

    def package_info(self):
        self.cpp_info.srcdirs = ["misc", ]
        self.cpp_info.libs = tools.collect_libs(self)
