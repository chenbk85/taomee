cp -v ./games/lib*.so ../StableGameserv/games/
cp -v ./games/*.xml ../StableGameserv/games/
cp -v ./conf/* ../StableGameserv/conf
cp -v ./conf/angelfight/* ../StableGameserv/conf/angelfight/
cp -v ./gameserver ../StableGameserv/GameservTest
cp -v ./readme ./changelog ../StableGameserv
cd ../StableGameserv
#file=gameserv.tar
tar cvf gameserv.tar conf games GameservTest readme changelog
#expect -f ../gameserv/gameserv.tcl
cd -
