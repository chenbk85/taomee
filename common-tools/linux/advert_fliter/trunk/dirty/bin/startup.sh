cd `dirname $0` 
kill -TERM `ps -ef | grep "dirty ../conf/bench.conf" | grep -v grep | awk '{print $2}'`
sleep 1
./dirty ../conf/bench.conf
