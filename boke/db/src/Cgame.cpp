/*
 * =========================================================================
 *
 *        Filename: Cgame.cpp
 *
 *        Version:  1.0
 *        Created:  2011-05-13 15:52:03
 *        Description:   
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */

#include    "Cgame.h"
#include <time.h>
Cgame::Cgame(mysql_interface * db ) 
	:CtableRoute100x10(db, "POP" , "t_game" , "userid", "gameid")
{

}

int	Cgame::get(userid_t userid, uint32_t gameid, uint32_t& win_time, uint32_t& lose_time, uint32_t& draw_time)
{
	GEN_SQLSTR( this->sqlstr, "select win_time, lose_time, draw_time from %s where userid=%u and gameid=%u", 
				this->get_table_name(userid), userid, gameid);
	STD_QUERY_ONE_BEGIN(this->sqlstr , GAME_NOFIND_ERR  );
		INT_CPY_NEXT_FIELD(win_time);
		INT_CPY_NEXT_FIELD(lose_time);
		INT_CPY_NEXT_FIELD(draw_time);
	STD_QUERY_ONE_END();
}

int	Cgame::insert(userid_t userid, uint32_t gameid, uint32_t win_time, uint32_t lose_time, uint32_t draw_time)
{
	sprintf( this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u)",
		this->get_table_name(userid), userid,gameid,win_time,lose_time,draw_time);
	return this->exec_insert_sql( this->sqlstr, GAME_IS_EXISTED_ERR );
}

int	Cgame::update(userid_t userid, uint32_t gameid, uint32_t win_time, uint32_t lose_time, uint32_t draw_time)
{
	sprintf( this->sqlstr, "update %s set win_time= %u,lose_time=%u, draw_time=%u where userid=%u and gameid=%u ",
		this->get_table_name(userid), win_time,lose_time, draw_time, userid,gameid);
	return this->exec_update_sql( this->sqlstr, GAME_NOFIND_ERR );
}

int	Cgame::add_game_info(userid_t userid,  uint32_t gameid ,uint32_t win_flag)
{
	uint32_t win_time = 0;
	uint32_t lose_time = 0;
	uint32_t draw_time = 0;
	int ret = this->get(userid, gameid, win_time, lose_time, draw_time);
	switch (win_flag) {
	case 0:
		lose_time ++;
		break;
	case 1:
		win_time ++;
		break;
	case 2:
		draw_time ++;
		break;
	}
	
	if (ret == GAME_NOFIND_ERR) {
		ret = this->insert(userid, gameid, win_time, lose_time, draw_time);
	} else {
		ret = this->update(userid, gameid, win_time, lose_time, draw_time);
	}

	return ret;
}

int	Cgame::add_game_info(userid_t userid,  uint32_t gameid ,uint32_t win_time, uint32_t lose_time, uint32_t draw_time)
{
	uint32_t win_time_db = 0;
	uint32_t lose_time_db = 0;
	uint32_t draw_time_db = 0;
	int ret = this->get(userid, gameid, win_time_db, lose_time_db, draw_time_db);

	win_time += win_time_db;
	lose_time += lose_time_db;
	draw_time += draw_time_db;
	
	if (ret == GAME_NOFIND_ERR) {
		ret = this->insert(userid, gameid, win_time, lose_time, draw_time);
	} else {
		ret = this->update(userid, gameid, win_time, lose_time, draw_time);
	}

	return ret;
}

int	Cgame::get_list(userid_t userid, std::vector<game_info_t> &game_list )
{
    GEN_SQLSTR(this->sqlstr, "select gameid,win_time ,lose_time, draw_time from %s where userid=%u ",
            this->get_table_name(userid),userid);
    STD_QUERY_WHILE_BEGIN(this->sqlstr,game_list);
        INT_CPY_NEXT_FIELD(item.gameid);
        INT_CPY_NEXT_FIELD(item.win_time);
        INT_CPY_NEXT_FIELD(item.lose_time);
        INT_CPY_NEXT_FIELD(item.draw_time);
    STD_QUERY_WHILE_END();
}

