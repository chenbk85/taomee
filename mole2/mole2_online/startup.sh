cd `dirname $0` 
#rm  -f ./bin/libm2online.so 
cp ./build/libm2online.so ./bin/	
cp ./build/libm2data.so ./bin/	
killall -9 m2_online 
sleep 1
./m2_online ./bench.lua
