#!/bin/bash
./scripts/qpl_deps.sh
cd qpl
mkdir -p build
mkdir -p install
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$(pwd)/../install ..
cmake --build . --target install --parallel 4