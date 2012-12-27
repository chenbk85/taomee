pkill -9 gf_login
#export LD_LIBRARY_PATH=/home/sylar/asyc_main_login/trunk/bin/
export LD_LIBRARY_PATH=./bin/
sleep 1
./gf_login ./bench.conf

