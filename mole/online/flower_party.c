/*
 * =====================================================================================
 *
 *       Filename:  flower_party.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/07/2012 02:17:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericlee (), ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include <libtaomee/list.h>
#include "exclu_things.h"
#include "small_require.h"
#include "flower_party.h"

#define FP_BOUQUET_NEED					3				//花篮中需要的花束数量
#define FP_FLOWER_NEED					5				//花束中花朵数量
#define FP_VIP_FLOWER_CLOSE_TIEMR		(5*60)			//vip花篮关闭时间
#define FP_BROADCAST_TIEMR				(20)			//特殊时段广播定时器
#define FP_GAME_MAP_ID					47				//游戏地图ID
#define FP_FLOWER_KIND_CNT				8
#define FP_RED_ROSE_IDX					5				//红玫瑰ID
#define FP_BLACK_ROSE_IDX				8				//第八号为黑玫瑰
#define FP_IS_VIP_FLOWER(idx)			( (idx) < 4 ? 1 : 0 )		//前三朵为vip花
#define FP_PETAL_ITEM_ID				1351301			//花瓣的ITemID

#define FP_AWARD_GET_DAY_LIMIT			100				//每天获取的次数上限
#define FP_AWARD_LIMIT_DAY_TYPE			31354			//同swap.xml中的DayType

static bouquet_info_t	bouquet_info[FP_FLOWER_NEED];				//当前花束所需要的花朵 内容为花朵索引
static int				is_vip_flower_open = 0;						//vip花朵是否可以获取
static uint32_t			flower_complete_rate = 0;					//插花 完成度
static uint32_t			is_black_rose = 0;							//黑色玫瑰花


//奖励物品信息
static uint32_t			fp_award_info[][2] = {
	{1230091, 1},
	{1230092, 1},
	{1230034, 1},
	{1230016, 1},
	{1230038, 1},
	{1230007, 1},
	{1230008, 1},
	{1230018, 1},
	{1230025, 1},
	{1230029, 1},
};

static fp_timer_t		game_timer;
static fp_timer_t		broadcast_timer;

static inline int is_black_rose_time()
{
	uint32_t cur_hour = get_now_tm()->tm_hour;
	uint32_t cur_min = get_now_tm()->tm_min;
	int today = get_today();
#ifndef TW_VER
//2.10 19:30~20:00 2.11, 2.12 14:00~14:30
#define DAY_1 20120210
#define DAY_11 20120217
#define DAY_2 20120211
#define DAY_22 20120218
#define DAY_3 20120212
#define DAY_33 20120219
#else
#define DAY_1 20120217
#define DAY_1 20120224
#define DAY_2 20120218
#define DAY_2 20120225
#define DAY_3 20120219
#define DAY_3 20120226
#endif

	switch(today)
	{
	case DAY_1:
	case DAY_11:
		if( 19 == cur_hour && cur_min >= 30 )
		{
			return 1;
		}
		break;
	case DAY_2:
	case DAY_22:
	case DAY_3:
	case DAY_33:
		if( 14 == cur_hour && cur_min < 30 )
		{
			return 1;
		}
		break;
	default:
		return 0;
	}
	return 0;
}

static inline void fp_new_bouquet()
{
	int i;
	for( i = 0; i < FP_FLOWER_NEED; ++i )
	{
		if( i == 0 )
		{
			bouquet_info[i].flower_idx = 1 + rand() % 3;
		}
		else if( i == 1 )
		{
			if( 1 == is_black_rose )
			{
				bouquet_info[i].flower_idx = FP_BLACK_ROSE_IDX;
			}
			else
			{
				bouquet_info[i].flower_idx = 5;
			}
		}
		else
		{
			int flower_idx_max = FP_FLOWER_KIND_CNT - 4;
			bouquet_info[i].flower_idx = 4 + rand() % flower_idx_max;
			if( bouquet_info[i].flower_idx == FP_RED_ROSE_IDX && 1 == is_black_rose )
			{
				bouquet_info[i].flower_idx = FP_BLACK_ROSE_IDX;
			}
		}
		bouquet_info[i].is_set = 0;
	}
}

int fp_game_broadcast( void* onwer, void* data )
{
	if( 1 == is_black_rose_time() )
	{
		if( 0 == is_black_rose )
		{
			is_black_rose = 1;
			fp_update_bouquet(1);
		}
	}
	else
	{
		if( 1 == is_black_rose )
		{
			is_black_rose = 0;
			fp_update_bouquet(1);
		}
	}
	if( 1 == is_black_rose )
	{
		//broadcast to all players
#ifndef TW_VER
		uint8_t broadcast_msg[] = "RK隐藏在粒粒小广场，请小摩尔立即前往调查！";
#else
		uint8_t broadcast_msg[] = "RK隱藏在粒粒小廣場，親小摩爾立即前往調查！";
#endif
		uint8_t n_msg[1024] = {0};
		int l = sizeof(protocol_t);
		PKG_UINT32(n_msg, CBMT_FLOWER_PATY_BLACK_ROSE, l);
		PKG_UINT32(n_msg, sizeof(broadcast_msg), l);
		PKG_STR(n_msg, broadcast_msg, l, sizeof(broadcast_msg));
		init_proto_head(n_msg, PROTO_TELL_FLASH_SOME_MSG, l);
		send_to_all_players(NULL, n_msg, l, 0);
	}
	INIT_LIST_HEAD(&broadcast_timer.timer_list);
	ADD_TIMER_EVENT(&broadcast_timer, fp_game_broadcast, NULL, get_now_tv()->tv_sec + FP_BROADCAST_TIEMR );
	return 0;
}

int fp_game_initialize()
{
	DEBUG_LOG( "fp_game_initialize ..." );
	is_vip_flower_open = 0;
	flower_complete_rate = 0;
	is_black_rose = is_black_rose_time();
	fp_new_bouquet();
	fp_game_broadcast( NULL, NULL );
	return 0;
}

int fp_open_vip_flower_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	CHECK_BODY_LEN( len, 0 );
	if( p->tiles->id != FP_GAME_MAP_ID )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_not_valid_map, 1 );
	}
	if( !ISVIP(p->flag) )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_not_vip, 1 );
	}
	else
	{
		//如果未开启则广播开启 并开启定时器
		if( 0 == is_vip_flower_open )
		{
			is_vip_flower_open = 1;
			response_proto_head_map( FP_GAME_MAP_ID, PROTO_FP_OPEN_VIP_FLOWER );
			p->waitcmd = 0;
			INIT_LIST_HEAD(&game_timer.timer_list);
			ADD_TIMER_EVENT(&game_timer, fp_close_vip_flower, NULL, get_now_tv()->tv_sec + FP_VIP_FLOWER_CLOSE_TIEMR );
			return 0;
		}
		else
		{
			return send_to_self_error( p, p->waitcmd, -ERR_fp_vip_flower_was_already_open, 1 );
		}
	}
}

int fp_get_game_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	if( p->tiles->id != FP_GAME_MAP_ID )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_not_valid_map, 1 );
	}

	CHECK_BODY_LEN( len, 0 );
	int msg_len = sizeof(protocol_t);
	PKG_UINT32( msg, flower_complete_rate * 100 / FP_BOUQUET_NEED, msg_len );
	PKG_UINT32( msg, is_vip_flower_open, msg_len );
	PKG_UINT32( msg, is_black_rose, msg_len );
	int i;
	for( i = 0; i < FP_FLOWER_NEED; ++i )
	{
		PKG_UINT8( msg, bouquet_info[i].flower_idx, msg_len );
		PKG_UINT8( msg, bouquet_info[i].is_set, msg_len );
	}
	PKG_UINT32( msg, p->tiles->sprite_num, msg_len );
	list_head_t* player;
	list_for_each( player, &p->tiles->sprite_list_head )
	{
		sprite_t* l = list_entry( player, sprite_t, map_list );
		if( !IS_NPC_ID(l->id) )
		{
			PKG_UINT32( msg, l->id, msg_len );
			PKG_UINT8( msg, l->fp_flower_idx, msg_len );
		}
	}
	init_proto_head( msg, p->waitcmd, msg_len );
	return send_to_self( p, msg, msg_len, 1 );
}

int fp_close_vip_flower( void* onwer, void* data )
{
	is_vip_flower_open = 0;
	response_proto_head_map( FP_GAME_MAP_ID, PROTO_FP_VIP_FLOWER_CLOSE );
	return 0;
}

int fp_get_flower_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	if( p->tiles->id != FP_GAME_MAP_ID )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_not_valid_map, 1 );
	}
	int flower_idx;
	CHECK_BODY_LEN( len, sizeof(flower_idx) );
	int j = 0;
	UNPKG_UINT32( body, flower_idx, j );
	if( flower_idx <= 0 || flower_idx > FP_FLOWER_KIND_CNT )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_fp_get_flower_failed, 1 );
	}
	if( is_vip_flower_open == 0 && FP_IS_VIP_FLOWER(flower_idx) )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_fp_get_flower_failed, 1 );
	}
	if( (FP_BLACK_ROSE_IDX == flower_idx && 0 == is_black_rose) ||
		(FP_RED_ROSE_IDX == flower_idx && 1 == is_black_rose)
		)

	{
		return send_to_self_error( p, p->waitcmd, -ERR_fp_get_flower_failed, 1 );
	}
	p->fp_flower_idx = (char)flower_idx;
	int msg_len = sizeof(protocol_t);
	PKG_UINT32( msg, p->id, msg_len );
	PKG_UINT32( msg, flower_idx, msg_len );
	init_proto_head( msg, p->waitcmd, msg_len );
	send_to_map( p, msg, msg_len, 1 );
	return 0;
}

int fp_set_flower_cmd(sprite_t* p, const uint8_t* body, int len)
{
	int desIdx;
	CHECK_VALID_ID( p->id );
	CHECK_BODY_LEN( len, sizeof(desIdx) );

	if( p->tiles->id != FP_GAME_MAP_ID )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_not_valid_map, 1 );
	}

	int j = 0;
	UNPKG_UINT32( body, desIdx, j );
	if( desIdx < 0 || desIdx >= FP_FLOWER_NEED )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_fp_set_flower_failed, 1 );
	}

	if( 0 == bouquet_info[desIdx].is_set &&
		bouquet_info[desIdx].flower_idx == p->fp_flower_idx )
	{
		bouquet_info[desIdx].is_set = 1;
		p->fp_flower_idx = 0;
		int msg_len = sizeof(protocol_t);
		PKG_UINT32( msg, p->id, msg_len );
		PKG_UINT32( msg, desIdx, msg_len );
		init_proto_head( msg, p->waitcmd, msg_len );
		send_to_map( p, msg, msg_len, 1 );
		fp_update_bouquet(0);
		return 0;
	}
	else
	{
		return send_to_self_error( p, p->waitcmd, -ERR_fp_set_flower_failed, 1 );
	}
}

int fp_update_bouquet( int is_force )
{
	DEBUG_LOG( " fp_update_bouquet IsForce:[%u], IsBlackRose:[%u]", is_force, is_black_rose );
	int complete_cnt = 0;
	int i;
	for( i = 0; i < FP_FLOWER_NEED; ++i )
	{
		if( 1 == bouquet_info[i].is_set )
		{
			complete_cnt++;
		}
	}

	if( 1 == is_force || complete_cnt == FP_FLOWER_NEED )
	{
		//update bouquet
		fp_new_bouquet();

		int can_get_award = 0;
		if( 0 == is_force )
		{
			flower_complete_rate++;
		}
		if( flower_complete_rate == FP_BOUQUET_NEED )
		{
			can_get_award = 1;
			list_head_t* player;
			map_t* p_map = get_map( FP_GAME_MAP_ID );
			if( !p_map )
			{
				ERROR_RETURN( ("fp_update_bouquet Invalid Map ID:[%u]", FP_GAME_MAP_ID), -1 );
			}
			list_for_each( player, &p_map->sprite_list_head )
			{
				sprite_t* l = list_entry( player, sprite_t, map_list );
				if( !IS_NPC_ID(l->id) )
				{
					l->fp_can_get_award = 1;
				}
			}
			flower_complete_rate = 0;
		}
		int msg_len = sizeof(protocol_t);
		int type = 0;
		if( 1 == is_force )
		{
			if( 1 == is_black_rose )
			{
				type = 2;
			}
			else
			{
				type = 1;
			}
		}
		PKG_UINT32( msg, type, msg_len );
		PKG_UINT32( msg, flower_complete_rate * 100 / FP_BOUQUET_NEED, msg_len );
		PKG_UINT32( msg, can_get_award, msg_len );
		int i;
		for( i = 0; i < FP_FLOWER_NEED; ++i )
		{
			PKG_UINT8( msg, bouquet_info[i].flower_idx, msg_len );
			PKG_UINT8( msg, bouquet_info[i].is_set, msg_len );
		}
		int modify_player_cnt = 0;
		int modify_pos = msg_len;
		PKG_UINT32( msg, modify_player_cnt, msg_len );
		if( 1 == is_force )
		{
			list_head_t* player;
			map_t* p_map = get_map( FP_GAME_MAP_ID );
			if( !p_map )
			{
				ERROR_RETURN( ("fp_update_bouquet Invalid Map ID:[%u]", FP_GAME_MAP_ID), -1 );
			}
			list_for_each( player, &p_map->sprite_list_head )
			{
				sprite_t* l = list_entry( player, sprite_t, map_list );
				if( !IS_NPC_ID(l->id) )
				{
					if( ( 1 == is_black_rose && l->fp_flower_idx == FP_RED_ROSE_IDX ) ||
						 ( 0 == is_black_rose && l->fp_flower_idx == FP_BLACK_ROSE_IDX )
						 )
					{
						l->fp_flower_idx = 0;
						modify_player_cnt++;
						PKG_UINT32( msg, l->id, msg_len );
					}
				}
			}
		}
		PKG_UINT32( msg, modify_player_cnt, modify_pos );

		init_proto_head( msg, PROTO_FP_BOUQUET_UPDATE, msg_len );

		send_to_map3( FP_GAME_MAP_ID, msg, msg_len );
	}
	return 0;
}

int fp_get_award_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );

	if( p->tiles->id != FP_GAME_MAP_ID )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_not_valid_map, 1 );
	}

	if( 0 == p->fp_can_get_award )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_fp_can_not_get_award, 1 );
	}

	return db_set_sth_done( p, FP_AWARD_LIMIT_DAY_TYPE, FP_AWARD_GET_DAY_LIMIT, p->id );
}

int fp_get_award_day_limit_callback( sprite_t* p )
{
	p->fp_can_get_award = 0;
	uint32_t award_idx = rand() % ( sizeof(fp_award_info) / (sizeof(unsigned int) * 2) );

	int petal_cnt;
	int award_item_id;
	int award_item_cnt;
	if( 1 == is_black_rose )
	{
		petal_cnt		= 7;
		award_item_id	= 1230090;
		award_item_cnt	= 1;
	}
	else
	{
		petal_cnt		= 5;
		award_item_id	= fp_award_info[award_idx][0];
		award_item_cnt	= fp_award_info[award_idx][1];
	}
	//notice db to add award item
	{
		int dblen = 0;
		uint8_t buff[128];
		PKG_H_UINT32( buff, 0, dblen );
		PKG_H_UINT32( buff, 2, dblen );
		PKG_H_UINT32( buff, 0, dblen );
		PKG_H_UINT32( buff, 0, dblen );

		pkg_item_kind( p, buff, award_item_id, &dblen );
		PKG_H_UINT32( buff, award_item_id, dblen );
		PKG_H_UINT32( buff, award_item_cnt, dblen );
		item_kind_t* ik = find_kind_of_item( award_item_id );
		if( !ik )
		{
			ERROR_RETURN( ("invalid Item ID:[%u]", award_item_id), -1 );
		}
		item_t* it = get_item( ik, award_item_id );
		if( !it )
		{
			ERROR_RETURN( ("invalid Item ID:[%u]", award_item_id), -1 );
		}
		PKG_H_UINT32( buff, it->max, dblen );

		pkg_item_kind( p, buff, FP_PETAL_ITEM_ID, &dblen );
		PKG_H_UINT32( buff, FP_PETAL_ITEM_ID, dblen );
		PKG_H_UINT32( buff, petal_cnt, dblen );
		ik = find_kind_of_item( FP_PETAL_ITEM_ID );
		if( !ik )
		{
			ERROR_RETURN( ("invalid Item ID:[%u]", FP_PETAL_ITEM_ID), -1 );
		}
		it = get_item( ik, FP_PETAL_ITEM_ID );
		if( !it )
		{
			ERROR_RETURN( ("invalid Item ID:[%u]", FP_PETAL_ITEM_ID), -1 );
		}
		PKG_H_UINT32( buff, it->max, dblen );
		send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, dblen, buff, p->id );
	}

	int msg_len = sizeof(protocol_t);

	PKG_UINT32( msg, 2, msg_len );
	PKG_UINT32( msg, FP_PETAL_ITEM_ID, msg_len );
	PKG_UINT32( msg, petal_cnt, msg_len );
	PKG_UINT32( msg, award_item_id, msg_len );
	PKG_UINT32( msg, award_item_cnt, msg_len );
	init_proto_head( msg, p->waitcmd, msg_len );
	return send_to_self( p, msg, msg_len, 1 );
}


