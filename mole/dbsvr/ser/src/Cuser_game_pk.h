/*
 * =====================================================================================
 *
 *       Filename:  Cuser_game_pk.h
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
#ifndef Cuser_game_pk_INC
#define Cuser_game_pk_INC

#include "CtableRoute100.h"
#include "proto.h"
#include "benchapi.h"

/* 
 * @brief 保存修改密码信息的类
 */
class Cuser_game_pk : public CtableRoute100 {

public:
	Cuser_game_pk(mysql_interface *db);
	int insert(userid_t userid,stru_game_pk *p_in );
	int update_db(userid_t userid,bool win_flag );
	int get(userid_t userid ,stru_game_pk *p_out);
	int get_db(userid_t userid, stru_game_pk *p_out);
	int add_win_lose(userid_t userid,bool win_flag );
	/* 
	int get_db(uint32_t gameid,uint32_t userid , uint32_t *p_score);
	int get(uint32_t gameid,uint32_t userid , uint32_t *p_score);
	int set(stru_game_score *p_in );
	int update(stru_game_score *p_in );
	int update_db(stru_game_score *p_in );

	int get_list(userid_t userid,  
		uint32_t *p_count, game_get_list_out_item **pp_out_item );
	int set(uint32_t gameid,uint32_t userid,uint32_t pk_flag  );


 */
};

#endif
