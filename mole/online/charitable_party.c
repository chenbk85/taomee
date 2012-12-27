/*
 * =====================================================================================
 *
 *       Filename:  charitable_party.c
 *
 *    Description:  爱心天使慈善派对
 *
 *        Version:  1.0
 *        Created:  12/26/2011 01:52:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericlee
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include <math.h>
#include "item.h"
#include "central_online.h"
#include "exclu_things.h"
#include "charitable_party.h"
#include "small_require.h"
#include "service.h"

#define CHP_DONATE_MAP_ID			142
#define CHP_BADGE_ITEM_ID			190902
#define CHP_AUCTION_UPDATE_TIME_INI	30
#define CHP_AUCTION_UPDATE_TIME		(15*60)
#define CHP_TIMER_UPDATE_SERVER_ID	6
#define CHP_ANGEL_SEED_ID_BEGIN		1353201
#define CHP_ANGEL_SEED_ID_END		1353380
#define CHP_ITEM_CHARGE_BASE		10

//不可捐献的物品ID
static uint32_t donate_ban_list[] = { 1353304, 1353302, 1353299, 1353294, 1353292, 1353280, 1353273, 1353266, 1353275,
	1353274, 1353260, 1353244, 1353204, 1353222, 1353236, 1353308, 1270066, 1270072, 1270073, 1270078, 1270079, 1270033,	1270038, 1270010, 1270058, 1230083, 1230058, 1230057, 1230051, 1230054, 1593069};
static uint32_t donate_ban_size = sizeof(donate_ban_list)/sizeof(uint32_t);

//稀有物品ID
static uint32_t donate_rare_item_list[] ={ 1270041, 1270040, 1593034, 1593037, 1593065, 1353296, 1353271, 1353288, 1353290,
	1353303, 1353305, 1353307, 1353306 };

static uint32_t donate_rare_item_size = sizeof(donate_rare_item_list)/sizeof(uint32_t);

static chp_timer_t my_timer;

extern config_cache_t config_cache;

int charparty_donate_item_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t item_id;
	uint32_t item_cnt;
	CHECK_BODY_LEN( len, sizeof(item_cnt) + sizeof(item_id) );
	int j = 0;
	UNPKG_UINT32( body, item_id, j );
	UNPKG_UINT32( body, item_cnt, j );
	if( item_cnt < 1 )
	{
		response_proto_uint32_uint32( p, p->waitcmd, CHPDR_INVALID_ITEM, 0, 0 );
		return 0;
	}
	chp_donate_info_t* info = (chp_donate_info_t*)p->session;
	//check valid item
	item_kind_t* ik = find_kind_of_item( item_id );
	if( !ik )
	{
		response_proto_uint32_uint32( p, p->waitcmd, CHPDR_INVALID_ITEM, 0, 0 );
		return 0;
	}
	else
	{
		item_t* it = get_item( ik, item_id );
		if( !it )
		{
			response_proto_uint32_uint32( p, p->waitcmd, CHPDR_INVALID_ITEM, 0, 0 );
			return 0;
		}

		int i;
		for( i = 0; i < donate_ban_size; ++i )
		{
			if( item_id == donate_ban_list[i] )
			{
				response_proto_uint32_uint32( p, p->waitcmd, CHPDR_INVALID_ITEM, 0, 0 );
				return 0;
			}
		}
		switch( ik->kind )
		{
		//天使种子
		case ANGEL_ITEM_KIND:
			{
				if( item_id >= CHP_ANGEL_SEED_ID_BEGIN && item_id <= CHP_ANGEL_SEED_ID_END )
				{
					info->type = CHPDT_ANGEL;
				}
				else
				{
					response_proto_uint32_uint32( p, p->waitcmd, CHPDR_INVALID_ITEM, 0, 0 );
					return 0;
				}
			}
			break;
		case CUTE_PIG_KIND:
			{
				info->type = CHPDT_PIG;
			}
			break;
		case HOMELAND_SEED_KIND:
			//农场种子
			{
				info->type = CHPDT_ARG;
			}
			break;
		case ANIMAL_BABY_ITEM:
			//牧场
			{
				info->type = CHPDT_MU;
			}
			break;
		default:
			{
				response_proto_uint32_uint32( p, p->waitcmd, CHPDR_INVALID_ITEM, 0, 0 );
				return 0;
			}
			break;
		}
	}
	info->user_id = p->id;
	info->item_id = item_id;
	info->item_cnt = item_cnt;

	//check if item exist
	uint8_t db_buff[128] = {0};
	uint32_t item_need[] = { item_id, CHP_BADGE_ITEM_ID};
	uint32_t item_kind = sizeof(item_need)/sizeof(uint32_t);
	int send_len = 0;
	PKG_H_UINT32(db_buff, 2, send_len);
	PKG_H_UINT32(db_buff, item_kind, send_len);
	int i;
	for( i = 0; i < item_kind; ++i )
	{
		PKG_H_UINT32( db_buff, item_need[i], send_len );
		pkg_item_kind( p, db_buff, item_need[i], &send_len );
	}
	return send_request_to_db(SVR_PROTO_GET_ITEM_ARRAY, p, send_len, db_buff, p->id);
}

int charparty_check_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t count;
	CHECK_BODY_LEN_GE( len, sizeof(count) );
	uint32_t item_id;
	uint32_t item_cnt;
	uint32_t badge_id;
	uint32_t badge_cnt;
	int j = 0;
	UNPKG_H_UINT32( buf, count, j );

	CHECK_BODY_LEN( len, sizeof(count) + count * sizeof(uint32_t) * 2 );
	UNPKG_H_UINT32( buf, item_id, j);
	UNPKG_H_UINT32( buf, item_cnt, j);
	chp_donate_info_t* info = (chp_donate_info_t*)p->session;
	if( info->item_id != item_id || item_cnt < info->item_cnt )
	{
		response_proto_uint32_uint32( p, p->waitcmd, CHPDR_NOT_ENOUGH_ITEMS, 0, 0 );
		return 0;
	}

	if( count == 2 )
	{
		UNPKG_H_UINT32( buf, badge_id, j );
		UNPKG_H_UINT32( buf, badge_cnt, j );
	}
	else
	{
		badge_cnt = 0;
	}

	uint32_t badge_award_cnt = info->item_cnt / 3;
	if( 0 == badge_award_cnt )
	{
		badge_award_cnt = 1;
	}
	*(uint32_t*)( p->session + sizeof(chp_donate_info_t) ) = badge_award_cnt;

	//扣除物品 增加奖章
	{
		int sendlen = 0;
		uint8_t buff[128];
		PKG_H_UINT32( buff, 1, sendlen );
		PKG_H_UINT32( buff, 1, sendlen );
		PKG_H_UINT32( buff, 203, sendlen );
		PKG_H_UINT32( buff, 0, sendlen );
		item_kind_t* ik = find_kind_of_item(info->item_id);
		item_t* it = get_item( ik, info->item_id );
		pkg_item_kind( p, buff, info->item_id, &sendlen );
		PKG_H_UINT32( buff, info->item_id, sendlen );
		PKG_H_UINT32( buff, info->item_cnt, sendlen );

		ik = find_kind_of_item(CHP_BADGE_ITEM_ID);
		it = get_item( ik, CHP_BADGE_ITEM_ID );
		if( badge_cnt >= it->max )
		{
			badge_award_cnt = 0;
		}					 
		else
		{
			if( badge_cnt + badge_award_cnt > it->max )
			{
				badge_award_cnt = it->max - badge_cnt;
			}
		}
		pkg_item_kind( p, buff, CHP_BADGE_ITEM_ID, &sendlen );
		PKG_H_UINT32( buff, CHP_BADGE_ITEM_ID, sendlen );
		PKG_H_UINT32( buff, badge_award_cnt, sendlen );
		PKG_H_UINT32( buff, it->max, sendlen );
		send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, p->id );
	}

	//统计 捐献的人数
	{
		uint32_t msgbuff[2]= {p->id, 1};
		msglog(statistic_logfile, 0x0409C2D6, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}

	uint8_t db_buf[128] = {0};
	int msg_len = 0;
	PKG_H_UINT32( db_buf, info->type, msg_len );
	PKG_H_UINT32( db_buf, info->item_id, msg_len );
	PKG_H_UINT32( db_buf, info->item_cnt, msg_len );
	PKG_H_UINT32( db_buf, badge_cnt + badge_award_cnt, msg_len );
	PKG_STR( db_buf, p->nick, msg_len, sizeof(p->nick) );
	return send_request_to_db( SVR_PROTO_DONATE_ITEM, p, msg_len, db_buf, p->id );
}

int charparty_donate_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t result;
	CHECK_BODY_LEN( len, 0 );
	result = CHPDR_OK;

	chp_donate_info_t* info = (chp_donate_info_t*)p->session;
	uint32_t badge_award_cnt = *(uint32_t*)( p->session + sizeof(chp_donate_info_t) );

	response_proto_uint32_uint32( p, p->waitcmd, result, badge_award_cnt, 0 );

	//here we need broadcast to entire servers

	uint32_t msg_type = 0;
	int i;
	for( i = 0; i < donate_rare_item_size; ++i )
	{
		if( donate_rare_item_list[i] == info->item_id )
		{
			msg_type = 1;
			break;
		}
	}
	uint8_t msg_buf[128] = {0};
	int msg_len = sizeof(protocol_t);
	PKG_UINT32( msg_buf, msg_type, msg_len );
	PKG_UINT32( msg_buf, p->id, msg_len );
	PKG_UINT32( msg_buf, info->item_id, msg_len );
	PKG_UINT32( msg_buf, info->item_cnt, msg_len );
	PKG_STR( msg_buf, p->nick, msg_len, sizeof(p->nick) );
	init_proto_head( msg_buf, PROTO_CHP_DONATE_NOTICE, msg_len );

	switch( msg_type )
	{
	case 0:
		{
			//DEBUG_LOG( "Player:[%u] Donate Item Broad to Map ItemID:[%u]", info->user_id, info->item_id );
			map_t* map_id = get_map( CHP_DONATE_MAP_ID );
			send_to_map2( map_id, msg_buf, msg_len );
		}
		break;
	case 1:
		{
			//DEBUG_LOG( "Player:[%u] Donate Item Broadcast to All Server ItemID:[%u]", info->user_id, info->item_id );
			tell_flash_some_msg_across_svr( NULL, CBMT_CHP_DONATE_ITEM, msg_len, (char*)msg_buf );
		}
		break;
	}
	return 0;
}

int charparty_broadcast_donate_info(const void* buf, int len)
{
	//DEBUG_LOG( "Donate Rare Item Broadcast to all players" );
	send_to_all_players( NULL, (uint8_t*)buf, len, 0 );
	return 0;
}

int charparty_get_auction_list_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t list_type;		//1：拉取竞拍列表， 2：拉取上一轮结果
	CHECK_BODY_LEN( len, sizeof(list_type) );
	int j = 0;
	UNPKG_UINT32( body, list_type, j );
	if( list_type != 2 && list_type != 1 )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_chp_invalid_cmd, 1 );
	}
	else
	{
		return send_request_to_db( SVR_PROTO_GET_AUCTION_LIST, p, sizeof(list_type), &list_type, p->id );
	}
}

int charparty_get_auction_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t time_left;
	uint32_t auction_cnt;
	uint32_t expect_len = sizeof(time_left) + sizeof(auction_cnt);
	CHECK_BODY_LEN_GE( len, expect_len );
	int j = 0;
	UNPKG_H_UINT32( buf, time_left, j );
	if( time_left > CHP_AUCTION_UPDATE_TIME )
	{
		time_left = 0;
	}
	else
	{
		time_left = CHP_AUCTION_UPDATE_TIME - time_left;
	}
	
	UNPKG_H_UINT32( buf, auction_cnt, j );
	expect_len += auction_cnt * sizeof(auction_info_t);

	CHECK_BODY_LEN( len, expect_len );
	int msg_len = sizeof(protocol_t);
	PKG_UINT32( msg, time_left, msg_len );
	PKG_UINT32( msg, auction_cnt, msg_len );
	auction_info_t* auction_info;
	int i;
	for( i = 0; i < auction_cnt; ++i )
	{
		auction_info = (auction_info_t*)(buf + j );
		j += sizeof(*auction_info);
		PKG_UINT32( msg, auction_info->item_id, msg_len );
		PKG_UINT32( msg, auction_info->contributor_id, msg_len );
		PKG_UINT32( msg, auction_info->buyer_id, msg_len );
		PKG_UINT32( msg, auction_info->cur_prize, msg_len );
		PKG_STR( msg, auction_info->contributor_nick, msg_len, sizeof(auction_info->contributor_nick) );
		PKG_STR( msg, auction_info->buyer_nick, msg_len, sizeof(auction_info->buyer_nick) );
	}
	init_proto_head( msg, p->waitcmd, msg_len );
	return send_to_self( p, msg, msg_len, 1 );
}

int charparty_auction_item_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	typedef struct auction_cmd_s
	{
		uint32_t item_id;
		uint32_t my_prize;
	}auction_cmd_t;
	CHECK_BODY_LEN( len, sizeof(auction_cmd_t) );
	auction_cmd_t cmd;
	int j = 0;
	UNPKG_UINT32( body, cmd.item_id, j );
	UNPKG_UINT32( body, cmd.my_prize, j );
	if( cmd.my_prize <= CHP_ITEM_CHARGE_BASE )
	{
		response_proto_uint32( p, p->waitcmd, CHPAR_LESS_MONEY, 0 );
		return 0;
	}

	if( p->yxb >= cmd.my_prize )
	{
		*(uint32_t*)p->session = cmd.my_prize;
		uint8_t db_buf[128] = {0};
		int j = 0;
		PKG_H_UINT32( db_buf, cmd.item_id, j );
		PKG_H_UINT32( db_buf, cmd.my_prize, j );
		PKG_STR( db_buf, p->nick, j, sizeof(p->nick) );
		send_request_to_db( SVR_PROTO_AUCTION_ITEM, p, j, db_buf, p->id );
	}
	else
	{
		response_proto_uint32( p, p->waitcmd, CHPAR_NOT_ENOUGH_MONEY, 0 );
	}
	return 0;
}

int charparty_auction_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct auction_back_s
	{
		uint32_t result;
		uint32_t old_user_id;
		uint32_t old_user_prize;
	}__attribute__((packed)) auction_back_t;
	CHECK_BODY_LEN( len, sizeof(auction_back_t) );
	auction_back_t* back = (auction_back_t*)buf;
	if( CHPAR_OK == back->result )
	{
		uint32_t prize = *(uint32_t*)p->session;
		//冻结竞价款
		p->yxb -= prize;
		int sendlen = 0;
		uint8_t buff[128];
		PKG_H_UINT32( buff, 1, sendlen );
		PKG_H_UINT32( buff, 0, sendlen );
		PKG_H_UINT32( buff, 203, sendlen );
		PKG_H_UINT32( buff, 0, sendlen );
		pkg_item_kind( p, buff, 0, &sendlen );
		PKG_H_UINT32( buff, 0, sendlen );
		PKG_H_UINT32( buff, prize, sendlen );
		send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, p->id );

		//先前的竞拍者存在，则把押金返还给他
		if( back->old_user_prize > 0 )
		{
			int sendlen = 0;
			uint8_t buff[128];
			PKG_H_UINT32( buff, 0, sendlen );
			PKG_H_UINT32( buff, 1, sendlen );
			PKG_H_UINT32( buff, 203, sendlen );
			PKG_H_UINT32( buff, 0, sendlen );
			item_kind_t* ik_gold = find_kind_of_item(0);
			item_t* it_gold = get_item( ik_gold, 0 );
			pkg_item_kind( p, buff, 0, &sendlen );
			PKG_H_UINT32( buff, 0, sendlen );
			PKG_H_UINT32( buff, back->old_user_prize, sendlen );
			PKG_H_UINT32( buff, it_gold->max, sendlen );
			send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, back->old_user_id );
			//通知玩家游戏币返还
			//DEBUG_LOG( "Notice Palyer:[%u] Update Money", back->old_user_id );
			sendlen = 0;
			PKG_H_UINT32( buff, back->old_user_id, sendlen );
			PKG_H_UINT32( buff, back->old_user_prize, sendlen );
			tell_flash_some_msg_across_svr( NULL, CBMT_UPDATE_MONEY, sendlen, (char*)buff );
		}
		//统计 成功出价竞拍
		{
			uint32_t msgbuff[2]= {p->id, 1};
			msglog(statistic_logfile, 0x0409C2D7, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
	}
	response_proto_uint32( p, p->waitcmd, back->result, 0 );
	return 0;
}

int charparty_timer_initialize()
{
	if( config_cache.bc_elem->online_id == CHP_TIMER_UPDATE_SERVER_ID )
	{
		DEBUG_LOG( "Charitable Party Auction Timer Begin" );
		INIT_LIST_HEAD(&my_timer.timer_list);
		ADD_TIMER_EVENT(&my_timer, charparty_auction_list_update_timer, NULL, get_now_tv()->tv_sec + CHP_AUCTION_UPDATE_TIME_INI );
	}
	return 0;
}
int charparty_auction_list_update_timer( void* onwer, void* data )
{
	DEBUG_LOG( "Charitable Partyp Auction List Update ..." );
	ADD_TIMER_EVENT(&my_timer, charparty_auction_list_update_timer, NULL, get_now_tv()->tv_sec + CHP_AUCTION_UPDATE_TIME );
	send_request_to_db( SVR_PROTO_UPDATE_AUCTION_LIST, NULL, 0, NULL, 0 );
	return 0;
}

int charparty_auction_list_update_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t time_left;
	uint32_t auction_cnt;
	uint32_t expect_len = sizeof(time_left) + sizeof(auction_cnt);
	CHECK_BODY_LEN_GE( len, expect_len );
	int j = 0;
	UNPKG_H_UINT32( buf, time_left, j );
	if( time_left > CHP_AUCTION_UPDATE_TIME )
	{
		time_left = 0;
	}
	else
	{
		time_left = CHP_AUCTION_UPDATE_TIME - time_left;
	}

	UNPKG_H_UINT32( buf, auction_cnt, j );
	expect_len += auction_cnt * sizeof(auction_info_t);
	CHECK_BODY_LEN( len, expect_len );
	int msg_len = sizeof(protocol_t);
	PKG_UINT32( msg, time_left, msg_len );
	PKG_UINT32( msg, auction_cnt, msg_len );
	auction_info_t* auction_info;
	int i;
	DEBUG_LOG( "=================================Charparty Auction List Update============================" );
	DEBUG_LOG( "Auction Cnt:[%u]", auction_cnt );
	for( i = 0; i < auction_cnt; ++i )
	{
		auction_info = (auction_info_t*)(buf + j );
		j += sizeof(*auction_info);
		DEBUG_LOG( "Idx:[%u] Buyer ID:[%u] Item:[%u] Prize[%u]", i, auction_info->buyer_id, auction_info->item_id, auction_info->cur_prize );
		if( auction_info->buyer_id == 0 || auction_info->cur_prize == 0 )
		{
			continue;
		}

		//统计 拍得物品的人数
		{
			uint32_t msgbuff[2]= {auction_info->buyer_id, 1};
			msglog(statistic_logfile, 0x0409C2D8, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
		//统计 系统输出物品数量
		{
			const uint32_t item_list[] = { 1220157, 190802, 190749, 190803, 190748, 1593028, 1270055, 1270063, 1593026, 1270078,
				1270079, 1270066, 1270072, 1270073, 1270038, 1270010, 1270058, 1593034, 1593037, 1593065, 1593041, 1353296, 1353297,
				1353271, 1353223, 1353226, 1353255, 1270012, 1230044, 1230026, 1270021, 1593021, 1593061 };
			uint32_t list_cnt = sizeof(item_list)/sizeof(uint32_t);
			uint32_t msg_id = 0x0409C2D9;
			int idx;
			for( idx = 0; idx < list_cnt; ++idx )
			{
				if( auction_info->item_id == item_list[idx] )
				{
					msg_id += idx;
					uint32_t msgbuff[2]= { auction_info->buyer_id, 1 };
					msglog(statistic_logfile, msg_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
					break;
				}
			}
		}
		
		DEBUG_LOG( "ItemID:[%u] ContributorID:[%u], BuyID:[%u], Prize:[%u]", 
			auction_info->item_id, auction_info->contributor_id, auction_info->buyer_id, auction_info->cur_prize );
		//添加拍得物品，并赠送徽章
		{
			int sendlen = 0;
			uint8_t buff[128];
			PKG_H_UINT32( buff, 0, sendlen );
			PKG_H_UINT32( buff, 2, sendlen );
			PKG_H_UINT32( buff, 203, sendlen );
			PKG_H_UINT32( buff, 0, sendlen );
			
			item_kind_t* ik = find_kind_of_item(auction_info->item_id);
			item_t*it = get_item( ik, auction_info->item_id );
			pkg_item_kind( p, buff, auction_info->item_id, &sendlen );
			PKG_H_UINT32( buff, auction_info->item_id, sendlen );
			PKG_H_UINT32( buff, 1, sendlen );
			PKG_H_UINT32( buff, it->max, sendlen );

			ik = find_kind_of_item(CHP_BADGE_ITEM_ID);
			it = get_item( ik, CHP_BADGE_ITEM_ID );
			pkg_item_kind( p, buff, CHP_BADGE_ITEM_ID, &sendlen );
			PKG_H_UINT32( buff, CHP_BADGE_ITEM_ID, sendlen );
			PKG_H_UINT32( buff, 1, sendlen );
			PKG_H_UINT32( buff, it->max, sendlen );
			send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, auction_info->buyer_id );
		}
		//拍到物品后发送邮件
		{
			item_kind_t* ik = find_kind_of_item( auction_info->item_id );
			if( !ik )
			{
				ERROR_RETURN( ("charparty_auction_list_update_callback Invalid Item ID:[%u]", auction_info->item_id), -1 );
			}
			item_t* it = get_item( ik, auction_info->item_id );
			if( !it )
			{
				ERROR_RETURN( ("charparty_auction_list_update_callback Invalid Item ID:[%u]", auction_info->item_id), -1 );
			}
			uint32_t loacl_kind;
			switch( ik->kind )
			{
				//天使种子
			case ANGEL_ITEM_KIND:
				{
					loacl_kind = CHPDT_ANGEL;
				}
				break;
			case CUTE_PIG_KIND:
				{
					loacl_kind = CHPDT_PIG;
				}
				break;
			case HOMELAND_SEED_KIND:
				//农场种子
				{
					loacl_kind = CHPDT_ARG;
				}
				break;
			case ANIMAL_BABY_ITEM:
				//牧场
				{
					loacl_kind = CHPDT_MU;
				}
				break;
			case SPECIAL_ITEM_KIND:
			case HOMELAND_ITEM_KIND:
				//其他系统提供的收集品
				{
					loacl_kind = CHPDT_COLLECT;
				}
				break;
			default:
				{
					ERROR_LOG( "charparty_auction_list_update_callback Invalid Item ID:[%u] Kind:[%u]", auction_info->item_id, ik->kind );
					continue;
				}
				break;
			}
			loacl_kind -= 1;
			char post_msg[512] = {0};
#ifndef TW_VER
			char npc_name[] = "伊莲";
			char item_pos_name[][16] = { {"家园仓库"}, {"牧场仓库"}, {"天使园仓库"}, {"猪倌背包"}, {"百宝箱"} };
			char post_msg_format[] = "恭喜你在第二届爱心天使慈善派对上成功拍下%s，已经放入你的%s了，并获得一枚慈善勋章。";
#else
			char npc_name[] = "伊蓮";
			char item_pos_name[][16] = { {"家園倉庫"}, {"牧場倉庫"}, {"天使園倉庫"}, {"豬官背包"}, {"百寶箱"} };
			char post_msg_format[] = "恭喜你在第二屆愛心天使慈善派對上成功拍下%s，已經放入你的%s了，並獲得一枚慈善勳章。";
#endif
			snprintf( post_msg, sizeof(post_msg), post_msg_format, it->name, item_pos_name[loacl_kind] );
			send_postcard( npc_name, 0, auction_info->buyer_id, 1000297, post_msg, 0 );
		}
	}
	return 0;
}

/*
 * @brief 20120113新年盛宴 捐献物品
 */
int donate_food_item_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t itemid = 0;
	uint32_t item_cnt = 0;
	int i = 0;
	UNPKG_UINT32(body, itemid, i);
	UNPKG_UINT32(body, item_cnt, i);

	if (item_cnt == 0 /*|| item_cnt > 30*/) {
		ERROR_RETURN(("error item cnt:uid[%u] itemid[%u] count[%u]", p->id, itemid, item_cnt), -1);
	}

	item_t* itm_prop = get_item_prop(itemid);
    if (!itm_prop || !itm_sellable(itm_prop)) {
        ERROR_RETURN( ("Item %u not found or not donate: uid=%u itmflag=0x%X",
                        itemid, p->id, (itm_prop ? itm_prop->tradability : 0)), -1 );
    }
    
    item_kind_t* kind = find_kind_of_item(itemid);
    if (!kind || !(kind->kind == SPECIAL_ITEM_KIND)) {
        ERROR_RETURN(("error item kind, item=%u, uid=%u", itemid, p->id), -1);
	}

	const uint32_t day_type = 50015;
	*(uint32_t*)p->session = itemid;
	*(uint32_t*)(p->session + 4) = item_cnt;
	return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &day_type, p->id);
}

#define DONATE_FOOD_DAY_LIMIT_CNT 3

int user_check_donate_food_item_cnt_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t day_cnt = *(uint32_t*)buf;
	if (day_cnt >= DONATE_FOOD_DAY_LIMIT_CNT) {
		return send_to_self_error(p, p->waitcmd, -ERR_donate_food_item_limit, 1);
	}
	uint32_t itemid = *(uint32_t*)p->session;
	uint32_t item_cnt = *(uint32_t*)(p->session + 4);

	const uint32_t prize_id = 190909;
	uint32_t prize_cnt = 0;
	if (item_cnt >= 20) {
		prize_cnt = 5;
	} else if (item_cnt >= 10) {
		prize_cnt = 3;
	} else if (item_cnt > 0) {
		prize_cnt = 1;
	}
	DEBUG_LOG("user_check_donate_food_item_cnt_callback: uid[%u] itemid[%u] cnt[%u]", p->id,itemid, item_cnt);
	uint32_t db_buff[] = {1, 1, 0, 0,  0, itemid, item_cnt, 0, prize_id, prize_cnt, 99999};
	*(uint32_t*)p->session = prize_cnt;
	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, sizeof(db_buff), db_buff, p->id);
}

int donate_food_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t prize_cnt = *(uint32_t*)p->session;
	response_proto_uint32( p, p->waitcmd, prize_cnt, 0 );
	
	uint32_t msgbuff[] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C2FE, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	
	const uint32_t day_type = 50015;
	int32_t db_buff[] = {day_type, DONATE_FOOD_DAY_LIMIT_CNT, 1};
	send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, sizeof(db_buff), db_buff, p->id);
	return 0;
}
