/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_fip_card_info.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/01/2011 02:06:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_BATTLE_FIP_CARD_INFO
#define CUSER_BATTLE_FIP_CARD_INFO


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_battle_flip_card_info:public CtableRoute100x10
{
	private:

	public:
		Cuser_battle_flip_card_info(mysql_interface *db);

		int get_info(userid_t userid, user_battle_get_flip_info_out_header *p_out,flip_cd_bin_t *other);
		int insert(userid_t userid, user_battle_game_over_caculate_in *p_in);
		int remove(userid_t userid);
		int get_battle_over_info(userid_t userid, game_over_cal_info_t *out, flip_cd_bin_t *other);
};
#endif
