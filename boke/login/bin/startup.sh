cd ` dirname $0 `
pkill -9 Login 
sleep 1
./Login ../conf/bench.conf
