./build.sh

ISA-L Bandwidth-Granularity Sweep:
=====
./run_granularity_sweep_isa-l-4cores-throughput.sh
./parse_granularity.sh

ISA-L Latency-Granularity Sweep:
=====
./run_calgary_isal.sh


ISSUES
======
ISA-L cannot compress random text buffers or those without lz repetitions (never ending compression)
* tested on v2.31
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


Bandwidth Testing procedure from: 
https://dougallj.wordpress.com/2022/08/20/faster-zlib-deflate-decompression-on-the-apple-m1-and-x86/
https://gist.github.com/dougallj/68bc40410109e055e8482519b8691043
