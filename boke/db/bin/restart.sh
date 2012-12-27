#!/bin/bash
cd $(dirname $0 ) 
./daemon.sh stop
./daemon.sh start 
