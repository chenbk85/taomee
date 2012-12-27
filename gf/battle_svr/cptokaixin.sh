#/bin/bash
sh ./mk_kaixing.sh
tar -czvf btl.tar ./bin/* ./conf/* ./data/*  ./ai/lua/*
sshpass -p "08GtRb8Z" scp -p22 -o StrictHostKeyChecking=no btl.tar gongfupai@27.131.221.207:~/BtlsvrA
sshpass -p '08GtRb8Z' ssh -p22 -o StrictHostKeyChecking=no -lgongfupai 27.131.221.207  "pkill -9 BtlsvrA;cd ~/BtlsvrA/; tar -xvf btl.tar ;./startup.sh"
