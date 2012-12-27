/*
 * =====================================================================================
 * 
 *       Filename:  Cgame_score.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CGAME_SCORE_INCL
#define  CGAME_SCORE_INCL
#include "Ctable.h"
#include "proto.h"
class Cgame_score :public Ctable{
	protected:
		int insert(uint32_t gameid,userid_t userid, uint32_t score ,char * nick );
	public:
		Cgame_score(mysql_interface * db ); 
		int update_score(uint32_t gameid, userid_t userid, uint32_t score,char * nick  );

		inline int get_score(uint32_t gameid, userid_t userid,uint32_t *p_id, uint32_t *score );
		inline int get_game_min_score(uint32_t gameid, uint32_t *id, uint32_t *score );
		inline int change_user(uint32_t gameid, uint32_t oldid,
				userid_t userid, uint32_t score,char * nick );
		int get_score_list(uint32_t gameid,uint32_t *count, score_list_item ** pp_item );
		int init_score_list(uint32_t gameid,uint32_t count );
		int delete_list(uint32_t gameid);
		int change_score(uint32_t gameid, uint32_t userid, uint32_t score);
};

#endif   /* ----- #ifndef CGAME_SCORE_INCL  ----- */

