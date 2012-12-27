#!/bin/sh
#ssh -p 56000 online@114.80.98.7 -o 'BatchMode yes' "cd /opt/taomee/cache_svr/bin/&&mv libcache_serv.so libcache_serv.so.bak\
#	&&cd /opt/taomee/cache_svr/log/&&rm -f *"
scp -P56000 ./libcache_serv.so online@192.168.0.7:/opt/taomee/cache_svr/bin/
#ssh online@114.80.98.7 -p 56000 -o 'BatchMode yes' "/opt/taomee/cache_svr/startup.sh" &
