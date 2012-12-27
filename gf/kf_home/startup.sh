#!/bin/sh
pkill -9 kf_home
sleep 3
./kf_home bench.conf ./bin/lib_kf_home.so
