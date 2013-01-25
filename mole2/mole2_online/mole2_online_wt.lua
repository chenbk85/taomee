--程序名称
local program_name= string.match( get_program_name() ,"([^/]+)$" )
--内网ip
local ip=get_ip(1) ;
--公网ip
local public_ip=get_ip(2) ;

-- dir to store logs
log_dir = "./log" 
-- log level
log_level = "7" 
-- max size of each log file
log_size = "104857600" 
-- max number of log files per log level
max_log_files = "100" 
tlog_file_interval_sec =  900 
incoming_packet_max_size = "32768" 

-- size of the buffer to hold incoming/outcoming packets
shmq_length = "8388608" 
-- running mode of the program
run_mode = "background" 

-- mcast address
mcast_ip = "239.0.0.240" 
-- mcast port
mcast_port = "5538" 
-- interface on which arriving multicast datagrams will be received
mcast_incoming_if = "eth1" 
-- interface for outgoing multicast datagrams
mcast_outgoing_if = ip 

project_name = "wtmole2" 
phone_numbers = "g_mole2_online" 

post_svr_ip = "192.168.0.32" 
post_svr_port = "30004" 




-- dll file
data_dll_file = "./bin/libm2data.so" 
dll_file = "./bin/libm2online.so" 

-- bind file
--bind_conf = "./bind.conf" 

-- statistic file
statistic_file = "/opt/taomee/stat/spool/inbox/bin.log" 

--dbproxy_port   30004
--dbproxy_ip             10.1.1.23
service_dbproxy = "wtmole2.dbproxy" 
service_switch = "wtmole2.switch" 
service_batrserv = "wtmole2.btlsvr" 
service_homeserv = "wtmole2.home" 

--登录登出记录
report_svr_ip = "192.168.240.43" 
report_svr_port = "11001" 


idc_type = "2" 
channel_id = "60" 
security_code = "89123489" 

--# mcast config for synchronize the name and ip address of a given service
addr_mcast_ip = "239.0.0.240" 
addr_mcast_port = "5601" 
--## interface on which arriving multicast datagrams will be received
addr_mcast_incoming_if = "eth1" 
--## interface for outgoing multicast datagrams
addr_mcast_outgoing_if =  ip

codesvr_ip = "192.168.1.156" 
codesvr_port = "58111" 


channel_id_2 = "90" 
security_code_2 = "73926476" 
in_vipserv_ip = "10.1.1.65" 
vipserv_ip = "192.168.1.59" 
vipserv_port = "14409"

dx_chat_svr_ip = "192.168.73.32" 
wt_chat_svr_ip = "192.168.31.68" 
in_chat_svr_ip = "10.1.1.58" 
chat_svr_port = "30004" 
in_spacetime_svr_ip = "10.1.1.44" 
spacetime_svr_ip = "192.168.1.156" 
spacetime_svr_port = "26122" 
in_mall_svr_ip = "10.1.1.47" 
mall_svr_ip = "192.168.1.45" 
mall_svr_port = "12403" 

warning_ip = "192.168.4.68" 
warning_port = "33001" 
--bind
-- item 格式 { id, "server.name", "ip", port  } 
async_server_bind_map={ }

project_name ="mole2"
project_id ="7"
svc_type="online"


online_server_name="mole2.online"

listen_interface ="eth0"
--200 个分配
--1,100 奇数
if (program_name=="onlinea" ) then
new_server_list    = "1-50";
new_listen_ports    ="8080"
--listen_ports   ="8080"
--server_start_id=1
--server_end_id=50
end

--1,100 偶数 
if (program_name=="onlineb" ) then
new_server_list    = "51-100";
new_listen_ports    ="8081"
--listen_ports   ="8081"
--server_start_id=51
--server_end_id=100
end

--tm_dirty_ads 与广告检测相关的配置,由业务自行选择配置与否
--广告检测服务端IP
tm_dirty_ads_udp_report_ip       ="192.168.4.68"
--广告检 测服务端port
tm_dirty_shm_file_path     ="./data/tm_dirty_shm_file"
tm_dirty_ads_udp_report_port     = 28000

cfg_center_ip    ="10.1.1.141"
cfg_center_port   =  3389

