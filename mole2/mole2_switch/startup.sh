cd `dirname $0`
pkill -9 Mole2Switch
sleep 1
pwd
./Mole2Switch ./bench.conf
