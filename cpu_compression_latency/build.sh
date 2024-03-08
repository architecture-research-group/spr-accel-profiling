#!/bin/bash
sudo apt install  -y nasm 
git submodule update --init --recursive .
cd isa-l
./autogen.sh
./configure
make
sudo make install

cd lzbench-isal
make -j