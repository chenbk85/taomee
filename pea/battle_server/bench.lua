--程序名称
local program_name = string.match( get_program_name(), "(%w+)$" );
--内网ip
local ip = get_ip(1);
--公网ip
local public_ip = get_ip(2);

print ( program_name .." "..public_ip.." "..ip );


dll_file = "./libbattle.so"

-- 不同的配置
if (program_name == "btlsvra") then
    require "./test"
else
    require "./dev"
end

-- item 格式 { id, "server.name", "ip", port  } 
async_server_bind_map = { }

local server_name = "ml.btlsvr";
local bind_id = 33;
async_server_bind_map[0] = { bind_id, server_name, ip, 6400 + bind_id };
--if ( ip=="192.168.0.1" and program_name=="onlineA" ) then
    --for i = 2, 5, 2 do
        --async_server_bind_map[ #async_server_bind_map + 1]={ i,online_server_name ,public_ip, 6000+i };
    --end
--end


net_loop_interval = 10


--log level
log_level = 8
--max size of each log file
log_size = 104857600
--max number of log files per log level
max_log_files = 100

--max opened fd allowed. If this item is missing, then the default value 20000 will be used
max_open_fd = 20000
--size of the buffer to hold incoming/outcoming packets
shmq_length = 11685760
--running mode of the program
run_mode = "background"


--mcast port
mcast_port = 8888
--interface on which arriving multicast datagrams will be received
mcast_incoming_if = "eth0"

--mcast config for synchronize the name and ip address of a given service
--mcast ip , 5 for gongfu
addr_mcast_ip = mcast_ip
--mcast port
addr_mcast_port = 8889
--interface on which arriving multicast datagrams will be received
addr_mcast_incoming_if = "eth0"

--warning_ip = "192.168.0.39"
--warning_port = 33001
--project_name = "pea"
--phone_numbers = "13761071357,13808888888"



--tm_dirty
--是否启动脏词检测功能:0不启动，1启动。默认启动
tm_dirty_use_dirty_logical = 0
--本地脏词路径
tm_dirty_local_dirty_file_path = "./data/dirty.dat"
--脏词服务器地址 格式：ip:port。多个服务器用分号(半角)隔开，最多16个。
tm_dirty_server_addr = "10.1.1.155:28000;192.168.4.68:28000;192.168.4.68:28001"
--脏词更新周期 单位：秒.最少10秒
tm_dirty_update_cycle = 600
