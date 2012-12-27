/*
 * =====================================================================================
 *
 *       Filename:  Cgf_contest.cpp
 *
 *    Description:  天下第一比武大会情况
 *
 *        Version:  1.0
 *        Created:  05/23/2011 12:45:50 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#include "Cgf_contest.h"
#include "db_error.h"
#include "proto.h"


class Cgf_contest_info::Cgf_contest_total : public Ctable
{
	public:
		Cgf_contest_total(mysql_interface *db)
		   : Ctable(db, "GF_OTHER", "t_gf_contest_team") 
		{

		}

		int get_one(uint32_t  team_id, uint32_t *count)
		{
			GEN_SQLSTR(sqlstr, "select count(*) from %s where team_id=%u",
				   	this->get_table_name(), team_id);
			STD_QUERY_ONE_BEGIN(sqlstr, GF_CONTEST_TEAM_ERR);
				INT_CPY_NEXT_FIELD(*count);
			STD_QUERY_ONE_END();
		}

		int add_team_member_cnt(uint32_t team_id)
		{
			uint32_t count = 0;
			get_one(team_id, &count);

			if (count) {
				GEN_SQLSTR(sqlstr, "update %s set member_cnt=member_cnt + 1 where team_id =%u;",
					this->get_table_name(), team_id);
			} else {
				GEN_SQLSTR(sqlstr, "insert into %s (team_id, member_cnt) values (%u, %u)",
						this->get_table_name(), team_id, 1);
			}

			return this->exec_update_sql(this->sqlstr, GF_CONTEST_TEAM_ERR);
		}

		int add_team_donate_cnt(uint32_t team_id, uint32_t add_cnt) 
		{
			GEN_SQLSTR(sqlstr, "update %s set donate_cnt=donate_cnt+%u where team_id=%u;",
					this->get_table_name(), add_cnt, team_id);
			return this->exec_update_sql(this->sqlstr, GF_CONTEST_TEAM_ERR);
		}

		int get_all_team_info(contest_team_info_t ** out, uint32_t * count)
		{
			GEN_SQLSTR(sqlstr, "select team_id, leader_id, leader_tm, leader_nick, member_cnt,\
				   	donate_cnt from %s", this->get_table_name());
			STD_QUERY_WHILE_BEGIN(this->sqlstr, out, count);
		   		INT_CPY_NEXT_FIELD( (*out+i)->team_id);
				INT_CPY_NEXT_FIELD( (*out+i)->leader_id);
				INT_CPY_NEXT_FIELD( (*out+i)->leader_tm);
				BIN_CPY_NEXT_FIELD( (*out+i)->leader_name, sizeof((*out+i)->leader_name) );
				INT_CPY_NEXT_FIELD( (*out+i)->member_cnt);
				INT_CPY_NEXT_FIELD( (*out+i)->donate_cnt);
			STD_QUERY_WHILE_END();
		}


};


Cgf_contest_info::Cgf_contest_info(mysql_interface * db) 
	:Ctable(db, "GF_OTHER", "t_gf_contest_info") 
{
	total_team_info = new Cgf_contest_total(db);
	
}

/** 
 * @brief 加入 
 * 
 */
int Cgf_contest_info::join_contest_team(uint32_t uid, uint32_t role_time,
	   	gf_join_contest_team_in * in)
{

	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql, (char*)(in->nick_name), NICK_LEN);
	GEN_SQLSTR(sqlstr, "replace into %s (userid, role_regtime, team_id, nick_name, donate_cnt) \
		   	values (%u, %u, %u, '%s', %u);",
		   	this->get_table_name(), uid, role_time, in->team_id, nick_mysql, 0);  
	int ret = exec_update_list_sql(this->sqlstr, GF_CONTEST_INFO_ERR);
	if (ret == SUCC) { 
		return total_team_info->add_team_member_cnt(in->team_id);
	}
	return ret;
}

/** 
 * @brief  拉自己的
 * 
 */
int Cgf_contest_info::get_one_team(uint32_t uid, uint32_t role_tm, uint32_t *team_id,
	   	uint32_t * self_cnt, uint32_t * win_stage) 
{
	GEN_SQLSTR(sqlstr, "select team_id, donate_cnt, win_stage from %s where userid=%u and role_regtime=%u;",
			this->get_table_name(), uid, role_tm);

	STD_QUERY_ONE_BEGIN(sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*team_id);
		INT_CPY_NEXT_FIELD(*self_cnt);
		INT_CPY_NEXT_FIELD(*win_stage);
	STD_QUERY_ONE_END();
}

/** 
 * @brief  捐献
 * 
 * @return 
 */
int Cgf_contest_info::contest_donate_plant(uint32_t uid, uint32_t role_tm, uint32_t add_cnt)
{
	GEN_SQLSTR(sqlstr, "update %s set donate_cnt=donate_cnt+%u where userid=%u and \
		   	role_regtime=%u;", this->get_table_name(), add_cnt, uid, role_tm);

	int ret = exec_update_sql(this->sqlstr, GF_CONTEST_INFO_ERR);
	if (ret == SUCC) {
		uint32_t team_id = 0;
		uint32_t self_donate = 0;
		uint32_t win_stage = 0;
		ret = get_one_team(uid, role_tm, &team_id, &self_donate, &win_stage);
		if (ret == SUCC) {
			return total_team_info->add_team_donate_cnt(team_id, add_cnt);
		} 
	}
	return ret;
}

/** 
 * @brief 拉所有队伍总信息 
 * 
 */
int Cgf_contest_info::get_contest_team_info(contest_team_info_t **teams, uint32_t *count)
{
	return total_team_info->get_all_team_info(teams, count);
}

/** 
 * @brief 拉自己队伍前3信息 
 * 
 */
int Cgf_contest_info::get_contest_donate_info(donate_t ** in, uint32_t team_id, uint32_t * count) 
{
	GEN_SQLSTR(sqlstr, "select userid, role_regtime, nick_name, donate_cnt from \
		   	%s where team_id=%u order by donate_cnt desc limit 3;", this->get_table_name(), team_id);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, in, count);
	   		INT_CPY_NEXT_FIELD( (*in+i)->uid);
			INT_CPY_NEXT_FIELD( (*in+i)->role_tm);
			BIN_CPY_NEXT_FIELD( (*in+i)->nick_name, sizeof((*in)->nick_name) );
			INT_CPY_NEXT_FIELD( (*in+i)->donate_cnt);
	STD_QUERY_WHILE_END();
}


