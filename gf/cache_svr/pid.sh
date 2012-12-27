#!/bin/sh
ps -elf > f
for pid in $(awk '/CacheSvr/{print $4 }' f)
do
	kill -s HUP $pid
done
