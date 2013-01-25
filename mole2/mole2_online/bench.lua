--得到所在地类型

--程序名称
local program_name= string.match( get_program_name() ,"(%w+)$" )
--内网ip
local ip=get_ip(1) ;
--公网ip
local public_ip=get_ip(2) ;

cfg_center_ip ="10.1.1.5"
cfg_center_port =13330
listen_mode       ="new_listen_mode"
server_type ="online"
net_type="telcom"

-- dir to store logs
log_dir = "./log" 
-- log level
log_level = "7" 
-- max size of each log file
tlog_file_interval_sec =  "900" 

incoming_packet_max_size = "32768" 

-- size of the buffer to hold incoming/outcoming packets
shmq_length = "8388608" 
-- running mode of the program
run_mode = "background" 

project_name = "mole2" 
phone_numbers = "g_mole2_online" 



--print ("------------------------------------------------")
--print ( string.format("参数: 程序名： %s  公网ip: %s 内网ip: %s ", program_name, public_ip,ip  )) ;
--print ("------------------------------------------------")

if (ip=="10.1.1.46" )   then
	dofile("./mole2_online_46.lua");
	return ;

elseif (ip=="10.1.1.154" )   then
	dofile("./mole2_online_154.lua");
	return ;
elseif ( ip=="10.1.1.134" ) then
	dofile("./mole2_online_134.lua");
	return ;
elseif ( string.match(ip ,"^192.168.113." )  ) then 
	dofile("./mole2_online_dx.lua");
	cfg_center_ip ="192.168.0.27"

	net_type="telcom"
	--电信
	return ;
elseif ( string.match(ip ,"^192.168.31." )  ) then
	dofile("./mole2_online_wt.lua");
	cfg_center_ip ="192.168.0.27"
	net_type="netcom"
	--网通
	return ;
end

print ("不在合法的ip上" )
return ;

