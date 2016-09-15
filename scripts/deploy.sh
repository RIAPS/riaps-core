#!/bin/bash

echo COPY to BBB1
scp ../build/Debug/actor_pub \
    ../build/Debug/actor_sub \
    ../build/Debug/libriaps.so \
    ../build/Debug/rdiscoveryd \
    root@bbb1.local:/root \


echo COPY to BBB2
scp ../build/Debug/actor_pub \
    ../build/Debug/actor_sub \
    ../build/Debug/libriaps.so \
    ../build/Debug/rdiscoveryd \
    root@bbb2.local:/root \
