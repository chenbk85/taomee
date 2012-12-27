cd ` dirname $0 `
pkill -9 Switch
sleep 1
./Switch ../conf/bench.conf
