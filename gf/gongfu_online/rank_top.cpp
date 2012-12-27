/*
 * =====================================================================================
 *
 *       Filename:  rank_top.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/24/2011 01:46:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#include "rank_top.hpp"
#include "player.hpp"
#include "dbproxy.hpp"
#include "cli_proto.hpp"
#include "utils.hpp"
#include "cachesvr.hpp"
#include <kf/utils.hpp>

using namespace taomee;

//range type info: 1 for time, 2 for score
//id = 3 驯鹿活动 ,id = 1 万圣杯， id=4真假武圣实时数据, id=5真假武圣上周比赛数据 
//id = 2 上古仪式, id = 6 九天连环阵, id = 7 红蓝师傅, id=8登录送超灵侠士, id=9 red blue master id = 11 临时队伍精英积分排行
static uint32_t ranker_id_type[14] = {0,
	2, 1, 0, 1, 1, 0, 0, 0, 2, 0, 2, 2, 2
};

//ranker range oder limit, now only 10 or 100
static uint32_t ranker_id_range[14] = {0,
	100, 10, 0, 10, 10, 0, 0, 10, 0, 0, 100, 100, 100
};

//为区分以前的活动， 所有数据存在CacheSvr上的个人活动 ID从101开始，
//索引活动的排行数序时，必须减去100， 如活动101的排序
//type = cache_ranker_id_type[101 - 100]
//
static uint32_t cache_ranker_id_type[11] = {

};


class Ranker {
 public:
  Ranker(uint32_t id, uint32_t range):ranker_recoder(range), ranker_id(id)
  {
  }

  void clear_old_info()
  {
	  ranker_recoder.clear();
	 // ranker_recoder.resize(ranker_id_range[ranker_id]);
  }

  uint32_t get_user_range(uint32_t uid, uint32_t role_tm)
  {
	  uint32_t range = 0;
	  for (std::vector<rank_info_t>::iterator it = ranker_recoder.begin();
			  it != ranker_recoder.end(); ++it) {
		  ++range;
		  if (it->userid == uid && it->role_tm == role_tm) {
			  return range;
		  }
	  }
	  return 0;
  }
  std::vector<rank_info_t> ranker_recoder;
  uint32_t last_update_time;
  uint32_t ranker_id;
};


std::map<uint32_t, Ranker*> RankerMap;

Ranker * find_ranker_by_id(uint32_t ranker_id)
{
	std::map<uint32_t, Ranker*>::iterator it = ::RankerMap.find(ranker_id);
	if (it != ::RankerMap.end()) {
		return (it->second);
	}

	return NULL;
}

typedef std::vector<rank_info_t>::iterator RankIterator;

void cache_ranker_info(Ranker * ranker, rank_info_t * begin, rank_info_t * end)
{
	ranker->clear_old_info();
	while (begin != end) {
		rank_info_t  tmp = *begin;
		(ranker->ranker_recoder).push_back(tmp);
		++begin;
	}

	ranker->last_update_time = get_now_tv()->tv_sec;
}


int db_insert_rank_info(player_t * p, uint32_t rank_id, rank_info_t * info)
{
	int idx = 0;
	pack_h(dbpkgbuf, rank_id, idx);
	if (ranker_id_type[rank_id]) {
		pack_h(dbpkgbuf, ranker_id_type[rank_id], idx);
	} else {
		ERROR_LOG("INSERT %u INTO NOT EXIST RANKER ID %u", p->id, rank_id); 
		return 0;
	}
	pack_h(dbpkgbuf, info->userid, idx);
	pack_h(dbpkgbuf, info->role_tm, idx);
	pack_h(dbpkgbuf, info->role_type, idx);
	pack(dbpkgbuf, info->nick_name, max_nick_size, idx);
	pack_h(dbpkgbuf, info->score, idx);
    return send_request_to_db(0, p->id, p->role_tm, dbproto_insert_ranker, dbpkgbuf, idx);
		
}

int db_list_ranker(player_t * p, uint32_t ranker_id)
{
	int idx = 0;
	uint32_t ranker_type = ranker_id_type[ranker_id];
	uint32_t ranker_range = ranker_id_range[ranker_id];

	pack_h(dbpkgbuf, ranker_id, idx);
	pack_h(dbpkgbuf, ranker_type, idx);
	pack_h(dbpkgbuf, ranker_range, idx);

	if (p) {
		return send_request_to_db(p, p->id, p->role_tm, dbproto_list_ranker, dbpkgbuf, idx);
	} else {
		return send_request_to_db(NULL, 0, 0, dbproto_list_ranker, dbpkgbuf, idx);
	}
}

int send_ranker_info_to_player(player_t * p, Ranker * ranker)
{
	int idx = sizeof(cli_proto_t);
	uint32_t count = ranker->ranker_recoder.size();
	pack(pkgbuf, ranker->ranker_id, idx);
	pack(pkgbuf, count, idx);
	for (uint32_t i = 0; i < count; ++i) {
		rank_info_t & info = ranker->ranker_recoder[i];
		pack(pkgbuf, info.userid, idx);
		pack(pkgbuf, info.role_tm, idx);
		pack(pkgbuf, info.role_type, idx);
		pack(pkgbuf, info.nick_name, max_nick_size, idx);
		pack(pkgbuf, info.score, idx);
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p ,pkgbuf, idx, 1);
}

int player_list_ranker_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	uint32_t ranker_id = 0;
	int idx = 0;
	unpack(body, ranker_id, idx);

	//must edit the ranker_id_type and ranker_id_range befor adding a new ranker list
	if (ranker_id > 12 || !ranker_id_type[ranker_id]  || !ranker_id_range[ranker_id]) {
		ERROR_LOG("invaild ranker range %u", p->id);
		return -1;
	}

	Ranker * ranker = find_ranker_by_id(ranker_id);

	uint32_t now_time = get_now_tv()->tv_sec;

	if (ranker && (ranker->last_update_time + 2 * ranker_refresh_time > now_time)) {
		return send_ranker_info_to_player(p, ranker);
	} else {
		return db_list_ranker(p, ranker_id);
	}	
}

int db_list_ranker_callback(player_t *p, uint32_t uid, void * body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t count = 0;
	uint32_t ranker_id = 0;
	int idx = 0;
	unpack_h(body, ranker_id, idx);
	unpack_h(body, count, idx);

	CHECK_VAL_EQ(bodylen, (sizeof(rank_info_t) * count + 8));

	rank_info_t *begin = (reinterpret_cast<rank_info_t*>((char*)body + 8));

	Ranker * ranker = find_ranker_by_id(ranker_id);
	if (!ranker) {
		ranker = new Ranker(ranker_id, ranker_id_range[ranker_id]);
		RankerMap.insert(std::map<uint32_t, Ranker*>::value_type(ranker_id, ranker));
	}


	cache_ranker_info(ranker, begin, begin + count);

	return send_ranker_info_to_player(p, ranker);
}

void proc_auto_ranker_event()
{
	static uint32_t last_update_tm = 0;
	uint32_t now_time = get_now_tv()->tv_sec;
	if (last_update_tm == 0) {
		last_update_tm = now_time;
	} else if (last_update_tm != now_time) { 
		last_update_tm = now_time;
		for (std::map<uint32_t, Ranker *>::iterator it = RankerMap.begin();
					it != RankerMap.end(); ++it) {
			if ((it->second)->last_update_time +  5 * ranker_refresh_time < now_time) {
				db_list_ranker(0, (it->second)->ranker_id);
			}
		}
	}
}

int proc_auto_list_rank_info_callback(void * body, uint32_t bodylen, uint32_t ret)
{
	if (!ret && bodylen >= 8) {
		uint32_t count = 0;
		uint32_t ranker_id = 0;
		int idx = 0;
		unpack_h(body, ranker_id, idx);
		unpack_h(body, count, idx);

		DEBUG_LOG("AUTO GET RANKER %u INFO FROM DB COUNT %u", ranker_id, count);  

		if (bodylen != (sizeof(rank_info_t) * count + 8) ) {
			return 0;
		}

		rank_info_t *begin = (reinterpret_cast<rank_info_t*>((char*)body + 8));

		Ranker * ranker = find_ranker_by_id(ranker_id);
		if (ranker) {
			cache_ranker_info(ranker, &begin[0], &begin[count]);   
		} 
	}
	return 0;
}


int get_player_ranker_range(player_t *p, uint32_t ranker_id)
{
	Ranker * ranker = find_ranker_by_id(ranker_id);
	if (ranker) {
		return ranker->get_user_range(p->id, p->role_tm);
	} 
	return 0;
}

void init_player_rank(player_t * p, rank_info_t * rank, uint32_t score)
{
	rank->role_tm = p->role_tm;
	rank->userid = p->id;
	rank->score = score;
	rank->role_type = p->role_type;
	memcpy(rank->nick_name, p->nick, max_nick_size);
}


int db_save_rank_info(player_t * p, uint32_t acitve_id, uint32_t score)
{
	rank_info_t info; 
	info.userid = p->id;
	info.role_tm = p->role_tm;
	info.role_type = p->role_type;
	memcpy(info.nick_name, p->nick,  max_nick_size);
	info.score = score;

	return db_insert_rank_info(p, acitve_id, &info);

}

static uint32_t team_ranker_id_type[13] = {0,
	2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2  
};

#define UNPACK_INT(buf, val, idx) \
	int val = 0; \
	unpack(buf, val, idx)

#define UNPACK_H_INT(buf, val, idx) \
	int val = 0;\
	unpack_h(buf, val, idx)



int list_team_active_ranker_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	int idx = 0;
	UNPACK_INT(body, active_id, idx);
	UNPACK_INT(body, range_start, idx);
	UNPACK_INT(body, range_end, idx);

	if (team_ranker_id_type[active_id] == 0) {
		return -1;
	}	

	idx = 0;
	char buf[12] = {0};
	pack_h(buf, active_id, idx);
	pack_h(buf, team_ranker_id_type[active_id], idx);
	pack_h(buf, range_start, idx);
	pack_h(buf, range_end, idx);
	pack_h(buf, p->team_info.team_id, idx);

	return send_request_to_cachesvr(p, p->id, p->role_tm, cache_list_team_active_ranker, buf, idx);
}

void cache_player_active_rank(player_t * p, uint32_t active_id, int rank)
{
	ranker_range_t range;
	range.active_id = active_id;
	range.range = rank;
	if (active_id > 10000) {
		p->p_range_info->team_active_range->insert(std::map<uint32_t, ranker_range_t>::value_type(active_id, range));
	} else {
		p->p_range_info->single_active_range->insert(std::map<uint32_t, ranker_range_t>::value_type(active_id, range));
	}
}

int list_team_active_ranker_callback(player_t * p, cachesvr_proto_t * data)
{
	int idx = 0;
	UNPACK_H_INT(data->body, active_id, idx);
	UNPACK_H_INT(data->body, start_range, idx);
	UNPACK_H_INT(data->body, end_range, idx);
	UNPACK_H_INT(data->body, cnt, idx);
	UNPACK_H_INT(data->body, self_score, idx);
	UNPACK_H_INT(data->body, self_rank, idx);
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, active_id, idx);
	pack(pkgbuf, self_score, idx);
	pack(pkgbuf, self_rank, idx);
    pack(pkgbuf, cnt, idx);	

	//reset player team_set info
	cache_player_active_rank(p, active_id + 10000, self_rank);


	CHECK_VAL_EQ(data->len, sizeof(team_rank_info_t) * cnt + 24 + sizeof(cachesvr_proto_t));
	for (int i = 0; i < cnt; i++) {
		team_rank_info_t * info = reinterpret_cast<team_rank_info_t *>(data->body + 24 + i * sizeof(team_rank_info_t));
		pack(pkgbuf, info->team_id, idx);
		pack(pkgbuf, info->team_nick, max_nick_size, idx);
		pack(pkgbuf, info->captain_id, idx);
		pack(pkgbuf, info->captain_role_tm, idx);
		pack(pkgbuf, info->captain_nick, max_nick_size, idx);
		pack(pkgbuf, info->team_member_cnt, idx);
		pack(pkgbuf, info->team_score, idx);
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

//active > 10000 for team_active
int get_player_active_range(player_t * p, uint32_t active_id)
{
	if (active_id > 10000) {
		uint32_t team_active_id = active_id - 10000;
		std::map<uint32_t, ranker_range_t>::iterator it = p->p_range_info->team_active_range->find(team_active_id);
		if (it != p->p_range_info->team_active_range->end()) {
			return (it->second).range;
		}
	} else {
		std::map<uint32_t, ranker_range_t>::iterator it = p->p_range_info->single_active_range->find(active_id);
		if (it != p->p_range_info->single_active_range->end()) {
			return (it->second).range;
		}
	}
	return -1;
}

int list_single_active_ranker_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	int idx = 0;
	UNPACK_INT(body, active_id, idx);
	UNPACK_INT(body, range_start, idx);
	UNPACK_INT(body, range_end, idx);

	if (ranker_id_type[active_id] == 0) {
		return -1;
	}	

	idx = 0;
	pack_h(dbpkgbuf, active_id, idx);
	pack_h(dbpkgbuf, ranker_id_type[active_id], idx);
	pack_h(dbpkgbuf, range_start, idx);
	pack_h(dbpkgbuf, range_end, idx);
	return send_request_to_cachesvr(p, p->id, p->role_tm, cache_list_single_active_ranker, dbpkgbuf, idx);
}

int list_single_active_ranker_callback(player_t *p, cachesvr_proto_t * data)
{
	int idx = 0;
	UNPACK_H_INT(data->body, active_id, idx);
	UNPACK_H_INT(data->body, start_range, idx);
	UNPACK_H_INT(data->body, end_range, idx);
	UNPACK_H_INT(data->body, self_score, idx);
	UNPACK_H_INT(data->body, self_rank, idx);
	UNPACK_H_INT(data->body, cnt, idx);

	CHECK_VAL_EQ(data->len - sizeof(cachesvr_proto_t), 24 + cnt * sizeof(single_rank_info_t));
	//reset player team_set info
	cache_player_active_rank(p, active_id, self_rank);

	idx = sizeof(cli_proto_t);
	pack(pkgbuf, active_id, idx);
	pack(pkgbuf, self_score, idx);
	pack(pkgbuf, self_rank, idx);
	pack(pkgbuf, cnt, idx);

	for (int i = 0; i < cnt; ++i) {
		single_rank_info_t * rank = reinterpret_cast<single_rank_info_t*>(data->body + 24 + i * sizeof(single_rank_info_t));
		pack(pkgbuf, rank->userid, idx);
		pack(pkgbuf, rank->role_tm, idx);
		pack(pkgbuf, rank->role_type, idx);
		pack(pkgbuf, rank->level, idx);
		pack(pkgbuf, rank->nick_name, max_nick_size, idx);
		pack(pkgbuf, rank->score, idx);
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


void add_player_active_score(player_t * p, uint32_t active_id, uint32_t add_score)
{
	int idx = 0;
	pack_h(dbpkgbuf, active_id, idx);
	pack_h(dbpkgbuf, add_score, idx);
	pack_h(dbpkgbuf, p->role_type, idx);
	pack_h(dbpkgbuf, (int)(p->lv), idx);
	pack(dbpkgbuf, p->nick, max_nick_size, idx);
	send_request_to_cachesvr(p, p->id, p->role_tm, 	cache_save_single_active_info, dbpkgbuf, idx);
}



