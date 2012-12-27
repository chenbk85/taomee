pkill -9 new_login
rm -f ./log/*
export LD_LIBRARY_PATH=/home/sylar/asyc_main_login/trunk/bin/
export LD_LIBRARY_PATH=./bin/
sleep 1
./new_login ./bench.conf

