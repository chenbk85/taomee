#!/bin/sh
export LD_LIBRARY_PATH=/opt/taomee/monster/monster/server/trunk/lib/mysql-iface-1.0.1/
./bin/db_server ./conf/db_server.conf ./bin/libdbserver.so
