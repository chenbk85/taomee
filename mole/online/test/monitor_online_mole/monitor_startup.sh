rm -f nohup.out
pkill -9 -f mole_monitor_online
nohup ./mole_monitor_online_svr &
nohup ./mole_monitor_online_cli.pl &
