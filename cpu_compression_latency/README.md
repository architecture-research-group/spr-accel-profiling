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
harnesses don't work if files are small
ISA-L cannot compress random text files (never ending compression)
* tested on v2.31
```
(base) n869p538@sapphire:cpu_compression_latency$ ./run_random_isal.sh
g++ -o  isal_zlib_decompress  -Iisa-l/include harness-isal_zlib_decompress.cpp -lisal -lz
# hangs because deflate call does not stop
```
ISA-L cannot compress random text files >=2048B without lz repetitions


Bandwidth Testing procedure from: 
https://dougallj.wordpress.com/2022/08/20/faster-zlib-deflate-decompression-on-the-apple-m1-and-x86/
https://gist.github.com/dougallj/68bc40410109e055e8482519b8691043
