#!/bin/bash

# check compiler version
MINGW32_VERSION="$(i686-w64-mingw32-g++ --version | grep -o '..-win32')"

# check installed
if [ -z MINGW32_VERSION ]; then
    echo "Check the README.md file:"
    echo "    Install Mingw-w64 using: `sudo apt install build-essential mingw-w64`."
    exit 1
fi

if [ ! -d "./bin" ]; then
    mkdir bin
fi

# check runtime library: libstdc++-6.dll
if [ ! -f "./bin/libstdc++-6.dll" ]; then
    cp "/usr/lib/gcc/i686-w64-mingw32/$MINGW32_VERSION/libstdc++-6.dll" "./bin/"
fi

# check runtime library: libgcc_s_dw2-1.dll
if [ ! -f "./bin/libgcc_s_dw2-1.dll" ]; then
    cp "/usr/lib/gcc/i686-w64-mingw32/$MINGW32_VERSION/libgcc_s_dw2-1.dll" "./bin/"
fi

make
wine cmd /c run_both.bat
