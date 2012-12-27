pkill -9 TRADE
sleep 1
rm -f ./log/*
./TRADE ./bench.conf
