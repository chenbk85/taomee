--程序名称
local program_name = string.match( get_program_name(), "(%w+)$" );
--内网ip
local ip = get_ip(1);
--公网ip
local public_ip = get_ip(2);

print ( program_name .." "..public_ip.." "..ip );


--so file
    dll_file = "./libonline.so"

-- 不同的配置
if (program_name == "onlinea") then
    require "./test"
else
    require "./dev"
end


-- item 格式 { id, "server.name", "ip", port  } 
async_server_bind_map = { }

local server_name = "pea.online";
async_server_bind_map[0] = { bind_id, server_name, ip, 6800 + bind_id };

--if ( ip=="192.168.0.1" and program_name=="onlineA" ) then
    --for i = 2, 5, 2 do
        --async_server_bind_map[ #async_server_bind_map + 1]={ i,online_server_name ,public_ip, 6000+i };
    --end
--end

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
tm_dirty_shm_file_path = "/home/singku/pea/dirty/tm_dirty_shm_file"

--battle switch
battle_switch_name = "pea.btlsw"

--dbproxy
dbproxy_name = "pea.dbproxy"
