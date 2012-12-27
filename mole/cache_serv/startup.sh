#!/bin/sh
pkill CacheSvr
rm -rf log/*
sleep 3
./CacheSvr conf/bench.conf ./bin/libcache_serv.so
