#!/bin/bash

echo COPY to BBB1
scp ../build/Debug/actor_pub \
    ../build/Debug/librdiscovery.so \
    ../build/Debug/libpublisher_component.so \
    ../build/Debug/riaps_discoveryd_armhf \
    ../build/Debug/pub_conf.json \
    ../build/Debug/libzcm.so \
    ~/Work/dependencies/libzmq/armhf/lib/libzmq.so.5 \
    root@bbb1.local:/root \


echo COPY to BBB2
scp ../build/Debug/actor_sub \
    ../build/Debug/librdiscovery.so \
    ../build/Debug/riaps_discoveryd_armhf \
    ../build/Debug/sub_conf.json \
    ../build/Debug/libsubscriber_component.so \
    ../build/Debug/libzcm.so \
    ~/Work/dependencies/libzmq/armhf/lib/libzmq.so.5 \
    root@bbb2.local:/root \

