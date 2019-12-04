import os
from conans import ConanFile, CMake, tools


class DiligentGraphics(ConanFile):
    name = "DiligentGraphics"
    version = "2.4.40478aa.c3bc561"
    license = "Apache License 2.0"
    url = "https://github.com/DiligentGraphics/DiligentGraphics"
    description = " A modern cross-platform low-level graphics library and rendering framework"
    topics = ("gamedev", "graphics-engine", "renderer", "rendering", "graphics-library", "3d-engine")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = "cmake"

    def source(self):
        os.mkdir("DiligentGraphics")

        core_version = "40478aa"
        self.run("git clone --recursive https://github.com/DiligentGraphics/DiligentCore.git DiligentGraphics/DiligentCore")
        with tools.chdir("DiligentGraphics/DiligentCore"):
            self.run("git reset --hard %s" % core_version)

        tools_version = "c3bc561"
        self.run("git clone --recursive https://github.com/DiligentGraphics/DiligentTools.git DiligentGraphics/DiligentTools")
        with tools.chdir("DiligentGraphics/DiligentTools"):
            self.run("git reset --hard %s" % tools_version)

        with tools.chdir("DiligentGraphics"):
            if not os.path.exists("CMakeLists.txt"):
                f=open("CMakeLists.txt","w")
                f.write("cmake_minimum_required (VERSION 3.6)\n")
                f.write("set_property(GLOBAL PROPERTY USE_FOLDERS ON)\n")
                f.write("project(DiligentGraphics)\n")
                f.write("add_subdirectory(DiligentCore)\n")
                f.write("add_subdirectory(DiligentTools)\n")
                f.close()

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="DiligentGraphics")
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.configure(source_folder="DiligentGraphics")
        cmake.install()        
        self.copy("*.h", src="DiligentGraphics/DiligentTools/NativeApp/include/",
            dst="headers/DiligentTools/NativeApp/include/",
            excludes=("Android", "IOS", "MacOS", "UWP", "Win32"))
        self.copy("*.h", src="DiligentGraphics/DiligentTools/NativeApp/Linux/xcb_keysyms/",
            dst="headers/DiligentTools/NativeApp/Linux/xcb_keysyms/")
        self.copy("*.h", src="DiligentGraphics/DiligentCore/Graphics/GraphicsTools/include/",
            dst="headers/DiligentCore/Graphics/GraphicsTools/include/")

        self.copy("*.a", src="DiligentTools/NativeApp/Linux", dst="lib", keep_path=False)
        self.copy("*.a", src="DiligentTools/NativeApp", dst="lib", keep_path=False)
        self.copy("libDiligentCore.a", src="DiligentCore", dst="lib", keep_path=False)
        self.copy("*.a", src="DiligentCore/Common", dst="lib", keep_path=False)
        self.copy("*.a", src="DiligentCore/Graphics/GraphicsTools", dst="lib", keep_path=False)
        self.copy("*.so", src="DiligentCore/Graphics/GraphicsEngineVulkan", dst="lib", keep_path=False)
        self.copy("*.so", src="DiligentCore/Graphics/GraphicsEngineOpenGL", dst="lib", keep_path=False)
        self.copy("*.a", src="DiligentTools/TextureLoader", dst="lib", keep_path=False)
        self.copy("*.a", src="DiligentTools/Imgui", dst="lib", keep_path=False)
        self.copy("*.a", src="DiligentTools/ThirdParty", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.includedirs = [
            'headers/DiligentCore/Common/interface',
            'headers/DiligentCore/Primitives/interface',
            'headers/DiligentCore/Graphics/GraphicsTools/include',
            'headers/DiligentCore/Graphics/GraphicsAccessories/interface',
            'headers/DiligentCore/Graphics/GraphicsEngine/interface',
            'headers/DiligentCore/Graphics/GraphicsEngineOpenGL/interface',
            'headers/DiligentCore/Graphics/GraphicsEngineVulkan/interface',
            'headers/DiligentCore/Graphics/HLSL2GLSLConverterLib/interface',
            'headers/DiligentCore/Platforms/Basic/interface',
            'headers/DiligentCore/Platforms/Linux/interface',
            'headers/DiligentCore/Platforms/interface',
            "headers/DiligentTools/NativeApp/include",
            "headers/DiligentTools/NativeApp/include/Linux",
            'headers/DiligentTools/NativeApp/Linux',
            'headers/DiligentTools/AssetLoader/interface',
            'headers/DiligentTools/Imgui/interface',
            'headers/DiligentTools/TextureLoader/interface',
            'headers/DiligentTools/ThirdParty/imgui',
        ]

        self.cpp_info.libs = tools.collect_libs(self)
