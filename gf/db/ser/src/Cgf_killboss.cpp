#include <algorithm>
#include "db_error.h"
#include "Cgf_killboss.h"

using namespace std;


//¼ÓÈë
Cgf_killboss::Cgf_killboss(mysql_interface * db ) 
	:CtableRoute( db,"GF" ,"t_gf_killboss","userid")
{ 

}

int Cgf_killboss::add_killed_boss(userid_t userid, uint32_t role_regtime, gf_add_killed_boss_in* p_in )
{
	sprintf( this->sqlstr, "insert into %s (userid,role_regtime,bossid) values \
		(%u, %u, %u)" ,
		this->get_table_name(userid), userid, role_regtime, p_in->boss_id);
	return this->exec_insert_sql (this->sqlstr, GF_KILLED_BOSS_EXISTED_ERR );	
}

int Cgf_killboss::get_killed_boss(userid_t userid, uint32_t role_regtime, uint32_t* p_count, 
	gf_get_killed_boss_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select stageid, grade1, grade2, grade3, grade4, grade5, grade6 from %s \
		where userid=%u and role_regtime=%u;", this->get_table_name(userid), userid, role_regtime);
	uint32_t tmp = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->stage_id );
		INT_CPY_NEXT_FIELD( tmp );
		(*pp_list+i)->grade[0] = tmp;
		INT_CPY_NEXT_FIELD( tmp );
		(*pp_list+i)->grade[1] = tmp;
		INT_CPY_NEXT_FIELD( tmp );
		(*pp_list+i)->grade[2] = tmp;
		INT_CPY_NEXT_FIELD( tmp );
		(*pp_list+i)->grade[3] = tmp;
		INT_CPY_NEXT_FIELD( tmp );
		(*pp_list+i)->grade[4] = tmp;
		INT_CPY_NEXT_FIELD( tmp );
		(*pp_list+i)->grade[5] = tmp;
	STD_QUERY_WHILE_END();
}


int Cgf_killboss::get_killed_boss_kf(userid_t userid, uint32_t role_regtime, uint32_t* p_count, 
	gf_get_killed_boss_kf_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select stageid, grade1, grade2, grade3, grade4 from %s \
		where userid=%u and role_regtime=%u;", this->get_table_name(userid), userid, role_regtime);
	uint32_t tmp = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->stage_id );
		INT_CPY_NEXT_FIELD( tmp );
		(*pp_list+i)->grade[0] = tmp;
		INT_CPY_NEXT_FIELD( tmp );
		(*pp_list+i)->grade[1] = tmp;
		INT_CPY_NEXT_FIELD( tmp );
		(*pp_list+i)->grade[2] = tmp;
		INT_CPY_NEXT_FIELD( tmp );
		(*pp_list+i)->grade[3] = tmp;
	STD_QUERY_WHILE_END();
}

int Cgf_killboss::clear_role_killboss(userid_t userid,uint32_t role_regtime)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where userid=%u and role_regtime=%u;",
		this->get_table_name(userid), userid, role_regtime);
	
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

int Cgf_killboss::set_stage_info(userid_t userid, uint32_t role_regtime, gf_set_role_stage_info_in* p_in)
{
	uint32_t grade_lv = 0, grade_first_tm = 0, grade_s_tm = 0, grade_damage;
	int dbret = get_stage_info(userid, role_regtime, p_in, &grade_lv, &grade_first_tm, &grade_s_tm, &grade_damage);
	if ( dbret == GF_KILLED_BOSS_NOTFIND_ERR)
	{
		return insert_stage_info(userid, role_regtime, p_in);
	}
	else if ( dbret != SUCC)
	{
		return dbret;
	}
	time_t now = time (NULL);
	sprintf(this->sqlstr, "update %s set userid=userid", this->get_table_name(userid));

	//grade
	DEBUG_LOG("%u %u", grade_lv, p_in->stage_grade);
	if (!grade_lv || grade_lv > p_in->stage_grade) 
	{
		sprintf(this->sqlstr + strlen(this->sqlstr), ",grade%u=%u", p_in->difficulty, p_in->stage_grade);
	}
	//first time
	DEBUG_LOG("%u ", grade_first_tm);
	if (!grade_first_tm)
	{
		sprintf(this->sqlstr + strlen(this->sqlstr), ",grade%u_first_tm=%u", p_in->difficulty, (uint32_t)now);
	}
	DEBUG_LOG("%u %u", grade_s_tm, p_in->stage_grade);
	//s time
	if (p_in->stage_grade == 1 && !grade_s_tm)
	{
		sprintf(this->sqlstr + strlen(this->sqlstr), ",grade%u_s_tm=%u", p_in->difficulty, (uint32_t)now);
	}
	//s cnt
	if (p_in->stage_grade == 1) 
	{
		sprintf(this->sqlstr + strlen(this->sqlstr), ",grade%u_s_cnt=grade%u_s_cnt+1", p_in->difficulty, p_in->difficulty);
	}
	// total cnt
	sprintf(this->sqlstr + strlen(this->sqlstr), ",grade%u_cnt=grade%u_cnt+1", p_in->difficulty, p_in->difficulty);

	//damage
	DEBUG_LOG("%u %u", p_in->stage_grade, p_in->total_dam);
	if (p_in->total_dam > grade_damage)
	{
		sprintf(this->sqlstr + strlen(this->sqlstr), ",grade%u_damage=%u", p_in->difficulty, p_in->total_dam);
	}
	

	sprintf(this->sqlstr + strlen(this->sqlstr), " where userid=%u and role_regtime=%u and stageid=%u;", 
		userid, role_regtime, p_in->stageid);

	return this->exec_update_sql(this->sqlstr, GF_KILLED_BOSS_NOTFIND_ERR);	
}

int Cgf_killboss::get_stage_info(userid_t userid, uint32_t role_regtime, gf_set_role_stage_info_in* p_in,
	uint32_t* grade_lv, uint32_t* grade_first_tm, uint32_t* grade_s_tm, uint32_t* grade_damage)
{
	GEN_SQLSTR(this->sqlstr,"select grade%u, grade%u_first_tm, grade%u_s_tm, grade%u_damage from %s \
		where userid=%u and role_regtime=%u and stageid=%u;", 
		p_in->difficulty,  p_in->difficulty,  p_in->difficulty,  p_in->difficulty,  
		this->get_table_name(userid), userid, role_regtime, p_in->stageid);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, GF_KILLED_BOSS_NOTFIND_ERR);
		INT_CPY_NEXT_FIELD( *grade_lv );
		INT_CPY_NEXT_FIELD( *grade_first_tm );
		INT_CPY_NEXT_FIELD( *grade_s_tm );
		INT_CPY_NEXT_FIELD( *grade_damage );
	STD_QUERY_ONE_END();	
}

int Cgf_killboss::insert_stage_info(userid_t userid, uint32_t role_regtime, gf_set_role_stage_info_in* p_in)
{
	time_t now = time (NULL);
	// got 'S' level
	if (p_in->stage_grade == 1) 
	{
		sprintf( this->sqlstr, "insert into %s (userid,role_regtime,stageid,bossid,\
			grade%u,grade%u_first_tm,grade%u_s_tm,\
			grade%u_s_cnt,grade%u_cnt,grade%u_damage) values \
			(%u, %u, %u, %u,\
			 %u, %u, %u, \
			 %u, %u, %u)" ,
			this->get_table_name(userid), 
			p_in->difficulty, p_in->difficulty, p_in->difficulty, 
			p_in->difficulty, p_in->difficulty,  p_in->difficulty,
			
			userid, role_regtime, p_in->stageid, p_in->bossid,
			p_in->stage_grade, (uint32_t)now, (uint32_t)now,
			1, 1, p_in->total_dam);
	} 
	else 
	{
		sprintf( this->sqlstr, "insert into %s (userid,role_regtime,stageid,bossid,\
			grade%u,grade%u_first_tm,\
			grade%u_cnt,grade%u_damage) values \
			(%u, %u, %u, %u,\
			 %u, %u, \
			 %u, %u)" ,
			this->get_table_name(userid), 
			p_in->difficulty, p_in->difficulty, p_in->difficulty,
			p_in->difficulty, 
			
			userid, role_regtime, p_in->stageid, p_in->bossid,
			p_in->stage_grade, (uint32_t)now,
			1, p_in->total_dam);
	}
	return this->exec_insert_sql (this->sqlstr, GF_KILLED_BOSS_EXISTED_ERR );	
}


int Cgf_killboss::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

