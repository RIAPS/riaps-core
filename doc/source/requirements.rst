Set Up the Development Machine
##############################

Setup the crosscompiler
=======================

RIAPS Framework is prepared to support multiple architectures: amd64 and armhf. Both amd64 and armhf dependencies are included
in the RIAPS .deb packages, but in order to compile (and crosscompile) the RIAPS applications the build environment must
be set up on the development machine.

First the armhf repositories must be added to the apt sources:

.. code:: bash

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

Add the armhf architecture to the development machine:

.. code:: bash

    sudo dpkg --add-architecture armhf
    sudo apt-get update

Install the build tools:

.. code:: bash

    sudo apt-get install crossbuild-essential-armhf gdb-multiarch build-essential cmake \
    libtool libtool-bin autoconf autogen

Install RIAPS dependencies
==========================

On 18.04+ install the following packages:

.. code:: bash

   sudo apt install byacc flex python3-dev pkg-config libtool libtool-bin build-essential autoconf \
                    libboost-dev autogen libpython3-dev cython3 python3-setuptools libncurses5-dev \
                    libreadline-dev nettle-dev libgnutls28-dev libmsgpack-dev libpcap-dev
   sudo apt install libpython3-dev:armhf libncurses5-dev:armhf libreadline-dev:armhf \
                    nettle-dev:armhf libgnutls28-dev:armhf libmsgpack-dev:armhf libpcap-dev:armhf

Install RIAPS Packages
======================

Download and install:
 - `riaps-externals <https://github.com/RIAPS/riaps-externals/releases>`_
 - `riaps-core <https://github.com/RIAPS/riaps-core/releases>`_

Test the Install
================

.. note::

   Todo: Test the install