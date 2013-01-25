--得到所在地类型
--程序名称
local program_name=get_program_name() ;
--内网ip
local ip=get_ip(1) ;
--公网ip
local public_ip=get_ip(2) ;


log_dir = "../log" 
log_level = 7 
log_size = 32000000 

pkg_timeout = 5 
worker_num = 2 
send_sem_key = 97866 
recv_sem_key = 97868 
max_log_files = 0 
tlog_file_interval_sec = 86400 

DB_IP = "10.1.1.46" 
DB_USER = "root" 
DB_PASSWD = "taomee" 

LOG_VIP_ITEM_ADDR = "10.1.1.46:13001" 

IS_LOG_DEBUG_SQL = 1 
LOG_HEX_DATA_FLAG = 1 
CHANGE_LOG_ADDR = "10.1.1.58:30004" 

cfg_center_ip ="10.1.1.5"
cfg_center_port =13330

server_bench_bind_map={ }
local port=14001
server_bench_bind_map[ #server_bench_bind_map+1]={ ip , port, "tcp",0 };
server_bench_bind_map[ #server_bench_bind_map+1]={ ip , port, "udp",0 };


