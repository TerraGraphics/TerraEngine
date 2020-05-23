import os
from conans import ConanFile, CMake, tools
from conans.tools import download, unzip


class RttrConan(ConanFile):
    name = "rttr"
    version = "0.9.6"
    license = "MIT"
    url = "https://github.com/rttrorg/rttr"
    description = "Conan package for rttr."
    topics = ("reflection")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "rtti": [True, False],
    }
    default_options = {
        "rtti": True,
    }
    exports = ['patches/*']
    generators = "cmake"

    def source(self):
        zip_name = "v%s.zip" % self.version
        download("%s/archive/%s" % (self.url, zip_name), zip_name, verify=False)
        unzip(zip_name)
        os.unlink(zip_name)
        base_path = os.path.join(self.source_folder, "rttr-{}".format(RttrConan.version), "CMake")
        tools.patch(base_path=base_path, patch_file="patches/utility.cmake.patch")

    def build(self):
        cmake = CMake(self)
        cmake.definitions["BUILD_RTTR_DYNAMIC"] = "OFF"
        cmake.definitions["BUILD_UNIT_TESTS"] = "OFF"
        cmake.definitions["BUILD_STATIC"] = "ON"
        cmake.definitions["BUILD_WITH_STATIC_RUNTIME_LIBS"] = "OFF"
        cmake.definitions["BUILD_BENCHMARKS"] = "OFF"
        cmake.definitions["BUILD_EXAMPLES"] = "OFF"
        cmake.definitions["BUILD_DOCUMENTATION"] = "OFF"
        cmake.definitions["BUILD_INSTALLER"] = "OFF"
        cmake.definitions["BUILD_PACKAGE"] = "OFF"
        cmake.definitions["USE_PCH"] = "ON"
        cmake.definitions["CUSTOM_DOXYGEN_STYLE"] = "ON"
        cmake.definitions["BUILD_WEBSITE_DOCU"] = "OFF"

        if self.options.rtti:
            cmake.definitions["BUILD_WITH_RTTI"] = "ON"
        else:
            cmake.definitions["BUILD_WITH_RTTI"] = "OFF"
        cmake.configure(source_folder="rttr-{}".format(RttrConan.version))
        cmake.build()

    def package(self):
        include_folder = "%s-%s/src" % (self.name, self.version)
        self.copy("*.h"  , dst="include", src=include_folder)
        self.copy("*.rc" , dst="include", src=include_folder)
        self.copy("*.h"  , dst="include", src="src")
        self.copy("*.rc" , dst="include", src="src")
        self.copy("*.a"  , dst="lib", keep_path=False)
        self.copy("*.so" , dst="lib", keep_path=False)
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["rttr_core_d"] if self.settings.build_type == "Debug" else ["rttr_core"]
