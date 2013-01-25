#!/bin/bash
cd $(dirname $0)
./reload -a -c 1 -s mole2.batrserv -m ./libbatrserv.so -g 239.0.0.3 -b 10.1.1.46 -p 5601 -i eth0

