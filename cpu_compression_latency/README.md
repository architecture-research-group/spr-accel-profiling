./build.sh
ISA-L Bandwidth-Granularity Sweep:
./run_granularity_sweep_isa-l-4cores-throughput.sh
./parse_granularity.sh

ISA-L Core Sweep:
* TODO



Testing procedure from: 
https://dougallj.wordpress.com/2022/08/20/faster-zlib-deflate-decompression-on-the-apple-m1-and-x86/
https://gist.github.com/dougallj/68bc40410109e055e8482519b8691043

Optimizations of zlib-dougallj: https://dougallj.wordpress.com/2022/08/20/faster-zlib-deflate-decompression-on-the-apple-m1-and-x86/
- root tables that fit in cache
- TODO


Reporting Decompression Bandwidth 
DougallJ: uses 
compressed_size * iterations_per_run / (times[0] / 1000.0)

Intel-CPA_SampleCode: uses bytes consumed per unit time to produce bandwidth in Megabits/second
https://vscode.dev/github/neel-patel-1/qatlib/blob/orig/quickassist/lookaside/access_layer/src/sample_code/performance/compression/cpa_sample_code_dc_utils.c#L2027

Are "bytes consumed" the number of bytes in the compressed payload or number of bytes in the uncompressed payload?

qatDCPerform tracks throughput:
https://vscode.dev/github/neel-patel-1/qatlib/blob/orig/quickassist/lookaside/access_layer/src/sample_code/performance/compression/qat_compression_main.c#L938

gets throughput using startCyclesTimestamp,
https://vscode.dev/github/neel-patel-1/qatlib/blob/orig/quickassist/lookaside/access_layer/src/sample_code/performance/compression/qat_compression_utils.c#L1999
num_responses, bytes_per_packet
