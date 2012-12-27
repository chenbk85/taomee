#!/bin/sh
./deploy.sh
sshpass -p "moledb142857" scp -P 22000 ../StableOnline/online.tar moledb@10.1.1.145:~/online
sshpass -p "moledb142857" ssh moledb@10.1.1.145 -p22000 "cd ~/online && tar -xvf online.tar > /dev/null && mv OnlineTest1 OnlineNew\
			&& ./startup.sh > /dev/null && rm -f online.tar" 
