#!/bin/bash
git submodule update --init --recursive . 
cd qatlib 
./autogen.sh
./configure
sudo make -j
sudo make -j install
sudo make samples

cd ..
./bind.sh
