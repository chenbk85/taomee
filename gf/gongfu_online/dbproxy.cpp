#include <cstring>
#include <arpa/inet.h>

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>

#include <async_serv/net_if.h>
}

#include "cli_proto.hpp"
#include "player.hpp"
#include "common_op.hpp"
#include "item.hpp"
#include "task.hpp"
#include "skill.hpp"
#include "login.hpp"
#include "dbproxy.hpp"
#include "restriction.hpp"
#include "warehouse.hpp"
#include "ambassador.hpp"
#include "vip_impl.hpp"
#include "apothecary.hpp"
#include "mail.hpp"
#include "store_sales.hpp"
#include "trade.hpp"
#include "coupons.hpp"
#include "kill_boss.hpp"
#include "achievement.hpp"
#include "title_system.hpp"
#include "ap_toplist.hpp"
#include "global_data.hpp"
#include "use_item_history.hpp"
#include "contest.hpp"
#include "home_impl.hpp"
#include "card.hpp"
#include "fight_team.hpp"
#include "store_sales.hpp"
#include "rank_top.hpp"
#include "other_active.hpp"
/**
  * @brief data type for handlers that handle protocol packages from dbproxy
  */
//typedef int (*dbproto_hdlr_t)(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief data type for handles that handle protocol packages from dbproxy
  */
/*struct dbproto_handle_t {
	dbproto_handle_t(dbproto_hdlr_t h = 0, uint32_t l = 0, uint8_t cmp = 0)
		{ hdlr = h; len = l; cmp_method = cmp; }

	dbproto_hdlr_t		hdlr;
	uint32_t			len;
	uint8_t				cmp_method;
};*/

/*! save handles for handling protocol packages from dbproxy */
//static dbproto_handle_t db_handles[65536];

/*! udp socket address for db server */
//static struct sockaddr_in dbsvr_udp_sa[3];

//static struct sockaddr_in dbsvr_onoff_udp_sa;


/*! dbproxy socket fd */
//int proxysvr_fd = -1;

/*! dbsvr udp socket fd */
//int dbsvr_udp_fd[2] = {-1, -1};

/*! dbsvr udp socket fd */
//int dbsvr_onoff_udp_fd = -1;


/*! for packing protocol data and send to dbproxy */
//uint8_t dbpkgbuf[dbproto_max_len];
//-----------------------------------------------------------------

/**
  * @brief set dbproxy protocol handle
  */
#define SET_DB_HANDLE(op_, len_, cmp_) \
		do { \
			if (db_handles[dbproto_ ## op_].hdlr != 0) { \
				ERROR_LOG("duplicate cmd=%u name=%s", dbproto_ ## op_, #op_); \
				return false; \
			} \
			db_handles[dbproto_ ## op_] = dbproto_handle_t(db_ ## op_ ## _callback, len_, cmp_); \
		} while (0)

//-----------------------------------------------------------------
/**
 * @brief initlise udp socket
 * @param global usage (be careful)
 */
static int init_udp_socket(uint8_t order);

/**
  * @brief send a request to db proxy
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_request_to_db(player_t* p, userid_t id, uint32_t role_tm, uint16_t cmd, const void* body_buf, uint32_t body_len)
{
	static uint8_t dbbuf[dbproto_max_len];

	if (proxysvr_fd == -1) {
		proxysvr_fd = connect_to_service(config_get_strval("dbproxy_name"), 0, 65535, 1);
	}

	if ((proxysvr_fd == -1) || 	(body_len > (sizeof(dbbuf) - sizeof(db_proto_t)))) {
		ERROR_LOG("send to dbproxy failed: fd=%d len=%d", proxysvr_fd, body_len);
		if (p) {
			if (p->waitcmd == cli_proto_login) {
				return -1;
			}
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}
		return 0;
	}

	db_proto_t* pkg = reinterpret_cast<db_proto_t*>(dbbuf);
	pkg->len = sizeof(db_proto_t) + body_len ;
	pkg->seq = (p ? ((p->fd << 16) | p->waitcmd) : 0);
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	pkg->role_tm  = role_tm;
	TRACE_LOG("cmd=%X id=%u role_tm=%u",pkg->cmd, pkg->id, pkg->role_tm);
	memcpy(pkg->body, body_buf, body_len);

	return net_send(proxysvr_fd, dbbuf, pkg->len);
}

/**
  * @brief send a request to db proxy
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_request_to_db_no_roletm(player_t* p, userid_t id, uint16_t cmd, const void* body_buf, uint32_t body_len)
{
	static uint8_t dbbuf[dbproto_max_len];

	if (proxysvr_fd == -1) {
		proxysvr_fd = connect_to_service(config_get_strval("dbproxy_name"), 0, 65535, 1);
	}

	if ((proxysvr_fd == -1) || 	(body_len > (sizeof(dbbuf) - sizeof(db_proto_t)))) {
		ERROR_LOG("send to dbproxy failed: fd=%d len=%d", proxysvr_fd, body_len);
		if (p) {
			if (p->waitcmd == cli_proto_login) {
				return -1;
			}
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}
		return 0;
	}

	db_proto_t* pkg = reinterpret_cast<db_proto_t*>(dbbuf);
	pkg->len = sizeof(db_proto_t) + body_len - sizeof(uint32_t);
	pkg->seq = (p ? ((p->fd << 16) | p->waitcmd) : 0);
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	TRACE_LOG("send_requset_to_db:cmd=%x id=%u role_tm=%u",pkg->cmd, pkg->id, pkg->role_tm);
	memcpy(&(pkg->role_tm), body_buf, body_len);

	return net_send(proxysvr_fd, dbbuf, pkg->len);
}

int send_request_to_seerproxy(player_t* p, userid_t id, uint16_t cmd, const void* body, uint32_t body_len, uint8_t domain)
{
	static uint8_t seerbuf[vipproto_max_len];

	if (storesvr_fd == -1) {
		/* TODO: 支持 connect_to_service() */
		//vipsvr_id = connect_to_service(config_get_strval("service_vipsvr"), 0, 65535, 1);
		seer_fd[domain] = connect_to_svr(config_get_strval("seer_ip"), config_get_intval("seer_port", 0), 65535, 1);
	}

	if ((seer_fd[domain] == -1) || (body_len > (sizeof(seerbuf) - sizeof(store_proto_t)))) {
		ERROR_LOG("send to seersvr failed: fd=%d len=%d", seer_fd[domain], body_len);
		if (p) {
			if (p->waitcmd == cli_proto_login
					|| p->waitcmd == cli_proto_enter_own_home || p->waitcmd == cli_proto_enter_other_home ) {
				return -1;
			}
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}
		return 0;
	}

	store_proto_t* pkg = reinterpret_cast<store_proto_t*>(seerbuf);
	pkg->len = sizeof(store_proto_t) + body_len;
	pkg->seq = (p ? ((p->fd << 16) | p->waitcmd) : 0);
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	memcpy(pkg->body, body, body_len);
    KDEBUG_LOG(p->id, "SEND to SEER SERVER uid=[%u] pkglen=[%u]\t", id, pkg->len);
	return net_send(seer_fd[domain], seerbuf, pkg->len);
}

/**
  * @brief send a UDP request to db server
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_udp_request_to_db(const player_t* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len, uint8_t index)
{
	static uint8_t dbbuf[dbproto_max_len];

	if (body_len > (sizeof(dbbuf) - sizeof(db_proto_t))) {
		ERROR_LOG("send to udp db failed: fd=%d len=%d", dbsvr_udp_fd[index], body_len);
		return -1;
	}

	db_proto_t* pkg = reinterpret_cast<db_proto_t*>(dbbuf);
	pkg->len = sizeof(db_proto_t) + body_len - 4;
	pkg->seq = 0;
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	TRACE_LOG("SEND submit fd:%d, len:%u  body_len:%u", dbsvr_udp_fd[index], pkg->len, body_len);
	memcpy(&(pkg->role_tm), dbpkgbuf, body_len); // dbproxy proto head lenth diff

	if (sendto(dbsvr_udp_fd[index], dbbuf, pkg->len, 0, reinterpret_cast<const sockaddr*>(&(dbsvr_udp_sa[index])), sizeof(struct sockaddr_in)) < 0 ) {
		if (init_udp_socket(index) < 0) {
			ERROR_LOG("send submit failed and init udp socket to db server failed too");
			return -1;
		}
		if (sendto(dbsvr_udp_fd[index], dbbuf, pkg->len, 0, reinterpret_cast<const sockaddr*>(&(dbsvr_udp_sa[index])), sizeof(struct sockaddr_in)) < 0 ) {
			ERROR_LOG("send submit failed fd:%d, len:%u", dbsvr_udp_fd[index], pkg->len);
		}
	}
	return 0;
}

/**
  * @brief send a UDP request to db server
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_udp_onoff_to_db(const player_t* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len)
{
	static uint8_t dbbuf[dbproto_max_len];

	if (body_len > (sizeof(dbbuf) - sizeof(db_proto_t))) {
		ERROR_LOG("send to udp db failed: fd=%d len=%d", dbsvr_onoff_udp_fd, body_len);
		return -1;
	}

	db_proto_t* pkg = reinterpret_cast<db_proto_t*>(dbbuf);
	pkg->len = sizeof(db_proto_t) + body_len - 4;
	pkg->seq = 0;
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	TRACE_LOG("SEND onoff fd:%d, len:%u  body_len:%u", dbsvr_onoff_udp_fd, pkg->len, body_len);
	memcpy(&(pkg->role_tm), dbpkgbuf, body_len); // dbproxy proto head lenth diff

	if (sendto(dbsvr_onoff_udp_fd, dbbuf, pkg->len, 0, reinterpret_cast<const sockaddr*>(&dbsvr_onoff_udp_sa), sizeof(dbsvr_onoff_udp_sa)) < 0 ) {
		if (init_dbsvr_onoff_socket() < 0) {
			ERROR_LOG("send submit failed and init udp socket to db server failed too");
			return -1;
		}
		if (sendto(dbsvr_onoff_udp_fd, dbbuf, pkg->len, 0, reinterpret_cast<const sockaddr*>(&dbsvr_onoff_udp_sa), sizeof(dbsvr_onoff_udp_sa)) < 0 ) {
			TRACE_LOG("send submit failed fd:%d, len:%u", dbsvr_onoff_udp_fd, pkg->len);
		}
	} 
	return 0;
}

bool dbproto_need_waitcmd(uint32_t cmd)
{
	return  (cmd != dbproto_team_tax_coin && 
			cmd != dbproto_get_user_public_info &&
			cmd != dbproto_get_pvp_game_info &&
		    cmd != dbproto_get_player_stat_info);	
}

/**
  * @brief handle package return from dbproxy
  * @param dbpkg package from dbproxy
  * @param pkglen length of dbpkg
  */
void handle_db_return(db_proto_t* dbpkg, uint32_t pkglen)
{
	if (!dbpkg->seq) {
		if (dbpkg->cmd == dbproto_list_ranker) {
			proc_auto_list_rank_info_callback(dbpkg->body, pkglen - sizeof(db_proto_t), dbpkg->ret);
		}
		return;
	}


	uint32_t waitcmd = dbpkg->seq & 0xFFFF;
	int      connfd  = dbpkg->seq >> 16;

	player_t* p = get_player_by_fd(connfd);
	if (!p || ((p->waitcmd != waitcmd) && dbproto_need_waitcmd(dbpkg->cmd))) {
		ERROR_LOG("connection has been closed: dbpkg id=%u cmd=0x%X "
					"waitcmd=%u seq=%X ",	dbpkg->id, dbpkg->cmd,
					p ? p->waitcmd : 0, dbpkg->seq);
		return;
	}

	KDEBUG_LOG(p->id, "DB R\t[uid=%u %u cmd=0x%X %u ret=%u]",
				p->id, dbpkg->id, dbpkg->cmd, waitcmd, dbpkg->ret);

	int err = -1;
	uint32_t bodylen = dbpkg->len - sizeof(db_proto_t);

	if (dbpkg->cmd == dbproto_check_session) {
		bodylen = 0;
	}

	//db timeout
	//if (dbpkg->ret == 1017) {
	if (dbpkg->len == 18) {
		if (dbpkg->cmd != 2818) {
			//ERROR_LOG("dbpkg len error[%u %u %u %u %u]", dbpkg->len, dbpkg->seq, dbpkg->cmd, dbpkg->ret, dbpkg->id);
		}
		bodylen = 0;
	}

	// handle severe db error
	if ((p->waitcmd == cli_proto_login) && dbpkg->ret && dbpkg->cmd != dbproto_check_session) {
		ERROR_LOG("severe db error: uid=%u dbcmd=0x%X ret=%u", p->id, dbpkg->cmd, dbpkg->ret);
		goto ret;
	}
	
	if ( ((db_handles[dbpkg->cmd].cmp_method == cmp_must_eq) && (bodylen != db_handles[dbpkg->cmd].len))
			|| ((db_handles[dbpkg->cmd].cmp_method == cmp_must_ge) && (bodylen < db_handles[dbpkg->cmd].len)) ) {
		if ((bodylen != 0) || (dbpkg->ret == 0)) {
			ERROR_LOG("invalid package len=%u needlen=%u cmd=0x%X cmpmethod=%d ret=%u",
						bodylen, db_handles[dbpkg->cmd].len, dbpkg->cmd, 
						db_handles[dbpkg->cmd].cmp_method, dbpkg->ret);
			goto ret;
		}
	}

	err = db_handles[dbpkg->cmd].hdlr(p, dbpkg->id, dbpkg->body, bodylen, dbpkg->ret);

ret:
	if (err) {
		close_client_conn(p->fd);
	}
}

/**
 * @brief 
 */
#define USER_FEEDBACK_ORDER 0
#define USER_CHAT_ORDER     1
#define USER_SHOP_ITEM_LOG	2
static int init_udp_socket(uint8_t order)
{
    const char *port_name, *ip_name;
    if (order == USER_FEEDBACK_ORDER) {
        port_name = "dbsvr_udp_port";
        ip_name   = "dbsvr_udp_ip";
    } else if (order == USER_CHAT_ORDER) {
        port_name = "dbsvr_udp_chat_port";
        ip_name   = "dbsvr_udp_chat_ip";
    } else if (order == USER_SHOP_ITEM_LOG) {
        port_name = "shop_item_log_port";
        ip_name   = "shop_item_log_ip";
    }
    if (dbsvr_udp_fd[order] != -1) {
        close(dbsvr_udp_fd[order]);
    }
    bzero(&(dbsvr_udp_sa[order]), sizeof(dbsvr_udp_sa));
	dbsvr_udp_sa[order].sin_family = AF_INET;
	dbsvr_udp_sa[order].sin_port = htons(config_get_intval(port_name, 0));
	if (inet_pton(AF_INET, config_get_strval(ip_name), &(dbsvr_udp_sa[order].sin_addr)) <= 0)
		return -1;
	dbsvr_udp_fd[order] = socket(PF_INET, SOCK_DGRAM, 0);

	TRACE_LOG("init_dbsvr_udp_socket: fd[%d] addr[%s : %d]", dbsvr_udp_fd[order], 
		config_get_strval(ip_name), config_get_intval(port_name, 0));
    return dbsvr_udp_fd[order];
}
/**
  * @brief init UDP socket for db server
  */
int	init_dbsvr_udp_socket()
{
    //Here: for user_feedback "dbsvr_udp_port(_ip)"
    if (init_udp_socket(USER_FEEDBACK_ORDER) < 0) {
        ERROR_LOG("Failed to init udp socket to db server"); 
        return -1;
    }
    //Here: for pub info "dbsvr_udp_chat_port(_ip)"
    if (init_udp_socket(USER_CHAT_ORDER) < 0) {
        ERROR_LOG("Failed to init udp chat socket to db server"); 
        return -1;
    }
	//Here: for shop item info "dbsvr_udp_chat_port(_ip)"
    if (init_udp_socket(USER_SHOP_ITEM_LOG) < 0) {
        ERROR_LOG("Failed to init udp chat socket to db server"); 
        return -1;
    }
    return 1;
}

/**
  * @brief init UDP socket for db server
  */
int	init_dbsvr_onoff_socket()
{
	if (dbsvr_onoff_udp_fd != -1) {
		close(dbsvr_onoff_udp_fd);
	}
	bzero(&dbsvr_onoff_udp_sa, sizeof(dbsvr_onoff_udp_sa));
	dbsvr_onoff_udp_sa.sin_family = AF_INET;
	dbsvr_onoff_udp_sa.sin_port = htons(config_get_intval("dbsvr_onoff_udp_port", 0));
	if (inet_pton(AF_INET, config_get_strval("dbsvr_onoff_udp_ip"), &(dbsvr_onoff_udp_sa.sin_addr)) <= 0)
		return -1;
	dbsvr_onoff_udp_fd = socket(PF_INET, SOCK_DGRAM, 0);

	TRACE_LOG("init_dbsvr_onoff_socket: fd[%d] addr[%s : %d]", dbsvr_onoff_udp_fd, 
		config_get_strval("dbsvr_onoff_udp_ip"), config_get_intval("dbsvr_onoff_udp_port", 0));
	return dbsvr_onoff_udp_fd;
}


/**
  * @brief init handles to handle protocol packages from dbproxy
  * @return true if all handles are inited successfully, false otherwise
  */
bool init_db_proto_handles()
{
	memset(db_handles, 0, sizeof(db_handles));
	/* user info */
	SET_DB_HANDLE(verify_pay_passwd, 0, cmp_must_eq);
	SET_DB_HANDLE(is_set_pay_passwd, sizeof(is_set_pay_passwd_rsp_t) - 4, cmp_must_eq);
	/* operation, bodylen, compare method */
	SET_DB_HANDLE(get_player, sizeof(get_player_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(change_usr_nickname, 4, cmp_must_ge);
	SET_DB_HANDLE(get_usr_simple_info, sizeof(get_user_simple_info_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(get_usr_detail_info, sizeof(get_user_detail_info_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(get_hunter_top_info, sizeof(hunter_top_header_t), cmp_must_ge);
	SET_DB_HANDLE(get_hunter_self_info, sizeof(db_hunter_info_t), cmp_must_eq);
	SET_DB_HANDLE(set_user_flag, 0, cmp_must_eq);
	SET_DB_HANDLE(set_amb_task_status, 1, cmp_must_eq);
	/* for friends list */
	SET_DB_HANDLE(add_friend, 4, cmp_must_eq);
	SET_DB_HANDLE(del_friend, 0, cmp_must_eq);
	SET_DB_HANDLE(get_friend_list, 4, cmp_must_ge);

	SET_DB_HANDLE(get_user_public_info, sizeof(get_user_public_info_rsp_t), cmp_must_ge);

	/* for blacklist operations */
	SET_DB_HANDLE(add_blacklist, sizeof(userid_t), cmp_must_eq); //user id 
	SET_DB_HANDLE(del_blacklist, 0, cmp_must_eq);
	SET_DB_HANDLE(get_blacklist, sizeof(uid_list_t), cmp_must_ge);
	
	// for player's task
	SET_DB_HANDLE(get_done_task_list, 4, cmp_must_ge);
	SET_DB_HANDLE(get_going_task_buf, 4, cmp_must_ge);
	SET_DB_HANDLE(get_all_task_list, 8, cmp_must_ge);
	SET_DB_HANDLE(set_task_flag, 4, cmp_must_eq);
	SET_DB_HANDLE(set_task_buf, 4, cmp_must_eq);
	SET_DB_HANDLE(swap_item, sizeof(swap_item_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(get_fumo_info, sizeof(get_killed_boss_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(get_double_exp_data, sizeof(get_double_exp_data), cmp_must_ge);
	SET_DB_HANDLE(set_double_exp_data, sizeof(get_double_exp_data), cmp_must_ge);
	SET_DB_HANDLE(get_max_times_chapter, 4, cmp_must_ge);

	SET_DB_HANDLE(get_clothes_list, sizeof(get_clothes_list_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(get_item_list, sizeof(get_item_list_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(get_packs, sizeof(get_packs_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(buy_item, sizeof(buy_item_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(sell_item, sizeof(sell_item_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(batch_sell_item, sizeof(batch_sell_item_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(buy_clothes, sizeof(buy_clothes_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(sell_clothes, sizeof(sell_clothes_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(repair_all_clothes, sizeof(repair_all_clothes_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(wear_clothes, sizeof(wear_clothes_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(decompose_attire, sizeof(decompose_attire_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(compose_attire, sizeof(compose_attire_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(user_store_item, sizeof(store_item_list_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(query_gcoin_balance, 4, cmp_must_ge);
	SET_DB_HANDLE(coupons_exchange, sizeof(db_coupons_exchange_t), cmp_must_ge);
	SET_DB_HANDLE(get_strengthen_material_list, sizeof(get_item_list_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(strengthen_attire, sizeof(db_strengthen_attire_rsp_t), cmp_must_eq);
	
	SET_DB_HANDLE(set_itembind, sizeof(set_itembind_rsp_t), cmp_must_eq);
	
	SET_DB_HANDLE(discard_item, sizeof(db_item_elem_t), cmp_must_eq);
	SET_DB_HANDLE(discard_clothes, sizeof(db_clothes_elem_t), cmp_must_eq);
	
	SET_DB_HANDLE(use_skill_book, sizeof(use_skill_book_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(use_item, sizeof(use_item_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(skill_bind, sizeof(skill_bind_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(reset_skill, sizeof(reset_skill_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(learn_new_skill, sizeof(learn_new_skill_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(add_buff, sizeof(add_buf_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(get_client_buf, max_client_buf_len, cmp_must_eq);
	
	SET_DB_HANDLE(get_skills, sizeof(get_skills_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(skill_upgrade, sizeof(skill_upgrade_rsp_t), cmp_must_eq);

	SET_DB_HANDLE(add_action_count, sizeof(db_add_action_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(query_restriction_list, sizeof(query_res_list_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(get_warehouse_item_list, sizeof(get_warehouse_item_list_rsp_t),   cmp_must_ge);
	SET_DB_HANDLE(move_item_warehouse2bag, sizeof(move_item_rsp_t),  cmp_must_eq);
	SET_DB_HANDLE(move_item_bag2warehouse, sizeof(move_item_rsp_t),  cmp_must_eq);

	SET_DB_HANDLE(get_warehouse_clothes_item_list,  sizeof(get_warehouse_clothes_item_list_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(move_clothes_item_warehouse2bag,  sizeof(move_clothes_item_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(move_clothes_item_bag2warehouse,  sizeof(move_clothes_item_rsp_t), cmp_must_ge);

	SET_DB_HANDLE(learn_secondary_pro, sizeof(learn_secondary_pro_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(secondary_pro_fuse,  sizeof(secondary_pro_fuse_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(get_secondary_pro_list, sizeof(get_secondary_pro_list_rsp_t), cmp_must_ge);

	SET_DB_HANDLE(mail_head_list,  sizeof(mail_head_list_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(mail_body,  sizeof(mail_body_rsp_t),  cmp_must_ge);
	SET_DB_HANDLE(delete_mail, 4,  cmp_must_eq);
	SET_DB_HANDLE(take_mail_enclosure, 20, cmp_must_ge);
	SET_DB_HANDLE(send_mail,  sizeof(send_mail_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(send_system_mail, sizeof(send_mail_rsp_t), cmp_must_eq);

	SET_DB_HANDLE(set_extern_grid_count,  0, cmp_must_eq);
#ifdef DEV_SUMMON	
	SET_DB_HANDLE(get_summon_list, sizeof(db_get_summon_list_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(hatch_summon_mon, sizeof(db_hatch_summon_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(set_summon_nick, sizeof(db_set_summon_nick_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(feed_summon_mon, sizeof(db_feed_summon_mon_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(set_fight_summon, sizeof(db_set_fight_summon_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(evolve_summon, sizeof(db_evolve_summon_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(re_evolve_summon, sizeof(db_re_evolve_summon_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(allocate_exp_to_summon, sizeof(db_allocate_exp_t), cmp_must_eq);
	SET_DB_HANDLE(fresh_summon_attr, sizeof(db_fresh_attr_rsp_t), cmp_must_eq);
	//SET_DB_HANDLE(add_allocator_exp, 4, cmp_must_eq);
    SET_DB_HANDLE(use_sum_skills_scroll, sizeof(db_use_skills_scroll_t), cmp_must_eq);
    SET_DB_HANDLE(renew_summon_skills, sizeof(db_renew_summon_skills_t), cmp_must_eq);
#endif
    /* for dog */
    SET_DB_HANDLE(get_numen_list, sizeof(db_get_numen_list_rsp_t), cmp_must_ge);
    SET_DB_HANDLE(invite_numen, sizeof(db_invite_numen_rsp_t), cmp_must_eq);
    SET_DB_HANDLE(make_numen_sex, sizeof(db_make_numen_sex_rsp_t), cmp_must_eq);
    SET_DB_HANDLE(change_numen_nick, sizeof(db_change_numen_nick_rsp_t), cmp_must_eq);

	SET_DB_HANDLE(add_session, 12, cmp_must_eq);
	SET_DB_HANDLE(check_session, 0, cmp_must_eq);
	SET_DB_HANDLE(get_player_hero_top, 4, cmp_must_eq);
	SET_DB_HANDLE(set_player_hero_top, 0, cmp_must_eq);

	SET_DB_HANDLE(get_sell_log, 4, cmp_must_ge);
	SET_DB_HANDLE(get_coins, sizeof(get_coins_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(get_donate_count, sizeof(donate_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(add_item, 16, cmp_must_eq);

	SET_DB_HANDLE(get_buy_item_limit_list,  sizeof(get_buy_item_limit_list_rsp_t), cmp_must_ge);
	
	SET_DB_HANDLE(get_kill_boss_list, sizeof(get_kill_boss_list_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(get_use_item_list,  sizeof(get_use_item_list_rsp_t), cmp_must_ge);

	SET_DB_HANDLE(get_home_data, sizeof(get_home_data_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(set_home_data, sizeof(set_home_back_rsp_t), cmp_must_eq);

	SET_DB_HANDLE(get_achievement_data_list, sizeof(get_achievement_data_list_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(get_achieve_title, 4, cmp_must_ge);
	//SET_DB_HANDLE(notify_achieve_title, 5, cmp_must_eq);
	SET_DB_HANDLE(set_player_achieve_title, 4, cmp_must_eq);
	SET_DB_HANDLE(get_ap_toplist, sizeof(get_ap_toplist_rsp_t),  cmp_must_ge);
	SET_DB_HANDLE(query_forbiden_friend_flag, sizeof(query_forbiden_friend_flag_rsp_t), cmp_must_eq);

	SET_DB_HANDLE(get_ring_task_list, sizeof(get_ring_task_list_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(get_ring_task_history_list, sizeof(get_ring_task_history_list_rsp_t),  cmp_must_ge);

	SET_DB_HANDLE(get_home_log, sizeof(get_home_log_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(get_summon_dragon_list, sizeof(summon_dragon_list_rsp), cmp_must_ge);

	//for contest 天下第一比武大会
	SET_DB_HANDLE(join_contest_team, 4, cmp_must_eq);
	SET_DB_HANDLE(get_contest_team_info, 4, cmp_must_ge);
	SET_DB_HANDLE(get_contest_donate, 12, cmp_must_ge);
	SET_DB_HANDLE(contest_donate_plant, 4, cmp_must_eq);
	SET_DB_HANDLE(donate_item, 4, cmp_must_ge);
	SET_DB_HANDLE(get_self_contest_info, 12, cmp_must_eq);
	//SET_DB_HANDLE(get_taotai_game_info, 8, cmp_must_eq);
	SET_DB_HANDLE(upgrade_item, 12, cmp_must_ge);
	SET_DB_HANDLE(swap_action, 16, cmp_must_ge);
	SET_DB_HANDLE(get_swap_action, 4, cmp_must_ge);
	//SET_DB_HANDLE(get_advance_game_info, 8, cmp_must_eq);
	SET_DB_HANDLE(get_pvp_game_info, 4, cmp_must_ge);
	SET_DB_HANDLE(get_final_game_rank_list, 4, cmp_must_ge);
	SET_DB_HANDLE(team_member_reward, 4, cmp_must_ge);

	//for card
	SET_DB_HANDLE(get_card_list, 4, cmp_must_ge);
	SET_DB_HANDLE(insert_card, sizeof(db_insert_card_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(erase_card, sizeof(db_erase_card_rsp_t), cmp_must_eq);


	//for offline mode
	SET_DB_HANDLE(get_offline_info, sizeof(db_offline_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(exchange_coins_exploit, sizeof(exchange_coins_exploit_t), cmp_must_eq);
	SET_DB_HANDLE(create_fight_team, sizeof(create_fight_team_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(set_player_team_flg, sizeof(set_player_team_flg_rsp_t), cmp_must_eq);
	
	SET_DB_HANDLE(add_fight_team_member, sizeof(add_fight_team_member_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(get_team_info, sizeof(get_team_info_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(del_team_member, sizeof(del_team_member_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(del_team, 0, cmp_must_eq);
	SET_DB_HANDLE(get_team_top10, 4, cmp_must_ge);
	SET_DB_HANDLE(contribute_team, sizeof(db_contribute_team_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(fetch_team_coin, sizeof(db_fetch_team_coin_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(team_active_enter, sizeof(team_active_enter_t), cmp_must_eq);
	SET_DB_HANDLE(get_team_active_top100, 12, cmp_must_ge);
	SET_DB_HANDLE(search_team_info, 4, cmp_must_ge);
//	SET_DB_HANDLE(gain_team_exp, 4, cmp_must_eq);  
	SET_DB_HANDLE(team_tax_coin, 12, cmp_must_eq);

//	SET_DB_HANDLE(set_offline_info, sizeof(db_offline_rsp_t), cmp_must_eq);
//	SET_DB_HANDLE(list_wuseng_info, 4, cmp_must_ge);
	SET_DB_HANDLE(list_ranker, 4, cmp_must_ge);

	SET_DB_HANDLE(get_other_info, 4, cmp_must_ge);
	SET_DB_HANDLE(get_player_community, sizeof(get_player_community_info_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(set_power_user_info, sizeof(db_power_user_info_t), cmp_must_ge);

    //师徒
    SET_DB_HANDLE(master_add_prentice, sizeof(db_master_add_prentice_t), cmp_must_eq);
    SET_DB_HANDLE(get_master_info, 4, cmp_must_ge);
    SET_DB_HANDLE(get_prentice_info, 4, cmp_must_ge);

    SET_DB_HANDLE(get_reward_player_list, 8, cmp_must_ge);
    SET_DB_HANDLE(check_update_trade_info, 0, cmp_must_eq);

	SET_DB_HANDLE(get_other_active_info, sizeof(other_active_store_rsp_t), cmp_must_eq);

	SET_DB_HANDLE(list_simple_role_info, 4, cmp_must_ge);
	SET_DB_HANDLE(get_other_active_info_list, sizeof(other_active_info_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(get_single_other_active, sizeof(other_active_t), cmp_must_ge);

	SET_DB_HANDLE(set_player_fate, sizeof(db_set_player_fate_t), cmp_must_eq);

	SET_DB_HANDLE(distract_clothes_strength,  sizeof(db_distract_clothes_strength_rsp_t), cmp_must_eq);

	SET_DB_HANDLE(get_player_stat_info, 4, cmp_must_ge);
	return true;
}

