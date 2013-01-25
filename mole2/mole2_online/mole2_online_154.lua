--程序名称
local program_name= string.match( get_program_name() ,"(%w+)$" )
--内网ip
local ip=get_ip(1) ;
--公网ip
local public_ip=get_ip(2) ;

new_server_list    = "1-4";
new_listen_ports    ="8080;8081"


-- dir to store logs
log_dir = "./log" 
-- log level
log_level = "8" 
-- max size of each log file
log_size = "104857600" 

tlog_file_interval_sec =  86400	

-- size of the buffer to hold incoming/outcoming packets
shmq_length = "8388608" 
-- running mode of the program
run_mode = "background" 

-- mcast address
mcast_ip = "239.0.0.1" 
-- mcast port
mcast_port = "5555" 
-- interface on which arriving multicast datagrams will be received
mcast_incoming_if = "eth0" 
-- interface for outgoing multicast datagrams
mcast_outgoing_if = "10.1.1.154" 

incoming_packet_max_size = "32768" 

-- dll file
data_dll_file = "./bin/libm2data.so" 
dll_file = "./bin/libm2online.so" 

-- bind file
bind_conf = "./bind.conf" 

-- statistic file
--statistic_file = "/home/ian/stat/client/inbox/bin.log" 
statistic_file = "./bin.log" 

--dbproxy_port	30004
--dbproxy_ip		10.1.1.23
service_dbproxy = "mole2.dbproxy" 
service_switch = "mole2.switch46" 
service_batrserv = "mole2.batrserv" 
service_homeserv = "mole2.home" 
service_cross = "mole2.cross" 
codesvr_ip = "10.1.1.44" 
codesvr_port = "58111" 
idc_type = "3" 
report_svr_ip = "10.1.1.24" 
report_svr_port = "30005" 
post_svr_ip = "10.1.1.58" 
post_svr_port = "30004" 
channel_id = "60" 
security_code = "12345678" 

channel_id_2 = "90" 
security_code_2 = "73926476" 
-- 短信报警服务器IP。当系统出现异常时，发送UDP报文给短信报警服务器，指定的手机号码就能收到短信报警
warning_ip = "192.168.0.39" 
-- # 短信报警服务器端口
warning_port = "33001" 
project_name = "mole2" 
phone_numbers = "13671814436" 


--# mcast config for synchronize the name and ip address of a given service
addr_mcast_ip = "239.0.0.3" 
addr_mcast_port = "5601" 
--## interface on which arriving multicast datagrams will be received
addr_mcast_incoming_if = "eth0" 
--## interface for outgoing multicast datagrams
addr_mcast_outgoing_if = ip 

dx_chat_svr_ip = "192.168.8.91" 
wt_chat_svr_ip = "192.168.240.43" 
in_chat_svr_ip = "10.1.1.58" 
chat_svr_port = "30004" 
in_spacetime_svr_ip = "10.1.1.44" 
spacetime_svr_ip = "192.168.0.156" 
spacetime_svr_port = "26122" 
in_mall_svr_ip = "10.1.1.47" 
mall_svr_ip = "192.168.1.45" 
mall_svr_port = "12403" 

in_vipserv_ip = "10.1.1.65" 
vipserv_port = "14409"

warning_ip = "192.168.4.68" 
warning_port = "33001" 

-------------------------------------------
project_name ="mole2"
project_id ="7"
svc_type="online"
server_start_id=1
server_end_id=2


listen_interface ="eth0"
listen_ports   ="8010;8011"


--tm_dirty_ads 与广告检测相关的配置,由业务自行选择配置与否
--广告检测服务端IP
tm_dirty_ads_udp_report_ip       ="192.168.4.68"
--广告检 测服务端port
tm_dirty_shm_file_path     ="./data/tm_dirty_shm_file"
tm_dirty_ads_udp_report_port     = 28000



