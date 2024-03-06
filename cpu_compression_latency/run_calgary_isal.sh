#!/bin/bash
SIZES=( 1024 4096 $((256 * 1024 )) $(( 1024 * 1024 )) )
taskset -c 20 make
echo "Level,Ratio,median(ns),avg(ns),BW(GB/s),payloadsize(B),compressedSize(B)"; 
for i in ${SIZES[@]}; do taskset -c 20 ./isal_zlib_decompress ${i} 0 ;  done | sort  -g -k2 -t,
