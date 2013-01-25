cd `dirname $0`
pkill -9 M2home
#rm -rf ./log/*
sleep 1
./M2home ./bench.conf
