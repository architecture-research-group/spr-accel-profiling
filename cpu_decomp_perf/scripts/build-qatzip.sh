#!/bin/bash

export ICP_ROOT=$(pwd)/QAT20.L.1.0.50-00003


echo 1024 | sudo tee /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
sudo rmmod usdm_drv
sudo insmod $ICP_ROOT/build/usdm_drv.ko max_huge_pages=1024 max_huge_pages_per_process=48


cd QATzip/
./autogen.sh
QZ_ROOT=$(pwd) ./configure --with-ICP_ROOT=$ICP_ROOT
make clean
make -j
sudo make install