--程序名称
local program_name= string.match( get_program_name() ,"([^/]+)$" )
--内网ip
local ip=get_ip(1) ;
--公网ip
local public_ip=get_ip(2) ;

-- 不同的配置
if (program_name == "dbproxya") then
    require "./test"
else
    require "./dev"
end


-- log level
log_level = 8 
-- max size of each log file
log_size = 104857600 
-- max number of log files per log level
max_log_files = 100 

-- size of the buffer to hold incoming/outcoming packets
shmq_length = 11685760 
-- running mode of the program
run_mode = "background" 

-- mcast port
mcast_port = 8888 
-- interface on which arriving multicast datagrams will be received
mcast_incoming_if = "eth0" 
-- interface for outgoing multicast datagrams
mcast_outgoing_if = ip 
incoming_packet_max_size = "3276800" 

-- dll file
dll_file = "./libproxy.so" 

-- bind file
LOG_PROTO_FLAG = "0" 
route_file = "./route_smyang.xml" 

------加载其它模块列表列表 以","分开-----begin-----
--DLL_FILE_LIST		 	 ./libuser_passwd_cache.so, ./libsu_change_log.so,./libmole_db.so
------加载其它模块列表------end----


--# mcast config for synchronize the name and ip address of a given service
addr_mcast_ip = mcast_ip
addr_mcast_port = 8889
--### interface on which arriving multicast datagrams will be received
addr_mcast_incoming_if = "eth0" 
--
addr_mcast_outgoing_if = ip 

--------------------------------
--bind
-- item 格式 { id, "server.name", "ip", port  } 
async_server_bind_map={ }
local server_name = "pea.dbproxy";
local bind_id = 8;
async_server_bind_map[0]={ bind_id, server_name, ip, 21000 + bind_id }


--tm_dirty
--是否启动脏词检测功能:0不启动，1启动。默认启动
tm_dirty_use_dirty_logical = 0
--本地脏词路径
tm_dirty_local_dirty_file_path = "./data/dirty.dat"
--脏词服务器地址 格式：ip:port。多个服务器用分号(半角)隔开，最多16个。
tm_dirty_server_addr = "10.1.1.155:28000;192.168.4.68:28000;192.168.4.68:28001"
--脏词更新周期 单位：秒.最少10秒
tm_dirty_update_cycle = 600

