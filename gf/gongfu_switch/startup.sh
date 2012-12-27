#!/bin/sh
pkill -9 gfnow_switch
sleep 1
./gfnow_switch bench.conf ./bin/lib_gongfu_switch.so
