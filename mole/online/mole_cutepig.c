/*
 * =====================================================================================
 *
 *       Filename:  mole_cutepig.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  09/06/2011 10:03:55 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericlee, ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include <math.h>
#include "item.h"
#include "central_online.h"
#include "exclu_things.h"
#include "mole_cutepig.h"
#include "small_require.h"

#define	CP_BATHE_TIME_LAST				(6 * 60 * 60)
#define	CP_PREGNANCY_TIME				(24 * 60 * 60)
#define	CP_RAND_LUCKY_TIME				(20 * 60)
#define	CP_MAX_LEVEL					40
#define CP_CUR_MAX_LEVEL				30
#define	CP_MAX_TASK_CNT					20
#define CP_TASK_CNT						6
#define CP_BATHE_EXP					2
#define CP_FEED_EXP						2
#define CP_TICKLE_EXP					2
#define CP_MAX_LEVEL_AWARD				5

#define CP_SYS_USER_ID					0
#define CP_SYS_PIG_ID					0
#define CP_SYS_PIG_ITEM_ID				1593000

#define CP_PIG_LIMIT					22
#define CP_2ND_PART_LIMIT				22
#define CP_2ND_PART_VIP_LIMIT			200
#define CP_TAKE_OFF_CLOTHES_ITEM_ID		1603005
#define CP_SHOW_NPC_CNT					5
#define CP_SHOW_NPC_PIG_CNT				7
#define CP_SHOW_NPC_PK_AWARD_MAX_CNT	7

//收集图鉴兑换美美猪
#define CP_IE_MAX_PIG_CNT				5
#define CP_IE_MAX_EXCHANGE_CNT			1

static uint32_t bs_npc_pig_infos[CP_SHOW_NPC_CNT][2] = {
	{300, 1000},
	{200,500},
	{100,200},
	{50,100},
	{50,100}
};//NPC的猪的属性

static uint32_t huashen_pig_infos[6][2] = {
	{800,1000},
	{600,800},
	{400,600},
	{200,400},
	{100,300},
	{100,200}
};//花婶的猪的信息

#define CP_EXPLOR_MINE_MAX			50
static cp_pig_map_explor_mine_t pig_explor_mine_info[CP_EXPLOR_MINE_MAX];
static int pig_explor_mine_cnt = 0;

#define  MAX_MAP_MINE_RANDS_ID  40
static map_mine_rand_t map_mine_rands[MAX_MAP_MINE_RANDS_ID];


typedef struct bs_award_info_s
{
	uint32_t rate;
	uint32_t item_id;
	uint32_t item_cnt;
}bs_award_info_t;

typedef struct bs_npc_info_s
{
	uint32_t pig_cnt;
	uint32_t ticket_cnt;
	uint32_t award_cnt;
	bs_award_info_t award_info[CP_SHOW_NPC_PK_AWARD_MAX_CNT];
}bs_npc_info_t;

static bs_npc_info_t bs_npc_pk_award[CP_SHOW_NPC_CNT];

typedef struct
{
	uint32_t item_id;
	uint32_t item_cnt;
}cp_levelup_award;

typedef struct
{
	uint32_t exp;
	uint32_t award_cnt;
	cp_levelup_award award_info[CP_MAX_LEVEL_AWARD];
}cp_level_info_t;

typedef struct
{
	uint32_t		task_count;
	cp_task_info_t	task_info[CP_MAX_TASK_CNT];
}cp_task_t;

typedef struct cp_ie_s
{
	uint32_t in_cnt;
	uint32_t breed_need[CP_IE_MAX_PIG_CNT];
	uint32_t out_pig_id;
	uint32_t flag_idx;
}cp_ie_t;

typedef struct cp_ie_info_s
{
	uint32_t	exchange_num;
	cp_ie_t		exchange_info[CP_IE_MAX_EXCHANGE_CNT];
}cp_ie_info_t;

//收集美美猪图鉴兑换猪仔
static cp_ie_info_t			illustrate_exchange_info;

static cp_level_info_t		level_info[CP_MAX_LEVEL];
static cp_task_t			cp_task_info;
static cp_process_info_t	cp_process_info;

int cutepig_get_game_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id);
	uint32_t tarID;
	CHECK_BODY_LEN( len, sizeof(tarID) );
	int j = 0;
	UNPKG_UINT32( body, tarID, j );
	CHECK_VALID_ID( tarID );

	//统计 进入猪倌
	if( p->id == tarID )
	{
		uint32_t msgbuff[4]= {p->id, 1, p->id, 1};
		msglog(statistic_logfile, 0x0409BCFA, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}

	return send_request_to_db( SVR_PROTO_CUTE_PIG_GET_INFO, p, sizeof(p->id), &p->id, tarID );
}

int cutepig_get_game_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int expectLen = sizeof(cp_game_info_t);
	CHECK_BODY_LEN_GE( len, expectLen );
	cp_game_info_t* game_info = (cp_game_info_t*)buf;

	if( id == p->id )
	{
		p->cp_level	= game_info->level;
		p->cp_exp	= game_info->exp;
		if( game_info->state == CGIS_NEW_PLAYER )
		{
			send_request_to_db( SVR_PROTO_CP_SET_GLOBLE_LEVEL, NULL, sizeof(game_info->level), &game_info->level, p->id );
			//统计 等级
			{
				uint32_t msgbuff[2]= {p->id, p->cp_level};
				msglog(statistic_logfile, 0x0409BD05, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
			}
		}
	}

	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, game_info->state, sendlen );
	PKG_UINT32( msg, game_info->level, sendlen );
	PKG_UINT32( msg, game_info->exp, sendlen );
	uint32_t next_level = game_info->level + 1;
	if( next_level > CP_CUR_MAX_LEVEL )
	{
		next_level = CP_CUR_MAX_LEVEL;
	}
	PKG_UINT32( msg, level_info[next_level - 1].exp, sendlen );
	PKG_UINT32( msg, game_info->show_stage_lv, sendlen );
	PKG_UINT32( msg, game_info->machine_lvl, sendlen );
	PKG_UINT32( msg, game_info->proc_fac_lv, sendlen );
	PKG_UINT32( msg, game_info->honor, sendlen );
	PKG_UINT32( msg, game_info->feed_cnt, sendlen );
	PKG_UINT32( msg, game_info->process_cnt, sendlen );
	//PKG_UINT32( msg, game_info->gold, sendlen );
	PKG_UINT32( msg, game_info->bath_time, sendlen );
	PKG_UINT32( msg, game_info->formation, sendlen );
	PKG_UINT32( msg, game_info->background_id, sendlen );
	PKG_STR( msg, game_info->formation_msg, sendlen, sizeof(game_info->formation_msg) );
	PKG_UINT32( msg, game_info->pig_count, sendlen );

	cp_pig_info_simple_t* pig_info = NULL;

	expectLen += sizeof(*pig_info) * game_info->pig_count;
	CHECK_BODY_LEN( len, expectLen );
	int i;
	for( i = 0; i < game_info->pig_count; ++i )
	{
		pig_info = (cp_pig_info_simple_t*)((char*)game_info + sizeof(*game_info) + sizeof(*pig_info) * i);
		//get pig info and it's item info
		PKG_UINT32( msg, pig_info->pig_id, sendlen );
		PKG_UINT32( msg, pig_info->item_id, sendlen );
		PKG_UINT32( msg, pig_info->breed, sendlen );
		PKG_UINT32( msg, pig_info->sex, sendlen );
		PKG_UINT32( msg, pig_info->hungry_degree, sendlen );
		PKG_UINT32( msg, pig_info->state, sendlen );
		PKG_UINT32( msg, pig_info->tickle_cnt, sendlen );
		PKG_UINT32( msg, pig_info->transform_id, sendlen );
		PKG_UINT32( msg, pig_info->dress_1, sendlen );
		PKG_UINT32( msg, pig_info->dress_2, sendlen );
	}
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self(p, msg, sendlen, 1);
}

int cutepig_get_pig_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	typedef struct check_pig_s
	{
		uint32_t tar_id;
		uint32_t pig_id;
	}__attribute__((packed)) check_pig_t;
	CHECK_VALID_ID( p->id );
	CHECK_BODY_LEN( len, sizeof(check_pig_t) );
	check_pig_t info;
	int j = 0;
	UNPKG_UINT32( body, info.tar_id, j );
	UNPKG_UINT32( body, info.pig_id, j );
	return send_request_to_db( SVR_PROTO_CP_GET_PIG_INFO, p, sizeof(info.pig_id), &info.pig_id, info.tar_id );
}

int cutepig_get_pig_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN( len, sizeof(cp_pig_info_t) );
	cp_pig_info_t* pig_info = (cp_pig_info_t*)buf;
	switch( p->waitcmd )
	{
	case PROTO_CP_GET_PIG_INFO:
		{
			int sendlen = sizeof(protocol_t);
			PKG_UINT32( msg, pig_info->pig_id, sendlen );
			PKG_UINT32( msg, pig_info->item_id, sendlen );
			PKG_STR( msg, pig_info->mother_name, sendlen, sizeof(pig_info->mother_name) );
			PKG_STR( msg, pig_info->father_name, sendlen, sizeof(pig_info->father_name) );
			PKG_STR( msg, pig_info->name, sendlen, sizeof(pig_info->name) );
			
			PKG_UINT32( msg, pig_info->state, sendlen );
			PKG_UINT32( msg, pig_info->age, sendlen );
			PKG_UINT32( msg, pig_info->lifetime, sendlen );
			PKG_UINT32( msg, pig_info->hungry_degree, sendlen );
			PKG_UINT32( msg, pig_info->breed, sendlen );
			PKG_UINT32( msg, pig_info->generation, sendlen );
			//特殊处理，特种猪在前端不注明
			uint32_t isSP = ( pig_info->sex > 1 );
			pig_info->sex %= 2;
			PKG_UINT32( msg, pig_info->sex, sendlen );
			PKG_UINT32( msg, isSP, sendlen );
			PKG_UINT32( msg, pig_info->growth/10, sendlen );
			PKG_UINT32( msg, pig_info->weight/10, sendlen );
			PKG_UINT32( msg, pig_info->glamour/10, sendlen );
			PKG_UINT32( msg, pig_info->strength/10, sendlen );
			PKG_UINT32( msg, pig_info->transform_id, sendlen );
			//there's something wrong
			PKG_UINT32( msg, pig_info->transform_time, sendlen );
			PKG_UINT32( msg, pig_info->mate_cnt, sendlen );
			PKG_UINT32( msg, pig_info->tickle_cnt, sendlen );
			PKG_UINT32( msg, pig_info->train_point, sendlen );
			PKG_UINT32( msg, pig_info->parturition_time, sendlen );
			PKG_UINT32( msg, pig_info->dress_1, sendlen );
			PKG_UINT32( msg, pig_info->dress_2, sendlen );
			PKG_UINT32( msg, pig_info->able_insect, sendlen );
			init_proto_head( msg, p->waitcmd, sendlen );
			return send_to_self(p, msg, sendlen, 1);
		}
		break;
	case PROTO_CP_MATE:
		{
			cp_mate_info_t* mate_info = (cp_mate_info_t*)p->session;
			//self pig info
			if( id == p->id )
			{
				if( pig_info->state & PS_MATURE &&				//已成年
					!(pig_info->state & PS_PREGNANT) &&			//不在怀孕状态
					pig_info->hungry_degree < 100 &&			//饥饿度没有满
					pig_info->sex == CPS_FEMALE &&				//必须是母猪
					pig_info->lifetime - pig_info->age > CP_PREGNANCY_TIME	//有足够的寿命怀孕
					)
				{
					uint32_t space_need = 1;
					if( pig_info->state & PS_DOUBLE )
					{
						space_need = 2;
					}

					if( space_need > pig_info->space_left )
					{
						return cutepig_mate_result( p, CMR_NOT_ENOGH_SPACE, 0 );
					}

					memcpy( &mate_info->my_pig_info, pig_info, sizeof(*pig_info) );
					if( mate_info->tar_user_id == CP_SYS_USER_ID )
					{
						item_kind_t* ik_sys_pig	= find_kind_of_item( CP_SYS_PIG_ITEM_ID );
						item_t* it_sys_pig		= get_item( ik_sys_pig, CP_SYS_PIG_ITEM_ID );
						mate_info->tar_pig_info.item_id = CP_SYS_PIG_ITEM_ID;
						mate_info->tar_pig_info.generation = 1;
						mate_info->mate_cost = it_sys_pig->u.cp_base_info_tag.mate_cost;
						memcpy( mate_info->tar_pig_info.name, it_sys_pig->name, sizeof(mate_info->tar_pig_info.name) );
						if( mate_info->gold_cnt >= mate_info->mate_cost )
						{
							return cutepig_mate_check_callback( p, p->id, NULL, 0 );
						}
						else
						{
							return cutepig_mate_result( p, CMR_NOT_ENOUGH_MONEY, 0 );
						}

					}
					else
					{
						return send_request_to_db( SVR_PROTO_CP_GET_PIG_INFO, p, sizeof(mate_info->tar_pig_id), &mate_info->tar_pig_id, mate_info->tar_user_id );
					}
				}
				else
				{
					return cutepig_mate_result( p, CMR_CAN_NOT_MATE, 0 );
				}
			}
			//tar pig info
			else if( id == mate_info->tar_user_id )
			{
				if( pig_info->state & PS_MATURE &&					//已成年
					pig_info->mate_cnt < CUTE_PIG_MAX_MATE_CNT &&	//交配次数未满
					//pig_info->hungry_degree < 100 &&				//饥饿度没有满
					pig_info->sex == CPS_MALE						//必须是公猪
					)
				{
					memcpy( &mate_info->tar_pig_info, pig_info, sizeof(*pig_info) );
					item_kind_t* ik_male = find_kind_of_item( pig_info->item_id );
					item_t*	it_male = get_item( ik_male, pig_info->item_id );
					mate_info->mate_cost = it_male->u.cp_base_info_tag.mate_cost * mate_info->tar_pig_info.generation;
					if( mate_info->gold_cnt >= mate_info->mate_cost )
					{
						uint32_t db_buf[] = { mate_info->tar_pig_id, mate_info->mate_cost };
						return send_request_to_db( SVR_PROTO_CP_MATE_CHECK, p, sizeof(db_buf), db_buf, mate_info->tar_user_id );
					}
					else
					{
						return cutepig_mate_result( p, CMR_NOT_ENOUGH_MONEY, 0 );
					}
				}
				else
				{
					return cutepig_mate_result( p, CMR_TAR_CAN_NOT_MATE, 0 );
				}
			}
			else
			{
				ERROR_LOG("cutepig_get_pig_info_callback Invalid back ID:[%u]", id );
			}
		}
		break;
	case PROTO_CP_NPC_WEIGHT_PK:
		return cutepig_npc_weight_pk_callback( p, pig_info->weight/10 );
		break;
	default:
		break;
	}
	return 0;
}

int cutepig_mate_check_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	cp_mate_info_t* mate_info = (cp_mate_info_t*)p->session;
	uint32_t isOK;
	if( mate_info->tar_user_id == CP_SYS_USER_ID )
	{
		isOK = 1;
	}
	else
	{
		CHECK_BODY_LEN( len, sizeof(isOK) );
		isOK = *(uint32_t*)buf;
	}

	if( 1 == isOK )
	{
		int baby_cnt = mate_info->my_pig_info.state & PS_DOUBLE ? 2 : 1;
		int i;
		uint8_t db_buf[1024];
		int j = 0;
		PKG_H_UINT32( db_buf, mate_info->my_pig_id, j );
		PKG_H_UINT32( db_buf, mate_info->mate_cost, j );
		PKG_H_UINT32( db_buf, baby_cnt, j );
		cp_new_pig_info_t* pig_info;

		item_kind_t* ik_my_pig = find_kind_of_item(mate_info->my_pig_info.item_id);
		if( !ik_my_pig )
		{
			ERROR_RETURN( ("Invalid item id:[%u]", mate_info->my_pig_info.item_id), -1);
		}
		item_t* item_my_pig = get_item( ik_my_pig, mate_info->my_pig_info.item_id );
		if( !item_my_pig )
		{
			ERROR_RETURN( ("Invalid item id:[%u]", mate_info->my_pig_info.item_id), -1);
		}
		item_kind_t* ik_male_pig = find_kind_of_item(mate_info->tar_pig_info.item_id);
		if( !ik_male_pig )
		{
			ERROR_RETURN( ("Invalid item id:[%u]", mate_info->tar_pig_info.item_id), -1);
		}
		item_t* item_male_pig = get_item( ik_male_pig, mate_info->tar_pig_info.item_id );
		if( !item_male_pig )
		{
			ERROR_RETURN( ("Invalid item id:[%u]", mate_info->tar_pig_info.item_id), -1);
		}
		uint32_t var_pig_id = item_my_pig->u.cp_base_info_tag.var_id;
		for( i = 0; i < baby_cnt; ++i )
		{
			pig_info = (cp_new_pig_info_t*)((char*)db_buf + j);
			memcpy( pig_info->father_name, mate_info->tar_pig_info.name, sizeof(pig_info->father_name) );
			memcpy( pig_info->mother_name, mate_info->my_pig_info.name, sizeof(pig_info->mother_name) );
			cp_pig_info_t* src_pig;
			//使用变异道具 随机选择变异猪

			uint32_t isVar = 0;
			if( mate_info->my_pig_info.state & PS_VAR )
			{
				int temp = rand()%2;
				uint32_t var_id1 = item_my_pig->u.cp_base_info_tag.var_id;
				uint32_t var_id2 = item_male_pig->u.cp_base_info_tag.var_id;
				if( temp == 1 )
				{
					temp = var_id1;
					var_id1 = var_id2;
					var_id2 = temp;
				}
				if( var_id1 == 0 )
				{
					if( var_id2 == 0 )
					{
						isVar = 0;
					}
					else
					{
						var_pig_id = var_id2;
						isVar = 1;
					}
				}
				else
				{
					var_pig_id = var_id1;
					isVar = 1;
				}
			}
			else
			{
				uint32_t randVal = rand()%100;
				uint32_t randExpect = item_my_pig->u.cp_base_info_tag.var_rat;
				if( randVal < randExpect )
				{
					var_pig_id = item_my_pig->u.cp_base_info_tag.var_id;
					isVar = 1;
				}
				else
				{
					randExpect += item_male_pig->u.cp_base_info_tag.var_rat;
					if( randVal < randExpect )
					{
						var_pig_id = item_male_pig->u.cp_base_info_tag.var_id;
						isVar = 1;
					}
				}
			}
			if( 1 == isVar )
			{
				item_kind_t* ik = find_kind_of_item(var_pig_id);
				if( !ik )
				{
					ERROR_RETURN( ("Invalid item id:[%u]", var_pig_id), -1);
				}
				item_t* item_pig = get_item( ik, var_pig_id );
				if( !item_pig )
				{
					ERROR_RETURN( ("Invalid item id:[%u]", var_pig_id), -1);
				}
				pig_info->sex		= CPS_VAR;
				pig_info->breed		= item_pig->u.cp_base_info_tag.breed;
				pig_info->generation= 1;
				pig_info->item_id	= var_pig_id;
				pig_info->weight	= item_pig->u.cp_base_info_tag.weight;
				pig_info->glamour	= item_pig->u.cp_base_info_tag.glamour;
				pig_info->strength	= item_pig->u.cp_base_info_tag.strength;
				pig_info->growth	= item_pig->u.cp_base_info_tag.growth;
				pig_info->lifetime	= item_pig->u.cp_base_info_tag.lifetime;
				pig_info->pirce		= item_pig->u.cp_base_info_tag.price;
				memcpy( pig_info->name, item_pig->name, CUTE_PIG_MAX_NAME_LEN );
			}
			else
			{
				pig_info->sex = rand()%2;
				if( 1 == rand()%2 )
				{
					src_pig = &mate_info->my_pig_info;
				}
				else
				{
					src_pig = &mate_info->tar_pig_info;
				}
				item_kind_t* ik = find_kind_of_item(src_pig->item_id);
				if( !ik )
				{
					ERROR_RETURN( ("Invalid item id:[%u]", src_pig->item_id), -1);
				}
				item_t* it = get_item( ik, src_pig->item_id );
				if( !it )
				{
					ERROR_RETURN( ("Invalid item id:[%u]", src_pig->item_id), -1);
				}

				pig_info->breed		= it->u.cp_base_info_tag.breed;
				pig_info->generation= src_pig->generation + 1;
				if( pig_info->generation > 5 )
				{
					pig_info->generation = 5;
				}
				float price_up		= it->u.cp_base_info_tag.price_up;
				price_up /= 10;
				float weight_up		= it->u.cp_base_info_tag.weight_up;
				weight_up /= 10;
				float grow_up		= it->u.cp_base_info_tag.grow_up;
				grow_up /= 10;
				float str_up		= it->u.cp_base_info_tag.str_up;
				str_up /= 10;
				float glamour_up	= it->u.cp_base_info_tag.glamour_up;
				glamour_up /= 10;
				pig_info->pirce		= it->u.cp_base_info_tag.price * pow( price_up, pig_info->generation - 1 );
				pig_info->item_id	= pig_info->sex == it->u.cp_base_info_tag.sex ? src_pig->item_id : it->u.cp_base_info_tag.oppsite_id;
				pig_info->glamour	= it->u.cp_base_info_tag.glamour * pow( glamour_up, pig_info->generation - 1 );
				pig_info->weight	= it->u.cp_base_info_tag.weight * pow( weight_up, pig_info->generation - 1 );
				pig_info->strength	= it->u.cp_base_info_tag.strength * pow( str_up, pig_info->generation - 1 );
				pig_info->growth	= it->u.cp_base_info_tag.growth * pow( grow_up, pig_info->generation - 1 );
				pig_info->lifetime	= it->u.cp_base_info_tag.lifetime;

				memcpy( pig_info->name, it->name, CUTE_PIG_MAX_NAME_LEN );
			}

			j += sizeof(cp_new_pig_info_t);
		}
		return send_request_to_db( SVR_PROTO_CP_MATE, p, j, db_buf, p->id );
	}
	else
	{
		return cutepig_mate_result( p, CMR_TAR_CAN_NOT_MATE, 0 );
	}
}

int cutepig_get_knapsack_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	return send_request_to_db( SVR_PROTO_CP_GET_KNAPSACK_INFO, p, 0, NULL, p->id );
}

int cutepig_get_knapsack_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t count;
	int expect_len = sizeof(count);
	CHECK_BODY_LEN_GE( len, expect_len );
	int j = 0;
	UNPKG_H_UINT32( buf, count, j );

	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, count, sendlen );

	int i;
	uint32_t item_id;
	uint32_t item_cnt;
	for( i = 0; i < count; ++i )
	{
		UNPKG_H_UINT32( buf, item_id, j );
		UNPKG_H_UINT32( buf, item_cnt, j );
		PKG_UINT32( msg, item_id, sendlen );
		PKG_UINT32( msg, item_cnt, sendlen );
	}
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}

int cutepig_raise_pig_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t pig_id;
	CHECK_BODY_LEN( len, sizeof(pig_id) );
	int j = 0;
	UNPKG_UINT32( body, pig_id, j );

	item_kind_t* ik = find_kind_of_item(pig_id);
	if( !ik || ik->kind != CUTE_PIG_KIND )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_cutepig_raise_invalid_pig, 1 );
	}
	else
	{
		item_t* item_info = get_item( ik, pig_id );
		if( !item_info )
		{
			ERROR_RETURN(("Invalid item info,ID:[%u]", pig_id), -1);
		}
		cp_base_info_t* pig_info = &item_info->u.cp_base_info_tag;
		uint8_t db_buf[128];
		int sendlen = 0;
		PKG_H_UINT32( db_buf, pig_id, sendlen );
		PKG_H_UINT32( db_buf, pig_info->breed, sendlen );
		PKG_H_UINT32( db_buf, pig_info->sex, sendlen );
		PKG_H_UINT32( db_buf, pig_info->weight, sendlen );
		PKG_H_UINT32( db_buf, pig_info->glamour, sendlen );
		PKG_H_UINT32( db_buf, pig_info->strength, sendlen );
		PKG_H_UINT32( db_buf, pig_info->growth, sendlen );
		PKG_H_UINT32( db_buf, pig_info->lifetime, sendlen );
		PKG_H_UINT32( db_buf, pig_info->price, sendlen );
		PKG_STR( db_buf, item_info->name, sendlen, CUTE_PIG_MAX_NAME_LEN );
		return send_request_to_db( SVR_PROTO_CP_RAISE_NEW_PIG, p, sendlen, db_buf, p->id );
	}
}

int cutepig_raise_pig_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN( len ,sizeof(cp_pig_raise_back_t) + sizeof(uint32_t) );
	uint32_t isOK = *(uint32_t*)buf;
	cp_pig_raise_back_t* pig_info = (cp_pig_raise_back_t*)( buf + sizeof(isOK) );

	send_honor_event_to_db(NULL, CHT_50TIMES_PIG, 0, p->id);

	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, isOK, sendlen );
	PKG_UINT32( msg, pig_info->pig_id, sendlen );
	PKG_UINT32( msg, pig_info->item_id, sendlen );
	//PKG_STR( msg, pig_info->name, sendlen, sizeof(pig_info->name) );
	PKG_UINT32( msg, pig_info->breed, sendlen );
	PKG_UINT32( msg, pig_info->sex, sendlen );
	PKG_UINT32( msg, pig_info->hungry_degree, sendlen );
	PKG_UINT32( msg, pig_info->state, sendlen );
	PKG_UINT32( msg, pig_info->transform_id, sendlen );
	PKG_UINT32( msg, pig_info->dress_1, sendlen );
	PKG_UINT32( msg, pig_info->dress_2, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}

int cutepig_feed_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	cp_feed_info_t* info = (cp_feed_info_t*)p->session;
	int j = 0;
	UNPKG_UINT32( body, info->tar_id, j );
	UNPKG_UINT32( body, info->fodder_id, j );
	item_kind_t* ik = find_kind_of_item( info->fodder_id );
	if( !ik || ik->kind != CUTE_PIG_ITEM_KIND )
	{
		ERROR_RETURN( ("Invalid fodder ID:[%u]", info->fodder_id ), -1 );
	}
	item_t* it = get_item( ik, info->fodder_id );
	if( !it || it->u.cp_item_tag.type != CIK_FODDER )
	{
		ERROR_RETURN( ("Invalid fodder ID:[%u]", info->fodder_id ), -1 );
	}

	uint32_t db_buf[] = { info->tar_id, info->fodder_id };
	return send_request_to_db( SVR_PROTO_CP_FEED_GET_INFO, p, sizeof(db_buf), db_buf, p->id );
}

int cutepig_feed_get_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t fodder_cnt;
	cp_feed_info_t* info = (cp_feed_info_t*)p->session;
	CHECK_BODY_LEN( len, sizeof(uint32_t) * 2);
	info->is_friend = *(uint32_t*)buf;
	fodder_cnt = *((uint32_t*)buf + 1);

	if( fodder_cnt == 0 )
	{
		int sendlen = sizeof(protocol_t);
		PKG_UINT32( msg, FR_NOT_ENOUGH_FODDER, sendlen );
		PKG_UINT32( msg, 0, sendlen );
		PKG_UINT32( msg, 0, sendlen );
		PKG_UINT32( msg, 0, sendlen );
		PKG_UINT32( msg, 0, sendlen );
		PKG_UINT32( msg, 0, sendlen );
		init_proto_head( msg, p->waitcmd, sendlen );
		return send_to_self( p, msg, sendlen, 1 );
	}
	else
	{
		cp_feed_info_t* info = (cp_feed_info_t*)p->session;
		uint32_t db_buf[] = { p->id, info->fodder_id, fodder_cnt };
		return send_request_to_db( SVR_PROTO_CP_FEED, p, sizeof(db_buf), db_buf, info->tar_id );
	}
}

int cutepig_feed_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct cp_feed_back_s
	{
		uint32_t result;
		uint32_t consume;
		uint32_t feed_cnt;
		uint32_t added_weight;
	}__attribute__((packed)) cp_feed_back_t;
	CHECK_BODY_LEN( len, sizeof(cp_feed_back_t) );
	cp_feed_back_t* info = (cp_feed_back_t*)buf;
	cp_feed_info_t* feed_info = (cp_feed_info_t*)p->session;
	uint32_t exp = 0;
	if( info->result == FR_OK )
	{
		//统计 喂食
		{
			uint32_t msgbuff[2]= {p->id, 1};
			msglog(statistic_logfile, 0x0409BCFE, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
		//给好友喂食才加经验
		if( 1 == feed_info->is_friend )
		{
			exp = CP_FEED_EXP;
			cp_add_exp_gold_info_t add_info;
			add_info.p = p;
			add_info.tar_id = p->id;
			add_info.cur_lv = p->cp_level;
			add_info.cur_exp = p->cp_exp;
			add_info.add_exp = &exp;
			add_info.add_gold = 0;
			add_info.is_from_db = 0;
			cutepig_add_exp_gold( &add_info );
			
			if (feed_info->tar_id == p->id) {//给自己的猪喂食
				send_honor_event_to_db(NULL, CHT_3TIMES_LOGIN, 0, p->id);
			} else {
				send_honor_event_to_db(NULL, CHT_20TIMES_FEED, 0, p->id);
			}
		}

		int dblen = 0;
		uint8_t buff[128];
		PKG_H_UINT32( buff, 1, dblen );
		PKG_H_UINT32( buff, 0, dblen );
		PKG_H_UINT32( buff, 0, dblen );
		PKG_H_UINT32( buff, 0, dblen );
		pkg_item_kind( p, buff, feed_info->fodder_id, &dblen );
		PKG_H_UINT32( buff, feed_info->fodder_id, dblen );
		PKG_H_UINT32( buff, info->consume, dblen );
		send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, dblen, buff, p->id );
		cutepig_notice_db_sth_done( p, CSDE_FEED );
	}
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, info->result, sendlen );
	PKG_UINT32( msg, info->consume, sendlen );
	PKG_UINT32( msg, info->feed_cnt, sendlen );
	PKG_UINT32( msg, exp, sendlen );
	PKG_UINT32( msg, info->added_weight, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}

int cutepig_bathe_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t tarID;
	CHECK_BODY_LEN( len, sizeof(tarID) );
	int j = 0;
	UNPKG_UINT32( body, tarID, j );
	return send_request_to_db( SVR_PROTO_CP_BATHE, p, sizeof(p->id), &p->id, tarID );
}

int cutepig_bathe_callback(sprite_t* p, uint32_t id, char* buf, int len)

{
	typedef struct bathe_back_s
	{
		uint32_t isOK;
		uint32_t time;
	}__attribute__((packed)) bathe_back_t;
	CHECK_BODY_LEN( len, sizeof(bathe_back_t) );
	bathe_back_t* info = (bathe_back_t*)buf;
	uint32_t exp = 0;
	if( 1 == info->isOK )
	{
		exp = CP_BATHE_EXP;
		cp_add_exp_gold_info_t add_info;
		add_info.p = p;
		add_info.tar_id = p->id;
		add_info.cur_lv = p->cp_level;
		add_info.cur_exp = p->cp_exp;
		add_info.add_exp = &exp;
		add_info.add_gold = 0;
		add_info.is_from_db = 0;
		cutepig_add_exp_gold( &add_info );
		//统计 洗澡
		{
			uint32_t msgbuff[2]= {p->id, 1};
			msglog(statistic_logfile, 0x0409BD03, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
		cutepig_notice_db_sth_done( p, CSDE_BATHE );

		send_honor_event_to_db(NULL, CHT_10TIMES_BATHE, 0, p->id);
	}
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, info->isOK, sendlen );
	PKG_UINT32( msg, exp, sendlen );
	PKG_UINT32( msg, info->time/60, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1);
}

int cutepig_tickle_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	int pig_id;
	CHECK_BODY_LEN( len, sizeof(pig_id) );
	int j = 0;
	UNPKG_UINT32( body, pig_id, j );
	return send_request_to_db( SVR_PROTO_CP_TICKLE, p, sizeof(pig_id), &pig_id, p->id );
}

int cutepig_tickle_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct tickle_back_s
	{
		int pig_id;
		int isOK;
		int added_weight;
		int	added_glamour;
		int added_strength;
	}__attribute__((packed)) tickle_back_t;
	CHECK_BODY_LEN( len, sizeof(tickle_back_t) );
	tickle_back_t* info = (tickle_back_t*)buf;
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, info->pig_id, sendlen );
	PKG_UINT32( msg, info->isOK, sendlen );
	PKG_UINT32( msg, info->added_weight, sendlen );
	PKG_UINT32( msg, info->added_glamour, sendlen );
	PKG_UINT32( msg, info->added_strength, sendlen );
	uint32_t exp = 0;
	if(info->isOK == 1)
	{
		exp = CP_TICKLE_EXP;
		cp_add_exp_gold_info_t add_info;
		add_info.p = p;
		add_info.tar_id = p->id;
		add_info.cur_lv = p->cp_level;
		add_info.cur_exp = p->cp_exp;
		add_info.add_exp = &exp;
		add_info.add_gold = 0;
		add_info.is_from_db = 0;
		cutepig_add_exp_gold( &add_info );
		//统计 逗逗
		{
			uint32_t msgbuff[2]= {p->id, 1};
			msglog(statistic_logfile, 0x0409BD02, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}

		send_honor_event_to_db(NULL, CHT_10TIMES_PLAY, 0, p->id);
	}
	PKG_UINT32( msg, exp, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}

int cutepig_training_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	int pig_id, type_id;
	CHECK_BODY_LEN( len, sizeof(pig_id) + sizeof(type_id) );
	int j = 0;
	UNPKG_UINT32( body, pig_id, j );
	UNPKG_UINT32( body, type_id, j );
	uint32_t db_buf[] = { pig_id, type_id };
	return send_request_to_db( SVR_PROTO_CP_TRAIN, p, sizeof(db_buf), db_buf, p->id );
}

int cutepig_training_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct train_back_s
	{
		uint32_t pig_id;
		uint32_t isOK;
		uint32_t exp;
		uint32_t added_weight;
		uint32_t added_glamour;
		uint32_t added_strenght;
	}__attribute__((packed)) train_back_t;
	CHECK_BODY_LEN( len, sizeof(train_back_t) );
	train_back_t* info = (train_back_t*)buf;
	if( 1 == info->isOK )
	{
		cp_add_exp_gold_info_t add_info;
		add_info.p = p;
		add_info.tar_id = p->id;
		add_info.cur_lv = p->cp_level;
		add_info.cur_exp = p->cp_exp;
		add_info.add_exp = &info->exp;
		add_info.add_gold = 0;
		add_info.is_from_db = 1;
		cutepig_add_exp_gold( &add_info );
		//统计 体质训练、肉质训练
		if( info->added_weight > 0 )
		{
			uint32_t msgbuff[2]= {p->id, 1};
			msglog(statistic_logfile, 0x0409BD00, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
	}
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, info->pig_id, sendlen );
	PKG_UINT32( msg, info->isOK, sendlen );
	PKG_UINT32( msg, info->exp, sendlen );
	PKG_UINT32( msg, info->added_weight, sendlen );
	PKG_UINT32( msg, info->added_glamour, sendlen );
	PKG_UINT32( msg, info->added_strenght, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}

int cutepig_set_name_cmd(sprite_t* p, const uint8_t* body, int len)
{
	typedef struct set_name_s
	{
		uint32_t	pig_id;
		uint8_t		cur_name[CUTE_PIG_MAX_NAME_LEN];
	}__attribute__((packed)) set_name_t;
	CHECK_VALID_ID( p->id );
	CHECK_BODY_LEN( len, sizeof(set_name_t) );
	set_name_t info;
	int j = 0;
	UNPKG_UINT32( body, info.pig_id, j );
	UNPKG_STR( body, info.cur_name, j, sizeof(info.cur_name) );
	CHECK_DIRTYWORD( p, info.cur_name );
	return send_request_to_db( SVR_PROTO_CP_SET_NAME, p, sizeof(set_name_t), &info, p->id );
}

int cutepig_set_name_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t isOK;
	CHECK_BODY_LEN( len, sizeof(isOK) );
	isOK = *(uint32_t*)buf;
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, isOK, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen , 1 );
}

int cutepig_set_formation_cmd(sprite_t* p, const uint8_t* body, int len)
{
	typedef struct
	{
		uint32_t	formation_id;
		uint8_t		msg[56];
	}__attribute__((packed))cp_formation_cmd_t;
	CHECK_VALID_ID( p->id );
	CHECK_BODY_LEN( len, sizeof(cp_formation_cmd_t) );
	cp_formation_cmd_t cmd;
	int j = 0;
	UNPKG_UINT32( body, cmd.formation_id, j );
	UNPKG_STR( body, cmd.msg, j, sizeof(cmd.msg) );
	CHECK_DIRTYWORD( p, cmd.msg );
	return send_request_to_db( SVR_PROTO_CP_SET_FORMATION, p, sizeof(cmd), &cmd, p->id );
}


int	cutepig_set_formation_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t isOK;
	CHECK_BODY_LEN( len, sizeof(isOK) );
	isOK = *(uint32_t*)buf;
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, isOK, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen , 1 );
}

int cutepig_buy_item_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	CHECK_BODY_LEN( len, sizeof(cp_buy_cmd_t) );
	cp_buy_cmd_t cmd;
	int j = 0;
	UNPKG_UINT32( body, cmd.item_id, j );
	UNPKG_UINT32( body, cmd.item_cnt, j );
	item_kind_t* ik = find_kind_of_item(cmd.item_id);
	if( !ik || (ik->kind != CUTE_PIG_KIND && ik->kind != CUTE_PIG_ITEM_KIND && ik->kind != CUTE_PIG_CLOTHES_KIND) )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_cutepig_invalid_item, 1 );
	}
	else
	{
		item_t* buy_item = get_item( ik, cmd.item_id );
		if( !buy_item ||
			buy_item->u.cp_base_info_tag.buy_level > cutepig_get_cur_level(p->cp_exp)
			)
		{
			return send_to_self_error( p, p->waitcmd, -ERR_cutepig_can_not_buy_item, 1 );
		}
		cmd.cost = buy_item->price * cmd.item_cnt;
		int sendlen = 0;
		uint8_t buff[1024];
		PKG_H_UINT32( buff, 1, sendlen );
		PKG_H_UINT32( buff, 1, sendlen );
		PKG_H_UINT32( buff, 203, sendlen );
		PKG_H_UINT32( buff, 0, sendlen );
		pkg_item_kind( p, buff, CP_GOLD_ITEM_ID, &sendlen );
		PKG_H_UINT32( buff, CP_GOLD_ITEM_ID, sendlen );
		PKG_H_UINT32( buff, cmd.cost, sendlen );

		pkg_item_kind( p, buff, cmd.item_id, &sendlen );
		PKG_H_UINT32( buff, cmd.item_id, sendlen );
		PKG_H_UINT32( buff, cmd.item_cnt, sendlen );
		PKG_H_UINT32( buff, 9999, sendlen );
		memcpy( p->session, &cmd, sizeof(cmd) );
		return send_request_to_db( SVR_PROTO_EXCHG_ITEM, p, sendlen, buff, p->id );
	}
}

int cutepig_buy_exchange_ok(sprite_t*p)
{
	cp_buy_cmd_t* buy_info = (cp_buy_cmd_t*)p->session;

	cp_add_exp_gold_info_t add_info;
	add_info.p = p;
	add_info.tar_id = p->id;
	add_info.cur_lv = p->cp_level;
	add_info.cur_exp = p->cp_exp;
	add_info.add_exp = NULL;
	add_info.add_gold = -buy_info->cost;
	add_info.is_from_db = 1;
	cutepig_add_exp_gold( &add_info );
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, buy_info->item_id, sendlen );
	PKG_UINT32( msg, buy_info->item_cnt, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}


int cutepig_sale_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t isPig;
	uint32_t expectLen = sizeof(isPig);
	CHECK_BODY_LEN_GE( len, expectLen );
	int j = 0;
	UNPKG_UINT32( body, isPig, j );
	if( 1 == isPig )
	{
		uint32_t pig_count;
		expectLen += sizeof(pig_count);
		CHECK_BODY_LEN_GE( len, expectLen );
		UNPKG_UINT32( body, pig_count, j );
		uint32_t pig_id;
		expectLen += pig_count * sizeof(pig_id);
		uint8_t db_buf[128];
		int dbLen = 0;
		PKG_H_UINT32( db_buf, pig_count, dbLen );
		int i;
		for( i = 0; i < pig_count; ++i )
		{
			UNPKG_UINT32( body, pig_id, j );
			PKG_H_UINT32( db_buf, pig_id, dbLen );
		}
		return send_request_to_db( SVR_PROTO_CP_SALE_PIG, p, dbLen, db_buf, p->id );
	}
	else
	{
		uint32_t item_id, item_cnt;
		UNPKG_UINT32( body, item_id, j );
		UNPKG_UINT32( body, item_cnt, j );
		item_kind_t* ik = find_kind_of_item(item_id);
		if( !ik || (ik->kind != CUTE_PIG_KIND && ik->kind != CUTE_PIG_ITEM_KIND && ik->kind != CUTE_PIG_CLOTHES_KIND) )
		{
			return send_to_self_error( p, p->waitcmd, -ERR_cutepig_invalid_item, 1 );
		}

		item_t* sale_item = get_item( ik, item_id );
		int sendlen = 0;
		uint8_t buff[1024];
		PKG_H_UINT32( buff, 1, sendlen );
		PKG_H_UINT32( buff, 1, sendlen );
		PKG_H_UINT32( buff, 203, sendlen );
		PKG_H_UINT32( buff, 0, sendlen );

		pkg_item_kind( p, buff, item_id, &sendlen );
		PKG_H_UINT32( buff, item_id, sendlen );
		PKG_H_UINT32( buff, item_cnt, sendlen );

		item_kind_t* ik_gold = find_kind_of_item(CP_GOLD_ITEM_ID);
		item_t* it_gold = get_item( ik_gold, CP_GOLD_ITEM_ID );

		pkg_item_kind( p, buff, CP_GOLD_ITEM_ID, &sendlen );
		PKG_H_UINT32( buff, CP_GOLD_ITEM_ID, sendlen );
		PKG_H_UINT32( buff, sale_item->sell_price * item_cnt, sendlen );
		PKG_H_UINT32( buff, it_gold->max, sendlen );
		uint32_t* award = (uint32_t*)p->session;
		*award = sale_item->sell_price * item_cnt;
		return send_request_to_db( SVR_PROTO_EXCHG_ITEM, p, sendlen, buff, p->id );
	}
}

int cutepig_sale_exchange_ok(sprite_t*p)
{
	uint32_t* award = (uint32_t*)p->session;
	cp_add_exp_gold_info_t add_info;
	add_info.p = p;
	add_info.tar_id = p->id;
	add_info.cur_lv = p->cp_level;
	add_info.cur_exp = p->cp_exp;
	add_info.add_exp = NULL;
	add_info.add_gold = *award;
	add_info.is_from_db = 1;
	cutepig_add_exp_gold( &add_info );
	int sendlen = sizeof(protocol_t);
	uint32_t result = 0;
	uint32_t sale_cnt = 0;
	PKG_UINT32( msg, *award, sendlen );
	PKG_UINT32( msg, result, sendlen );
	PKG_UINT32( msg, sale_cnt, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1);
}

int cutepig_sale_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct cp_sale_pig_back_s
	{
		uint32_t award;			//获得金钱数目
		uint32_t result;		//交易结果
		uint32_t sale_cnt;		//剩余贩卖数量
	}__attribute__((packed)) cp_sale_pig_back_t;
	CHECK_BODY_LEN( len, sizeof(cp_sale_pig_back_t) );
	cp_sale_pig_back_t* info = (cp_sale_pig_back_t*)buf;
	if( info->award > 0 )
	{
		cp_add_exp_gold_info_t add_info;
		add_info.p = p;
		add_info.tar_id = p->id;
		add_info.cur_lv = p->cp_level;
		add_info.cur_exp = p->cp_exp;
		add_info.add_exp = NULL;
		add_info.add_gold = info->award;
		add_info.is_from_db = 1;
		cutepig_add_exp_gold( &add_info );
		//统计 卖猪
		{
			uint32_t msgbuff[2]= {p->id, 1};
			msglog(statistic_logfile, 0x0409BD04, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
	}

	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, info->award, sendlen );
	PKG_UINT32( msg, info->result, sendlen );
	PKG_UINT32( msg, info->sale_cnt, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1);
}

int cutepig_mate_cmd(sprite_t* p, const uint8_t* body, int len)
{
	//program
	//get user's gold
	//get user's pig info
	//get tar pig info
	//notify tar pig mate
	//if callback ok notify user's pig mate include baby's infomation
	//notify to player mate ok
	CHECK_VALID_ID( p->id );
	CHECK_BODY_LEN( len, sizeof(uint32_t) * 3 );
	cp_mate_info_t* mate_info = (cp_mate_info_t*)p->session;
	int j = 0;
	UNPKG_UINT32( body, mate_info->my_pig_id, j );
	UNPKG_UINT32( body, mate_info->tar_user_id, j );
	UNPKG_UINT32( body, mate_info->tar_pig_id, j );
	if( mate_info->tar_user_id == p->id )
	{
		return cutepig_mate_result( p, CMR_INVALID_USER, 0 );
	}
	else if( mate_info->tar_user_id == CP_SYS_USER_ID )
	{
		if( mate_info->tar_pig_id != CP_SYS_PIG_ID )
		{
			return cutepig_mate_result( p, CMR_INVALID_PIG_ID, 0 );
		}
	}
	else
	{
		CHECK_VALID_ID( mate_info->tar_user_id );
	}
	mate_info->gold_cnt = p->yxb;
	//get self pig info
	return send_request_to_db( SVR_PROTO_CP_GET_PIG_INFO, p, sizeof(mate_info->my_pig_id), &mate_info->my_pig_id, p->id );

}

int cutepig_mate_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t result;
	CHECK_BODY_LEN( len, sizeof(result) );
	result = *(uint32_t*)buf;
	cp_mate_info_t* mate_info = (cp_mate_info_t*)p->session;
	if( CMR_OK != result )
	{
		mate_info->mate_cost = 0;
	}
	else
	{
		send_honor_event_to_db(NULL, CHT_10TIMES_MATE, 0, p->id);
	}

	return cutepig_mate_result( p, result, mate_info->mate_cost );
}

int cutepig_mate_result( sprite_t* p, uint32_t result, uint32_t cost )
{
	if( !p )
	{
		ERROR_LOG( "cutepig_mate_result Invalid Sprtite" );
		return -1;
	}
	if( CMR_OK == result )
	{
		cp_add_exp_gold_info_t add_info;
		add_info.p = p;
		add_info.tar_id = p->id;
		add_info.cur_lv = p->cp_level;
		add_info.cur_exp = p->cp_exp;
		add_info.add_exp = NULL;
		add_info.add_gold = -cost;
		add_info.is_from_db = 1;
		cutepig_add_exp_gold( &add_info );
		//统计 培育
		{
			uint32_t msgbuff[2]= {p->id, 1};
			msglog(statistic_logfile, 0x0409BD01, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
	}
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, result, sendlen );
	PKG_UINT32( msg, cost, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen);
	return send_to_self( p, msg, sendlen, 1 );
}

int cutepig_get_players_male_pig_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t userID;
	CHECK_BODY_LEN( len , sizeof(userID) );
	int j = 0;
	UNPKG_UINT32( body, userID, j );
	if( userID == CP_SYS_USER_ID )
	{
		item_kind_t* ik	= find_kind_of_item( CP_SYS_PIG_ITEM_ID );
		item_t* it		= get_item( ik, CP_SYS_PIG_ITEM_ID );

		int sendlen = sizeof(protocol_t);
		PKG_UINT32( msg, 1, sendlen );
		PKG_UINT32( msg, CP_SYS_PIG_ID, sendlen );
		PKG_UINT32( msg, CP_SYS_PIG_ITEM_ID, sendlen );
		PKG_UINT32( msg, it->u.cp_base_info_tag.breed, sendlen );
		PKG_UINT32( msg, 1, sendlen );
		PKG_STR( msg, it->name, sendlen, CUTE_PIG_MAX_NAME_LEN );
		init_proto_head( msg, p->waitcmd, sendlen );
		return send_to_self( p, msg, sendlen, 1 );
	}
	else
	{
		return send_request_to_db( SVR_PROTO_CP_GET_MALE_PIG_INFO, p, 0, NULL, userID );
	}
}

int cutepig_get_players_male_pig_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t	pig_id;
		uint32_t	item_id;
		uint32_t	breed;
		uint32_t	generation;
		uint8_t		pig_name[CUTE_PIG_MAX_NAME_LEN];
	}__attribute__((packed)) cp_male_pig_info_t;

	uint32_t pigCount;
	int expectLen = sizeof(pigCount);
	CHECK_BODY_LEN_GE( len , expectLen );
	int j = 0;
	UNPKG_H_UINT32( buf, pigCount, j );
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, pigCount, sendlen );
	expectLen += pigCount * sizeof(cp_male_pig_info_t);
	CHECK_BODY_LEN( len, expectLen );
	cp_male_pig_info_t info;
	int i;
	for( i = 0; i < pigCount; ++i )
	{
		UNPKG_H_UINT32( buf, info.pig_id, j );
		UNPKG_H_UINT32( buf, info.item_id, j );
		UNPKG_H_UINT32( buf, info.breed, j );
		UNPKG_H_UINT32( buf, info.generation, j );
		UNPKG_STR( buf, info.pig_name, j, sizeof(info.pig_name) );
		PKG_UINT32( msg, info.pig_id, sendlen );
		PKG_UINT32( msg, info.item_id, sendlen );
		PKG_UINT32( msg, info.breed, sendlen );
		PKG_UINT32( msg, info.generation, sendlen );
		PKG_STR( msg, info.pig_name, sendlen, sizeof(info.pig_name) );
	}
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}
int cutepig_add_exp_gold( cp_add_exp_gold_info_t* info )
{
	if( !info )
	{
		return -1;
	}
	if( info->p && info->p->id != info->tar_id )
	{
		ERROR_RETURN( ( "cutepig_add_exp_gold sprite exist but id not eque id[%u], p->id[%u]", info->tar_id, info->p->id ), -1 );
	}
	//猪倌没有创建
	if( info->cur_lv == 0 )
	{
		return -1;
	}

	if( info->p )
	{
		info->p->yxb += info->add_gold;
	}
	if( info->add_exp )
	{
		int32_t db_exp_mod = 0;
		if( 1 == info->is_from_db )
		{
			info->cur_exp += *info->add_exp;
			if( info->cur_exp > level_info[CP_CUR_MAX_LEVEL - 1].exp )
			{
				db_exp_mod = info->cur_exp - level_info[CP_CUR_MAX_LEVEL - 1].exp;
				if( *info->add_exp < db_exp_mod )
				{
					*info->add_exp = 0;
				}
				else
				{
					*info->add_exp -= db_exp_mod;
				}
				info->cur_exp = level_info[CP_CUR_MAX_LEVEL - 1].exp;
				cutepig_db_add_exp( info->tar_id, -db_exp_mod );
			}
		}
		else if( 0 == info->is_from_db )
		{
			if( info->cur_exp + *info->add_exp > level_info[CP_CUR_MAX_LEVEL - 1].exp )
			{
				int32_t reduce = info->cur_exp - level_info[CP_CUR_MAX_LEVEL - 1].exp;
				if( reduce < 0 )
				{
					*info->add_exp += reduce;
					db_exp_mod = *info->add_exp;
				}
				else
				{
					*info->add_exp = 0;
					db_exp_mod -= reduce;
				}
				info->cur_exp = level_info[CP_CUR_MAX_LEVEL - 1].exp;
			}
			else
			{
				db_exp_mod = *info->add_exp;
				info->cur_exp += *info->add_exp;
			}
			cutepig_db_add_exp( info->tar_id, db_exp_mod );
		}
	}
	if( info->p )
	{
		info->p->cp_exp = info->cur_exp;
	}

	uint32_t cur_level = cutepig_get_cur_level( info->cur_exp );
	if( cur_level > info->cur_lv )
	{
		info->cur_lv = cur_level;
		if( info->p )
		{
			info->p->cp_level = info->cur_lv;
			int sendlen = sizeof(protocol_t);
			uint8_t send_buf[64];
			PKG_UINT32( send_buf, cur_level, sendlen );
			PKG_UINT32( send_buf, level_info[cur_level - 1].award_cnt, sendlen );
			int i;
			for( i = 0; i < level_info[cur_level - 1].award_cnt; ++i )
			{
				PKG_UINT32( send_buf, level_info[cur_level - 1].award_info[i].item_id, sendlen );
				PKG_UINT32( send_buf, level_info[cur_level - 1].award_info[i].item_cnt, sendlen );
			
			}

			init_proto_head( send_buf, PROTO_CP_LEVEL_UP_NOTICE, sendlen );
			send_to_self( info->p, send_buf, sendlen, 0 );
		}
		//add items, notice DB
		int sendlen = 0;
		uint8_t buff[1024];
		PKG_H_UINT32( buff, 0, sendlen );
		PKG_H_UINT32( buff, level_info[cur_level - 1].award_cnt, sendlen );
		PKG_H_UINT32( buff, 0, sendlen );
		PKG_H_UINT32( buff, 0, sendlen );
		int i;
		for ( i = 0; i < level_info[cur_level - 1].award_cnt; ++i )
		{
			pkg_item_kind( NULL, buff, level_info[cur_level - 1].award_info[i].item_id, &sendlen );
			PKG_H_UINT32( buff, level_info[cur_level - 1].award_info[i].item_id, sendlen );
			PKG_H_UINT32( buff, level_info[cur_level - 1].award_info[i].item_cnt, sendlen );
			item_kind_t* ik = find_kind_of_item( level_info[cur_level - 1].award_info[i].item_id );
			item_t* it = get_item( ik, level_info[cur_level - 1].award_info[i].item_id );
			PKG_H_UINT32( buff, it->max, sendlen );
		}
		send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, info->tar_id );

		send_request_to_db( SVR_PROTO_CP_SET_GLOBLE_LEVEL, NULL, sizeof(cur_level), &cur_level, info->tar_id );
		send_request_to_db( SVR_PROTO_CP_UPDATE_LEVEL, NULL, sizeof(cur_level), &cur_level, info->tar_id );

		//统计 等级
		{
			uint32_t msgbuff[2]= {info->tar_id, info->cur_lv};
			msglog(statistic_logfile, 0x0409BD05, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
	}
	return 0;
}

int cutepig_get_cur_level( uint32_t exp )
{
	int i;
	for( i = CP_MAX_LEVEL - 1; i >= 0; --i )
	{
		if( exp >= level_info[i].exp )
		{
			return i + 1;
		}
	}
	ERROR_LOG( "cutepig_get_cur_level ERROR" );
	return 1;
}

int cutepig_load_config_file()
{
	int ret = cutepig_load_level_info("./conf/cp_level_info.xml");
	if( 0 != ret )
	{
		return ret;
	}
	ret = cutepig_load_task_info("./conf/cp_task_info.xml");
	if( 0 != ret)
	{
		return ret;
	}
	ret = cutepig_load_process_info( "./conf/cp_process_info.xml" );
	if( 0 != ret)
	{
		return ret;
	}

	ret = cutepig_load_bs_npc_pk_award_info( "./conf/cpBsNpcPKAwardInfo.xml" );
	if( 0 != ret)
	{
		return ret;
	}
	ret = cutepig_load_ie_info( "./conf/cp_ie_info.xml" );
	if( 0 != ret)
	{
		return ret;
	}

	return ret;
}

int cutepig_load_level_info( char* file )
{
	xmlDocPtr doc;
	xmlNodePtr root;
	int err = -1;

	doc = xmlParseFile (file);
	if (!doc)
	{
		ERROR_RETURN (("load cutepig level info failed"), -1);
	}

	root = xmlDocGetRootElement(doc);
	if (!root)
	{
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}
	if( root )
	{
		xmlNodePtr chl_element_group = root->children;
		uint32_t cur_level = 1;
		uint32_t expect_level = 1;
		while( chl_element_group )
		{
			if( 0 == xmlStrcmp( chl_element_group->name, (const xmlChar *)"LevelInfo" ) )
			{
				if( expect_level > CP_MAX_LEVEL )
				{
					ERROR_LOG( "Parse CutePig Level Info out of range" );
					break;
				}
				DECODE_XML_PROP_UINT32( cur_level, chl_element_group, "Level");
				if( cur_level != expect_level )
				{
					DEBUG_LOG( "Parse CutePig Level Info Invalid Level:[%u], Expect:[%u]", cur_level, expect_level );
				}
				DECODE_XML_PROP_UINT32( level_info[expect_level - 1].exp, chl_element_group, "Exp");
				uint32_t* award_cnt = &level_info[expect_level - 1].award_cnt;
				*award_cnt = 0;
				cp_levelup_award* award = level_info[expect_level - 1].award_info;
				xmlNodePtr child = chl_element_group->children;
				while( child )
				{
					if( 0 == xmlStrcmp( child->name, (const xmlChar *)"Award" ) )
					{
						DECODE_XML_PROP_UINT32( award[*award_cnt].item_id, child, "ItemID");
						DECODE_XML_PROP_UINT32( award[*award_cnt].item_cnt, child, "ItemCnt");
						++(*award_cnt);
					}
					child = child->next;
				}
				++expect_level;
			}
			chl_element_group = chl_element_group->next;
		}
		//后续经验补齐
		int i;
		for( i = expect_level - 1; i < CP_MAX_LEVEL; ++i )
		{
			level_info[i].exp = 99999999;
		}
	}
	err = 0;
	DEBUG_LOG( "CutePig Load Level Info OK" );
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load CutePig Level Info %s", file);
}

int cutepig_load_task_info( char* file )
{
	xmlDocPtr doc;
	xmlNodePtr root;
	int err = -1;

	doc = xmlParseFile (file);
	if (!doc)
	{
		ERROR_RETURN (("load cutepig task info failed"), -1);
	}

	root = xmlDocGetRootElement(doc);
	if (!root)
	{
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}
	if( root )
	{
		xmlNodePtr chl_element_group = root->children;
		uint32_t cur_ID = 0;
		uint32_t* expect_ID = &cp_task_info.task_count;
		*expect_ID = 0;
		while( chl_element_group )
		{
			if( 0 == xmlStrcmp( chl_element_group->name, (const xmlChar *)"Task" ) )
			{
				if( *expect_ID >= CP_MAX_TASK_CNT )
				{
					ERROR_LOG( "Parse CutePig Task Info out of range" );
					break;
				}
				DECODE_XML_PROP_UINT32( cur_ID, chl_element_group, "ID");
				if( cur_ID != (*expect_ID) + 1 )
				{
					DEBUG_LOG( "Parse CutePig Task Info Invalid Level:[%u], Expect:[%u]", cur_ID, (*expect_ID) + 1 );
				}
				DECODE_XML_PROP_UINT32( cp_task_info.task_info[*expect_ID].award_exp, chl_element_group, "Exp");
				DECODE_XML_PROP_UINT32( cp_task_info.task_info[*expect_ID].award_gold, chl_element_group, "Gold");
				DECODE_XML_PROP_UINT32( cp_task_info.task_info[*expect_ID].count, chl_element_group, "Count");
				DECODE_XML_PROP_UINT32( cp_task_info.task_info[*expect_ID].min_level, chl_element_group, "MinLevel");
				DECODE_XML_PROP_UINT32( cp_task_info.task_info[*expect_ID].max_level, chl_element_group, "MaxLevel");
				DECODE_XML_PROP_UINT32( cp_task_info.task_info[*expect_ID].task_type, chl_element_group, "Type");
				uint32_t* awardCnt = &cp_task_info.task_info[*expect_ID].item_kind;
				*awardCnt = 0;
				xmlNodePtr child = chl_element_group->children;
				while( child )
				{
					if( 0 == xmlStrcmp( child->name, (const xmlChar *)"Award" ) )
					{
						if( *awardCnt >= CUTE_PIG_TASK_AWARD_ITEM_CNT )
						{
							ERROR_LOG( "Parse CutePig Task Info Award out of range TaskID:[%u]", *expect_ID );
							break;
						}
						DECODE_XML_PROP_UINT32( cp_task_info.task_info[*expect_ID].item_info[*awardCnt].item_id, child, "ItemID");
						DECODE_XML_PROP_UINT32( cp_task_info.task_info[*expect_ID].item_info[*awardCnt].item_cnt, child, "Count");
						++(*awardCnt);
					}
					child = child->next;
				}
				++(*expect_ID);
			}
			chl_element_group = chl_element_group->next;
		}
	}
	err = 0;
	DEBUG_LOG( "CutePig Load Task Info OK Task Cout:[%u]", cp_task_info.task_count );
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load CutePig Task Info %s", file);
}

int cutepig_load_process_info( char* file )
{
	xmlDocPtr doc;
	xmlNodePtr root;
	int err = -1;

	doc = xmlParseFile (file);
	if (!doc)
	{
		ERROR_RETURN (("load cutepig process info failed"), -1);
	}

	root = xmlDocGetRootElement(doc);
	if (!root)
	{
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}
	if( root )
	{
		xmlNodePtr chl_element_group = root->children;
		uint32_t cur_ID = 0;
		uint32_t* expect_ID = &cp_process_info.process_count;
		*expect_ID = 0;
		while( chl_element_group )
		{
			if( 0 == xmlStrcmp( chl_element_group->name, (const xmlChar *)"Proc" ) )
			{
				if( *expect_ID >= CUTE_PIG_MAX_PROCESS_TYPE )
				{
					ERROR_LOG( "Parse CutePig Process Info out of range" );
					break;
				}
				DECODE_XML_PROP_UINT32( cur_ID, chl_element_group, "ID");
				if( cur_ID != *expect_ID)
				{
					DEBUG_LOG( "Parse CutePig Process Info Invalid ID:[%u], Expect:[%u]", cur_ID, *expect_ID );
				}
				DECODE_XML_PROP_UINT32( cp_process_info.process_info[*expect_ID].award, chl_element_group, "Award" );
				DECODE_XML_PROP_UINT32( cp_process_info.process_info[*expect_ID].pig_breed, chl_element_group, "PigBreed" );
				DECODE_XML_PROP_UINT32( cp_process_info.process_info[*expect_ID].proc_level, chl_element_group, "ProcLv" );

				uint32_t* ItemCnt = &cp_process_info.process_info[*expect_ID].item_kind_in;
				*ItemCnt = 0;
				xmlNodePtr child = chl_element_group->children;
				while( child )
				{
					if( 0 == xmlStrcmp( child->name, (const xmlChar *)"ItemIn" ) )
					{
						if( *ItemCnt >= CUTE_PIG_MAX_PROCESS_ITEM_KIND )
						{
							ERROR_LOG( "Parse CutePig Process Info Item Kind out of range ProcID:[%u]", *expect_ID );
							break;
						}
						DECODE_XML_PROP_UINT32( cp_process_info.process_info[*expect_ID].item_id_in[*ItemCnt], child, "ID" );
						DECODE_XML_PROP_UINT32( cp_process_info.process_info[*expect_ID].item_cnt_in[*ItemCnt], child, "Count" );
						++(*ItemCnt);
					}
					child = child->next;
				}
				++(*expect_ID);
			}
			chl_element_group = chl_element_group->next;
		}
	}
	err = 0;
	DEBUG_LOG( "CutePig Load Process Info OK Process Count:[%u]", cp_process_info.process_count );
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load CutePig Process Info %s", file);
}

int cutepig_get_friend_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t count;
	uint32_t user_id;
	CHECK_BODY_LEN_GE( len, sizeof(count) );
	int j = 0;
	UNPKG_UINT32( body, count, j );
	if(count > 250)
	{
		ERROR_RETURN(("too many users, count=%d", count), -1);
	}
	CHECK_BODY_LEN( len, sizeof(count) + count*sizeof(user_id) );
	uint8_t db_buf[1024];
	int sendlen = 0;
	PKG_H_UINT32( db_buf, count, sendlen );
	int i;
	for( i = 0; i < count; ++i )
	{
		UNPKG_UINT32( body, user_id, j );
		PKG_H_UINT32( db_buf, user_id, sendlen );
	}
	return send_request_to_db( SVR_PROTO_CP_GET_FRIEND_INFO, p, sendlen, db_buf, p->id );
}

int cutepig_get_friend_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int expectLen = sizeof(uint32_t);
	CHECK_BODY_LEN_GE( len, expectLen );
	uint32_t count;
	int j = 0;
	UNPKG_H_UINT32( buf, count, j );
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, count, sendlen );
	int i;
	uint32_t user_id;
	uint32_t cp_level;
	for( i = 0; i < count; ++i )
	{
		UNPKG_H_UINT32( buf, user_id, j );
		UNPKG_H_UINT32( buf, cp_level, j );
		PKG_UINT32( msg, user_id, sendlen );
		PKG_UINT32( msg, cp_level, sendlen );
	}
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}

int cutepig_get_task_list_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	return send_request_to_db( SVR_PROTO_CP_GET_TASK_INFO, p, 0, NULL, p->id );
}

int cutepig_get_task_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct
	{
		uint32_t task_id;
		uint32_t task_state;
		uint32_t task_cnt;
	}back_task_info_t;
	uint32_t count;
	int expectLen = sizeof(count);
	CHECK_BODY_LEN_GE( len, expectLen );
	int j = 0;
	UNPKG_H_UINT32( buf, count, j );
	expectLen += count * sizeof(back_task_info_t);
	CHECK_BODY_LEN( len, expectLen );
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, count, sendlen );
	int i;
	for( i = 0; i < count; ++i )
	{
		back_task_info_t* info = (back_task_info_t*)(buf + j);
		PKG_UINT32( msg, info->task_id, sendlen );
		PKG_UINT32( msg, info->task_cnt, sendlen );
		PKG_UINT32( msg, info->task_state, sendlen );
		j += sizeof(back_task_info_t);
	}
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1);
}

int cutepig_accept_task_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t task_id;
	CHECK_BODY_LEN( len, sizeof(task_id) );
	int j = 0;
	UNPKG_UINT32( body, task_id, j );
	DEBUG_LOG("[userid: %u, taskid: %u]", p->id, task_id);
	if(task_id > 0 && task_id <= cp_task_info.task_count &&
		p->cp_level >= cp_task_info.task_info[task_id - 1].min_level &&
		p->cp_level <= cp_task_info.task_info[task_id - 1].max_level )
	{
		uint32_t db_buf[] = { task_id, cp_task_info.task_info[task_id - 1].task_type, cp_task_info.task_info[task_id - 1].count };
		return send_request_to_db( SVR_PROTO_CP_ACCEPT_TASK, p, sizeof(db_buf), db_buf, p->id );
	}
	else
	{
		return send_to_self_error( p, p->waitcmd, -ERR_cutepig_invalid_task, 1 );
	}
}

int cutepig_accept_task_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t isOK;
	CHECK_BODY_LEN( len, sizeof(isOK) );
	isOK = *(uint32_t*)buf;
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, isOK, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}

int cutepig_submit_task_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t task_id;
	int j = 0;
	UNPKG_UINT32( body, task_id, j );
	*(uint32_t*)p->session = task_id;
	if( task_id > 0 && task_id <= cp_task_info.task_count )
	{
		return send_request_to_db( SVR_PROTO_CP_SUBMIT_TASK, p, sizeof(task_id), &task_id, p->id );
	}
	else
	{
		return send_to_self_error( p, p->waitcmd, -ERR_cutepig_invalid_task, 1 );
	}
}

int cutepig_submit_task_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t isOK;
	CHECK_BODY_LEN( len, sizeof(isOK) );
	isOK = *(uint32_t*)buf;
	if( 1 == isOK )
	{
		uint32_t task_id = *(uint32_t*)p->session;
		task_id -= 1;
		int sendlen = 0;
		uint8_t buff[1024];
		PKG_H_UINT32( buff, 0, sendlen );
		PKG_H_UINT32( buff, 1 + cp_task_info.task_info[task_id].item_kind, sendlen );
		PKG_H_UINT32( buff, 203, sendlen );
		PKG_H_UINT32( buff, 0, sendlen );

		item_kind_t* ik_gold = find_kind_of_item(CP_GOLD_ITEM_ID);
		item_t* it_gold = get_item( ik_gold, CP_GOLD_ITEM_ID );

		uint32_t add_exp = cp_task_info.task_info[task_id].award_exp;
		cp_add_exp_gold_info_t add_info;
		add_info.p = p;
		add_info.tar_id = p->id;
		add_info.cur_lv = p->cp_level;
		add_info.cur_exp = p->cp_exp;
		add_info.add_exp = &add_exp;
		add_info.add_gold = cp_task_info.task_info[task_id].award_gold;
		add_info.is_from_db = 0;
		cutepig_add_exp_gold( &add_info );

		pkg_item_kind( p, buff, CP_GOLD_ITEM_ID, &sendlen );
		PKG_H_UINT32( buff, CP_GOLD_ITEM_ID, sendlen );
		PKG_H_UINT32( buff, cp_task_info.task_info[task_id].award_gold, sendlen );
		PKG_H_UINT32( buff, it_gold->max, sendlen );
		int i;
		item_kind_t* ik_item;
		item_t* it_item;
		for( i = 0; i < cp_task_info.task_info[task_id].item_kind; ++i )
		{
			ik_item = find_kind_of_item( cp_task_info.task_info[task_id].item_info[i].item_id );
			it_item = get_item( ik_item, cp_task_info.task_info[task_id].item_info[i].item_id );
			pkg_item_kind( p, buff, cp_task_info.task_info[task_id].item_info[i].item_id, &sendlen );
			PKG_H_UINT32( buff, cp_task_info.task_info[task_id].item_info[i].item_id, sendlen );
			PKG_H_UINT32( buff, cp_task_info.task_info[task_id].item_info[i].item_cnt, sendlen );
			PKG_H_UINT32( buff, ik_item->max, sendlen );
		}
		send_request_to_db( SVR_PROTO_EXCHG_ITEM, p, sendlen, buff, p->id );

		//send to clietn
		sendlen = sizeof(protocol_t);
		PKG_UINT32( msg, isOK, sendlen );
		PKG_UINT32( msg, cp_task_info.task_info[task_id].award_gold, sendlen );
		PKG_UINT32( msg, add_exp, sendlen );
		PKG_UINT32( msg, cp_task_info.task_info[task_id].item_kind, sendlen );
		for( i = 0; i < cp_task_info.task_info[task_id].item_kind; ++i )
		{
			PKG_UINT32( msg, cp_task_info.task_info[task_id].item_info[i].item_id, sendlen );
			PKG_UINT32( msg, cp_task_info.task_info[task_id].item_info[i].item_cnt, sendlen );
		}
		init_proto_head( msg, p->waitcmd, sendlen );
		return send_to_self( p, msg, sendlen, 1 );
	}
	else
	{
		int sendlen = sizeof(protocol_t);
		PKG_UINT32( msg, isOK, sendlen );
		PKG_UINT32( msg, 0, sendlen );
		PKG_UINT32( msg, 0, sendlen );
		PKG_UINT32( msg, 0, sendlen );
		init_proto_head( msg, p->waitcmd, sendlen );
		return send_to_self( p, msg, sendlen, 1 );
	}
}

int cutepig_process_cmd(sprite_t* p, const uint8_t* body, int len)
{
	typedef struct
	{
		uint32_t proc_id;
		uint32_t pig_id;
	}__attribute__((packed)) cp_process_cmd_t;
	CHECK_VALID_ID( p->id );
	cp_process_cmd_t cmd;
	CHECK_BODY_LEN( len, sizeof(cmd) );
	int j = 0;
	UNPKG_UINT32( body, cmd.proc_id, j );
	UNPKG_UINT32( body, cmd.pig_id, j );
	if( cmd.proc_id < cp_process_info.process_count )
	{
		*(uint32_t*)p->session = cmd.proc_id;
		uint8_t db_buf[128];
		int j = 0;
		PKG_H_UINT32( db_buf, cmd.pig_id, j );
		PKG_H_UINT32( db_buf, cp_process_info.process_info[cmd.proc_id].pig_breed, j );
		PKG_H_UINT32( db_buf, cp_process_info.process_info[cmd.proc_id].proc_level, j );
		PKG_H_UINT32( db_buf, cp_process_info.process_info[cmd.proc_id].award, j );

		PKG_H_UINT32( db_buf, cp_process_info.process_info[cmd.proc_id].item_kind_in, j );
		int i;
		for( i = 0; i < cp_process_info.process_info[cmd.proc_id].item_kind_in; ++i )
		{
			PKG_H_UINT32( db_buf, cp_process_info.process_info[cmd.proc_id].item_id_in[i], j );
			PKG_H_UINT32( db_buf, cp_process_info.process_info[cmd.proc_id].item_cnt_in[i], j );
		}
		return send_request_to_db( SVR_PROTO_CP_PROCESS, p, j, &db_buf, p->id );
	}
	else
	{
		return send_to_self_error( p, p->waitcmd, -ERR_cutepig_invalid_process, 1 );
	}
}

int cutepig_process_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct
	{
		uint32_t result;
		uint32_t award;
		uint32_t pig_item_id;
	}__attribute__((packed)) cp_process_back_t;

	CHECK_BODY_LEN( len, sizeof(cp_process_back_t) );
	cp_process_back_t* back = (cp_process_back_t*)buf;
	uint32_t add_exp;
	if( 0 == back->result )
	{
		item_kind_t* ik = find_kind_of_item( back->pig_item_id );
		item_t* it = get_item( ik, back->pig_item_id );
		add_exp = it->u.cp_base_info_tag.proc_exp;
		cp_add_exp_gold_info_t add_info;
		add_info.p = p;
		add_info.tar_id = p->id;
		add_info.cur_lv = p->cp_level;
		add_info.cur_exp = p->cp_exp;
		add_info.add_exp = &add_exp;
		add_info.add_gold = 0;
		add_info.is_from_db = 0;
		cutepig_add_exp_gold( &add_info );

		send_honor_event_to_db(NULL, CHT_1TIMES_MAKE, 0, p->id);
		send_honor_event_to_db(NULL, CHT_10TIMES_DISH, 0, p->id);

		//统计 加工
		{
			uint32_t msgbuff[2]= {p->id, 1};
			msglog(statistic_logfile, 0x0409BCFF, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
		uint32_t proc_id = *(uint32_t*)p->session;
		//统计 加工明细
		{
			uint32_t msgbuff[2]= {p->id, 1};
			msglog(statistic_logfile, 0x0409BD4A + proc_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
	}
	else
	{
		add_exp = 0;
	}
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, back->result, sendlen );
	PKG_UINT32( msg, back->award, sendlen );
	PKG_UINT32( msg, add_exp, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}

int cutepig_get_notice_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t user_id;
	int j = 0;
	UNPKG_UINT32( body, user_id, j );
	return send_request_to_db( SVR_PROTO_CP_GET_NOTICE_INFO, p, 0, NULL, user_id );
}

int cutepig_get_notice_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct
	{
		uint32_t time;
		uint32_t type;
		uint32_t opt_user_id;
		uint32_t itemid;
		uint32_t param;
		uint8_t	 name[CUTE_PIG_MAX_NAME_LEN];
	}__attribute__((packed))cp_notice_info_t;
	uint32_t count;
	CHECK_BODY_LEN_GE( len, sizeof(count) );
	int j = 0;
	UNPKG_H_UINT32( buf, count, j );
	CHECK_BODY_LEN( len, sizeof(count) + count * sizeof(cp_notice_info_t) );
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, count, sendlen );
	cp_notice_info_t* notice;
	int i;
	for( i = 0; i < count; ++i )
	{
		notice = (cp_notice_info_t*)(buf + j + sizeof(cp_notice_info_t) * i );
		PKG_UINT32( msg, notice->time, sendlen );
		PKG_UINT32( msg, notice->type, sendlen );
		PKG_UINT32( msg, notice->opt_user_id, sendlen );
		PKG_UINT32( msg, notice->itemid, sendlen );
		PKG_UINT32( msg, notice->param, sendlen );
		PKG_STR( msg, notice->name, sendlen, sizeof(notice->name) );
	}
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}

int cutepig_get_rand_lucky_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t isGetLucky;
	CHECK_BODY_LEN( len, sizeof(isGetLucky) );
	int j = 0;
	UNPKG_UINT32( body, isGetLucky, j );
	if( !isGetLucky || get_now_tv()->tv_sec - p->cp_last_lucky_time < CP_RAND_LUCKY_TIME )
	{
		int sendlen = sizeof(protocol_t);
		PKG_UINT32( msg, 0, sendlen );
		PKG_UINT32( msg, 0, sendlen );
		PKG_UINT32( msg, 0, sendlen );
		PKG_UINT32( msg, CP_RAND_LUCKY_TIME - ( get_now_tv()->tv_sec - p->cp_last_lucky_time ), sendlen );
		init_proto_head( msg, p->waitcmd, sendlen );
		return send_to_self( p, msg, sendlen, 1 );
	}
	else
	{
		return send_request_to_db( SVR_PROTO_CP_GET_RAND_LUCKY, p, 0, NULL, p->id );
	}
}

int cutepig_get_rand_lucky_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct
	{
		uint32_t result;
		uint32_t item_id;
		uint32_t item_cnt;
	}__attribute__((packed)) cp_rand_lucky_back_t;

	CHECK_BODY_LEN( len, sizeof(cp_rand_lucky_back_t) );

	//统计 领取随机好运
	{
		uint32_t msgbuff[2]= {p->id, 1};
		msglog(statistic_logfile, 0x0409BCFC, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}

	p->cp_last_lucky_time = get_now_tv()->tv_sec;
	cp_rand_lucky_back_t* back = (cp_rand_lucky_back_t*)buf;
	if( back->item_id == CP_EXP_ITEM_ID )
	{
		cp_add_exp_gold_info_t add_info;
		add_info.p = p;
		add_info.tar_id = p->id;
		add_info.cur_lv = p->cp_level;
		add_info.cur_exp = p->cp_exp;
		add_info.add_exp = &(back->item_cnt);
		add_info.add_gold = 0;
		add_info.is_from_db = 1;
		cutepig_add_exp_gold( &add_info );
	}
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, back->result, sendlen );
	PKG_UINT32( msg, back->item_id, sendlen );
	PKG_UINT32( msg, back->item_cnt, sendlen );
	PKG_UINT32( msg, CP_RAND_LUCKY_TIME, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	send_to_self( p, msg, sendlen, 1 );
	return 0;
}

int cutepig_get_illustrate_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	return send_request_to_db( SVR_PROTO_CP_GET_ILLUSTRATE, p, 0, NULL, p->id );
}

int cutepig_get_illustrate_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t count;
	uint32_t breed;
	CHECK_BODY_LEN_GE( len, sizeof(count) );
	int j = 0;
	UNPKG_H_UINT32( buf, count, j );
	CHECK_BODY_LEN( len, sizeof(count) + count * sizeof(breed) );
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, count, sendlen );
	int i;
	for( i = 0; i < count; ++i )
	{
		UNPKG_H_UINT32( buf, breed, j );
		PKG_UINT32( msg, breed, sendlen );
	}
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}

int cutepig_db_add_exp( uint32_t user_id, int32_t exp )
{
	if( exp == 0 )
	{
		return 0;
	}
	int dblen = 0;
	uint8_t buff[128];
	PKG_H_UINT32( buff, exp > 0 ? 0 : 1, dblen );
	PKG_H_UINT32( buff, exp > 0 ? 1 : 0, dblen );
	PKG_H_UINT32( buff, 0, dblen );
	PKG_H_UINT32( buff, 0, dblen );
	pkg_item_kind( NULL, buff, CP_EXP_ITEM_ID, &dblen );
	PKG_H_UINT32( buff, CP_EXP_ITEM_ID, dblen );
	if( exp > 0 )
	{
		PKG_H_UINT32( buff, exp, dblen );
		item_kind_t* ik = find_kind_of_item( CP_EXP_ITEM_ID );
		item_t* it = get_item( ik, CP_EXP_ITEM_ID );
		PKG_H_UINT32( buff, it->max, dblen );
	}
	else
	{
		PKG_H_UINT32( buff, -exp, dblen );
	}
	return send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, dblen, buff, user_id );
}

int cutepig_upgrade_building_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t building_id;
	CHECK_BODY_LEN( len, sizeof(building_id) );
	int j = 0;
	UNPKG_UINT32( body, building_id, j );
	return send_request_to_db( SVR_PROTO_CP_UPGRADE_BUILDING, p, sizeof(building_id), &building_id, p->id );
}

int cutepig_upgrade_building_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct
	{
		uint32_t result;
		uint32_t cost;
	}__attribute__((packed)) cp_upgrade_back_t;
	CHECK_BODY_LEN( len, sizeof(cp_upgrade_back_t) );
	cp_upgrade_back_t* back = (cp_upgrade_back_t*)buf;
	p->yxb -= back->cost;
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, back->result, sendlen );
	PKG_UINT32( msg, back->cost, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}

int cutepig_use_item_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t item_id;
	uint32_t tar_id;
	CHECK_BODY_LEN( len, sizeof(item_id) + sizeof(tar_id) );
	int j = 0;
	UNPKG_UINT32( body, item_id, j );
	UNPKG_UINT32( body, tar_id, j );

	item_kind_t* ik = find_kind_of_item( item_id );
	if( !ik )
	{
		ERROR_RETURN( ("Use Item Invalid Item ID:[%u]", item_id ), -1 );
	}
	switch( ik->kind )
	{
	case CUTE_PIG_ITEM_KIND:
		break;
	case CUTE_PIG_CLOTHES_KIND:
		break;
	default:
		ERROR_RETURN( ("Use Item Invalid Item Kind:[%u]", ik->kind ), -1 );
	}
	item_t* it = get_item( ik, item_id );
	if( !it )
	{
		ERROR_RETURN( ("Use Item Invalid Item ID:[%u]", item_id ), -1 );
	}

	switch( ik->kind )
	{
	case CUTE_PIG_ITEM_KIND:
		{
			switch( it->u.cp_item_tag.type )
			{
			case CIK_SP_ITEM:
				{
					*(uint32_t*)p->session = item_id;
					*(uint32_t*)( p->session + sizeof(item_id) ) = tar_id;
					uint32_t db_buf[] = { item_id, tar_id };
					return send_request_to_db( SVR_PROTO_CP_USE_ITEM, p, sizeof(db_buf), &db_buf, p->id );
				}
				break;
			case CIK_SHOW_ITEM:
				{
					*(uint32_t*)p->session = item_id;
					uint32_t db_buf[] = { item_id, it->u.cp_item_tag.time_last };
					return send_request_to_db( SVR_PROTO_CP_USE_SHOW_STAGE_ITEM, p, sizeof(db_buf), &db_buf, p->id );
				}
				break;
			default:
				{
					ERROR_RETURN( ("Use Item Invalid Item ID:[%u]", item_id ), -1 );
				}
				break;
			}
		}
		break;
	case CUTE_PIG_CLOTHES_KIND:
		{
			*(uint32_t*)p->session = item_id;
			*(uint32_t*)( p->session + sizeof(item_id) ) = tar_id;
			uint32_t is_off = item_id == CP_TAKE_OFF_CLOTHES_ITEM_ID ? 1 : 0;
			uint32_t db_buf[] = { tar_id, item_id, is_off };
			return send_request_to_db( SVR_PROTO_CP_SET_CLOTHES, p, sizeof(db_buf), &db_buf, p->id );
		}
		break;
	default:
		break;
	}
	return 0;
}

int cutepig_use_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t result;
	CHECK_BODY_LEN( len, sizeof(result) );
	result = *(uint32_t*)buf;

	if( *(uint32_t*)p->session == 1613108 )
	{
		send_honor_event_to_db(NULL, CHT_5TIMES_EXTEND, 0, p->id);
	}

	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, result, sendlen );
	PKG_UINT32( msg, *(uint32_t*)p->session, sendlen );
	PKG_UINT32( msg, *(uint32_t*)( p->session + sizeof(uint32_t) ), sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}

int cutepig_use_show_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t result;
	CHECK_BODY_LEN( len, sizeof(result) );
	result = *(uint32_t*)buf;
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, result, sendlen );
	PKG_UINT32( msg, *(uint32_t*)p->session, sendlen );
	uint32_t nil = 0;
	PKG_UINT32( msg, nil, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}

int cutepig_set_clothes_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t result;
	CHECK_BODY_LEN( len, sizeof(result) );
	result = *(uint32_t*)buf;
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, result, sendlen );
	PKG_UINT32( msg, *(uint32_t*)p->session, sendlen );
	PKG_UINT32( msg, *(uint32_t*)( p->session + sizeof(uint32_t) ), sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}

int cutepig_get_ticket_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	int week_day = get_now_tm()->tm_wday;
	uint32_t item_id = 1613119 + ( week_day + 6 ) % 7 ;
	return send_request_to_db( SVR_PROTO_CP_GET_TICKET, p, sizeof(item_id), &item_id, p->id );
}

int cutepig_get_ticket_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t result;
	CHECK_BODY_LEN( len, sizeof(result) );
	result = *(uint32_t*)buf;
	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, result, sendlen );
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self( p, msg, sendlen, 1 );
}

int cutepig_notice_db_sth_done(sprite_t* p, uint32_t event_id)
{
	return send_request_to_db( SVR_PROTO_CP_STH_DONE, NULL, sizeof(event_id), &event_id, p->id );
}

int cutepig_use_card_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	CHECK_BODY_LEN( len, sizeof(cp_use_card_info_t) );
	cp_use_card_info_t* info = (cp_use_card_info_t*)p->session;
	int j = 0;
	UNPKG_UINT32( body, info->card_id, j );
	UNPKG_UINT32( body, info->tar_id, j );
	if( info->card_id != CC_ANGEL &&
		info->card_id != CC_DEMON &&
		info->card_id != CC_CUPID &&
		info->card_id != CC_RECOVER
		)
	{
		return cutepig_use_card_send_result( p, CUCR_INVALID_CARD );
	}
	//不可以给自己使用恶魔卡
	if( info->card_id == CC_DEMON && info->tar_id == p->id )
	{
		return cutepig_use_card_send_result( p, CUCR_CAN_NOT_USE_TO_SELF );
	}

	//get the count of the card
	j = 0;
	uint8_t buff[13];
	pkg_item_kind( p, buff, info->card_id, &j );
	PKG_H_UINT32(buff, info->card_id, j);
	PKG_H_UINT32(buff, info->card_id + 1, j);
	PKG_UINT8(buff, 2, j);
	return send_request_to_db( SVR_PROTO_GET_ITEM, p, j, buff, p->id);
}

int cutepig_get_card_count_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t count;
	uint32_t card_id;
	uint32_t card_cnt;
	int j = 0;
	UNPKG_H_UINT32( buf, count, j );
	if( count != 0 )
	{
		UNPKG_H_UINT32( buf, card_id, j );
		UNPKG_H_UINT32( buf, card_cnt, j );
	}

	//not enough card
	if( 0 == count || card_cnt == 0 )
	{
		return cutepig_use_card_send_result( p, CUCR_NOT_ENOUGH_CARD );
	}
	else
	{
		cp_use_card_info_t* info = (cp_use_card_info_t*)p->session;
		uint32_t db_buf[] = { info->card_id, p->id };
		return send_request_to_db( SVR_PROTO_CP_USE_CARD, p, sizeof(db_buf), db_buf, info->tar_id );
	}
}

int cutepig_use_card_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t result;
	CHECK_BODY_LEN( len, sizeof(result) );
	result = *(uint32_t*)buf;
	return cutepig_use_card_send_result( p, result );
}

int cutepig_use_card_send_result( sprite_t* p, uint32_t result )
{
	CHECK_VALID_ID( p->id );
	//reduce the item number
	if( result == CUCR_OK )
	{
		cp_use_card_info_t* info = (cp_use_card_info_t*)p->session;
		uint8_t buff[128];
		int dblen = 0;
		PKG_H_UINT32( buff, 1, dblen );
		PKG_H_UINT32( buff, 0, dblen );
		PKG_H_UINT32( buff, 0, dblen );
		PKG_H_UINT32( buff, 0, dblen );
		pkg_item_kind( p, buff, info->card_id, &dblen );
		PKG_H_UINT32( buff, info->card_id, dblen );
		PKG_H_UINT32( buff, 1, dblen );
		send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, dblen, buff, p->id );
	}
	int j = sizeof(protocol_t);
	PKG_UINT32( msg, result, j );
	init_proto_head( msg, p->waitcmd, j );
	return send_to_self( p, msg, j, 1 );
}

int cutepig_get_card_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t tar_id;
	CHECK_BODY_LEN( len, sizeof(tar_id) );
	int j = 0;
	UNPKG_UINT32( body, tar_id, j );
	CHECK_VALID_ID(tar_id);
	return send_request_to_db( SVR_PROTO_CP_GET_CARD_INFO, p, 0, NULL, tar_id );
}

int cutepig_get_card_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t card_cnt;
	typedef struct cp_card_info_s
	{
		uint32_t card_id;
		uint32_t time_lasts;
		uint32_t operator_id;
	}__attribute__((packed)) cp_card_info_t;
	uint32_t expectLen = sizeof(card_cnt);
	CHECK_BODY_LEN_GE( len, expectLen );
	uint32_t j = 0;
	UNPKG_H_UINT32( buf, card_cnt, j );
	expectLen += sizeof(cp_card_info_t)*card_cnt;
	CHECK_BODY_LEN( len, expectLen );
	uint32_t send_len = sizeof(protocol_t);
	PKG_UINT32( msg, card_cnt, send_len );
	cp_card_info_t card_info;
	int i;
	for( i = 0; i < card_cnt; ++i )
	{
		UNPKG_H_UINT32( buf, card_info.card_id, j );
		UNPKG_H_UINT32( buf, card_info.time_lasts, j );
		UNPKG_H_UINT32( buf, card_info.operator_id, j );
		PKG_UINT32( msg, card_info.card_id, send_len );
		PKG_UINT32( msg, card_info.time_lasts, send_len );
		PKG_UINT32( msg, card_info.operator_id, send_len );
	}
	init_proto_head( msg, p->waitcmd, send_len );
	return send_to_self( p, msg, send_len, 1 );
}

int cutepig_exchange_pig_with_items_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );

	uint8_t db_buff[1024] = {0};
	uint32_t item_need[] = { 14021, 14024, 14023 };
	int send_len = 0;
	PKG_H_UINT32(db_buff, 2, send_len);
	PKG_H_UINT32(db_buff, 3, send_len);
	int i;
	for( i = 0; i < 3; ++i )
	{
		PKG_H_UINT32( db_buff, item_need[i], send_len );
		pkg_item_kind( p, db_buff, item_need[i], &send_len );
	}
	return send_request_to_db(SVR_PROTO_GET_ITEM_ARRAY, p, send_len, db_buff, p->id);
}

int cutepig_exchange_pig_with_items_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t count;
	CHECK_BODY_LEN_GE( len, sizeof(count) );
	uint32_t item_id;
	uint32_t item_cnt;
	int j = 0;
	UNPKG_H_UINT32( buf, count, j );
	if( count != 3 )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_cutepig_exchange_pig_failed, 1 );
	}
	CHECK_BODY_LEN( len, sizeof(count) + count * (sizeof(item_id) + sizeof(item_cnt)) );
	int i = 0;
	for(i = 0; i < count; i++) {
		UNPKG_H_UINT32( buf, item_id, j);
		UNPKG_H_UINT32( buf, item_cnt, j);
		if( item_cnt < 1 )
		{
			return send_to_self_error( p, p->waitcmd, -ERR_cutepig_exchange_pig_failed, 1 );
		}
	}
	uint32_t exchange_pig_daytype = 31214;
	return db_set_sth_done( p, exchange_pig_daytype, 1, p->id );
}

int cutepig_exchange_pig_sth_done_back( sprite_t* p )
{
	uint32_t exchagne_pig_id = 1593036;
	uint8_t buff[128];
	int dblen = 0;
	PKG_H_UINT32( buff, 0, dblen );
	PKG_H_UINT32( buff, 1, dblen );
	PKG_H_UINT32( buff, 0, dblen );
	PKG_H_UINT32( buff, 0, dblen );
	pkg_item_kind( p, buff, exchagne_pig_id, &dblen );
	PKG_H_UINT32( buff, exchagne_pig_id, dblen );
	PKG_H_UINT32( buff, 1, dblen );
	PKG_H_UINT32( buff, 99999, dblen );
	send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, dblen, buff, p->id );
	response_proto_head( p, p->waitcmd, 0 );
	return 0;
}

int cutepig_set_background_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	uint32_t background_id;
	int j = 0;
	CHECK_BODY_LEN( len, sizeof(background_id) );
	UNPKG_UINT32( body, background_id, j );
	item_kind_t* ik = find_kind_of_item( background_id );
	if( !ik || ik->kind != CUTE_PIG_ITEM_KIND )
	{
		ERROR_RETURN( ("Invalid Item ID:[%d]", background_id), -1 );
	}
	item_t* it = get_item( ik, background_id );
	if( !it || it->u.cp_item_tag.type != CIK_BACKGROUND )
	{
		ERROR_RETURN( ("Invalid Cute Pig background Item ID:[%d]", background_id), -1 );
	}
	return send_request_to_db( SVR_PROTO_CP_SET_BACKGROUND, p, sizeof(background_id), &background_id, p->id );
}

int cutepig_set_background_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t ret;
	CHECK_BODY_LEN( len, sizeof(ret) );
	ret = *(uint32_t*)buf;
	int j = sizeof(protocol_t);
	PKG_UINT32( msg, ret, j );
	init_proto_head( msg, p->waitcmd, j );
	return send_to_self( p, msg, j, 1 );
}

int cutepig_swap_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t is_to_primary;
	uint32_t swap_count;
	uint32_t expect_len = sizeof(is_to_primary) + sizeof(swap_count);
	CHECK_BODY_LEN_GE( len, expect_len );
	int j = 0;
	UNPKG_UINT32( body, is_to_primary, j );
	UNPKG_UINT32( body, swap_count, j );

	if( swap_count >= CP_PIG_LIMIT )
	{
		ERROR_RETURN( ( "Cute Pig Swap Invalid Pig Count:[%u]", swap_count ), -1 );
	}

	if (is_to_primary == 2){
	    uint32_t msgbuff[2]= {p->id, 1};
		msglog(statistic_logfile, 0x0409C33C, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	uint32_t pig_id;
	expect_len += swap_count * sizeof( pig_id );
	CHECK_BODY_LEN( len, expect_len );
	int i;
	uint8_t db_buf[256];
	int db_len = 0;
	PKG_H_UINT32( db_buf, is_to_primary, db_len );
	PKG_H_UINT32( db_buf, swap_count, db_len );
	for( i = 0; i < swap_count; ++i )
	{
		UNPKG_UINT32( body, pig_id, j );
		PKG_H_UINT32( db_buf, pig_id, db_len );
	}
	return send_request_to_db( SVR_PROTO_CP_SWAP_PIG, p, db_len, db_buf, p->id );
}

int cutepig_swap_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t ret;
	CHECK_BODY_LEN( len, sizeof(ret) );
	ret = *(uint32_t*)buf;
	response_proto_uint32( p, p->waitcmd, ret, 0 );
	return 0;
}

int cutepig_get_show_stage_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t tar_id;
	CHECK_BODY_LEN( len, sizeof(tar_id) );
	int j = 0;
	UNPKG_UINT32( body, tar_id, j );
	CHECK_VALID_ID( tar_id );
	//进入美美馆人数统计
	{
		uint32_t msgbuff[2]= {p->id, 1};
		uint32_t statistics_id = 0x0409BF0B;
		msglog(statistic_logfile, statistics_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	return send_request_to_db( SVR_PROTO_CP_GET_SHOW_STAGE_INFO, p, 0, NULL, tar_id );
}

int cutepig_get_show_stage_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t stage_lv;
	uint32_t show_cnt;
	uint32_t item_cnt;
	uint32_t item_id;
	uint32_t item_time;
	uint32_t expect_len = sizeof(stage_lv) + sizeof(show_cnt) + sizeof(item_cnt);
	CHECK_BODY_LEN_GE( len, expect_len );
	int j = 0;
	UNPKG_H_UINT32( buf, stage_lv, j );
	UNPKG_H_UINT32( buf, show_cnt, j );
	UNPKG_H_UINT32( buf, item_cnt, j );
	expect_len += item_cnt * ( sizeof(item_id) + sizeof(item_time) );
	CHECK_BODY_LEN( len, expect_len );
	int send_len = sizeof(protocol_t);
	PKG_UINT32( msg, stage_lv, send_len );
	PKG_UINT32( msg, show_cnt, send_len );
	PKG_UINT32( msg, item_cnt, send_len );
	int i;
	for( i = 0 ; i < item_cnt; ++i )
	{
		UNPKG_H_UINT32( buf, item_id, j );
		UNPKG_H_UINT32( buf, item_time, j );
		PKG_UINT32( msg, item_id, send_len );
		PKG_UINT32( msg, item_time, send_len );
	}
	init_proto_head( msg, p->waitcmd, send_len );
	return send_to_self( p, msg, send_len, 1 );
}

int cutepig_beauty_show_cmd(sprite_t* p, const uint8_t* body, int len)
{
	typedef struct cp_show_cmd_s
	{
		uint32_t tar_id;
		uint32_t my_pig_id[CP_SHOW_MAX_PIG_CNT];
		uint32_t tar_pig_id[CP_SHOW_MAX_PIG_CNT];
		uint32_t item_cnt[CP_SHOW_MAX_ITEM_CNT];
	}__attribute__((packed)) cp_show_cmd_t;

	cp_show_cmd_t cmd_info;
	CHECK_BODY_LEN( len, sizeof(cmd_info) );
	int data_cnt = sizeof(cmd_info)/sizeof(uint32_t);
	uint32_t* data_ptr = (uint32_t*)(&cmd_info);
	int i;
	for( i = 0; i < data_cnt; ++i )
	{
		data_ptr[i] = ntohl( *( (uint32_t*)(body) + i ) );
	}

	int is_valid_tar = cmd_info.tar_id > 0;
	if( 1 == is_valid_tar )
	{
		CHECK_VALID_ID( cmd_info.tar_id );
	}

	//自己或对方的猪 重复
	if(		( cmd_info.my_pig_id[0] == cmd_info.my_pig_id[1] && 0 != cmd_info.my_pig_id[0] ) ||
			( cmd_info.my_pig_id[1] == cmd_info.my_pig_id[2] && 0 != cmd_info.my_pig_id[1] ) ||
			( cmd_info.my_pig_id[2] == cmd_info.my_pig_id[0] && 0 != cmd_info.my_pig_id[2] ) ||
			( is_valid_tar && 
				(	( cmd_info.tar_pig_id[0] == cmd_info.tar_pig_id[1] && 0 != cmd_info.tar_pig_id[0] ) ||
					( cmd_info.tar_pig_id[1] == cmd_info.tar_pig_id[2] && 0 != cmd_info.tar_pig_id[1] ) ||
					( cmd_info.tar_pig_id[2] == cmd_info.tar_pig_id[0] && 0 != cmd_info.tar_pig_id[2] )
				)
			)
		)
	{
		return send_to_self_error( p, p->waitcmd, -ERR_cp_show_invalid_info, 1 );
		//ERROR_RETURN( ("cutepig_beauty_show_cmd invalid pig ID"), -1 );
	}
	
	int pig_cnt = 0;
	//自己的猪的数量和对方不一致
	for( i = 0; i < CP_SHOW_MAX_PIG_CNT; ++i )
	{
		if( is_valid_tar && (cmd_info.my_pig_id[i] > 0) ^ (cmd_info.tar_pig_id[i] > 0) )
		{
			return send_to_self_error( p, p->waitcmd, -ERR_cp_show_invalid_info, 1 );
			//ERROR_RETURN( ("cutepig_beauty_show_cmd invalid pig ID"), -1 );
		}
		if( cmd_info.my_pig_id[i] > 0 )
		{
			++pig_cnt;
		}
	}
	int item_cnt1 = 0;
	int item_cnt2 = 0;
	for( i = 0; i < CP_SHOW_MAX_ITEM_CNT/2; ++i )
	{
		if( cmd_info.item_cnt[i] > 0 )
		{
			++item_cnt1;
		}
		if( cmd_info.item_cnt[i + CP_SHOW_MAX_ITEM_CNT/2] > 0 )
		{
			++item_cnt2;
		}
	}
	//道具数量超过猪的数量
	if( pig_cnt < item_cnt1 || pig_cnt < item_cnt2 || ( !is_valid_tar && ( item_cnt1 > 0 || item_cnt2 > 0 ) ) )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_cp_show_invalid_info, 1 );
		//ERROR_RETURN( ( "cutepig_beauty_show_cmd item cnt out of limit" ), -1 );
	}
	if( 0 == cmd_info.tar_id )
	{
		memset( cmd_info.item_cnt, 0, sizeof(cmd_info.item_cnt) );
	}
	//每种道具使用量<=3
	for( i = 0; i < CP_SHOW_MAX_ITEM_CNT; ++i )
	{
		if( cmd_info.item_cnt[i] > 3 )
		{
			cmd_info.item_cnt[i] = 3;
		}
	}

	cp_show_result_info_t* ret = (cp_show_result_info_t*)p->session;
	ret->result = CSR_OK;
	ret->result_left = 1;
	ret->my_score = 0;
	ret->tar_score = 0;
	ret->pig_cnt = pig_cnt;
	ret->tar_id = cmd_info.tar_id;
	memcpy( ret->item_cnt, cmd_info.item_cnt, sizeof(ret->item_cnt) );
	memcpy( ret->my_pig, cmd_info.my_pig_id, sizeof(ret->my_pig) );
	typedef struct cp_show_info_s
	{
		uint32_t tar_id;
		uint32_t is_user;		//是否为主动pk
		uint32_t pig_id[CP_SHOW_MAX_PIG_CNT];
		uint32_t item_cnt[CP_SHOW_MAX_ITEM_CNT];
	}__attribute__((packed)) cp_show_info_t;
	cp_show_info_t db_info;
	//send to self's DB
	db_info.tar_id = cmd_info.tar_id;
	db_info.is_user = 1;
	memcpy( db_info.pig_id, cmd_info.my_pig_id, sizeof(db_info.pig_id) );
	memcpy( db_info.item_cnt, cmd_info.item_cnt, sizeof(db_info.item_cnt) );
	send_request_to_db( SVR_PROTO_CP_SHOW, p, sizeof(db_info), &db_info, p->id );
	// send to tar's DB
	if( is_valid_tar )
	{
		ret->result_left++;
		db_info.tar_id = p->id;
		db_info.is_user = 0;
		memcpy( db_info.pig_id, cmd_info.tar_pig_id, sizeof(db_info.pig_id) );
		memset( db_info.item_cnt, 0, sizeof(db_info.item_cnt) );
		send_request_to_db( SVR_PROTO_CP_SHOW, p, sizeof(db_info), &db_info, cmd_info.tar_id );
	}
	return 0;
}

int cutepig_beauty_show_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	if( p->waitcmd == PROTO_CP_BEAUTY_SHOW_WITH_NPC )
	{
		return cutepig_beauty_show_with_npc_callback(p, id, buf, len);
	}
	typedef struct cp_show_back_s
	{
		uint32_t ret;
		uint32_t cur_exp;
		uint32_t cur_lv;
		int32_t score;
	}__attribute__((packed)) cp_show_back_t;
	CHECK_BODY_LEN( len, sizeof(cp_show_back_t) );
	cp_show_back_t* back_info = (cp_show_back_t*)buf;
	cp_show_result_info_t* ret = (cp_show_result_info_t*)p->session;
	if( CSR_INVALID_DATA != back_info->ret )
	{
		back_info->score /= 10;
		if( p->id == id )
		{
			ret->my_score = back_info->score;
		}
		else
		{
			ret->tar_score = back_info->score;
			ret->tar_lv = back_info->cur_lv;
			ret->tar_exp = back_info->cur_exp;
			//被挑战方不考虑次数限制
			if( back_info->ret == CSR_LIMIT )
			{
				back_info->ret = CSR_OK;
			}
		}

		if( ret->result != CSR_LIMIT )
		{
			ret->result = back_info->ret;
		}
		ret->result_left--;
		if( 0 == ret->result_left )
		{
			uint32_t add_exp;
			uint32_t add_gold;
			uint32_t added_attr = 0;
			uint32_t is_win = 0;	//斗秀是否获胜
			//美美猪表演统计
			if( 0 == ret->tar_id )
			{
				uint32_t msgbuff[2]= {p->id, 1};
				uint32_t statistics_id = 0x0409BDAB;
				msglog(statistic_logfile, statistics_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
				statistics_id += ret->pig_cnt;
				msglog(statistic_logfile, statistics_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
			}
			//pk统计
			else
			{
				uint32_t msgbuff[2]= {p->id, 1};
				uint32_t statistics_id = 0x0409BEE0;
				msglog(statistic_logfile, statistics_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
				statistics_id += ret->pig_cnt;
				msglog(statistic_logfile, statistics_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
			}
			//pk道具使用统计
			uint32_t item_cnt = 0;
			int i;
			for( i = 0; i < CP_SHOW_MAX_ITEM_CNT; ++i )
			{
				item_cnt += ret->item_cnt[i];
			}
			if( item_cnt > 0 )
			{
				uint32_t msgbuff[2]= {p->id, item_cnt};
				msglog(statistic_logfile, 0x0409BEE4, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
			}

			if( ret->result == CSR_LIMIT )
			{
				add_exp = 0;
				add_gold = 0;
			}
			else 
			{
				//single player
				if( 0 == ret->tar_id )
				{
					add_exp = 5 + ret->pig_cnt * 5;
					switch( ret->pig_cnt )
					{
					case 1:
						add_gold = 500;
						break;
					case 2:
						add_gold = 800;
						break;
					case 3:
						add_gold = 1200;
						break;
					default:
						add_gold = 0;
						break;
					}
					cp_add_exp_gold_info_t add_info;
					add_info.p = p;
					add_info.tar_id = p->id;
					add_info.cur_lv = p->cp_level;
					add_info.cur_exp = p->cp_exp;
					add_info.add_exp = &add_exp;
					add_info.add_gold = add_gold;
					add_info.is_from_db = 0;
					cutepig_add_exp_gold( &add_info );
				}
				//pk
				else
				{
					ret->my_score += ret->item_cnt[SI_ADD_20]*20 + ret->item_cnt[SI_ADD_100]*100 +
						ret->item_cnt[SI_ADD_150]*150 + ret->item_cnt[SI_ADD_50]*50;

					ret->tar_score -= ret->item_cnt[SI_MIN_20]*20 + ret->item_cnt[SI_MIN_100]*100 +
						ret->item_cnt[SI_MIN_150]*150 + ret->item_cnt[SI_MIN_50]*50;
					if( ret->tar_score < 0 )
					{
						ret->tar_score = 0;
					}
					if( ret->my_score > ret->tar_score )
					{
						is_win = 1;

						add_exp = 10 + ret->pig_cnt * 10;
						add_gold = 500 + ret->pig_cnt * 500;

						cp_add_exp_gold_info_t add_info;
						add_info.p = p;
						add_info.tar_id = p->id;
						add_info.cur_lv = p->cp_level;
						add_info.cur_exp = p->cp_exp;
						add_info.add_exp = &add_exp;
						add_info.add_gold = add_gold;
						add_info.is_from_db = 0;
						cutepig_add_exp_gold( &add_info );

						added_attr = 10*10;
						uint8_t db_buf[128];
						int db_len = 0;
						PKG_H_UINT32( db_buf, ret->pig_cnt, db_len );
						int i;
						for( i = 0; i < ret->pig_cnt; ++i )
						{
							PKG_H_UINT32( db_buf, ret->my_pig[i], db_len );
							PKG_H_UINT32( db_buf, added_attr, db_len );
						}
						//add attr for pigs
						send_request_to_db( SVR_PROTO_CP_ADD_ATTR, NULL, db_len, db_buf, p->id );
						added_attr /= 10;
					}
					else
					{
						add_exp = 10 + ret->pig_cnt * 10;
						add_exp /= 2;
						add_gold = 500 + ret->pig_cnt * 500;
						add_gold /= 2;

						cp_add_exp_gold_info_t add_info;
						add_info.p = NULL;
						add_info.tar_id = ret->tar_id;
						add_info.cur_lv = ret->tar_lv;
						add_info.cur_exp = ret->tar_exp;
						add_info.add_exp = &add_exp;
						add_info.add_gold = add_gold;
						cutepig_add_exp_gold( &add_info );
						// send to client
						add_exp = 0;
						add_gold = 0;
					}
					
					send_honor_event_to_db(NULL, CHT_20TIMES_PK, 0, p->id);
					send_honor_event_to_db(NULL, CHT_30TIMES_WIN, is_win, p->id);
				}
			}

			if( add_gold > 0 )
			{
				int sendlen = 0;
				uint8_t buff[1024];
				PKG_H_UINT32( buff, 0, sendlen );
				PKG_H_UINT32( buff, 1, sendlen );
				PKG_H_UINT32( buff, 203, sendlen );
				PKG_H_UINT32( buff, 0, sendlen );
				item_kind_t* ik_gold = find_kind_of_item(CP_GOLD_ITEM_ID);
				item_t* it_gold = get_item( ik_gold, CP_GOLD_ITEM_ID );

				pkg_item_kind( p, buff, CP_GOLD_ITEM_ID, &sendlen );
				PKG_H_UINT32( buff, CP_GOLD_ITEM_ID, sendlen );
				PKG_H_UINT32( buff, add_gold, sendlen );
				PKG_H_UINT32( buff, it_gold->max, sendlen );
				send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, p->id );
			}
			int j = sizeof(protocol_t);
			PKG_UINT32( msg, ret->result, j );
			PKG_UINT32( msg, ret->my_score, j );
			PKG_UINT32( msg, ret->tar_score, j );
			PKG_UINT32( msg, add_exp, j );
			PKG_UINT32( msg, add_gold, j );
			PKG_UINT32( msg, added_attr, j );
			init_proto_head( msg, p->waitcmd, j );
			return send_to_self( p, msg, j, 1 );
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return send_to_self_error( p, p->waitcmd, -ERR_cp_show_invalid_info, 1 );
		//ERROR_RETURN( ( "cutepig_beauty_show_callback Invalid ret:[%u]", back_info->ret ), -1 );
	}
}

int cutepig_set_pig_follow_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t pig_id;
	uint32_t is_follow;
	CHECK_BODY_LEN( len, sizeof(pig_id) + sizeof(is_follow) );

	int j = 0;
	UNPKG_UINT32( body, pig_id, j );
	UNPKG_UINT32( body, is_follow, j );

	if( 0 == is_follow )
	{
		pig_id = p->animal_nbr;
	}
	return set_sth_follow( p, pig_id, is_follow, FAT_PIG );
}

int cutepig_set_pig_follow_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	enum CP_FOLLOW_RET
	{
		CPFR_BACK		= 0,
		CPFR_FOLLOW,
		CPFR_ERROR,
	};

	typedef struct cp_pig_follow_back_s
	{
		uint32_t ret;
		uint32_t pig_id;
		uint32_t item_id;
		uint32_t state;
		uint32_t transform;
		uint32_t transform_time;
		uint32_t dress1;
		uint32_t dress2;
	}__attribute__((packed)) cp_pig_follow_back_t;

	CHECK_BODY_LEN( len, sizeof(cp_pig_follow_back_t) );
	cp_pig_follow_back_t* back_info = (cp_pig_follow_back_t*)buf;

	int send_len = sizeof (protocol_t);

	PKG_UINT32 ( msg, back_info->pig_id, send_len );
	PKG_UINT32 ( msg, back_info->item_id, send_len );
	PKG_UINT32 ( msg, back_info->state, send_len );
	PKG_UINT32 ( msg, back_info->transform, send_len );
	PKG_UINT32 ( msg, back_info->transform_time, send_len );
	PKG_UINT32 ( msg, back_info->dress1, send_len );
	PKG_UINT32 ( msg, back_info->dress2, send_len );
	if( CPFR_FOLLOW == back_info->ret )
	{
		p->animal_nbr = back_info->pig_id;
		memset( &p->animal_follow_info, 0, sizeof(p->animal_follow_info) );
		p->animal_follow_info.tag_cp.pig_id = back_info->pig_id;
		p->animal_follow_info.tag_cp.item_id = back_info->item_id;
		p->animal_follow_info.tag_cp.state = back_info->state;
		p->animal_follow_info.tag_cp.transform = back_info->transform;
		p->animal_follow_info.tag_cp.transform_time = back_info->transform_time;
		p->animal_follow_info.tag_cp.dress1 = back_info->dress1;
		p->animal_follow_info.tag_cp.dress2 = back_info->dress2;
	}
	else if( CPFR_BACK == back_info->ret )
	{
		p->animal_nbr = 0;
		memset( &p->animal_follow_info, 0, sizeof(p->animal_follow_info) );
	}
	else
	{
		return send_to_self_error( p, p->waitcmd, -ERR_cp_follow_invalid_pig, 1 );
		//ERROR_RETURN( ( "cutepig_set_pig_follow_callback type err:[%u %u]", p->id, back_info->ret ), -1 );
	}
	init_proto_head ( msg, p->waitcmd, send_len );
	send_to_map( p, msg, send_len, 1 );
	return 0;
}

int cutepig_get_2nd_game_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id);
	uint32_t tar_id;
	CHECK_BODY_LEN( len, sizeof(tar_id) );
	int j = 0;
	UNPKG_UINT32( body, tar_id, j );
	CHECK_VALID_ID(tar_id);
	return send_request_to_db( SVR_PROTO_CP_GET_2ND__GAME_INFO, p, 0, NULL, tar_id );
}

int cutepig_get_secondary_game_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t pig_cnt;
	int expectLen = sizeof(pig_cnt);
	CHECK_BODY_LEN_GE( len, expectLen );
	int j = 0;
	UNPKG_H_UINT32( buf, pig_cnt, j );
	uint32_t pig_id;
	uint32_t item_id;
	expectLen += pig_cnt * ( sizeof(pig_id) + sizeof(item_id) );
	CHECK_BODY_LEN( len, expectLen );

	int sendlen = sizeof(protocol_t);
	PKG_UINT32( msg, pig_cnt, sendlen );
	int i;
	for( i = 0; i < pig_cnt; ++i )
	{
		UNPKG_H_UINT32( buf, pig_id, j );
		UNPKG_H_UINT32( buf, item_id, j );
		PKG_UINT32( msg, pig_id, sendlen );
		PKG_UINT32( msg, item_id, sendlen );
	}
	init_proto_head( msg, p->waitcmd, sendlen );
	return send_to_self(p, msg, sendlen, 1);
}

static uint32_t get_beauty_game_day_idx(uint32_t* day_idx)
{
	*day_idx = 0;//无效时段
#ifndef TW_VER
	if( get_today() >= 20111209 && get_today() <= 20111211 )
	{
		*day_idx = get_today() - 20111209 + 1;
#else
	if( get_today() >= 201112016 && get_today() <= 20111218 )
	{
		*day_idx = get_today() - 20111216 + 1;
#endif
	} 
	return *day_idx;
}

/*
 * @brief 美美斗秀赛获得奖励  8286
 */
int cutepig_get_beauty_game_prize_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id);

	if( p->cp_win_beauty_show == 0 )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_cp_show_invalid_info, 1 );
	}

	uint32_t day_type = 50009;		//美美斗秀赛
	uint32_t day_cnt = 30;			//每日限制30次

	*(uint32_t*)p->session = p->cp_win_beauty_show % 10 == 1 ? 1 : 0;
	*(uint32_t*)( p->session + sizeof(uint32_t) ) = p->cp_win_beauty_show / 10;
	p->cp_win_beauty_show = 0;
	return db_set_sth_done( p, day_type, day_cnt, p->id );
}

static int get_beauty_prize( sprite_t* p )
{
	const uint32_t item_id = 1351173;
	uint32_t item_cnt;
	uint32_t is_win = *(uint32_t*)p->session;
	uint32_t npc_id = *(uint32_t*)( p->session + sizeof(uint32_t) );
	int db_data;
	if( is_win == 0 )
	{
		db_data = -1;
		item_cnt = 5;
	}
	else
	{
		item_cnt = bs_npc_pk_award[npc_id].ticket_cnt;
		db_data = 1;
	}
	//notice db the result
	send_request_to_db( SVR_PROTO_CP_SHOW_SET_GAME_STATE, NULL, sizeof(db_data), &db_data, p->id );

	int sendlen = 0;
	uint8_t buff[1024];
	PKG_H_UINT32( buff, 0, sendlen );
	PKG_H_UINT32( buff, 2, sendlen );
	PKG_H_UINT32( buff, 203, sendlen );
	PKG_H_UINT32( buff, 0, sendlen );
	item_kind_t* ik = find_kind_of_item(item_id);
	item_t* it = get_item( ik, item_id );
	pkg_item_kind( p, buff, item_id, &sendlen );
	PKG_H_UINT32( buff, item_id, sendlen );
	PKG_H_UINT32( buff, item_cnt, sendlen );
	PKG_H_UINT32( buff, it->max, sendlen );
	uint32_t award_item_id = 0;
	uint32_t award_item_cnt = 0;
	if( 1 == is_win )
	{
		int i;
		int rate = rand()%100;
		int cur_rate = 0;
		for( i = 0; i < bs_npc_pk_award[npc_id].award_cnt; ++i )
		{
			cur_rate += bs_npc_pk_award[npc_id].award_info[i].rate;
			if( rate < cur_rate )
			{
				award_item_id = bs_npc_pk_award[npc_id].award_info[i].item_id;
				award_item_cnt = bs_npc_pk_award[npc_id].award_info[i].item_cnt;
				break;
			}
		}
	}
	ik = find_kind_of_item( award_item_id );
	it = get_item( ik, award_item_id );
	pkg_item_kind( p, buff, award_item_id, &sendlen );
	PKG_H_UINT32( buff, award_item_id, sendlen );
	PKG_H_UINT32( buff, award_item_cnt, sendlen );
	PKG_H_UINT32( buff, it->max, sendlen );
	send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, p->id );

	int l = sizeof(protocol_t);
	PKG_UINT32( msg, item_id, l );
	PKG_UINT32( msg, item_cnt, l );
	PKG_UINT32( msg, award_item_id, l );
	PKG_UINT32( msg, award_item_cnt, l );
	init_proto_head( msg, p->waitcmd, l );
	return send_to_self( p, msg, l, 1 );
}

int cutepig_check_beauty_game_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
// 	uint32_t day_idx = *(uint32_t*)(p->session);
// 	uint32_t day_act = *(uint32_t*)(p->session + 4);
// 	if( day_idx != 0 && day_act == 1 )
// 	{
// 		return send_request_to_db( SVR_SYSARG_JOIN_BEAUTY_CONTEND, p, sizeof(day_idx), &day_idx, p->id );
// 	}
	return get_beauty_prize( p );
}

int cutepig_get_beauty_game_prize_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN( len, sizeof(uint32_t) );	
	uint32_t all_cnt = *(uint32_t*)buf;
	
	uint32_t day_act = *(uint32_t*)(p->session + 4);

	if( day_act == 1 )
	{
		uint32_t msg_type = 0;
		if( (all_cnt % 100000) == 0 )
		{
			msg_type = 3;
		}
		else if( (all_cnt % 10000) == 0 )
		{
			msg_type = 2;
		}
		if( msg_type == 2 || msg_type == 3 )
		{
			DEBUG_LOG("tell user get 10000 times prize: uid[%u]", p->id);
			tell_cp_get_beauty_prize_across_svr( p, &msg_type, sizeof(msg_type) );
		}

		if( (all_cnt % 1000) == 0 )
		{
			msg_type = 1;
			const uint32_t itemid = 1351156;
			const uint32_t item_cnt = 25;
			uint32_t db_buf[] = {0, 1, 0, 0, 99, itemid, item_cnt, 99999};
			send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf), db_buf, p->id);

			DEBUG_LOG("get 1000 times prize: uid[%u] cnt[%u]", p->id, itemid);
			int l = sizeof( protocol_t );
			PKG_UINT32( msg, msg_type, l );
			PKG_UINT32( msg, all_cnt, l );
			init_proto_head( msg, PROTO_CP_TELL_BEAUTY_LIMIT_GIFT, l );
			send_to_self( p, msg, l, 0 );
		}
	}

	return get_beauty_prize( p );
}

/*
 * @brief 美美斗秀赛查询昨天、今天及总的斗秀次数  8287
 */
int cutepig_get_beauty_game_record_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	CHECK_BODY_LEN( len, 0 );

	return send_request_to_db(SVR_SYSARG_GET_BEAUTY_GAME_RECORD, p, 0, NULL, p->id);
}

int cutepig_get_beauty_game_record_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t value_1;
		uint32_t value_2;
		uint32_t value_3;
		uint32_t total;
	}__attribute__((packed)) game_record_cbk_pack_t;

	CHECK_BODY_LEN(len, sizeof(game_record_cbk_pack_t));
	game_record_cbk_pack_t* cbk_pack = (game_record_cbk_pack_t*)buf;
	uint32_t day_idx = 0;
	get_beauty_game_day_idx( &day_idx );
	uint32_t value_1 = 0;
	uint32_t value_2 = 0;
	if( day_idx == 1 )
	{
		value_1 = 0;
		value_2 = cbk_pack->value_1;
	}
	else if( day_idx == 2 )
	{
		value_1 = cbk_pack->value_1;
		value_2 = cbk_pack->value_2;
	}
	else if( day_idx == 3)
	{
		value_1 = cbk_pack->value_2;
		value_2 = cbk_pack->value_3;
	}

	int l = sizeof(protocol_t);
	PKG_UINT32( msg, value_1, l );
	PKG_UINT32( msg, value_2, l );
	PKG_UINT32( msg, cbk_pack->total, l );
	init_proto_head( msg, p->waitcmd, l );
	return send_to_self( p, msg, l, 1 );
}

/*
 * @brief 美美斗秀赛 主动通知 定时特定次数礼包  8288
 */
int cutepig_get_beauty_game_limit_gift(uint32_t gift_type)
{
	DEBUG_LOG("cutepig_get_beauty_game_limit_gift: cmd[%d] gift_type[%u]", PROTO_CP_TELL_BEAUTY_LIMIT_GIFT, gift_type);
	int l = sizeof( protocol_t ); 
	PKG_UINT32( msg, gift_type, l );
	PKG_UINT32( msg, 0, l );
	init_proto_head( msg, PROTO_CP_TELL_BEAUTY_LIMIT_GIFT, l );
	send_to_map3( 47, msg, l );
	return 0;
}

/*
 * @brief 美美斗秀赛 领取奖励	 8289
 */
int cutepig_get_beauty_game_gift_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN( len, 0 );
	CHECK_VALID_ID( p->id );
	return send_request_to_db(SVR_SYSARG_GET_BEAUTY_GAME_GIFT, p, 0, NULL, p->id);
}


int cutepig_check_beauty_game_time_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t gift_cnt;
		uint32_t time_right;
	}__attribute__((packed)) check_game_gift_cbk_pack_t;
	CHECK_BODY_LEN( len, sizeof(check_game_gift_cbk_pack_t) );
	check_game_gift_cbk_pack_t* cbk_pack = (check_game_gift_cbk_pack_t*)buf;
	if( cbk_pack->time_right > 70 )
	{
		return send_to_self_error(p, p->waitcmd, -ERR_cp_beauty_game_time_err, 1);
	}
	*(uint32_t*)p->session = cbk_pack->gift_cnt;
	const uint32_t day_type = 50011;	//美美斗秀赛获得奖励数量限制
	*(uint32_t*)(p->session + 4) = day_type;
	return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &day_type, p->id);
}

int cutepig_get_beauty_game_gift_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t day_cnt = *(uint32_t*)buf;
	uint32_t gift_cnt = *(uint32_t*)(p->session);
	uint32_t day_type = *(uint32_t*)(p->session + 4);
	if( day_cnt == gift_cnt )
	{
		return send_to_self_error(p, p->waitcmd, -ERR_cp_has_get_beauty_gift, 1);
	}

	uint32_t gift_type = (gift_cnt % 10 == 0) ? 1 : 0;
	const uint32_t beauty_game_gift[] = {1351156, 1613250, 1613253, 1613270, 1613273, 1613290, 1613291, 1613251, 1613272};
	uint32_t itemid = 0;
	uint32_t item_cnt = 1;
	if( gift_type == 0 )
	{
		uint32_t gift_idx = rand() % (sizeof(beauty_game_gift) / sizeof(uint32_t) - 2);
		itemid = beauty_game_gift[gift_idx];
		if( gift_idx == 0 )
		{
			item_cnt = 10;
		}
	}
	else
	{
		uint32_t gift_idx = rand() % (sizeof(beauty_game_gift) / sizeof(uint32_t));
		itemid = beauty_game_gift[gift_idx];
		if( gift_idx == 0 )
		{
			item_cnt = 50;
		}
	}
	const item_t* itm = get_item_prop(itemid);
	uint32_t flag ;
	int temp_len = 0;
	pkg_item_kind(p, (uint8_t*)(&flag), itemid, &temp_len);
	uint32_t db_buf_1[] = {0, 1, 0, 0, flag, itemid, item_cnt, itm->max};
	send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf_1), db_buf_1, p->id);

	//设置已经领取过
	uint32_t db_buf_2[2] = {day_type, gift_cnt};
	send_request_to_db(SVR_PROTO_SET_DAY_COUNT, NULL, sizeof(db_buf_2), db_buf_2, p->id);
	
	int l = sizeof( protocol_t );
	PKG_UINT32(msg, itemid, l);
	PKG_UINT32(msg, item_cnt, l);
	init_proto_head( msg, p->waitcmd, l );
	return send_to_self( p, msg, l, 1 );
}

/*
 * @brief 美美斗秀赛 与npc对战  8290
 */
int cutepig_beauty_show_with_npc_cmd(sprite_t* p, const uint8_t* body, int len)
{
	typedef struct cp_show_cmd_s
	{
		uint32_t show_type;	//0表示用自己的猪 1表示用npc的猪
		uint32_t npc_id;
		uint32_t my_pig_id[CP_SHOW_MAX_PIG_CNT];
		uint32_t item_cnt[CP_SHOW_MAX_ITEM_CNT];
	}__attribute__((packed)) cp_show_cmd_t;

	cp_show_cmd_t cmd_info;
	CHECK_BODY_LEN( len, sizeof(cmd_info) );
	int data_cnt = sizeof(cmd_info)/sizeof(uint32_t);
	uint32_t* data_ptr = (uint32_t*)(&cmd_info);
	int i;
	for( i = 0; i < data_cnt; ++i )
	{
		data_ptr[i] = ntohl( *( (uint32_t*)(body) + i ) );
	}

	cp_show_with_npc_info_t* show_info = (cp_show_with_npc_info_t*)p->session;

	//DEBUG_LOG( "cutepig_beauty_show_with_npc_cmd userid[%u] %u %u %u", p->id, cmd_info.tar_id, cmd_info.my_pig_id, cmd_info.tar_pig_id);

	show_info->npc_id = cmd_info.npc_id;
	//非法NPC编号
	if( cmd_info.npc_id >= CP_SHOW_NPC_CNT )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_cp_show_invalid_info, 1 );
	}

	show_info->is_npcs_pig = 1;
	if( cmd_info.show_type == 0 )
	{
		show_info->is_npcs_pig = 0;
		//自己的猪 重复
		if(	( cmd_info.my_pig_id[0] == cmd_info.my_pig_id[1] && 0 != cmd_info.my_pig_id[0] ) ||
			( cmd_info.my_pig_id[1] == cmd_info.my_pig_id[2] && 0 != cmd_info.my_pig_id[1] ) ||
			( cmd_info.my_pig_id[2] == cmd_info.my_pig_id[0] && 0 != cmd_info.my_pig_id[2] ) )
		{
			return send_to_self_error( p, p->waitcmd, -ERR_cp_show_invalid_info, 1 );
			//ERROR_RETURN( ("cutepig_beauty_show_cmd invalid pig ID"), -1 );
		}
		memcpy( show_info->my_pig, cmd_info.my_pig_id, sizeof(show_info->my_pig) );
		show_info->pig_cnt = 0;
		for( i = 0; i < CP_SHOW_MAX_PIG_CNT; ++i )
		{
			if( cmd_info.my_pig_id[i] > 0 )
			{
				show_info->pig_cnt++;
			}
		}
		//猪的数量与指定的npc的猪的数量不符
		if( show_info->pig_cnt != bs_npc_pk_award[show_info->npc_id].pig_cnt )
		{
			return send_to_self_error( p, p->waitcmd, -ERR_cp_show_invalid_info, 1 );
		}
	}
	else
	{
		show_info->pig_cnt = bs_npc_pk_award[show_info->npc_id].pig_cnt;
	}

	int item_cnt1 = 0;
	int item_cnt2 = 0;
	for( i = 0; i < CP_SHOW_MAX_ITEM_CNT/2; ++i )
	{
		if( cmd_info.item_cnt[i] > 0 )
		{
			++item_cnt1;
		}
		if( cmd_info.item_cnt[i + CP_SHOW_MAX_ITEM_CNT/2] > 0 )
		{
			++item_cnt2;
		}
	}
	//道具数量超过猪的数量
	if( show_info->pig_cnt < item_cnt1 || show_info->pig_cnt < item_cnt2 )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_cp_show_invalid_info, 1 );
		//ERROR_RETURN( ( "cutepig_beauty_show_cmd item cnt out of limit" ), -1 );
	}

	//每种道具使用量<=3
	for( i = 0; i < CP_SHOW_MAX_ITEM_CNT; ++i )
	{
		if( cmd_info.item_cnt[i] > 3 )
		{
			cmd_info.item_cnt[i] = 3;
		}
	}

	p->cp_win_beauty_show = 0;

	memcpy( show_info->item_cnt, cmd_info.item_cnt, sizeof(cmd_info.item_cnt));

	//用npc的猪来对战
	if( cmd_info.show_type == 1 )
	{
		const uint32_t day_type = 50012;	//每天使用NPC猪的次数限制 3次
		return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &day_type, p->id);
	}

	typedef struct cp_show_info_s
	{
		uint32_t tar_id;
		uint32_t is_user;		//是否为主动pk
		uint32_t pig_id[CP_SHOW_MAX_PIG_CNT];
		uint32_t item_cnt[CP_SHOW_MAX_ITEM_CNT];
	}__attribute__((packed)) cp_show_info_t;
	cp_show_info_t db_info = {0};
	//send to self's DB
	db_info.tar_id = cmd_info.npc_id;
	db_info.is_user = 2;
	memcpy( db_info.pig_id, cmd_info.my_pig_id, sizeof(db_info.pig_id) );
	memcpy( db_info.item_cnt, cmd_info.item_cnt, sizeof(db_info.item_cnt) );
	return send_request_to_db( SVR_PROTO_CP_SHOW, p, sizeof(db_info), &db_info, p->id );
}

int y_show_score(sprite_t* p, int32_t score)
{
	const uint32_t npc_score[6] = {200, 100, 300, 50, 150, 250};
	cp_show_with_npc_info_t* show_info = (cp_show_with_npc_info_t*)p->session;

	int32_t my_score = 0;
	int32_t tar_score = 0;
	//用NPC的猪
	if( 1 == show_info->is_npcs_pig )
	{
		int i;
		for( i = 0; i < show_info->pig_cnt; ++i )
		{
			my_score += npc_score[rand()%6] + rand()%100;
		}
		//统计项
		{
			uint32_t msgbuff[4]= {0};
			if( ISVIP(p->flag) )
			{
				msgbuff[0] = p->id;
				msgbuff[1] = 1;
			}
			else
			{
				msgbuff[2] = p->id;
				msgbuff[3] = 1;
			}
			msglog(statistic_logfile, 0x0409BF0E, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
	}
	else
	{
		my_score = score;
	}
	//参与指定NPC斗秀赛的数量统计
	{
		uint32_t msgbuff[6] = { 0, 0, p->id, 1, 0, 0};
		uint32_t* msg_ptr = msgbuff;
		if( ISVIP(p->flag) )
		{
			msg_ptr += 2;
		}
		uint32_t msg_id = 0x0409BF16 + show_info->npc_id;
		msglog(statistic_logfile, msg_id, get_now_tv()->tv_sec, msg_ptr, sizeof(uint32_t) * 4);
	}

	int i;
	for( i = 0; i < show_info->pig_cnt; ++i )
	{
		tar_score += bs_npc_pig_infos[show_info->npc_id][0] + 
			rand() % ( bs_npc_pig_infos[show_info->npc_id][1] - bs_npc_pig_infos[show_info->npc_id][0] );
	}

	my_score += show_info->item_cnt[SI_ADD_20]*20 + show_info->item_cnt[SI_ADD_100]*100 +
		show_info->item_cnt[SI_ADD_150]*150 + show_info->item_cnt[SI_ADD_50]*50;
	tar_score -= show_info->item_cnt[SI_MIN_20]*20 + show_info->item_cnt[SI_MIN_100]*100 +
		show_info->item_cnt[SI_MIN_150]*150 + show_info->item_cnt[SI_MIN_50]*50;
	if( tar_score < 0 )
	{
		tar_score = 0;
	}
	if( my_score == tar_score )
	{
		my_score += 10;
	}
	//十位数表示NpcID
	//个位表示胜负 1胜2负
	p->cp_win_beauty_show = show_info->npc_id * 10;
	p->cp_win_beauty_show += my_score > tar_score ? 1 : 2;
	DEBUG_LOG("y_show_score: uid[%u] score[%u] npcscore[%u] flag[%d]", p->id, my_score, tar_score, p->cp_win_beauty_show);

	//统计项
	{
		uint32_t cnt = 0;
		int i;
		for( i = 0; i < CP_SHOW_MAX_ITEM_CNT; ++i )
		{
			cnt += show_info->item_cnt[i];
		}
		if( cnt > 0 )
		{
			uint32_t msgbuff[2]= {p->id, cnt};
			msglog(statistic_logfile, 0x0409BEE4, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
	}

	int j = sizeof(protocol_t);
	PKG_UINT32( msg, (my_score > tar_score) ? 1 : 0, j );
	PKG_UINT32( msg, my_score, j );
	PKG_UINT32( msg, tar_score, j );
	init_proto_head( msg, p->waitcmd, j );
	return send_to_self( p, msg, j, 1 );
}

int cutepig_beauty_show_with_npc_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct cp_show_back_s
	{
		uint32_t ret;
		uint32_t cur_exp;
		uint32_t cur_lv;
		int32_t score;
	}__attribute__((packed)) cp_show_back_t;
	CHECK_BODY_LEN( len, sizeof(cp_show_back_t) );
	cp_show_back_t* back_info = (cp_show_back_t*)buf;
//	cp_show_result_info_t* ret = (cp_show_result_info_t*)p->session;
	
	if( CSR_INVALID_DATA == back_info->ret )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_cp_show_invalid_info, 1 );
	}
	
	DEBUG_LOG("cutepig_beauty_show_with_npc_callback: uid[%u] score[%u]", p->id, back_info->score);
	
	return y_show_score( p, back_info->score / 10 );
}

int cp_beauty_show_check_use_npc_pig_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t day_cnt = *(uint32_t*)buf;
	if( day_cnt >= 3 )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_cp_use_npc_pig_limit, 1 );
	}

	const uint32_t day_type = 50012;
	db_set_sth_done(NULL, day_type, 3, p->id);
	return y_show_score( p, 0 );
}

int cutepig_get_bs_npc_pk_achieve_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	return send_request_to_db( SVR_PROTO_CP_SHOW_GET_ACHIEVE_INFO, p, 0, NULL, p->id );
}

int cutepig_get_bs_npc_pk_achieve_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct cp_show_achieve_info_s
	{
		uint32_t state;			//成就领取状态
		int32_t result_cnt;		//胜负次数，正数为连胜次数，负数为连败次数
	}__attribute__((packed)) cp_show_achieve_info_t;
	CHECK_BODY_LEN( len, sizeof(cp_show_achieve_info_t) );
	cp_show_achieve_info_t* back_info = (cp_show_achieve_info_t*)buf;
	int send_len = sizeof(protocol_t);
	PKG_UINT32( msg, back_info->state, send_len );
	PKG_UINT32( msg, back_info->result_cnt, send_len );
	init_proto_head( msg, p->waitcmd, send_len );
	return send_to_self( p, msg, send_len, 1 );
}

int cutepig_get_bs_npc_pk_achieve_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	int32_t achieve_id;
	CHECK_BODY_LEN( len, sizeof(achieve_id) );
	int j = 0;
	UNPKG_UINT32( body, achieve_id, j );
	if( achieve_id > CSA_INVALID && achieve_id < CSA_MAX )
	{
		*(uint32_t*)p->session = achieve_id;
		return send_request_to_db( SVR_PROTO_CP_SHOW_GET_ACHIEVE, p, sizeof(achieve_id), &achieve_id, p->id );
	}
	else
	{
		ERROR_RETURN( ("cutepig_get_achieve_cmd Invalid achieve ID:[%u]", achieve_id ), -1 );
	}
}

int cutepig_get_bs_npc_pk_achieve_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t ret;
	CHECK_BODY_LEN( len, sizeof(ret) );
	ret = *(uint32_t*)buf;
	int j = sizeof(protocol_t);
	PKG_UINT32( msg, ret, j );
	uint32_t item_id = 0;
	uint32_t item_cnt = 0;
	if( ret ==  CSGAR_OK )
	{
		int32_t achieve_id = *(uint32_t*)p->session;
		uint32_t broadcast = 0;
		switch( achieve_id )
		{
		case CSA_WIN:
		case CSA_LOSE:
			item_id = CP_GOLD_ITEM_ID;
			item_cnt = 500;
			p->yxb += item_cnt;
			break;
		case CSA_COMBO_WIN_10:
			item_id = 14062;
			item_cnt = 1;
			broadcast = 1;
			break;
		case CSA_COMBO_WIN_15:
			item_id = 14061;
			item_cnt = 1;
			broadcast = 1;
			break;
		case CSA_COMBO_LOSE_5:
			item_id = 14063;
			item_cnt = 1;
			broadcast = 1;
			break;
		default:
			break;
		}
		//统计项
		{
			uint32_t msgbuff[2]= {p->id, 1};
			uint32_t msg_id = 0x0409BF1B;
			msglog(statistic_logfile, msg_id + achieve_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
		

		//本服公告
		if( 1 == broadcast )
		{
			uint32_t msg_type = 1;
			uint32_t msg_len = 0;
			uint8_t n_msg[1024] = {0};
			uint8_t broadcast_msg[512] = {0};
#ifndef TW_VER
			uint8_t achieve_msg[3][32] = {
				"无与伦比的猪猪",
				"漂亮的猪猪",
				"不气馁的猪猪"
			};
			sprintf( (char*)broadcast_msg, "%s达成了【%s】称号，真令人羡慕！", p->nick, achieve_msg[achieve_id] );
#else
			uint8_t achieve_msg[3][32] = {
				"無與倫比的豬豬",
				"漂亮的豬豬",
				"不氣餒的豬豬"
			};
			sprintf( (char*)broadcast_msg, "%s達成了【%s】稱號，真令人羡慕！", p->nick, achieve_msg[achieve_id] );
#endif
			msg_len = strlen((char*)broadcast_msg);
			int l = sizeof(protocol_t);
			PKG_UINT32(n_msg, msg_type, l);
			PKG_UINT32(n_msg, msg_len, l);
			PKG_STR(n_msg, (uint8_t*)broadcast_msg, l, msg_len);
			init_proto_head(n_msg, PROTO_TELL_FLASH_SOME_MSG, l);
			send_to_all_players(p, n_msg, l, 0);
		}

		int sendlen = 0;
		uint8_t buff[1024];
		PKG_H_UINT32( buff, 0, sendlen );
		PKG_H_UINT32( buff, 1, sendlen );
		PKG_H_UINT32( buff, 203, sendlen );
		PKG_H_UINT32( buff, 0, sendlen );
		item_kind_t* ik = find_kind_of_item(item_id);
		item_t* it = get_item( ik, item_id );
		pkg_item_kind( p, buff, item_id, &sendlen );
		PKG_H_UINT32( buff, item_id, sendlen );
		PKG_H_UINT32( buff, item_cnt, sendlen );
		PKG_H_UINT32( buff, it->max, sendlen );
		send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, p->id );
	}
	PKG_UINT32( msg, item_id, j );
	PKG_UINT32( msg, item_cnt, j );
	init_proto_head( msg, p->waitcmd, j );
	return send_to_self( p, msg, j, 1 );
}

int cutepig_load_bs_npc_pk_award_info( char* file )
{
	xmlDocPtr doc;
	xmlNodePtr root;
	int err = -1;

	doc = xmlParseFile (file);
	if (!doc)
	{
		ERROR_RETURN (("load cutepig bs npc pk award info failed"), -1);
	}

	root = xmlDocGetRootElement(doc);
	if (!root)
	{
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}
	if( root )
	{
		xmlNodePtr chl_element_group = root->children;
		uint32_t npc_id = 0;
		while( chl_element_group )
		{
			if( 0 == xmlStrcmp( chl_element_group->name, (const xmlChar *)"BsNpcInfo" ) )
			{
				DECODE_XML_PROP_UINT32( bs_npc_pk_award[npc_id].pig_cnt, chl_element_group, "PigCnt");
				if( bs_npc_pk_award[npc_id].pig_cnt > 3 || bs_npc_pk_award[npc_id].pig_cnt < 1 )
				{
					DEBUG_LOG( "Parse CutePig BS Npc PK Award Info Invalid PigCnt:[%u]", bs_npc_pk_award[npc_id].pig_cnt);
					bs_npc_pk_award[npc_id].pig_cnt = 1;
				}
				DECODE_XML_PROP_UINT32( bs_npc_pk_award[npc_id].ticket_cnt, chl_element_group, "TicketCnt");
				uint32_t* award_cnt = &bs_npc_pk_award[npc_id].award_cnt;
				*award_cnt = 0;
				bs_award_info_t* award = bs_npc_pk_award[npc_id].award_info;
				xmlNodePtr child = chl_element_group->children;
				while( child )
				{
					if( 0 == xmlStrcmp( child->name, (const xmlChar *)"Award" ) )
					{
						DECODE_XML_PROP_UINT32( award[*award_cnt].rate, child, "Rate");
						DECODE_XML_PROP_UINT32( award[*award_cnt].item_id, child, "ItemID");
						DECODE_XML_PROP_UINT32( award[*award_cnt].item_cnt, child, "ItemCnt");
						++(*award_cnt);
					}
					child = child->next;
				}
				++npc_id;
			}
			chl_element_group = chl_element_group->next;
		}
	}
	err = 0;
	DEBUG_LOG( "CutePig Load BS Npc PK Award Info OK" );
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load CutePig BS Npc PK Award Info %s", file);
}

int cutepig_npc_weight_pk_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t pig_id;
	CHECK_BODY_LEN( len, sizeof(pig_id) );
	int j = 0;
	UNPKG_UINT32( body, pig_id, j );
	uint32_t day_idx = 0;
	get_beauty_game_day_idx(&day_idx);
	int is_invalid = 1;
	switch( day_idx )
	{
	case 1:
		if( get_now_tm()->tm_hour == 20 && get_now_tm()->tm_min >= 0 && get_now_tm()->tm_min <= 30 )
		{
			is_invalid = 0;
		}
		break;
	case 2:
	case 3:
		if( get_now_tm()->tm_hour == 14 && get_now_tm()->tm_min >= 0 && get_now_tm()->tm_min <= 30 )
		{
			is_invalid = 0;
		}
		break;
	default:
		break;
	}
	
//	if( 1 == is_invalid )
//	{
//		return send_to_self_error( p, p->waitcmd, -ERR_cp_npc_weight_pk_invalid_time, 1 );
//	}
	//称重赛参与统计
	{
		uint32_t msgbuff[6] = { 0, 0, p->id, 1, 0, 0};
		uint32_t* msg_ptr = msgbuff;
		if( ISVIP(p->flag) )
		{
			msg_ptr += 2;
		}
		msglog(statistic_logfile, 0x0409BF20, get_now_tv()->tv_sec, msg_ptr, sizeof(uint32_t) * 4);
	}

	*(uint32_t*)p->session = pig_id;

	uint32_t day_type = 50013;
	uint32_t day_cnt = 20;
	return db_set_sth_done( p, day_type, day_cnt, p->id );
}

int cutepig_npc_weight_pk_callback(sprite_t* p, uint32_t pig_weight)
{
	uint32_t npc_pig_id = rand()%6;
	uint32_t npc_weight = huashen_pig_infos[npc_pig_id][0] + rand() %( huashen_pig_infos[npc_pig_id][1] - huashen_pig_infos[npc_pig_id][0] );
	uint32_t is_win = pig_weight > npc_weight ? 1 : 0;
	if( 1 == is_win )
	{
		//增加一个中间物品用于兑换成就奖励
		uint32_t item_id = 1351174;
		uint32_t item_cnt = 1;
		int sendlen = 0;
		uint8_t buff[1024];
		PKG_H_UINT32( buff, 0, sendlen );
		PKG_H_UINT32( buff, 1, sendlen );
		PKG_H_UINT32( buff, 203, sendlen );
		PKG_H_UINT32( buff, 0, sendlen );
		item_kind_t* ik = find_kind_of_item(item_id);
		item_t* it = get_item( ik, item_id );
		pkg_item_kind( p, buff, item_id, &sendlen );
		PKG_H_UINT32( buff, item_id, sendlen );
		PKG_H_UINT32( buff, item_cnt, sendlen );
		PKG_H_UINT32( buff, it->max, sendlen );
		send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, p->id );
	}
	int j = sizeof(protocol_t);
	PKG_UINT32( msg, is_win, j );
	PKG_UINT32( msg, npc_pig_id, j );
	PKG_UINT32( msg, pig_weight, j );
	PKG_UINT32( msg, npc_weight, j );
	init_proto_head( msg, p->waitcmd, j );
	return send_to_self( p, msg, j, 1 );
}

int cutepig_npc_weight_pk_check_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t pig_id = *(uint32_t*)p->session;
	return send_request_to_db( SVR_PROTO_CP_GET_PIG_INFO, p, sizeof(pig_id), &pig_id, p->id );
}

int cutepig_get_npc_weight_pk_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t idx_begin = 180;
	uint32_t idx_end = 182;
	uint32_t result = 0;
	{
		int n = (177 - 1) / 32;
		int b = (177 - 1) % 32;
		if (p->only_one_flag[n] & (1 << b))
		{
			result |= ( 1 << (0) );
		}
	}
	int i;
	for( i = idx_begin; i <= idx_end; ++i )
	{
		int n = (i - 1) / 32;
		int b = (i - 1) % 32;
		if (p->only_one_flag[n] & (1 << b))
		{
			result |= ( 1 << (i - idx_begin + 1) );
		}
	}
	response_proto_uint32( p, p->waitcmd, result, 0 );
	return 0;
}

int cutepig_load_ie_info( char* file )
{
	xmlDocPtr doc;
	xmlNodePtr root;
	int err = -1;

	doc = xmlParseFile (file);
	if (!doc)
	{
		ERROR_RETURN (("load cutepig ie info failed"), -1);
	}

	root = xmlDocGetRootElement(doc);
	if (!root)
	{
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}
	if( root )
	{
		xmlNodePtr chl_element_group = root->children;
		uint32_t* exchange_id = &illustrate_exchange_info.exchange_num;
		*exchange_id = 0;
		while( chl_element_group )
		{
			if( 0 == xmlStrcmp( chl_element_group->name, (const xmlChar *)"IEInfo" ) )
			{
				DECODE_XML_PROP_UINT32( illustrate_exchange_info.exchange_info[*exchange_id].out_pig_id, chl_element_group, "PigID");
				DECODE_XML_PROP_UINT32( illustrate_exchange_info.exchange_info[*exchange_id].flag_idx, chl_element_group, "FlagIdx");
				uint32_t* in_cnt = &illustrate_exchange_info.exchange_info[*exchange_id].in_cnt;
				*in_cnt = 0;
				xmlNodePtr child = chl_element_group->children;
				while( child )
				{
					if( 0 == xmlStrcmp( child->name, (const xmlChar *)"BreedNeed" ) )
					{
						DECODE_XML_PROP_UINT32( illustrate_exchange_info.exchange_info[*exchange_id].breed_need[*in_cnt], child, "BreedType");
						++(*in_cnt);
					}
					child = child->next;
				}
				++exchange_id;
			}
			chl_element_group = chl_element_group->next;
		}
	}
	err = 0;
	DEBUG_LOG( "CutePig Load IE Info OK" );
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load CutePig IE Info %s", file);
}

int cutepig_get_pig_by_illustrate_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t exchange_id;
	CHECK_BODY_LEN( len, sizeof(exchange_id) );
	int j = 0;
	UNPKG_UINT32( body, exchange_id, j );
	if( exchange_id > illustrate_exchange_info.exchange_num ||										//无效的兑换ID
		1 == check_only_one_bit( p, illustrate_exchange_info.exchange_info[exchange_id].flag_idx )	//已经兑换过了
		)
	{
		return send_to_self_error( p, p->waitcmd, -ERR_cp_ie_invalid_exchange_id, 1 );
	}

	*(uint32_t*)p->session = exchange_id;
	uint8_t db_buf[128];
	j = 0;
	PKG_H_UINT32( db_buf, illustrate_exchange_info.exchange_info[exchange_id].in_cnt, j );
	int i;
	for( i = 0; i < illustrate_exchange_info.exchange_info[exchange_id].in_cnt; ++i )
	{
		PKG_H_UINT32( db_buf, illustrate_exchange_info.exchange_info[exchange_id].breed_need[i], j );
	}
	return send_request_to_db( SVR_PROTO_CP_ILLUSTRATE_CHECK, p, j, db_buf, p->id );
}

int cutepig_get_pig_by_illustrate_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t ret;
	CHECK_BODY_LEN( len, sizeof(ret) );
	ret = *(uint32_t*)buf;
	uint32_t award_pig_id = 0;
	if( 1 == ret )
	{
		uint32_t exchange_id = *(uint32_t*)p->session;
		if( 0 == set_only_one_bit( p, illustrate_exchange_info.exchange_info[exchange_id].flag_idx ) )
		{
			return send_to_self_error( p, p->waitcmd, -ERR_cp_ie_invalid_exchange_id, 1 );
			//ERROR_RETURN( ("cutepig_get_pig_by_illustrate_callback FlagIdx:[%u],has been set already", illustrate_exchange_info.exchange_info[exchange_id].flag_idx ), -1 );
		}

		award_pig_id = illustrate_exchange_info.exchange_info[exchange_id].out_pig_id;

		int sendlen = 0;
		uint8_t buff[256];
		PKG_H_UINT32( buff, 0, sendlen );
		PKG_H_UINT32( buff, 1, sendlen );
		PKG_H_UINT32( buff, 203, sendlen );
		PKG_H_UINT32( buff, 0, sendlen );

		item_kind_t* ik = find_kind_of_item( award_pig_id );
		item_t* it = get_item( ik, award_pig_id );
		pkg_item_kind( p, buff, award_pig_id, &sendlen );
		PKG_H_UINT32( buff, award_pig_id, sendlen );
		PKG_H_UINT32( buff, 1, sendlen );
		PKG_H_UINT32( buff, it->max, sendlen );
		send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, p->id );
	}
	response_proto_uint32_uint32( p, p->waitcmd, ret, award_pig_id, 0 );
	return 0;
}

/*
 * @brief 肥肥馆荣誉 拉取所有荣誉
 */
int cutepig_get_all_honor_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t uid = 0;
	int i = 0;
	UNPKG_UINT32(body, uid, i);
	CHECK_VALID_ID(uid);

	DEBUG_LOG("cutepig_get_all_honor_cmd:uid[%u] check_id[%u]", p->id, uid);
	return send_request_to_db(SVR_PROTO_CP_GET_ALL_HONOR, p, 0, NULL, uid);
}

int cutepig_get_all_honor_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t count;
	}__attribute__((packed)) get_all_honor_cbk_head_t;
	typedef struct {
		uint32_t honor_id;
		uint32_t finish;
		uint32_t honor_cnt;
		uint32_t honor_max;
	}__attribute__((packed)) get_all_honor_cbk_pack_t;
	CHECK_BODY_LEN_GE(len, sizeof(get_all_honor_cbk_head_t));
	get_all_honor_cbk_head_t* cbk_head = (get_all_honor_cbk_head_t*)buf;
	CHECK_BODY_LEN(len, sizeof(get_all_honor_cbk_head_t) + cbk_head->count * sizeof(get_all_honor_cbk_pack_t));
	get_all_honor_cbk_pack_t* p_cbk_pack = (get_all_honor_cbk_pack_t*)(buf + sizeof(get_all_honor_cbk_head_t));

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_head->count, l);
	int i = 0;
	for (i = 0; i < cbk_head->count; i++) {
		PKG_UINT32(msg, (p_cbk_pack + i)->honor_id, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->honor_cnt, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->honor_max, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 肥肥馆荣誉 设置挑战荣誉
 */
int send_honor_event_to_db(sprite_t* p, uint32_t honor_id, uint32_t expend, uint32_t id)
{
	uint32_t db_buf[] = {honor_id, expend};
	return send_request_to_db(SVR_PROTO_CP_SET_HONOR, p, sizeof(db_buf), db_buf, id);
}

int exchange_user_piglet_house_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	int j = 0;
	uint32_t index_m = 0;
	uint32_t index_p = 0;
	UNPKG_UINT32(body,index_m,j);
	UNPKG_UINT32(body,index_p,j);

	uint32_t db_buff[2] = {index_m, index_p};
	return  send_request_to_db(SVR_PROTO_USER_EXCHANGE_PIGLET_HOUSE, p, 8, db_buff, p->id);
}

int exchange_user_piglet_house_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t state = 0;
	CHECK_BODY_LEN(len, sizeof(state));
	unpkg_host_uint32((uint8_t *)buf, &state);
	response_proto_uint32(p, p->waitcmd, state, 0);
	return 0;
}
int cutepiglet_guider_get_work_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_VALID_ID( p->id );
	CHECK_BODY_LEN( bodylen, 0);
	return  send_request_to_db(SVR_PROTO_USER_GET_PIGLET_GUIDER_STEP, p, 0, NULL, p->id);
}
int cute_piglet_guider_get_work_callback(sprite_t*p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	int msg_len = sizeof(protocol_t);
	uint32_t state = *(uint32_t*)buf;
	PKG_UINT32(msg, state, msg_len);
	init_proto_head(msg, p->waitcmd, msg_len);
	return send_to_self(p, msg, msg_len, 1);
}
int cutepiglet_guider_set_work_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	uint32_t step = 0;
	CHECK_BODY_LEN(bodylen, sizeof(step));
	int j = 0;
	UNPKG_UINT32(body,step,j);
	if(step < 1 || step > 7){
		return send_to_self_error( p, p->waitcmd, -ERR_cutepig_guide_step_invalid_pig, 1 );
	}
	 send_request_to_db(SVR_PROTO_USER_SET_PIGLET_GUIDER_STEP, NULL, 4, &step, p->id);
	 int msg_len = sizeof(protocol_t);
	 init_proto_head(msg, p->waitcmd, msg_len);
	 return send_to_self(p, msg, msg_len, 1);
}
int load_cutepig_explor_mine_conf(const char* file)
{
    pig_explor_mine_cnt = 0;
	int i, err = -1;
	xmlDocPtr doc;
	xmlNodePtr cur;
	doc = xmlParseFile(file);
	if (!doc) {
		ERROR_RETURN(("load cutepig explor mine config failed"), -1);
	}

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	i = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"MapMine"))){
		    DECODE_XML_PROP_INT(i, cur, "MapID");
            if (i > CP_EXPLOR_MINE_MAX || i <= 0)
            {
                ERROR_RETURN(("error count=%d, id=%d ", i, pig_explor_mine_info[i].id), -1);
            }
			pig_explor_mine_info[i - 1].id = i;
			DECODE_XML_PROP_INT_DEFAULT(pig_explor_mine_info[i-1].lvl_limit, cur, "LvlLimit", 0);
			DECODE_XML_PROP_INT_DEFAULT(pig_explor_mine_info[i-1].nead_time, cur, "NeedTime", 0);
			DECODE_XML_PROP_INT_DEFAULT(pig_explor_mine_info[i-1].gemid, cur, "GemID", 0);
			decode_xml_prop_arr_int_default ((int32_t *)pig_explor_mine_info[i-1].special_pig, 2, cur, "SpecialPig", 0);
			DECODE_XML_PROP_INT_DEFAULT(pig_explor_mine_info[i-1].rate, cur, "Rate", 0);
			DECODE_XML_PROP_INT_DEFAULT(pig_explor_mine_info[i-1].special_rate, cur, "SpecialRate", 0);
			DECODE_XML_PROP_INT_DEFAULT(pig_explor_mine_info[i-1].energy, cur, "Energy", 0);
			i++;
		}

		cur = cur->next;
	}

    pig_explor_mine_cnt = i;
    
	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load cutepig explor mine %s", file);
}

static int parse_single_rand_item(map_item_rand_t* iut, int* cnt, xmlNodePtr cur)
{
	int id, j = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Item"))) {
			if (j == MAX_EXCHANGE_ITEMS) {
				ERROR_RETURN(("too many items"), -1);
			}

			DECODE_XML_PROP_INT (id, cur, "ID");
			if (!(iut[j].itm = get_item_prop(id))) {
				ERROR_RETURN(("can't find item=%d", id), -1);
			}

			DECODE_XML_PROP_INT (iut[j].count, cur, "Count");
			if (iut[j].count < 0) {
				ERROR_RETURN(("error count=%d, item=%d", iut[j].count, id), -1);
			}

			decode_xml_prop_float_default(&iut[j].rand_start, cur, "RandStart", 0.0);
			if (iut[j].rand_start > 100.00) {
				ERROR_RETURN(("error rand_start=%f, item=%d", iut[j].rand_start, id), -1);
			}

			decode_xml_prop_float_default(&iut[j].rand_end, cur, "RandEnd", 0.0);
			if (iut[j].rand_end > 100.00 || iut[j].rand_end < iut[j].rand_start) {
				ERROR_RETURN(("error rand_end=%f, item=%d", iut[j].rand_end, id), -1);
			}
			j++;
		}
		cur = cur->next;
	}
	*cnt = j;

	return 0;
}

int load_map_mine_rand_conf(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;

	doc = xmlParseFile (file);
	if (!doc) {
		ERROR_RETURN (("load map mine rand item config failed"), -1);
	}

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}
	
	int i, err = -1;
	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"MapEntry"))) {
			DECODE_XML_PROP_INT(i, cur, "ID");
			if (i > MAX_MAP_MINE_RANDS_ID || i <= 0) {
				ERROR_LOG ("parse %s failed, id=%d", file, i);
				goto exit;
			}
			map_mine_rands[i - 1].mapid = i;
			chl = cur->xmlChildrenNode;
			if ( (parse_single_rand_item(map_mine_rands[i-1].map_items, 
			    &(map_mine_rands[i-1].item_cnt), chl) != 0))
				goto exit;
		}
		cur = cur->next;
	}
	err = 0;

exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load map mine rand item file %s", file);
}


static uint32_t rand_map_single_item(int mapid)
{
	float rand_num = (rand() % 10000) / 100.00;
	uint32_t i = 0;
	for (i = 0; i < map_mine_rands[mapid - 1].item_cnt; i++) {
		if (rand_num >= map_mine_rands[mapid - 1].map_items[i].rand_start
			&& rand_num < map_mine_rands[mapid - 1].map_items[i].rand_end) {
			break;
		}
	}
	return i;
}


int cutepig_get_piglet_machine_work_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
    uint32_t user = 0;
    CHECK_BODY_LEN(len, sizeof(user));

    int j = 0;
    UNPKG_UINT32(body, user, j);
	return send_request_to_db( SVR_PROTO_USER_GET_PIGLET_MACHINE_WORK, p, 4, &(p->id), user );
}

int cutepig_get_piglet_machine_work_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct
	{
	    uint32_t state;
		uint32_t machine_lvl;
		uint32_t warhouse_lvl;
		uint32_t current_exp;
		uint32_t current_lvl;
		uint32_t work_energy_piglets;
		uint32_t pig_cnt;
		uint32_t machine_cnt;
	}__attribute__((packed)) machine_work_header_t;

	typedef struct
	{
		uint32_t piglet_index;
		uint32_t pigletid;
		uint32_t breed;
		uint32_t sex;
		uint32_t dress[2];
		uint32_t energy;
		uint32_t max_energy;
	}__attribute__((packed)) machine_work_pig_t;

	typedef struct
	{
		uint32_t tool_type;
		uint32_t tool_index;
		uint32_t tool_lvl;
		uint32_t work_state;
		uint32_t left_time;
		uint32_t total_time;
		uint32_t glue_id;
	}__attribute__((packed)) machine_work_tool_t;

	uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C33B,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	
	CHECK_BODY_LEN_GE(len, sizeof(machine_work_header_t));
	machine_work_header_t* p_header = (machine_work_header_t*)buf;
	CHECK_BODY_LEN(len, sizeof(machine_work_header_t) + p_header->pig_cnt * sizeof(machine_work_pig_t)
	    + p_header->machine_cnt * sizeof(machine_work_tool_t));
	int l = sizeof(protocol_t);
	DEBUG_LOG("[machine_level:%u warehouse_level:%u current_exp:%u]", p_header->machine_lvl,  p_header->warhouse_lvl,  p_header->current_exp);
    PKG_UINT32(msg, p_header->state, l);
	PKG_UINT32(msg, p_header->machine_lvl, l);
	PKG_UINT32(msg, p_header->warhouse_lvl, l);
	PKG_UINT32(msg, p_header->current_exp, l);

	uint32_t next_level = p_header->current_lvl + 1;
	if( next_level > CP_CUR_MAX_LEVEL )
	{
		next_level = CP_CUR_MAX_LEVEL;
	}
	DEBUG_LOG("[next_level: %u  next_exp: %u]", next_level, level_info[next_level - 1].exp);
	PKG_UINT32( msg, level_info[next_level - 1].exp, l );
	PKG_UINT32( msg, p_header->current_lvl, l );
	PKG_UINT32(msg, p_header->work_energy_piglets, l);

	PKG_UINT32(msg, p_header->pig_cnt, l);
	PKG_UINT32(msg, p_header->machine_cnt, l);

    machine_work_pig_t *p_pig_info = (machine_work_pig_t*)(buf + sizeof(machine_work_header_t));
	DEBUG_LOG("[index: %u  id: %u  breed: %u  sex: %u  dress_1: %u  dress_2: %u  energy: %u]", p_pig_info->piglet_index, p_pig_info->pigletid,p_pig_info->breed,  p_pig_info->sex, p_pig_info->dress[0], p_pig_info->dress[1], p_pig_info->energy);
	int i = 0;
	for (i = 0; i < p_header->pig_cnt; i++) {
	    PKG_UINT32(msg, p_pig_info->piglet_index, l);
	    PKG_UINT32(msg, p_pig_info->pigletid, l);
	    PKG_UINT32(msg, p_pig_info->breed, l);
	    PKG_UINT32(msg, p_pig_info->sex, l);
	    PKG_UINT32(msg, p_pig_info->dress[0], l);
	    PKG_UINT32(msg, p_pig_info->dress[1], l);
	    PKG_UINT32(msg, p_pig_info->energy, l);
	    PKG_UINT32(msg, p_pig_info->max_energy, l);
	    p_pig_info++;
	}

	machine_work_tool_t *p_tool_info = (machine_work_tool_t*)(buf + sizeof(machine_work_header_t)
	    + p_header->pig_cnt * sizeof(machine_work_pig_t));
	int j = 0;
	for (j = 0; j < p_header->machine_cnt; j++) {

	    PKG_UINT32(msg, p_tool_info->tool_type, l);
	    PKG_UINT32(msg, p_tool_info->tool_index, l);
	    PKG_UINT32(msg, p_tool_info->tool_lvl, l);
	    PKG_UINT32(msg, p_tool_info->work_state, l);
	    PKG_UINT32(msg, p_tool_info->left_time, l);
	    PKG_UINT32(msg, p_tool_info->total_time, l);
		 PKG_UINT32(msg, p_tool_info->glue_id, l);
	    p_tool_info++;
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int cutepig_user_pig_explor_mine_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	int j = 0;
	uint32_t pigid = 0;
	uint32_t mapid = 0;
	UNPKG_UINT32(body, pigid, j);
	UNPKG_UINT32(body, mapid, j);

	if (mapid >= pig_explor_mine_cnt || mapid < 1){
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

    uint32_t nead_time = pig_explor_mine_info[mapid-1].nead_time;
    uint32_t energy = pig_explor_mine_info[mapid-1].energy;
    uint32_t db_buff[4] = {pigid, mapid, nead_time, energy};
    
	return  send_request_to_db(SVR_PROTO_USER_PIG_EXPLOR_MAP_MINE, p, sizeof(db_buff), db_buff, p->id);
}

int cutepig_user_pig_explor_mine_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C33D,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	
	uint32_t state = 0;
	CHECK_BODY_LEN(len, sizeof(state));
	unpkg_host_uint32((uint8_t *)buf, &state);
	response_proto_uint32(p, p->waitcmd, state, 0);
	return 0;
}

int cutepig_get_explor_mine_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	return send_request_to_db(SVR_PROTO_GET_USER_EXPLOR_MINE_INFO, p, 0, NULL, p->id);
}

int cutepig_get_explor_mine_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t count;
	}__attribute__((packed)) explor_mine_info_head_t;
	typedef struct {
		uint32_t mapid;
		uint32_t pigid;
		uint32_t pig_type;
		uint32_t end_time;
	}__attribute__((packed)) explor_mine_info_t;
	CHECK_BODY_LEN_GE(len, sizeof(explor_mine_info_head_t));
	explor_mine_info_head_t* p_header = (explor_mine_info_head_t*)buf;
	CHECK_BODY_LEN(len, sizeof(explor_mine_info_head_t) + p_header->count * sizeof(explor_mine_info_t));
	explor_mine_info_t* p_mine_info = (explor_mine_info_t*)(buf + sizeof(explor_mine_info_head_t));

	int l = sizeof(protocol_t);
    int l_tmp = l;
	PKG_UINT32(msg, p_header->count, l);

	int n = 0;
	int i = 0;
	for (i = 0; i < p_header->count; i++) {
		uint32_t cur_time = get_now_tv()->tv_sec;
		if ((cur_time + 30) > (p_mine_info + i)->end_time){
		    n = n + 1;
		    uint32_t mapid = (p_mine_info + i)->mapid;
		    uint32_t index = rand_map_single_item(mapid);
		    uint32_t itemid = map_mine_rands[mapid - 1].map_items[index].itm->id;
		    uint32_t count = map_mine_rands[mapid - 1].map_items[index].count;
		    uint32_t pigid = (p_mine_info + i)->pigid;
		    uint32_t pig_type = (p_mine_info + i)->pig_type;
		    db_exchange_single_item_op(p, 202, itemid, count, 0);
		    uint32_t db_buff[3] = {1, mapid, pigid};
		    send_request_to_db(SVR_PROTO_USER_PIGLET_FINISH_EXPLOR_MINE, NULL, sizeof(db_buff), db_buff, p->id);
		    uint32_t rand_cnt = rand()%100;
		    uint32_t rand_rate = pig_explor_mine_info[mapid-1].rate;
		    if(pig_type == pig_explor_mine_info[mapid-1].special_pig[0] ||
		        pig_type == pig_explor_mine_info[mapid-1].special_pig[1])
		   {
		        rand_rate = pig_explor_mine_info[mapid-1].special_rate;
		    }
		    if (rand_cnt < rand_rate){
		        db_exchange_single_item_op(p, 202, pig_explor_mine_info[mapid-1].gemid, 1, 0);
		    }
		}
		else
		{
		    PKG_UINT32(msg, (p_mine_info + i)->mapid, l);
		    PKG_UINT32(msg, (p_mine_info + i)->pigid, l);
		    PKG_UINT32(msg, (p_mine_info + i)->end_time, l);
		}
		
	}

    PKG_UINT32(msg, (p_header->count - n), l_tmp);
    
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int cutepig_user_piglet_melt_ore_cmd(sprite_t * p,const uint8_t * body,int len)
{
    uint32_t stove_index = 0;
    uint32_t ore_id = 0, batch_melt = 0;
    uint32_t piglet_count = 0;
    uint32_t piglet_index = 0;
    int i = 0;
    CHECK_BODY_LEN_GE(len, 16);
	UNPKG_UINT32(body, stove_index, i);
	UNPKG_UINT32(body, ore_id, i);
	UNPKG_UINT32(body, batch_melt, i);
	UNPKG_UINT32(body, piglet_count, i);
	DEBUG_LOG("piglet_count: %u", piglet_count);
	CHECK_BODY_LEN(len, 16+piglet_count*4);
	if ((piglet_count > 3) || (!piglet_count))
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	uint8_t dbbuf[1024] = {};
	int l = 0;
	PKG_H_UINT32(dbbuf,stove_index,l);
	PKG_H_UINT32(dbbuf,ore_id,l);
	PKG_H_UINT32(dbbuf,batch_melt,l);
	PKG_H_UINT32(dbbuf,piglet_count,l);

	int j = 0;
	for (j = 0; j < piglet_count; j++)
	{
	    UNPKG_UINT32(body, piglet_index, i);
	    PKG_H_UINT32(dbbuf, piglet_index, l);
	}

    return send_request_to_db(SVR_PROTO_CP_USER_PIGLET_MELT_ORE, p, l, dbbuf, p->id);
}


int cutepig_user_piglet_melt_ore_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t state;
		uint32_t tool_type;
		uint32_t tool_index;
		uint32_t tool_level;
		uint32_t work_state;
		uint32_t left_time;
		uint32_t total_time;
	}__attribute__((packed)) machine_st_t;

	uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C33E,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	CHECK_BODY_LEN(len, sizeof(machine_st_t));

	machine_st_t *header = (machine_st_t*)buf;
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, header->state, l);
	PKG_UINT32(msg, header->tool_type, l);
	PKG_UINT32(msg, header->tool_index, l);
	PKG_UINT32(msg, header->tool_level, l);
	PKG_UINT32(msg, header->work_state, l);
	PKG_UINT32(msg, header->left_time, l);
	PKG_UINT32(msg, header->total_time, l);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}

int cutepig_user_produce_machine_part_cmd(sprite_t * p,const uint8_t * body,int len)
{
    uint32_t m_tool_index = 0;
    uint32_t obj_part = 0, obj_count = 0;
    uint32_t piglet_count = 0;
    uint32_t piglet_index = 0;
    int i = 0;
    CHECK_BODY_LEN_GE(len, 16);
	UNPKG_UINT32(body, m_tool_index, i);
	UNPKG_UINT32(body, obj_part, i);
	UNPKG_UINT32(body, obj_count, i);
	UNPKG_UINT32(body, piglet_count, i);	
	CHECK_BODY_LEN(len, 16+piglet_count*4);
	if ((piglet_count > 3) || (!piglet_count))
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	uint8_t dbbuf[1024] = {};
	int l = 0;
	PKG_H_UINT32(dbbuf,m_tool_index,l);
	PKG_H_UINT32(dbbuf,obj_part,l);
	PKG_H_UINT32(dbbuf,obj_count,l);
	PKG_H_UINT32(dbbuf,piglet_count,l);

	int j = 0;
	for (j = 0; j < piglet_count; j++)
	{
	    UNPKG_UINT32(body, piglet_index, i);
	    PKG_H_UINT32(dbbuf, piglet_index, l);
	}

    return send_request_to_db(SVR_PROTO_CP_USER_PIGLET_PRODUCE_MACHINE_PART, p, l, dbbuf, p->id);
}

int cutepig_user_produce_machine_part_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	

	typedef struct{
        uint32_t state;
        uint32_t tool_type;
        uint32_t tool_index;
        uint32_t tool_level;
        uint32_t work_state;
        uint32_t left_time;
        uint32_t total_time;
    }__attribute__((packed)) machine_st_t;

    CHECK_BODY_LEN(len, sizeof(machine_st_t));

    uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C33F,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

    machine_st_t *header = (machine_st_t*)buf;
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, header->state, l);
    PKG_UINT32(msg, header->tool_type, l);
    PKG_UINT32(msg, header->tool_index, l);
    PKG_UINT32(msg, header->tool_level, l);
    PKG_UINT32(msg, header->work_state, l);
    PKG_UINT32(msg, header->left_time, l);
    PKG_UINT32(msg, header->total_time, l);

    init_proto_head(msg, p->waitcmd, l);
    return send_to_self(p, msg, l, 1);
}

int cutepig_user_pig_finish_work_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	int j = 0;
	uint32_t tool_type = 0;
	uint32_t tool_index = 0;
	UNPKG_UINT32(body, tool_type, j);
	UNPKG_UINT32(body, tool_index, j);
	
    uint32_t db_buff[2] = {tool_type, tool_index};
    
	return  send_request_to_db(SVR_PROTO_USER_PIGLET_FINISH_WORK, p, sizeof(db_buff), db_buff, p->id);
}

int cutepig_user_pig_finish_work_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t state = 0;
	uint32_t itemid = 0;
	uint32_t count = 0;
	CHECK_BODY_LEN(len, sizeof(state)+sizeof(itemid)+sizeof(count));
	unpkg_host_uint32_uint32_uint32((uint8_t *)buf, &state, &itemid, &count);
	response_proto_uint32_uint32_uint32(p, p->waitcmd, state, itemid, count, 0);
	return 0;
}

int cutepig_get_user_piglet_work_machine_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	int j = 0;
	uint32_t tool_type = 0;
	uint32_t tool_index = 0;
	UNPKG_UINT32(body, tool_type, j);
	UNPKG_UINT32(body, tool_index, j);
	
    uint32_t db_buff[2] = {tool_type, tool_index};
    
	return  send_request_to_db(SVR_PROTO_USER_GET_PIGLET_WORK_MACHINE, p, sizeof(db_buff), db_buff, p->id);
}

int cutepig_get_user_piglet_work_machine_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
	    uint32_t tool_type;
        uint32_t tool_index;
        uint32_t tool_level;
        uint32_t end_time;
		uint32_t pig_count;
	}__attribute__((packed)) work_machine_info_head_t;
	typedef struct {
		uint32_t pigid;
	}__attribute__((packed)) piglet_info_t;
	CHECK_BODY_LEN_GE(len, sizeof(work_machine_info_head_t));
	work_machine_info_head_t* p_header = (work_machine_info_head_t*)buf;
	CHECK_BODY_LEN(len, sizeof(work_machine_info_head_t) + p_header->pig_count * sizeof(piglet_info_t));
	piglet_info_t* p_info = (piglet_info_t*)(buf + sizeof(work_machine_info_head_t));

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, p_header->pig_count, l);
	int i = 0;
	for (i = 0; i < p_header->pig_count; i++) {
		PKG_UINT32(msg, (p_info + i)->pigid, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int cutepiglet_use_accelerate_machine_tool_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	typedef struct acc_machine_tool{
		uint32_t uid;
		uint32_t toolid;
		uint32_t type;
		uint32_t index;
	}acc_mh_t;
	CHECK_BODY_LEN(bodylen, sizeof(acc_mh_t));
	
	acc_mh_t* head = (acc_mh_t*)p->session;
	int offset = 0;
	UNPKG_UINT32(body, head->uid, offset);
	UNPKG_UINT32(body, head->toolid, offset);
	UNPKG_UINT32(body, head->type, offset);
	UNPKG_UINT32(body, head->index, offset);
	
	uint8_t db_buf[16];
	int off = 0;
	PKG_H_UINT32( db_buf, 0, off);
	PKG_H_UINT32( db_buf, 1, off);
	PKG_H_UINT32( db_buf, head->toolid, off);
	pkg_item_kind(p,db_buf, head->toolid, &off);
	return send_request_to_db(SVR_PROTO_GET_ITEM_ARRAY, p, sizeof(db_buf), db_buf, p->id);

}

int cutepiglet_use_accelerate_machine_tool_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t flag = 0;
	flag = *(uint32_t*)buf;
	if(flag == 0){
		 return send_to_self_error(p, p->waitcmd, -ERR_use_acc_machine_tool, 1);
	}
	typedef struct acc_machine_tool{
		uint32_t uid;
		uint32_t toolid;
		uint32_t type;
		uint32_t index;
	}acc_mh_t;

	acc_mh_t* head = (acc_mh_t*)p->session;
	db_delete_single_item_op(p, 202, head->toolid, 1, 0);
	if(head->toolid == 1614011 || head->toolid == 1614009 || head->toolid == 1614010){
			return cute_piglet_get_use_special_acc_tool_cnt_callback(p, p->id, buf,  len);
	}
	else{
		int offset = sizeof(protocol_t);
		PKG_UINT32(msg, 0, offset);
		init_proto_head(msg, p->waitcmd, offset);
		return send_to_self(p, msg, offset, 1);
	}
}
int cute_piglet_get_use_special_acc_tool_cnt_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct acc_machine_tool{
		uint32_t uid;
		uint32_t toolid;
		uint32_t type;
		uint32_t index;
	}acc_mh_t;
	uint32_t cnt = *(uint32_t*)(p->session + sizeof(acc_mh_t));
	int i = sizeof(protocol_t);
	if(cnt < 4){
		db_add_single_item_op(p->id, 0, 1614004, 1);
		uint32_t day_buf[] = {50019, 99, 1};
		send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, sizeof(day_buf), day_buf, p->id);
		PKG_UINT32(msg, 1, i);
		PKG_UINT32(msg, 1614004, i);
	}
	else if(cnt == 4){
		uint32_t day_buf[] = {50019, 99, 1};
		send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, sizeof(day_buf), day_buf, p->id);
		db_add_single_item_op(p->id, 0, 1614004, 1);
		db_add_single_item_op(p->id, 0, 1614005, 1);
		PKG_UINT32(msg, 2, i);
		PKG_UINT32(msg, 1614004, i);
		PKG_UINT32(msg, 1614005, i);
	}
	else if(cnt > 4 && cnt <= 19){
		uint32_t day_buf[] = {50019, 99, 1};
		send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, sizeof(day_buf), day_buf, p->id);
		PKG_UINT32(msg, 0, i);
	}
	else{
		return send_to_self_error(p, p->waitcmd, -ERR_use_glue_to_machine_day_limit, 1);
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int cutepig_user_produce_special_machine_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	int j = 0;
	uint32_t type = 0;
	uint32_t tool_id = 0;
	UNPKG_UINT32(body, type, j);
	UNPKG_UINT32(body, tool_id, j);
	
    uint32_t db_buff[2] = {type, tool_id};
    
	return  send_request_to_db(SVR_PROTO_USER_CP_PRODUCE_SPECIAL_MACHINE, p, sizeof(db_buff), db_buff, p->id);
}

int cutepig_user_produce_special_machine_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}
int cutepig_user_give_random_machine_something_cmd(sprite_t* p, uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	CHECK_VALID_ID( p->id );

	int user = 0, offset = 0;
	UNPKG_UINT32(body, user, offset);

	return send_request_to_db(SVR_PROTO_USER_CP_RANDOM_MACHINE_PRODUCT, p, 0, NULL, user);
}

int cutepig_use_give_random_machine_something_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = 0, offset = 0;
	UNPKG_H_UINT32(buf, count, offset);

	typedef struct items{
		uint32_t matchine_type;
		uint32_t matchine_index;
		uint32_t itemid;
		uint32_t count;
	}items_t;
	CHECK_BODY_LEN(len, sizeof(items_t)*count + 4);

	items_t *head = (items_t*)(buf+4);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, count, i);
	uint32_t k = 0;
	for(; k < count; ++k){
		PKG_UINT32(msg, head->matchine_type, i);
		PKG_UINT32(msg, head->matchine_index, i);
		PKG_UINT32(msg, head->itemid, i);
		PKG_UINT32(msg, head->count, i);
	}
	if(k > 0){
		uint32_t msg_buff[2] = {1, p->id};
		msglog(statistic_logfile, 0x0409C341,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int cutepiglet_take_back_mining_pig_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, sizeof(uint32_t));
	CHECK_VALID_ID(p->id);

	int mapid = 0, j = 0;
	UNPKG_UINT32(body, mapid, j);

	send_request_to_db(SVR_PROTO_USER_CP_TAKE_BACK_MINING, NULL, sizeof(uint32_t), &mapid, p->id);

	int i = sizeof(protocol_t);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}
