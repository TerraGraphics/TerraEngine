from conans import ConanFile, CMake, tools


class ImGui(ConanFile):
    name = "imgui"
    commit_sha = "05bc204"
    version = f"1.78.docking.{commit_sha}"
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
        self.run(f"git clone {self.url}")
        with tools.chdir("imgui"):
            self.run(f"git reset --hard {self.commit_sha}")

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
