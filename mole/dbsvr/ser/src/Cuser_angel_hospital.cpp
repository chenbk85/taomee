/*
 * =====================================================================================
 *
 *       Filename:  Cuser_angel_hospital.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/22/2011 10:21:09 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include <time.h> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "common.h"
#include "Cuser_angel_hospital.h"

struct angel_recover_st angel_in_hospital[] = {
	{1353403, 4*60},
	{1353405, 4*60},
	{1353407, 2*60},
	{1353408, 2*60},
	{1353409, 3*60},
	{1353410, 3*60},
	{1353411, 2*60},
	{1353412, 2*60},
	{1353413, 2*60},
	{1353415, 36*60},
	{1353417, 36*60},
	{1353418, 8*60},
	{1353422, 22*60},
	{1353424, 24*60},
	{1353427, 18*60},
	{1353429, 12*60},
	{1353431, 12*60},
	{1354035, 24*60},
	{1354037, 24*60},
	{1354039, 24*60},
	{1354041, 12*60},
	{1354043, 24*60},
	{1354045, 18*60},
	{1354047, 36*60},
	{1354059, 12*60},
	{1354061, 8*60},
	{1354063, 18*60},
	{1354065, 18*60},
	{1354067, 24*60},
	{1354069, 8*60},
	{1354071, 16*60},
	{1354073, 12*60},
	{1354075, 36*60},
	{1354077, 18*60},
	{1354079, 24*60},
	{1354081, 24*60},
	{1354083, 12*60},
	{1354085, 15*60},
	{1354087, 12*60},
	{1354089, 24*60},
	{1354090, 36*60},
	{1354091, 12*60},
	{1354093, 12*60},
	{1354095, 12*60},
	{1354097, 12*60},
	{1354098, 12*60},
	{1354100, 24*60},
	{1354102, 36*60},
	{1354104, 12*60},
	{1354106, 12*60},
	{1354108, 12*60},
	{1354110, 12*60},
	{1354112, 24*60},
	{1354114, 24*60},
	{1354116, 12*60},
	{1354118, 8*60},
	{1354120, 6*60},
	{1354122, 12*60},
	{1354124, 12*60},
	{1354126, 12*60},
	{1354128, 8*60},
	{1354130, 12*60},
	{1354132, 12*60},
	{1354134, 36*60},
	{1354135, 36*60},
	{1354136, 12*60},
	{1354138, 12*60},
	{1354140, 24*60},
	{1354142, 36*60},
	{1354144, 12*60},
	{1354146, 36*60},
	{1354148, 36*60},
	{1354150, 36*60},
	{1354154, 36*60},
	{1354156, 36*60},
	{1354159, 24*60},
	{1354161, 12*60},
	{1354163, 12*60},
	{1354165, 12*60},
	{1354167, 12*60},
	{1354169, 24*60},
	{1354171, 36*60},
	{1354173, 12*60},
	{1354175, 24*60},
	{1354177, 12*60},
	{1354179, 8*60},
	{1354181, 36*60},
	{1354183, 12*60},
	{1354187, 18*60},
	{1354189, 36*60},
	{1354191, 12*60},
	{1354193, 18*60},
	{1354195, 18*60},
	{1354199, 24*60},
	{1354201, 18*60},
	{1354197, 18*60},
	{1354203, 36*60},
	{1350041, 36*60},
	{1354211, 36*60},
	{1354213, 24*60},
	{1354215, 36*60},
	{1354217, 48*60},
	{1354218, 36*60},
	{1354220, 36*60},
	{1354226, 36*60},
	{1354229, 36*60},
	{1354231, 36*60},
	{1354233, 36*60},
	{1354235, 36*60},
	{1354237, 36*60},
	{1354238, 36*60},
	{1354240, 36*60},
	{1354242, 36*60},
	{1354244, 36*60},
};

Cuser_angel_hospital::Cuser_angel_hospital(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_angel_hospital", "userid")
{

}

int Cuser_angel_hospital::insert(userid_t userid, uint32_t angelid)
{
	sprintf(this->sqlstr, "insert into %s values(NUll, %u, %u, %u, 0)",
			this->get_table_name(userid),
			userid,
			angelid,
			(uint32_t)time(0)
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_angel_hospital::get_state(userid_t userid, uint32_t id, uint32_t &state)
{
	sprintf(this->sqlstr, "select state from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (state);
	STD_QUERY_ONE_END();
}
int Cuser_angel_hospital::go_out_hospital(userid_t userid, uint32_t id)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_angel_hospital::get_angelid(userid_t userid, uint32_t id, uint32_t &angelid, uint32_t &state)
{
	sprintf(this->sqlstr, "select angelid, state from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (angelid);
		INT_CPY_NEXT_FIELD (state);
	STD_QUERY_ONE_END();
}
int Cuser_angel_hospital::recover(userid_t userid, uint32_t id, uint32_t angelid)
{
	int32_t recover_time = 0;
	get_recover_time(angelid, recover_time);
	recover_time = recover_time * 60;
	sprintf(this->sqlstr, "update %s set state = 1, time = time - %u  where userid = %u and id = %u",
			this->get_table_name(userid),
			recover_time,	
			userid,
			id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_angel_hospital::select(userid_t userid, user_get_angel_in_hospital_out_item **pp_list,
	   	uint32_t *p_count)
{
	sprintf(this->sqlstr, "select id, angelid, time, state from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	int32_t enter_time = 0, recover_time = 0; 
	int32_t tmp = 0;
	int32_t now = time(0);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->angelid);
		INT_CPY_NEXT_FIELD(enter_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->state);
		get_recover_time((*pp_list + i)->angelid, recover_time);
		tmp = ((recover_time > (now - enter_time)/60) ? (recover_time - (now - enter_time)/60) : 0);	
		if((*pp_list + i)->state == 1){
			(*pp_list + i)->health_time = 0;
		}
		else{
			(*pp_list + i)->health_time = tmp ;
		}
	STD_QUERY_WHILE_END();	

}
int Cuser_angel_hospital::get_recover_time(uint32_t angelid, int32_t &time)
{
	for(uint32_t i = 0; i< sizeof(angel_in_hospital)/sizeof(angel_recover_st); ++i){
		if(angel_in_hospital[i].angelid == angelid){
			time = angel_in_hospital[i].recover_time;
			break;
		}
	}
	return SUCC;
}
