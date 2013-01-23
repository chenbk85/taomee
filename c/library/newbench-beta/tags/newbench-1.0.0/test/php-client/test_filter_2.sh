#!/bin/bash

for ((a=1; a <= 100 ; a++))
do
    php test_filter_no_recv.php 50077 1 "$a" &
    read
done
