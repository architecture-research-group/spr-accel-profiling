
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


* intel mlc reported bandwidth for spr 4416+
```sh
(base) n869p538@sapphire:~$ wget https://downloadmirror.intel.com/793041/mlc_v3.11.tgz
--2023-12-14 22:42:05--  https://downloadmirror.intel.com/793041/mlc_v3.11.tgz
Resolving downloadmirror.intel.com (downloadmirror.intel.com)... 108.157.142.9, 108.157.142.34, 108.157.142.32, ...
Connecting to downloadmirror.intel.com (downloadmirror.intel.com)|108.157.142.9|:443... connected.
HTTP request sent, awaiting response... 200 OK
Length: 1050545 (1.0M) [application/octet-stream]
Saving to: ‘mlc_v3.11.tgz.1’

mlc_v3.11.tgz.1                                100%[==================================================================================================>]   1.00M  --.-KB/s    in 0.01s

2023-12-14 22:42:05 (82.8 MB/s) - ‘mlc_v3.11.tgz.1’ saved [1050545/1050545]

(base) n869p538@sapphire:~$ mkdir mlc
(base) n869p538@sapphire:~$ mv ml
mlc/                mlc_v3.11.tgz       mlc_v3.11.tgz.1     mlnx_l2fwd_testing/
(base) n869p538@sapphire:~$ mv mlc_v3.11.tgz mlc
(base) n869p538@sapphire:~$ cd mlc/
(base) n869p538@sapphire:mlc$ tar -xf mlc_v3.11.tgz
(base) n869p538@sapphire:mlc$ ls
'Intel Memory Latency Tools Outbound License Agreement.pdf'   Linux   mlc_v3.11.tgz   readme_mlc_v3.11.pdf   Windows
(base) n869p538@sapphire:mlc$ cd Linux/
(base) n869p538@sapphire:Linux$ ls
mlc  redist.txt
(base) n869p538@sapphire:Linux$ sudo ./mlc --bandwidth_matrix
Intel(R) Memory Latency Checker - v3.11
Command line parameters: --bandwidth_matrix

Using buffer size of 100.000MiB/thread for reads and an additional 100.000MiB/thread for writes
Measuring Memory Bandwidths between nodes within system
Bandwidths are in MB/sec (1 MB/sec = 1,000,000 Bytes/sec)
Using all the threads from each core if Hyper-threading is enabled
Using Read-only traffic type
                Numa node
Numa node            0       1
       0        112664.2        63136.5
       1        63287.5 112624.1
(base) n869p538@sapphire:Linux$ sudo ./mlc --latency_matrix
Intel(R) Memory Latency Checker - v3.11
Command line parameters: --latency_matrix

Using buffer size of 2000.000MiB
Measuring idle latencies for sequential access (in ns)...
                Numa node
Numa node            0       1
       0         113.0   167.9
       1         169.6   106.4
(base) n869p538@sapphire:Linux$ sudo ./mlc --max_bandwidth
Intel(R) Memory Latency Checker - v3.11
Command line parameters: --max_bandwidth

Using buffer size of 100.000MiB/thread for reads and an additional 100.000MiB/thread for writes

Measuring Maximum Memory Bandwidths for the system
Will take several minutes to complete as multiple injection rates will be tried to get the best bandwidth
Bandwidths are in MB/sec (1 MB/sec = 1,000,000 Bytes/sec)
Using all the threads from each core if Hyper-threading is enabled
Using traffic with the following read-write ratios
ALL Reads        :      222587.27
3:1 Reads-Writes :      209030.57
2:1 Reads-Writes :      207439.92
1:1 Reads-Writes :      201931.21
Stream-triad like:      201446.18

(base) n869p538@sapphire:Linux$
```

