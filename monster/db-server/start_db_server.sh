#!/bin/sh
export LD_LIBRARY_PATH=/opt/taomee/monster/taomee/monster/server/trunk/lib/so/
./bin/db_server ./conf/db_server.conf ./bin/libdbserver.so
