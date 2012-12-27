#!/bin/bash
make clean all 2>make_log
sleep 1
vim make_log
