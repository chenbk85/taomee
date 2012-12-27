pkill -9 Cache
sleep 1
rm -f ./log/*
./Cache ./bench.conf
