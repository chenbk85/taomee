/*
 * =====================================================================================
 *
 *       Filename:  Cgf_pvp_game.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/01/2011 01:37:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Saga (), saga@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#ifndef __GF_CGF_PVP_GAME_H__
#define __GF_CGF_PVP_GAME_H__

#include "CtableRoute.h"
#include "proto.h" 
#include "benchapi.h"
class Cgf_pvp_game:public Ctable
{
	public:
		Cgf_pvp_game(mysql_interface *db);

        int get_role_pvp_game_info(uint32_t userid, uint32_t role_tm, uint32_t* do_times,uint32_t* win_times, uint32_t * flower, uint32_t *rank, uint32_t *seg, uint32_t pvp_type);

        int get_final_score_rank_list(gf_get_pvp_game_rank_out_item** pData, uint32_t* count ,uint32_t rank);

        int insert_pvp_game_info(uint32_t userid, uint32_t role_tm, uint32_t lv, char* nick,
            uint32_t win_flag, uint32_t pvp_type);
        int update_pvp_game_info(uint32_t userid, uint32_t role_tm, uint32_t win_flag, uint32_t pvp_type);
        int set_pvp_game_info(uint32_t userid, uint32_t role_tm, uint32_t lv, uint8_t* nick, 
            uint32_t done, uint32_t win, uint32_t pvp_type, uint32_t flower_cnt);

        int insert_pvp_game_flower(uint32_t userid, uint32_t role_tm, uint32_t flower, uint32_t pvp_type);
        int update_pvp_game_flower(uint32_t userid, uint32_t role_tm, uint32_t flower, uint32_t pvp_type);
        int set_pvp_game_flower(uint32_t userid, uint32_t role_tm, uint32_t flower, uint32_t pvp_type);

    private:

        int calc_pvp_game_score(uint32_t userid,uint32_t role_tm,int d,int win,uint32_t flower,uint32_t type);
};
#endif


