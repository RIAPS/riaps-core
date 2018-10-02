# RIAPS C++ Component Framework

[![Build Status](https://travis-ci.com/RIAPS/riaps-core.svg?token=pyUEeBLkG7FqiYPhyfxp&branch=master)](https://travis-ci.com/RIAPS/riaps-core)

# About

This repository contains the discovery service and the C++ implementation of the component framework.

# Requirements

## Set up the crosscompile environment

Add the armhf repositories:

```sh
sudo apt-get install software-properties-common apt-transport-https -y      
sudo add-apt-repository -r "deb http://us.archive.ubuntu.com/ubuntu/ bionic main restricted" || true
sudo add-apt-repository -n "deb [arch=amd64,i386] http://us.archive.ubuntu.com/ubuntu/ bionic main restricted"
sudo add-apt-repository -r "deb http://us.archive.ubuntu.com/ubuntu/ bionic-updates main restricted" || true
sudo add-apt-repository -n "deb [arch=amd64,i386] http://us.archive.ubuntu.com/ubuntu/ bionic-updates main restricted"
sudo add-apt-repository -r "deb http://us.archive.ubuntu.com/ubuntu/ bionic universe" || true
sudo add-apt-repository -n "deb [arch=amd64,i386] http://us.archive.ubuntu.com/ubuntu/ bionic universe"
sudo add-apt-repository -r "deb http://us.archive.ubuntu.com/ubuntu/ bionic-updates universe" || true
sudo add-apt-repository -n "deb [arch=amd64,i386] http://us.archive.ubuntu.com/ubuntu/ bionic-updates universe"
sudo add-apt-repository -r "deb http://us.archive.ubuntu.com/ubuntu/ bionic multiverse" || true
sudo add-apt-repository -n "deb [arch=amd64,i386] http://us.archive.ubuntu.com/ubuntu/ bionic multiverse"
sudo add-apt-repository -r "deb http://us.archive.ubuntu.com/ubuntu/ bionic-updates multiverse"
sudo add-apt-repository -n "deb [arch=amd64,i386] http://us.archive.ubuntu.com/ubuntu/ bionic-updates multiverse"
sudo add-apt-repository -r "deb http://us.archive.ubuntu.com/ubuntu/ bionic-backports main restricted universe multiverse" || true
sudo add-apt-repository -n "deb [arch=amd64,i386] http://us.archive.ubuntu.com/ubuntu/ bionic-backports main restricted universe multiverse"
sudo add-apt-repository -r "deb http://security.ubuntu.com/ubuntu bionic-security main restricted" || true
sudo add-apt-repository -n "deb [arch=amd64,i386] http://security.ubuntu.com/ubuntu bionic-security main restricted"
sudo add-apt-repository -r "deb http://security.ubuntu.com/ubuntu bionic-security universe" || true
sudo add-apt-repository -n "deb [arch=amd64,i386] http://security.ubuntu.com/ubuntu bionic-security universe"
sudo add-apt-repository -r "deb http://security.ubuntu.com/ubuntu bionic-security multiverse" || true
sudo add-apt-repository -n "deb [arch=amd64,i386] http://security.ubuntu.com/ubuntu bionic-security multiverse"

# Add armhf repositories
sudo add-apt-repository -r "deb [arch=armhf] http://ports.ubuntu.com/ubuntu-ports bionic main universe multiverse" || true
sudo add-apt-repository -n "deb [arch=armhf] http://ports.ubuntu.com/ubuntu-ports bionic main universe multiverse"
sudo add-apt-repository -r "deb [arch=armhf] http://ports.ubuntu.com/ubuntu-ports bionic-updates main universe multiverse" || true
sudo add-apt-repository  -n "deb [arch=armhf] http://ports.ubuntu.com/ubuntu-ports bionic-updates main universe multiverse"

```

Add the armhf architecture:
```sh
sudo dpkg --add-architecture armhf
sudo apt-get update
```

Install build tools:

```sh
sudo apt-get install crossbuild-essential-armhf gdb-multiarch build-essential cmake  libtool libtool-bin \
autoconf autogen 
```

Some of the dependencies are available from the apt repos, install them:

```sh
sudo apt-get install libreadline-dev libreadline-dev:armhf byacc flex libpcap-dev libpcap-dev:armhf \
libzmq5 libzmq3-dev libzmq3-dev:armhf libsystemd-dev libsystemd-dev:armhf libuuid1:armhf liblz4-1:armhf \
libncurses5-dev libncurses5-dev:armhf nettle-dev nettle-dev:armhf
```

## Install riaps dependencies

Download and install [riaps-externals](https://github.com/RIAPS/riaps-externals/releases).
It contains both the armhf and amd64 dependencies of RIAPS. The externals package is installed under
the `/opt/riaps/{arch}/ directory.`

```sh
sudo dpkg -i riaps-externals-amd64.deb
```

Install libpython3.5-dev

```sh
sudo apt install libpython3.5-dev
```

Download and install [riaps-core](https://github.com/RIAPS/riaps-core/releases).

```sh
sudo dpkg -i riaps-core-amd64.deb
```

# Build riaps-core


Clone the repository

```sh
git clone -b pybindcomponents https://github.com/RIAPS/riaps-core.git
```

Build riaps-core and DistributedEstimatorPybind

```
mkdir riaps-core/build && cd riaps-core/build
cmake -Darch=amd64 ../
make -j4

cp ./rdiscoveryd /opt/riaps/amd64/bin/
cp ./libriaps.so /opt/riaps/amd64/lib/
```

The built components are in riaps-core/bin directory:

```
ls -x ../bin
globalestimator.cpython-35m-x86_64-linux-gnu.so
localestimator.cpython-35m-x86_64-linux-gnu.so
sensor.cpython-35m-x86_64-linux-gnu.so 
```


