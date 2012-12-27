#!/bin/sh
cd `dirname $0`
./reload -a -c 1 -s pop.online -m ./libonline.so -g 239.0.0.3 -b 10.1.1.101 -p 5608 -i eth0
