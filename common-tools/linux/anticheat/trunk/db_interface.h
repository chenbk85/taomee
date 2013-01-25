#ifndef __DB_INTERFACE_H__
#define __DB_INTERFACE_H__



#define DBIF_STATISTICS_INTERVAL		(60)
#define DEF_CFG_PATH					"./conf/dbif.conf"



#pragma pack(1)


#pragma pack()



struct dbif_config_t {
    /** 是否是后台进程 */
    cfg_bool_t  background;
    /** 是否是 debug 模式(debug不关闭0,1,2) */
    cfg_bool_t  debug_mode;

	/** bind 地址属性: all:0.0.0.0, outer/eth0:外网地址, inner/eth1:内网地址 */
	char		bind_ethx[16];
	/** bind 地址(字符串): 根据 bind_addr_attr 算出来 */
	char		bind_ip[INET_ADDRSTRLEN];
	/** bind 端口号(本机序) */
	uint16_t	bind_port;
	/** ip:port, 用 ip:port 拼成 */
	char		dbif_addr[INET_ADDRSTRLEN + 16];

	/* for log */
	char		log_dir[4096];    
	int			max_log_lvl; 
	int			max_log_size;
	int			max_log_file;
	char		log_prefix[128]; 
};
extern struct dbif_config_t dbif_config;



/**
 *@brief 创建一个 udp 的 socket, 并 bind 到给定的 addr 上, 其中 addr 支持的格式有:
 * [ipv6]:port
 * ipv6
 * [ipv6]
 * ipv4:port
 * ipv4
 *
 * 注意: 调用者要保证 addr 不为 NULL;
 */
int create_udp_socket(const char *addr);


void send_resp_to_client(evutil_socket_t fd, short what, void *arg);
void recv_req_from_client(evutil_socket_t fd, short what, void *arg);

#endif /* __DB_INTERFACE_H__ */
