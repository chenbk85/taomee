--开发的环境配置
log_dir = "./log"
mcast_ip = "239.0.0.5"

local ip = get_ip(1);

if (ip == "10.1.5.26") then
    bind_id = 8
else
    bind_id = 10
end

