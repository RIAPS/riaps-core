Set Up the Development Machine
##############################

Install RIAPS dependencies
==========================

On 18.04+ install the following packages:

.. code:: bash

   sudo apt install byacc flex python3-dev pkg-config libtool libtool-bin build-essential autoconf \
                    libboost-dev autogen libpython3-dev cython3 python3-setuptools libncurses5-dev \
                    libreadline-dev nettle-dev libgnutls28-dev libmsgpack-dev
   sudo apt install libpython3-dev:armhf libncurses5-dev:armhf libreadline-dev:armhf \
                    nettle-dev:armhf libgnutls28-dev libmsgpack-dev

Install RIAPS Packages
======================

Download and install:
 - `riaps-externals <https://github.com/RIAPS/riaps-externals/releases>`_
 - `riaps-core <https://github.com/RIAPS/riaps-core/releases>`_

Test the Install
================

// TODO