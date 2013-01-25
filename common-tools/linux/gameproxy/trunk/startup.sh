export LD_LIBRARY_PATH=./bin:$LD_LIBRARY_PATH
pkill -9 extend 
sleep 1
./extend ./bench.conf
