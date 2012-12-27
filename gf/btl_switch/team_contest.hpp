/*
 * =====================================================================================
 *
 *       Filename:  team_contest.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/10/2012 04:57:22 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#ifndef TEAM_CONTEST_HPP
#define TEAM_CONTEST_HPP
extern "C"  {
#include <stdint.h>
}

struct cachesvr_proto_t;


uint32_t get_team_server_id(uint32_t team_id);

uint32_t auto_find_team_server_id(uint32_t team_id);

int get_server_team_info_from_cachesvr();

void clear_out_date_team_contest_info();

void handle_server_info_form_cache_svr(cachesvr_proto_t * pkg);

void init_team_contest_server_info();
#endif


