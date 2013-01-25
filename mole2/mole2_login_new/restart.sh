#!/bin/bash
cd `dirname $0`
pkill -9 M2Login
#rm -rf ./log/* 
./M2Login bench.conf
