#include "sprite.h"
#include "util.h"
#include "battle.h"
#include "center.h"
#include "mail.h"
#include "sns.h"
#include "viponly.h"
#include "activity.h"
/********新的命令分发 start*******/
#include <libtaomee++/proto/Ccmd_map.h>
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>
#include <libtaomee++/utils/strings.hpp>
#include "./proto/mole2_switch.h"
#include "./proto/mole2_switch_enum.h"

extern int g_log_send_buf_hex_flag;
//-------------------------------------------------------
//函数定义
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    int proto_name(sprite_t* initor, uint32_t rcverid, Cmessage *c_in, uint32_t ret) ;
#include "./proto/mole2_switch_bind_for_cli_request.h"
//-------------------------------------------------------------
//对应的结构体
#include "./proto/mole2_switch.h"
//-------------------------------------------------------------
//命令绑定
typedef   int(*P_DEALFUN_T)(sprite_t* initor, uint32_t rcverid, Cmessage *c_in, uint32_t ret);
Ccmd_map< Ccmd< P_DEALFUN_T> >  g_switch_cmd_map;
//命令map
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    {cmdid, new (c_out), md5_tag,bind_bitmap ,proto_name },

Ccmd< P_DEALFUN_T> g_switch_cmd_list[]={
#include "./proto/mole2_switch_bind_for_cli_request.h"
};
/********新的命令分发 end*******/

int init_switch_handle_funs()
{
    g_switch_cmd_map.initlist(g_switch_cmd_list,sizeof(g_switch_cmd_list)/sizeof(g_switch_cmd_list[0]));
	return 0;
}
// handle chatting across online servers
int chat_across_svr_callback(sprite_t* initor, uint32_t rcverid, const void* buf, int len, uint32_t ret)
{
	int rt = 0;

	uint32_t  rcvid = *((uint32_t*)buf);
	sprite_t* rcver = get_sprite(rcvid);

	if (rcver) {
		send_to_self(rcver, ((uint8_t*)buf) + 4, len - 4, 0);
	} else if(rcvid == 2){
		send_to_trade_map(((uint8_t*)buf) + 4, len - 4);
	} else {
		//*((uint32_t*)buf) = len;
		//db_send_mail(0, rcvid, buf, len);
		rt = COERR_user_offline;
	}

	return rt;
}

int add_cmd_across_svr_callback(sprite_t* initor, uint32_t rcverid, const void* buf, uint32_t len, uint32_t ret)
{
	CHECK_BODY_LEN_GE(len, sizeof(protocol_t) + 4);
	uint32_t  rcvid = *((uint32_t*)buf);
	sprite_t* rcver = get_sprite(rcvid);

	if (rcver) {
		add_cmd_pending_head(rcver, ((uint8_t*)buf) + 4);
	}

	return 0;
}

void noti_all_beast_kill()
{
	uint8_t out[256];
	int i = sizeof(protocol_t);

	PKG_UINT32(out, 0, i);

	PKG_UINT32(out, sys_info_beast, i);
	PKG_UINT32(out, beast_cnt, i);
	PKG_UINT32(out, 0, i);
	PKG_UINT32(out, 0, i);

	KDEBUG_LOG(0, "SEND ALL GET BEAST");

	init_proto_head(out, proto_cli_sys_info, i);
	send_to_all(out, i);
}


int onli_sync_beast_cnt_callback(sprite_t* initor, uint32_t rcverid, const void* buf, uint32_t len, uint32_t ret)
{
	return 0;
	CHECK_BODY_LEN(len, 4);
	int old_cnt = beast_cnt;
	beast_cnt = *(uint32_t *)buf;
	DEBUG_LOG("ONLI\t[%u]", beast_cnt);
	if ((old_cnt < MAX_KILL_BEAST_CNT / 2 && beast_cnt >= MAX_KILL_BEAST_CNT / 2) \
		|| (old_cnt < MAX_KILL_BEAST_CNT && beast_cnt >= MAX_KILL_BEAST_CNT)) {
		noti_all_beast_kill();
	}

	if (old_cnt == -1 && beast_cnt != -1) {
		uint8_t buf[32];
		int i = sizeof(protocol_t);
		PKG_UINT32(buf, beast_cnt, i);
		init_proto_head(buf, cli_sync_beast_kill_cmd, i);
		send_to_all(buf, i);
	}

	return 0;
}


//------------------------------ callbacks ----------------------------------------
int chk_online_across_svr_callback(sprite_t* initor, uint32_t rcverid, const uint8_t* buf, uint32_t len, uint32_t ret)
{
	if (len != 0) {
		CHECK_BODY_LEN_GE(len, 2);

		uint32_t frd_cnt = ntohl(*(uint32_t*)(initor->session + sizeof(protocol_t)));
		int i = 0, domain_cnt;
		UNPKG_H_UINT16(buf, domain_cnt, i);

		int j = 0, domain_id;
		uint32_t online_cnt, online_id, friend_cnt, friend_id;
		for (; j != domain_cnt; ++j) {
			UNPKG_H_UINT16(buf, domain_id, i);
			UNPKG_H_UINT32(buf, online_cnt, i);

			uint32_t k;
			for (k = 0; k != online_cnt; ++k ) {
				UNPKG_H_UINT32(buf, online_id, i);
				UNPKG_H_UINT32(buf, friend_cnt, i);
				uint32_t l;
				for (l = 0; l != friend_cnt; ++l ) {
					UNPKG_H_UINT32(buf, friend_id, i);
					PKG_UINT32(initor->session, friend_id, initor->sess_len);
					PKG_UINT32(initor->session, online_id, initor->sess_len);
					PKG_MAP_ID(initor->session, 0, initor->sess_len);
					frd_cnt ++;
				}
			}
		}

		i = sizeof(protocol_t);
		PKG_UINT32(initor->session, frd_cnt, i);
	}

	init_proto_head(initor->session, initor->waitcmd, initor->sess_len);
	send_to_self(initor, initor->session, initor->sess_len, 1);
	initor->sess_len = 0;
	return 0;
}

int sync_vip_op(sprite_t* initor, uint32_t rcverid, const uint8_t* buf, int len, int ret)
{
	CHECK_BODY_LEN_GE(len, 25);
	DEBUG_LOG("=== SYNC_VIP_OP ==== ");

	int i = 0;
	uint32_t flag, months, isauto, end, begin, vipflag, chanel;

	UNPKG_H_UINT32(buf, flag, i);
	UNPKG_H_UINT32(buf, months, i);
	UNPKG_H_UINT32(buf, end, i);
	UNPKG_H_UINT32(buf, isauto, i);
	UNPKG_H_UINT32(buf, begin, i);
	UNPKG_H_UINT32(buf, chanel, i);
	UNPKG_H_UINT8(buf, vipflag, i);

	ret = 0;
	sprite_t* p = get_sprite(rcverid);
	if (p) {
		if (vipflag) {
			DEBUG_LOG("=== SYNC_VIP_OP ==== ");
			if (ISVIP(p->flag))
				p->flag |= 1 << flag_vip_ever_bit;
			p->flag |= 0x1;
			if(!(p->flag & (1 << flag_vip_1mail))) {
				if(send_dbmail(p,23)) {
					p->flag |= (1 << flag_vip_1mail);
					db_set_flag(NULL, p->id, flag_vip_1mail, 1);
				}
			}

			if (chanel != 18) {
				if (p->vip_end > begin && end > p->vip_end) {
					p->vip_day_5+= (end - p->vip_end)/86400;
					p->vip_day_6+= (end - p->vip_end)/86400;
					p->draw_cnt_gift += (end - p->vip_end) / 86400 / 2;
				} else if (p->vip_end < begin && end > begin) {
					p->vip_day_5+= (end - begin)/86400;
					p->vip_day_6+= (end - begin)/86400;
					p->draw_cnt_gift += (end - begin) / 86400 / 2;
				}

				p->flag |= (1 << flag_vip_charged);

				cli_get_draw_cnt_out cli_out;
				cli_out.draw_cnt = p->draw_cnt_day >= p->draw_max_cnt? 0 : p->draw_max_cnt - p->draw_cnt_day;
				uint32_t vip_draw_cnt = VIP_DRAW_CNT(p);
				uint32_t vip_draw_cnt_use = VIP_DRAW_CNT_USE(p);
				cli_out.vip_draw_cnt = vip_draw_cnt_use >= vip_draw_cnt ? 0 : vip_draw_cnt - vip_draw_cnt_use;
				send_msg_to_self(p, cli_get_draw_cnt_cmd, &cli_out, 0);
			}

			msg_log_bus_mole2_sync_vip(p->id,p->nick);
		} else {
			p->flag &= ~1u;
			p->flag |= (1 << flag_vip_ever_bit);
		}

		p->vip_auto = isauto;
		p->vip_begin = begin;
		p->vip_end = end;
		
		recalc_vip_exp_level(p);
		response_user_vip_info(p);
		//DEBUG_LOG("=== SYNC_VIP_OP ==== ");
		if ( p->vip_day_5 >=90 ) {
			DEBUG_LOG(" p->vip_day_5 ");
			noti_cli_can_get_dragon_out cli_out;
			cli_out.show_vip_niti=0;
			cli_out.count_old = (p->vip_time_1 / 86400 / 90) - p->vip_dragon_cnt;
			cli_out.count_new = (p->vip_time_2 / 86400 / 90) - p->vip_dragon_2;
			cli_out.count_3= (p->vip_time_3 / 86400 / 30);
			cli_out.count_5= (p->vip_day_5/ 90);
			//item_t* pday = cache_get_day_limits(p,ITEM_ID_DRAGON_EGG_6);
			//cli_out.count_6= (p->vip_day_6 >= 30 && pday->count<1)?1:0;
			//cli_out.noti_count_6=(p->vip_noti_flag_6==0 && pday->count<1)?1:0;
			cli_out.count_6= 0;
			cli_out.noti_count_6=0;
			DEBUG_LOG("=== SYNC_VIP_OP ==== 555");
			send_msg_to_self(p, noti_cli_can_get_dragon_cmd, &cli_out, 0);
		}else if(p->vip_day_6 >= 30){
			noti_cli_can_get_dragon_out cli_out;
			cli_out.show_vip_niti=0;
			cli_out.count_old = (p->vip_time_1 / 86400 / 90) - p->vip_dragon_cnt;
			cli_out.count_new = (p->vip_time_2 / 86400 / 90) - p->vip_dragon_2;
			cli_out.count_3= (p->vip_time_3 / 86400 / 30);
			cli_out.count_5= (p->vip_day_5/ 90);
			//item_t* pday = cache_get_day_limits(p,ITEM_ID_DRAGON_EGG_6);
			//cli_out.count_6= (p->vip_day_6 >= 30 && pday->count<1)?1:0;
			//cli_out.noti_count_6=(p->vip_noti_flag_6==0 && pday->count<1)?1:0;
			cli_out.count_6= 0;
			cli_out.noti_count_6=0;
			DEBUG_LOG("=== SYNC_VIP_OP ==== 666");
			send_msg_to_self(p, noti_cli_can_get_dragon_cmd, &cli_out, 0);
		}
		//}
	} else {
		ret = COERR_user_offline;
	}

	KDEBUG_LOG(rcverid, "SYNC VIP\t[%u %u %u %u %u %u %u]", flag, months, end, isauto, begin, chanel, vipflag);
	return ret;
}

int sync_vip_exp_op(sprite_t* initor, uint32_t rcverid, const void* buf, int len, int ret)
{
	CHECK_BODY_LEN_GE(len, 6);

	ret = 0;
	sprite_t* p = get_sprite(rcverid);
	if (p) {
		p->vip_base_exp = *(uint32_t *)buf;
		recalc_vip_exp_level(p);
		response_user_vip_info(p);
	} else {
		ret = COERR_user_offline;
	}
	
	KDEBUG_LOG(rcverid, "SYNC VIP EXP\t[offline=%d]", ret);
	return ret;
}


int kick_user_op(sprite_t* initor, uint32_t rcverid, const uint8_t* buf, int len, int ret)
{
	CHECK_BODY_LEN(len, 8);

	if (ret != 0) {
		KERROR_LOG (0, "invalid ret when kick user: [ret = %d]", ret);
		return ret;
	}
	
	int i = 0;
	uint32_t uid;

	UNPKG_H_UINT32(buf, uid, i);
	sprite_t* p = get_sprite(uid);
	// multiple login
	if (p) {
		KDEBUG_LOG(uid, "relogin other");
		// send multiple login notification
		send_to_self_error(p, proto_cli_login, cli_err_login_from_other, 0);
		// kick the previous login instance offline
		del_sprite(p, 1);
	}

	return 0;
}

int onli_challenge_battle_callback(sprite_t* initor, uint32_t rcverid, const uint8_t* buf, int len, uint32_t ret)
{
	userid_t challengee = *(uint32_t*)initor->session;
	//uint32_t btid=*(uint32_t*)(initor->session+8);
	if (ret) {
		KERROR_LOG(rcverid, "challengee not online\t[%u]", challengee);
		// cancle challenge
		
		// send error message to client
		if (initor->btr_info) {
			batrserv_cancel_battle(initor);
			return 0;
		}
		return send_to_self_error(initor, initor->waitcmd, cli_err_user_offline, 1);
	}
	KDEBUG_LOG(rcverid, "ONLINE CHALLENGE BATTLE CALLBACK \t[offline=%d  challegee=%u]", ret,challengee);
	notify_self_team_challenging(initor, challengee);
	if (initor->pk_switch_is_on()) {
		response_proto_uint32(initor, initor->waitcmd, 3, 1, 0);
	} else{
		response_proto_uint32(initor, initor->waitcmd, 1, 1, 0);
	}
	
	return 0;
}

int onli_apply_pk_callback(sprite_t* initor, uint32_t rcverid, const uint8_t* buf, int len, uint32_t ret)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t result = ret ? 0 : *(uint32_t *)buf;

	sprite_t* p = get_sprite(rcverid);
	if (p) {	
		KDEBUG_LOG(p->id, "APPLY SWITCH CALLBACK\t[%u]", result);
		uint8_t out[32];
		int i = sizeof(protocol_t);
		PKG_UINT32(out, result, i);
		init_proto_head(out, p->waitcmd, i);
		return send_to_team(p, out, i, 1);
	}
	return 0;
}

int onli_cancel_pk_apply_callback(sprite_t* initor, uint32_t rcverid, const uint8_t* buf, int len, uint32_t ret)
{
	CHECK_BODY_LEN(len, 0);
	KDEBUG_LOG(rcverid, "CANCEL PK APPLY CALLBACK\t[%d]", ret);

	sprite_t* p = get_sprite(rcverid);
	if (p) {
		if (ret) {
			return send_to_self_error(p, p->waitcmd, ret, 1);
		} 
		p->team_state = 0;
		int i = sizeof(protocol_t);
		init_proto_head(msg, p->waitcmd, i);
		return send_to_team(p, msg, i, 1);
	}
	return 0;
}

int onli_clear_btr_info_callback(sprite_t* initor, uint32_t rcverid, const uint8_t* buf, int len, uint32_t ret)
{
	CHECK_BODY_LEN(len, 8);
	battle_id_t btid = *(battle_id_t *)buf;
	KDEBUG_LOG(rcverid, "CANCEL PK APPLY CALLBACK\t[%u %u]", high32_val(btid), low32_val(btid));

	sprite_t* p = get_sprite(BATTLE_USER_ID(btid));
	if (p && p->btr_info) {
		batrserv_cancel_battle(p);
	}
	return 0;
}

int sync_info_to_user_callback(sprite_t* initor, uint32_t rcverid, const uint8_t* buf, int len, uint32_t ret)
{
	CHECK_BODY_LEN(len, sizeof(syn_info_t));
	syn_info_t *p_syn_info= (syn_info_t*)buf;
	sprite_t* p = get_sprite(p_syn_info->obj_userid);
	if (p ) {
		if (p_syn_info->opt_type==1  ){//通知错误码
			send_to_self_error(p, proto_cli_challenge,p_syn_info->v1 , 0);
			if (p->btr_info) {
				batrserv_cancel_battle(p);
				del_user_battle_info(p);
			}
		}

	}
	return 0;
}


int onli_auto_challenge_callback(sprite_t* initor, uint32_t rcverid, const uint8_t* buf, int len, uint32_t ret)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t teamid = *(uint32_t *)buf;
	uint32_t type = *(uint32_t *)(buf + 4);

	sprite_t* p = get_sprite(rcverid);

	if (p && !p->btr_info) {
		KDEBUG_LOG(p->id, "SWITCH AUTO CHALLENGE CALLBACK\t[%u %u %u]", rcverid, teamid, type);

		init_challenger_battle(p, 1, be_pk);
		p->btr_info->pkteam = teamid;
		p->team_state = 0;
		/*if(type==3){
			p->gvg_challenge_tag=1;
		}*/
		batrserv_challenge_battle(p, teamid, (type == 2), proto_cli_challenge);
	} else {
		KDEBUG_LOG(rcverid, "user not exsited or in battle\t[%u]", p ? p->id : 0);
	}
	return 0;
}


int onli_auto_fight_callback(sprite_t* initor, uint32_t rcverid, const uint8_t* buf, int len, uint32_t ret)
{
	CHECK_BODY_LEN(len, 8);
	battle_id_t btid = *(battle_id_t *)buf;
	//uint32_t type =*(uint32_t *)buf
	sprite_t* p = get_sprite(rcverid);
	if (p && !p->btr_info) {
		KDEBUG_LOG(p->id, "SWITCH AUTO FIGHT CALLBACK\t[%u %u]", high32_val(btid), low32_val(btid));
		/*
		if(type==3){
			p->gvg_challenge_tag=1;
		}
		*/
		init_challengee_battle(p, btid, 1);
		batrserv_accept_battle(p);
		p->team_state = 0;
		send_to_switch(NULL, COCMD_onli_auto_fight, 4, &p->btr_info->pkteam, p->id);
	} else {
		KDEBUG_LOG(rcverid, "user not exsited or in battle\t[%u %u]", high32_val(btid), low32_val(btid));
	}
	return 0;
}

int challenge_battle_onli_callback(sprite_t* initor, uint32_t rcverid, const uint8_t* buf, int len, uint32_t ret)
{
	CHECK_BODY_LEN(len, 8);
	battle_id_t btid = *(battle_id_t *)buf;
	sprite_t* challged = get_sprite(rcverid);
	if (challged) {
		DEBUG_LOG("find :len=%u",len );
		if (challged->pk_switch_is_on() ) {
			DEBUG_LOG("find:len=%u",len );
			int ret=0;
			if (challged->btr_team && !TEAM_LEADER(challged)) {
				ret=cli_err_other_not_leader;
			}
			if (team_busy(challged)) {
				ret=cli_err_user_busy;
			} 

			//ret=cli_err_user_busy;
			
			if (ret==0){
				init_challengee_battle(challged, btid, 0);
				batrserv_accept_battle(challged);
			}else{
				DEBUG_LOG("SYNC INFO");
				syn_info_t  syn_info={ };
				syn_info.obj_userid=low32_val(btid );
				syn_info.src_userid=challged->id;
				syn_info.opt_type=1;//表示错误码
				syn_info.v1=ret;//表示错误码
				syn_info_to_user(challged ,syn_info);
				return 0;
			}


		} else {

			KDEBUG_LOG(challged->id, "BATTLE ONLINE NOTIFY OTHER LEADER\t[%u %u]", high32_val(btid), low32_val(btid));
			notify_other_leader_challenging_state(challged, btid, 1);
		}
	
		//TODO
	}
	KDEBUG_LOG(rcverid, "CHALLENGE BATTLE CALLBACK\t[btid=%u %u]", high32_val(btid), low32_val(btid));
	return 0;
}

int notify_across_uint32_callback(sprite_t* initor, uint32_t rcverid, const uint8_t* buf, int len, uint32_t ret)
{
	CHECK_BODY_LEN(len, 12);
	uint32_t uid;
	uint32_t cmd;
	uint32_t value;
	int recvlen = 0;
	int sendlen = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, uid, recvlen);
	UNPKG_H_UINT32(buf, cmd, recvlen);
	UNPKG_H_UINT32(buf, value, recvlen);
	sprite_t* p = get_sprite(uid);
	if (p) {
		uint8_t	msg_buf[cli_proto_max_len];
		PKG_UINT32(msg_buf, value, sendlen);
		init_proto_head(msg_buf, cmd, sendlen);
		send_to_self(p, msg_buf, sendlen, 0);
	}
	return 0;
}

static inline int
broadcast_msg_op(sprite_t* initor, uint32_t rcverid, const uint8_t* buf, int len, int ret)
{
	CHECK_BODY_LEN_GE(len, 10);

	int i = 0;
	uint32_t uid, msg_type;
	uint16_t msg_len;

	UNPKG_H_UINT32(buf, uid, i);
	UNPKG_H_UINT32(buf, msg_type, i);
	UNPKG_H_UINT16(buf, msg_len, i);

	int len2 = len - 10;
	CHECK_BODY_LEN(len2, msg_len);
	CHECK_BODY_LEN_LE(msg_len, 400);

	ret = 0;
	if (!uid) {
		send_to_all((void*)(buf + 10), msg_len);
	} else {
		sprite_t* p = get_sprite(uid);
		if (p) {
			send_to_self(p, ((uint8_t*)buf) + 10, msg_len, 0);
		} else {
			ret = COERR_user_offline;
		}
	}

	DEBUG_LOG("BROADCAST MSG\t[recvid=%u msgtype=%u msglen=%d offline=%d]", uid, msg_type, msg_len, ret);
	return ret;
}

int switch_add_luckystar_exchange_cnt(sprite_t* initor, uint32_t rcverid, Cmessage *c_out, uint32_t ret)
{
	switch_add_luckystar_exchange_cnt_out *p_out=P_OUT;
	DEBUG_LOG("switch_add_luckystar_exchange_cnt %u %u ",p_out->ret,rcverid);
	
	sprite_t* p = get_sprite(rcverid);
	if( !p ){
		return 0;
	}
	if(p_out->ret){
		return send_to_self_error(p, cli_exchange_lucky_star_cmd, cli_err_item_cnt_max, 1);
	}
	return get_luck_star_reward(p);
}

int switch_to_user_callback(sprite_t* initor, uint32_t rcverid, const uint8_t * buf, uint32_t len, uint32_t ret)
{
	return 0;
}

int handle_switch_return(switch_proto_t* dbpkg, int len)
{
	sprite_t* p = NULL;
	if (dbpkg->seq) {
		int waitcmd = dbpkg->seq & 0xFFFF;
		int conn    = dbpkg->seq >> 16;
		// to prevent reclosing the connection, it returns 0
		if ( !(p = get_sprite_by_fd(conn)) || (p->waitcmd != waitcmd) ) {
			ERROR_RETURN(("connection has been closed, id=%u, cmd=0x%x,0x%x, seq=%u",
					dbpkg->sender_id, dbpkg->cmd, waitcmd, dbpkg->seq), 0);
		}
	}

	int err = -1;

	DEBUG_LOG("PKG FROM SWITCH:cmd=%u",dbpkg->cmd);
	Ccmd< P_DEALFUN_T> * p_cmd_item =g_switch_cmd_map.getitem(dbpkg->cmd);
	if (p_cmd_item ){
	    Cmessage * msg;
	    if (dbpkg->ret==0){//成功
	        //还原对象
	        p_cmd_item->proto_pri_msg->init( );
	        byte_array_t in_ba ( ((char *)dbpkg->body), len-sizeof(switch_proto_t));
	        //失败
	        if (!p_cmd_item->proto_pri_msg->read_from_buf(in_ba)) {
	            DEBUG_LOG( "err:pkg 111 ");
	            return 0;
	        }
	        //客户端多上传报文
	        if (!in_ba.is_end()) {
	            DEBUG_LOG("cerr:pkg 2222 ");
	            return 0;
	        }
	        msg=p_cmd_item->proto_pri_msg;
	    } else {
	        msg= NULL;
	    }
		//DEBUG_LOG("switch func====opid:%u",dbpkg->cmd);
		p_cmd_item->func(p, dbpkg->sender_id, msg, dbpkg->ret);
	}else{
#define DO_MESSAGE(n_, func_) \
	case n_: err = (func_)(p, dbpkg->sender_id, dbpkg->body, len - sizeof(switch_proto_t), dbpkg->ret); break
		switch (dbpkg->cmd) {
			DO_MESSAGE(COCMD_kick_user, kick_user_op);
			DO_MESSAGE(COCMD_broadcast_msg, broadcast_msg_op);
			DO_MESSAGE(COCMD_sync_vip, sync_vip_op);
			DO_MESSAGE(COCMD_sync_vip_exp, sync_vip_exp_op);

			DO_MESSAGE(COCMD_chat_across_svr, chat_across_svr_callback);
			DO_MESSAGE(COCMD_add_cmd_across_svr, add_cmd_across_svr_callback);
			DO_MESSAGE(COCMD_onli_sync_beast_cnt, onli_sync_beast_cnt_callback);

			DO_MESSAGE(COCMD_chk_if_online, chk_online_across_svr_callback);
			DO_MESSAGE(COCMD_onli_challenge_battle, onli_challenge_battle_callback);
			DO_MESSAGE(COCMD_challenge_battle_onli, challenge_battle_onli_callback);
			DO_MESSAGE(COCMD_notify_across, notify_across_uint32_callback);
			DO_MESSAGE(COCMD_onli_apply_pk, onli_apply_pk_callback);
			DO_MESSAGE(COCMD_onli_auto_challenge, onli_auto_challenge_callback);
			DO_MESSAGE(COCMD_onli_auto_fight, onli_auto_fight_callback);
			DO_MESSAGE(COCMD_onli_cancel_pk_apply, onli_cancel_pk_apply_callback);
			DO_MESSAGE(COCMD_onli_clear_btr_info, onli_clear_btr_info_callback);
			DO_MESSAGE(COCMD_sync_info_to_user, sync_info_to_user_callback);
			DO_MESSAGE(COCMD_switch_to_user, switch_to_user_callback);
#undef DO_MESSAGE
			default:
				KERROR_LOG(0, "Unsupported CMD=0x%X", dbpkg->cmd);
				err = -1;
				break;
		}
	}
	

	return err;
}

//--------------------------------------------------------------------------
