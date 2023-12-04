#!/bin/bash
./scripts/accel-config_deps.sh

cd idxd-config
./autogen.sh
./configure CFLAGS='-g -O2' --prefix=/usr --sysconfdir=/etc \
    --libdir=/usr/lib64 --enable-test=yes
make
make check
sudo make install