#!/bin/bash
# Script configure IAA devices
# Usage : ./configure_iaa_user <mode> <start,end> <wq_size>
# mode: 0 - shared, 1 - dedicated
# devices: 0 - all devices or start and end device number.
# For example, 1, 7 will configure all the Socket0 devices in host or 0, 3
# will configure all the Socket0 devices in guest
# 9, 15 will configure all the Socket1 devices and son on
# 1 will configure only device 1
# wq_size: 1-128
#
# select iax config
#
dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo ${dir}
#
# count iax instances
#
iax_dev_id="0cfe"
num_iax=$(lspci -d:${iax_dev_id} | wc -l)
echo "Found ${num_iax} IAX instances"

#
# disable iax wqs and devices
#
echo "Disable IAX"
for ((i = 1; i < 2; i += 1)); do
    echo disable wq iax${i}/wq${i}.0 >& /dev/null
    sudo accel-config disable-wq iax${i}/wq${i}.0 >& /dev/null
    echo disable iax iax${i} >& /dev/null
    sudo accel-config disable-device iax${i} >& /dev/null
done

#
# enable all iax devices and wqs
#
start=1 end=2
echo "Enable IAX ${start} to ${end}"
for ((i = ${start}; i < ${end}; i += 1)); do
    # Config Engines and groups
    sudo accel-config config-engine iax1/engine1.0 --group-id=0
    sudo accel-config config-engine iax1/engine1.1 --group-id=0
    sudo accel-config config-engine iax1/engine1.2 --group-id=0
    sudo accel-config config-engine iax1/engine1.3 --group-id=0
    sudo accel-config config-engine iax1/engine1.4 --group-id=0
    sudo accel-config config-engine iax1/engine1.5 --group-id=0
    sudo accel-config config-engine iax1/engine1.6 --group-id=0
    sudo accel-config config-engine iax1/engine1.7 --group-id=0
    # Config WQ: group 0, size = 128, priority=10, mode=shared, type = user,
    # name=iax_crypto, threashold=128, block_on_fault=1, driver_name=user
    # echo "sudo accel-config config-wq iax${i}/wq${i}.0 -g 0 -s $wq_size -p 10 -m ${mode} -y user -n user${i} -t $wq_size -b 1 -d user"

    # sudo accel-config config-wq iax${i}/wq${i}.0 -g 0 -s $wq_size -p 10 -m ${mode} -y user -n user${i} -t $wq_size -b 1 -d user
    sudo accel-config config-wq --group-id=0 --mode=dedicated --type=user --wq-size=128 --driver-name=idxd --name=app --priority=10 iax1/wq1.0
    # echo enable device iax${i}
    sudo accel-config enable-device iax1
    # echo enable wq iax1/wq1.0
    sudo accel-config enable-wq iax1/wq1.0
done