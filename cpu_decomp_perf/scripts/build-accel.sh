#!/bin/bash
git submodule update --init QAT20.L.1.0.50-00003/
cd  QAT20.L.1.0.50-00003/
./configure
make -j
sudo make -j install
sudo make samples -j GRANULARITY_CODE=1 LATENCY_CODE=1
sudo ./build/cpa_sample_code runTests=32 getLatency=1 | tee results.txt