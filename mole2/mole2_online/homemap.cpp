#include <errno.h>
#include <net/if.h>
#include <glib.h>

#include "map"
#include "battle.h"
#include "homemap.h"
#include "items.h"
#include "sns.h"
#include "./proto/mole2_home.h"
#include "./proto/mole2_home_enum.h"

#include <libtaomee++/utils/tcpip.h>
#include <libtaomee++/proto/Ccmd_map.h>
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>

//-------------------------------------------------------
//函数定义
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    int proto_name( userid_t uid, sprite_where_t* ,  Cmessage* c_in ,int ret ) ;
#include "./proto/mole2_home_bind_for_online.h"


//-------------------------------------------------------------
//对应的结构体
#include "./proto/mole2_home.h"
//-------------------------------------------------------------
//命令绑定
typedef   int(*P_DEALFUN_T)( userid_t uid, sprite_where_t* spwhere, Cmessage* c_in,int ret );

#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    {cmdid, new (c_out), md5_tag,bind_bitmap ,proto_name },

Ccmd< P_DEALFUN_T> g_home_cmd_list[]={
#include "./proto/mole2_home_bind_for_online.h"

};
//命令map
Ccmd_map< Ccmd< P_DEALFUN_T> >  g_home_cmd_map;

int init_home_handle_funs()
{
    g_home_cmd_map.initlist(g_home_cmd_list,sizeof(g_home_cmd_list)/sizeof(g_home_cmd_list[0]));
	return 0;
}


//------------------------------------------------------

typedef struct clean_sprite_ol {
	uint32_t idx;
	sprite_where_t* p_home;
} clean_sprite_ol_t;

static gboolean free_spriteinfo_by_key(gpointer key, gpointer spr, gpointer userdata)
{
	g_slice_free1(sizeof(sprite_ol_t), spr);
	return TRUE;
}

void init_home_maps()
{
	maps = g_hash_table_new(g_int64_hash, g_int64_equal);
	map_copys = g_hash_table_new(g_int64_hash, g_int64_equal);	
}

static gboolean free_homemaps_by_key(gpointer key, gpointer spr, gpointer userdata)
{
	sprite_where_t* p = (sprite_where_t *)spr;
	g_hash_table_foreach_remove(p->sprites, free_spriteinfo_by_key, 0);
	g_hash_table_destroy(p->sprites);
	KDEBUG_LOG(low32_val(p->mapid), "FREE HOME MAP [%u]", high32_val(p->mapid));
	g_slice_free1(sizeof *p, p);
	return TRUE;
}

static void do_clean_sprite_ol_by_idx(gpointer key, gpointer value, gpointer data)
{
	clean_sprite_ol_t* p_cs = (clean_sprite_ol_t *)data;
	sprite_ol_t* p = (sprite_ol_t *)value;
	
	if ((p->sprite_info.userid % MAX_HOMESERV_NUM) == p_cs->idx) {
		KDEBUG_LOG(p->sprite_info.userid, "CLEAN WITH CHK [idx=%u %u]", p->sprite_info.userid % MAX_HOMESERV_NUM, p_cs->idx);
		g_hash_table_remove(p_cs->p_home->sprites, &p->sprite_info.userid);
		g_slice_free1(sizeof(sprite_ol_t), value);
	}
}

static void try_del_home(sprite_where_t* p_home)
{
	if (!g_hash_table_size(p_home->sprites)) {
		g_hash_table_destroy(p_home->sprites);
		g_hash_table_remove(maps, &p_home->mapid);
		g_slice_free1(sizeof(*p_home), p_home);
	}
}

static void do_clean_home_by_idx(gpointer key, gpointer value, gpointer data)
{
	sprite_where_t* p_home = (sprite_where_t *)value;
	clean_sprite_ol_t csp = { *(int*)data, p_home};
	
	g_hash_table_foreach(p_home->sprites, do_clean_sprite_ol_by_idx, &csp);

	try_del_home(p_home);
}

static inline gboolean noti_cli_leave_war(gpointer key, gpointer value, gpointer user_data)
{
	sprite_t* p = (sprite_t *)value;
	send_msg_to_self(p, noti_cli_leave_war_cmd, NULL, 0);
	p->team_state = 0;
	KDEBUG_LOG(p->id, "NEED LEAVE WAR");
	return TRUE;
}

void clean_home_by_idx(int idx)
{
	g_hash_table_foreach(maps, do_clean_home_by_idx, &idx);
	KDEBUG_LOG(0, "HOME USER CNT [%d %u]", idx, g_hash_table_size(maps));
	g_hash_table_foreach_steal(war_sprites, noti_cli_leave_war, NULL);
}

void fini_home_maps()
{
	g_hash_table_foreach_remove(maps, free_homemaps_by_key, 0);
	g_hash_table_destroy(maps);
}

void remove_all_across_svr_sprites()
{
	g_hash_table_foreach_remove(maps, free_homemaps_by_key, 0);
}

//----------------------------------------------
void traverse_across_svr_sprites(map_id_t mapid, void (*action)(void* key, void* sinfo, void* data), void* data)
{
	sprite_where_t* spwhere = (sprite_where_t *)g_hash_table_lookup(maps, &mapid);
	if (spwhere) {
		g_hash_table_foreach(spwhere->sprites, action, data);
	}
}

void try_del_home_by_id(map_id_t homeid)
{
	sprite_where_t* p_home = (sprite_where_t *)g_hash_table_lookup(maps, &homeid);
	if (p_home) {
		try_del_home(p_home);
	}
}


//----------------------------------------------
static void enter_map_op(userid_t uid, sprite_where_t* spwhere, const uint8_t* body, uint32_t len)
{
	uint32_t user_cnt = *(uint32_t*)body;
	KDEBUG_LOG(uid, "ENTER HASH HOME [homeid=%lu u_cnt=%u len=%u]", spwhere->mapid, user_cnt, len);
	CHECK_BODY_LEN_GE_VOID(len, 4 + sizeof(sprite_base_info_t) * user_cnt);
	char* p_offset = ((char*)body) + 4;

	for (uint32_t i = 0; i < user_cnt; ++i) {
		userid_t userid = *(userid_t*)p_offset;
		sprite_ol_t* p	= (sprite_ol_t *)g_hash_table_lookup(spwhere->sprites, &userid);
		if(!p) {
			p = (sprite_ol_t *)g_slice_alloc(sizeof(sprite_ol_t));
		}
		memcpy(p, p_offset, sizeof(sprite_base_info_t));
		p_offset += sizeof(sprite_base_info_t);
		KDEBUG_LOG(p->sprite_info.userid, "ADD_SPRITE_OL [n=%s attire_cnt=%u pet_f=%u]----------",
				p->sprite_info.nick, p->sprite_info.attire_cnt, p->sprite_info.pet_followed);
		if (p->sprite_info.pet_followed) {
			memcpy(&p->pet_sinfo, p_offset, sizeof(pet_simple_info_t));
			p_offset += sizeof(pet_simple_info_t);
			KDEBUG_LOG(p->sprite_info.userid, "ADD_PET_OL [petid=%u lv=%u]", p->pet_sinfo.petid, p->pet_sinfo.pet_level);
		}
		g_hash_table_insert(spwhere->sprites, &(p->sprite_info.userid), p);	
	}
}

static void get_home_user_op(userid_t uid, sprite_where_t* spwhere, const uint8_t* body, int len)
{
	KDEBUG_LOG(0, "GET HOME USER INFO");
}

void home_del_a_user(sprite_where_t *spwhere, uint32_t leave_uid)
{
	sprite_ol_t* p = (sprite_ol_t *)g_hash_table_lookup(spwhere->sprites, &leave_uid);
	if (p) {
		g_hash_table_remove(spwhere->sprites, &leave_uid);
		g_slice_free1(sizeof(sprite_ol_t), p);
	}
	try_del_home(spwhere);
}

static void cli_leave_map_op(userid_t uid, sprite_where_t* spwhere, const uint8_t* body, int len)
{
	if (!spwhere) {
		return ;
	}
	KDEBUG_LOG(uid, "FREE SPRITE_OL [homeid=%u-%u]", high32_val(spwhere->mapid), low32_val(spwhere->mapid));
	CHECK_BODY_LEN_VOID(len, sizeof(protocol_t) + 20);
	int offset = sizeof(protocol_t);
	userid_t leave_uid;
	UNPKG_UINT32(body, leave_uid, offset);
	home_del_a_user(spwhere,leave_uid);
}

static void send_to_self_op(userid_t uid, sprite_where_t* spwhere, uint8_t* body, int len)
{
	KDEBUG_LOG(uid,"send_to_self_op");

	sprite_t* p = get_sprite(uid);
	if (p) {
		send_to_self(p, body, len, 0);
		protocol_t* proto = (protocol_t *)body;
		if (noti_cli_war_update_score_cmd == ntohs(proto->cmd)) {
			int i = sizeof(protocol_t) + 4;
			uint32_t uid;
			UNPKG_UINT32(body, uid, i);
			if (p->id == uid)
				p->uiflag = 1;
		}
		log_buf(p->id, body, len);
	}
}


static void noti_cli_war_over_op(userid_t uid, sprite_where_t* spwhere, const uint8_t* body, int len)
{
	sprite_t* p = get_sprite(uid);
	if (p) {
		CHECK_BODY_LEN_VOID(len, 50);
		int i = sizeof(protocol_t);
		noti_cli_war_over_out cli_out;
		for (uint32_t loop = 0; loop < 4; loop ++) {
			UNPKG_UINT32(body, cli_out.rank[loop].id, i);
			UNPKG_UINT32(body, cli_out.rank[loop].count, i);
			KDEBUG_LOG(p->id, "WAR OVER RANK\t[%u %u %u]", loop, cli_out.rank[loop].id, cli_out.rank[loop].count);
			if (p->hero_team == cli_out.rank[loop].id) {
				uint32_t expfactor = p->uiflag ? get_exp_factor(p) : 0;
				stru_item tmpitem;
				item_t* pday = cache_get_day_limits(p, ITEM_ID_ROCK_CHIP);
				tmpitem.count = expfactor ? 5 - loop : 0;
				if (pday->count + tmpitem.count > 100)
					tmpitem.count = pday->count >= 100 ? 0 : 100 - pday->count;
				tmpitem.count = cache_add_kind_item(p, ITEM_ID_ROCK_CHIP, tmpitem.count);
				tmpitem.itemid = ITEM_ID_ROCK_CHIP;
				cli_out.itemlist.push_back(tmpitem);
				if (tmpitem.count) {
					db_add_item(NULL, p->id, ITEM_ID_ROCK_CHIP, tmpitem.count);
					pday->count += tmpitem.count;
					db_day_add_ssid_cnt(NULL, p->id, ITEM_ID_ROCK_CHIP, tmpitem.count, 100);
				}
				
				uint32_t exp = pow(p->level, 0.9) * pow(1.0 / (loop + 1), 0.5) * 60;
				exp = exp * expfactor / 100;
				exp = add_exp_to_sprite(p, exp);
				tmpitem.itemid = item_id_exp;
				tmpitem.count = exp;
				cli_out.itemlist.push_back(tmpitem);
				KDEBUG_LOG(p->id, "WAR GET EXP\t[%u %u]", exp, expfactor);
			}
		}

		send_msg_to_self(p, noti_cli_war_over_cmd, &cli_out, 0);
		g_hash_table_remove(war_sprites, &p->id);
		msg_log_war_fin(p->id);
	}
}

	
int home_game_noti_start (userid_t uid, sprite_where_t* spwhere,Cmessage *c_out , int ret)
{
	home_game_noti_start_out *p_out= P_OUT;
	KDEBUG_LOG(uid, "WAR START:out=%p",p_out);
	sprite_t* p = get_sprite(uid);
	if (p) {
		p->pvp_pk_can_get_item=2;//设置失败
		KDEBUG_LOG(p->id,"pvp_pk_can_get_item:%u" ,p->pvp_pk_can_get_item );
		send_msg_to_self(p, home_game_noti_start_cmd , p_out, 0);
		msg_log_pvp_user(uid,p->pvp_type);
	}
	return 0;
}

static void action_op(userid_t uid, sprite_where_t* spwhere, const uint8_t* body, int len)
{
	sprite_ol_t* spinfo = (sprite_ol_t *)g_hash_table_lookup(spwhere->sprites, &uid);
	if (spinfo) {
		int i = sizeof(protocol_t) + 4;
		UNPKG_UINT32(body, spinfo->sprite_info.base_action, i);
		UNPKG_UINT32(body, spinfo->sprite_info.direction, i);
	}
}

static void walk_op(userid_t uid, sprite_where_t* spwhere, const uint8_t* body, int len)
{
	sprite_ol_t* so = (sprite_ol_t *)g_hash_table_lookup(spwhere->sprites, &uid);
	if (so) {
		int i = sizeof(protocol_t) + 4;
		UNPKG_UINT32(body, so->sprite_info.pos_x, i);
		UNPKG_UINT32(body, so->sprite_info.pos_y, i);
		UNPKG_UINT32(body, so->sprite_info.direction, i);
		//reset_common_action(&(so->player));
	}
}

static void set_user_nick_op(userid_t uid, sprite_where_t* spwhere, const uint8_t* body, int len)
{
	sprite_ol_t* so = (sprite_ol_t *)g_hash_table_lookup(spwhere->sprites, &uid);
	if (so) {
		int i = sizeof(protocol_t) + 4;
		UNPKG_STR(body, so->sprite_info.nick, i, USER_NICK_LEN);
	}
}

static void get_users_from_hs(userid_t uid, sprite_where_t* spwhere, const uint8_t* body, uint32_t len)
{
	CHECK_BODY_LEN_GE_VOID(len, 4);
	uint32_t usr_cnt = *(uint32_t*)body;
	
	int idx = 4;
	uint32_t loop;
	for (loop = 0; loop < usr_cnt; loop++) {
		CHECK_BODY_LEN_GE_VOID(len, idx + sizeof(sprite_base_info_t));
		sprite_base_info_t* psbit = (sprite_base_info_t*)(body + idx);
		pet_simple_info_t* ppsi = NULL;
		if (psbit->pet_followed) {
			CHECK_BODY_LEN_GE_VOID(len, idx + sizeof(sprite_ol_t));
			ppsi = (pet_simple_info_t*)(body + idx + sizeof(sprite_base_info_t));
		}
		KDEBUG_LOG(psbit->userid, "unpkg Sprite\t[idx=%u followed=%x mpid=%lu cnt=%u]", idx, psbit->pet_followed, spwhere->mapid, usr_cnt);
		int cpy_len = 0;
		sprite_ol_t* pso = (sprite_ol_t *)g_hash_table_lookup(spwhere->sprites, &(psbit->userid));
		if (pso) {
			cpy_len = ppsi ? sizeof(sprite_ol_t) : sizeof(sprite_base_info_t);
			memcpy(pso, body + idx, cpy_len);
			idx += cpy_len;
		} else {
			KDEBUG_LOG(psbit->userid, "Alloc Sprite [in map %lu-------------------------]", spwhere->mapid);
			pso = (sprite_ol_t *)g_slice_alloc(sizeof *pso);
			cpy_len = ppsi ? sizeof(sprite_ol_t) : sizeof(sprite_base_info_t);
			memcpy(pso, body + idx, cpy_len);
			idx += cpy_len;
			g_hash_table_insert(spwhere->sprites, &(psbit->userid), pso);
		}
	}
	
}

//-------------------------------------------------------
static int sprite_info_chg_op(uint32_t uid, map_id_t mapid, uint32_t opid, uint8_t* buf, int len,int ret)
{
	CHECK_BODY_LEN_GE(len, 0);
	
	sprite_where_t* spwhere    = (sprite_where_t *)g_hash_table_lookup(maps, &mapid);

	if (!spwhere && opid != proto_cli_leave_map && mapid!=0) {
		KDEBUG_LOG(uid, "ALLOC HOME HASH MAP [map_type=%u map_low=%u]", high32_val(mapid), low32_val(mapid));
		spwhere          = (sprite_where_t *)g_slice_alloc0(sizeof *spwhere);
		spwhere->mapid   = mapid;
		spwhere->sprites = g_hash_table_new(g_int_hash, g_int_equal);
		g_hash_table_insert(maps, &(spwhere->mapid), spwhere);
	}



#define DISPATCH_OP(op_, func_) \
		case (op_): (func_)(uid, spwhere, buf, len); break
#define DISPATCH_OP_BROADCAST(op_, func_) \
		case (op_): (func_)(uid, spwhere, buf, len); send_to_map3(mapid, buf, len); break
#define DISPATCH_OP_SELF(op_) \
		case (op_): send_to_self_op(uid, spwhere, buf, len); break

	//DEBUG_LOG("=====opid:%u",opid );
    Ccmd< P_DEALFUN_T> * p_cmd_item =g_home_cmd_map.getitem(opid );
	if (p_cmd_item ){
	    Cmessage * msg;
	    if (ret==0){//成功
	        //还原对象
	        p_cmd_item->proto_pri_msg->init( );
	
	        byte_array_t in_ba ( ((char *)buf), len );
	
	        //失败
	        if (!p_cmd_item->proto_pri_msg->read_from_buf(in_ba)) {
	            KDEBUG_LOG(uid , "err:pkg 111 ");
	            return 0;
	        }
	
	        //客户端多上传报文
	        if (!in_ba.is_end()) {
	            KDEBUG_LOG(uid, "cerr:pkg 2222 ");
	            return 0;
	        }
	        msg=p_cmd_item->proto_pri_msg;
	    } else {
	        msg= NULL;
	    }

		DEBUG_LOG("homefunc=================opid:%u",opid  );
		p_cmd_item->func(uid, spwhere,msg, ret);
	}else{
		switch (opid) {
			DISPATCH_OP(SOP_enter_map, enter_map_op);
			DISPATCH_OP(SOP_get_home_user, get_home_user_op);
			DISPATCH_OP(SOP_get_users_from_HS, get_users_from_hs);
			DISPATCH_OP(SOP_user_show, action_op);
			DISPATCH_OP(SOP_user_walk, walk_op);		
			DISPATCH_OP(SOP_set_user_nick, set_user_nick_op);

			DISPATCH_OP_BROADCAST(proto_cli_leave_map, cli_leave_map_op);

			DISPATCH_OP(noti_cli_war_over_cmd, noti_cli_war_over_op);
			DISPATCH_OP_SELF(cli_get_positions_info_cmd);
			DISPATCH_OP_SELF(cli_war_challenge_pos_cmd);
			DISPATCH_OP_SELF(cli_war_play_card_cmd);
			DISPATCH_OP_SELF(noti_cli_war_be_attacked_cmd);
			DISPATCH_OP_SELF(noti_cli_war_challenge_result_cmd);
			DISPATCH_OP_SELF(cli_leave_position_cmd);
			DISPATCH_OP_SELF(noti_cli_war_update_score_cmd);
			
		default:
			send_to_map3(mapid, buf, len);
			break;
		}
#undef DISPATCH_OP
#undef DISPATCH_OP_BROADCAST
#undef DISPATCH_OP_SELF
	}


	return 0;
}

/*! public fuction */
int handle_homeserv_pkg(homeserv_proto_t* compkg, int len)
{
	KDEBUG_LOG(compkg->id, "HOME OPID\t[%u %u %lu opid=%u %u]", compkg->online_id, get_server_id(), compkg->mapid, compkg->opid, len);
	if (compkg->online_id != get_server_id() 
			|| compkg->opid == SOP_enter_map || (compkg->opid >= 1300 && compkg->opid < 1400)) {
		homeserv_proto_t* svrproto = compkg;
		sprite_info_chg_op(svrproto->id, svrproto->mapid, svrproto->opid, svrproto->body, len - sizeof(homeserv_proto_t ) ,svrproto->ret);
	}
	return 0;
}
/*********************pvp****************************/
int home_game_get_map_id(userid_t uid, sprite_where_t* spwhere,Cmessage *c_out , int ret)
{
	if (ret !=0 ) return 0;
	home_game_get_map_id_out *p_out= P_OUT;
	KDEBUG_LOG(uid, "home_game_get_map_id_out:out=%p",p_out);
	cli_game_pvp_get_map_id_out cli_out;	
	cli_out.mapid=p_out->mapid&0xFFFFFFFF;
	cli_out.maptype=(p_out->mapid>>32);
	sprite_t* p = get_sprite(uid);
	if (p) {
		send_msg_to_self(p, cli_game_pvp_get_map_id_cmd ,&cli_out , 1);
	}
	return 0;
}

int home_game_noti_left_time(userid_t uid, sprite_where_t* spwhere,Cmessage *c_out , int ret)
{
	if (ret !=0 ) return 0;
	home_game_noti_left_time_out *p_out= P_OUT;
	KDEBUG_LOG(uid, "home_game_noti_left_time:out=%p",p_out);
	sprite_t* p = get_sprite(uid);
	if (p) {
		send_msg_to_self(p, home_game_noti_left_time_cmd , p_out, 0);
	}
	return 0;
}

int home_game_noti_info(userid_t uid, sprite_where_t* spwhere,Cmessage *c_out , int ret)
{
	if (ret !=0 ) return 0;
	home_game_noti_info_out *p_out= P_OUT;
	KDEBUG_LOG(uid, "home_game_noti_left_time:out=%p",p_out);
	sprite_t* p = get_sprite(uid);
	if(p){
		if(p_out->is_win==1){
			p->pvp_scores+= 5;
			p->pvp_pk_can_get_item=1;
			db_day_add_ssid_cnt(NULL, p->id, ssid_week_pvp_scores, 5, -1);
		}
		send_msg_to_self(p,	cli_noti_home_pvp_info_cmd,p_out, 0);
		KDEBUG_LOG(p->id ,"p->pvp_pk_can_get_item:%u", p->pvp_pk_can_get_item );
	}
	return 0;
}

int home_game_noti_pvp_winner(userid_t uid, sprite_where_t* spwhere,Cmessage *c_out , int ret)
{
	if (ret !=0 ) return 0;
	home_game_noti_pvp_winner_out *p_out= P_OUT;
	KDEBUG_LOG(uid, "home_game_noti_info:out=%p",p_out);
	sprite_t* p = get_sprite(uid);
	if (p) {
		send_msg_to_self(p,	cli_noti_home_pvp_pk_info_cmd ,p_out, 0);
		msg_log_pvp_start(uid,p->pvp_type);
	}
	return 0;
}
/*-----------------------trade-----------------------*/

int home_trade_get_mapid(userid_t uid, sprite_where_t* spwhere,Cmessage *c_out , int ret)
{
	sprite_t* p = get_sprite(uid);
	if(!p)
		return -1;
	if (ret !=0 ) {
		return send_to_self_error(p,p->waitcmd,cli_err_enter_fail,1);
	}
	home_trade_get_mapid_out *p_out= P_OUT;
	KDEBUG_LOG(uid, "home_trade_get_map_id_out:roomid=%u",p_out->roomid);
	KF_LOG("trade_enter_in",p->id,"xiaomee:%u roomid:%u",p->xiaomee,p_out->roomid);
	cli_trade_get_mapid_out cli_out;	
	//map_id_t mapid=GET_TRADE_MAPID(p_out->roomid);
	cli_out.roomid=p_out->roomid;
	cli_out.mapid=11216;
	cli_out.maptype=0xFC000000+p_out->roomid;
	p->roomid=cli_out.roomid;
	return send_msg_to_self(p, cli_trade_get_mapid_cmd ,&cli_out , 1);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  定时器半小时关闭店铺
 */
/* ----------------------------------------------------------------------------*/
int noti_close_shop(void*owner, void *data)
{
	sprite_t *p=(sprite_t *)owner;
	KDEBUG_LOG(p->id, "time out:shop close roomid=%u shopid=%u shop_state=%u xiaomee=%u"
			,p->roomid, p->shop_id , p->shop_state, p->xiaomee);
	if(p->shop_state > shop_init ){
		p->shop_state=shop_init;
		//通知用户自己店铺关闭  使用1634协议
		cli_trade_change_shop_out cli_out;
		cli_out.cmd=4;
		cli_out.uid=p->id;
		cli_out.sid=p->shop_id;
		cli_out.state=shop_init;
		send_msg_to_self(p,cli_trade_change_shop_cmd,&cli_out,1);
		//发给home 修改状态
		home_trade_change_shop_in h_in;
		h_in.cmd=3;
		h_in.roomid=p->roomid;
		h_in.sid=p->shop_id;
		return send_msg_to_homeserv(p->id, p->tiles->id, home_trade_change_shop_cmd ,&h_in);
	}
	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  register shop 的返回
 */
/* ----------------------------------------------------------------------------*/
int home_trade_change_shop(userid_t uid, sprite_where_t* spwhere,Cmessage *c_out , int ret)
{
	sprite_t* p = get_sprite(uid);
	if(!p)
		return -1;
	if (ret != 0 ) {
		return send_to_self_error(p,p->waitcmd,cli_err_open_shop_fail,1);
	}
	p->shop_state=shop_register;
	p->register_shop_time=get_now_sec();
	p->p_open_shop_timer=ADD_ONLINE_TIMER(p, n_noti_close_shop, NULL, 1800);
	KDEBUG_LOG(uid, "home_trade_register_shop succ:xiaomee=%u %u",p->xiaomee,p->register_shop_time);
	KF_LOG("trade_register_shop",p->id,"xiaomee:%u shopid:%u roomid:%u",p->xiaomee,p->shop_id,p->roomid);
	db_add_xiaomee(NULL, p->id, -500);
	p->xiaomee -= 500 ;
	
	home_trade_change_shop_out cli_out;
	cli_out.cmd=0;
	cli_out.uid=uid;
	cli_out.sid=p->shop_id;
	cli_out.state=p->shop_state;
	cli_out.prof=p->prof;
	memcpy(cli_out.nick,p->shopname,sizeof(cli_out.nick));
	KDEBUG_LOG(uid, "home_trade_register_shop succ:sid=%u %u",cli_out.sid,cli_out.uid);
	return send_msg_to_self(p,cli_trade_change_shop_cmd, &cli_out, 1);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  其他online的用户拉取店铺的商品列表
 */
/* ----------------------------------------------------------------------------*/
int home_trade_get_items(userid_t uid, sprite_where_t* spwhere,Cmessage *c_out , int ret)
{
	sprite_t* p = get_sprite(uid);
	home_trade_get_items_out *p_out=P_OUT;
	cli_trade_get_items_out h_in;
	h_in.uid=p_out->uid;
	if(!p || p->shop_state != shop_open){
		h_in.lefttime=-1;
	}
	else{
		h_in.lefttime=p->register_shop_time+1800>get_now_sec()?(p->register_shop_time+1800-get_now_sec()):0;
		trade_get_items(p, h_in);
	}
	KDEBUG_LOG(uid, "from home:home_trade_get_item[sendid=%u %u %lu]",p_out->uid,p?p->shop_state:0,h_in.items.size());
	return send_msg_to_homeserv(uid,spwhere->mapid , home_trade_noti_items_cmd ,&h_in);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  cli_trade_get_items_cmd发往home的返回
 */
/* ----------------------------------------------------------------------------*/
int home_trade_noti_items(userid_t uid, sprite_where_t* spwhere,Cmessage *c_out , int ret)
{
	sprite_t* p = get_sprite(uid);
	if(!p)
		return -1;
	if( ret != 0 ){
		return send_to_self_error(p,p->waitcmd,cli_err_cannot_get,1);
	}
	home_trade_noti_items_out *p_out=P_OUT;
	KDEBUG_LOG(uid, "from home:home_trade_noti_items[sendid=%u lefttime=%u]",p_out->uid,p_out->left_time);
	return send_msg_to_self(p,cli_trade_get_items_cmd,p_out,1);
}
/* ----------------------------------------------------------------------------*/
/**
 * @brief  cli_trade_get_shop_info 的返回
 */
/* ----------------------------------------------------------------------------*/
int home_trade_get_shop_info(userid_t uid, sprite_where_t* spwhere,Cmessage *c_out , int ret)
{
	sprite_t* p = get_sprite(uid);
	if(!p)
		return -1;
	home_trade_get_shop_info_out *p_out=P_OUT;
	KDEBUG_LOG(uid, "from home:home_trade_get_shop_info");
	//for( uint32_t loop=0 ;loop<6  ; loop++ ){
		//DEBUG_LOG("xxxxx %u %u %u %s",p_out->info[loop].state,p_out->info[loop].uid,p_out->info[loop].prof,p_out->info[loop].nick);
	//}
	
	return send_msg_to_self(p,p->waitcmd,p_out,1);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  有店铺状态改变时 小屋会发给本地图除本人外的所有人
 */
/* ----------------------------------------------------------------------------*/
int home_trade_noti_shop_state(userid_t uid, sprite_where_t* spwhere,Cmessage *c_out , int ret)
{
	sprite_t* p = get_sprite(uid);
	if(!p)
		return -1;
	home_trade_noti_shop_state_out *p_out=P_OUT;
	cli_trade_change_shop_out cli_out;
	KDEBUG_LOG(uid, "from home:home_trade_noti_shop_state cmd=%u sid=%u shop_state=%u uid=%u prof=%u %s",
			p_out->cmd, p_out->sid, p_out->state, p_out->uid,p_out->prof,p_out->nick);
	if( p_out->cmd == 0 || p_out->cmd == 5 ){//有人抢占店铺  则发六个参数 见cli_trade_change_shop
		return send_msg_to_self(p, cli_trade_change_shop_cmd, p_out, 0);
	}else{//其他状态则发四个参数
		cli_out.cmd=p_out->cmd;
		cli_out.uid=p_out->uid;
		cli_out.sid=p_out->sid;
		cli_out.state=p_out->state;
		return send_msg_to_self(p, cli_trade_change_shop_cmd, &cli_out, 0);
	}
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  其他online上用户来购买商品
 */
/* ----------------------------------------------------------------------------*/
int home_trade_buy_item(userid_t uid, sprite_where_t* spwhere,Cmessage *c_out , int ret)
{
	sprite_t* p = get_sprite(uid);
	home_trade_buy_item_callback_in h_in;
	home_trade_buy_item_out *p_out=P_OUT;
	cli_trade_buy_item_out out;
	int32_t money=0,tax=0;
	h_in.uid=p_out->uid;
	if(!p || p->shop_state != shop_open || p_out->roomid != p->roomid){
		h_in.ret=cli_err_shop_state;
		return send_msg_to_homeserv(uid,spwhere->mapid , home_trade_buy_item_callback_cmd ,&h_in);
	}
	if( p->add_item_timestamp > time(NULL) ){
		h_in.ret=cli_err_havnot_this_item;
		return send_msg_to_homeserv(uid,spwhere->mapid , home_trade_buy_item_callback_cmd ,&h_in);
	}
	KDEBUG_LOG(p->id,"home_trade_buy_item[buyer=%u roomid=%u itemid%u itemcnt=%u tradeid=%u seller's xiaomee%u]",
			p_out->uid, p->roomid,p_out->itemid, p_out->itemcnt ,p_out->tradeid, p->xiaomee);
	KF_LOG("trade_buy_from_home",p->id,"buyer:%u itemid:%u itemcnt:%u tradeid:%u",
			p_out->uid, p_out->itemid, p_out->itemcnt ,p_out->tradeid);
	KF_LOG("trade_seller_info",p->id,"xiaomee:%u shopid:%u roomid:%u",p->xiaomee,p->shop_id,p->roomid);
	if(IS_ITEM(p_out->itemid)){/*buy item*/
		//背包里物品是否存在
		if (!cache_item_have_cnt(p,p_out->itemid,p_out->itemcnt)) {
			h_in.ret=cli_err_havnot_this_item;
			return send_msg_to_homeserv(uid,spwhere->mapid , home_trade_buy_item_callback_cmd ,&h_in);
		}
		//商店里物品是否存在
		if(!check_trade_item_exist(p, p_out->itemid, p_out->tradeid, p_out->itemcnt)){
			h_in.ret=cli_err_item_cannot_buy;
			return send_msg_to_homeserv(p->id, p->tiles->id, home_trade_buy_item_callback_cmd ,&h_in);
		}
		if( !check_trade_xiaomee_enough(p_out->xiaomee,p, p_out->itemid, p_out->itemcnt, p_out->tradeid) ){
			h_in.ret=cli_err_xiaomee_not_enough;
			return send_msg_to_homeserv(uid,spwhere->mapid , home_trade_buy_item_callback_cmd ,&h_in);
		}

		trade_item_t tmp={};
		tmp=trade_del_item_ex(p, p_out->itemid,p_out->itemcnt, p_out->tradeid);
		money=tmp.price*tmp.itemcnt; 
		tax=(tmp.tax)*tmp.itemcnt; 
		KDEBUG_LOG(p->id,"trade start item:seller=%u buyer=%u itemid=%u itemcnt=%u money=%u tax=%u",
					 p->id,p_out->uid, tmp.itemid, tmp.itemcnt, money , tax);
		out.itemid=p_out->itemid;
		h_in.itemid=p_out->itemid;
		
		/*卖家加钱 扣物品*/
		p->xiaomee += money-tax;//seller add xiaomee in cache
		db_add_xiaomee(NULL,p->id, money-tax);//seller add xiaomee in db
		//cache_add_kind_item(p,tmp.itemid,-1*tmp.itemcnt);//seller del item in cache
		cache_reduce_kind_item(p,tmp.itemid,tmp.itemcnt);
		db_add_item(NULL, p->id,tmp.itemid, -1*tmp.itemcnt);//seller del item in db

		/*买家扣钱 加物品*/
		db_add_xiaomee(NULL,p_out->uid, money*(-1));//buyer del xiaomee in db
		db_add_item(NULL, p_out->uid, tmp.itemid, tmp.itemcnt);//buer add item in db

		add_to_trade_record(p, p_out->uid,&tmp);
		KF_LOG("trade_buy_item_succ",p->id,"seller:%u buyer:%u roomid:%u shopid:%u itemid:%u itemcnt:%u money:%u tax:%u",
					p->id, p_out->uid,p->roomid,p->shop_id, tmp.itemid, tmp.itemcnt, money , tax);
		}else{/*buy cloth*/
			//背包里物品是否存在
			if (!cache_get_bag_cloth(p,p_out->itemid)) {
				h_in.ret=cli_err_havnot_this_item;
				return send_msg_to_homeserv(uid,spwhere->mapid , home_trade_buy_item_callback_cmd ,&h_in);
			}
			//商店里物品是否存在
			if(!check_trade_cloth_exist(p, p_out->itemid, p_out->tradeid, p_out->itemcnt)){
				h_in.ret=cli_err_item_cannot_buy;
				return send_msg_to_homeserv(uid,spwhere->mapid , home_trade_buy_item_callback_cmd ,&h_in);
			}

			if( !check_trade_xiaomee_enough(p_out->xiaomee,p, p_out->itemid, p_out->itemcnt, p_out->tradeid) ){
				h_in.ret=cli_err_xiaomee_not_enough;
				return send_msg_to_homeserv(uid,spwhere->mapid , home_trade_buy_item_callback_cmd ,&h_in);
			}
			trade_cloth_t tmp={};
			KDEBUG_LOG(p->id,"trade start cloth:seller=%u buyer=%u clothid=%u clothtype=%u money=%u tax=%u",
					p->id, p_out->uid, tmp.clothid, tmp.clothtype, money , tax);
			tmp=trade_del_cloth_ex(p, p_out->itemid, p_out->tradeid);
			money=tmp.price; 
			tax=tmp.tax; 

			/*加钱 扣物品 扣钱 加物品*/
			p->xiaomee+=money-tax;//seller cache add xiaomee
			db_add_xiaomee(NULL,p->id,  money-tax);//seller db add xiaomee
			body_cloth_t *pbc=cache_get_bag_cloth(p, tmp.clothid);
			out.itemid=pbc->clothtype;
			h_in.itemid=pbc->clothtype;
			db_add_cloth_new(NULL,p_out->uid,money,pbc);//buyer db add cloth and del money

			cache_del_bag_cloth(p, tmp.clothid);//seller cache del cloth			
			db_del_cloth(NULL, p->id ,tmp.clothid);//sller db del cloth

			add_to_trade_record(p,p->id,&tmp);
			KF_LOG("trade_buy_cloth_succ",p->id,"seller:%u buyer:%u roomid:%u shopid:%u clothid:%u clothtype:%u money:%u tax:%u"
					,p->id, p_out->uid,p->roomid, p->shop_id,tmp.clothid, tmp.clothtype, money , tax);
	}
	//send to seller
	out.uid=uid;
	out.itemcnt=p_out->itemcnt;
	out.money=money;
	out.tax=tax;
	//KDEBUG_LOG(p->id,"lllllll%u %u",out.money,p->xiaomee);
	send_msg_to_self(p,cli_trade_buy_item_cmd , &out, 0);
	//send to buyer
	h_in.itemcnt=p_out->itemcnt;
	h_in.money=money;
	h_in.tax=tax;
	return send_msg_to_homeserv(uid,spwhere->mapid , home_trade_buy_item_callback_cmd ,&h_in);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  cli_trade_buy_item 发往其他online的返回
 */
/* ----------------------------------------------------------------------------*/
int home_trade_buy_item_callback(userid_t uid, sprite_where_t* spwhere,Cmessage *c_out , int ret)
{
	sprite_t* p = get_sprite(uid);
	if(!p)
		return -1;
	if( ret !=0 ){
		return send_to_self_error(p,p->waitcmd,ret,1);
	}
	home_trade_buy_item_callback_out *p_out=P_OUT;
	//cache del xiaomee
	p->xiaomee =(p->xiaomee>p_out->money)?(p->xiaomee-p_out->money):0;
	cache_add_kind_item(p,p_out->itemid,p_out->itemcnt);
	KDEBUG_LOG(uid, "home_trade_buy_item_callback:sellerid=%u itemid=%u itemcnt=%u money=%u",
			p_out->uid, p_out->itemid, p_out->itemcnt, p_out->money);
	//send to sel
	cli_trade_buy_item_out out;
	out.uid=p_out->itemid;
	out.itemid=p_out->itemid;
	out.itemcnt=p_out->itemcnt;
	out.money=p_out->money;
	out.tax=p_out->tax;
	return send_msg_to_self(p,cli_trade_buy_item_cmd, &out, 1);
}
