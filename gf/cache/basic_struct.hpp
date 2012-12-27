/*
 * =====================================================================================
 *
 *       Filename:  basic_struct.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/11/2012 09:14:06 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#ifndef BASIC_STRUCT_HPP
#define BASIC_STRUCT_HPP

struct team_active_info_t {
	uint32_t active_id;
	uint32_t team_id;
	uint32_t add_score;
	uint32_t out_date_tm;
}__attribute__((packed));

struct team_basic_info_t {
	uint32_t captain_id;
	uint32_t captain_tm;
	uint8_t  captain_nick[16];
	uint8_t  team_nick[16];
	uint32_t member_cnt;
}__attribute__((packed));

struct active_info_t {
	uint32_t active_id;
	uint32_t add_score;
}__attribute__((packed));

struct player_basic_info_t {
	uint32_t role_type;
	uint32_t level;
	char     nick_name[16];
}__attribute__((packed));



#endif


