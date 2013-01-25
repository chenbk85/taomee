cd ` dirname $0 `
pkill -9 Batrserv
sleep 1
./Batrserv ../conf/config.lua
