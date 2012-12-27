pkill -9 HomeSVR
sleep 1
rm -f ./log/*
./Trade ./bench.conf
