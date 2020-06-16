import os
from conans import tools, ConanFile, AutoToolsBuildEnvironment


class CpgfConan(ConanFile):
    name = "cpgf"
    version = "1.6.1"
    license = "Apache License 2.0"
    url = "https://github.com/cpgf/cpgf"
    description = "cpgf library "
    topics = ("reflection")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "platform": ["mingw", "linux", "vc05", "vc08", "vc10", "cb_mingw", "cb_nmake", "cb_linux", "xcode"],
    }
    default_options = {
        "platform": "linux",
    }
    generators = "cmake"

    def source(self):
        zip_name = "v%s.zip" % self.version
        tools.download("{}/archive/{}".format(self.url, zip_name), zip_name, verify=False)
        tools.unzip(zip_name)
        os.unlink(zip_name)

    def build(self):
        dir_name = os.path.join("{}-{}".format(self.name, self.version), "build")
        with tools.chdir(dir_name):
            autotools = AutoToolsBuildEnvironment(self)
            autotools.make([str(self.options.platform), "TARGET=lib"])

    def package(self):
        self.copy("*.a", dst="lib", keep_path=False)
        self.copy("*.h*", dst="include", src=os.path.join("{}-{}".format(self.name, self.version), "include"))

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
