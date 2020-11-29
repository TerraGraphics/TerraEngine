import os
from conans import ConanFile, tools


class FastNoise(ConanFile):
    name = "fastnoise"
    version = f"1.0.1"
    license = "MIT"
    url = "https://github.com/Auburn/FastNoise"
    description = "coherent noise-generating library for C++"
    topics = ("noise")
    settings = "os", "arch", "compiler", "build_type"
    options: dict = {}
    default_options: dict = {}
    generators = "cmake_find_package"

    def source(self):
        tools.download(f"{self.url}/archive/v{self.version}.tar.gz", "src.tar.gz")
        tools.untargz("src.tar.gz")

    def package(self):
        self.copy("*.h*", dst="include", src=os.path.join(f"FastNoise-{self.version}", "Cpp"))
