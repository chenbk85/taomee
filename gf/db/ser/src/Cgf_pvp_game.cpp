/*
 * =====================================================================================
 *
 *       Filename:  Cgf_pvp_game.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/25/2011 11:07:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Saga (), saga@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#include <algorithm>
#include "db_error.h"
#include "Cgf_pvp_game.h"


Cgf_pvp_game::Cgf_pvp_game(mysql_interface *db)
		:Ctable(db, "GF_OTHER", "t_gf_taotai_game")
{
}

int Cgf_pvp_game::get_role_pvp_game_info(uint32_t userid, uint32_t role_tm, uint32_t* do_times, uint32_t* win_times,  uint32_t *flower, uint32_t *rank, uint32_t *seg, uint32_t pvp_type)
{
    GEN_SQLSTR(sqlstr, "select do_times, win_times, flower,final_rank,segment from %s where userid=%u and role_regtime=%u and pvp_type=%u;",
        this->get_table_name(), userid, role_tm, pvp_type);
    STD_QUERY_ONE_BEGIN(sqlstr, ROLE_ID_EXISTED_ERR);
        INT_CPY_NEXT_FIELD(*do_times);
		INT_CPY_NEXT_FIELD(*win_times);
		INT_CPY_NEXT_FIELD(*flower);
		INT_CPY_NEXT_FIELD(*rank);
		INT_CPY_NEXT_FIELD(*seg);
    STD_QUERY_ONE_END();
}

int Cgf_pvp_game::get_final_score_rank_list(gf_get_pvp_game_rank_out_item** pData, uint32_t* count ,uint32_t rank)
{
    if (rank == 1) {
        GEN_SQLSTR(sqlstr,"select userid, role_regtime, nick, lv, score from %s where pvp_type=2 and lv<51 and score <> 0 order by score desc limit 100", get_table_name());
    } else if (rank == 2) {
        GEN_SQLSTR(sqlstr,"select userid, role_regtime, nick, lv, score from %s where pvp_type=2 and lv>50 and score <> 0 order by score desc limit 100", get_table_name());
    } else {
        GEN_SQLSTR(sqlstr,"select userid, role_regtime, nick, lv, score from %s where pvp_type=2 and score <> 0 order by score desc limit 100", get_table_name());
    }
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pData, count);
        	INT_CPY_NEXT_FIELD( (*pData+i)->userid);
	        INT_CPY_NEXT_FIELD( (*pData+i)->roletm);
			BIN_CPY_NEXT_FIELD( (*pData+i)->nick, 16);
		    INT_CPY_NEXT_FIELD( (*pData+i)->lv);
	   		INT_CPY_NEXT_FIELD( (*pData+i)->score);
	STD_QUERY_WHILE_END();	
}

int Cgf_pvp_game::calc_pvp_game_score(uint32_t userid,uint32_t role_tm, int d, int win,uint32_t flower,uint32_t type)
{
    uint32_t done = 0, has_win  = 0, has_flower = 0, rank = 0, seg;
    
    if (flower != 0) {
        int ret = this->get_role_pvp_game_info(userid, role_tm, &done, &has_win, &has_flower, &rank, &seg, type);
        if (ret != SUCC) {
            return 0;
        }
    }
    done       += d;
    has_win    += win;
    has_flower += flower;
    DEBUG_LOG("uid=[%u] calc_pvp_game_score [%u %u %u]", userid, done, has_win, has_flower);

    int plus_score = has_win * 3 + has_flower / 5;
    int duce_score = done > has_win ? (done - has_win) : 0;

    return plus_score > duce_score ? (plus_score - duce_score) : 0;
}

int Cgf_pvp_game::insert_pvp_game_info(uint32_t userid, uint32_t role_tm, uint32_t lv, char* nick,
    uint32_t win_flag, uint32_t pvp_type)
{
    char nick_mysql[mysql_str_len(16)];
    set_mysql_string(nick_mysql, nick, 16);
    if (pvp_type == 2) {
        uint32_t score = win_flag ? 3 : 0;
        sprintf( this->sqlstr, "insert into %s (userid,role_regtime,lv,nick,do_times,win_times,score,pvp_type) \
             values (%u, %u, %u, '%s', %u, %u, %u, %u)" ,
            this->get_table_name(), userid, role_tm, lv, nick_mysql, 1, win_flag, score, pvp_type);
    } else {
        sprintf( this->sqlstr, "insert into %s (userid,role_regtime,lv,nick,do_times,win_times,pvp_type) \
             values (%u, %u, %u, '%s', %u, %u, %u)" ,
            this->get_table_name(), userid, role_tm, lv, nick_mysql, 1, win_flag, pvp_type);
    }
	return this->exec_insert_sql (this->sqlstr, ROLE_ID_EXISTED_ERR );	
}

int Cgf_pvp_game::update_pvp_game_info(uint32_t userid, uint32_t role_tm, uint32_t win_flag,
    uint32_t pvp_type)
{
    if (pvp_type == 2) {
        int score = calc_pvp_game_score(userid, role_tm, win_flag ? 0 : 1, win_flag, 0, 2);
        if (win_flag) {
            GEN_SQLSTR(this->sqlstr,"update %s set win_times=win_times+%u, score=%d \
                where userid=%u and role_regtime=%u and pvp_type=%u;",
                this->get_table_name(), win_flag, score, userid, role_tm, pvp_type);
        } else {
            GEN_SQLSTR(this->sqlstr,"update %s set do_times=do_times+1, score=%d \
                where userid=%u and role_regtime=%u and pvp_type=%u;",
                this->get_table_name(), score, userid, role_tm, pvp_type);
        }

    } else {
        if (win_flag) {
            GEN_SQLSTR(this->sqlstr,"update %s set win_times=win_times+%u \
                where userid=%u and role_regtime=%u and pvp_type=%u;",
                this->get_table_name(), win_flag, userid, role_tm, pvp_type);
        } else {
            GEN_SQLSTR(this->sqlstr,"update %s set do_times=do_times+1 \
                where userid=%u and role_regtime=%u and pvp_type=%u;",
                this->get_table_name(), userid, role_tm, pvp_type);
        }
    }
    return this->exec_update_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);
}

int Cgf_pvp_game::set_pvp_game_info(uint32_t userid, uint32_t role_tm, uint32_t lv, uint8_t* nick,
    uint32_t done, uint32_t win, uint32_t pvp_type, uint32_t flower_cnt)
{
    char nick_mysql[mysql_str_len(16)];
    set_mysql_string(nick_mysql, (char*)nick, 16);
    if (pvp_type == 2) {
        uint32_t score = calc_pvp_game_score(userid, role_tm, done, win, 0, 2);
        sprintf( this->sqlstr, "replace into %s (userid,role_regtime,lv,nick,do_times,win_times,score,pvp_type,flower) \
             values (%u, %u, %u, '%s', %u, %u, %u, %u, %u)" ,
            this->get_table_name(), userid, role_tm, lv, nick_mysql, done, win, score, pvp_type, flower_cnt);
    } else {
        sprintf( this->sqlstr, "replace into %s (userid,role_regtime,lv,nick,do_times,win_times,pvp_type) \
             values (%u, %u, %u, '%s', %u, %u, %u)" ,
            this->get_table_name(), userid, role_tm, lv, nick_mysql, done, win, pvp_type);
    }
	return this->exec_insert_sql (this->sqlstr, ROLE_ID_EXISTED_ERR );	
/*
    int ret = update_pvp_game_info(userid, role_tm, lv, nick, win_flag, pvp_type);
    if (ret == ROLE_ID_NOFIND_ERR) {
        ret = insert_pvp_game_info(userid, role_tm, lv, nick, win_flag, pvp_type);
    }
    return ret;
    */
}

int Cgf_pvp_game::insert_pvp_game_flower(uint32_t userid, uint32_t role_tm, uint32_t flower, uint32_t pvp_type)
{
    sprintf( this->sqlstr, "insert into %s (userid,role_regtime,flower,pvp_type) values \
		(%u, %u, %u, %u)" ,
		this->get_table_name(), userid, role_tm, flower, pvp_type);
	return this->exec_insert_sql (this->sqlstr, ROLE_ID_EXISTED_ERR );	
}

int Cgf_pvp_game::update_pvp_game_flower(uint32_t userid, uint32_t role_tm, uint32_t flower, uint32_t pvp_type)
{
    if (pvp_type == 2) {
        int score = calc_pvp_game_score(userid, role_tm, 0, 0, flower, 2);
        GEN_SQLSTR(this->sqlstr,"update %s set flower=flower+%u , score=%d\
            where userid=%u and role_regtime=%u and pvp_type=%u;",
            this->get_table_name(), flower, score, userid, role_tm, pvp_type);
    } else {
        GEN_SQLSTR(this->sqlstr,"update %s set flower=flower+%u \
            where userid=%u and role_regtime=%u and pvp_type=%u;",
            this->get_table_name(), flower, userid, role_tm, pvp_type);
    }
    return this->exec_update_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);
}

int Cgf_pvp_game::set_pvp_game_flower(uint32_t userid, uint32_t role_tm, uint32_t flower, uint32_t pvp_type)
{
    /*
    if (pvp_type == 2) {
        uint32_t score = calc_pvp_game_score(userid, role_tm, 0, 0, flower, 2);
        sprintf( this->sqlstr, "replace into %s (userid,role_regtime,flower,score,pvp_type) \
             values (%u, %u, %u, %u, %u)" ,
            this->get_table_name(), userid, role_tm, flower, score, pvp_type);
    } else {
        sprintf( this->sqlstr, "replace into %s (userid,role_regtime,flower,pvp_type) \
             values (%u, %u, %u, %u)" ,
            this->get_table_name(), userid, role_tm, flower, pvp_type);
    }
	return this->exec_insert_sql (this->sqlstr, ROLE_ID_EXISTED_ERR );	
    */
    int ret = update_pvp_game_flower(userid, role_tm, flower, pvp_type);
    if (ret == ROLE_ID_NOFIND_ERR) {
        ret = insert_pvp_game_flower(userid, role_tm, flower, pvp_type);
    }
    return ret;
}
