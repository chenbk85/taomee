#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee/project/utilities.h>

#include "CProxyDealSeer2.hpp"
#include "CProxyClientManage.hpp"
#include "CProxyRoute.hpp"
#include "utils.hpp"

using namespace taomee;

int
CProxyDealSeer2::add_items_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	add_items_t* p_item = reinterpret_cast<add_items_t*>(body);
	CHECK_VAL_EQ(bodylen, static_cast<int>(sizeof(add_items_t) + p_item->item_cnt * sizeof(add_item_info_t)));

	int idx = sizeof(seer2_db_proto_t);
	uint16_t operator_type = 28;
	pack_h(pkgbuf, operator_type, idx);
	pack_h(pkgbuf, p_item->item_cnt, idx);
	DEBUG_LOG("SEER2:ADD ITEMS\t[uid=%u item_cnt=%u]",info->userid, p_item->item_cnt);
	for (uint32_t i = 0; i < p_item->item_cnt; i++) {
		add_item_info_t* p_item_info = &(p_item->items[i]);
		pack_h(pkgbuf, static_cast<uint8_t>(0), idx);
		pack_h(pkgbuf, static_cast<uint8_t>(p_item_info->item_type), idx);
		pack_h(pkgbuf, p_item_info->item_id, idx);
		pack_h(pkgbuf, p_item_info->item_count, idx);
		DEBUG_LOG("SEER2:ITEMS\t[type=%u id=%u cnt=%u max=%u]",p_item_info->item_type, 
				p_item_info->item_id, p_item_info->item_count, p_item_info->max_count);
	}
	pack_h(pkgbuf, static_cast<uint32_t>(0), idx);
	pack_h(pkgbuf, static_cast<uint32_t>(0), idx);
	
	init_db_proto_head(pkgbuf, info, 0, seer2_dbproto_add_items, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealSeer2::add_notify_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	CHECK_VAL_EQ(bodylen, static_cast<int>(sizeof(add_notify_t)));
	add_notify_t* p_noti = reinterpret_cast<add_notify_t*>(body);

	int idx = sizeof(seer2_switch_proto_t);
	pack_h(pkgbuf, p_noti->receive_id, idx);
	pack_h(pkgbuf, p_noti->npc_id, idx);
	pack_h(pkgbuf, p_noti->msg_time, idx);
	pack_h(pkgbuf, p_noti->msg_len, idx);
	pack(pkgbuf, p_noti->msgs, p_noti->msg_len, idx);
	DEBUG_LOG("SEER2:ADD NOTIFY\t[uid=%u rece=%d msglen=%u]",info->userid, p_noti->receive_id, p_noti->msg_len);

	init_sw_proto_head(pkgbuf, info, 0, seer2_swproto_add_notify, idx);
	send_request_to_svr(info, pkgbuf, idx);
	
	//back to client
	idx = sizeof(cli_proto_t);
	init_cli_header(cli_pkgbuf, idx, info, cli_add_notify, 0);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}

int
CProxyDealSeer2::handle_db_return(void* data, int len)
{
	seer2_db_proto_t* pkg = reinterpret_cast<seer2_db_proto_t*>(data);
	//int connfd = pkg->seq >> 16;
	//int idx = pkg->seq & 0xFFFF;

	request_info_t* info = g_clients.get_request_info(pkg->seq);
	if (!info) {
		//ERROR_LOG("fd error[fd=%d]", connfd);
		ERROR_LOG("fd error[fd=%d]", pkg->seq);
		return -1;			    
	}
	/*if (info->id != idx) {
		ERROR_LOG("idx error[fd=%d idx=[%d %d] uid=%u]", connfd, info->id, idx, info->userid);
		return -1;			    
	}*/
	DEBUG_LOG("DB R\t[pkglen=%u idx=%u uid=%u cmd=%d ret=%d]", pkg->len, pkg->seq, pkg->uid, pkg->cmd, pkg->ret);

	int err = -1;
	uint32_t body_len = pkg->len - sizeof(seer2_db_proto_t);

	switch (pkg->cmd) {
		case seer2_dbproto_add_items:
			err = dbproto_add_items_callback(info, pkg->body, body_len, pkg->ret);
			break;
		default:
			ERROR_LOG("cmd error[cmd=%d]", pkg->cmd);
	}

	if (err != 0) {
		close_client_conn(info->fd);
	}

	return 0;
}

int
CProxyDealSeer2::handle_switch_return(void* data, int len)
{
	seer2_switch_proto_t* pkg = reinterpret_cast<seer2_switch_proto_t*>(data);

	request_info_t* info = g_clients.get_request_info(pkg->seq);
	if (!info) {
		ERROR_LOG("fd error[fd=%d]", pkg->seq);
		return -1;			    
	}

	DEBUG_LOG("SWITCH R\t[pkglen=%u idx=%u uid=%u cmd=%d ret=%d]", pkg->len, pkg->seq, pkg->uid, pkg->cmd, pkg->ret);

	int err = -1;
	uint32_t body_len = pkg->len - sizeof(seer2_db_proto_t);

	switch (pkg->cmd) {
		/*case seer2_swproto_add_notify:
			err = swproto_add_notify_callback(info, pkg->body, body_len, pkg->ret);
			break;*/
		default:
			ERROR_LOG("switch return cmd error[cmd=%d]", pkg->cmd);
	}

	if (err != 0) {
		close_client_conn(info->fd);
	}

	return 0;
}

int 
CProxyDealSeer2::init_db_proto_head(void* header, request_info_t* info, uint32_t seq, uint16_t cmd, uint32_t len)
{
	seer2_db_proto_t* proto = reinterpret_cast<seer2_db_proto_t*>(header);

	proto->len = len;
	//proto->seq = (info->fd << 16) | (info->id) ;
	proto->seq = info->id;
	proto->cmd = cmd;
	proto->ret = 0;
	proto->uid = info->userid;

	return 0;
}


int 
CProxyDealSeer2::init_sw_proto_head(void* header, request_info_t* info, uint32_t seq, uint16_t cmd, uint32_t len)
{
	seer2_switch_proto_t* proto = reinterpret_cast<seer2_switch_proto_t*>(header);

	proto->len = len;
	//proto->seq = (info->fd << 16) | (info->id) ;
	proto->seq = info->id;
	proto->cmd = cmd;
	proto->ret = 0;
	proto->uid = info->userid;

	return 0;
}

int
CProxyDealSeer2::dbproto_add_items_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);

	int idx = sizeof(cli_proto_t);
	init_cli_header(cli_pkgbuf, idx, info, cli_add_items, ret);
	//g_clients.send_to_client(info->fd, cli_pkgbuf, idx);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}

/*int
CProxyDealSeer2::swproto_add_notify_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	if (ret != 0) {
		return send_header_to_client(info, info->waitcmd, ret);		
	}
	
	int idx = sizeof(cli_proto_t);
	init_cli_header(cli_pkgbuf, idx, info, cli_add_notify, ret);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}*/


