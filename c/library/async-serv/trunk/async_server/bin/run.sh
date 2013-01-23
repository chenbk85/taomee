#!/bin/bash

rm -fr ../log/bench/*
rm -fr ../log/load_presure/*
rm -fr ../log/proxy/*

red_clr="\033[31m"
grn_clr="\033[32m"
end_clr="\033[0m"

pid=`cat ./bench_pid`
result=`ps -p $pid | wc -l`

if [ $result -gt 1 ]; then
    kill `cat ./bench_pid`
    sleep 1
    pid=`cat ./bench_pid`
    result=`ps -p $pid | wc -l`
    if [ $result -gt 1 ]; then
        printf "$red_clr%s$end_clr\n" "bench is still running"
    else
        printf "$grn_clr%s$end_clr\n" "bench has been stopped"
    fi
else
    printf "$red_clr%s$end_clr\n" "bench is not running"
fi

./async_server ../etc/bench.conf
#-----------------------------

pid=`cat ./proxy_pid`
result=`ps -p $pid | wc -l`

if [ $result -gt 1 ]; then
    kill `cat ./proxy_pid`
    sleep 1
    pid=`cat ./proxy_pid`
    result=`ps -p $pid | wc -l`
    if [ $result -gt 1 ]; then
        printf "$red_clr%s$end_clr\n" "proxy is still running"
    else
        printf "$grn_clr%s$end_clr\n" "proxy has been stopped"
    fi
else
    printf "$red_clr%s$end_clr\n" "proxy is not running"
fi

./async_server ../etc/proxy.conf


#killall async_server
#rm -fr ../log/bench/*
#rm -fr ../log/load_presure/*
#rm -fr ../log/proxy/*
#sleep 1
#./async_server ../etc/bench.conf
#./async_server ../etc/proxy.conf
#./async_server ../etc/load_presure.conf
