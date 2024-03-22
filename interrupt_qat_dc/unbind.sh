#!/bin/bash
DPDK_USERTOOL=./dpdk-devbind.py
sudo $DPDK_USERTOOL -u 0000:76:00.0
sudo $DPDK_USERTOOL -u 0000:f3:00.0
