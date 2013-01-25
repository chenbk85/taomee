cd `dirname $0` 
kill -TERM `ps -ef | grep "advert_filter ../conf/bench.conf" | grep -v grep | awk '{print $2}'`
sleep 1
#valgrind --leak-check=full ./advert_filter ../conf/bench.conf
./advert_filter ../conf/bench.conf
