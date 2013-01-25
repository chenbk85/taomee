/**
 *============================================================
 *  @file     CProxyRoute.hpp
 *  @brief    build connect with client and proxy request to some game...
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *============================================================
 */

#ifndef	CPROXYROUTE_HPP
#define	CPROXYROUTE_HPP

#include <libtaomee++/inet/pdumanip.hpp>
#include <map>

extern "C" {
#include <libtaomee/project/types.h>
}

/**
 * @brief game types
 */
enum games {
	game_mole 	= 1,
	game_seer	= 2,
	game_xhx	= 5,
	game_gf		= 6,
	game_hero	= 7,
	game_seer2	= 9,
	game_mdd	= 10,
	game_mdd_ren	= 11,
	game_mdd_sina	= 12,
	game_mdd_kaxin	= 13,
	max_game_flag = 20,
};

/**
 * @brief game zones
 */
enum zones {
	//zone_all = 0,
	zone_tel = 0,
	zone_cnc = 1,
	max_zone,
};

/**
 * @brief game svr types
 */
enum svr_types {
	svr_dbproxy = 1,
	svr_switch  = 2,
	max_svrs,
};

enum channel_ids {
	xiaoba		= 1,
	email_send	= 2,
	customer_service_fixed	= 3,
	project_2125	= 4,
	goods_trade		= 5,
	max_channel_id = 20,
};

/**
 * @brief client commands
 */
enum cli_cmds {
	cli_cmd_start = 0,

	cli_get_friends_list	 = 1001,
	cli_get_user_nick 		 = 1002,
	cli_get_user_info 		 = 1003,
	cli_get_user_cloth_info  = 1004,
	cli_send_mail		     = 1105,
	cli_add_items		     = 1106,

	cli_goods_add_items		 = 1201,
	cli_goods_check_items	 = 1202,

	cli_add_notify	 = 1301,

	cli_cmd_end,
};

enum cli_errors {
	cli_err_email_len_err	= 10001,

	cli_err_base_dberr = 100000,
};

#pragma pack(1)

/**
 * @brief client protocol type
 */
struct cli_proto_t {
	uint32_t len;
	uint16_t cmd;
	uint32_t uid;
	uint32_t seq;
	uint32_t ret;
	uint32_t role_tm;
	uint16_t game_flag;
	uint16_t game_zone;
	uint16_t svr_type;
	
	uint16_t channel_id;
	char	 verify_code[32];
	uint8_t	 body[];
};

#pragma pack()

class CProxyDealBase;
class CProxyDealGf;

/**
 * @brief game server ip、port、fd infomation
 */
struct svr_data_t {
	int fd;
	char ip[16];
	u_short port;

	void init(const char* p_ip, int n_port);
	void svr_connect();
};

struct svr_info_t {
	svr_info_t(uint32_t flag, uint32_t zone, uint32_t type):
		game_flag(flag),game_zone(zone),svr_type(type){}

	uint32_t game_flag;
	uint32_t game_zone;
	uint32_t svr_type;
};

typedef std::map<int, svr_info_t> SvrMap;

class CProxyRoute {
public:
	/**
	 * @brief default constructor
	 */
	CProxyRoute();
	/**
	 * @brief default destructor
	 */
	~CProxyRoute();

	/**
	 * @brief dispatches protocol packages from client
	 * @param data package from client
	 * @param fdsess fd session
	 * @return value that was returned by the protocol handling function
	 */
	int dispatch(void* data, fdsession_t* fdsess, int len);

	/**
	 * @brief dispatches protocol packages from server
	 * @param svr_fd the fd of game server
	 * @param data package from server
	 * @param len package len from server
	 * @return value that was returned by the protocol handling function
	 */
	int handle_svr_return(int svr_fd, void* data, int len);

	/**
	 * @brief reload sever information config
	 * @param fielname
	 * @return 0:success
	 */
	int reload_svr_config(char* filename);
	/**
	 * @brief reload security code config
	 * @param fielname
	 * @return 0:success
	 */
	int reload_security_config(char* filename);
	/**
	 * @brief init server information from reading config file
	 */
	int init_svr_info(char* game, char* zone, char* svr_type, char* ip, char* port);

	const char* req_verify_md5(const char* req_body, int req_body_len, uint16_t channel_id);
	int check_security_code(void* data);
	uint32_t get_security_code(uint16_t channel_id);
	
	/**
	 * @brief get server infomation by flag
	 */
	svr_data_t* get_svr(int flag, int zone, int type);

	void add_to_svrmap(int fd, uint32_t flag, uint32_t zone, uint32_t type);
	void del_to_svrmap(int fd);

	/**
	 * @brief fd of server closed
	 * @param fd
	 */
	void svr_fd_closed(int fd);
private:
	int cmd_dispatch(void* data, uint32_t idx);
	int customer_service_fixed_dispatch(void* data, uint32_t idx);

	/**
	 * @brief get server flag
	 */
	int get_game_flag(char* p_game);
	int get_game_zone(char* p_zone);
	int get_game_svr(char* p_svr);
	int get_game_flag_by_fd(int fd);

	int send_errormsg_to_client(int fd, uint32_t err, cli_proto_t* p_header);
private:
	/*! security codes */
	uint32_t security_codes[max_channel_id];
	/*! server information */
	svr_data_t m_svr[max_game_flag][max_zone][max_svrs];
	SvrMap svr_map_;

	/*! the point of Deal class */
	CProxyDealBase* m_deals[max_game_flag];
	/*! client package header */
	struct cli_proto_t header_;
};

extern CProxyRoute g_proxy_route;

#endif
