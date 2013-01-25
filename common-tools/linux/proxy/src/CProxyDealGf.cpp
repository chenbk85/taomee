#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee/project/utilities.h>
#include <stdio.h>

#include "CProxyDealGf.hpp"
#include "CProxyClientManage.hpp"
#include "CProxyRoute.hpp"
#include "utils.hpp"

using namespace taomee;

int
CProxyDealGf::customer_service_fixed_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	int idx = sizeof(gf_db_proto_t);
	pack(pkgbuf, body, bodylen, idx);

	init_db_proto_head(pkgbuf, info, 0, info->waitcmd, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealGf::get_friends_list_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	int idx = sizeof(gf_db_proto_t);
	pack_h(pkgbuf, 1, idx);

	init_db_proto_head(pkgbuf, info, 0, dbproto_get_friends_list, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealGf::get_user_info_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	int idx = sizeof(gf_db_proto_t);

	init_db_proto_head(pkgbuf, info, 0, dbproto_get_user_info, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealGf::send_mail_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	send_mail_t* p_mail_info = reinterpret_cast<send_mail_t*>(body);
	
	CHECK_VAL_EQ(bodylen, static_cast<int>(sizeof(send_mail_t) + p_mail_info->enclosure_cnt * sizeof(item_info_t)));

	gf_db_send_mail_t* mail_pkg = reinterpret_cast<gf_db_send_mail_t*>(pkgbuf + sizeof(gf_db_proto_t));
	mail_pkg->send_id = p_mail_info->send_id;
	mail_pkg->recv_id = info->userid;
	mail_pkg->mail_templet = p_mail_info->mail_type;
	mail_pkg->max_mail_limit = 100;
	memset(mail_pkg->mail_title, 0x00, sizeof(mail_pkg->mail_title));
	memset(mail_pkg->mail_body, 0x00, sizeof(mail_pkg->mail_body));
	memset(mail_pkg->mail_num_enclosure, 0x00, sizeof(mail_pkg->mail_num_enclosure));
	memset(mail_pkg->mail_item_enclosure, 0x00, sizeof(mail_pkg->mail_item_enclosure));
	memset(mail_pkg->mail_equip_enclosure, 0x00, sizeof(mail_pkg->mail_equip_enclosure));

	if (p_mail_info->title_len > sizeof(mail_pkg->mail_title) || p_mail_info->msg_len > sizeof(mail_pkg->mail_body)) {
		ERROR_LOG("gf email msg len error[%u %u %u]", info->userid, p_mail_info->title_len, p_mail_info->msg_len);
		return send_header_to_client(info, info->waitcmd, cli_err_email_len_err);
	}
	memcpy(mail_pkg->mail_title, p_mail_info->title, p_mail_info->title_len);
	memcpy(mail_pkg->mail_body, p_mail_info->msg, p_mail_info->msg_len);
	
	DEBUG_LOG("GF:SEND EMAIL\t[uid=%u mail=%u item_cnt=%u]", info->userid, mail_pkg->mail_templet, p_mail_info->enclosure_cnt);
	for (uint32_t i = 0; i < p_mail_info->enclosure_cnt; i++) {
		item_info_t* p_enclosure = &(p_mail_info->enclosures[i]);
		if (p_enclosure->item_type == t_item) {
			int len = strlen(mail_pkg->mail_item_enclosure);
			if (len != 0) {
				sprintf(mail_pkg->mail_item_enclosure + len, "\t");
			}
			sprintf(mail_pkg->mail_item_enclosure + strlen(mail_pkg->mail_item_enclosure), "%u %u", 
					p_enclosure->item_id, p_enclosure->item_count);	
			DEBUG_LOG("EMIAL ITEM\titem[id=%u cnt=%u]", p_enclosure->item_id, p_enclosure->item_count);
		} else if (p_enclosure->item_type == t_clothes) {
			int len = strlen(mail_pkg->mail_item_enclosure);
			if (len != 0) {
				sprintf(mail_pkg->mail_item_enclosure + len, "\t");
			}
			sprintf(mail_pkg->mail_equip_enclosure + strlen(mail_pkg->mail_equip_enclosure), "%u %u %u %u %u", 
					p_enclosure->item_id, 0, 0, 0, 0);	
			DEBUG_LOG("EMIAL ITEM\tclothes[id=%u]", p_enclosure->item_id);
		} else if (p_enclosure->item_type == t_attribute) {
			int len = strlen(mail_pkg->mail_num_enclosure);
			if (len != 0) {
				sprintf(mail_pkg->mail_num_enclosure + len, "\t");
			}
			sprintf(mail_pkg->mail_num_enclosure + strlen(mail_pkg->mail_num_enclosure), "%u %u", 
					p_enclosure->item_id, p_enclosure->item_count);	
			DEBUG_LOG("EMIAL ITEM\tattribute[id=%u]", p_enclosure->item_id);
		}
	}

	int idx = sizeof(gf_db_proto_t) + sizeof(gf_db_send_mail_t);
	init_db_proto_head(pkgbuf, info, 0, dbproto_send_mail, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealGf::noti_mail_sending_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	int idx = sizeof(gf_switch_proto_t);
	pack(pkgbuf, body,bodylen, idx);

	init_sw_proto_head(pkgbuf, info, 0, swproto_noti_mail_sending, idx);
	send_request_to_svr(info, pkgbuf, idx, 2);

	//g_clients.del_client(info->id);
	return 0;
	
}

int
CProxyDealGf::handle_db_return(void* data, int len)
{
	gf_db_proto_t* pkg = reinterpret_cast<gf_db_proto_t*>(data);
	//int connfd = pkg->seq >> 16;
	//int idx = pkg->seq & 0xFFFF;

	request_info_t* info = g_clients.get_request_info(pkg->seq);
	if (!info) {
		//ERROR_LOG("fd error[fd=%d]", connfd);
		ERROR_LOG("fd error[idx=%d]", pkg->seq);
		return -1;			    
	}
	/*if (info->id != idx) {
		ERROR_LOG("idx error[fd=%d idx=[%d %d] uid=%u]", connfd, info->id, idx, info->userid);
		return -1;			    
	}*/

	DEBUG_LOG("DB R\t[pkglen=%u idx=%u uid=%u cmd=%d ret=%d]", pkg->len, pkg->seq, pkg->uid, pkg->cmd, pkg->ret);

	int err = -1;
	uint32_t body_len = pkg->len - sizeof(gf_db_proto_t);

	//TODO: customer_service
	if (info->channel_id == customer_service_fixed) {
		err = dbproto_customer_service_fixed_callback(info, pkg->body, body_len, pkg->ret);	
	} else {
		switch (pkg->cmd) {
			case dbproto_get_friends_list:
				err = dbproto_get_friends_list_callback(info, pkg->body, body_len, pkg->ret);
				break;
			case dbproto_get_user_info:
				err = dbproto_get_user_info_callback(info, pkg->body, body_len, pkg->ret);
				break;
			case dbproto_send_mail:
				err = dbproto_send_mail_callback(info, pkg->body, body_len, pkg->ret);
				break;
			default:
				ERROR_LOG("cmd error[cmd=%d]", pkg->cmd);
		}
	}


	if (err != 0) {
		//close_client_conn(connfd);
		close_client_conn(info->fd);
	}

	return 0;
}

int 
CProxyDealGf::init_db_proto_head(void* header, request_info_t* info, uint32_t seq, uint16_t cmd, uint32_t len)
{
	gf_db_proto_t* proto = reinterpret_cast<gf_db_proto_t*>(header);

	proto->len = len;
	//proto->seq = (info->fd << 16) | (info->id) ;
	proto->seq = info->id ;
	proto->cmd = cmd;
	proto->ret = 0;
	proto->uid = info->userid;
	proto->role_tm = info->roletm;

	return 0;
}

int 
CProxyDealGf::init_sw_proto_head(void* header, request_info_t* info, uint32_t seq, uint16_t cmd, uint32_t len)
{
	gf_switch_proto_t* proto = reinterpret_cast<gf_switch_proto_t*>(header);

	proto->len = len;
	//proto->seq = (info->fd << 16) | (info->id) ;
	proto->seq = 0;
	proto->cmd = cmd;
	proto->ret = 0;
	proto->uid = info->userid;

	return 0;
}

int
CProxyDealGf::dbproto_customer_service_fixed_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);

	int idx = sizeof(cli_proto_t);
	pack(cli_pkgbuf, body, body_len, idx);

	init_cli_header(cli_pkgbuf, idx, info, info->waitcmd, ret);
	//g_clients.send_to_client(info->fd, cli_pkgbuf, idx);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}

int
CProxyDealGf::dbproto_get_friends_list_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);

	int idx = 0;
	int db_idx = 0;
	uint32_t friends_cnt = 0;
	unpack_h(body, friends_cnt, db_idx);

	CHECK_VAL_EQ(body_len, static_cast<int>(8 * friends_cnt + 4));

	DEBUG_LOG("GET FRIEND CNT\t[%u %u]", info->userid, friends_cnt);
	
	idx = sizeof(cli_proto_t);
	pack_h(cli_pkgbuf, friends_cnt, idx);
	for (uint32_t i = 0; i < friends_cnt; i++) {
		uint32_t friend_id = 0;
		uint32_t friend_type = 0;
		unpack_h(body, friend_id, db_idx);
		unpack_h(body, friend_type, db_idx);
		pack_h(cli_pkgbuf, friend_id, idx);
	}
	init_cli_header(cli_pkgbuf, idx, info, cli_get_friends_list, 0);
	//g_clients.send_to_client(info->fd, cli_pkgbuf, idx);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}

int
CProxyDealGf::dbproto_get_user_info_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);

	int idx = sizeof(cli_proto_t);
	pack(cli_pkgbuf, body, body_len, idx);
	init_cli_header(cli_pkgbuf, idx, info, cli_get_user_info, 0);
	//g_clients.send_to_client(info->fd, cli_pkgbuf, idx);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}

int
CProxyDealGf::dbproto_send_mail_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);
	
	int idx = sizeof(cli_proto_t);
	init_cli_header(cli_pkgbuf, idx, info, cli_send_mail, 0);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	//g_clients.send_to_client(info->id, cli_pkgbuf, idx, false);
	//noti_mail_sending_cmd(info, body, body_len);
	
	DEBUG_LOG("SEND MAIL RETURN");
	return 0;
}

