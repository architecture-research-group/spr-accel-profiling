#!/bin/bash
SIZES=( 1024 4096 65536 $((256 * 1024 )) $(( 1024 * 1024 )) )
n=1000
taskset -c 20 make
for i in ${SIZES[@]}; do taskset -c 40 ./isal_zlib_decompress ${i} ${n} ;  done | sort -k1 -g -k4 -t,
