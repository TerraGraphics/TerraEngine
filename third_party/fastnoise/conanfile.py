from conans import ConanFile, CMake, tools


class FastNoise(ConanFile):
    name = "fastnoise"
    commit_sha = "faba444"
    version = f"0.4.{commit_sha}"
    license = "MIT"
    url = "https://github.com/Auburns/FastNoise"
    description = "coherent noise-generating library for C++"
    topics = ("noise")
    settings = "os", "arch", "compiler", "build_type"
    options: dict = {}
    default_options: dict = {}
    generators = "cmake"
    exports_sources = "CMakeLists.txt"

    def source(self):
        self.run(f"git clone {self.url} repo")
        with tools.chdir("repo"):
            self.run(f"git reset --hard {self.commit_sha}")

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
