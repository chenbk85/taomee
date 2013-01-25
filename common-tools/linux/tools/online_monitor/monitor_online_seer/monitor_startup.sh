rm -f nohup.out
pkill -9 -f seer_monitor_online
nohup ./seer_monitor_online_svr &
