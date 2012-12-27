#!/bin/sh
export LD_LIBRARY_PATH=/home/monster/lib/
./bin/ucount_server ./conf/ucount_server.conf ./bin/libucountserver.so
