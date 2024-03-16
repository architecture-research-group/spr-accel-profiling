#!/bin/bash
RATIOS=( 1 2 4 8  )
LZGEN=./lzdatagen/lzdgen
core=40

n=1000
for i in ${RATIOS[@]}; do 
	[ ! -f "r_$i" ] && $LZGEN -r $i -s 3m r_$i
	taskset -c $core ./zlib_decompress 4096 ${n} r_$i ;
done | sort -k1 -g -k2 -t, | tee log
echo "DECOOMPRESS"
grep Decompress log | awk -F, '{printf("%s,%s,%s\n",$1,$3,$5);}'
echo "COOMPRESS"
grep Compress  log | awk -F, '{printf("%s,%s,%s\n",$1,$3,$5);}'
