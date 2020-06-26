#!/bin/bash
if [[ "$OSTYPE" == "linux-gnu" ]]; then
#	libs/bx/tools/bin/linux/genie --gcc=linux-gcc-6 gmake
    rm -R -d build/projects/cmake
	libs/GENie/bin/linux/genie /scripts=libs/GENie/src --gcc=linux-gcc-6 cmake
elif [[ "$OSTYPE" == "darwin"* ]]; then
	libs/bx/tools/bin/darwin/genie "$@"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
	libs/bx/tools/bin/windows/genie.exe "$@"
fi

#pushd projects/gmake-linux
#make CXX="g++" CC="gcc" config="debug64"
#popd
