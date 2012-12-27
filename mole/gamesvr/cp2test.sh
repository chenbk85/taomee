#!/bin/sh
./deploy.sh
sshpass -p "moledb142857" scp ../StableGameserv/gameserv.tar moledb@10.1.1.145:~/gameserv
sshpass -p "moledb142857" ssh moledb@10.1.1.145 "cd ~/gameserv && tar -xvf gameserv.tar && mv GameservTest gameserver" 
sshpass -p "moledb142857" ssh moledb@10.1.1.145 "cd ~/gameserv && ./startup.sh > /dev/null && rm -f gameserv.tar" 
