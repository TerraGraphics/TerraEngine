# TerraEngine - Game engine

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/e40797b1630c4f12be1f1e1077073fba)](https://app.codacy.com/gh/TerraGraphics/TerraEngine?utm_source=github.com&utm_medium=referral&utm_content=TerraGraphics/TerraEngine&utm_campaign=Badge_Grade_Settings)

## Build

### Required packages installation

```console
sudo pip install conan
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
conan remote add terragraphics https://api.bintray.com/conan/terragraphics/conan-packages
```

### Additional packages installation

#### Install "[include what you use](https://github.com/include-what-you-use/include-what-you-use)"

* For arch linux:

```console
yay -S include-what-you-use
```

#### Install [scc](https://github.com/boyter/scc) for command "make cloc"

```console
go get -u github.com/boyter/scc/
```

### CMake build options

* TERRA_IWYU_ENABLE (default OFF) - for enable/disable for "[include what you use](https://github.com/include-what-you-use/include-what-you-use)"

### Compile

```console
cmake -B build
cmake --build build -j
```
