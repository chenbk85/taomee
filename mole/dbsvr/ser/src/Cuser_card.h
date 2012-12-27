/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_card.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年12月22日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  zheng, zheng@taomee.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_CARD_INCL
#define  CUSER_CARD_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#define CARDGAME_SET_WIN	 1
#define CARDGAME_SET_LOSE 	0

enum rank_exp
{
    rank1_exp = 100,
    rank2_exp = 250,
    rank3_exp = 500,
    rank4_exp = 1000,
    rank5_exp = 2000,
    rank6_exp = 3000,
    rank7_exp = 4000,
    rank8_exp = 5000,
    rank9_exp = 7000
};

#define WIN_BONUS   50
#define LOST_BONUS  10
#define MAX_CARD_EXP_DAY  7000 


class Cuser_card : public CtableRoute100x10 
{
private:
	inline int getCardList_db(userid_t userid, user_cardlist *p_cardlist);
	inline int getCardGameInfo_db(userid_t userid, user_card_info *p_out);
	inline int setCardList_db(userid_t userid, user_cardlist *p_cardlist);
	int setCardList(userid_t userid, user_cardlist *p_cardlist);
	inline int add_lower_card_db(userid_t userid);
	inline int get_grade(uint32_t wincnt,uint32_t lost_cnt  );

public:
	Cuser_card(mysql_interface *db);
	int init(userid_t userid,uint32_t lower_count ,user_cardlist *p_in);
	int getCardGameInfo(userid_t userid, user_card_info *p_out);
	int	addCard(userid_t userid, uint32_t cardid);
	int addWinLose(userid_t userid, uint32_t flag);			// flag: 0 for win; 1 for lose
	int getCardList(userid_t userid, user_cardlist *p_cardlist);
	int add_lower_card(userid_t userid,user_card_add_lower_card_out *p_out );
	int get_enable_count(userid_t userid,user_card_get_enable_count_out *p_out );
	int get_enable_count(user_card_info*p_info);
	int update_win_lose (userid_t userid,user_card_set_win_lose_in *p_in );
	int getCardGameInfo_ex(userid_t userid,user_card_getinfo_out *p_out);
	int getcard_opt(userid_t userid, user_card_opt_info *p_out);
	int check_id(userid_t userid, uint32_t id, uint32_t *p_isexisted );
};

#endif   /* ----- #ifndef CUSER_CARD_INCL  ----- */

