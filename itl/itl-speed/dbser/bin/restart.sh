#!/bin/bash
cd $(dirname $0 ) 
./daemon.sh stop
sleep 2
./daemon.sh start 
