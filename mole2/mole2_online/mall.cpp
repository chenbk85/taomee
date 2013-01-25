#include <arpa/inet.h>
#ifdef __cplusplus
extern "C"
{
#endif
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/inet/tcp.h>
#include <async_serv/async_serv.h>

#include <libtaomee/dataformatter/bin_str.h>
#ifdef __cplusplus
}
#endif


#include "mall.h"
#include "cli_proto.h"
#include "cli_login.h"
#include "items.h"
#include "./proto/mole2_db.h"


product_t* get_product(uint32_t product_id)
{
	return (product_t *)g_hash_table_lookup(all_products, &product_id);
}

static gboolean free_product(gpointer key, gpointer pvi, gpointer userdata)
{
	g_slice_free1(sizeof(product_t),pvi);
	return TRUE;
}

void init_products()
{
	all_products = g_hash_table_new(g_int_hash, g_int_equal);
}

int load_products(xmlNodePtr cur)
{
	uint32_t ID = 0;
	uint32_t level = 0;
	uint32_t itemid = 0;
	uint32_t product_id = 0;

	product_t *product = NULL;

	g_hash_table_foreach_remove(all_products, free_product, 0);
	for (cur = cur->xmlChildrenNode; cur; cur = cur->next) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Mall")) {
			DECODE_XML_PROP_UINT32(ID, cur, "ID");

			//DEBUG_LOG("000000 %u",ID);
			if(ID <= 2000) continue;

			DECODE_XML_PROP_UINT32(itemid, cur, "ItemID");
			DECODE_XML_PROP_UINT32(product_id, cur, "MerchandiseID");
			decode_xml_prop_uint32_default(&level, cur, "Level",0);

			product = get_product(product_id);
			if (product) {
				ERROR_RETURN(("duplicated PRODUCT ID=%u",product_id), -1);				
			}

			if (get_item(itemid)) {
				product = (product_t*)g_slice_alloc0(sizeof(product_t));
				product->type = 1;
				product->level = level;
				product->itemid = itemid;
				product->id = product_id;
				g_hash_table_insert(all_products, &product->id, product);
				//DEBUG_LOG("1111111111 %u %u",itemid,product_id);
				TRACE_LOG("PRODUCT ITEM\t[%u]", product->itemid);
			} else if(get_cloth(itemid)) {
				product = (product_t*)g_slice_alloc0(sizeof(product_t));
				product->type = 2;
				product->level = level;
				product->itemid = itemid;
				product->id = product_id;
				decode_xml_prop_uint32_default(&product->validday, cur, "ValidDay",0);
				g_hash_table_insert(all_products, &product->id, product);
				TRACE_LOG("PRODUCT CLOTH\t[%u]", product->itemid);
			} else {
				ERROR_RETURN(("can't find PRODUCT ID=%u %u",product_id, itemid), -1);
			}
		}
	}
	return 0;
}

typedef struct {
	userid_t userid;
	uint32_t product_id;
	uint16_t product_count;
	uint8_t  is_vip;
	uint8_t  buy_place;
	uint8_t  pay_passwd[16];
	uint32_t log_type;
	uint8_t  log_info[16];
	uint32_t ext_data_len;
	uint8_t  ext_data[];
}__attribute__((packed)) buy_req_t;

typedef struct {
	uint32_t opid;
	uint32_t m1_price;
	uint32_t m1_balance;
	uint32_t m2_add;
	uint32_t m2_balance;
	uint32_t data_len;
	uint8_t  data[];
}__attribute__((packed)) buy_ret_t;

typedef struct {
	uint32_t product_id;
	uint32_t price;
	uint32_t vip_price;
	uint32_t novip_price;
	uint8_t  type;
	uint8_t  category;
	uint32_t gift_money_num;
	uint8_t  must_vip;
	uint32_t max_limit;
	uint32_t total_count;
	uint32_t current_count;
	uint8_t  is_valid;
	uint8_t  flag;
}__attribute__((packed)) product_detail_t;

int mall_get_money_balance(sprite_t* p, Cmessage* c_in)
{
	return send_request_to_mall(p,proto_mall_get_money_balance,NULL,0);
}

int mall_get_money_balance_callback(sprite_t* p, uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_MALL_ERR(p, ret);
	uint32_t balance;
	
	int j = 0;
	UNPKG_H_UINT32(body, balance, j);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, balance, i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int mall_get_items_price(sprite_t* p, Cmessage* c_in)
{
	mall_get_items_price_in *p_in = P_IN;
	
	uint32_t i;
	int bytes = 0;
	char buff[4096] = {0};
	
	for (i = 0; i < p_in->items.size(); i++) {
		PKG_H_UINT32(buff, p_in->items[i], bytes);
		DEBUG_LOG("xxxx %u",p_in->items[i]);
	}

	return send_request_to_mall(p, proto_mall_get_items_price, buff, bytes);
}

int mall_get_items_price_callback(sprite_t* p, uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_MALL_ERR(p, ret);

	int j = 0;
	uint32_t count;
	UNPKG_H_UINT32(body, count, j);
	CHECK_BODY_LEN(bodylen, j + count * 18);

	uint32_t loop;
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, count, i);
	for (loop = 0; loop < count; loop++) {
		uint32_t product_id;
		uint16_t status;
		uint32_t price;
		uint32_t vip_price;
		uint32_t novip_price;

		UNPKG_H_UINT32(body, product_id, j);
		UNPKG_H_UINT16(body, status, j);
		UNPKG_H_UINT32(body, price, j);
		UNPKG_H_UINT32(body, vip_price, j);
		UNPKG_H_UINT32(body, novip_price, j);

		PKG_UINT32(msg, product_id, i);
		PKG_UINT16(msg, status, i);
		PKG_UINT32(msg, price, i);
		PKG_UINT32(msg, vip_price, i);
		PKG_UINT32(msg, novip_price, i);
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int mall_get_item_detail(sprite_t* p, Cmessage* c_in)
{
	mall_get_item_detail_in *p_in = P_IN;
	return send_request_to_mall(p, proto_mall_get_item_detail, &p_in->itemid, sizeof(p_in->itemid));
}

int mall_get_item_detail_callback(sprite_t* p, uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_MALL_ERR(p, ret);

	product_detail_t *itm = (product_detail_t*)body;

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, itm->product_id, i);
	PKG_UINT32(msg, itm->price, i);
	PKG_UINT32(msg, itm->vip_price, i);
	PKG_UINT32(msg, itm->novip_price, i);
	PKG_UINT32(msg, itm->type, i);
	PKG_UINT32(msg, itm->category, i);
	PKG_UINT32(msg, itm->gift_money_num, i);
	PKG_UINT32(msg, itm->must_vip, i);
	PKG_UINT32(msg, itm->max_limit, i);
	PKG_UINT32(msg, itm->total_count, i);
	PKG_UINT32(msg, itm->current_count, i);
	PKG_UINT32(msg, itm->is_valid, i);
	PKG_UINT32(msg, itm->flag, i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int mall_buy_item(sprite_t* p, Cmessage* c_in)
{
	mall_buy_item_in *p_in = P_IN;

	char buff[4096] = {0};
	product_t *product = NULL;
	buy_req_t *buy = (buy_req_t*)buff;

	buy->product_id = p_in->itemid;
	buy->product_count = p_in->count;
	str2hex(p_in->passwd,32,buy->pay_passwd);

	char str[64]={0};
	memcpy(str,p_in->passwd,32);
	KDEBUG_LOG(p->id,"mall_buy_item id=%u count=%u productid=%u",p_in->itemid,p_in->count,buy->product_id);

	product = get_product(buy->product_id);
	if(!product) {
		return send_to_self_error(p, p->waitcmd, cli_err_havnot_this_item, 1);
	}

	if(product->type == 1) {
		KDEBUG_LOG(p->id, "MALL ITEM\t[%u]", product->itemid);
		buy->ext_data_len = 0;
		PKG_H_UINT32(buy->ext_data,1,buy->ext_data_len);
		PKG_H_UINT32(buy->ext_data,product->itemid,buy->ext_data_len);
		PKG_H_UINT32(buy->ext_data,buy->product_count,buy->ext_data_len);
	} else if(product->type == 2) {
		KDEBUG_LOG(p->id, "MALL CLOTH\t[%u]", product->itemid);
		cloth_t* psc = get_cloth(product->itemid);
		if(!psc) {
			return send_to_self_error(p, p->waitcmd, cli_err_cloth_id_invalid, 1);
		}
		CHECK_CLOTH_LV_VALID(p, psc, product->itemid, product->level);
		cloth_attr_t pca = {};
		cloth_lvinfo_t* pclv = &psc->clothlvs[product->level];
		gen_cloth_attr(psc, product->level, pclv, &pca, product->validday);
		buy->ext_data_len = 0;
		buy->product_count = 1;

		if(get_bag_cloth_cnt(p) < get_max_grid(p))
			pca.grid = 9999;
		else
			pca.grid = 22222;

		PKG_H_UINT32(buy->ext_data,1,buy->ext_data_len);
		PKG_H_UINT32(buy->ext_data,0,buy->ext_data_len);//gettime
		PKG_H_UINT32(buy->ext_data,pca.clothtype,buy->ext_data_len);
		PKG_H_UINT32(buy->ext_data,pca.grid,buy->ext_data_len);
		PKG_H_UINT32(buy->ext_data,pca.hp_max,buy->ext_data_len);
		PKG_H_UINT32(buy->ext_data,pca.mp_max,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.cloth_level,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.duration,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.duration_max,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.attack,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.mattack,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.defense,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.mdefense,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.speed,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.spirit,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.resume,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.hit,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.dodge,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.crit,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.fightback,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.rpoison,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.rlithification,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.rlethargy,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.rinebriation,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.rconfusion,buy->ext_data_len);
		PKG_H_UINT16(buy->ext_data,pca.roblivion,buy->ext_data_len);
		PKG_H_UINT32(buy->ext_data,pca.quality,buy->ext_data_len);
		PKG_H_UINT32(buy->ext_data,pca.validday,buy->ext_data_len);
		PKG_H_UINT32(buy->ext_data,pca.crystal_attr,buy->ext_data_len);
		PKG_H_UINT32(buy->ext_data,pca.bless_type,buy->ext_data_len);
	}

	buy->userid = p->id;
	buy->buy_place = 0;
	buy->is_vip = ISVIP(p->flag);
	//for wt TODO 
	buy->log_type = (idc_type - 1) % 2;
	//buy->log_type = 1; 
	snprintf((char*)buy->log_info,15,"test");

	*(uint32_t *)p->session = buy->product_id;
	*(uint32_t *)(p->session + 4) = buy->product_count;

	return send_request_to_mall(p, proto_mall_buy_item, buff, sizeof(buy_req_t) + buy->ext_data_len);
}

int mall_buy_item_callback(sprite_t* p, uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_MALL_ERR(p, ret);

	buy_ret_t *buy = (buy_ret_t*)body;

	CHECK_BODY_LEN(bodylen, sizeof(buy_ret_t) + buy->data_len);

	
	uint32_t product_id = *(uint32_t *)p->session;
	uint32_t product_count = *(uint32_t *)(p->session + 4);
	product_t *product = get_product(product_id);

	if (product->type == 1) {
		cache_add_kind_item(p, product->itemid, product_count);
	}

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, product_id, i);
	PKG_UINT32(msg, product_count, i);
	PKG_UINT32(msg, product->itemid, i);
	PKG_UINT32(msg, buy->m1_price, i);
	PKG_UINT32(msg, buy->m1_balance, i);
	PKG_UINT32(msg, buy->m2_add, i);
	PKG_UINT32(msg, buy->m2_balance, i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}


#define SET_MALL_HANDLE(op_, func_, len_, cmp_) \
		do { \
			if (rstart == 0 && mall_handles[op_].func != 0) { \
				ERROR_RETURN(("duplicate cmd=%u", op_), -1); \
			} \
			mall_handles[op_].func = func_; \
			mall_handles[op_].len = len_; \
			mall_handles[op_].cmp_method = cmp_; \
		} while (0)

int init_mall_proto_handles(int rstart)
{
	SET_MALL_HANDLE(proto_mall_get_money_balance,mall_get_money_balance_callback,4,cmp_must_eq);
	SET_MALL_HANDLE(proto_mall_get_items_price,mall_get_items_price_callback,4,cmp_must_ge);
	SET_MALL_HANDLE(proto_mall_get_item_detail,mall_get_item_detail_callback,sizeof(product_detail_t),cmp_must_eq);
	SET_MALL_HANDLE(proto_mall_buy_item,mall_buy_item_callback,sizeof(buy_ret_t),cmp_must_ge);
	return 0;
}

void handle_mall_return(db_proto_t* cpkg, uint32_t pkglen)
{
	sprite_t* p = get_sprite(cpkg->id);
	if (!p) {
		KERROR_LOG(cpkg->id, "user not find: cmd=%u ret=%u", cpkg->cmd, cpkg->ret);
		return;
	}

	if ((p->waitcmd == 3200 && cpkg->cmd != proto_mall_get_money_balance) ||
		(p->waitcmd == 3201 && cpkg->cmd != proto_mall_get_items_price) ||
		(p->waitcmd == 3202 && cpkg->cmd != proto_mall_get_item_detail) ||
		(p->waitcmd == 3203 && cpkg->cmd != proto_mall_buy_item)	
		) {
		KERROR_LOG(cpkg->id, "cmd not match: wcmd=%u cmd=%u ret=%u", p->waitcmd, cpkg->cmd, cpkg->ret);
		return;
	}
	
	KDEBUG_LOG(p->id, "MALL\t[c=%u ret=%u]", cpkg->cmd, cpkg->ret);

	int err = -1;
	uint32_t bodylen = cpkg->len - sizeof(db_proto_t);

	if (!mall_handles[cpkg->cmd].func) {
		KERROR_LOG(0, "MALL_handle is NULL: cmd=%d", cpkg->cmd);
		return;
	}

	if ( ((mall_handles[cpkg->cmd].cmp_method == cmp_must_eq) && (bodylen != mall_handles[cpkg->cmd].len))
			|| ((mall_handles[cpkg->cmd].cmp_method == cmp_must_ge) && (bodylen < mall_handles[cpkg->cmd].len)) ) {
		if ((bodylen != 0) || (cpkg->ret == 0)) {
			KERROR_LOG(p->id, "invalid package len=%u needlen=%u cmd=0x%X cmpmethod=%d ret=%u",
						bodylen, mall_handles[cpkg->cmd].len, cpkg->cmd, mall_handles[cpkg->cmd].cmp_method, cpkg->ret);
			goto ret;
		}
	}

	err = mall_handles[cpkg->cmd].func(p, cpkg->body, bodylen, cpkg->ret);

ret:
	if (err) {
		KDEBUG_LOG(p->id, "MALL RET KICK USER OFF\t[%u]", err);
		close_client_conn(p->fd);
	}
}


