#include <algorithm>
#include "Cgf_plant.h"
#include "db_error.h"

using namespace std;
//加入
Cgf_plant::Cgf_plant(mysql_interface * db ) 
	:CtableRoute( db,"GF" ,"t_gf_plant","userid")
{ 

}

int Cgf_plant::get_plant_list(userid_t userid, uint32_t role_regtime, uint32_t* p_count, gf_get_plant_list_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select field_id, plant_tm, plant_id, status, status_tm, \
		fruit_cnt, water_tm, speed_per, output_per \
        from %s where userid=%u and role_regtime=%u order by plant_tm;",
		this->get_table_name(userid),userid, role_regtime);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->field_id );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->plant_tm );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->plant_id );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->status );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->status_tm );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->fruit_cnt );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->water_tm );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->speed_per );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->output_per );
	STD_QUERY_WHILE_END();
}

int Cgf_plant::add_plant(userid_t userid, uint32_t role_regtime, uint32_t plant_tm, uint32_t plant_id, uint32_t status, uint32_t status_tm)
{
	GEN_SQLSTR( this->sqlstr, "insert into %s (userid,role_regtime,plant_tm,plant_id,status, status_tm) \
        values (%u, %u, %u, %u, %u, %u);" ,
		this->get_table_name(userid), userid, role_regtime, plant_tm, plant_id, status, status_tm);
	return this->exec_insert_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);	
}

int Cgf_plant::del_plant(userid_t userid,uint32_t role_regtime, uint32_t field_id)
{
    GEN_SQLSTR(this->sqlstr, "delete from %s where userid=%u and role_regtime=%u and field_id=%u",
        this->get_table_name(userid), userid, role_regtime, field_id);

	return this->exec_update_sql(this->sqlstr, ROLE_ID_NOFIND_ERR);
}

int Cgf_plant::set_plant_status(userid_t userid , uint32_t role_regtime, gf_set_plant_status_in* p_in)
{
	GEN_SQLSTR(this->sqlstr,"replace into %s (userid,role_regtime,field_id,\
		plant_id,plant_tm,status,status_tm,fruit_cnt,water_tm,speed_per,output_per) values \
		( %u, %u, %u, \
		%u, %u, %u, %u, %u, %u, %u, %u );",
			this->get_table_name(userid), userid, role_regtime, p_in->field_id, 
			p_in->plant_id, p_in->plant_tm, p_in->status, p_in->status_tm, p_in->fruit_cnt, p_in->water_tm, p_in->speed_per, p_in->output_per);
	return this->exec_update_list_sql( this->sqlstr, SUCC);
}

int Cgf_plant::add_effect_to_all_plants(userid_t userid , uint32_t role_regtime, gf_add_effect_to_all_plants_in* p_in)
{
	GEN_SQLSTR(this->sqlstr,"update %s set speed_per=speed_per+%u, output_per=output_per+%u where userid=%u and role_regtime=%u;",
			this->get_table_name(userid), p_in->speed_per, p_in->output_per, userid, role_regtime);
	return this->exec_update_list_sql( this->sqlstr, SUCC);
}


int Cgf_plant::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}



