cd 	`dirname $0`
rm -rf ../log/*
pkill -9 WebProxy
sleep 1
./WebProxy ./bench.conf ./libwebproxy.so
