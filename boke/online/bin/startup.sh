cd `dirname $0` 
pkill -9 poponline 
sleep 1
./poponline ../conf/bench.conf
