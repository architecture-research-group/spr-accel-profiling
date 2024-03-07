#!/bin/bash
echo "payloadsize(kb),numcores,comp(MB/s),decomp(MB/s)"
for i in $(ls -1 results_* | sort -t_ -k 2 -g); 
do 
echo -n $(echo ${i} | grep -Eo '[0-9]+'), ; 
echo -n "$(grep 'isa-l' $i | wc -l),$(grep 'isa-l' $i | awk -F, '{comp_sum+=$2; decomp_sum+=$3; ratio=$6;} END{printf("%s,%s\n",comp_sum,decomp_sum)}') "; echo ; 
done
