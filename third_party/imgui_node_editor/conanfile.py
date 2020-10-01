from conans import ConanFile, CMake, tools


class ImguiNodeEditor(ConanFile):
    name = "imgui_node_editor"
    commit_sha = "687a72f"
    version = f"0.9.{commit_sha}"
    license = "MIT"
    url = "https://github.com/thedmd/imgui-node-editor"
    description = "Node Editor using ImGui "
    topics = ("node-editor")
    settings = "os", "compiler", "build_type", "arch"
    options: dict = {}
    default_options: dict = {}
    generators = "cmake"
    exports_sources = "CMakeLists.txt"
    imgui = "imgui/1.78.docking.05bc204@terragraphics/wip"

    def source(self):
        self.run(f"git clone {self.url} repo")
        with tools.chdir("repo"):
            self.run(f"git reset --hard {self.commit_sha}")

    def configure(self):
        self.requires(self.imgui)

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
