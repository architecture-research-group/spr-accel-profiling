#!/bin/bash
block=64
NUM_CORES=( 1 4 16 64 )
LZBENCH=$(pwd)/lzbench-isal/lzbench
CALGARY=/lib/firmware/calgary
[ -f "results_1cores_64kbchunk" ] && \
	echo "Remove your results" && exit -1
for num_cores in ${NUM_CORES[@]}; do 
	CORES=( `seq 1 $num_cores` ) # Full Intel Performance SampleCode Comparison
	echo "$num_cores cores ... "
	for j in ${CORES[@]}; do
		taskset -c ${j} $LZBENCH -elibdeflate,3/isa-l,2/zlib,3 -o4 -b${block} $CALGARY >> results_${num_cores}cores_${block}kbchunk  &
	done
	wait
done

# rm -rf result_*_bw

