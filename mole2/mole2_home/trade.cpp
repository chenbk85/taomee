/*
 * =========================================================================
 *
 *        Filename: trade.cpp
 *
 *        Version:  1.0
 *        Created:  2012-02-16 12:19:07
 *        Description: 交易相关协议 
 *
 *        Author:  francisco (francisco@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */
#include  <time.h>
#include  <libtaomee++/proto/proto_util.h>
#include  "proto.h"
#include  "util.h"
#include  "./proto/mole2_home_enum.h"
#include  "./proto/mole2_home.h"
#include  "market.h"

/* ----------------------------------------------------------------------------*/
/**
 * @brief  发给一个用户
 */
/* ----------------------------------------------------------------------------*/
static int send_msg_to_user(sprite_ol_t*p ,Cmessage* msg, uint32_t opid ,uint32_t ret)
{
	DEBUG_LOG("Cmarket::send_msg_to_a_user(homeid = %lu, userid = %u, opid = %u ret=%u)",p->homeid, p->id, opid,ret);
	home_proto_t head_pkg;
	init_pkg_proto(&(head_pkg) ,0,61002,0, p->homeid , opid);
	head_pkg.id=p->id;
	head_pkg.ret=ret;
	head_pkg.onlineid= p->onlineid;
	if ( all_fds[p->onlineid]){
		send_msg_to_client(all_fds[p->onlineid], (char*)&head_pkg, msg);
	}
	DEBUG_LOG("START SEND PKG" );
	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  发给本房间内所有用户 除了某个用户
 */
/* ----------------------------------------------------------------------------*/
static int send_msg_to_room_except_uid( home_t* p_home, uint32_t roomid ,Cmessage* msg, uint32_t optid ,uint32_t ret, uint32_t id)
{
	std::set<uint32_t>::iterator it;
	for( it=g_market.rooms[roomid].enter_users.begin() ; it != g_market.rooms[roomid].enter_users.end() ; it++  ){
		uint32_t uid=*it;
		sprite_ol_t *lp=(sprite_ol_t*) g_hash_table_lookup(p_home->sprites, &uid);
		if( uid != id && lp){
			send_msg_to_user(lp,msg,optid,ret);
		}
	}
	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  检查某个房间是否已经满，如果未满则加入房间的等待进入队列
 * 在 proto_enter_home_op
 */
/* ----------------------------------------------------------------------------*/
int home_trade_get_mapid(home_t* p_home, sprite_ol_t* p , Cmessage* c_in)
{
	home_trade_get_mapid_in *p_in=P_IN;
	home_trade_get_mapid_out out;
	DEBUG_LOG("home_trade_get_mapid:uid=%u roomid=%u",p->id,p_in->roomid);
	if( p_in->roomid == 0xffffffff ){
		for( uint32_t loop=0 ; loop<g_market.rooms_cnt ; loop++ ){
			if(!g_market.rooms[loop].is_full()){
				g_market.add_user_to_market(p->id,loop);
				out.roomid=loop;
				DEBUG_LOG("get_mapid:roomid=%u",loop);
				//printroom(loop);
				return send_msg_to_user(p,&out,home_trade_get_mapid_cmd,0);
			}
		}
		return send_msg_to_user(p,&out,home_trade_get_mapid_cmd,-1);
	}else{
		if(!g_market.rooms[p_in->roomid].is_full()) { 
			g_market.add_user_to_market(p->id,p_in->roomid);	
			out.roomid=p_in->roomid;
			return send_msg_to_user(p,&out,home_trade_get_mapid_cmd,0);
		}
	}
	return send_msg_to_user(p,&out,home_trade_get_mapid_cmd,-1);
}
	
int home_trade_leave_map(home_t* p_home, sprite_ol_t* p , Cmessage* c_in)
{
	g_market.del_user_from_market(p->id);
	DEBUG_LOG("del user[%u]",p->id);
	return 0;
}
/* ----------------------------------------------------------------------------*/
/**
 * @brief  修改店铺状态
 */
/* ----------------------------------------------------------------------------*/
int home_trade_change_shop(home_t* p_home, sprite_ol_t* p , Cmessage* c_in)
{
	home_trade_change_shop_in *p_in=P_IN;
	KDEBUG_LOG(p->id,"home_trade_change_shop[cmd=%u roomid=%u sid=%u]",p_in->cmd, p_in->roomid, p_in->sid);
	home_trade_noti_shop_state_out h_out;
	h_out.cmd=p_in->cmd;
	h_out.uid=p->id;
	h_out.sid=p_in->sid;
	switch (p_in->cmd){
		case 0:{//register
				uint32_t ret=0;
				if(!g_market.rooms[p_in->roomid].check_shop_valid(p_in->sid)){
					ret=-1;	
				}else{
					g_market.rooms[p_in->roomid].register_shop(p->id,p_in->sid);
				}
				send_msg_to_user(p,&h_out,home_trade_change_shop_cmd,ret);
				h_out.state=shop_register;
				h_out.prof=p->sprite_info.profession;
				memcpy(h_out.nick,p_in->nick,sizeof(h_out.nick));
				memcpy(g_market.rooms[p_in->roomid].shops[p_in->sid].shopname,p_in->nick,sizeof(p_in->nick));
				break;
			   }
		case 1://hault
			h_out.state=shop_register;
			g_market.rooms[p_in->roomid].hault_shop(p_in->sid);
			break;
		case 2 ://open
			h_out.state=shop_open;
			g_market.rooms[p_in->roomid].open_shop(p_in->sid);
			break;
		case 3 ://close
			g_market.rooms[p_in->roomid].close_shop(p_in->sid);
			h_out.state=shop_init;
			break;
		case 5://change nick
			memcpy(g_market.rooms[p_in->roomid].shops[p_in->sid].shopname,p_in->nick,sizeof(p_in->nick));
			h_out.prof=p->sprite_info.profession;
			h_out.state=g_market.rooms[p_in->roomid].shops[p_in->sid].state;
			memcpy(h_out.nick,p_in->nick,sizeof(h_out.nick));
			send_msg_to_user(p,&h_out,home_trade_noti_shop_state_cmd,0);
			break;
		default :
			break;
	}
	return send_msg_to_room_except_uid(p_home, p_in->roomid, &h_out, home_trade_noti_shop_state_cmd, 0,p->id);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  拉取商品列表 转发到其他online
 */
/* ----------------------------------------------------------------------------*/
int home_trade_get_items(home_t* p_home, sprite_ol_t* p , Cmessage* c_in)
{
	home_trade_get_items_in *p_in=P_IN;
	home_trade_get_items_in out;
	sprite_ol_t *lp=(sprite_ol_t*) g_hash_table_lookup(p_home->sprites, &p_in->uid);
	KDEBUG_LOG(p->id,"home_trade_get_items[recvid=%u]",p_in->uid);
	if(lp){
		out.uid=p->id;
		send_msg_to_user(lp,&out,home_trade_get_items_cmd,0);	
	}else{
		send_msg_to_user(p,&out,home_trade_noti_items_cmd,-1);	
	}
	return 0;
}
/* ----------------------------------------------------------------------------*/
/**
 * @brief  拉取商品列表返回  发回给用户
 */
/* ----------------------------------------------------------------------------*/
int home_trade_noti_items(home_t* p_home, sprite_ol_t* p , Cmessage* c_in)
{
	home_trade_noti_items_in *p_in=P_IN;
	sprite_ol_t *lp=(sprite_ol_t*) g_hash_table_lookup(p_home->sprites, &p_in->uid);
	KDEBUG_LOG(p->id,"home_trade_noti_items[recvid=%u lefttime=%u]",p_in->uid,p_in->left_time);
	uint32_t ret=p_in->left_time==0xFFFFFFFF?-1:0;
	if(lp){
		p_in->uid=p->id;
		send_msg_to_user(lp,p_in,home_trade_noti_items_cmd,ret);	
	}
	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  拉取店铺的状态信息
 */
/* ----------------------------------------------------------------------------*/
int home_trade_get_shop_info(home_t* p_home, sprite_ol_t* p , Cmessage* c_in)
{
	home_trade_get_shop_info_in *p_in=P_IN;
	home_trade_get_shop_info_out out;
	KDEBUG_LOG(p->id,"home_trade_get_shop_info[ roomid=%u]",p_in->roomid);
	for( uint8_t loop=0 ; loop<MAX_SHOP_CNT ; loop++ ){
		memset(out.info[loop].nick,0,sizeof(out.info[loop].nick));
		Shop  * p_cur_shop= &(g_market.rooms[p_in->roomid].shops[loop]);

		DEBUG_LOG("shop_info[loop=%u state=%u %u %lu %s]",loop, p_cur_shop->state,
				p_cur_shop->uid,sizeof(out.info[loop].nick),out.info[loop].nick);
		
		if( p_cur_shop->state){
			sprite_ol_t *lp=(sprite_ol_t*) g_hash_table_lookup(p_home->sprites,&(p_cur_shop->uid));
			if( lp ){
				out.info[loop].state=p_cur_shop-> state;
				out.info[loop].uid=p_cur_shop->uid;
				out.info[loop].prof=p->sprite_info.profession;
				memcpy(out.info[loop].nick,p_cur_shop->shopname,sizeof(out.info[loop].nick));
			}
		}
	}
	return send_msg_to_user(p,&out,home_trade_get_shop_info_cmd,0);	
}


/* ----------------------------------------------------------------------------*/
/**
 * @brief  买东西  装发到其他online的卖家
 */
/* ----------------------------------------------------------------------------*/
int home_trade_buy_item(home_t* p_home, sprite_ol_t* p , Cmessage* c_in)
{
	home_trade_buy_item_in* p_in=P_IN;
	sprite_ol_t *lp=(sprite_ol_t*) g_hash_table_lookup(p_home->sprites, &p_in->uid);
	KDEBUG_LOG(p->id,"home_trade_buy_item[recvid=%u]",p_in->uid);
	if(lp){
		p_in->uid=p->id;
		send_msg_to_user(lp,p_in,home_trade_buy_item_cmd,0);	
	}else{
		send_msg_to_user(p,p_in,home_trade_buy_item_callback_cmd,cli_err_user_offline);	
	}
	return 0;

}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 买东西返回  发回到买家的online 
 */
/* ----------------------------------------------------------------------------*/
int home_trade_buy_item_callback(home_t* p_home, sprite_ol_t* p , Cmessage* c_in)
{
	home_trade_buy_item_callback_in* p_in=P_IN;
	sprite_ol_t *lp=(sprite_ol_t*) g_hash_table_lookup(p_home->sprites, &p_in->uid);
	KDEBUG_LOG(p->id,"home_trade_buy_item_callback[recvid=%u %u]",p_in->uid,p_in->itemid);
	if(lp){
		if( p_in->ret!=0){
			return send_msg_to_user(lp,p_in,home_trade_buy_item_callback_cmd, p_in->ret);	
		}
		p_in->uid=p->id;
		send_msg_to_user(lp,p_in,home_trade_buy_item_callback_cmd,0);	
	}	
	return 0;
}
