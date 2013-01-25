--得到所在地类型

--程序名称
local program_name=get_program_name() ;
--内网ip
local ip=get_ip(1) ;
--公网ip
local public_ip=get_ip(2) ;


local local_type="";
if ( string.match(ip ,"^192.168.8." )  ) then
 	local_type="dx";
elseif ( string.match(ip ,"^192.168.240." )  ) then
 	local_type="wt";
end

print ("------------------------------------------------")
print ( string.format("参数: 程序名： %s  公网ip: %s 内网ip: %s ", program_name, public_ip,ip  )) ;
print ( string.format("类型 : %s",local_type )) ;
print ("------------------------------------------------")

--处理内网环境
if (ip=="10.1.1.46" ) then
	dofile("../etc/mole2_db_46.lua" );
	return ;
elseif (ip=="10.1.1.134" ) then
	dofile("../etc/mole2_db_134.lua" );
	return ;
end


if local_type=="" then
	print ("不在合法的ip上" )
	return ;
end



--------- 全局变量
log_dir = "../log"
log_level = 7
log_size = 32000000

pkg_timeout = 5
worker_num = 30
send_sem_key = 97856
recv_sem_key = 97858
max_log_files = 0
log_save_next_file_interval_min = 15

DB_IP = "localhost"
DB_USER = "dbuser"
DB_PASSWD = "MyDKs2KdSI"

if (local_type=="dx" ) then
	LOG_VIP_ITEM_ADDR ="192.168.8.51:13001"
	CHANGE_LOG_ADDR  ="192.168.8.91:30004"
else
	LOG_VIP_ITEM_ADDR = "192.168.240.40:13001"
	CHANGE_LOG_ADDR = "192.168.240.43:30004"
end

--#====日志===begin="
--#sql"
IS_LOG_DEBUG_SQL = 0
--#16进制"
LOG_HEX_DATA_FLAG = 0



--处理 bind.conf
-- item 格式 {   "ip", port, "tcp/udp" ,  0 ("idle timeout")  } 
server_bench_bind_map={ }
local port=13001
server_bench_bind_map[ #server_bench_bind_map+1]={ ip , port, "tcp",0 };
server_bench_bind_map[ #server_bench_bind_map+1]={ ip , port, "udp",0 };



