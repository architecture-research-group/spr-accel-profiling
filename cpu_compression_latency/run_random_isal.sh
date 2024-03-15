#!/bin/bash
SIZES=( 1024 4096 16384  )
SIZES_LOG=( `seq 8 21` )
n=1000
max_size=$(( 4 * 1024 * 1024 ))
[ ! -f "foo.bin" ] && ./gen_entropy.sh
taskset -c 20 make
echo > decomp_output
for i in ${SIZES_LOG[@]}; 
do 
	echo "taskset -c 40 ./isal_zlib_decompress $(( 1 << ${i} )) ${n} foo.bin >> decomp_output"
	taskset -c 40 ./isal_zlib_decompress $(( 1 << ${i} )) ${n} foo.bin >> decomp_output
done 
sort -k1 -g -k4 -t, decomp_output > log
echo "DECOOMPRESS"
grep Decompress log | awk -F, '{printf("%s,%s\n",$1,$5);}'
echo "COOMPRESS"
grep Compress log | awk -F, '{printf("%s,%s\n",$1,$5);}'

