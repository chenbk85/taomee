#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee/project/utilities.h>
#include <stdio.h>

#include "CProxyDealHero.hpp"
#include "CProxyClientManage.hpp"
#include "CProxyRoute.hpp"
#include "utils.hpp"

using namespace taomee;

int
CProxyDealHero::customer_service_fixed_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	int idx = sizeof(hero_db_proto_t);
	pack(pkgbuf, body, bodylen, idx);

	init_db_proto_head(pkgbuf, info, 0, info->waitcmd, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealHero::get_friends_list_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	int idx = sizeof(hero_db_proto_t);
	pack_h(pkgbuf, 1, idx);

	init_db_proto_head(pkgbuf, info, 0, hero_dbproto_get_friends_list, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealHero::get_user_info_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	int idx = sizeof(hero_db_proto_t);

	init_db_proto_head(pkgbuf, info, 0, hero_dbproto_get_user_info, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealHero::send_mail_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	send_mail_t* p_mail_info = reinterpret_cast<send_mail_t*>(body);
	
	CHECK_VAL_EQ(bodylen, static_cast<int>(sizeof(send_mail_t) + p_mail_info->enclosure_cnt * sizeof(item_info_t)));

	hero_db_send_mail_t* mail_pkg = reinterpret_cast<hero_db_send_mail_t*>(pkgbuf + sizeof(hero_db_proto_t));
	mail_pkg->flag = 0;
	mail_pkg->type = p_mail_info->enclosure_cnt == 0 ? 2 : 3;
	mail_pkg->send_id = p_mail_info->send_id;
	mail_pkg->themeid = p_mail_info->mail_type;
	memset(mail_pkg->nick, 0x00, sizeof(mail_pkg->nick));
	memset(mail_pkg->mail_title, 0x00, sizeof(mail_pkg->mail_title));
	memset(mail_pkg->mail_body, 0x00, sizeof(mail_pkg->mail_body));
	memset(mail_pkg->mail_item, 0x00, sizeof(mail_pkg->mail_item));
	memset(mail_pkg->mail_ids, 0x00, sizeof(mail_pkg->mail_ids));

	if (mail_pkg->themeid ==10005) {
		mail_pkg->type = 3;
	}

	if (p_mail_info->title_len > sizeof(p_mail_info->title) || p_mail_info->msg_len > sizeof(p_mail_info->msg)) {
		ERROR_LOG("gf email msg len error[%u %u %u]", info->userid, p_mail_info->title_len, p_mail_info->msg_len);
		return send_header_to_client(info, info->waitcmd, cli_err_email_len_err);
	}

	memcpy(mail_pkg->nick, p_mail_info->nick, NICK_LEN);
	memcpy(mail_pkg->mail_title, p_mail_info->title, p_mail_info->title_len);
	memcpy(mail_pkg->mail_body, p_mail_info->msg, p_mail_info->msg_len);

	int tmpidx = 0;
	pack_h(mail_pkg->mail_item, p_mail_info->enclosure_cnt, tmpidx);
	DEBUG_LOG("HERO:SEND EMAIL\t[uid=%u mail=%u item_cnt=%u]", info->userid, p_mail_info->mail_type, p_mail_info->enclosure_cnt);
	for (uint32_t i = 0; i < p_mail_info->enclosure_cnt; i++) {
		item_info_t* p_enclosure = &(p_mail_info->enclosures[i]);
		if (p_enclosure->item_type == t_item) {
			pack_h(mail_pkg->mail_item, p_enclosure->item_id, tmpidx);
			pack_h(mail_pkg->mail_item, static_cast<uint16_t>(p_enclosure->item_count), tmpidx);
			DEBUG_LOG("EMIAL ITEM\titem[id=%u cnt=%u]", p_enclosure->item_id, p_enclosure->item_count);
		} else if (p_enclosure->item_type == t_clothes) {
			ERROR_LOG("hero enclosure only support item[%u]!", p_enclosure->item_id);
			return -1;
		}
	}

	int idx = sizeof(hero_db_proto_t) + sizeof(hero_db_send_mail_t);
	init_db_proto_head(pkgbuf, info, 0, hero_dbproto_send_mail, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealHero::goods_add_items_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	goods_add_items_t* items_info = reinterpret_cast<goods_add_items_t*>(body);

	uint32_t pos_len = sizeof(goods_add_items_t) + sizeof(goods_add_items_info_t) * items_info->count;
	uint32_t ext_data_len = *(reinterpret_cast<uint32_t*>(body + pos_len));
	CHECK_VAL_EQ(bodylen, static_cast<int>(pos_len + 4 + ext_data_len));

	int idx = sizeof(hero_db_proto_t);
	if (items_info->item_type == 1) { /*add items*/
		pack_h(pkgbuf, items_info->count, idx);
		for (uint32_t i = 0; i < items_info->count; i++) {
			goods_add_items_info_t* info = &(items_info->goods_items[i]);
			pack_h(pkgbuf, info->item_id, idx);
			pack_h(pkgbuf, info->item_count, idx);
			DEBUG_LOG("HERO:GOODS ADD ITEMS\titem[id=%u cnt=%u]", info->item_id, info->item_count);
		}
		init_db_proto_head(pkgbuf, info, 0, hero_dbproto_goods_add_items, idx);
	} else if (items_info->item_type == 2) { /*add clothes*/
		char* ext_data = reinterpret_cast<char*>(body + pos_len + 4);
		pack(pkgbuf, ext_data, ext_data_len, idx);
		DEBUG_LOG("HERO:GOODS ADD CLOTHESS\text_data_len=%d", ext_data_len);
		init_db_proto_head(pkgbuf, info, 0, hero_dbproto_goods_add_clothes, idx);
	}	

	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealHero::goods_check_items_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	goods_check_items_t* items_info = reinterpret_cast<goods_check_items_t*>(body);

	CHECK_VAL_EQ(bodylen, static_cast<int>(sizeof(goods_check_items_t) + items_info->count * sizeof(goods_check_items_info_t)));

	if (items_info->items_type == 1) { /*items neednot check*/
		int cli_idx = sizeof(cli_proto_t);
		init_cli_header(cli_pkgbuf, cli_idx, info, cli_goods_check_items, 0);
		return g_clients.send_to_client(info->id, cli_pkgbuf, cli_idx);
	}

	int idx = sizeof(hero_db_proto_t);
	pack_h(pkgbuf, items_info->count, idx);
	for (uint32_t i = 0; i < items_info->count; i++) {
		goods_check_items_info_t* info = &(items_info->check_items[i]);
		pack_h(pkgbuf, info->item_id, idx);
		pack_h(pkgbuf, info->add_count, idx);
		pack_h(pkgbuf, info->max_limit, idx);
		DEBUG_LOG("HERO:GOODS CHECK ITEMS\titem[id=%u cnt=%u max=%u]", info->item_id, info->add_count, info->max_limit);
	}

	init_db_proto_head(pkgbuf, info, 0, hero_dbproto_goods_check_items, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealHero::handle_db_return(void* data, int len)
{
	hero_db_proto_t* pkg = reinterpret_cast<hero_db_proto_t*>(data);
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
	uint32_t body_len = pkg->len - sizeof(hero_db_proto_t);

	//TODO: customer_service
	if (info->channel_id == customer_service_fixed) {
		err = dbproto_customer_service_fixed_callback(info, pkg->body, body_len, pkg->ret);	
	} else {
		switch (pkg->cmd) {
			case hero_dbproto_get_friends_list:
				err = dbproto_get_friends_list_callback(info, pkg->body, body_len, pkg->ret);
				break;
			case hero_dbproto_send_mail:
				err = dbproto_send_mail_callback(info, pkg->body, body_len, pkg->ret);
				break;
			case hero_dbproto_goods_add_items:
			case hero_dbproto_goods_add_clothes:
				err = dbproto_goods_add_items_callback(info, pkg->body, body_len, pkg->ret);
				break;
			case hero_dbproto_goods_check_items:
				err = dbproto_goods_check_items_callback(info, pkg->body, body_len, pkg->ret);
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
CProxyDealHero::init_db_proto_head(void* header, request_info_t* info, uint32_t seq, uint16_t cmd, uint32_t len)
{
	hero_db_proto_t* proto = reinterpret_cast<hero_db_proto_t*>(header);

	proto->len = len;
	//proto->seq = (info->fd << 16) | (info->id) ;
	proto->seq = info->id;
	proto->cmd = cmd;
	proto->ret = 0;
	proto->uid = info->userid;
	//proto->role_tm = info->roletm;

	return 0;
}

int
CProxyDealHero::dbproto_customer_service_fixed_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
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
CProxyDealHero::dbproto_get_friends_list_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);

	int idx = 0;
	int db_idx = 0;
	uint32_t friends_cnt = 0;
	unpack_h(body, friends_cnt, db_idx);

	CHECK_VAL_EQ((uint32_t)body_len, 8 * friends_cnt + 4);

	DEBUG_LOG("get friends cnt[%u %u]", info->userid, friends_cnt);
	
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
CProxyDealHero::dbproto_get_user_info_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
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
CProxyDealHero::dbproto_send_mail_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);
	
	int idx = sizeof(cli_proto_t);
	init_cli_header(cli_pkgbuf, idx, info, cli_send_mail, 0);
	//g_clients.send_to_client(info->fd, cli_pkgbuf, idx);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}

int
CProxyDealHero::dbproto_goods_add_items_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);
	
	int idx = sizeof(cli_proto_t);
	init_cli_header(cli_pkgbuf, idx, info, cli_goods_add_items, 0);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}

int
CProxyDealHero::dbproto_goods_check_items_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);
	
	int idx = sizeof(cli_proto_t);
	init_cli_header(cli_pkgbuf, idx, info, cli_goods_check_items, 0);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}

