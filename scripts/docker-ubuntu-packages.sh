#!/usr/bin/env bash


export DEBIAN_FRONTEND=noninteractive


apt-get install -y build-essential \
    autoconf automake gdb git libffi-dev zlib1g-dev libssl-dev python3 software-properties-common  python-is-python3 \
    gawk wget git diffstat unzip texinfo gcc-multilib build-essential chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev xterm python3-subunit mesa-common-dev \
    file lz4 zstd telnet \
    language-pack-en-base \
   device-tree-compiler build-essential git perl libxml-libxml-perl cmake gcc-10 g++-10


apt-get install python-software-properties
add-apt-repository ppa:ubuntu-toolchain-r/test



echo "deb http://dk.archive.ubuntu.com/ubuntu/ xenial main" >> /etc/apt/sources.list
echo "deb http://dk.archive.ubuntu.com/ubuntu/ xenial universe" >> /etc/apt/sources.list

apt-get -q update >/dev/null
apt-get install -y gcc-4.8 g++-4.8
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50
update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 50
