#include "utils.hpp"

#include "cli_proto.hpp"
#include "player.hpp"

#include "cli_proto.hpp"
#include "cachesvr.hpp"
#include "global_data.hpp"
#include "rank_top.hpp"
#include "common_op.hpp"

using namespace taomee;

uint8_t cache_buf[8192];

//int cachesvr_fd = -1;
void report_user_basic_info_to_cachesvr(player_t * p)
{
	int idx = 0;
	pack_h(cache_buf, p->role_type, idx);
	pack_h(cache_buf, p->lv, idx);
	pack(cache_buf, p->nick, max_nick_size, idx);
	send_request_to_cachesvr(p, p->id, p->role_tm, cache_basic_info_report, cache_buf, idx);
	TRACE_LOG("PLAYER %u %u REPROT BASIC INFO TO CACHESVR", p->id, p->role_tm);
}


/**
  * @brief send a request to cache server 
  * @param p the player who launches the request
  * @param uid id of the requested player
  * @param cmd command 
  * @param body body of the request
  * @param bodylen length of package body
  * @return 0 on success, -1 on error
  */
int send_request_to_cachesvr(player_t* p, userid_t uid, uint32_t role_tm, uint32_t cmd, const void* body, uint32_t bodylen)
{
	static uint8_t ch_pkg[8192];

	if (cachesvr_fd == -1) {
		cachesvr_fd = connect_to_svr(config_get_strval("cachesvr_ip"), config_get_intval("cachesvr_port", 0), 65535, 1);
	}

	if ((cachesvr_fd == -1) || (bodylen > (8192 - sizeof(cachesvr_proto_t)))) {
		ERROR_LOG("send package to cache server failed: fd=%d bodylen=%d", cachesvr_fd, bodylen);
		if (p) {
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}
		return 0;
	}

	uint32_t len = sizeof(cachesvr_proto_t) + bodylen;

	//Build the package
	cachesvr_proto_t* pkg = reinterpret_cast<cachesvr_proto_t*>(ch_pkg);
	pkg->len = len;
	pkg->seq = (p ? ((p->fd) << 16) | (p->waitcmd) : 0); // set higher 16 bytes fd and lower 16 bytes waitcmd
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->uid = uid;
	pkg->role_tm = role_tm;
	memcpy(pkg->body, body, bodylen);

	int ret = net_send(cachesvr_fd, ch_pkg, len);
	if (ret) {
		ERROR_LOG("SEND TO CACHESVR FAIL %d %d %d", ret, cachesvr_fd, len); 
		cachesvr_fd = -1;
	}
	return ret;
}

void handle_cachesvr_return(cachesvr_proto_t* data, int len)
{
	player_t * p = NULL;
	uint32_t waitcmd = 0;
	if (data->seq) {
		waitcmd = data->seq & 0xFFFF; // lower 16 bytes
		int	 connfd  = data->seq >> 16; //higher 16 bytes
		p = get_player_by_fd(connfd);
		if (!p || p->waitcmd != waitcmd) {
			ERROR_LOG("process connection to switch error\t[uid=%u cmd=%u seq=%X]",
				data->uid, waitcmd, data->seq);
			return;								
		}
	} else {
		p = get_player(data->uid);
		if (!p || p->role_tm != data->role_tm) {
			ERROR_LOG("process connection to switch error\t[uid=%u cmd=%u seq=%X]",
					data->uid, waitcmd, data->seq);
			return;							
		}
	}

	switch (data->cmd) {
		case cache_list_team_active_ranker:
			list_team_active_ranker_callback(p, data);
			break;
		case cache_list_single_active_ranker:
			list_single_active_ranker_callback(p, data);
			break;
		case cache_generate_magic_number:
			cache_generate_magic_number_callback(p, data);
			break;
		case cache_bind_magic_number:
			cache_bind_magic_number_callback(p, data);
			break;
		case cache_get_magic_number_creator:
			cache_get_magic_number_creator_callback(p, data);
			break;
		default:
			break;
	}
}




