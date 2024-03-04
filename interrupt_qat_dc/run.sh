#!/bin/bash
taskset -c 40 ./qatlib/cpa_sample_code runTests=32 getLatency=1 | tee raw_comp_latency.txt