#!/bin/bash
make
echo "Ratio,median(ns),avg(ns),BW(GB/s),payloadsize(B),compressedSize(B)"; for i in calgary*; do taskset -c 40 ./zstd_compress $i $( wc -c $i | grep -Eo '[0-9]+' ) 10000 | tail -n 1;  done | sort  -g -k5 -t,
