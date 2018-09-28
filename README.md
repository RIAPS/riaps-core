# RIAPS C++ components with pybind11

[![Build Status](https://travis-ci.com/RIAPS/riaps-core.svg?token=pyUEeBLkG7FqiYPhyfxp&branch=master)](https://travis-ci.com/RIAPS/riaps-core)

This branch contains experiment code. Don't use it in production!

# Requirements

Download and install [riaps-externals](https://github.com/RIAPS/riaps-externals/releases/tag/0.6.4)

```sh
sudo dpkg -i riaps-externals-amd64.deb
```

Install libpython3.5-dev

```sh
sudo apt install libpython3.5-dev
```

Clone riaps-core

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


