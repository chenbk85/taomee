#!/bin/sh
pkill -9 PPCacheSer
sleep 3
./PPCacheSer conf/bench.conf ./bin/libcache_serv.so
