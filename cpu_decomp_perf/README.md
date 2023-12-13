
### Decompression Latency of Google Files using ZSTD:

tree:
```
* /zswap_overhead: bpftrace and zswap/cgroup setup scripts for VM zswap latency analysis
* test.py: google trace decompression latencies
* harness-*.cpp: Decompression Latency programs used by test.py
```

* spr setup for qat accelerator latency measurement (output to `result.csv`)
```sh
git submodule update --init --recursive .
./scripts/build-accel.sh
./scripts/build-qatzip.sh

cd HyperCompressBench
./gen_source_data.sh
python3 reconstruct.py

sudo cp -r QATzip/4xxx_bak/* /etc # TODO: change QATZIP to use nanos() instead of tv_usec
sudo service qat_service restart

python3 -m venv env
pip install -r requirements.txt
python qatzip_test.py
```

* run test in `test.py` (only for CPU tests)
```sh
git submodule update --init --recursive .
mkdir -p google-corpus
python3 -m venv env
pip install -r requirements.txt
python3 test.py
```

#### issue:
* currently requires running `python3 test.py` twice
* once to generate the corpus
* second w/ `gen_corpus` commented to run benchmark
* output to `result.csv`
* also need to `mkdir -p google-corpus`

```sh
(base) n869p538@emerald:cpu_decomp_perf$ python3 -m venv env
(base) n869p538@emerald:cpu_decomp_perf$ source env/bin/activate
(env) (base) n869p538@emerald:cpu_decomp_perf$ pip install -r requirements.txt
Collecting lz4==4.3.2 (from -r requirements.txt (line 1))
  Downloading lz4-4.3.2-cp311-cp311-manylinux_2_17_x86_64.manylinux2014_x86_64.whl (1.3 MB)
     ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 1.3/1.3 MB 18.1 MB/s eta 0:00:00
Collecting zstd==1.5.5.1 (from -r requirements.txt (line 2))
  Downloading zstd-1.5.5.1-cp311-cp311-manylinux_2_17_x86_64.manylinux2014_x86_64.whl (1.8 MB)
     ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 1.8/1.8 MB 76.7 MB/s eta 0:00:00
Installing collected packages: zstd, lz4
Successfully installed lz4-4.3.2 zstd-1.5.5.1

[notice] A new release of pip is available: 23.2.1 -> 23.3.1
[notice] To update, run: pip install --upgrade pip
(env) (base) n869p538@emerald:cpu_decomp_perf$ python3 gen_corpus.py
(env) (base) n869p538@emerald:cpu_decomp_perf$ python3 test.py
1024
zstd,google-corpus/000290_cl0_ws16_1024,4.338983,29161.000000,0.035115
1024
zstd,google-corpus/004113_cl0_ws16_1024,2.211663,40947.000000,0.025008
1024
zstd,google-corpus/007174_cl0_ws16_1024,1.706667,37713.000000,0.027152
1024
zstd,google-corpus/001982_cl1_ws10_1024,1.477633,41771.000000,0.024515
1024
zstd,google-corpus/004475_cl1_ws10_1024,1.259533,19942.000000,0.051349
1024
zstd,google-corpus/002134_cl1_ws10_1024,1.259533,41372.000000,0.024751
32768
zstd,google-corpus/002246_cl0_ws16_32768,4.674465,98646.000000,0.332178
32768
zstd,google-corpus/002168_cl0_ws16_32768,1.274127,75516.000000,0.433921
131072
zstd,google-corpus/000657_cl0_ws15_131072,1.941002,54781.000000,2.392654
8388608
zstd,google-corpus/001689_cl4_ws23_8388608,5.365725,9667153.000000,0.867743
(env) (base) n869p538@emerald:cpu_decomp_perf$
```


#### Dependenceis:
```
Package: liblz4-dev
Version: 1.9.3-2build2

```

