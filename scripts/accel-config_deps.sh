#!/bin/bash

echo y | sudo apt install build-essential
echo y | sudo apt install autoconf automake autotools-dev libtool pkgconf asciidoc xmlto
echo y | sudo apt install uuid-dev libjson-c-dev libkeyutils-dev libz-dev libssl-dev
echo y | sudo apt install debhelper devscripts debmake quilt fakeroot lintian asciidoctor
echo y | sudo apt install file gnupg patch patchutils