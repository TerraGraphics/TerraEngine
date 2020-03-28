import os
from conans import ConanFile, CMake, tools


class ImguiNodeEditor(ConanFile):
    name = "imgui_node_editor"
    version = "0.9.9868e99"
    license = "MIT License"
    url = "https://github.com/thedmd/imgui-node-editor"
    description = "Node Editor using ImGui "
    topics = ("node-editor")
    settings = "os", "compiler", "build_type", "arch"
    options = { }
    default_options = { }
    generators = "cmake"
    exports_sources = "CMakeLists.txt"
    build_requires = "imgui/1.75.docking.16a9488@terragraphics/wip"

    def source(self):
        self.run("git clone %s repository" % ImguiNodeEditor.url)
        commit_version = "9868e99"
        with tools.chdir("repository"):
            self.run("git reset --hard %s" % commit_version)
        self.run("rm ./repository/NodeEditor/CMakeLists.txt")
        self.run("cp -r ./repository/NodeEditor/* .")

    def _create_cmake(self):
        cmake = CMake(self)
        cmake.configure()
        return cmake

    def build_requirements(self):
        self.build_requires(ImguiNodeEditor.build_requires)

    def build(self):
        cmake = self._create_cmake()
        cmake.build()

    def package(self):
        cmake = self._create_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
