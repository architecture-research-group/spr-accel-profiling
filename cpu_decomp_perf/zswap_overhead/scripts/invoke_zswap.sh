#!/bin/bash
cat  /sys/fs/cgroup/parent/child/cgroup.procs | xargs sudo kill -KILL
echo $$ | sudo tee /sys/fs/cgroup/parent/child/cgroup.procs

stress-ng --vm-bytes 3G --vm-keep -m 1 --metrics-brief  --vm-method  galpat-0