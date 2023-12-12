

* term 1: setup cgroup, zswap, and invoke swap to far memory
```
./scripts/setup.sh
./scripts/create_cgroup.sh
./scripts/invoke_zswap.sh
```

* term 2: validate cgroup zswap usage && trace latency
```
 ./scripts/print_cgroup_stats.sh
```