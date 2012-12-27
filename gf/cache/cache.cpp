/*
 * =====================================================================================
 *
 *       Filename:  cache.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/21/2012 04:24:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
extern "C" {
#include <hiredis/hiredis.h>
#include <stdlib.h>
#include <libtaomee/timer.h>
#include <assert.h>
}
#include <libtaomee/log.h>

#include <libtaomee++/inet/pdumanip.hpp>
#include <kf/utils.hpp>

#include "cli_proto.hpp"
#include "basic_struct.hpp"

using namespace taomee;

#include "cache.hpp"

redisContext *c = NULL;

redisReply *reply = NULL;


int init_redis_connect()
{
    c = redisConnect((char*)"127.0.0.1", 7379);
    if (c->err) {
        printf("Connection error: %s\n", c->errstr);
		return -1;
    }
	return 0;
}

int fini_redis_connect()
{
	return 0;
}


uint32_t get_acitive_out_date_tm(uint32_t active_id)
{
	if (active_id == 12) {
		return get_today_last_sec();
	}
	return 0;
}

void keep_redis_server_connect_alive()
{
	static int last_update_tm = 0;
	if (last_update_tm  != get_now_tv()->tv_sec) {
		last_update_tm = get_now_tv()->tv_sec;
		redisReply * reply = redisCommand(c, "PING");
		if (reply) {
			freeReplyObject(reply);
		}
	}
}

int CacheSvr::report_user_basic_info(svr_proto_t * pkg, fdsession_t * fdsess)
{
//	redisReply * reply = redisCommand(c, "HSET basic_info:%d  %u:%u %b", 
//			pkg->uid % 100, pkg->uid, pkg->role_tm, pkg->body, pkg->len - sizeof(svr_proto_t));
//	freeReplyObject(reply);
//	DEBUG_LOG("Player %u %u report basic info buffer len %u", pkg->uid, pkg->role_tm, pkg->len);
	return 0;
}

int CacheSvr::set_team_contest_server(svr_proto_t * pkg, fdsession_t * fdsess)
{
	int idx = 0;
	uint32_t team_id = 0;
	uint32_t server_id = 0;
	unpack_h(pkg->body, team_id, idx);
	unpack_h(pkg->body, server_id, idx);
//	redisReply * reply = redisCommand(c, "SET team_contest_server:%u %u", team_id, server_id);
//	freeReplyObject(reply);
	DEBUG_LOG("SET TEAM CONTEST SERVER INFO TEAM_ID %u SERVER ID %u", team_id, server_id);

	//Insert to SERVER TEAM SET
	redisReply * reply = redisCommand(c, "SADD server:%u:contest_team %u", server_id, team_id);
	freeReplyObject(reply);

	uint32_t out_time = get_today_last_sec();
	reply = redisCommand(c, "EXPIREAT server:%u:contest_team %u", server_id, out_time);
	freeReplyObject(reply);
	DEBUG_LOG("SET SERVER %u TEAM INFO OUT DATE TIME %u", server_id, out_time);

	return 0;
}

int CacheSvr::get_team_contest_server(svr_proto_t * pkg, fdsession_t * fdsess)
{
	int idx = 0;
	uint32_t start_svr = 0;
	uint32_t end_svr = 0;
	unpack_h(pkg->body, start_svr, idx);
	unpack_h(pkg->body, end_svr, idx);
	DEBUG_LOG("TRY GET SEVR FORM %u TO %u TEAM INFO", start_svr, end_svr);
	if (start_svr && end_svr && (start_svr <= end_svr)) {
		uint32_t cnt = 0;
		idx = sizeof(svr_proto_t);
		int ifx = idx + 4;
		for (uint32_t i = start_svr; i <= end_svr; i++) {
			redisReply * reply = redisCommand(c, "SMembers server:%u:contest_team", i);
			DEBUG_LOG("GET SERVER %u Contest TEAM", i);
			if (reply->type == REDIS_REPLY_ARRAY) {
				for (uint32_t j = 0; j < reply->elements; j++) {
					redisReply * e_reply = reply->element[j];
					pack_h(pkgbuf, i, ifx);
					uint32_t team_id = atoi(e_reply->str);
					pack_h(pkgbuf, team_id, ifx);
					DEBUG_LOG("SERVER LIST ELEMNT[%u] IS %u", j, i);
					cnt++;
				}	
			}
		}

		pack_h(pkgbuf, cnt, idx);
		idx = ifx;
		init_pkg_form_pkg(pkgbuf, pkg, idx);
		return send_pkg(fdsess, pkgbuf, idx);
	}
	return 0;
}

int CacheSvr::player_get_team_contest_server(svr_proto_t * pkg, fdsession_t * fdsess)
{
	int idx = 0;
	uint32_t team_id = 0;
	unpack_h(pkg->body, team_id, idx);

	redisReply * reply = redisCommand(c, "GET team_contest_server:%u", team_id);
	uint32_t server_id = reply->integer;
	freeReplyObject(reply);

	idx = sizeof(svr_proto_t);
	pack_h(pkgbuf, server_id, idx);
	init_pkg_form_pkg(pkgbuf, pkg, idx);
	return send_pkg(fdsess, pkgbuf, idx);
}


void get_cached_team_basic_info(team_basic_info_t * team_info, uint32_t team_id)
{
	redisReply * reply = redisCommand(c, "HGET team_info:%u %u", team_id % 10, team_id);  
	if (reply->type == REDIS_REPLY_STRING) {
		memcpy(team_info, reply->str, sizeof(team_basic_info_t));
	}
	freeReplyObject(reply);	
	//team basic info
}

int get_team_active_score(uint32_t team_id, uint32_t active_id)
{
	redisReply * reply = redisCommand(c, "ZSCORE active:team_set:%u %u", active_id, team_id);
	int score = -1;
	if (reply->type == REDIS_REPLY_STRING) {
		score =  atoi(reply->str);
	}
	freeReplyObject(reply);
	return score;
}

uint32_t get_team_active_rank(uint32_t team_id, uint32_t active_id, uint32_t range_type)
{
	redisReply * reply = NULL;
	if (range_type == 1) {
		reply = redisCommand(c, "ZRANK  active:team_set:%u %u", active_id, team_id);
	} else {
		reply = redisCommand(c, "ZREVRANK active:team_set:%u %u", active_id, team_id);
	}
	
	int rank = -1;
	if (reply && reply->type == REDIS_REPLY_INTEGER) {
		rank = reply->integer;
	}
	freeReplyObject(reply);
	return rank;
}

int CacheSvr::list_team_ranker_info(svr_proto_t * pkg, fdsession_t * fdsess)
{
	struct list_ranker_request_t {
		uint32_t active_id;
		uint32_t range_type; //1: order by , 2:order by desc
		uint32_t start_index;
		uint32_t end_index;
		uint32_t self_team_id;
	}__attribute__((packed));

	list_ranker_request_t * info = reinterpret_cast<list_ranker_request_t*>(pkg->body);
	
	redisReply * reply = NULL;	

	if (info->range_type == 1) {
		reply = redisCommand(c, "ZRANGE active:team_set:%u %u %u WITHSCORES", 
							info->active_id, info->start_index, info->end_index);  
	} else {
		reply = redisCommand(c, "ZREVRANGE active:team_set:%u %u %u WITHSCORES",
							info->active_id, info->start_index, info->end_index);
	}
	int idx = sizeof(svr_proto_t);
	uint32_t cnt = 0;
	if (reply->type == REDIS_REPLY_ARRAY) {
		cnt = reply->elements / 2;
	}
	pack_h(pkgbuf, info->active_id, idx);
	pack_h(pkgbuf, info->start_index, idx);
	pack_h(pkgbuf, info->end_index, idx);
	pack_h(pkgbuf, cnt, idx);
	pack_h(pkgbuf, get_team_active_score(info->self_team_id, info->active_id),  idx);
	pack_h(pkgbuf, get_team_active_rank(info->self_team_id, info->active_id, info->range_type), idx);

	for (uint32_t i = 0; i < reply->elements; i = i + 2) {
		redisReply * team_reply = reply->element[i];
		redisReply * score_reply = reply->element[i+1];
		uint32_t team_id = atoi(team_reply->str);
		uint32_t score_id = atoi(score_reply->str);
		pack_h(pkgbuf, team_id, idx);
		pack_h(pkgbuf, score_id, idx);
		team_basic_info_t team_info;
		get_cached_team_basic_info(&team_info, team_id);
		pack(pkgbuf, &team_info, sizeof(team_basic_info_t), idx);
	}
	freeReplyObject(reply);
	init_pkg_form_pkg(pkgbuf, pkg, idx);
	return send_pkg(fdsess, pkgbuf, idx);
}

int CacheSvr::save_team_active_info(svr_proto_t * pkg, fdsession_t * fdsess) 
{
	team_active_info_t * active_info = reinterpret_cast<team_active_info_t*>(pkg->body); 

	team_basic_info_t * basic_info = reinterpret_cast<team_basic_info_t*>(pkg->body + sizeof(team_active_info_t));
	
	redisReply * reply = redisCommand(c, "HSET team_info:%u %u %b", active_info->team_id % 10, active_info->team_id,  basic_info, sizeof(team_basic_info_t));  
	freeReplyObject(reply);	
	//team basic info
	
	//save for active_map
	
	reply = redisCommand(c, "ZINCRBY active:team_set:%u %u %u", active_info->active_id, active_info->add_score, active_info->team_id);  


	freeReplyObject(reply);
	return 0;
}


int CacheSvr::player_save_active_info(svr_proto_t * pkg, fdsession_t * fdsess)
{
	active_info_t * active_info = reinterpret_cast<active_info_t*>(pkg->body);
	player_basic_info_t * player_info = reinterpret_cast<player_basic_info_t*>(pkg->body + sizeof(active_info_t));

	//resave user basic_info
	redisReply * reply = redisCommand(c, "HSET basic_info:%u  %u:%u %b", 
			pkg->uid % 100, pkg->uid, pkg->role_tm, player_info, sizeof(player_basic_info_t));
	freeReplyObject(reply);

	//add score to active info
	reply = redisCommand(c, "ZINCRBY active:player_set:%u %u %u:%u", active_info->active_id, active_info->add_score,
		   	pkg->uid, pkg->role_tm);  
	freeReplyObject(reply);


	uint32_t tm = get_acitive_out_date_tm(active_info->active_id);
	if (tm) {
//		reply = redisCommand(c, "TTL active:player_set:%u");
//		int ttl = reply->integer;
//		freeReplyObject(reply);
//		if (ttl < 0) {
			reply = redisCommand(c, "EXPIREAT active:player_set:%u %u", active_info->active_id, tm);
			freeReplyObject(reply);
//		}
	}
	return 0;
}

void get_uid_and_tm_by_str(char * str, uint32_t * userid, uint32_t * role_tm)
{
	char *uid_str = strtok(str, ":");
	if (uid_str != NULL) {
		*userid = atoi(uid_str);
		char * role_tm_str = strtok(NULL, ":");
		if (role_tm_str != NULL) {
			*role_tm = atoi(role_tm_str);
		}
	}
}

int get_player_active_score(uint32_t uid, uint32_t role_tm, uint32_t active_id)
{
	redisReply * reply = redisCommand(c, "ZSCORE active:player_set:%u %u:%u", active_id, uid, role_tm);
	int score = -1;
	if (reply->type == REDIS_REPLY_STRING) {
		score =  atoi(reply->str);
	}
	freeReplyObject(reply);
	return score;
}

int get_player_active_rank(uint32_t uid, uint32_t role_tm, uint32_t active_id, uint32_t range_type)
{
	redisReply * reply = NULL;
	if (range_type == 1) {
		reply = redisCommand(c, "ZRANK  active:player_set:%u %u:%u", active_id, uid, role_tm);
	} else {
		reply = redisCommand(c, "ZREVRANK active:player_set:%u %u:%u", active_id, uid, role_tm);
	}
	int rank = -1;
	if (reply && reply->type == REDIS_REPLY_INTEGER) {
		rank = reply->integer;
	}
	freeReplyObject(reply);
	return rank;
}

void get_user_baisc_info(uint32_t uid, uint32_t role_tm, player_basic_info_t * basic_info)
{
	redisReply * reply = redisCommand(c, "HGET basic_info:%u  %u:%u ", 
			uid % 100, uid, role_tm);
	if (reply->type == REDIS_REPLY_STRING) {
		memcpy(basic_info, reply->str, sizeof(player_basic_info_t));
	}
	freeReplyObject(reply);
}


int CacheSvr::player_list_active_ranker_info(svr_proto_t * pkg, fdsession_t * fdsess)
{
	struct list_single_ranker_request_t {
		uint32_t active_id;
		uint32_t range_type; //1: order by , 2:order by desc
		uint32_t start_index;
		uint32_t end_index;
	}__attribute__((packed));

	list_single_ranker_request_t * req = reinterpret_cast<list_single_ranker_request_t*>(pkg->body);

	if (req->range_type == 1) {
		reply = redisCommand(c, "ZRANGE active:player_set:%u %u %u WITHSCORES", 
							req->active_id, req->start_index, req->end_index);  
	} else {
		reply = redisCommand(c, "ZREVRANGE active:player_set:%u %u %u WITHSCORES",
							req->active_id, req->start_index, req->end_index);
	}

	uint32_t info_cnt = 0;
	if (reply->type == REDIS_REPLY_ARRAY) {
		info_cnt = reply->elements / 2;
	}

	int idx = sizeof(svr_proto_t);
	pack_h(pkgbuf, req->active_id, idx);
	pack_h(pkgbuf, req->start_index, idx);
	pack_h(pkgbuf, req->end_index, idx);
	pack_h(pkgbuf, get_player_active_score(pkg->uid, pkg->role_tm, req->active_id), idx);
	pack_h(pkgbuf, get_player_active_rank(pkg->uid, pkg->role_tm, req->active_id, req->range_type), idx);
	pack_h(pkgbuf, info_cnt, idx);
	for (uint32_t i = 0; i < reply->elements; i = i+ 2) {
		redisReply * user_reply = reply->element[i];
		redisReply * score_reply = reply->element[i + 1];
		uint32_t user_score = atoi(score_reply->str);
		uint32_t uid = 0;
		uint32_t role_tm = 0;
		get_uid_and_tm_by_str(user_reply->str, &uid, &role_tm);
		player_basic_info_t basic_info;
		get_user_baisc_info(uid, role_tm, &basic_info);
		pack_h(pkgbuf, uid, idx);
		pack_h(pkgbuf, role_tm, idx);
		pack(pkgbuf, &basic_info, sizeof(basic_info), idx);
		pack_h(pkgbuf, user_score, idx);
	}
	freeReplyObject(reply);
	init_pkg_form_pkg(pkgbuf, pkg, idx);
	return send_pkg(fdsess, pkgbuf, idx);
}

int CacheSvr::reset_player_achieve(svr_proto_t * pkg, fdsession_t * fdsess)
{
	int achieve_point = 0;
	int idx = 0;
	unpack_h(pkg->body, achieve_point, idx);
	int old_rank = get_player_active_rank(pkg->uid, pkg->role_tm, 13, 1);
	uint32_t add_point = 0; 
	if (old_rank == -1) {
		add_point = achieve_point;
	} else {
		int old_point = get_player_active_score(pkg->uid, pkg->role_tm, 13);
		if (achieve_point > old_point) {
			add_point = achieve_point - old_point;
		}
	}

	if (add_point) {

		player_basic_info_t * player_info = reinterpret_cast<player_basic_info_t*>(pkg->body + sizeof(achieve_point));

		//resave user basic_info
		redisReply * reply = redisCommand(c, "HSET basic_info:%u  %u:%u %b", 
				pkg->uid % 100, pkg->uid, pkg->role_tm, player_info, sizeof(player_basic_info_t));
		freeReplyObject(reply);

		//add score to active info
		reply = redisCommand(c, "ZINCRBY active:player_set:%u %u %u:%u", 13,  add_point,
				pkg->uid, pkg->role_tm);  
		freeReplyObject(reply);
	}

	return 0;
}

void rand_magic_no(char * dst_buffer)
{
	static	char ALL_CODES[33] = {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'P', 'A', 'S', 'D', 'F',
		'E', 'G', 'H', 'J', 'L', 'K', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '2', '3', '4', '5',
		'6', '7', '8', '9'};
	for (uint32_t i = 0; i < 10; i++) {
		int idx = rand() % 33;
		dst_buffer[i] = ALL_CODES[idx];
	}
	dst_buffer[10] = '\0';
}

void generate_magic_number(char * dst_buffer, uint32_t uid, uint32_t role_tm)
{
	redisReply * o_reply = NULL;
	uint32_t dst_char = 0;
	while (o_reply == NULL || o_reply->type != REDIS_REPLY_NIL) {
		memset(dst_buffer, 0, 10);
		rand_magic_no(dst_buffer);	
		dst_char = dst_buffer[5];
		if (o_reply) {
			freeReplyObject(o_reply);
		}
		o_reply = redisCommand(c, "HGET generate:magic_number:%u %s", dst_char, dst_buffer);
	}

	if (o_reply) {
		freeReplyObject(o_reply);
	}

	redisReply * n_reply = redisCommand(c, "HSET generate:magic_number:%u %s %u:%u", dst_char, dst_buffer, 
			uid, role_tm);
	freeReplyObject(n_reply);
}	

int CacheSvr::player_generate_magic_number(svr_proto_t * pkg, fdsession_t * fdsess)
{
	int idx = 0;
	uint32_t need_cnt = 0;
	unpack_h(pkg->body, need_cnt, idx);

	idx = sizeof(svr_proto_t);
	pack_h(pkgbuf, need_cnt, idx);
	for (uint32_t i = 0; i < need_cnt; ++i) {
	    char magic_number[11] = {0};
		generate_magic_number(magic_number, pkg->uid, pkg->role_tm);
		pack(pkgbuf, magic_number, 10, idx);
		KDEBUG_LOG(pkg->uid, "player %u | %u generate magic no %s", pkg->uid, pkg->role_tm, magic_number);
	}	
	init_pkg_form_pkg(pkgbuf, pkg, idx);
	return send_pkg(fdsess, pkgbuf, idx);
}

void get_magic_info_by_str(char * str, uint32_t * gen_userid, uint32_t * gen_role_tm, uint32_t * bind_uid)
{
	char *uid_str = strtok(str, ":");
	if (uid_str != NULL) {
		*gen_userid = atoi(uid_str);
		char * role_tm_str = strtok(NULL, ":");
		if (role_tm_str != NULL) {
			*gen_role_tm = atoi(role_tm_str);
			char * bind_uid_str = strtok(NULL, ":");
			if (bind_uid_str != NULL) {
				*bind_uid = atoi(bind_uid_str);
			}
		}
	}
}


int CacheSvr::player_bind_magic_number(svr_proto_t * pkg, fdsession_t * fdsess)
{
	char magic_no[11] = {0};
	int idx = 0;
	unpack(pkg->body, magic_no, 10, idx);
	uint32_t h_seed = magic_no[5];
	magic_no[10] = '\0';

	//check old bind is exist
	redisReply * reply = redisCommand(c, "HGET bind:magic_number:%u %u", pkg->uid % 100, pkg->uid);
	uint32_t err_no = 0;
	if (reply->type != REDIS_REPLY_NIL) {
		err_no = 130020;
	}
	freeReplyObject(reply);
	if (err_no == 0) {
		reply = redisCommand(c, "HGET generate:magic_number:%u %s", h_seed, magic_no);
		if (reply->type == REDIS_REPLY_STRING && err_no == 0) {
			uint32_t gen_uid = 0;
			uint32_t gen_role_tm = 0;
			uint32_t bind_uid = 0;
			get_magic_info_by_str(reply->str, &gen_uid, &gen_role_tm, &bind_uid); 
			if (bind_uid) {
				err_no = 130023;		
			} else {
				redisReply * s_reply = redisCommand(c, "HSET generate:magic_number:%u %s %u:%u:%u",
						h_seed, magic_no, gen_uid, gen_role_tm, pkg->uid);	
				freeReplyObject(s_reply);

				s_reply = redisCommand(c, "HSET bind:magic_number:%u %u %s", pkg->uid % 100, pkg->uid, magic_no);
				freeReplyObject(s_reply);
			}
		} else {
			err_no = 130022;
		}	
		freeReplyObject(reply);
	}

	idx = sizeof(svr_proto_t);
	init_pkg_head_full(pkgbuf, pkg->uid, pkg->role_tm, pkg->cmd, idx,  pkg->seq, err_no);
	return send_pkg(fdsess, pkgbuf, idx);
}

int CacheSvr::get_player_maigc_invitee(svr_proto_t * pkg, fdsession_t * fdsess)
{
	redisReply * reply = redisCommand(c, "HGET bind:magic_number:%u %u", pkg->uid % 100, pkg->uid);
	if (reply->type == REDIS_REPLY_STRING) {
		char magic_no[11] = {0};
		memcpy(magic_no, reply->str, sizeof(magic_no));
		uint32_t h_seed = magic_no[5];
		redisReply * n_reply = redisCommand(c, "HGET generate:magic_number:%u %s", h_seed, magic_no);
		if (n_reply->type == REDIS_REPLY_STRING) {
			uint32_t gen_uid = 0;
			uint32_t gen_role_tm = 0;
			uint32_t bind_uid = 0;
			get_magic_info_by_str(n_reply->str, &gen_uid, &gen_role_tm, &bind_uid); 
			int idx = sizeof(svr_proto_t);
			pack_h(pkgbuf, gen_uid, idx);
			pack_h(pkgbuf, gen_role_tm, idx);
			init_pkg_form_pkg(pkgbuf, pkg, idx);
			return send_pkg(fdsess, pkgbuf, idx);
		}
		freeReplyObject(reply);
	}
	freeReplyObject(reply);
	return 0;
}








