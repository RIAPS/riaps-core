#!/usr/bin/env bash

echo COPY to 192.168.1.120
sshpass -p 'riapspwd' scp ../../bin/rdiscoveryd riaps@192.168.1.120:/opt/riaps/armhf/bin

echo COPY to 192.168.1.120
sshpass -p 'riapspwd' scp ../../bin/libriaps.so riaps@192.168.1.120:/opt/riaps/armhf/lib

echo COPY to 192.168.1.121
sshpass -p 'riapspwd' scp ../../bin/rdiscoveryd riaps@192.168.1.121:/opt/riaps/armhf/bin

echo COPY to 192.168.1.121
sshpass -p 'riapspwd' scp ../../bin/libriaps.so riaps@192.168.1.121:/opt/riaps/armhf/lib