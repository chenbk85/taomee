/*
 * =====================================================================================
 *
 *       Filename:  Cgame.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/08/2009 11:18:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef Cgame_INC
#define Cgame_INC

#include "CtableRoute100.h"
#include "proto.h"
#include "benchapi.h"

/* 
 * @brief 保存修改密码信息的类
 */
class Cgame : public CtableRoute100 {

public:
	Cgame(mysql_interface *db);
	/* 插入修改信息 */

	int insert( stru_game_score * p_in);
	int get(uint32_t gameid,uint32_t userid , uint32_t *p_score);
	int set(stru_game_score *p_in );
	int update(stru_game_score *p_in );
	int update_db(stru_game_score *p_in );
	int get_db(uint32_t gameid,uint32_t userid , uint32_t *p_score);

	int   get_list_by_idlist(uint32_t gameid  , 
		stru_idlist * p_attireidlist, uint32_t *count, game_user_get_list_out_item ** pp_list);

};

#endif
