#!/bin/bash
SIZES=( `seq 10 20` )
CORES=( 40 41 42 43 )
LZBENCH=$(pwd)/lzbench-isal/lzbench
CALGARY=/lib/firmware/calgary
[ -f "results_*" ] && echo "Remove your results" && exit -1
for i in ${SIZES[@]}; do 
	echo "(de)compressing block size $i ... "
  for j in ${CORES[@]}; do
    block=$(( 1 << ( $i - 10 ) ))
    taskset -c ${j} $LZBENCH -elibdeflate,3 -eisa-l,2 -o4 -b${block} $CALGARY >> results_${block}kbchunk  &
  done
  wait
done

# rm -rf result_*_bw

