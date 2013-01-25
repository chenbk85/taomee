killall -9 CltServ
#pkill -15 CltServ
rm -f log/*
rm -f tmp/*
rm -rf mmap_file/*
./CltServ ./bench.conf
