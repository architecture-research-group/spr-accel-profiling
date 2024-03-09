#!/bin/bash
algs=( "isa-l" "libdeflate" "zlib" )
echo "payloadsize(kb),numcores,comp(Mbit/s),decomp(Mbit/s),ratio(%orig)"
for j in "${algs[@]}"; do
	echo $j
	for i in $(ls -1 results_* | sort -t_ -k 2 -g); 
	do 
		echo -n $(echo ${i} | grep -Eo '[0-9]+' | tail -n 1),
		echo -n "$(grep $j $i | wc -l),$(grep $j $i | awk -F, '{comp_sum+=$2; decomp_sum+=$3; ratio=$6;} END{printf("%s,%s,%f\n",comp_sum*8,decomp_sum*8,ratio/100)}') "; echo ; 
	done
done
