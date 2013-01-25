kill -TERM `ps -ef | grep "ReloadTest ./bench.conf" | grep -v grep | awk '{print $2}'`
sleep 1
./ReloadTest ./bench.conf
