--程序名称
local program_name=string.match( get_program_name() ,"(%w+)$" );
--内网ip
local ip=get_ip(1) ;
--公网ip
local public_ip=get_ip(2) ;

local local_type="dx";
if (ip =="10.1.1.46"  ) then
    local_type="test_46";
elseif (ip =="10.1.1.134"  ) then
    local_type="test_134";
elseif ( string.match(ip ,"^192.168.0." )  ) then
    local_type="dx";
elseif ( string.match(ip ,"^192.168.240." )  ) then
    local_type="wt";
end


print ( string.format("参数: %s %s %s ", program_name, public_ip,ip  )) ;
print ( string.format("类型 : %s",local_type )) ;
print ("------------------------------------------------")


log_dir = "../log"
log_level = 7
log_size = 104857600
max_log_files = 300  


if (local_type=="dx" or local_type=="wt"  ) then
	--外网
	log_save_next_file_interval_min= 15; 
else
	--内网
	log_save_next_file_interval_min= 0;
end

shmq_length = 20971520 
run_mode = "background"

--so--
dll_file = "./libbatrserv.so";
data_dll_file = "./libbatrserv_data.so";


service_dbproxy = "mole2.dbproxy";

--报警--
warning_ip = "192.168.4.68";
warning_port = "33001";
project_name = "mole2";
phone_numbers = "13671814436,13661873164,13818397496,13817755843,15921388605";


--tm_dirty_use_dirty_logical = 1
--脏词配置
if (local_type=="dx" or local_type=="wt"  ) then
	tm_dirty_use_dirty_logical = 1
	tm_dirty_local_dirty_file_path = "./data/dirty.dat"
	tm_dirty_server_addr = "192.168.4.68:28000;192.168.4.68:28001"
	tm_dirty_update_cycle = 600
end



--域名组播--
if ( local_type== "test_46" ) then
	addr_mcast_ip = "239.0.0.3";
	addr_mcast_incoming_if = "eth0";
else
	-- mcast config for synchronize the name and ip address of a given service
	addr_mcast_ip = "239.0.0.8";
	-- interface on which arriving multicast datagrams will be received
	addr_mcast_incoming_if = "eth1";
end

addr_mcast_port = "5601";
addr_mcast_outgoing_if = ip;


-- bind conf 

-- item 格式 { id, "server.name", "ip", port  } 
async_server_bind_map={ }


local online_server_name="mole2.batrserv";

for i = 1,3 do
	async_server_bind_map[ #async_server_bind_map +1]={ i,online_server_name, ip,  2100+i };
end


