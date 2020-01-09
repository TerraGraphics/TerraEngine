import os
import tempfile
from conans.tools import get_env
from conans import ConanFile, CMake, AutoToolsBuildEnvironment, tools


class Libucl(ConanFile):
    name = "libucl"
    version = "0.8.1"
    license = 'BSD 2-Clause "Simplified" License'
    url = "https://github.com/vstakhov/libucl"
    description = "Universal configuration library parser"
    topics = ("config")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
    }
    default_options = {
        "shared": False,
    }
    generators = "cmake"

    def source(self):
        filename = "{}.tar.gz".format(self.version)
        url = "https://github.com/vstakhov/libucl/archive/{}.tar.gz".format(self.version)
        dlfilepath = os.path.join(tempfile.gettempdir(), filename)
        if os.path.exists(dlfilepath) and not get_env("LIBUCL_DOWNLOAD", False):
            self.output.info("Skipping download. Using cached {}".format(dlfilepath))
        else:
            self.output.info("Downloading {} from {}".format(self.name, url))
            tools.download(url, dlfilepath)
        tools.untargz(dlfilepath)

    def build(self):
        dir_name = "libucl-{}".format(self.version)
        with tools.chdir(dir_name):
            self.run("bash autogen.sh")
            autotools = AutoToolsBuildEnvironment(self)
            autotools.configure()
            autotools.make()

    def package(self):
        self.copy("*.a", dst="lib/", keep_path=False)
        self.copy("*.h", dst="include", src="libucl-{}/include".format(self.version))

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
