/*
 * =====================================================================================
 *
 *       Filename: 
 *
 *    Description: 排行榜  
 *
 *        Version:  1.0
 *        Created:  10/11/2011 10:46:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus@taomee.com
 *        Company:  Taomee.sh
 *
 * =====================================================================================
 */
#include "Cgf_ranker.h"
#include "db_error.h"
#include "proto.h"

Cgf_ranker_info::Cgf_ranker_info(mysql_interface *db)
		   : Ctable(db, "GF_OTHER", "t_gf_ranker_info") 
{

}


int Cgf_ranker_info::list_ranker_info(uint32_t ranker_id,
									  uint32_t range_type,
									  uint32_t ranker_range,
									  gf_list_ranker_info_out_item ** out,
									  uint32_t * count)
{
	switch (range_type) {
		case range_type_time: 
			GEN_SQLSTR(sqlstr, "select userid, role_regtime, role_type, nick_name, score from %s \
					where ranker_id=%u order by score, role_regtime limit %u ", this->get_table_name(), ranker_id,  ranker_range);
			break;
		case range_type_score:
			GEN_SQLSTR(sqlstr, "select userid, role_regtime, role_type, nick_name, score from %s \
					where ranker_id=%u order by score desc, role_regtime limit %u", this->get_table_name(), ranker_id, ranker_range);
			break;
		default:
			return 0;
	}

	STD_QUERY_WHILE_BEGIN(this->sqlstr, out, count);
	  	INT_CPY_NEXT_FIELD( (*out+i)->userid);
		INT_CPY_NEXT_FIELD( (*out+i)->role_tm);
		INT_CPY_NEXT_FIELD( (*out+i)->role_type);
		BIN_CPY_NEXT_FIELD( (*out+i)->nick_name, sizeof((*out+i)->nick_name) );
		INT_CPY_NEXT_FIELD( (*out+i)->score);
	STD_QUERY_WHILE_END();
}


int Cgf_ranker_info::get_better_count(uint32_t ranker_id,
		                              uint32_t range_type,
									  uint32_t score,
									  uint32_t *better_count)
{

	switch (range_type) {
	case range_type_time:
		GEN_SQLSTR(sqlstr, "select count(*) from %s where ranker_id= %u and score <= %u",
				this->get_table_name(), ranker_id,  score);
		break;
	case range_type_score:
		GEN_SQLSTR(sqlstr, "select count(*) from %s where ranker_id =%u and score >= %u",
				this->get_table_name(), ranker_id,  score);
		break;
	default:
		return 0;
	}

	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*better_count);
	STD_QUERY_ONE_END();
}

int Cgf_ranker_info::get_user_old_score(uint32_t ranker_id,
										uint32_t userid,
				     				    uint32_t role_tm,
					                    uint32_t *score)
{
	GEN_SQLSTR(sqlstr, "select score from %s where ranker_id=%u and userid=%u and role_regtime=%u",
			this->get_table_name(), ranker_id, userid, role_tm);
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*score);
	STD_QUERY_ONE_END();
}

int Cgf_ranker_info::save_user_ranker_info(gf_set_ranker_info_in * in)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql, in->nick_name, NICK_LEN);
	
	GEN_SQLSTR(sqlstr, "replace into %s (userid, role_regtime, role_type, nick_name,\
		ranker_id, score) values (%u, %u, %u,'%s', %u, %u)", this->get_table_name(),
		in->userid, in->role_tm, in->role_type,  nick_mysql, in->ranker_id, in->score);

	return exec_update_list_sql(this->sqlstr, SUCC);

}

int Cgf_ranker_info::del_worst_info(uint32_t ranker_id, uint32_t range_type)
{
	switch (range_type) {
		case range_type_time:
			GEN_SQLSTR(sqlstr, "delete from %s where ranker_id=%u order by score desc limit 1",
					this->get_table_name(), ranker_id); 
			break;
		case range_type_score:
			GEN_SQLSTR(sqlstr, "delete from %s where ranker_id=%u order by score limit 1",
					this->get_table_name(), ranker_id);
			break;
		default:
			return 0;
	}

	return exec_update_list_sql(this->sqlstr, SUCC);
}
		

int Cgf_ranker_info::get_total_count(uint32_t ranker_id, uint32_t * count)
{
	GEN_SQLSTR(sqlstr, "select count(*) from %s where ranker_id=%u",
			   this->get_table_name(), ranker_id);
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();
}


		
