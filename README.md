# TerraEngine - Game engine

# Build

* Install externan packages:

```console
sudo pip install conan
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
conan remote add terragraphics https://api.bintray.com/conan/terragraphics/conan-packages
```

* Build:

```console
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=RELWITHDEBINFO -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ ../
cmake --build .
cd ..
```
