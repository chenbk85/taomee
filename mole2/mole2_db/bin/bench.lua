--得到所在地类型

--程序名称
local program_name=get_program_name() ;
--内网ip
local ip=get_ip(1) ;
--公网ip
local public_ip=get_ip(2) ;

svc_type="db"
project_name="mole2"

if ( string.match(ip ,"^192.168.73." )  ) then
	net_type="telcom"
elseif ( string.match(ip ,"^192.168.31." )  ) then
	net_type="netcom"
elseif ( ip =="10.1.1.154"   ) then
	net_type="telcom"
	dofile("./mole2_db_154.lua" );
	return ;
elseif ( ip =="10.1.1.134"   ) then
	net_type="telcom"
	dofile("./mole2_db_134.lua" );
	return ;

end

print ("------------------------------------------------")
print ( string.format("参数: 程序名： %s  公网ip: %s 内网ip: %s ", program_name, public_ip,ip  )) ;
print ("------------------------------------------------")

--------- 全局变量
if net_type==""  then
	print ("不在合法的ip上" )
	return ;
end


log_dir = "./log"
log_level = 7
log_size = 32000000

pkg_timeout = 5
worker_num = 30
send_sem_key = 97856
recv_sem_key = 97858
max_log_files = 0
tlog_file_interval_sec = 900
DB_IP = "localhost"
DB_USER = "dbuser"
DB_PASSWD = "MyDKs2KdSI"

if (net_type=="telcom" ) then
	LOG_VIP_ITEM_ADDR ="192.168.73.32:13001"
	CHANGE_LOG_ADDR  ="192.168.73.32:30004"
else
	LOG_VIP_ITEM_ADDR = "192.168.31.68:13001"
	CHANGE_LOG_ADDR = "192.168.31.68:30004"
end

--#====日志===begin="
--#sql"
IS_LOG_DEBUG_SQL = 1
--#16进制"
LOG_HEX_DATA_FLAG = 0


--处理 bind.conf
-- item 格式 {   "ip", port, "tcp/udp" ,  0 ("idle timeout")  } 
server_bench_bind_map={ }
local port=13001
server_bench_bind_map[ #server_bench_bind_map+1]={ ip , port, "tcp",0 };
server_bench_bind_map[ #server_bench_bind_map+1]={ ip , port, "udp",0 };


cfg_center_ip ="192.168.0.27"
cfg_center_port =3389

