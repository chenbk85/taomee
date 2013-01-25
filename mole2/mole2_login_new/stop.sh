kill -TERM `ps -ef | grep "M2Login ./bench.conf" | grep -v grep | awk '{print $2}'`
