#include <libtaomee/dataformatter/bin_str.h>

#include "proto.h"
#include "exclu_things.h"
#include "message.h"
#include "communicator.h"
#include "small_require.h"
#include "central_online.h"
#include "mole_homeland.h"
#include "mole_dungeon.h"

#define DUNGEON_HP_INC_VAL				10
#define DUNGEON_HP_INTERVAL				120
#define DUNGEON_ELEMENT_GET_TYPE(val)		( ( 0xFFFF0000 & ( val ) ) >> 16 )
#define DUNGEON_ELEMENT_GET_ID(val)			( 0x0000FFFF & ( val ) )
#define DUNGEON_ELEMENT_MAKE(type, id)		( ( (type) << 16 ) | ( id ) )
#define DUNGEON_MAX_LEVEL				20
#define DUNGEON_CUR_MAX_LEVEL			20

#define DUNGEON_MAX_EXPLORE_TIMES_PER_DAY	500

static uint32_t dungeon_max_exp_by_lv[DUNGEON_MAX_LEVEL]={0,48,138,268,454,712,1058,1508,2078,2784,
											3642,4668,5878,7288,8914,10772,12878,15248,17898,20844};

dungeon_all_map_info_t dungeon_map_info;
static dungeon_all_mine_info_t dungeon_mine_info;
static dungeon_all_boss_info_t dungeon_boss_info;
static dungeon_all_event_info_t dungeon_event_info;
static dungeon_all_trigger_info_t dungeon_trigger_info;

static inline uint32_t get_max_hp_by_level(uint32_t level)
{
	return 30 + 6 * level;
}

uint32_t dungeon_get_level_by_exp(uint32_t exp)
{
	int i;
	for(i = 0; i < DUNGEON_MAX_LEVEL; ++i)
	{
		if( exp < dungeon_max_exp_by_lv[i] )
		{
			return i;
		}
	}

	return DUNGEON_CUR_MAX_LEVEL;
}

int dungeon_begin_explore_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	uint32_t map_id, screen_id;

	CHECK_BODY_LEN(len, sizeof(uint32_t) * 2);
	int j = 0;
	UNPKG_UINT32(body, map_id, j);
	UNPKG_UINT32(body, screen_id, j);

	char invalid_id = 0;
	if( map_id > dungeon_map_info.map_count || 0 == map_id )
	{
		invalid_id = 1;
	}
	else
	{
		if( screen_id > dungeon_map_info.map_info[map_id - 1].screen_count || 0 == screen_id )
		{
			invalid_id = 1;
		}
	}
	if( invalid_id == 1 )
	{
		return send_to_self_error(p, p->waitcmd, -ERR_dungeon_invalid_map, 1);
	}
	//dungeon_on_explore_map_id = 0 表示无效地图,即从1开始
	if( p->dungeon_on_explore_map_id != map_id )
	{
		uint32_t db_buf[]={ map_id, screen_id };
		return send_request_to_db(SVR_PROTO_DUNGEON_BEGIN_EXPLORE, p, sizeof(db_buf), db_buf, p->id);
	}

	else
	{
		p->dungeon_on_explore_screen_id = screen_id;
		return dungeon_begin_explore_send_result(p);
	}

}

int dungeon_begin_explore_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	explore_begin_info_t* info;
	uint32_t element_kind;
	uint32_t friend_count;
	uint32_t expect_len = sizeof(explore_begin_info_t) + sizeof(element_kind) + sizeof(friend_count);
	CHECK_BODY_LEN_GE(len, expect_len);
	//clear item buff
	memset(p->dungeon_item_buf, 0, sizeof(p->dungeon_item_buf));
	//clear element buff
	memset(p->dungeon_element_counter, 0, sizeof(p->dungeon_element_counter));
	// clear random element buf
	memset(p->dungeon_rand_element_pos, 0, sizeof(p->dungeon_rand_element_pos));

	p->dungeon_area_id_for_boss = -1;
	int i;
	int j;
	info = (explore_begin_info_t*)buf;
	// save begin time
	p->dungeon_explore_last_time_count = time(NULL) - info->time_surplus;
	p->dungeon_explore_hp = info->hp;
	p->dungeon_explore_exp = info->exp;
	p->dungeon_on_explore_map_id = info->map_id;
	p->dungeon_on_explore_screen_id = info->screen_id;
	p->dungeon_explore_total_counter = info->explore_counter;

	// check valid hp value
	uint32_t max_hp = get_max_hp_by_level( dungeon_get_level_by_exp( p->dungeon_explore_exp ) );
	if( p->dungeon_explore_hp > max_hp )
	{
		p->dungeon_explore_hp = max_hp;
	}

	buf += sizeof(explore_begin_info_t);
	//后续数据为随机交互点的出现次数统计，内容为随机点ID以及出现次数
	j = 0;
	UNPKG_H_UINT32(buf, element_kind, j);
	expect_len += element_kind * sizeof(uint32_t) * 2;
	//数据长度校验
	CHECK_BODY_LEN_GE(len, expect_len);

	if( element_kind > DUNGEON_MAX_RANDOM_ELEMENT_TYPE )
	{
		ERROR_LOG("dungeon_begin_explore_callback random element kind out of size val;[%u]", element_kind);
		element_kind = DUNGEON_MAX_RANDOM_ELEMENT_TYPE;
	}
	uint32_t kind_counter[4];
	memset(kind_counter, 0, sizeof(kind_counter));
	uint32_t element_count;
	for(i = 0; i < element_kind; ++i)
	{
		element_counter_t* element_counter = &p->dungeon_element_counter[i];
		UNPKG_H_UINT32(buf, element_counter->element_id, j);
		UNPKG_H_UINT32(buf, element_count, j);
		element_counter->counter = (uint8_t)element_count;
		kind_counter[DUNGEON_ELEMENT_GET_TYPE(element_counter->element_id)] += element_counter->counter;
	}
	//好友数据
	UNPKG_H_UINT32(buf, friend_count, j);
	uint32_t friend_id;
	expect_len += friend_count * sizeof(friend_id);
	//最终校验
	CHECK_BODY_LEN(len, expect_len);
	//首次进入挖宝地图将向好友发送邀请邮件
	for(i = 0 ; i< friend_count; ++i)
	{
		UNPKG_H_UINT32(buf, friend_id, j);
		//send_postcard(p->nick, p->id, friend_id, 1000225, "", 0);
	}

	//clear explore counter
	memset(p->dungeon_area_explore_counter, -1, sizeof(p->dungeon_area_explore_counter));

	//create random area element

	dungeon_map_info_t* cur_map = &dungeon_map_info.map_info[p->dungeon_on_explore_map_id - 1];


	uint32_t rand_val;
	int rand_element_counter = 0;
	int s;
	for(s = 0; s < cur_map->screen_count; ++s)
	{
		dungeon_screen_info_t* cur_screen = &cur_map->screen_info[s];
		for(i = 0; i < cur_screen->area_count; ++i)
		{
			//固定元素
			if( cur_screen->area_info[i].rand_table == -1 )
			{
				p->dungeon_area_explore_counter[s][i] = 0;
			}
			//随机元素
			else
			{
				if( rand_element_counter > DUNGEON_MAX_RANDOM_ELEMENT_COUNT_PER_SCREEN )
				{
					break;
				}
				rand_val = rand()%100;
				if( rand_val >= cur_screen->area_info[i].rate )
				{
					continue;
				}
				uint32_t limitation;
				switch( cur_screen->area_info[i].area_type )
				{
				case ET_MINE:
					limitation = dungeon_mine_info.info[cur_screen->area_info[i].element_id].max_explore_times;
					break;
				case ET_BOSS:
					limitation = dungeon_boss_info.info[cur_screen->area_info[i].element_id].max_explore_times;
					break;
				case ET_EVENT:
					limitation = dungeon_event_info.info[cur_screen->area_info[i].element_id].max_explore_times;
					break;
				case ET_TRIGGER:
					limitation = dungeon_trigger_info.info[cur_screen->area_info[i].element_id].max_explore_times;
					break;
				default:
					break;
				}
				//超过总上限
				if( kind_counter[cur_screen->area_info[i].area_type] >= cur_map->random_element_limit.mine )
				{
					continue;
				}
				else
				{
					uint32_t val = DUNGEON_ELEMENT_MAKE(cur_screen->area_info[i].area_type, cur_screen->area_info[i].element_id);
					int j;
					for(j = 0; j < DUNGEON_MAX_RANDOM_ELEMENT_TYPE; ++j)
					{
						if( 0 == p->dungeon_element_counter[j].counter )
						{
							p->dungeon_element_counter[j].element_id = val;
							p->dungeon_element_counter[j].counter++;
							// 探索计数设定为0,默认是-1表示不可探索
							p->dungeon_area_explore_counter[s][i] = 0;
							// 记录随机元素的位置，以便切屏时可以恢复
							p->dungeon_rand_element_pos[s][rand_element_counter].element_id = i;
							p->dungeon_rand_element_pos[s][rand_element_counter].pos = rand()%cur_screen->rand_table[cur_screen->area_info[i].rand_table];
							rand_element_counter++;
							break;
						}
						else if( p->dungeon_element_counter[j].element_id == val )
						{
							if( p->dungeon_element_counter[j].counter < limitation )
							{
								p->dungeon_element_counter[j].counter++;
								p->dungeon_area_explore_counter[s][i] = 0;
								p->dungeon_rand_element_pos[s][rand_element_counter].element_id = (uint8_t)i;
								p->dungeon_rand_element_pos[s][rand_element_counter].pos = (uint8_t)rand()%cur_screen->rand_table[cur_screen->area_info[i].rand_table];
								rand_element_counter++;
							}
							break;
						}
					}
				}

			}
		}
	}

	//send the element counter to db
	uint8_t db_buf[512];
	uint32_t count = 0;
	j = 0;
	PKG_H_UINT32(db_buf, p->dungeon_on_explore_map_id, j);
	int count_pos = j;
	PKG_H_UINT32(db_buf, count, j);

	for(i = 0; i < DUNGEON_MAX_RANDOM_ELEMENT_TYPE; ++i)
	{
		if( 0 == p->dungeon_element_counter[i].counter )
		{
			break;
		}
		PKG_H_UINT32(db_buf, p->dungeon_element_counter[i].element_id, j);
		PKG_H_UINT32(db_buf, p->dungeon_element_counter[i].counter, j);
		count++;
	}
	PKG_H_UINT32(db_buf, count, count_pos);
	send_request_to_db(SVR_PROTO_DUNGEON_UPDATE_MAP_INFO, NULL, j, db_buf, p->id);

	//记录开始挖宝的时间
	p->dungeon_enter_map_time = time(NULL);

	return dungeon_begin_explore_send_result(p);

}

int dungeon_begin_explore_send_result(sprite_t * p)
{
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, p->dungeon_explore_hp, j);
	uint32_t level = dungeon_get_level_by_exp( p->dungeon_explore_exp );

	//统计 等级
	if( p->dungeon_explore_exp > 0 )
	{
		uint32_t msgbuff[2]= {p->id, level};
		msglog(statistic_logfile, 0x04088329, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}

	PKG_UINT32(msg, level, j);
	PKG_UINT32(msg, p->dungeon_explore_exp, j);
	uint32_t count = 0;
	int count_pos = j;

	PKG_UINT32(msg, count, j);
	uint32_t i;
	for(i = 0; i < DUNGEON_MAX_MAP_AREA_NUM; ++i)
	{
		if( 0 <= p->dungeon_area_explore_counter[p->dungeon_on_explore_screen_id - 1][i] )
		{
			int k;
			int is_rand = 0;
			for(k = 0; k < DUNGEON_MAX_RANDOM_ELEMENT_COUNT_PER_SCREEN; ++k)
			{
				if( 0 == p->dungeon_rand_element_pos[p->dungeon_on_explore_screen_id - 1][k].element_id )
				{
					break;
				}
				else if((uint8_t)(i+1) == p->dungeon_rand_element_pos[p->dungeon_on_explore_screen_id - 1][k].element_id)
				{
					is_rand = 1;
					break;
				}
			}
			if( is_rand )
			{
				continue;
			}
			PKG_UINT32(msg, i, j);
			PKG_UINT32(msg, p->dungeon_area_explore_counter[p->dungeon_on_explore_screen_id - 1][i], j);
			count++;
		}
	}
	PKG_UINT32(msg, count, count_pos);

	count_pos = j;
	count = 0;
	PKG_UINT32(msg, count, j);
	for(i = 0; i< DUNGEON_MAX_RANDOM_ELEMENT_COUNT_PER_SCREEN; ++i)
	{
		uint32_t area_id = p->dungeon_rand_element_pos[p->dungeon_on_explore_screen_id - 1][i].element_id;
		if( 0 != area_id )
		{
			area_id -= 1;
			PKG_UINT32(msg, area_id, j);
			PKG_UINT32(msg, p->dungeon_area_explore_counter[p->dungeon_on_explore_screen_id - 1][area_id], j);
			PKG_UINT32(msg, p->dungeon_rand_element_pos[p->dungeon_on_explore_screen_id - 1][i].pos, j);
			count++;
		}
	}
	PKG_UINT32(msg, count, count_pos);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);

}

int dungeon_get_bag_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
/*	if( 0 == p->dungeon_on_explore_map_id )
	{
		return send_to_self_error(p, p->waitcmd, ERR_dungeon_have_not_begin, 1);
	}
*/	return send_request_to_db(SVR_PROTO_DUNGEON_GET_BAG_INFO, p, 0, NULL, p->id);
}

int dungeon_get_bag_info_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	uint32_t kind;
	CHECK_BODY_LEN_GE(len, sizeof(kind));
	int j = 0;
	UNPKG_H_UINT32(buf, kind, j);
	CHECK_BODY_LEN(len, sizeof(uint32_t) + kind * sizeof(uint32_t) * 2);
	int k = sizeof(protocol_t);
	PKG_UINT32(msg, kind, k);
	uint32_t item_id, item_count;
	int i;
	for(i = 0; i < kind; ++i)
	{
		UNPKG_H_UINT32(buf, item_id, j);
		UNPKG_H_UINT32(buf, item_count, j);
		PKG_UINT32(msg, item_id, k);
		PKG_UINT32(msg, item_count, k);
	}
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int dungeon_explore_area_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	if(  0 == p->dungeon_on_explore_map_id )
	{
		return send_to_self_error(p, p->waitcmd, -ERR_dungeon_have_not_begin, 1);
	}
	uint32_t area_id;
	int j = 0;
	UNPKG_UINT32(body, area_id, j);

	if( area_id > dungeon_map_info.map_info[p->dungeon_on_explore_map_id - 1].screen_info[p->dungeon_on_explore_screen_id - 1].area_count )
	{
		return send_to_self_error(p, p->waitcmd, -ERR_dungeon_invalid_area, 1);
	}

	if( p->dungeon_area_explore_counter[p->dungeon_on_explore_screen_id - 1][area_id] < 0 )
	{
		return send_to_self_error(p, p->waitcmd, -ERR_dungeon_invalid_area, 1);
	}

	//计算恢复体力值
	uint32_t now = time(NULL);
	if( now - p->dungeon_explore_last_time_count >= DUNGEON_HP_INTERVAL )
	{
		p->dungeon_explore_hp += (now - p->dungeon_explore_last_time_count) / DUNGEON_HP_INTERVAL * DUNGEON_HP_INC_VAL;
		p->dungeon_explore_last_time_count += (now - p->dungeon_explore_last_time_count) / DUNGEON_HP_INTERVAL * DUNGEON_HP_INTERVAL;
	}

	uint32_t max_hp = get_max_hp_by_level( dungeon_get_level_by_exp( p->dungeon_explore_exp ) );
	if( p->dungeon_explore_hp > max_hp )
	{
		p->dungeon_explore_hp = max_hp;
	}

	area_info_t* cur_area = &dungeon_map_info.map_info[p->dungeon_on_explore_map_id - 1].screen_info[p->dungeon_on_explore_screen_id - 1].area_info[area_id];

	switch(cur_area->area_type)
	{
	case ET_MINE:
		return dungeon_explore_mine(p, cur_area->element_id, area_id);
	case ET_BOSS:
		return dungeon_explore_boss(p, cur_area->element_id, area_id);
	case ET_EVENT:
		return dungeon_explore_event(p, cur_area->element_id, area_id);
	case ET_TRIGGER:
		return dungeon_explore_trigger(p, cur_area->element_id, area_id);
	default:
		break;
	}
	return -1;
}

int dungeon_explore_mine(sprite_t * p,uint32_t element_id, uint32_t area_id )
{

	uint32_t hp_consum;
	uint32_t cur_count = p->dungeon_area_explore_counter[p->dungeon_on_explore_screen_id - 1][area_id];
	if( dungeon_mine_info.info[element_id].max_count<= cur_count)
	{
		return send_to_self_error(p, p->waitcmd, -ERR_dungeon_invalid_area, 1);
	}
	else
	{
		hp_consum = dungeon_mine_info.info[element_id].hp_consum;
		p->dungeon_area_explore_counter[p->dungeon_on_explore_screen_id - 1][area_id]++;
	}

	if( p->dungeon_explore_hp < hp_consum )
	{
		return send_to_self_error(p, p->waitcmd, -ERR_dungeon_not_enough_hp, 1);
	}
	else
	{
		p->dungeon_explore_hp -= hp_consum;
	}

	uint32_t exp_inc = dungeon_mine_info.info[element_id].exp_inc;

	int i;
	explore_region_t* cur_regeion = NULL;
	for(i = 0; i <	dungeon_mine_info.info[element_id].region_kind; ++i)
	{
		if(	dungeon_mine_info.info[element_id].region_info[i].max_explore_count > cur_count )
		{
			cur_regeion = &dungeon_mine_info.info[element_id].region_info[i];
			break;
		}
	}

	uint32_t offset = 0;
	uint32_t rand_val = rand()%100;
	award_info_t* cur_award = NULL;
	if( cur_regeion )
	{
		if( p->dungeon_explore_total_counter >= DUNGEON_MAX_EXPLORE_TIMES_PER_DAY )
		{
			rand_val = -1;
		}
		else if( p->dungeon_explore_failed_counter >= 5 )
		{
			for(i = 0; i < cur_regeion->award_kind; ++i)
			{
				offset += cur_regeion->award_info[i].rate;
			}
			rand_val = rand()%offset;
			offset = 0;
		}
		uint8_t	get_award = 0;
		for(i = 0; i < cur_regeion->award_kind; ++i)
		{
			offset += cur_regeion->award_info[i].rate;
			if( rand_val < offset )
			{
				cur_award = &cur_regeion->award_info[i];
				get_award = 1;
				break;
			}

		}

		if( 0 == get_award )
		{
			p->dungeon_explore_failed_counter++;
		}
		else
		{
			p->dungeon_explore_failed_counter = 0;
			p->dungeon_explore_total_counter++;
		}
	}

	//20111230活动，30%概率随进掉落地图碎片
	if( 0 && rand()%100 < 30 )
	{
		//统计 地图碎片输出
		{
			uint32_t msgbuff[2]= {p->id, 1};
			msglog(statistic_logfile, 0x0409C2D4, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
		uint32_t rand_item_list[] = { 1351212, 1351213, 1351214, 1351215, 1351216, 1351217, 1351218, 1351219, 1351220, 1351221,
			1351222, 1351223, 1351224, 1351225, 1351226, 1351227, 1351228, 1351229, 1351230, 1351231, 1351232, 1351233, 1351234,
			1351235, 1351236, 1351237, 1351238, 1351239, 1351240, 1351241 };
		uint32_t rand_item_cnt = sizeof(rand_item_list)/sizeof(uint32_t);
		uint32_t item_id = rand_item_list[rand() % rand_item_cnt];
		uint32_t item_cnt = 1;
		//notice db
		{
			int sendlen = 0;
			uint8_t buff[128];
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

		int msg_len = sizeof(protocol_t);
		PKG_UINT32( msg, item_id, msg_len );
		PKG_UINT32( msg, item_cnt, msg_len );
		init_proto_head( msg, PROTO_DUNGEON_GET_SYS_AWARD, msg_len );
		send_to_self( p, msg, msg_len, 0 );
	}
	
	return dungeon_explore_area_send_result(p, cur_award, area_id, exp_inc, hp_consum);
}

int dungeon_explore_boss(sprite_t * p,uint32_t element_id,uint32_t area_id)
{
	if( area_id > 127 )
	{
		DEBUG_LOG("dungeon explore boss area_id greater than 127, the value is:[%u]", area_id);
	}
	else
	{
		p->dungeon_area_id_for_boss = (char)area_id;
	}
	return dungeon_explore_area_send_result(p, NULL, area_id, 0, 0);
}

int dungeon_explore_event(sprite_t * p,uint32_t element_id,uint32_t area_id)
{
	uint32_t hp_consum;
	uint32_t cur_count = p->dungeon_area_explore_counter[p->dungeon_on_explore_screen_id - 1][area_id];
	if( 0 != cur_count)
	{
		return send_to_self_error(p, p->waitcmd, -ERR_dungeon_invalid_area, 1);
	}
	else
	{
		hp_consum = dungeon_event_info.info[element_id].hp_consum;
		p->dungeon_area_explore_counter[p->dungeon_on_explore_screen_id - 1][area_id] = 1;
	}
	if( p->dungeon_explore_hp < hp_consum )
	{
		return send_to_self_error(p, p->waitcmd, -ERR_dungeon_not_enough_hp, 1);
	}
	else
	{
		p->dungeon_explore_hp -= hp_consum;
	}

	uint32_t exp_inc = dungeon_event_info.info[element_id].exp_inc;

	uint32_t offset = 0;
	uint32_t rand_val = rand()%100;
	award_info_t* cur_award = NULL;
	int i;
	for(i = 0; i < dungeon_event_info.info[element_id].award_kind; ++i)
	{
		offset += dungeon_event_info.info[element_id].award_info[i].rate;
		if( rand_val < offset )
		{
			cur_award = &dungeon_event_info.info[element_id].award_info[i];
			break;
		}
	}
	return dungeon_explore_area_send_result(p, cur_award, area_id, exp_inc, hp_consum);

}

int dungeon_explore_trigger(sprite_t * p,uint32_t element_id,uint32_t area_id)
{
	uint32_t hp_consum;
	p->dungeon_area_explore_counter[p->dungeon_on_explore_screen_id - 1][area_id]++;
	hp_consum = dungeon_trigger_info.info[element_id].hp_consum;

	if( p->dungeon_explore_hp < hp_consum )
	{
		return send_to_self_error(p, p->waitcmd, -ERR_dungeon_not_enough_hp, 1);
	}
	else
	{
		p->dungeon_explore_hp -= hp_consum;
	}

	uint32_t exp_inc = dungeon_trigger_info.info[element_id].exp_inc;

	return dungeon_explore_area_send_result(p, NULL, area_id, exp_inc, hp_consum);
}

int dungeon_explore_area_send_result(sprite_t * p,award_info_t * award, uint32_t area_id, uint32_t exp_inc, uint32_t hp_consum)
{
	uint32_t last_level = dungeon_get_level_by_exp(p->dungeon_explore_exp);
	p->dungeon_explore_exp += exp_inc;
	uint32_t cur_level = dungeon_get_level_by_exp(p->dungeon_explore_exp);
	uint32_t is_level_up = 0;
	if( cur_level > last_level )
	{
		is_level_up = 1;
		p->dungeon_explore_hp = get_max_hp_by_level(cur_level);
	}
	//统计体力消耗
	uint32_t msgbuff[2]= {p->id, hp_consum};
	msglog(statistic_logfile, 0x04088326, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	uint32_t time_surplus = time(NULL) - p->dungeon_explore_last_time_count;
	uint32_t db_buf[]={ p->dungeon_explore_hp, p->dungeon_explore_exp, time_surplus, p->dungeon_explore_total_counter };
	send_request_to_db(SVR_PROTO_DUNGEON_UPDATE_HP_EXP, NULL, sizeof(db_buf), db_buf, p->id);

	int k = sizeof(protocol_t);
	PKG_UINT32(msg, area_id, k);
	PKG_UINT32(msg, p->dungeon_area_explore_counter[p->dungeon_on_explore_screen_id - 1][area_id], k);
	PKG_UINT32(msg, p->dungeon_explore_hp, k);
	PKG_UINT32(msg, p->dungeon_explore_exp, k);
	PKG_UINT32(msg, time_surplus, k);
	PKG_UINT32(msg, is_level_up, k);
	uint32_t item_kind;
	if( award )
	{
		item_kind = award->item_kind;
	}
	else
	{
		item_kind = 0;
	}
	PKG_UINT32(msg, item_kind, k);
	int i;
	for(i = 0; i < item_kind; ++i)
	{
		int n;
		for(n = 0; n < DUNGEON_MAX_ITEM_KIND; ++n)
		{
			if( 0 == p->dungeon_item_buf[n].item_id || award->item_info[i].item_id == p->dungeon_item_buf[n].item_id )
			{
				p->dungeon_item_buf[n].item_id = award->item_info[i].item_id;
				p->dungeon_item_buf[n].item_count += (uint8_t)award->item_info[i].item_count;
				break;
			}
		}
		PKG_UINT32(msg, award->item_info[i].item_id, k);
		PKG_UINT32(msg, award->item_info[i].item_count, k);
	}
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int dungeon_pick_item_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	if( 0 == p->dungeon_on_explore_map_id )
	{
		return send_to_self_error(p, p->waitcmd, -ERR_dungeon_have_not_begin, 1);
	}
	uint32_t item_id, item_count;
	CHECK_BODY_LEN(len, sizeof(item_id) + sizeof(item_count));
	int j = 0;
	UNPKG_UINT32(body, item_id, j);
	UNPKG_UINT32(body, item_count, j);

	uint8_t real_item_count = (uint8_t)item_count;

	int i;
	uint8_t bGetItem = 0;
	for(i = 0; i < DUNGEON_MAX_ITEM_KIND; ++i)
	{
		if( p->dungeon_item_buf[i].item_id == item_id)
		{
			if( p->dungeon_item_buf[i].item_count >= real_item_count )
			{
				p->dungeon_item_buf[i].item_count -= real_item_count;
				if( 0 == p->dungeon_item_buf[i].item_count )
				{
					p->dungeon_item_buf[i].item_id = 0;
				}
				bGetItem = 1;
				break;
			}

		}
	}
	if( bGetItem )
	{
		db_exchange_single_item_op(p, 202, item_id, item_count, 0);
		int k = sizeof(protocol_t);
		PKG_UINT32(msg, item_id, k);
		PKG_UINT32(msg, item_count, k);
		init_proto_head(msg, p->waitcmd, k);
		return send_to_self(p, msg, k, 1);
	}
	else
	{
		return send_to_self_error(p, p->waitcmd, -ERR_dungeon_no_item_to_pick, 1);
	}

}

int dungeon_use_item_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	uint32_t item_id;
	CHECK_BODY_LEN(len, sizeof(item_id));
	int j = 0;
	UNPKG_UINT32(body, item_id, j);
	return send_request_to_db(SVR_PROTO_DUNGEON_USE_ITEM, p, sizeof(item_id), &item_id, p->id);
}

int dungeon_use_item_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	typedef struct use_item_s
	{
		uint32_t item_id;
		uint32_t hp;
	} use_item_t;

	CHECK_BODY_LEN(len, sizeof(use_item_t));
	use_item_t *use_item_back = (use_item_t*)buf;

	//统计 消耗类道具使用
	uint32_t cmd_id = 0;
	if( use_item_back->item_id == 1453301 )
	{
		cmd_id = 0x04088327;
	}
	else if( use_item_back->item_id == 1453302 )
	{
		cmd_id = 0x04088328;
	}
	if( cmd_id != 0 )
	{
		uint32_t msgbuff[2]= {p->id, 1};
		msglog(statistic_logfile, cmd_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	//end

	p->dungeon_explore_hp = use_item_back->hp;

	uint32_t max_hp = get_max_hp_by_level( dungeon_get_level_by_exp( p->dungeon_explore_exp ) );
	if( p->dungeon_explore_hp > max_hp )
	{
		p->dungeon_explore_hp = max_hp;
	}

	int k = sizeof(protocol_t);
	PKG_UINT32(msg, use_item_back->item_id, k);
	PKG_UINT32(msg, p->dungeon_explore_hp, k);

	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int dungeon_end_explore_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	p->dungeon_on_explore_map_id = 0;
	int k = sizeof(protocol_t);
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int dungeon_load_map_info(char * file, dungeon_map_info_t * map_info)
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	int err = -1;

	memset(map_info, 0, sizeof (dungeon_map_info_t));

	doc = xmlParseFile (file);
	if (!doc)
	{
		ERROR_RETURN (("load dungeon map failed"), -1);
	}

	cur = xmlDocGetRootElement(doc);
	if (!cur)
	{
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}
	xmlNodePtr chl_screen = cur->children;
	uint32_t* screen_count = &map_info->screen_count;
	*screen_count = 0;
	while( chl_screen )
	{
		if( 0 == xmlStrcmp( chl_screen->name, (const xmlChar *)"Screen" ) )
		{
			if(*screen_count >= DUNGEON_MAX_SCREEN_NUM)
			{
				ERROR_RETURN (("dungeon_load_map_info Screen Count Out Of Max Range"), -1);
			}
			dungeon_screen_info_t* screen_info = &map_info->screen_info[*screen_count];
			DECODE_XML_PROP_UINT32(screen_info->map_id, chl_screen, "mapId");
			xmlNodePtr chl_area = chl_screen->children;
			screen_info->area_count = 0;
			screen_info->rand_table_count = 0;
			while( chl_area )
			{
				if( 0 == xmlStrcmp( chl_area->name, (const xmlChar *)"Area" ) )
				{
					if(screen_info->area_count >= DUNGEON_MAX_MAP_AREA_NUM)
					{
						ERROR_RETURN (("dungeon_load_map_info Area Count Out Of Max Range"), -1);
					}
				 	DECODE_XML_PROP_UINT32(screen_info->area_info[screen_info->area_count].area_type, chl_area, "Type");
					DECODE_XML_PROP_UINT32(screen_info->area_info[screen_info->area_count].element_id, chl_area, "ID");
					DECODE_XML_PROP_INT_DEFAULT(screen_info->area_info[screen_info->area_count].rand_table, chl_area, "RandTable", -1);
					DECODE_XML_PROP_INT_DEFAULT(screen_info->area_info[screen_info->area_count].rate, chl_area, "Rate", 100);
					screen_info->area_count++;
				}
				else if( 0 == xmlStrcmp( chl_area->name, (const xmlChar *)"RandTable" ) )
				{
					if(screen_info->rand_table_count >= DUNGEON_MAX_RANDOM_TABLE_NUM)
					{
						ERROR_RETURN (("dungeon_load_map_info RandomTable Count Out Of Max Range"), -1);
					}
					xmlNodePtr chl_rand_pos = chl_area->children;
					screen_info->rand_table[screen_info->rand_table_count] = 0;
					while( chl_rand_pos )
					{
						if( 0 == xmlStrcmp( chl_rand_pos->name, (const xmlChar *)"Position" ) )
						{
							screen_info->rand_table[screen_info->rand_table_count]++;
						}
						chl_rand_pos = chl_rand_pos->next;
					}
					screen_info->rand_table_count++;
				}
				chl_area = chl_area->next;
			}
			(*screen_count)++;
		}
		else if( 0 == xmlStrcmp( chl_screen->name, (const xmlChar *)"RandLimit" ) )
		{
			DECODE_XML_PROP_INT_DEFAULT(map_info->random_element_limit.mine, chl_screen, "Mine", -1);
			DECODE_XML_PROP_INT_DEFAULT(map_info->random_element_limit.boss, chl_screen, "Boss", -1);
			DECODE_XML_PROP_INT_DEFAULT(map_info->random_element_limit.event, chl_screen, "Event", -1);
			DECODE_XML_PROP_INT_DEFAULT(map_info->random_element_limit.trigger, chl_screen, "Trigger", -1);
		}
		chl_screen = chl_screen->next;
	}
	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load dungeon map file %s", file);
}

int dungeon_load_map_element(char * file)
{
	xmlDocPtr doc;
	xmlNodePtr root;
	int err = -1;

	memset(&dungeon_mine_info, 0, sizeof (dungeon_mine_info));
	memset(&dungeon_boss_info, 0, sizeof (dungeon_boss_info));
	memset(&dungeon_event_info, 0, sizeof (dungeon_event_info));
	memset(&dungeon_trigger_info, 0, sizeof (dungeon_trigger_info));

	doc = xmlParseFile (file);
	if (!doc)
	{
		ERROR_RETURN (("load dungeon map element failed"), -1);
	}

	root = xmlDocGetRootElement(doc);
	if (!root)
	{
		ERROR_LOG ("xmlDocGetRootElement error");
		//DEBUG_LOG("===dungeon load element get root  failed===");
		goto exit;
	}
	if( root )
	{
		xmlNodePtr chl_element_group = root->children;
		while( chl_element_group )
		{
			int ret;
			//DEBUG_LOG("===dungeon load element parse Tag is:%s===", chl_element_group->name);
			if( 0 == xmlStrcmp( chl_element_group->name, (const xmlChar *)"MineEnum" ) )
			{
				ret = dungeon_parse_mine_info(chl_element_group, &dungeon_mine_info);
			}
			else if( 0 == xmlStrcmp( chl_element_group->name, (const xmlChar *)"BossEnum" ) )
			{
				ret = dungeon_parse_boss_info(chl_element_group, &dungeon_boss_info);
			}
			else if( 0 == xmlStrcmp( chl_element_group->name, (const xmlChar *)"EventEnum" ) )
			{
				ret = dungeon_parse_event_info(chl_element_group, &dungeon_event_info);
			}
			else if( 0 == xmlStrcmp( chl_element_group->name, (const xmlChar *)"TriggerEnum" ) )
			{
				ret = dungeon_parse_trigger_info(chl_element_group, &dungeon_trigger_info);
			}
			else
			{
				ret = 0;
			}
			if( ret != 0 )
			{
				//DEBUG_LOG("===dungeon load element parse elemetn  failed===");
				goto exit;
			}
			chl_element_group = chl_element_group->next;
		}
	}
	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load dungeon map element file %s", file);
}

int dungeon_parse_mine_info(xmlNodePtr ptr, dungeon_all_mine_info_t * mine_info)
{
	DEBUG_LOG("===dungeon load element parse mine===");
	if( ptr )
	{
		xmlNodePtr chl_main = ptr->children;
		uint32_t* i = &mine_info->kind;
		while( chl_main )
		{
			//DEBUG_LOG("===dungeon load element parse Mine Tag is:%s===", chl_main->name);
			if( 0 != xmlStrcmp( chl_main->name, (const xmlChar *)"Mine" ) )
			{
				chl_main = chl_main->next;
				continue;
			}
			if( *i >= DUNGEON_MAP_MAX_MINE_KIND )
			{
				ERROR_RETURN (("dungeon_parse_mine_info Mine Count Out Of Max Range"), -1);
			}
			DECODE_XML_PROP_UINT32(mine_info->info[*i].hp_consum, chl_main, "Hp");
			DECODE_XML_PROP_UINT32(mine_info->info[*i].exp_inc, chl_main, "Exp");
			DECODE_XML_PROP_UINT32(mine_info->info[*i].max_explore_times, chl_main, "MaxCount");
			DECODE_XML_PROP_UINT32(mine_info->info[*i].max_count, chl_main, "ExplCount");
			xmlNodePtr chl_region = chl_main->children;
			uint32_t* region_kind = &mine_info->info[*i].region_kind;
			while( chl_region )
			{
				if( 0 != xmlStrcmp( chl_region->name, (const xmlChar *)"Region" ) )
				{
					chl_region = chl_region->next;
					continue;
				}
				if( *region_kind >= DUNGEON_MAX_REGION_KIND)
				{
					ERROR_RETURN (("dungeon_parse_mine_info Region Count Out Of Max Range MineID:[%u]", *i), -1);
				}
				explore_region_t* region = &mine_info->info[*i].region_info[*region_kind];
				DECODE_XML_PROP_UINT32(region->max_explore_count, chl_region, "Count");
				xmlNodePtr chl_award = chl_region->children;
				uint32_t* award_kind = &region->award_kind;
				while( chl_award )
				{
					if( 0 != xmlStrcmp( chl_award->name, (const xmlChar *)"Award" ) )
					{
						chl_award = chl_award->next;
						continue;
					}
					if( *award_kind >= DUNGEON_MAX_AWARD_KIND)
					{
						ERROR_RETURN (("dungeon_parse_mine_info Award Count Out Of Max Range MineID:[%u],RegionID:[%u]", *i, *region_kind), -1);
					}
					award_info_t* award = &region->award_info[*award_kind];
					DECODE_XML_PROP_UINT32(award->rate, chl_award, "Rate");
					xmlNodePtr chl_item = chl_award->children;
					uint32_t* item_kind = &award->item_kind;
					while( chl_item )
					{
						if( 0 != xmlStrcmp( chl_item->name, (const xmlChar *)"Item" ) )
						{
							chl_item = chl_item->next;
							continue;
						}
						if( *item_kind >= DUNGEON_MAX_AWARD_ITEM_KIND)
						{
							ERROR_RETURN (("dungeon_parse_mine_info AwardItem Count Out Of Max Range MineID:[%u],RegionID:[%u],AwardID:[%u]", *i, *region_kind, *award_kind), -1);
						}
						award_item_info_t* item_info = &(award->item_info[*item_kind]);
						DECODE_XML_PROP_UINT32(item_info->item_id, chl_item, "ID");
						DECODE_XML_PROP_UINT32(item_info->item_count, chl_item, "Count");
						chl_item = chl_item->next;
						(*item_kind)++;
					}
					chl_award = chl_award->next;
					(*award_kind)++;
				}
				chl_region = chl_region->next;
				(*region_kind)++;
			}
			chl_main = chl_main->next;
			(*i)++;
		}
	}
	return 0;
}

int dungeon_parse_boss_info(xmlNodePtr ptr,dungeon_all_boss_info_t * boss_info)
{
	DEBUG_LOG("===dungeon load element parse boss===");

	if( ptr )
	{
		xmlNodePtr chl_main = ptr->children;
		uint32_t* i = &boss_info->kind;
		while( chl_main )
		{
			//DEBUG_LOG("===dungeon load element parse Boss Tag is:%s===", chl_main->name);
			if( 0 != xmlStrcmp( chl_main->name, (const xmlChar *)"Boss" ) )
			{
				chl_main = chl_main->next;
				continue;
			}
			if( *i >= DUNGEON_MAP_MAX_BOSS_KIND)
			{
				ERROR_RETURN (("dungeon_parse_event_info Boss Count Out Of Max Range"), -1);
			}
			DECODE_XML_PROP_UINT32(boss_info->info[*i].hp_consum, chl_main, "Hp");
			DECODE_XML_PROP_UINT32(boss_info->info[*i].exp_inc, chl_main, "Exp");
			DECODE_XML_PROP_UINT32(boss_info->info[*i].max_explore_times, chl_main, "MaxCount");
			chl_main = chl_main->next;
			(*i)++;
		}
	}
	return 0;
}

int dungeon_parse_event_info(xmlNodePtr ptr,dungeon_all_event_info_t * event_info)
{
	DEBUG_LOG("===dungeon load element parse event===");

	if( ptr )
	{
		xmlNodePtr chl_main = ptr->children;
		uint32_t* i = &event_info->kind;
		while( chl_main )
		{
			//DEBUG_LOG("===dungeon load element parse Event Tag is:%s===", chl_main->name);
			if( 0 != xmlStrcmp( chl_main->name, (const xmlChar *)"Event" ) )
			{
				chl_main = chl_main->next;
				continue;
			}
			if( *i >= DUNGEON_MAP_MAX_EVENT_KIND)
			{
				ERROR_RETURN (("dungeon_parse_event_info Event Count Out Of Max Range"), -1);
			}
			DECODE_XML_PROP_UINT32(event_info->info[*i].hp_consum, chl_main, "Hp");
			DECODE_XML_PROP_UINT32(event_info->info[*i].exp_inc, chl_main, "Exp");
			DECODE_XML_PROP_UINT32(event_info->info[*i].max_explore_times, chl_main, "MaxCount");
			xmlNodePtr chl_award = chl_main->children;
			uint32_t* award_kind = &event_info->info[*i].award_kind;
			while( chl_award )
			{
				if( 0 != xmlStrcmp( chl_award->name, (const xmlChar *)"Award" ) )
				{
					chl_award = chl_award->next;
					continue;
				}
				if( *award_kind >= DUNGEON_MAX_AWARD_KIND)
				{
					ERROR_RETURN (("dungeon_parse_event_info Award Count Out Of Max Range EventID:[%u]", *i), -1);
				}
				award_info_t* award = &event_info->info[*i].award_info[*award_kind];
				DECODE_XML_PROP_UINT32(award->rate, chl_award, "Rate");
				xmlNodePtr chl_item = chl_award->children;
				uint32_t* item_kind = &award->item_kind;
				while( chl_item )
				{
					if( 0 != xmlStrcmp( chl_item->name, (const xmlChar *)"Item" ) )
					{
						chl_item = chl_item->next;
						continue;
					}
					if( *item_kind >= DUNGEON_MAX_AWARD_ITEM_KIND)
					{
						ERROR_RETURN (("dungeon_parse_event_info AwardItem Count Out Of Max Range EventID:[%u],AwardID[%u]", *i, *award_kind), -1);
					}
					award_item_info_t* item_info = &award->item_info[*item_kind];
					DECODE_XML_PROP_UINT32(item_info->item_id, chl_item, "ID");
					DECODE_XML_PROP_UINT32(item_info->item_count, chl_item, "Count");
					chl_item = chl_item->next;
					(*item_kind)++;
				}
				(*award_kind)++;
				chl_award = chl_award->next;
			}
			chl_main = chl_main->next;
			(*i)++;
		}
	}
	return 0;
}

int dungeon_parse_trigger_info(xmlNodePtr ptr,dungeon_all_trigger_info_t * trigger_info)
{
	DEBUG_LOG("===dungeon load element parse trigger===");

	if( ptr )
	{
		xmlNodePtr chl_main = ptr->children;
		uint32_t* i = &trigger_info->kind;
		while( chl_main )
		{
			//DEBUG_LOG("===dungeon load element parse Trigger Tag is:%s===", chl_main->name);
			if( 0 != xmlStrcmp( chl_main->name, (const xmlChar *)"Trigger" ) )
			{
				chl_main = chl_main->next;
				continue;
			}
			if( *i >= DUNGEON_MAP_MAX_TRIGGER_KIND)
			{
				ERROR_RETURN (("dungeon_parse_event_info Trigger Count Out Of Max Range Count:[%u]", *i), -1);
			}
			DECODE_XML_PROP_UINT32(trigger_info->info[*i].hp_consum, chl_main, "Hp");
			DECODE_XML_PROP_UINT32(trigger_info->info[*i].exp_inc, chl_main, "Exp");
			DECODE_XML_PROP_UINT32(trigger_info->info[*i].max_explore_times, chl_main, "MaxCount");
			DECODE_XML_PROP_UINT32(trigger_info->info[*i].max_count, chl_main, "ExplCount");
			chl_main = chl_main->next;
			(*i)++;
		}
	}
	return 0;
}

int dungeon_load_all_map_info()
{
char file_name[64];
uint32_t i;
for (i = 0; i < DUNGEON_MAP_MAX_SIZE; i++)
{
	if( dungeon_map_info.map_count >= DUNGEON_MAP_MAX_SIZE )
	{
		ERROR_RETURN (("dungeon_load_all_map_info Map Count Out Of Max Range"), -1);
	}
	memset(file_name, 0, sizeof(file_name));
	snprintf(file_name, sizeof(file_name), "./conf/DigTreasureMap_%u.xml", i + 1);
	DEBUG_LOG("===dungeon load file:%s ===", file_name);
	if (dungeon_load_map_info(file_name, &dungeon_map_info.map_info[i]) == -1)
	{
		return -1;
	}
	dungeon_map_info.map_count++;
}
//DEBUG_LOG("===dungeon load file: ./conf/DigTreasureElement.xml ===");
if( dungeon_load_map_element("./conf/DigTreasureElement.xml") == -1)
{
	return -1;
}
return 0;
}

int dungeon_syn_hp_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	if( 0 == p->dungeon_on_explore_map_id )
	{
		return send_to_self_error(p, p->waitcmd, -ERR_dungeon_have_not_begin, 1);
	}

	//计算恢复体力值
	uint32_t now = time(NULL);
	uint32_t surplus_time =  (now - p->dungeon_explore_last_time_count) % DUNGEON_HP_INTERVAL;
	if( now - p->dungeon_explore_last_time_count >= DUNGEON_HP_INTERVAL )
	{
		p->dungeon_explore_hp += (now - p->dungeon_explore_last_time_count) / DUNGEON_HP_INTERVAL * DUNGEON_HP_INC_VAL;
		p->dungeon_explore_last_time_count += (now - p->dungeon_explore_last_time_count) / DUNGEON_HP_INTERVAL * DUNGEON_HP_INTERVAL;
	}
	uint32_t max_hp = get_max_hp_by_level( dungeon_get_level_by_exp( p->dungeon_explore_exp ) );
	if( p->dungeon_explore_hp > max_hp )
	{
		p->dungeon_explore_hp = max_hp;
	}
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, p->dungeon_explore_hp ,j);
	PKG_UINT32(msg, surplus_time ,j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

//珍宝库部分

int dungeon_get_exhibit_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	uint32_t user_id;
	CHECK_BODY_LEN(len, sizeof(user_id));
	int j = 0;
	UNPKG_UINT32(body, user_id, j);
	CHECK_VALID_ID(user_id);

	//珍宝库进入次数统计
	uint32_t msgbuff[2]= {p->id, 1};
	msglog(statistic_logfile, 0x0408A33D, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	return send_request_to_db(SVR_PROTO_DUNGEON_GET_EXHIBIT_INFO, p, 0, NULL, user_id);
}

int dungeon_get_exhibit_info_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	typedef struct back_info_s
	{
		uint32_t is_vip;
		uint32_t level;
		uint32_t exp;
		uint32_t background;
		uint32_t exhibit_count;
	} back_info_t;

	typedef struct exhibit_info_s
	{
		uint32_t item_id;
		uint32_t exhibit_id;
		uint32_t state;
	}exhibit_info_t;
	back_info_t* back_info;
	exhibit_info_t exhibit_info;
	//第一次进入珍宝库或者进入挖宝地图要向好友发送一份邮件
	uint32_t friend_count;
	uint32_t expect_len = sizeof(back_info_t) + sizeof(friend_count);
	CHECK_BODY_LEN_GE(len, expect_len);
	back_info = (back_info_t*)buf;
	buf += sizeof(back_info_t);
	expect_len += back_info->exhibit_count * sizeof(exhibit_info_t);
	CHECK_BODY_LEN_GE(len, expect_len);
	int j = 0;
	int i;
	int k = sizeof(protocol_t);
	PKG_UINT32(msg, id, k);
	PKG_UINT32(msg, back_info->is_vip, k);
	PKG_UINT32(msg, back_info->exp, k);
	PKG_UINT32(msg, back_info->level, k);
	//更新全局数据
	dungeon_commit_level_to_db(p, p->dungeon_explore_exp);
	PKG_UINT32(msg, back_info->background, k);
	PKG_UINT32(msg, back_info->exhibit_count, k);
	for(i = 0; i < back_info->exhibit_count; ++i)
	{
		UNPKG_H_UINT32(buf, exhibit_info.item_id, j);
		UNPKG_H_UINT32(buf, exhibit_info.exhibit_id, j);
		UNPKG_H_UINT32(buf, exhibit_info.state, j);
		PKG_UINT32(msg, exhibit_info.exhibit_id, k);
		PKG_UINT32(msg, exhibit_info.item_id, k);
		PKG_UINT32(msg, exhibit_info.state, k);
	}

	UNPKG_H_UINT32(buf, friend_count, j);
	uint32_t friend_id;
	expect_len += friend_count * sizeof(friend_id);
	CHECK_BODY_LEN(len, expect_len);
	for(i = 0 ; i< friend_count; ++i)
	{
		UNPKG_H_UINT32(buf, friend_id, j);
		//send_postcard(p->nick, p->id, friend_id, 1000225, "", 0);
	}

	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int dungeon_get_storage_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	uint32_t user_id;
	uint32_t is_illustrated;
	CHECK_BODY_LEN(len, sizeof(user_id) + sizeof(is_illustrated));
	int j = 0;
	UNPKG_UINT32(body, user_id, j);
	UNPKG_UINT32(body, is_illustrated, j);
	CHECK_VALID_ID(user_id);
	uint32_t db_buf[]={is_illustrated};
	return send_request_to_db(SVR_PROTO_DUNGEON_GET_STORAGE_INFO, p, sizeof(db_buf), db_buf, user_id);
}

int dungeon_get_storage_info_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	uint32_t item_kind;
	CHECK_BODY_LEN_GE(len, sizeof(item_kind));
	int j = 0;
	UNPKG_H_UINT32(buf, item_kind, j);
	uint32_t item_id;
	uint32_t item_count;
	CHECK_BODY_LEN(len, sizeof(item_kind) + item_kind * ( sizeof(item_id) + sizeof(item_count) ) );
	int i;
	int k = sizeof(protocol_t);
	PKG_UINT32(msg, id, k);
	PKG_UINT32(msg, item_kind, k);
	for(i = 0; i < item_kind; ++i)
	{
		UNPKG_H_UINT32(buf, item_id, j);
		UNPKG_H_UINT32(buf, item_count, j);
		PKG_UINT32(msg, item_id, k);
		PKG_UINT32(msg, item_count, k);
	}
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int dungeon_set_exhibit_item_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	uint32_t item_id;
	uint32_t item_state;
	uint32_t exhibit_id;
	CHECK_BODY_LEN(len, sizeof(uint32_t) * 3);
	int j = 0;
	UNPKG_UINT32(body, exhibit_id, j);
	UNPKG_UINT32(body, item_id, j);
	UNPKG_UINT32(body, item_state, j);
	uint32_t db_buf[]={exhibit_id, item_id, item_state};
	return send_request_to_db(SVR_PROTO_DUNGEON_SET_EXHIBIT_ITEM, p, sizeof(db_buf), db_buf, p->id);
}

int dungeon_set_exhibit_item_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	typedef struct back_info_s
	{
		uint32_t exhibit_id;
		uint32_t item_id;
		uint32_t state;
	} back_info_t;
	back_info_t* back_info;
	CHECK_BODY_LEN(len, sizeof(back_info_t));
	back_info = (back_info_t*)buf;
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, back_info->exhibit_id, j);
	PKG_UINT32(msg, back_info->item_id, j);
	PKG_UINT32(msg, back_info->state, j);
	init_proto_head(msg, p->waitcmd, j);
	//摆放展示品统计
	if( back_info->state != 0 )
	{
		uint32_t msgbuff[2]= {p->id, 1};
		msglog(statistic_logfile, 0x0408A33E, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	return send_to_self(p, msg, j, 1);
}

int dungeon_swap_exhibit_item_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	uint32_t exhibit_id1, exhibit_id2;
	CHECK_BODY_LEN(len, sizeof(exhibit_id1) + sizeof(exhibit_id2));
	int j = 0;
	UNPKG_UINT32(body, exhibit_id1, j);
	UNPKG_UINT32(body, exhibit_id2, j);
	uint32_t db_buf[]={exhibit_id1, exhibit_id2};
	return send_request_to_db(SVR_PROTO_DUNGEON_SWAP_EXHIBIT_ITEM, p, sizeof(db_buf), db_buf, p->id);
}

int dungeon_swap_exhibit_item_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	typedef struct back_info_s
	{
		uint32_t exhibit_id1;
		uint32_t exhibit_id2;
	} back_info_t;
	CHECK_BODY_LEN(len, sizeof(back_info_t));
	back_info_t* back_info = (back_info_t*)buf;
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, back_info->exhibit_id1, j);
	PKG_UINT32(msg, back_info->exhibit_id2, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int dungeon_get_visitor_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	uint32_t user_id;
	int j = 0;
	UNPKG_UINT32(body, user_id, j);
	CHECK_VALID_ID(user_id);
	return send_request_to_db(SVR_PROTO_DUNGEON_GET_VISITOR_INFO, p, 0, NULL, user_id);
}

int dungeon_get_visitor_info_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	typedef struct visitor_info_s
	{
		uint32_t visitor_id;
		uint32_t time;
	} visitor_info_t;
	uint32_t visitor_count;
	CHECK_BODY_LEN_GE(len, sizeof(visitor_count));
	int j = 0;
	UNPKG_H_UINT32(buf, visitor_count, j);
	CHECK_BODY_LEN(len, sizeof(visitor_count) + visitor_count * sizeof(visitor_info_t));
	return send_request_to_db(SVR_PROTO_DUNGEON_GET_VISITOR_FULL_INFO, p, len, buf, id);
}

int dungeon_get_visitor_full_info_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	typedef struct visitor_info_s
	{
		uint32_t visitor_id;
		uint32_t time;
		uint32_t level;
	} visitor_info_t;
	uint32_t visitor_count;
	CHECK_BODY_LEN_GE(len, sizeof(visitor_count));
	int j = 0;
	UNPKG_H_UINT32(buf, visitor_count, j);
	CHECK_BODY_LEN(len, sizeof(visitor_count) + visitor_count * sizeof(visitor_info_t));
	int k = sizeof(protocol_t);
	PKG_UINT32(msg, visitor_count, k);
	int i;
	visitor_info_t* visitor_info = NULL;
	for(i = 0; i < visitor_count; ++i)
	{
		visitor_info = (visitor_info_t*)(buf + j);
		PKG_UINT32(msg, visitor_info->visitor_id, k);
		PKG_UINT32(msg, visitor_info->time, k);
		PKG_UINT32(msg, visitor_info->level, k);
		j += sizeof(visitor_info_t);
	}
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

inline int dungeon_add_exhibit_visitors(sprite_t *p, userid_t id)
{
	uint32_t db_buf[] = {p->id};
	return send_request_to_db(SVR_PROTO_DUNGEON_ADD_VISITOR, 0, sizeof(db_buf), db_buf, id);
}

inline int dungeon_commit_level_to_db(sprite_t * p,uint32_t level)
{
	return send_request_to_db(SVR_PROTO_DUNGEON_COMMIT_LEVEL, NULL, sizeof(level), &level, p->id);
}

int dungeon_set_background_map_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	uint32_t background_id;
	CHECK_BODY_LEN(len, sizeof(background_id));
	int j = 0;
	UNPKG_UINT32(body, background_id, j);
	return send_request_to_db(SVR_PROTO_DUNGEON_SET_BACKGROUND, p, sizeof(background_id), &background_id, p->id);
}

int dungeon_set_background_map_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	uint32_t background_id;
	CHECK_BODY_LEN(len, sizeof(background_id));
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, *(uint32_t*)buf, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int dungeon_set_exhibit_state_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	uint32_t id, state;
	CHECK_BODY_LEN(len, sizeof(uint32_t) * 2);
	int j = 0;
	UNPKG_UINT32(body, id, j);
	UNPKG_UINT32(body, state, j);
	uint32_t db_buf[]={id, state};
	return send_request_to_db(SVR_PROTO_DUNGEON_SET_EXHIBIT_STATE, p, sizeof(db_buf), db_buf, p->id);
}

int dungeon_set_exhibit_state_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	uint32_t exhibit_id, state;
	CHECK_BODY_LEN(len, sizeof(uint32_t) * 2);
	int j = 0;
	UNPKG_H_UINT32(buf, exhibit_id, j);
	UNPKG_H_UINT32(buf, state, j);
	int k = sizeof(protocol_t);
	PKG_UINT32(msg, exhibit_id, k);
	PKG_UINT32(msg, state, k);
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);

}

int dungeon_get_friend_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	uint32_t count;
	CHECK_BODY_LEN_GE(len, sizeof(uint32_t));
	int j = 0;
	UNPKG_UINT32(body, count, j);
	//好友数太多
	if( count > 201 )
	{
		return send_to_self_error(p, p->waitcmd, -ERR_dungeon_friend_count_too_large, 1);
	}
	uint32_t friend_id;
	CHECK_BODY_LEN(len, sizeof(count) + count * sizeof(friend_id));
	int k = 0;
	char db_buf[1024];
	PKG_H_UINT32(db_buf, count, k);
	int i;
	for(i = 0; i < count; ++i)
	{
		UNPKG_UINT32(body, friend_id, j);
		PKG_H_UINT32(db_buf, friend_id, k);
	}
	return send_request_to_db(SVR_PROTO_DUNGEON_GET_FRIEND_INFO, p, k, db_buf, p->id);
}

int dungeon_get_friend_info_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	typedef struct friend_info_s
	{
		uint32_t friend_id;
		uint32_t level;
	} friend_info_t;
	uint32_t count;
	CHECK_BODY_LEN_GE(len, sizeof(uint32_t));
	int j = 0;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN(len, sizeof(uint32_t) + count * sizeof(friend_info_t));
	int k = sizeof(protocol_t);
	PKG_UINT32(msg, count, k);
	friend_info_t* friend_info = NULL;
	int i;
	for(i = 0; i < count; ++i)
	{
		friend_info = (friend_info_t*)(buf + j);
		PKG_UINT32(msg, friend_info->friend_id, k);
		PKG_UINT32(msg, friend_info->level, k);
		j += sizeof(friend_info_t);
	}
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k , 1);
}

