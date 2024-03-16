./build.sh

ISA-L Bandwidth-Granularity Sweep:
=====
./run_granularity_sweep_isa-l-4cores-throughput.sh
./parse_granularity.sh

ISA-L Latency-Granularity Sweep:
=====
./run_calgary_isal.sh

ZLIB Ratio vs. Decompression Speed
====
./ratio_vs_latency.sh # everything after DECOOMPRESS format: size, ratio-%-orig, latency(ns)

Hypo: CPU DC Latencies are highly variable -- max latency is much higher than avg
Size,Level,AvgRatio,Direction,AvgLatency,MaxLatency
4096,2,0.382258,Compress,5836,121254
4096,2,0.382258,Compress,5606,76688



ISSUES
======
ISA-L cannot compress random text buffers or those without lz repetitions (never ending compression)
* tested on v2.31
* observation: Adaptive compression algorithms are error prone and hard to write
```
(base) n869p538@sapphire:cpu_compression_latency$ ./isal_zlib_decompress_old 4096 1000 foo.bin
num iterations:1000
num bufs:768
Size,Level,AvgRatio,Direction,AvgLatency,MaxLatency
4096,2,0.378437,Compress,6289.000000,36424.000000
4096,2,0.378437,Decompress,7991.000000,37740.000000
(base) n869p538@sapphire:cpu_compression_latency$ ./isal_zlib_decompress_old 4096 1000 no_lz.bin
num iterations:1000
num bufs:768

```
What is the minimum lzdatagen-comp-ratio that ISA-L can compress?
* observed 1.8
```
(base) n869p538@sapphire:cpu_compression_latency$ ./isal_zlib_decompress 4096 1000 ratio1_8
num iterations:1000
num bufs:768
Size,Level,AvgRatio,Direction,AvgLatency,MaxLatency
4096,2,0.706496,Compress,11408,421772
4096,2,0.706496,Decompress,13794,76216
(base) n869p538@sapphire:cpu_compression_latency$ ./isal_zlib_decompress 4096 1000 ratio1_6
num iterations:1000
num bufs:768
^C
```


Bandwidth Testing procedure from: 
https://dougallj.wordpress.com/2022/08/20/faster-zlib-deflate-decompression-on-the-apple-m1-and-x86/
https://gist.github.com/dougallj/68bc40410109e055e8482519b8691043
