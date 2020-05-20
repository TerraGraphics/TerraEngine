FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive \
	CC=clang \
	CXX=clang++

COPY conanfile.txt /conanfile.txt

RUN apt update && \
    apt install -y python3-pip git cmake clang libtool pkg-config libx11-dev libxcursor-dev libx11-xcb-dev libxcb-icccm4-dev libxcb-cursor-dev libxcb-keysyms1-dev libgl-dev && \
    \
    pip3 install conan==1.25.2 && \
    conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan && \
    conan remote add terragraphics https://api.bintray.com/conan/terragraphics/conan-packages && \
    \
    conan install /conanfile.txt -s build_type=Release -s compiler=clang -s compiler.version=10 -s compiler.libcxx=libstdc++11 -g=cmake --build=missing && \
    conan remove "*" -s -b -f && \
    \
	apt remove -y --auto-remove libtool pkg-config && \
	apt clean autoclean && \
	apt-get autoremove --yes && \
	rm -rf /var/lib/{apt,dpkg,cache,log}/
