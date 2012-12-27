/*
 * =====================================================================================
 *
 *       Filename:  Cuser_game.h
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
#ifndef Cuser_game_INC
#define Cuser_game_INC

#include "CtableRoute100.h"
#include "proto.h"
#include "benchapi.h"

/* 
 * @brief 保存修改密码信息的类
 */
class Cuser_game : public CtableRoute100 {

public:
	Cuser_game(mysql_interface *db);
	int insert(stru_game_score *p_in,uint32_t pk_flag );
	int get_db(uint32_t gameid,uint32_t userid , uint32_t *p_score);
	int get(uint32_t gameid,uint32_t userid , uint32_t *p_score);
	int set(stru_game_score *p_in );
	int update(stru_game_score *p_in );
	int update_db(stru_game_score *p_in );

	int get_list(userid_t userid,  
		uint32_t *p_count, game_get_list_out_item **pp_out_item );
	int set(uint32_t gameid,uint32_t userid,uint32_t pk_flag  );


	int set_pkflag_db(uint32_t gameid,uint32_t userid,uint32_t pk_flag  );

	int set_pkflag(uint32_t gameid,uint32_t userid,uint32_t pk_flag  );

	int get_game_thread(uint32_t gameid, uint32_t *p_game_limit);
};

#endif
