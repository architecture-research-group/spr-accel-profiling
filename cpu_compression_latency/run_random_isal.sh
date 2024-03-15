#!/bin/bash
SIZES=( 1024 4096 16384  )
SIZES_LOG=( `seq 8 21` )
n=1000
max_size=$(( 4 * 1024 * 1024 ))
[ ! -f "rand_file.txt" ] && head -c $max_size /dev/urandom > rand_file.txt
taskset -c 20 make
for i in ${SIZES_LOG[@]}; 
do 
	taskset -c 40 ./isal_zlib_decompress $(( 1 << ${i} )) ${n} rand_file.txt ; 
done | sort -k1 -g -k4 -t, | tee log
echo "DECOOMPRESS"
grep Decompress log | awk -F, '{printf("%s,%s\n",$1,$5);}'
echo "COOMPRESS"
grep Compress log | awk -F, '{printf("%s,%s\n",$1,$5);}'

