#!/bin/bash
taskset -c 20 make
echo "Level,Ratio,median(ns),avg(ns),BW(GB/s),payloadsize(B),compressedSize(B)"; 
for i in calgary*; do taskset -c 20 ./isal_zlib_decompress-single-buffer $i $( wc -c $i | grep -Eo '[0-9]+' ) 10 | tail -n 2;  done | sort  -g -k1 -k6 -t,
