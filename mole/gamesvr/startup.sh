#!/bin/bash
pkill -9 gameserver
sleep 1
rm -f log/*
./gameserver ./bench.conf
