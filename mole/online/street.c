#include <assert.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include <glib.h>
#include <libxml/tree.h>

#include <libtaomee/list.h>
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>
#include <statistic_agent/msglog.h>

#include "communicator.h"
#include "proto.h"
#include "util.h"
#include <libtaomee/log.h>
#include "logic.h"
#include "sprite.h"
#include "shmq.h"
#include "map.h"
#include "item.h"
#include "tasks.h"
#include "dll.h"
#include "service.h"
#include "mole_doctor.h"
#include "cache_svr.h"


#include "race_car.h"
#include "hallowmas.h"
#include "small_require.h"
#include "event.h"
#include "message.h"
#include "mole_doctor.h"
#include "exclu_things.h"
#include "street.h"

int get_employee_level(uint32_t pet_id)
{
	int lvls[] = {1,1,9,19,40};

	if(pet_id > 0 && pet_id <= sizeof(lvls) / sizeof(lvls[0])) {
		return lvls[pet_id-1];
	}

	return 100;
}

int get_employee_money(uint32_t pet_id)
{
	int money[] = {1000,1000,2000,4000,6000};

	if(pet_id > 0 && pet_id <= sizeof(money)/ sizeof(money[0])) {
		return money[pet_id-1];
	}

	return 100000;
}

int get_employee_limit(uint32_t pet_id)
{
	int limit[] = {1*3600*24,1*3600*24,2*3600*24,3*3600*24,4*3600*24};

	if(pet_id > 0 && pet_id <= sizeof(limit)/sizeof(limit[0])) {
		return limit[pet_id-1];
	}

	return 1*3600*24;
}

int get_employee_limit_user_lahm(uint32_t level,uint32_t skill)
{
	switch(level) {
		case 2:
			return 3600*24*1;
		case 3:
			return 3600*24*2;
		case 4:
			return 3600*24*3;
		case 5:
			return 3600*24*4;
		case 101:
			return skill == 0 ? 3600*24*5 : 3600*24*7;
	}

	return 1*3600*24;
}

int get_employee_salary_user_lahm(uint32_t level,uint32_t skill)
{
	switch(level) {
		case 2:
			return 600;
		case 3:
			return 800;
		case 4:
			return 1000;
		case 5:
			return 1200;
		case 101:
			return skill == 0 ? 1500 : 2000;
	}

	return 0;
}


int get_employee_max_count(uint32_t level)
{
	int i=0;
	int lvls[] = {1,9,19,40};

	for(i=0; i < sizeof(lvls)/sizeof(lvls[0]);i++) {
		if(level < lvls[i]) break;
	}

	return i + 1;
}


int calc_shop_level(uint32_t shop_exp)
{
	int i = 0;
	uint32_t lvl_min[] = {
		0,30,80,170,360,650,1040,1530,2120,2830,
		3660,4610,5860,6870,8200,9670,11280,13030,14920,16970,
		19180,21550,24080,26770,29720,32930,36400,40130,44120,48470,
		53180,58250,63680,69470,75760,82550,89840,97630,105920,114710,
		124200,134190,144680,155670,167160,179150,191640,204630,218120,232610,
		247600,263090,279180,295870,313160,331050,349540,368630,388320,409510};

	for(i=0; i < sizeof(lvl_min) / sizeof(lvl_min[0]); i++) {
		if(shop_exp < lvl_min[i]) break;
	}

	return i;
}

int calc_dish_total_level(uint32_t shop_dish)
{
	int i = 0;
	uint32_t lvl_min[] = {1,11,51,201,501,1001};

	for(i=0; i < sizeof(lvl_min) / sizeof(lvl_min[0]); i++) {
		if(shop_dish < lvl_min[i]) break;
	}

	return i;
}

int calc_dish_level(uint32_t cnt, uint32_t step[]) {
	int i = 0;

	for(i=0; i < 5 && step[i]; i++) {
		if(cnt < step[i]) break;
	}

	return i;
}

int grid_nearly(uint32_t grid1, uint32_t grid2)
{
	return ((grid1 == grid2) || (grid1 + 1 == grid2) || (grid2 + 1 == grid1));
}

int notify_dish_level_up(sprite_t *p,uint32_t itemid,uint32_t level,uint32_t addexp,uint32_t addcount)
{
	uint8_t buff[128]={0};
	int bytes = sizeof(protocol_t);
	PKG_UINT32(buff,itemid,bytes);
	PKG_UINT32(buff,level,bytes);
	PKG_UINT32(buff,addexp,bytes);
	PKG_UINT32(buff,addcount,bytes);
	init_proto_head(buff,PROTO_NOTIFY_DISH_LEVEL_UP,bytes);
	return send_to_self(p,buff,bytes,0);
}

void set_honor_bit(sprite_t * p,int bit) {
	if(p->id == GET_UID_SHOP_MAP(p->tiles->id)) {
		if(test_bit_on(p->tiles->mmdu[0].shop.honors,8,bit)) return;
	}

	DEBUG_LOG("set_honor_bit %u %d",p->id,bit);
	uint32_t buff[3] = {0};
	buff[0] = bit;
	switch(bit) {
	case 1:
		p->yxb += 1000;
		buff[1] = 0;
		buff[2] = 1000;
		p->tiles->mmdu[0].shop.money += 1000;
		break;
	case 2:
		buff[1] = 200;
		buff[2] = 0;
		p->tiles->mmdu[0].shop.exp += 200;
		break;
	case 3:
		buff[1] = 500;
		buff[2] = 0;
		p->tiles->mmdu[0].shop.exp += 500;
		break;
	case 4:
		buff[1] = 0;
		buff[2] = 0;
		break;
	case 5:
		buff[1] = 100;
		buff[2] = 0;
		p->tiles->mmdu[0].shop.exp += 100;
		break;
	case 6:
		buff[1] = 0;
		buff[2] = 0;
		break;
	case 7:
		buff[1] = 0;
		buff[2] = 0;
		break;
	case 8:
		buff[1] = 0;
		buff[2] = 0;
		break;
	case 9:
		buff[1] = 0;
		buff[2] = 3000;
		p->yxb += 3000;
		if(p->id == GET_UID_SHOP_MAP(p->tiles->id)) {
			p->tiles->mmdu[0].shop.money += 3000;
		}
		break;
	case 10:
		buff[1] = 300;
		buff[2] = 0;
		if(p->id == GET_UID_SHOP_MAP(p->tiles->id)) {
			p->tiles->mmdu[0].shop.exp += 300;
		}
		break;
	case 11:
		buff[1] = 0;
		buff[2] = 0;
		break;
	case 12:
		buff[1] = 0;
		buff[2] = 0;
		break;
	case 13:
		buff[1] = 1000;
		buff[2] = 0;
		if(p->id == GET_UID_SHOP_MAP(p->tiles->id)) {
			p->tiles->mmdu[0].shop.exp += 1000;
		}
		break;
	case 14:
		buff[1] = 0;
		buff[2] = 0;
		break;
	case 15:
		buff[1] = 0;
		buff[2] = 0;
		break;
	case 16:
		buff[1] = 0;
		buff[2] = 0;
		break;
	}
	if(p->id == GET_UID_SHOP_MAP(p->tiles->id)) {
		set_bit_on(p->tiles->mmdu[0].shop.honors,8,bit);
	}
	send_request_to_db(SVR_PROTO_SET_HONOR_BIT, NULL, 12, buff, p->id);

	int i =  sizeof(protocol_t);
	uint8_t p_msg[128] ={0};
	PKG_UINT32(p_msg,bit,i);
	init_proto_head(p_msg,PROTO_NOTIFY_HONOR_ID,i);
	send_to_self(p, p_msg, i, 0);
}

int check_condtion(sprite_t * p, uint32_t channel, uint32_t condtion)
{
	switch(channel){
	case 0:
		return 1;
	case 1:
		if(condtion <= p->tiles->mmdu[0].shop.last_level)
			return 1;
		break;
	case 2:
		if(check_fini_task(p,condtion))
			return 1;
		break;
	case 3:
		if(test_bit_on(p->tiles->mmdu[0].shop.honors,8,condtion))
			return 1;
		break;
	default:
		break;
	}

	return 0;
}

int db_add_shop_event_count(sprite_t * p, uint32_t id)
{
	return send_request_to_db(SVR_PROTO_ADD_SHOP_EVENT_COUNT,p,0,NULL,id);
}

int db_change_shop_exp(sprite_t * p,uint32_t exp, uint32_t id)
{
	return send_request_to_db(SVR_PROTO_ADD_SHOP_EXP,p,4,&exp,id);
}

int db_change_shop_money(sprite_t * p,uint32_t money, uint32_t id)
{
	return send_request_to_db(SVR_PROTO_ADD_SHOP_MONEY,p,4,&money,id);
}

int get_certificates_for_building_cmd(sprite_t * p,const uint8_t * body,int len)
{
	item_t *itm = get_item_prop(190658);

	if(!itm) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}

	DEBUG_LOG("%u cmd=%d MONEY=%d %d",p->id,p->waitcmd,p->yxb,itm->price);

	if(p->yxb < itm->price) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_enough_xiaomee, 1);
	}

	return db_buy_items(p, itm, 1, find_kind_of_item(190658), 0, 1);
}

int db_get_building_certificates(sprite_t *p)
{
	DEBUG_LOG("%u cmd=%d MONEY=%d",p->id,p->waitcmd,p->yxb);

	response_proto_uint32(p, p->waitcmd, p->yxb, 0);
	return 0;
}

int get_map_last_grid_cmd(sprite_t * p,const uint8_t * body,int len)
{
	return send_request_to_db(SVR_PROTO_GET_MAP_LAST_GRID_ID, p, 0, NULL, p->id);
}

int get_map_last_grid_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 12);

	int i = 0;
	uint32_t end_grid;
	uint32_t start_grid;
	uint32_t my_grid;

	UNPKG_H_UINT32(buf,end_grid,i); /*客户端减一*/
	UNPKG_H_UINT32(buf,start_grid,i);
	UNPKG_H_UINT32(buf,my_grid,i);

	if(my_grid != 0) my_grid--;
	if(start_grid != 0) start_grid--;

	DEBUG_LOG("get_map_last_grid_callback %d (%d %d)",my_grid,start_grid,end_grid);

	int bytes = sizeof(protocol_t);
	PKG_UINT32(msg,start_grid,bytes);
	PKG_UINT32(msg,end_grid,bytes);
	PKG_UINT32(msg,my_grid,bytes);

	init_proto_head(msg,p->waitcmd,bytes);
	return send_to_self(p,msg,bytes,1);
}

int db_set_pet_employed_uid(sprite_t* p, uint32_t id, uint32_t petid, uint32_t em_uid)
{
	uint32_t buff[2] = {0};

	buff[0] = petid;
	buff[1] = em_uid;
	return send_request_to_db(SVR_PROTO_SET_PET_EMPLOYED_UID, p, 8, buff, id);
}

int list_map_building_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	map_id_t mapid;
	uint32_t map_grid;
    CHECK_BODY_LEN(len, 12);
 	unpkg_mapid(body, &mapid, &i);
	UNPKG_UINT32(body,map_grid,i);

//	DEBUG_LOG("LIST MAP BUILDINGS\t[%u %llx self=%u grid=%u]", p->id, mapid, p->sub_grid,map_grid);

	if (mapid != p->tiles->id) {
		ERROR_LOG("uid=%u request map=%lu while in map=%lu", p->id, mapid, p->tiles->id);
	}

	map_grid++;
	p->sess_len = 0;
	PKG_H_UINT32(p->session,map_grid,p->sess_len);
	return send_request_to_db(SVR_PROTO_GET_MAP_BUILDING_INFO, p, 4, &map_grid, p->id);
}

int list_map_building_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i = 0;
	int pkgsize = 0;
	uint32_t buildings = 0;
	uint32_t islastgrid = 0;


	uint8_t name[16];
	uint32_t type;
	uint32_t owner;
	uint32_t style;
	uint32_t roomid;

	uint8_t* tmp = msg + sizeof(protocol_t);
	memset(tmp, 0, 4 + 4 * (32));
	UNPKG_H_UINT32(buf,buildings,i);
	UNPKG_H_UINT32(buf,islastgrid,i);
	PKG_UINT32(tmp, islastgrid, pkgsize);

	while(buildings) {
		UNPKG_H_UINT32(buf,roomid,i);
		UNPKG_H_UINT32(buf,owner,i);
		UNPKG_STR(buf, name, i, 16);
		UNPKG_H_UINT32(buf,style,i);
		UNPKG_H_UINT32(buf,type,i);

		PKG_UINT32(tmp,type,pkgsize);//type
		PKG_UINT32(tmp,style,pkgsize);//style
		PKG_UINT32(tmp,owner,pkgsize);//owner
		PKG_UINT32(tmp,roomid,pkgsize);//roomid
		PKG_STR(tmp, name, pkgsize, 16);//name
		buildings--;
	}

	pkgsize = 4 * 32 + 4 + sizeof(protocol_t);
	init_proto_head(msg, p->waitcmd, pkgsize);
	return send_to_self(p, msg, pkgsize, 1);
}

int set_building_name_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_BODY_LEN(len, 20);

	if(!p->tiles || p->id != GET_UID_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_shop_not_yours, 1);
	}

	p->sess_len = len;
	memcpy(p->session, body, len);
	*(uint32_t*)p->session = ntohl(*(uint32_t*)p->session);

	p->session[len] = 0;
	CHECK_DIRTYWORD(p, p->session + 4);

//	DEBUG_LOG("set_building_name_cmd(%u,%s)",*(uint32_t*)p->session,p->session + 4);

	return send_request_to_db(SVR_PROTO_CHANGE_BUILDING_NAME, p, len, p->session, p->id);
}

int set_building_name_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	response_proto_str(p, p->waitcmd, 16, p->session + 4, 0);

//	DEBUG_LOG("set_building_name_cmd(%u,%s)",*(uint32_t*)p->session,p->session + 4);

	return send_request_to_db(SVR_PROTO_SET_BD_NAME_IN_USER, 0, p->sess_len, p->session, p->id);
}

int create_building_cmd(sprite_t * p,const uint8_t * body,int len)
{
	uint32_t need_money = 0;
	uint32_t need_item = 190658;
	uint32_t room_style = 1330001;
	uint32_t inner_style = 1330004;
	uint32_t room_type  = ntohl(*(uint32_t*)(body + 16));

	CHECK_BODY_LEN(len, 20);

	char buf[20] = {0};
	memcpy(buf,body,16);
	CHECK_DIRTYWORD(p, buf);

	if(room_type != 31) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	if(ISVIP(p->flag)) {
		room_style = 1330002;
		inner_style = 1330004;
	}

	item_t *itm = get_item_prop(room_style);
	item_t *itm_in = get_item_prop(inner_style);
	if( !itm  || !itm_in ) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}

	need_money = itm->price + itm_in->price;
	if(p->yxb < need_money) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_enough_xiaomee, 1);
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session,need_money,p->sess_len);
	PKG_STR(p->session,buf, p->sess_len,16);
	PKG_H_UINT32(p->session,room_style,p->sess_len);
	PKG_H_UINT32(p->session,room_type, p->sess_len);
	PKG_H_UINT32(p->session,inner_style, p->sess_len);

	int i = 0;
	PKG_H_UINT32(buf, 1, i);
	PKG_H_UINT32(buf, need_item, i);
	return send_request_to_db(SVR_QUERY_ITEM_COUNT, p, i, buf, p->id);
}

int create_building_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i = 0;
	int need_money;
	uint32_t roomid;
	uint32_t room_style = 1330001;
	uint32_t inner_style = 1330004;

	CHECK_BODY_LEN(len, 4);
	UNPKG_H_UINT32(buf,roomid,i);

	i = 0;
	UNPKG_H_UINT32(p->session,need_money,i);
	db_change_xiaome(NULL,-need_money,0,0xBDCE,p->id);
	p->yxb -= need_money;

	i = 0;
	p->dining_flag = 1;
	p->dining_level = 1;
	PKG_H_UINT32(p->session,roomid,i);
	response_proto_uint32(p, p->waitcmd, roomid, 0);


	if(ISVIP(p->flag)) {
		room_style = 1330002;
		inner_style = 1330004;
	}
	uint32_t entertag[] = {1,0};
	msglog(statistic_logfile, 0x02020006, now.tv_sec, entertag, 4);
	msglog(statistic_logfile, 0x02023000 + room_style - 1330000, now.tv_sec, entertag, sizeof(entertag));
	msglog(statistic_logfile, 0x02023000 + inner_style - 1330000, now.tv_sec, entertag, sizeof(entertag));

	if(ISVIP(p->flag)) {
		uint32_t data[] = {1,0};
		msglog(statistic_logfile, 0x02020007, now.tv_sec, data, 8);
	} else {
		uint32_t data[] = {0,1};
		msglog(statistic_logfile, 0x02020007, now.tv_sec, data, 8);
	}

	msglog(statistic_logfile, 0x02021101, now.tv_sec, entertag, sizeof(entertag));
	msglog(statistic_logfile, 0x02021001, now.tv_sec, entertag, sizeof(entertag));
	return send_request_to_db(SVR_PROTO_ADD_BUILDING_TO_USER, 0, p->sess_len, p->session, p->id);
}

int notify_shop_info_change(sprite_t * p)
{
	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id) || p->id != GET_UID_SHOP_MAP(p->tiles->id)) {
		ERROR_RETURN(("notify_shop_info_change() map error"),-1);
	}
	uint8_t p_msg[128] = {0};
	int i = sizeof(protocol_t);
	int shop_level = calc_shop_level(p->tiles->mmdu[0].shop.exp);

	PKG_UINT32(p_msg,shop_level,i);
	PKG_UINT32(p_msg,p->tiles->mmdu[0].shop.exp,i);
	PKG_UINT32(p_msg,p->tiles->mmdu[0].shop.money,i);
	PKG_UINT32(p_msg,p->tiles->mmdu[0].shop.inner_style,i);

	init_proto_head(p_msg, PROTO_NOTIFY_SHOP_INFO_CHANGE, i);
	send_to_self(p, p_msg, i, 0);

	cs_set_shop_info(p->tiles->mmdu[0].shop.exp, p->tiles->mmdu[0].shop.money, p->id);

	if(p->tiles->mmdu[0].shop.last_level < shop_level) {
		if((p->tiles->mmdu[0].shop.last_level -1) / 3 < (shop_level - 1) / 3) {
			uint32_t leavetag[] = {0,1};
			uint32_t entertag[] = {1,0};
			msglog(statistic_logfile, 0x02021001 + (shop_level - 1) / 3, now.tv_sec, entertag, sizeof(entertag));
			msglog(statistic_logfile, 0x02021001 + (p->tiles->mmdu[0].shop.last_level - 1) / 3, now.tv_sec, leavetag, sizeof(leavetag));
		}
		p->tiles->mmdu[0].shop.last_level = shop_level;
		return send_request_to_db(SVR_PROTO_SET_SHOP_LEVEL, NULL, 4, &shop_level,  p->id);
	}

	return 0;
}

int set_building_style_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_BODY_LEN(len, 8);

	uint32_t roomid,style;

	if(!p->tiles || p->id != GET_UID_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_shop_not_yours, 1);
	}

	p->sess_len = 0;
	UNPKG_UINT32(body,roomid,p->sess_len);
	UNPKG_UINT32(body,style,p->sess_len);

	item_kind_t *ik_style = find_kind_of_item(style);

	if(!ik_style || ik_style->kind != STREET_ITEM_KIND) {
		return send_to_self_error(p, p->waitcmd, -ERR_style_item_err, 1);
	}

	item_t *itm = get_item(ik_style, style);
	if(!itm || itm->layer != 1 || itm->u.shop_tag.type != 31) {
		return send_to_self_error(p, p->waitcmd, -ERR_style_item_err, 1);
	}

	if(itm_vip_only(itm) && !ISVIP((p)->flag)) {
		return send_to_self_error(p, p->waitcmd, -ERR_style_item_err, 1);
	}

	if(p->yxb < itm->price) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_enough_xiaomee, 1);
	}

	if(!check_condtion(p,itm->u.shop_tag.channel, itm->u.shop_tag.condition)) {
		return send_to_self_error(p, p->waitcmd, -ERR_style_item_locked, 1);
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session,roomid,p->sess_len);
	PKG_H_UINT32(p->session,style,p->sess_len);
	PKG_H_UINT32(p->session,itm->price,p->sess_len);
	PKG_H_UINT32(p->session,itm->u.shop_tag.channel,p->sess_len);
	PKG_H_UINT32(p->session,itm->u.shop_tag.condition,p->sess_len);

	return send_request_to_db(SVR_PROTO_CHANGE_BUILDING_STYLE, p, p->sess_len, p->session, p->id);
}

int set_building_style_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t price = 0;
	uint32_t shop_style;

	int i = 4;
	UNPKG_H_UINT32(p->session,shop_style,i);
	UNPKG_H_UINT32(p->session,price,i);

	uint32_t leavetag[] = {0,1};
	uint32_t entertag[] = {1,0};
	msglog(statistic_logfile, 0x02023000 + shop_style - 1330000, now.tv_sec, entertag, sizeof(entertag));
	msglog(statistic_logfile, 0x02023000 + p->tiles->mmdu[0].shop.shop_style - 1330000, now.tv_sec, leavetag, sizeof(leavetag));

	p->yxb -= price;
	p->tiles->mmdu[0].shop.money -= price;
	p->tiles->mmdu[0].shop.shop_style = shop_style;
	response_proto_uint32_uint32(p, p->waitcmd, price, shop_style, 0);

	DEBUG_LOG("building_style %u style=%d money=%d",p->id,shop_style,price);

	send_request_to_db(SVR_PROTO_SET_BD_STYLE_IN_USER, NULL, p->sess_len, p->session, p->id);

	return notify_shop_info_change(p);
}

int set_building_inner_style_cmd(sprite_t * p,const uint8_t * body,int len)
{
	uint32_t roomid,inner_style;

	if(!p->tiles || p->id != GET_UID_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_shop_not_yours, 1);
	}

	p->sess_len = 0;
	CHECK_BODY_LEN(len, 8);
	UNPKG_UINT32(body,roomid,p->sess_len);
	UNPKG_UINT32(body,inner_style,p->sess_len);

	item_kind_t *ik_style = find_kind_of_item(inner_style);

	if(!ik_style || ik_style->kind != STREET_ITEM_KIND) {
		return send_to_self_error(p, p->waitcmd, -ERR_style_item_err, 1);
	}

	item_t *itm = get_item(ik_style, inner_style);
	if(!itm || itm->layer != 2 || itm->u.shop_tag.type != 31) {
		return send_to_self_error(p, p->waitcmd, -ERR_style_item_err, 1);
	}

	if(itm_vip_only(itm) && !ISVIP((p)->flag)) {
		return send_to_self_error(p, p->waitcmd, -ERR_style_item_err, 1);
	}

	if(p->yxb < itm->price) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_enough_xiaomee, 1);
	}

	if(!check_condtion(p,itm->u.shop_tag.channel, itm->u.shop_tag.condition)) {
		return send_to_self_error(p, p->waitcmd, -ERR_style_item_locked, 1);
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session,roomid,p->sess_len);
	PKG_H_UINT32(p->session,inner_style,p->sess_len);
	PKG_H_UINT32(p->session,itm->price,p->sess_len);
	PKG_H_UINT32(p->session,itm->u.shop_tag.stoves,p->sess_len);
	PKG_H_UINT32(p->session,itm->u.shop_tag.foodtables + 50,p->sess_len);

	return send_request_to_db(SVR_PROTO_SET_INNER_STYLE_IN_USER, p, p->sess_len, p->session, p->id);
}

int set_building_inner_style_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t price = 0;
	uint32_t inner_style;

	p->sess_len = 4;
	UNPKG_H_UINT32(p->session,inner_style,p->sess_len);
	UNPKG_H_UINT32(p->session,price,p->sess_len);
	response_proto_uint32_uint32(p, p->waitcmd, inner_style, price, 1);

	uint32_t leavetag[] = {0,1};
	uint32_t entertag[] = {1,0};
	msglog(statistic_logfile, 0x02023000 + inner_style - 1330000, now.tv_sec, entertag, sizeof(entertag));
	msglog(statistic_logfile, 0x02023000 + p->tiles->mmdu[0].shop.inner_style - 1330000, now.tv_sec, leavetag, sizeof(leavetag));

	p->yxb -= price;
	p->tiles->mmdu[0].shop.money -= price;
	p->tiles->mmdu[0].shop.inner_style = inner_style;
	return notify_shop_info_change(p);
}

int get_shop_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	uint32_t shop_uid;
	uint32_t shop_type;
	uint32_t shop_owner;

	CHECK_BODY_LEN(len,8);

	UNPKG_UINT32(body,shop_uid,i);
	UNPKG_UINT32(body,shop_type,i);

	p->sess_len = 0;
	shop_owner = (shop_uid == p->id);
	PKG_H_UINT32(p->session,shop_uid,p->sess_len);
	PKG_H_UINT32(p->session,shop_type,p->sess_len);
	PKG_H_UINT32(p->session,shop_owner,p->sess_len);

	return send_request_to_db(SVR_PROTO_GET_SHOP_INFO, p, p->sess_len, p->session, shop_uid);
}

int get_shop_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	struct info_hdr {
		uint32_t	userid;
		uint32_t	roomid;
		char		room_name[16];
		uint32_t	type_id;
		uint32_t	exp;
		int			xiaomee;
		uint32_t	evaluate;
		uint32_t	level;
		uint32_t	inner_style;
		uint32_t    shop_style;
		uint32_t	dish_total;
		uint32_t	addmoney;
		uint8_t		honors[8];
		uint32_t	level_5_count;
		uint32_t	dish_count;
		uint32_t	employer_count;
	}__attribute__((packed));

	struct	employee_item{
		uint32_t em_userid;
		char	 em_user_name[16];
		uint32_t em_petid;
		uint32_t pet_color;
		uint32_t pet_level;
		uint32_t pet_skill;
		uint32_t level;
		uint32_t time;
		uint32_t time_limit;
	}__attribute__((packed));

	int dish_bad = 0;

	int i;
	int pkgsize = sizeof(protocol_t);
	struct info_hdr *hdr_p = (struct info_hdr*)buf;

	CHECK_BODY_LEN_GE(len,sizeof(struct info_hdr));

	int shop_level = calc_shop_level(hdr_p->exp);
	if(shop_level > hdr_p->level) {
		if((hdr_p->level -1) / 3 < (shop_level - 1) / 3) {
			uint32_t leavetag[] = {0,1};
			uint32_t entertag[] = {1,0};
			msglog(statistic_logfile, 0x02021001 + (shop_level - 1) / 3, now.tv_sec, entertag, sizeof(entertag));
			msglog(statistic_logfile, 0x02021001 + (hdr_p->level - 1) / 3, now.tv_sec, leavetag, sizeof(leavetag));
		}
		hdr_p->level = shop_level;
		send_request_to_db(SVR_PROTO_SET_SHOP_LEVEL, NULL, 4, &shop_level,  hdr_p->userid);
	}

	/* room info*/
	PKG_UINT32(msg, hdr_p->userid, pkgsize);//owner
	PKG_UINT32(msg, hdr_p->roomid, pkgsize);//room id
	PKG_UINT32(msg, (hdr_p->roomid - 1) / 4, pkgsize);//room grid
	PKG_UINT32(msg, hdr_p->exp, pkgsize);//room exp
	PKG_UINT32(msg, hdr_p->xiaomee, pkgsize);//room money
	PKG_UINT32(msg, hdr_p->evaluate, pkgsize);//room good
	PKG_UINT32(msg, hdr_p->type_id, pkgsize);//room type
	PKG_UINT32(msg, hdr_p->level, pkgsize);//room level
	PKG_UINT32(msg, hdr_p->dish_total, pkgsize);//total dishs
	PKG_UINT32(msg, hdr_p->inner_style, pkgsize);//inner style
	PKG_UINT32(msg, hdr_p->shop_style, pkgsize);//inner style
	PKG_STR(msg, hdr_p->room_name, pkgsize, 16);//room name
	PKG_UINT32(msg, hdr_p->addmoney, pkgsize);//inner style
	PKG_UINT32(msg, hdr_p->level_5_count, pkgsize);//level_5_count

	i = sizeof(struct info_hdr);
	i += hdr_p->dish_count * sizeof(struct dish_item);
	i += hdr_p->employer_count * sizeof(struct employee_item);
	CHECK_BODY_LEN(len,i);

	/*food */
	struct dish_item *dish = (struct dish_item*)(buf + sizeof(struct info_hdr));
	PKG_UINT32(msg, hdr_p->dish_count, pkgsize);//foods
	for(i = 0; i < hdr_p->dish_count; i++) {
		PKG_UINT32(msg, dish->location, pkgsize);//food where
		PKG_UINT32(msg, dish->dish_id, pkgsize);//food itemid
		PKG_UINT32(msg, dish->id, pkgsize);//food index
		PKG_UINT32(msg, dish->count, pkgsize);//food count
		PKG_UINT32(msg, dish->state,pkgsize);//food state
		PKG_UINT32(msg, dish->time, pkgsize);//food time

		if(dish_bad == 0 && dish->state == 3 && dish->location < 50) {
			item_t *itm = get_item_prop(dish->dish_id);
			if(itm && itm->u.food_tag.timer + itm->u.food_tag.bad_timer <= dish->time) {
				dish_bad = 1;
			}
		}

		dish++;
	}

	/*players*/
	int count = 0;
	int bytes = pkgsize;
	uint32_t money,emlvl;
	struct employee_item *em_p = (struct employee_item*)(dish);
	PKG_UINT32(msg, hdr_p->employer_count, pkgsize);//players
	for(i = 0; i < hdr_p->employer_count; i++,em_p++) {
		if(em_p->time >= em_p->time_limit) {
			uint32_t buff[2] = {0};
			buff[0] = em_p->em_userid;
			buff[1] = em_p->em_petid;
			send_request_to_db(SVR_PROTO_DEL_EMPLOYEE,NULL,8,buff,GET_UID_SHOP_MAP(p->tiles->id));

			char p_msg[2048] ={0};
			if(em_p->em_userid != 10000) {
				pet_t *pet = get_pet(em_p->em_userid,em_p->em_petid);
				if(pet) RESET_PET_EMPLOYED(pet);
				db_set_pet_arg(NULL,PET_FLAG,0,em_p->em_petid,em_p->em_userid);
				db_set_pet_employed_uid(NULL,em_p->em_userid,em_p->em_petid,0);
#ifndef TW_VER
				if(em_p->em_userid != GET_UID_SHOP_MAP(p->tiles->id)) {
					int salary = get_employee_salary_user_lahm(em_p->pet_level,em_p->pet_skill);
					db_change_xiaome(NULL,salary,0,0,em_p->em_userid);
					sprintf(p_msg,"亲爱的主人我已从%u的餐厅打工回来啦，总共获得了%d摩尔豆，我干的不错吧。",GET_UID_SHOP_MAP(p->tiles->id),salary);
					send_postcard("摩摩商会",0,em_p->em_userid,1000128,p_msg,0);
					sprintf(p_msg,"%u的拉姆在你的餐厅打工结束了，已经回到它主人身边啦。",em_p->em_userid);
					send_postcard("摩摩商会",0,GET_UID_SHOP_MAP(p->tiles->id),1000133,p_msg,0);
				} else {
					sprintf(p_msg,"亲爱的主人，我已从你的餐厅中打工回来，赶紧给我准备一顿大餐吧，休息好了我还想继续工作。");
					send_postcard("摩摩商会",0,GET_UID_SHOP_MAP(p->tiles->id),1000128,p_msg,0);
				}
#else
				if(em_p->em_userid != GET_UID_SHOP_MAP(p->tiles->id)) {
					int salary = get_employee_salary_user_lahm(em_p->pet_level,em_p->pet_skill);
					db_change_xiaome(NULL,salary,0,0,em_p->em_userid);
					sprintf(p_msg,"親愛的主人我已從%u的餐廳打工回來啦，總共獲得了%d摩爾豆，我有努力工作喔！",GET_UID_SHOP_MAP(p->tiles->id),salary);
					send_postcard("摩摩商會",0,em_p->em_userid,1000128,p_msg,0);
					sprintf(p_msg,"%u的拉姆在你的餐廳打工結束了，已經回到它主人身邊啦。",em_p->em_userid);
					send_postcard("摩摩商會",0,GET_UID_SHOP_MAP(p->tiles->id),1000133,p_msg,0);
				} else {
					sprintf(p_msg,"親愛的主人，我已從你的餐廳中打工回來，趕緊給我準備一頓大餐吧，休息好了我還想繼續工作。");
					send_postcard("摩摩商會",0,GET_UID_SHOP_MAP(p->tiles->id),1000128,p_msg,0);
				}
#endif
			}
			continue;
		}

		if(em_p->em_userid == 10000) {
			money = get_employee_money(em_p->em_petid);
			emlvl = get_employee_level(em_p->em_petid);
		} else {
			money = 0;
			emlvl = em_p->level;
		}

		PKG_UINT32(msg, em_p->em_userid, pkgsize);//player id
		PKG_UINT32(msg, em_p->em_petid, pkgsize);//player petid
		PKG_STR(msg, em_p->em_user_name, pkgsize,16);//player name
		PKG_UINT32(msg, em_p->pet_color, pkgsize);//player color
		PKG_UINT32(msg, em_p->pet_level, pkgsize);//player level
		PKG_UINT32(msg, em_p->pet_skill, pkgsize);//player level
		PKG_UINT32(msg, emlvl, pkgsize);//employe level
		PKG_UINT32(msg, money, pkgsize);//employe money
		PKG_UINT32(msg, em_p->time, pkgsize);//employed time
		PKG_UINT32(msg, em_p->time_limit, pkgsize);//employe time limit
		count++;
	}

	hdr_p->employer_count = count;
	PKG_UINT32(msg, hdr_p->employer_count, bytes);//players

	item_t *itm = get_item_prop(hdr_p->inner_style);
	if(itm) {
		p->posX = itm->u.shop_tag.postion[0];
		p->posY = itm->u.shop_tag.postion[1];
	}

	cs_set_shop_info(hdr_p->exp,hdr_p->xiaomee,hdr_p->userid);

	if(hdr_p->userid == GET_UID_SHOP_MAP(p->tiles->id)) {
		p->tiles->mmdu[0].shop.roomid = hdr_p->roomid;
		p->tiles->mmdu[0].shop.last_level = hdr_p->level;
		p->tiles->mmdu[0].shop.exp	  = hdr_p->exp;
		p->tiles->mmdu[0].shop.money  = hdr_p->xiaomee;
		p->tiles->mmdu[0].shop.evaluate  = hdr_p->evaluate;
		p->tiles->mmdu[0].shop.dish_total = hdr_p->dish_total;
		p->tiles->mmdu[0].shop.employed_count = hdr_p->employer_count;
		p->tiles->mmdu[0].shop.inner_style = hdr_p->inner_style;
		p->tiles->mmdu[0].shop.shop_style = hdr_p->shop_style;
		memcpy(p->tiles->mmdu[0].shop.honors,hdr_p->honors,sizeof(hdr_p->honors));
		p->tiles->mmdu[0].shop.dish_bad = dish_bad;
		msglog(statistic_logfile, 0x04040101, now.tv_sec, &(p->id), 4);
	}

	if(hdr_p->userid == p->id) {
		p->yxb += hdr_p->addmoney;

		if(hdr_p->xiaomee >= 10000)
			set_honor_bit(p,5);
		if(hdr_p->xiaomee >= 50000)
			set_honor_bit(p,6);
		if(hdr_p->xiaomee >= 200000)
			set_honor_bit(p,7);
		if(hdr_p->xiaomee >= 500000)
			set_honor_bit(p,8);

		if(hdr_p->level_5_count >= 1)
			set_honor_bit(p,13);
		if(hdr_p->level_5_count >= 3)
			set_honor_bit(p,14);
		if(hdr_p->level_5_count >= 8)
			set_honor_bit(p,15);
		if(hdr_p->level_5_count >= 15)
			set_honor_bit(p,16);

	}

	uint32_t *puid = (uint32_t*)&msg[sizeof(protocol_t)];
	DEBUG_LOG("get_shop_info_callback %u %u %u",p->id,hdr_p->userid,ntohl(*puid));

	init_proto_head(msg, p->waitcmd, pkgsize);
	return send_to_self(p, msg, pkgsize, 1);
}


int get_employee_list_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i=0;
	uint32_t shop_uid;
	uint32_t shop_type;

	CHECK_BODY_LEN(len,8);
	UNPKG_UINT32(body,shop_uid,i);
	UNPKG_UINT32(body,shop_type,i);

	p->sess_len = 0;
	PKG_H_UINT32(p->session,shop_uid,p->sess_len);
	PKG_H_UINT32(p->session,shop_type,p->sess_len);

	return send_request_to_db(SVR_PROTO_LIST_EMPLOYEE, p, 0, NULL, shop_uid);
}

int get_employee_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	struct	employee_pet{
		uint32_t em_userid;
		uint32_t em_petid;
		uint32_t em_index;
		char	 em_name[16];
		uint32_t em_time;
		uint32_t em_money;
		uint32_t em_color;
		uint32_t em_level;
	}__attribute__((packed));

	int i = 0;
	int pkgsize = sizeof(protocol_t);

	uint32_t count = 0;
	UNPKG_H_UINT32(buf,count,i);
	PKG_UINT32(msg, count, pkgsize);//players

	struct	employee_pet *em_p = (struct employee_pet*)(buf + 4);
	for(i = 0; i < count; i++) {
		PKG_UINT32(msg, em_p[i].em_userid, pkgsize);//player id
		PKG_UINT32(msg, em_p[i].em_petid, pkgsize);//player petid
		PKG_STR(msg,em_p[i].em_name, pkgsize,16);//player name
		PKG_UINT32(msg, em_p[i].em_color, pkgsize);//player color
		PKG_UINT32(msg, em_p[i].em_level, pkgsize);//player level
		PKG_UINT32(msg, 0, pkgsize);//player skillbit
		PKG_UINT32(msg, get_employee_level(em_p[i].em_petid), pkgsize);//employe level
		PKG_UINT32(msg, get_employee_money(em_p[i].em_petid), pkgsize);//employe salary
		PKG_UINT32(msg, 0, pkgsize);//employed time
		PKG_UINT32(msg, get_employee_limit(em_p[i].em_petid), pkgsize);//employe limit
	}

	init_proto_head(msg, p->waitcmd, pkgsize);
	return send_to_self(p, msg, pkgsize, 1);
}

int add_employee_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	uint32_t userid;
	uint32_t pet_id;
	uint32_t pcolor;
	uint32_t plevel;
	uint32_t pskill;
	uint32_t emp_lv;
	uint32_t emoney;
	uint32_t second;
	uint32_t p_name[20]={0};
	uint32_t employ_max = get_employee_max_count(p->tiles->mmdu[0].shop.last_level);

	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	if(p->tiles->mmdu[0].shop.employed_count >= employ_max) {
		return send_to_self_error(p, p->waitcmd, -ERR_employee_count_limit, 1);
	}

	CHECK_BODY_LEN(len,32);

	UNPKG_UINT32(body, userid, i);
	UNPKG_UINT32(body, pet_id, i);
	UNPKG_UINT32(body, pcolor, i);
	UNPKG_UINT32(body, plevel, i);
	UNPKG_STR(body, p_name, i,16);

	if (userid == 10000) {
		if (GET_UID_SHOP_MAP(p->tiles->id) != p->id) {
			return send_to_self_error(p, p->waitcmd, -ERR_the_shop_not_yours, 1);
		}
		emp_lv = get_employee_level(pet_id);

		if(emp_lv > p->tiles->mmdu[0].shop.last_level) {
			return send_to_self_error(p, p->waitcmd, -ERR_shop_level_too_low, 1);
		}

		pskill = 0;
		emoney = get_employee_money(pet_id);
		second = get_employee_limit(pet_id);
	} else {
		if (!p->followed) {
			return send_to_self_error(p, p->waitcmd, -ERR_the_lahm_not_followed, 1);
		}

		if (PET_SICK(p->followed) || check_lahm_sick(p->followed,p->id)) {
			return send_to_self_error(p, p->waitcmd, -ERR_lahm_is_sick, 1);
		}

		if(p->followed->hungry < 30 || p->followed->thirsty < 30 ||
			p->followed->sanitary < 30 || p->followed->spirit < 30 ) {
			return send_to_self_error(p, p->waitcmd, -ERR_lahm_is_feel_bad, 1);
		}

		emp_lv = 0;
		emoney = 0;
		userid = p->id;
		pet_id = p->followed->id;
		pcolor = p->followed->color;
		pskill = p->followed->skill_bits;
		plevel = get_pet_level(p->followed);
		second = get_employee_limit_user_lahm(plevel,pskill);
		memcpy(p_name, p->followed->nick, 16);
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session,userid,p->sess_len);
	PKG_STR(p->session,p_name,p->sess_len,16);
	PKG_H_UINT32(p->session,pet_id,p->sess_len);
	PKG_H_UINT32(p->session,second,p->sess_len);
	PKG_H_UINT32(p->session,pcolor,p->sess_len);
	PKG_H_UINT32(p->session,plevel,p->sess_len);
	PKG_H_UINT32(p->session,pskill,p->sess_len);
	PKG_H_UINT32(p->session,emp_lv,p->sess_len);
	PKG_H_UINT32(p->session,emoney,p->sess_len);
	PKG_H_UINT32(p->session,employ_max,p->sess_len);

	return send_request_to_db(SVR_PROTO_ADD_EMPLOYEE, p, p->sess_len, p->session, GET_UID_SHOP_MAP(p->tiles->id));
}

int add_employee_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t userid;
	uint32_t pet_id;
	uint32_t pcolor;
	uint32_t plevel;
	uint32_t pskill;
	uint32_t emp_lv;
	uint32_t emoney;
	uint32_t second;
	uint32_t p_name[16]={0};

	CHECK_BODY_LEN(len,0);

	p->sess_len = 0;
	UNPKG_H_UINT32(p->session,userid,p->sess_len);
	UNPKG_STR(p->session,p_name,p->sess_len,16);
	UNPKG_H_UINT32(p->session,pet_id,p->sess_len);
	UNPKG_H_UINT32(p->session,second,p->sess_len);
	UNPKG_H_UINT32(p->session,pcolor,p->sess_len);
	UNPKG_H_UINT32(p->session,plevel,p->sess_len);
	UNPKG_H_UINT32(p->session,pskill,p->sess_len);
	UNPKG_H_UINT32(p->session,emp_lv,p->sess_len);
	UNPKG_H_UINT32(p->session,emoney,p->sess_len);

	p->sess_len = 0;
	int i = sizeof(protocol_t);
	PKG_UINT32(msg,userid,i);
	PKG_UINT32(msg,pet_id,i);
	PKG_STR(msg,p_name,i,16);
	PKG_UINT32(msg,pcolor,i);
	PKG_UINT32(msg,plevel,i);
	PKG_UINT32(msg,pskill,i);
	PKG_UINT32(msg,emp_lv,i);//employe level
	PKG_UINT32(msg,emoney,i);//employe money
	PKG_UINT32(msg,0,	   i);
	PKG_UINT32(msg,second,i);

	p->tiles->mmdu[0].shop.employed_count++;
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);

	if(userid != 10000) {
		SET_PET_EMPLOYED(p->followed);
		db_set_pet_arg(NULL,PET_FLAG,PET_STATUS_EMPLOYED,pet_id,userid);
		db_set_pet_employed_uid(NULL, userid, pet_id, GET_UID_SHOP_MAP(p->tiles->id));
		p->followed = NULL;
		if(GET_UID_SHOP_MAP(p->tiles->id) != p->id) {
			char buff[2048] = {0};
#ifndef TW_VER
			sprintf(buff,"%u的拉姆在你的餐厅中打工，要好好照顾它哦！",p->id);
			send_postcard("摩摩商会",0,GET_UID_SHOP_MAP(p->tiles->id),1000127,buff,0);
#else
			sprintf(buff,"%u的拉姆在你的餐廳中打工，要好好照顧它哦！",p->id);
			send_postcard("摩摩商會",0,GET_UID_SHOP_MAP(p->tiles->id),1000127,buff,0);
#endif
		}
	} else {
		p->yxb -= emoney;
		p->tiles->mmdu[0].shop.money -= emoney;
		notify_shop_info_change(p);
	}

	return 0;
}

int del_employee_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	uint32_t userid;
	uint32_t pet_id;

	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	CHECK_BODY_LEN(len,8);

	UNPKG_UINT32(body, userid, i);
	UNPKG_UINT32(body, pet_id, i);

	if(GET_UID_SHOP_MAP(p->tiles->id) != p->id) {
		userid = p->id;
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session,userid,p->sess_len);
	PKG_H_UINT32(p->session,pet_id,p->sess_len);

	return send_request_to_db(SVR_PROTO_DEL_EMPLOYEE, p, p->sess_len, p->session, GET_UID_SHOP_MAP(p->tiles->id));
}

int del_employee_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t userid;
	uint32_t pet_id;
	uint32_t emtime;
	uint32_t pet_level;
	uint32_t pet_skill;

	CHECK_BODY_LEN(len,12);

	int i = 0;
	UNPKG_H_UINT32(buf, emtime, i);
	UNPKG_H_UINT32(buf, pet_level, i);
	UNPKG_H_UINT32(buf, pet_skill, i);

	p->sess_len = 0;
	UNPKG_H_UINT32(p->session, userid, p->sess_len);
	UNPKG_H_UINT32(p->session, pet_id, p->sess_len);

	i = sizeof(protocol_t);
	PKG_UINT32(msg,userid,i);
	PKG_UINT32(msg,pet_id,i);

	p->tiles->mmdu[0].shop.employed_count--;
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);

	if(userid != 10000) {
		db_set_pet_arg(NULL,PET_FLAG,0,pet_id,userid);
		db_set_pet_employed_uid(NULL,userid,pet_id,0);

		char buff[2048] = {0};
		pet_t *pet = get_pet(userid,pet_id);
		if(pet) RESET_PET_EMPLOYED(pet);
		if(GET_UID_SHOP_MAP(p->tiles->id) != p->id) {//send msg to host
#ifndef TW_VER
			sprintf(buff,"%u的拉姆在你餐厅中打工结束，已经回到它主人身边。",p->id);
			send_postcard("摩摩商会",0,GET_UID_SHOP_MAP(p->tiles->id),1000133,buff,0);
#else
			sprintf(buff,"%u的拉姆在你餐廳中打工結束，已經回到它主人身邊。",p->id);
			send_postcard("摩摩商會",0,GET_UID_SHOP_MAP(p->tiles->id),1000133,buff,0);
#endif
		}
		if(GET_UID_SHOP_MAP(p->tiles->id) != userid){//send msg to user
#ifndef TW_VER
			if(emtime > 3600) {
				int salary = 500;
				if(pet_level > 1 && emtime + 10 >= get_employee_limit_user_lahm(pet_level, pet_skill)) {
					salary = get_employee_salary_user_lahm(pet_level,pet_skill);
				}
				db_change_xiaome(NULL,salary,0,0,userid);
				sprintf(buff,"亲爱的主人，我从%u的餐厅打工回来，总共获得了%d摩尔豆，我干的很不错吧！",p->id,salary);
			} else {
				sprintf(buff,"亲爱的主人，%u的餐厅不需要我的帮忙，所以我提前回来了，带我去其它餐厅看看吧！",p->id);
			}
			send_postcard("摩摩商会",0,userid,1000128,buff,0);
#else
			if(emtime > 3600) {
				int salary = 500;
				if(pet_level > 1 && emtime + 10 >= get_employee_limit_user_lahm(pet_level, pet_skill)) {
					salary = get_employee_salary_user_lahm(pet_level,pet_skill);
				}
				db_change_xiaome(NULL,salary,0,0,userid);
				sprintf(buff,"親愛的主人，我從%u的餐廳打工回來，總共獲得了%d摩爾豆，我有努力工作喔！",p->id,salary);
			} else {
				sprintf(buff,"親愛的主人，%u的餐廳不需要我的幫忙，所以我提前回來了，帶我去其它餐廳看看吧！",p->id);
			}
			send_postcard("摩摩商會",0,userid,1000128,buff,0);
#endif
		}
	}

	return 0;
}

int cooking_dish_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	uint32_t dish_id;
	uint32_t location;

	int tm_hour = get_now_tm()->tm_hour;
	if(tm_hour < 6) {
		WARN_LOG("WAIGUA:%u:%d",p->id,p->waitcmd);
		return send_to_self_error(p, p->waitcmd, -ERR_restuarant_closed, 1);
	}

	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	if(p->id != GET_UID_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_shop_not_yours, 1);
	}

	CHECK_BODY_LEN(len,8);
	UNPKG_UINT32(body, dish_id, i);
	UNPKG_UINT32(body, location, i);

	item_t *in_style = get_item_prop(p->tiles->mmdu[0].shop.inner_style);
	if(!in_style) {
		ERROR_RETURN(("inner_style=%d",p->tiles->mmdu[0].shop.inner_style),-1);
	}

	if(location < 1 || location > in_style->u.shop_tag.stoves) {
		DEBUG_LOG("LOCATION = %d stoves=%d ERR",location,in_style->u.shop_tag.stoves);
		return send_to_self_error(p, p->waitcmd, -ERR_shop_location_limit, 1);
	}

	item_kind_t *ik_style = find_kind_of_item(dish_id);
	if(!ik_style || ik_style->kind != RESTAURANT_ITEM_KIND) {
		DEBUG_LOG("RESTAURANT_ITEM_KIND");
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}

	item_t *itm = get_item(ik_style,dish_id);
	if(!itm || itm->layer == 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}

	if(!check_condtion(p,itm->u.food_tag.channel, itm->u.food_tag.condition)) {
		DEBUG_LOG("check_condtion id=%u item=%u ch=%d cond=%d",p->id,itm->id,itm->u.food_tag.channel, itm->u.food_tag.condition);
		return send_to_self_error(p, p->waitcmd, -ERR_food_item_locked, 1);
	}

	if(p->yxb < itm->price) {
		DEBUG_LOG("cooking_dish_cmd(%u, %d < %d)",p->id,p->yxb,itm->price);
		return send_to_self_error(p, p->waitcmd, -ERR_not_enough_xiaomee, 1);
	}

	struct req_3186 *req = (void*)p->session;

	req->addexp = 0;
	req->id = dish_id;
	req->price = itm->price;
	req->location = location;
	req->sell_price = itm->sell_price;
	req->count = itm->u.food_tag.count;
	req->channel = itm->u.food_tag.channel;
	req->condition = itm->u.food_tag.condition;
	memcpy(req->food,itm->u.food_tag.food,6 * sizeof(uint32_t));

	return send_request_to_db(SVR_PROTO_COOKING_DISH, p, sizeof(struct req_3186), p->session, p->id);
}

int cooking_dish_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i = 0;
	uint32_t dish_id;
	uint32_t dish_idx;
	uint32_t dish_loca;
	uint32_t dish_state;
	uint32_t money;
	uint32_t addexp;

	CHECK_BODY_LEN(len,8);
	UNPKG_H_UINT32(buf, dish_idx, i);
	UNPKG_H_UINT32(buf, dish_state, i);

	uint32_t msgbuff[] = {1,p->id};
	msglog(statistic_logfile, 0x02020009, now.tv_sec, msgbuff, sizeof(msgbuff));

	struct req_3186 *req = (void*)p->session;

	dish_id = req->id;
	dish_loca = req->location;
	money = req->price;
	addexp = req->addexp;

	p->yxb -= money;
	p->tiles->mmdu[0].shop.exp += addexp;
	p->tiles->mmdu[0].shop.money -= money;

	p->sess_len = 0;
	i = sizeof(protocol_t);
	PKG_UINT32(msg,dish_id,i);
	PKG_UINT32(msg,dish_idx,i);
	PKG_UINT32(msg,dish_loca,i);
	PKG_UINT32(msg,dish_state,i);
	PKG_UINT32(msg,p->yxb,i);

	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);
	return notify_shop_info_change(p);
}

int change_dish_state_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	uint32_t dish_id;
	uint32_t dish_idx;

	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	if(p->id != GET_UID_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_shop_not_yours, 1);
	}

	CHECK_BODY_LEN(len,8);
	UNPKG_UINT32(body, dish_id, i);
	UNPKG_UINT32(body, dish_idx, i);

	p->sess_len = 0;
	PKG_H_UINT32(p->session, dish_idx, p->sess_len);
	PKG_H_UINT32(p->session, dish_id,  p->sess_len);
	return send_request_to_db(SVR_PROTO_CHANGE_DISH_STATE, p, p->sess_len, p->session, p->id);
}

int change_dish_state_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i = 0;
	uint32_t dish_id;
	uint32_t dish_idx;
	uint32_t dish_loca;
	uint32_t dish_state;

	CHECK_BODY_LEN(len,12);
	response_proto_head(p, p->waitcmd,0);

	UNPKG_H_UINT32(buf, dish_idx, i);
	UNPKG_H_UINT32(buf, dish_state, i);
	UNPKG_H_UINT32(buf, dish_loca, i);

	i = 0;
	UNPKG_H_UINT32(p->session, dish_idx, i);
	UNPKG_H_UINT32(p->session, dish_id, i);

	p->sess_len = 0;
	i = sizeof(protocol_t);
	PKG_UINT32(msg,dish_id,i);
	PKG_UINT32(msg,dish_idx,i);
	PKG_UINT32(msg,dish_loca,i);
	PKG_UINT32(msg,dish_state,i);
	PKG_UINT32(msg,p->yxb,i);
	init_proto_head(msg, PROTO_COOKING_DISH, i);
	send_to_map(p, msg, i, 0);
	return 0;
}

int put_dish_in_box_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	uint32_t dish_id;
	uint32_t dish_idx;
	uint32_t dish_loca;
	uint32_t dish_to;
	uint32_t addexp;

	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	if(p->id != GET_UID_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_shop_not_yours, 1);
	}

	CHECK_BODY_LEN(len,16);
	UNPKG_UINT32(body, dish_id, i);
	UNPKG_UINT32(body, dish_idx, i);
	UNPKG_UINT32(body, dish_loca, i);
	UNPKG_UINT32(body, dish_to, i);

	item_t *in_style = get_item_prop(p->tiles->mmdu[0].shop.inner_style);
	if(!in_style) {
		ERROR_RETURN(("inner_style=%d",p->tiles->mmdu[0].shop.inner_style),-1);
	}

	if( dish_loca < 1 || dish_loca > in_style->u.shop_tag.stoves ||
		dish_to < 51 || dish_to > in_style->u.shop_tag.foodtables + 50) {
		DEBUG_LOG("LOCATION = %d stoves=%d dish_to=%d %d ERR",dish_loca,in_style->u.shop_tag.stoves,dish_to,in_style->u.shop_tag.foodtables);
		return send_to_self_error(p, p->waitcmd, -ERR_shop_location_limit, 1);
	}

	item_kind_t *ik_style = find_kind_of_item(dish_id);
	if(!ik_style || ik_style->kind != RESTAURANT_ITEM_KIND) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}

	item_t *itm = get_item(ik_style,dish_id);
	if(!itm) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}

	addexp = itm->u.food_tag.addexp;
	if(p->tiles->mmdu[0].shop.money < -100000) {
		addexp = 0;
	} else if(p->tiles->mmdu[0].shop.money < -50000) {
		addexp /= 2;
	}

	struct req_3187 *req = (void*)p->session;

	req->dish_idx = dish_idx;
	req->dish_id = dish_id;
	req->dish_to = dish_to;
	req->timer = itm->u.food_tag.timer;
	req->addexp = addexp;
	req->addcount = 0;
	req->timeout = itm->u.food_tag.timer + itm->u.food_tag.bad_timer;
	req->dish_local = dish_loca;

	DEBUG_LOG("%u bad_timer=%d", p->id, req->timeout);

	if(itm->u.food_tag.star[0]) {
		DEBUG_LOG("put dish have star");
		return send_request_to_db(SVR_PROTO_GET_DISH_LEVEL, p, 4, &dish_id, p->id);
	}
	return send_request_to_db(SVR_PROTO_PUT_DISH_IN_BOX, p, sizeof(struct req_3187)-4, p->session, p->id);
}

int put_dish_in_box_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t dish_id;
	uint32_t dish_idx;
	uint32_t dish_loca;
	uint32_t dish_to;
	uint32_t dish_toidx;

	uint32_t addexp;
	uint32_t addcount;
	uint32_t dish_total;
	uint32_t this_total;

	int i = 0;
	CHECK_BODY_LEN(len,12);
	UNPKG_H_UINT32(buf, dish_total, i);
	UNPKG_H_UINT32(buf, dish_toidx, i);
	UNPKG_H_UINT32(buf, this_total, i);

	if(dish_total >= 100) {
		if(dish_total >= 500) {
			if(dish_total >= 1000) {
				if(dish_total >= 2000) {
					set_honor_bit(p,4);
				} else {
					set_honor_bit(p,3);
				}
			} else {
				set_honor_bit(p,2);
			}
		} else {
			set_honor_bit(p,1);
		}
	}

	int last_level = calc_dish_total_level(p->tiles->mmdu[0].shop.dish_total);
	int this_level = calc_dish_total_level(dish_total);
	if(last_level < this_level) {
		uint32_t leavetag[] = {0,1};
		uint32_t entertag[] = {1,0};
		msglog(statistic_logfile, 0x02021101 + last_level, now.tv_sec, leavetag, sizeof(leavetag));
		msglog(statistic_logfile, 0x02021101 + this_level, now.tv_sec, entertag, sizeof(entertag));
	}

	uint32_t msgbuff[] = {1,p->id};
	msglog(statistic_logfile, 0x02020008, now.tv_sec, msgbuff, sizeof(msgbuff));

	struct req_3187 *req = (void*)p->session;

	dish_idx = req->dish_idx;
	dish_id  = req->dish_id;
	dish_to  = req->dish_to;
	addexp   = req->addexp;
	addcount = req->addcount;
	dish_loca= req->dish_local;

	item_t *itm = get_item_prop(dish_id);
	if(itm) {
		addcount += itm->u.food_tag.count;
	}

	i = sizeof(protocol_t);
	PKG_UINT32(msg,dish_id,i);
	PKG_UINT32(msg,dish_toidx,i);
	PKG_UINT32(msg,dish_loca,i);
	PKG_UINT32(msg,dish_to,i);
	PKG_UINT32(msg,this_total,i);
	PKG_UINT32(msg,addexp,i);
	PKG_UINT32(msg,addcount,i);

	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);

	p->tiles->mmdu[0].shop.exp += addexp;
	p->tiles->mmdu[0].shop.dish_total = dish_total;
	return notify_shop_info_change(p);
}

int eat_dish_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	uint32_t userid;
	uint32_t pet_id;
	uint32_t dishid;
	uint32_t index;
	uint32_t location;

	int tm_hour = get_now_tm()->tm_hour;
	if( tm_hour < 6 ) {
		WARN_LOG("WAIGUA:%u:%d",p->id,p->waitcmd);
		return send_to_self_error(p, p->waitcmd, -ERR_restuarant_closed, 1);
	}

	int tm_wday = get_now_tm()->tm_wday;
	if(!is_holiday(tm_wday)) {
		if( tm_hour < 12 || tm_hour >= 21 ||
			(tm_hour >= 14 && tm_hour < 18) ) {
			WARN_LOG("WAIGUA:%u:%d",p->id,p->waitcmd);
			return send_to_self_error(p, p->waitcmd, -ERR_restuarant_closed, 1);
		}
	}

	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	if(p->id != GET_UID_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_shop_not_yours, 1);
	}

	CHECK_BODY_LEN(len,20);
	UNPKG_UINT32(body, userid, i);
	UNPKG_UINT32(body, pet_id, i);
	UNPKG_UINT32(body, dishid, i);
	UNPKG_UINT32(body, index,  i);
	UNPKG_UINT32(body, location,  i);

	item_t *in_style = get_item_prop(p->tiles->mmdu[0].shop.inner_style);
	if(!in_style) {
		ERROR_RETURN(("inner_style=%d",p->tiles->mmdu[0].shop.inner_style),-1);
	}

	if(location < 101 || location > in_style->u.shop_tag.tables + 100) {
		DEBUG_LOG("LOCATION = %d tables=%d ERR",location,in_style->u.shop_tag.tables);
		return send_to_self_error(p, p->waitcmd, -ERR_shop_location_limit, 1);
	}

	item_kind_t *ik_style = find_kind_of_item(dishid);
	if(!ik_style || ik_style->kind != RESTAURANT_ITEM_KIND) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}

	item_t *itm = get_item(ik_style,dishid);
	if(!itm) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session,userid,p->sess_len);
	PKG_H_UINT32(p->session,pet_id,p->sess_len);
	PKG_H_UINT32(p->session,index,p->sess_len);
	PKG_H_UINT32(p->session,dishid,p->sess_len);
	PKG_H_UINT32(p->session,itm->sell_price,p->sess_len);
	PKG_H_UINT32(p->session,location,p->sess_len);

	return send_request_to_db(SVR_PROTO_EAT_DISH, p, p->sess_len - 4, p->session, GET_UID_SHOP_MAP(p->tiles->id));
}

int eat_dish_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t userid;
	uint32_t pet_id;
	uint32_t dishid;
	uint32_t index;
	uint32_t money;
	uint32_t location;
	uint32_t zero_count;
	uint32_t count_left;

	int i = 0;
	CHECK_BODY_LEN(len,8);
	UNPKG_H_UINT32(buf,zero_count,i);
	UNPKG_H_UINT32(buf,count_left,i);

	p->sess_len = 0;
	UNPKG_H_UINT32(p->session, userid, p->sess_len);
	UNPKG_H_UINT32(p->session, pet_id, p->sess_len);
	UNPKG_H_UINT32(p->session, index,  p->sess_len);
	UNPKG_H_UINT32(p->session, dishid, p->sess_len);
	UNPKG_H_UINT32(p->session, money, p->sess_len);
	UNPKG_H_UINT32(p->session, location, p->sess_len);

	p->yxb += money;
	p->tiles->mmdu[0].shop.money += money;

	i = sizeof(protocol_t);
	PKG_UINT32(msg,userid,i);
	PKG_UINT32(msg,pet_id,i);
	PKG_UINT32(msg,dishid,i);
	PKG_UINT32(msg,index, i);
	PKG_UINT32(msg,location, i);
	PKG_UINT32(msg,p->yxb, i);
	PKG_UINT32(msg,zero_count,i);
	PKG_UINT32(msg,count_left,i);

	init_proto_head(msg, p->waitcmd, i);
	send_to_self(p, msg, i, 1);

	if(p->tiles->mmdu[0].shop.money >= 10000)
		set_honor_bit(p,5);
	if(p->tiles->mmdu[0].shop.money >= 50000)
		set_honor_bit(p,6);
	if(p->tiles->mmdu[0].shop.money >= 200000)
		set_honor_bit(p,7);
	if(p->tiles->mmdu[0].shop.money >= 500000)
		set_honor_bit(p,8);

	return notify_shop_info_change(p);
}

int clean_dish_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	uint32_t dish_id;
	uint32_t index;
	uint32_t location;

	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	if(p->id != GET_UID_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_shop_not_yours, 1);
	}

	CHECK_BODY_LEN(len,12);
	UNPKG_UINT32(body, dish_id, i);
	UNPKG_UINT32(body, index,  i);
	UNPKG_UINT32(body, location,  i);

	item_t *in_style = get_item_prop(p->tiles->mmdu[0].shop.inner_style);
	if(!in_style) {
		ERROR_RETURN(("inner_style=%d",p->tiles->mmdu[0].shop.inner_style),-1);
	}

	if( location < 1 || location > in_style->u.shop_tag.foodtables + 50
		|| (location > in_style->u.shop_tag.stoves && location < 51) ) {
		DEBUG_LOG("LOCATION = %d stoves=%d foodtables=%d ERR",location,in_style->u.shop_tag.stoves,in_style->u.shop_tag.foodtables);
		return send_to_self_error(p, p->waitcmd, -ERR_shop_location_limit, 1);
	}

	item_kind_t *ik_style = find_kind_of_item(dish_id);
	if(!ik_style || ik_style->kind != RESTAURANT_ITEM_KIND) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}

	item_t *itm = get_item(ik_style,dish_id);
	if(!itm) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}

	if(p->yxb < 100) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_enough_xiaomee, 1);
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session,index , p->sess_len);
	PKG_H_UINT32(p->session,dish_id,p->sess_len);
	PKG_H_UINT32(p->session,100,p->sess_len);

	i = send_request_to_db(SVR_PROTO_CLEAN_DISH, p, p->sess_len, p->session, p->id);

	p->sess_len = 8;
	PKG_H_UINT32(p->session,location, p->sess_len);
	return i;
}

int clean_dish_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t dishid;
	uint32_t index;
	uint32_t location;

	p->yxb -= 100;
	p->tiles->mmdu[0].shop.money -= 100;

	p->sess_len = 0;
	UNPKG_H_UINT32(p->session, index,  p->sess_len);
	UNPKG_H_UINT32(p->session, dishid, p->sess_len);
	UNPKG_H_UINT32(p->session, location, p->sess_len);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg,dishid,i);
	PKG_UINT32(msg,index, i);
	PKG_UINT32(msg,location, i);
	PKG_UINT32(msg,p->yxb, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);
	return notify_shop_info_change(p);
}

int get_shop_board_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i=0;
	uint32_t shop_uid;
	uint32_t shop_type;

	CHECK_BODY_LEN(len,8);
	UNPKG_UINT32(body,shop_uid,i);
	UNPKG_UINT32(body,shop_type,i);

	p->sess_len = 0;
	PKG_H_UINT32(p->session,shop_uid,p->sess_len);
	PKG_H_UINT32(p->session,shop_type,p->sess_len);

	return send_request_to_db(SVR_PROTO_GET_BOARD_INFO, p, 4, p->session + 4, shop_uid);
}

int get_shop_board_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t shop_uid;
	uint32_t shop_type;
	uint32_t off_money;
	uint32_t off_evaluate;
	uint32_t level_5_count = 0;
	uint32_t myevent_count = 0;

	CHECK_BODY_LEN(len,16);

	p->sess_len = 0;
	UNPKG_H_UINT32(p->session,shop_uid,p->sess_len);
	UNPKG_H_UINT32(p->session,shop_type,p->sess_len);

	int i = 0;
	UNPKG_H_UINT32(buf,off_money,i);
	UNPKG_H_UINT32(buf,off_evaluate,i);
	UNPKG_H_UINT32(buf,level_5_count,i);
	UNPKG_H_UINT32(buf,myevent_count,i);

	i = sizeof(protocol_t);
	PKG_UINT32(msg,shop_uid,i);
	PKG_UINT32(msg,shop_type, i);
	PKG_UINT32(msg,off_money, i);
	PKG_UINT32(msg,off_evaluate, i);
	PKG_UINT32(msg,level_5_count, i);
	PKG_UINT32(msg,myevent_count, i);

	uint32_t info_m[] = {p->id};
	if(level_5_count > 15) level_5_count = 15;
	msglog(statistic_logfile, 0x02025000 + level_5_count, now.tv_sec, info_m, sizeof(info_m));

	init_proto_head(msg,p->waitcmd,i);
	return send_to_self(p,msg,i,1);
}

int get_shop_item_list_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i=0;
	uint32_t kind_id;
	uint32_t kind_layer;

	CHECK_BODY_LEN(len,8);
	UNPKG_UINT32(body,kind_id,i);
	UNPKG_UINT32(body,kind_layer,i);

	if(kind_id != 25 && kind_id != 26) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}

	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	if(GET_UID_SHOP_MAP(p->tiles->id) != p->id) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_shop_not_yours, 1);
	}

	int enable = 0;
	int bytes = sizeof(protocol_t);
	item_kind_t *ik = get_item_kind(kind_id);
	if(kind_id == 25) {
		int cnt = 0;
		int pos = bytes;
		PKG_UINT32(msg,ik->count,bytes);
		for(i=0; i < ik->count; i++) {
			if( ik->items[i].layer != kind_layer ||
				ik->items[i].u.shop_tag.type != 31)
				continue;
			cnt++;
			enable = check_condtion(p, ik->items[i].u.shop_tag.channel, ik->items[i].u.shop_tag.condition);
			PKG_UINT32(msg,ik->items[i].id,bytes);
			PKG_UINT32(msg,enable,bytes);
		}
		PKG_UINT32(msg,cnt,pos);
	} else {
		int pos = bytes;
		int max_order = 0;
		for(i=0; i < ik->count; i++) {
			if( ik->items[i].layer == 0)
				continue;
			enable = check_condtion(p, ik->items[i].u.food_tag.channel, ik->items[i].u.food_tag.condition);
			bytes = pos + 4 + (ik->items[i].layer - 1) * 8;
			PKG_UINT32(msg,ik->items[i].id,bytes);
			PKG_UINT32(msg,enable,bytes);
			if(ik->items[i].layer > max_order) {
				max_order = ik->items[i].layer;
			}
		}
		PKG_UINT32(msg,max_order,pos);
		bytes = pos + 4 + max_order * 8;
	}

	init_proto_head(msg,p->waitcmd,bytes);
	return send_to_self(p,msg,bytes,1);
}

int get_shop_item_lock_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i=0;
	uint32_t itemid;
	CHECK_BODY_LEN(len,4);
	UNPKG_UINT32(body,itemid,i);

	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	if(GET_UID_SHOP_MAP(p->tiles->id) != p->id) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_shop_not_yours, 1);
	}

	int enable = 0;
	item_kind_t *ik = find_kind_of_item(itemid);
	if(!ik) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}
	item_t *itm = get_item(ik, itemid);
	if(!itm) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}

	switch(ik->kind) {
	case STREET_ITEM_KIND:
		if(itm->u.shop_tag.type == 31) {
			enable = check_condtion(p, itm->u.shop_tag.channel,itm->u.shop_tag.condition);
		}
		break;
	case RESTAURANT_ITEM_KIND:
		if(ik->items[i].layer == 0) {
			enable = check_condtion(p, itm->u.shop_tag.channel,itm->u.shop_tag.condition);
		}
		break;
	default:
		break;
	}

	int bytes = sizeof(protocol_t);
	PKG_UINT32(msg,itemid,bytes);
	PKG_UINT32(msg,enable,bytes);
	init_proto_head(msg,p->waitcmd,bytes);
	return send_to_self(p,msg,bytes,1);
}

int sub_shop_evaluate_cmd(sprite_t * p,const uint8_t * body,int len)
{
	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	if(p->id != GET_UID_SHOP_MAP(p->tiles->id)) {
		int i = sizeof(protocol_t);
		PKG_UINT32(msg,p->tiles->mmdu[0].shop.evaluate,i);
		init_proto_head(msg,p->waitcmd,i);
		return send_to_self(p,msg,i,1);
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session,1,p->sess_len);
	return send_request_to_db(SVR_PROTO_SUB_EVALUATE, p, p->sess_len,p->session,p->id);
}

int sub_shop_evaluate_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i=0;
	uint32_t evaluate;

	CHECK_BODY_LEN(len,4);
	UNPKG_H_UINT32(buf,evaluate,i);
	p->tiles->mmdu[0].shop.evaluate = evaluate;

	i = sizeof(protocol_t);
	PKG_UINT32(msg,evaluate,i);
	init_proto_head(msg,p->waitcmd,i);
	return send_to_self(p,msg,i,1);
}

int get_shop_count_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	uint32_t shop_uid;
	uint32_t shop_type;

	CHECK_BODY_LEN(len,8);

	UNPKG_UINT32(body,shop_uid,i);
	UNPKG_UINT32(body,shop_type,i);

	if(!shop_uid || IS_NPC_ID(shop_uid) || IS_GUEST_ID(shop_uid)) {
		int i = sizeof(protocol_t);
		PKG_UINT32(msg,shop_uid,i);
		PKG_UINT32(msg,0,i);
		PKG_UINT32(msg,0,i);
		PKG_UINT32(msg,shop_type,i);
		init_proto_head(msg,p->waitcmd,i);
		return send_to_self(p,msg,i,1);
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session,p->id,p->sess_len);
	PKG_H_UINT32(p->session,shop_type,p->sess_len);
	return send_request_to_db(SVR_PROTO_GET_SHOP_COUNT, p, p->sess_len, p->session, shop_uid);
}

int get_shop_count_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t shop_type;
	uint32_t friendid;

	CHECK_BODY_LEN(len,8);
	p->sess_len = 0;
	UNPKG_H_UINT32(p->session,friendid,p->sess_len);
	UNPKG_H_UINT32(p->session,shop_type,p->sess_len);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg,id,i);
	PKG_UINT32(msg,*(uint32_t*)buf,i);
	PKG_UINT32(msg,*(uint32_t*)(buf + 4),i);
	PKG_UINT32(msg,shop_type,i);
	init_proto_head(msg,p->waitcmd,i);
	return send_to_self(p,msg,i,1);
}

int get_pet_employed_list_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_BODY_LEN(len,0);

	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	return send_request_to_db(SVR_PROTO_GET_PET_EMPLOYED_LIST, p, 0, NULL, GET_UID_SHOP_MAP(p->tiles->id));
}

int get_pet_employed_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i = 0,j;
	uint32_t count;
	uint32_t petid;
	uint32_t emuid;

	int pkgsize = sizeof(protocol_t);

	CHECK_BODY_LEN_GE(len,4);
	UNPKG_H_UINT32(buf,count,i);
	PKG_UINT32(msg,count,pkgsize);

	CHECK_BODY_LEN(len,4 + count * 8);
	for(j = 0; j < count; j++) {
		UNPKG_H_UINT32(buf,petid,i);
		UNPKG_H_UINT32(buf,emuid,i);

		PKG_UINT32(msg,petid,pkgsize);
		PKG_UINT32(msg,emuid,pkgsize);
	}

	init_proto_head(msg,p->waitcmd,pkgsize);
	return send_to_self(p, msg, pkgsize, 1);
}

int get_shop_honor_list_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i=0;
	CHECK_BODY_LEN(len,0);
	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	int bytes = sizeof(protocol_t);
	PKG_UINT32(msg,16,bytes);//now only send first three honors
	for(i=1; i <= 16; i++) {
		PKG_UINT32(msg,i,bytes);
		if(test_bit_on(p->tiles->mmdu[0].shop.honors,8,i)) {
			PKG_UINT32(msg,1,bytes);
		} else {
			PKG_UINT32(msg,0,bytes);
		}
	}

	init_proto_head(msg,p->waitcmd,bytes);
	return send_to_self(p,msg,bytes,1);
}

int get_friend_list_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	uint32_t uid;

	CHECK_BODY_LEN(len,4);
	UNPKG_UINT32(body,uid,i);

	return send_request_to_db(SVR_PROTO_GET_FRIEND,p,0, NULL, uid);
}

int get_friend_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i = 0,j;
	uint32_t count;
	uint32_t friends;
	int pkg_size = sizeof(protocol_t);
	CHECK_BODY_LEN_GE(len,4);
	UNPKG_H_UINT32(buf,count,i);
	PKG_UINT32(msg,count,pkg_size);

	for(j=0;j < count;j++) {
		UNPKG_H_UINT32(buf,friends,i);
		PKG_UINT32(msg,friends,pkg_size);
	}

	init_proto_head(msg,p->waitcmd,pkg_size);
	send_to_self(p,msg,pkg_size,1);
	return 0;
}

int get_restuarant_event_cmd(sprite_t * p,const uint8_t * body,int len)
{
	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	if(p->dining_flag == 0) {
		response_proto_uint32(p,p->waitcmd,0,0);
		return 0;
	}

	uint32_t type = 170;
	if(GET_UID_SHOP_MAP(p->tiles->id) == p->id) {
		type = 172;
		if(p->tiles->mmdu[0].shop.dish_bad) {
			type = 171;
		}
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session,type,p->sess_len);

	return db_chk_if_sth_done(p,&type);
}

int get_restuarant_event_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	p->restaurant_evt = 0;
	CHECK_BODY_LEN(len,4);

	switch(*(uint32_t*)p->session) {
		case 170:
			if(*(uint32_t*)buf < 5) {
				int val = rand() % 100;
				if(val < 40) {
					val = val % 8;
					p->restaurant_evt = val + 1;
				}
			}
			break;
		case 171:
			if(*(uint32_t*)buf < 2) {
				int val = rand() % 100;
				if(val < 50) {
					p->restaurant_evt = 100;
				}
			}
			break;
		case 172:
			if(*(uint32_t*)buf < 3) {
				int val = rand() % 100;
				if(val < 40) {
					val = val % 8;
					p->restaurant_evt = val + 200;
				}
			}
			break;
	}

	int bytes = sizeof(protocol_t);
	PKG_UINT32(msg, p->restaurant_evt, bytes);
	init_proto_head(msg,p->waitcmd,bytes);
	return send_to_self(p,msg,bytes,1);
}

int set_restuarant_event_cmd(sprite_t * p,const uint8_t * body,int len)
{
	uint32_t solution;
	uint32_t type = 170;

	CHECK_BODY_LEN(len,4);

	int i = 0;
	UNPKG_UINT32(body,solution,i);
	p->sess_len = 0;
	PKG_H_UINT32(p->session,solution, p->sess_len);

	if(p->restaurant_evt == 0) {
		ERROR_RETURN(("NO RESTAURANT EVENT FOR %u",p->id),-1);
	}

	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	if(GET_UID_SHOP_MAP(p->tiles->id) == p->id ) {
		if(p->restaurant_evt == 100) {
			p->tiles->mmdu[0].shop.dish_bad = 0;
			type = 171;
			return db_set_sth_done(p, type, 2, p->id);
		} else {
			type = 172;
			return db_set_sth_done(p, type, 3, p->id);
		}
	}

	return db_set_sth_done(p, type, 5, p->id);
}

int set_restuarant_event_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t event;
	uint32_t solution;

	event = p->restaurant_evt;

	p->sess_len = 0;
	p->restaurant_evt = 0;
	UNPKG_H_UINT32(p->session,solution,p->sess_len);

	uint32_t msgbuff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0405C1A2, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	switch(event) {
		case 1:
			switch(solution)
			{
				case 1:
					p->yxb += 50;
					db_change_shop_money(NULL,50,p->id);
					db_buy_itm_freely(NULL,p->id,190654,2);
					break;
				case 2:
					p->yxb += 5;
					db_change_shop_money(NULL,5,p->id);
					break;
			}
			break;
		case 2:
			switch(solution)
			{
				case 1:
					db_change_shop_exp(NULL,10,p->id);
					break;
				case 2:
					db_change_shop_exp(NULL,10,p->id);
					break;
			}
			break;
		case 3:
			switch(solution)
			{
				case 1:
					p->yxb += 100;
					db_change_shop_money(NULL,100,p->id);
					break;
				case 2:
					db_change_shop_exp(NULL,5,p->id);
					break;
			}
			break;
		case 4:
			switch(solution)
			{
				case 1:
					db_buy_itm_freely(NULL,p->id,190653,2);
					db_buy_itm_freely(NULL,p->id,190141,1);
					break;
				case 2:
					db_buy_itm_freely(NULL,p->id,190653,2);
					db_buy_itm_freely(NULL,p->id,190141,1);
					break;
			}
			break;
		case 5:
			switch(solution)
			{
				case 1:
					p->yxb += 200;
					db_change_shop_money(NULL,200,p->id);
					break;
				case 2:
					db_change_shop_exp(NULL,15,p->id);
					break;
			}
			break;
		case 6:
			switch(solution)
			{
				case 1:
					p->yxb += 100;
					db_change_shop_money(NULL,100,p->id);
					break;
				case 2:
					db_change_shop_exp(NULL,15,p->id);
					db_buy_itm_freely(NULL,p->id,190142,3);
					break;
			}
			break;
		case 7:
			switch(solution)
			{
				case 1:
					p->yxb += 80;
					db_change_shop_money(NULL,80,p->id);
					break;
				case 2:
					db_buy_itm_freely(NULL,p->id,190196,5);
					break;
			}
			break;
		case 8:
			switch(solution)
			{
				case 1:
					p->yxb += 50;
					db_change_shop_money(NULL,50,p->id);
					break;
				case 2:
					db_change_shop_exp(NULL,10,p->id);
					db_buy_itm_freely(NULL,p->id,190141,3);
					break;
			}
			break;
		case 100:
			return send_request_to_db(SVR_PROTO_GET_USER_DISH_ALL,p,0,NULL,p->id);
		case 200:
			switch(solution)
			{
				case 1:
					break;
				case 2:
					db_buy_itm_freely(NULL,p->id,190309,1);
					db_buy_itm_freely(NULL,p->id,190660,2);
					break;
			}
			break;
		case 201:
			switch(solution)
			{
				case 1:
				case 2:
					break;
				case 3:
					db_buy_itm_freely(NULL,p->id,190309,2);
					db_buy_itm_freely(NULL,p->id,1230005,1);
					break;
			}
			break;
		case 202:
			switch(solution)
			{
				case 1:
				case 2:
					break;
				case 3:
					db_buy_itm_freely(NULL,p->id,190141,3);
					db_buy_itm_freely(NULL,p->id,1230043,1);
					break;
			}
			break;
		case 203:
			switch(solution)
			{
				case 1:
				case 2:
					break;
				case 3:
					db_buy_itm_freely(NULL,p->id,190187,2);
					db_buy_itm_freely(NULL,p->id,1230021,1);
					break;
			}
			break;
		case 204:
			switch(solution)
			{
				case 1:
					break;
				case 2:
					db_buy_itm_freely(NULL,p->id,190671,1);
					break;
			}
			break;
		case 205:
			switch(solution)
			{
				case 1:
					break;
				case 2:
					p->yxb += 100;
					db_change_shop_money(NULL,100,p->id);
					break;
				case 3:
					db_buy_itm_freely(NULL,p->id,190187,3);
					break;
			}
			break;
		case 206:
			switch(solution)
			{
				case 1:
					break;
				case 2:
					p->yxb += 50;
					db_change_shop_money(NULL,50,p->id);
					db_buy_itm_freely(NULL,p->id,190141,3);
					break;
				case 3:
					db_change_shop_exp(NULL,20,p->id);
					break;
			}
			break;
		case 207:
			switch(solution)
			{
				case 1:
					break;
				case 2:
				case 3:
					db_buy_itm_freely(NULL,p->id,190142,3);
					break;
			}
			break;
		default:
			break;

	}

	p->sess_len = 0;
	PKG_UINT32(p->session,event,p->sess_len);
	PKG_UINT32(p->session,solution,p->sess_len);
	PKG_UINT32(p->session,p->yxb,p->sess_len);

	return db_add_shop_event_count(p, p->id);
}

int get_user_dish_all_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len,4);

	int count = *(int*)buf;
	struct	dish_item *pfood = (void*)(buf + 4);
	CHECK_BODY_LEN(len,4 + count * sizeof(struct	dish_item));

	switch(p->waitcmd)
	{
		case PROTO_SET_SHOP_EVENT:
			return magic_man_change_dish(p,pfood,count);
		case PROTO_GET_EAT_DISH_RAND:
			return get_magic_man_eat_dish(p,pfood,count);
	}

	ERROR_RETURN(("%u %d",id,p->waitcmd),-1);
}

int magic_man_change_dish(sprite_t* p, struct	dish_item *pfood, int count)
{
	int i = 0;
	uint32_t solution;

	p->sess_len = 0;
	UNPKG_H_UINT32(p->session,solution,p->sess_len);

	if(solution == 2) {
		db_change_shop_exp(NULL,20,p->id);
	} else {
		for(i = 0; i < count; i++,pfood++) {
			if(pfood->state == 3 && pfood->location < 50) {
				item_t *itm = get_item_prop(pfood->dish_id);
				if(itm && itm->u.food_tag.timer + itm->u.food_tag.bad_timer <= pfood->time) {
					uint32_t buff[3] = {0};
					buff[0] = pfood->id;
					buff[1] = pfood->dish_id;
					buff[2] = 0;
					DEBUG_LOG("uid=%u %u %u %u",p->id,pfood->id,pfood->dish_id,itm->price);
					send_request_to_db(SVR_PROTO_CLEAN_DISH,NULL,12,buff,p->id);
					if(solution == 1) {
						if(itm->price) {
							p->yxb += itm->price;
							db_change_shop_money(NULL,itm->price,p->id);
						} else {
							int j = 0;
							for(j = 0; j < 6; j += 2) {
								if(itm->u.food_tag.food[j] && itm->u.food_tag.food[j+1]) {
									DEBUG_LOG("add uid=%u itm=%u count=%u",p->id,itm->u.food_tag.food[j],itm->u.food_tag.food[j+1]);
									db_single_item_op_max_limit(NULL,p->id,itm->u.food_tag.food[j],itm->u.food_tag.food[j+1]);
									//db_single_item_op(NULL,p->id,itm->u.food_tag.food[j],itm->u.food_tag.food[j+1],1);
								}
							}
						}
					}
				}
			}
		}
	}

	int bytes = sizeof(protocol_t);
	PKG_UINT32(msg, 100, bytes);
	PKG_UINT32(msg, solution, bytes);
	PKG_UINT32(msg, p->yxb, bytes);
	init_proto_head(msg,p->waitcmd,bytes);
	return send_to_self(p,msg,bytes,1);
}

int db_set_dish_level(sprite_t * p,uint32_t dishid,uint32_t level, uint32_t id)
{
	uint32_t buff[2] = {dishid,level};
	return send_request_to_db(SVR_PROTO_SET_DISH_LEVEL,p,sizeof(buff),buff, id);
}

int get_dish_count_and_level_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_BODY_LEN(len,4);

	int j = 0;
	uint32_t dish_id;
	UNPKG_UINT32(body,dish_id,j);

	item_kind_t *ik_style = find_kind_of_item(dish_id);
	if(!ik_style || ik_style->kind != RESTAURANT_ITEM_KIND) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}

	return send_request_to_db(SVR_PROTO_GET_DISH_LEVEL, p, 4,&dish_id, p->id);
}

int get_dish_count_and_level_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	struct ret_309F *pkg = (void*)buf;
	CHECK_BODY_LEN(len,sizeof(struct ret_309F));

	switch(p->waitcmd) {
		case PROTO_PUT_DISH_IN_BOX:
		{
			struct req_3187 *req = (void*)p->session;
			item_t *itm = get_item_prop(req->dish_id);

			if(pkg->dish_lvl < 5) {
				int level = calc_dish_level(pkg->dish_cnt + 1,itm->u.food_tag.star);
				if(level > pkg->dish_lvl) {
					int addexp = itm->u.food_tag.addexp * level;
					req->addexp += addexp;
					int addcount = itm->u.food_tag.count * level;
					req->addcount += addcount;
					db_set_dish_level(NULL, req->dish_id, level, p->id);
					notify_dish_level_up(p, req->dish_id, level, addexp,addcount);
				}

				if(level == 5) {
					uint32_t info_m[] = {p->id};
					msglog(statistic_logfile, 0x02025200 + req->dish_id - 1340000, now.tv_sec, info_m, sizeof(info_m));
				}
			}

			if(pkg->dish_lvl == 5) {
				req->addexp += itm->u.food_tag.ex_exp;
				req->addcount += itm->u.food_tag.ex_cnt;
			}

			return send_request_to_db(SVR_PROTO_PUT_DISH_IN_BOX, p, sizeof(struct req_3187) - 4, p->session, p->id);
		}
	default:
		break;
	}

	 response_proto_uint32_uint32(p,p->waitcmd,pkg->dish_cnt,pkg->dish_lvl,0);
	 return 0;
}

int get_dish_count_and_level_all_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_BODY_LEN(len,0);
	return send_request_to_db(SVR_PROTO_GET_DISH_LEVEL_ALL, p, 0,NULL, p->id);
}

int get_dish_count_and_level_all_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	struct out_1046 {
		uint32_t dish_id;
		uint32_t dish_count;
		uint32_t dish_star;
	};

	CHECK_BODY_LEN_GE(len,4);
	uint32_t count = *(uint32_t*) buf;
	struct out_1046 *p_out = (void*)(buf + 4);
	CHECK_BODY_LEN(len,4 + count * sizeof(struct out_1046));

	int bytes = sizeof(protocol_t);
	PKG_UINT32(msg,count,bytes);
	int i = 0;
	while(i < count) {
		PKG_UINT32(msg,p_out->dish_id,bytes);
		PKG_UINT32(msg,p_out->dish_count,bytes);
		item_t *itm = get_item_prop(p_out->dish_id);
		int level = 0;
		if (itm != NULL) {
			level = calc_dish_level(p_out->dish_count,itm->u.food_tag.star);
			if(level > p_out->dish_star) {
				db_set_dish_level(NULL, p_out->dish_id, level, p->id);
			}
		}
		PKG_UINT32(msg,level,bytes);
		p_out++;
		i++;
	}

	init_proto_head(msg,p->waitcmd,bytes);
	return send_to_self(p,msg,bytes,1);
}

int get_dish_count_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_BODY_LEN(len,4);

	int j = 0;
	uint32_t dish_id;
	UNPKG_UINT32(body,dish_id,j);
	j = 0;
	PKG_H_UINT32(p->session,dish_id,j);
	return send_request_to_db(SVR_PROTO_GET_DISH_COUNT,p,4,&dish_id, p->id);
}

int get_dish_count_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len,4);

	int j = 0;
	uint32_t dish_id;
	UNPKG_H_UINT32(p->session,dish_id,j);

	response_proto_uint32_uint32(p,p->waitcmd,dish_id,*(uint32_t*)buf,0);
	return 0;
}

int get_magic_man_eat_dish_cmd(sprite_t * p,const uint8_t * body,int len)
{
	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	if(p->id != GET_UID_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_shop_not_yours, 1);
	}

	uint32_t dbpkg[4] = {0,16011,16012,0};
	return send_request_to_db(SVR_PROTO_CHK_ITEM, p, 13, dbpkg, p->id);
}

int get_magic_man_eat_dish(sprite_t* p, struct dish_item *pfood, int count)
{
	int i = 0;
	int index[20]={0};
	int val = rand();

	while(count-- > 0) {
		if(pfood[count].state == 6 && pfood[count].count >= 200) {
			index[i++] = count;
			DEBUG_LOG("%u,%d",pfood[count].dish_id,pfood[count].count);
		}
	}

	if(i==0) {
		return send_to_self_error(p, p->waitcmd, -ERR_no_dish_to_sell, 1);
	}

	i = index[val % i];
	DEBUG_LOG("%d %u,%d",i,pfood[i].dish_id,pfood[i].count);

	int dish_id = pfood[i].dish_id;
	int eat_cnt = pfood[i].count / 2;
	int sell_money = pfood[i].sell_money;

	val = val % 40;
	if(eat_cnt > 1000) eat_cnt = 1000;
	sell_money = sell_money * (80 + val);
	if(sell_money < 100) sell_money = 100;
	sell_money =  sell_money  * eat_cnt / 100;

	shop_map_info *shop = &p->tiles->mmdu[0].shop;

	shop->stack_len = 4 * 5;
	shop->stack_cmd = p->waitcmd;

	shop->stack[0] = dish_id;
	shop->stack[1] = eat_cnt;
	shop->stack[2] = sell_money;
	shop->stack[3] = pfood[i].id;
	shop->stack[4] = pfood[i].location;

	int bytes = sizeof(protocol_t);
	PKG_UINT32(msg, dish_id, bytes);
	PKG_UINT32(msg, eat_cnt, bytes);
	PKG_UINT32(msg, sell_money, bytes);
	init_proto_head(msg,p->waitcmd,bytes);
	return send_to_self(p,msg,bytes,1);
}

int set_magic_man_eat_dish_cmd(sprite_t * p,const uint8_t * body,int len)
{
	shop_map_info *shop = &p->tiles->mmdu[0].shop;
	if(shop->stack_cmd != PROTO_GET_EAT_DISH_RAND) {
		shop->stack_cmd = 0;
		shop->stack_len = 0;
		ERROR_RETURN(("%u %d %d",p->id,p->waitcmd,shop->stack_cmd),-1);
	}

	if(!p->tiles || !IS_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	if(p->id != GET_UID_SHOP_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_shop_not_yours, 1);
	}

	int j = 0;
	PKG_H_UINT32(p->session,0,j);
	PKG_H_UINT32(p->session,shop->stack[0],j);
	PKG_H_UINT32(p->session,shop->stack[1],j);
	PKG_H_UINT32(p->session,shop->stack[2],j);

	shop->stack_cmd = 0;
	shop->stack_len = 0;

	return db_single_item_op(p, p->id, 16011, 1, 0);
}

int set_magic_man_eat_dish_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int bytes = sizeof(protocol_t);
	shop_map_info *shop = &p->tiles->mmdu[0].shop;

	db_change_shop_money(NULL,shop->stack[2], p->id);

	p->yxb += shop->stack[2];
	shop->money += shop->stack[2];
	notify_shop_info_change(p);

	uint32_t info_m[] = {1};
	msglog(statistic_logfile, 0x0210A000 + 16011 - 16000, now.tv_sec, info_m, sizeof(info_m));

	PKG_UINT32(msg, shop->stack[0], bytes);
	PKG_UINT32(msg, shop->stack[1], bytes);
	PKG_UINT32(msg, shop->stack[2], bytes);
	PKG_UINT32(msg, shop->stack[3], bytes);
	PKG_UINT32(msg, shop->stack[4], bytes);
	init_proto_head(msg,p->waitcmd,bytes);
	return send_to_self(p,msg,bytes,1);
}

int add_shop_event_count_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len,4);

	uint32_t count = *(uint32_t*)buf;

	if(count >= 1000) {
		if(set_only_one_bit(p,14)) {
			set_honor_bit(p,12);
		}
	} else if(count >= 300){
		if(set_only_one_bit(p,13)) {
			set_honor_bit(p,11);
		}
	} else if(count >= 100) {
		if(set_only_one_bit(p,12)) {
			set_honor_bit(p,10);
		}
	} else if(count >= 50) {
		if(set_only_one_bit(p,11)) {
			set_honor_bit(p,9);
		}
	}

	uint32_t stage = 0;
	uint32_t enter_m[] = {1,0};
	uint32_t leave_m[] = {0,1};
	switch(count) {
		case 1:
			stage = 1;
			break;
		case 50:
			stage = 2;
			break;
		case 100:
			stage = 3;
			break;
		case 300:
			stage = 4;
			break;
		case 1000:
			stage = 5;
			break;
		default:break;
		msglog(statistic_logfile, 0x02031000 + stage, now.tv_sec, enter_m, sizeof(enter_m));
		if(stage) {
			msglog(statistic_logfile, 0x02031000 + stage - 1, now.tv_sec, leave_m, sizeof(leave_m));
		}
	}

	response_proto_str(p,p->waitcmd,p->sess_len,p->session,0);

	return 0;
}
