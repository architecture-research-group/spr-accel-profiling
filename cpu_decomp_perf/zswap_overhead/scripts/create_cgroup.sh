#!/bin/bash
max_mem=1G
max_swap=16G
en_zswap=Y
zswap_alg=zstd
zswap_poolsz=50
zswap_alloc=zsmalloc
cgroup_stats=( "/sys/fs/cgroup/parent/child/memory.swap.current" "/sys/fs/cgroup/parent/child/memory.current" )
# pool_total_size is in bytes
zswap_stats=(  "/sys/kernel/debug/zswap/reject_compress_poor" "/sys/kernel/debug/zswap/stored_pages" "/sys/kernel/debug/zswap/reject_alloc_fail" "/sys/kernel/debug/zswap/reject_kmemcache_fail" "/sys/kernel/debug/zswap/duplicate_entry" "/sys/kernel/debug/zswap/written_back_pages" "/sys/kernel/debug/zswap/pool_total_size" "/sys/kernel/debug/zswap/pool_limit_hit" )

# make cgroup
[ ! -d "/sys/fs/cgroup/parent" ] && sudo mkdir /sys/fs/cgroup/parent
if [ -d "/sys/fs/cgroup/parent/child" ]; then 
	cat  /sys/fs/cgroup/parent/child/cgroup.procs | xargs sudo kill -KILL
	sudo rmdir /sys/fs/cgroup/parent/child
fi
sudo mkdir /sys/fs/cgroup/parent/child

# memory limits
echo "+memory" | sudo tee  /sys/fs/cgroup/parent/cgroup.subtree_control
echo "${max_swap}" | sudo tee  /sys/fs/cgroup/parent//memory.swap.max
echo "${max_mem}" | sudo tee  /sys/fs/cgroup/parent//memory.max

# kill whole cgroup as soon as oom-killer is invoked
echo 1 | sudo tee /sys/fs/cgroup/parent/child/memory.oom.group

# zswap configs
echo $en_zswap | sudo tee  /sys/module/zswap/parameters/enabled
echo $zswap_alg | sudo tee  /sys/module/zswap/parameters/compressor
echo $zswap_alloc | sudo tee  /sys/module/zswap/parameters/zpool
echo $zswap_poolsz | sudo tee  /sys/module/zswap/parameters/max_pool_percent
