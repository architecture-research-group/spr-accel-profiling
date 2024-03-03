#!/bin/bash
taskset -c 20 make zstd_compress
echo "Level,Ratio,median(ns),avg(ns),BW(GB/s),payloadsize(B),compressedSize(B)"; for i in calgary*; do taskset -c 20 ./zstd_compress $i $( wc -c $i | grep -Eo '[0-9]+' ) 10000 | tail -n 2;  done | sort  -g -k1 -k6 -t,
