#ifndef CPROXYCLIENTMANAGE_HPP
#define CPROXYCLIENTMANAGE_HPP

extern "C" {
#include <assert.h>
#include <glib.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>
#include <async_serv/net_if.h>
}
#include <map>
#include <set>

struct request_info_t {
	int id;
	int	fd;
	uint16_t waitcmd;
	uint32_t userid;
	uint32_t roletm;
	uint16_t game_flag;
	uint16_t game_zone;
	uint16_t svr_type;
	uint16_t channel_id;
};

struct client_info_t {
	struct request_info_t info;
	fdsession_t* fdsess;
};

typedef std::map<int, struct client_info_t> ClientMap;
typedef std::set<uint32_t> ClientfdSet;

class ClientManage {
public:
	ClientManage();
	~ClientManage();

	uint32_t add_client(fdsession_t* fdsess, uint32_t uid, uint32_t role_tm, uint32_t flag, 
			uint32_t zone, uint32_t type, uint32_t channel);
	//int del_client(int fd);
	int del_clients(int fd);
	int del_client(uint32_t idx, bool complete = true);

	//struct client_info_t* get_client_info(int fd);
	struct client_info_t* get_client_info(uint32_t idx);

	//struct request_info_t* get_request_info(int fd);
	struct request_info_t* get_request_info(uint32_t idx);

	//int send_to_client(int fd, uint8_t* pkg, uint32_t pkglen);
	int send_to_client(uint32_t idx, uint8_t* pkg, uint32_t pkglen, bool flag = true);
private:
	uint32_t cli_idx;
	ClientMap m_client_map;
	ClientfdSet m_client_fd_set[30000];
};


extern ClientManage g_clients;

#endif
