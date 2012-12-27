#!/bin/sh
pkill -9 Register
sleep 1
./Register ../etc/bench.conf ./libregister.so  -s 4096







