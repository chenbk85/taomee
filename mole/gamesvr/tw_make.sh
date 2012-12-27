#!/bin/sh
dir=`date +%Y%m%d`
pkill -9 gameserver_tw
make -f make.tw clean all
cp gameserver_tw tw/
cp changelog tw/
cp games/*.so tw/games
cp games/*.xml tw/games
cp conf/*.xml tw/conf
cp conf/angelfight/*.xml tw/conf/angelfight
cd tw/
pkill -9 gameserver_tw
sleep 1
./gameserver_tw ./bench.conf
rm log/*
rm core.*
cd ..
file=tw_gameserv_$dir.tar.gz
tar cvfz $file tw 

