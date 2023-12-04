#!/bin/bash
git submodule update --init --recursive .
./scripts/build_accel-config.sh
./scripts/configure_iaa.sh
./scripts/build_qpl.sh