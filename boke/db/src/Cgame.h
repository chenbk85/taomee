/*
 * =========================================================================
 *
 *        Filename: Cgame.h
 *
 *        Version:  1.0
 *        Created:  2011-05-13 15:29:51
 *        Description:  
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */
#ifndef  CGAME_H
#define  CGAME_H

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"


class Cgame:public CtableRoute100x10
{
	int	insert(userid_t userid,  uint32_t gameid, uint32_t win_time, uint32_t lose_time, uint32_t draw_time);
	int	update(userid_t userid,  uint32_t gameid, uint32_t win_time, uint32_t lose_time, uint32_t draw_time);
	int	get(userid_t userid,  uint32_t gameid, uint32_t& win_time, uint32_t& lose_time, uint32_t& draw_time);
public:
	Cgame (mysql_interface * db);
	int	get_list(userid_t userid, std::vector<game_info_t> &game_list );
	int	add_game_info(userid_t userid,  uint32_t gameid ,uint32_t win_flag ); /* 0 lose 1 win 2 draw */
	int	add_game_info(userid_t userid,  uint32_t gameid ,uint32_t win_time, uint32_t lose_time, uint32_t draw_time ); 
};

#endif  /*CGAME_H*/
