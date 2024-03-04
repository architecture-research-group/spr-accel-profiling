#!/bin/bash
echo 0000:76:00.0 | sudo tee /sys/bus/pci/drivers/4xxx/bind
[ "$!" != 0 ] && echo "is device bound to vfio-pci -- try dpdk's devbind tool to check"
echo 0000:f3:00.0 | sudo tee /sys/bus/pci/drivers/4xxx/bind
[ "$!" != 0 ] && echo "is device bound to vfio-pci -- try dpdk's devbind tool to check"

