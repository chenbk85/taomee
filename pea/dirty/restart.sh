#!/bin/sh
./stop.sh
sleep 2
mkdir -p dirty_log
rm -f core.*
rm -f dirty_log/*
./dirty_agent ./dirty_agent.conf
