#!/bin/bash
SIZES=( 1024 4096 $((256 * 1024 )) $(( 1024 * 1024 )) )
CORES=( 20 21 22 23 )
n=1000
make isal_zlib_throughput
for i in ${SIZES[@]}; do 
  for j in ${CORES[@]}; do
  taskset -c ${j} ./isal_zlib_throughput ${i} ${n} >> result_${j}_bw  &
  done
done


# rm -rf result_*_bw