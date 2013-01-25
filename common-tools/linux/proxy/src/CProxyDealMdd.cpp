#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee/project/utilities.h>

#include "CProxyDealMdd.hpp"
#include "CProxyClientManage.hpp"
#include "CProxyRoute.hpp"
#include "utils.hpp"

using namespace taomee;

int
CProxyDealMdd::add_items_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	add_items_t* p_item = reinterpret_cast<add_items_t*>(body);
	CHECK_VAL_EQ(bodylen, static_cast<int>(sizeof(add_items_t) + p_item->item_cnt * sizeof(add_item_info_t)));

	int idx = sizeof(moledd_db_proto_t);
	pack_h(pkgbuf, static_cast<uint16_t>(p_item->item_cnt), idx);
	DEBUG_LOG("MDD:ADD ITEMS\t[uid=%u item_cnt=%u]",info->userid, p_item->item_cnt);
	for (uint32_t i = 0; i < p_item->item_cnt; i++) {
		add_item_info_t* p_item_info = &(p_item->items[i]);
		pack_h(pkgbuf, static_cast<uint32_t>(p_item_info->item_id), idx);
		pack_h(pkgbuf, static_cast<uint16_t>(p_item_info->item_count), idx);
		pack_h(pkgbuf, static_cast<uint32_t>(0), idx);
		DEBUG_LOG("MDD:ITEMS\t[id=%u cnt=%u]", p_item_info->item_id, p_item_info->item_count);
	}
	
	init_db_proto_head(pkgbuf, info, 0, moledd_dbproto_add_items, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealMdd::goods_add_items_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	int idx = sizeof(moledd_db_proto_t);
	pack(pkgbuf, body, bodylen, idx);
	DEBUG_LOG("MDD:GOODS ADD ITEMS\t[bodylen=%d]", bodylen);
	init_db_proto_head(pkgbuf, info, 0, moledd_dbproto_goods_add_items, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealMdd::goods_check_items_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	int idx = sizeof(moledd_db_proto_t);
	pack(pkgbuf, body, bodylen, idx);
	DEBUG_LOG("MDD:GOODS CHECK ITEMS\t[bodylen=%d]", bodylen);
	init_db_proto_head(pkgbuf, info, 0, moledd_dbproto_goods_check_items, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealMdd::handle_db_return(void* data, int len)
{
	moledd_db_proto_t* pkg = reinterpret_cast<moledd_db_proto_t*>(data);

	/*pkg->len = bswap(pkg->len);
	pkg->seq = bswap(pkg->seq);
	pkg->uid = bswap(pkg->uid);
	pkg->cmd = bswap(pkg->cmd);
	pkg->ret = bswap(pkg->ret);*/

	request_info_t* info = g_clients.get_request_info(pkg->seq);
	if (!info) {
		ERROR_LOG("fd error[fd=%d]", pkg->seq);
		return -1;			    
	}
	/*if (info->id != idx) {
		ERROR_LOG("idx error[fd=%d idx=[%d %d] uid=%u]", connfd, info->id, idx, info->userid);
		return -1;			    
	}*/
	DEBUG_LOG("DB R\t[pkglen=%u idx=%u uid=%u cmd=%d ret=%d]", pkg->len, pkg->seq, pkg->uid, pkg->cmd, pkg->ret);

	int err = -1;
	uint32_t body_len = pkg->len - sizeof(moledd_db_proto_t);

	switch (pkg->cmd) {
		case moledd_dbproto_add_items:
			err = dbproto_add_items_callback(info, pkg->body, body_len, pkg->ret);
			break;
		case moledd_dbproto_goods_add_items:
			err = dbproto_goods_add_items_callback(info, pkg->body, body_len, pkg->ret);
			break;
		case moledd_dbproto_goods_check_items:
			err = dbproto_goods_check_items_callback(info, pkg->body, body_len, pkg->ret);
			break;
		default:
			ERROR_LOG("cmd error[cmd=%d]", pkg->cmd);
	}

	if (err != 0) {
		//close_client_conn(connfd);
		close_client_conn(info->fd);
	}

	DEBUG_LOG("DB RETURN\t[uid=%u cmd=%u ret=%d]", info->userid, pkg->cmd, pkg->ret);
	return 0;
}

int 
CProxyDealMdd::init_db_proto_head(void* header, request_info_t* info, uint32_t seq, uint16_t cmd, uint32_t len)
{
	moledd_db_proto_t* proto = reinterpret_cast<moledd_db_proto_t*>(header);

	/*proto->len = bswap(len);
	proto->seq = bswap(info->id);
	proto->cmd = bswap(cmd);
	proto->ret = 0;
	proto->uid = bswap(info->userid);*/

	proto->len = len;
	proto->seq = info->id;
	proto->cmd = cmd;
	proto->ret = 0;
	proto->uid = info->userid;

	return 0;
}

int
CProxyDealMdd::dbproto_add_items_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);

	int idx = sizeof(cli_proto_t);
	init_cli_header(cli_pkgbuf, idx, info, cli_add_items, ret);
	//g_clients.send_to_client(info->fd, cli_pkgbuf, idx);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}

int
CProxyDealMdd::dbproto_goods_add_items_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);
	
	int idx = sizeof(cli_proto_t);
	init_cli_header(cli_pkgbuf, idx, info, cli_goods_add_items, 0);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}

int
CProxyDealMdd::dbproto_goods_check_items_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);
	
	int idx = sizeof(cli_proto_t);
	init_cli_header(cli_pkgbuf, idx, info, cli_goods_check_items, 0);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}


