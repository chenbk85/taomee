#!/bin/sh
export LD_LIBRARY_PATH=/opt/taomee/monster/taomee/monster/server/trunk/lib/mysql-iface-1.0.1/ 
./bin/db_cache_server ./conf/db_cache.conf ./bin/libdbcacheserver.so
