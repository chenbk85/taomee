/*
 * =====================================================================================
 *
 *       Filename:  Cuser_temp_team.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/20/2012 11:03:30 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_temp_team.h"

Cuser_temp_team::Cuser_temp_team(mysql_interface* db):CtableRoute100x10(db, "USER", "t_user_temp_team", "userid")
{

}
int  Cuser_temp_team::insert(userid_t userid,const char* teamname,uint32_t teamid, uint32_t logo)
{
	   char team_name_mysql[mysql_str_len(16)];
	   set_mysql_string(team_name_mysql,teamname,16);

       sprintf(this->sqlstr, "insert into %s values(%u,'%s',%u,%u,%u,%u)",
			   this->get_table_name(userid),userid, team_name_mysql, teamid,0,1,logo);
	   
	   STD_SET_RETURN_EX(this->sqlstr,USER_ID_EXISTED_ERR);
}
int Cuser_temp_team::add(userid_t userid,const char* teamname,uint32_t teamid, uint32_t logo)
{
	char team_name_mysql[mysql_str_len(16)];
	set_mysql_string(team_name_mysql,teamname,16);
    int ret = update(userid,teamname,teamid,logo);
    if(ret == USER_ID_NOFIND_ERR){
        ret = insert(userid,teamname,teamid,logo);		     
	}
	return ret;
}
int  Cuser_temp_team::get_one(uint32_t userid, const char * col, uint32_t *data)
{
     sprintf(this->sqlstr, "select %s from %s where userid = %u",col,this->get_table_name(userid),userid);
     STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
     *data=atoi_safe(NEXT_FIELD);
     STD_QUERY_ONE_END();
}
int  Cuser_temp_team::set_one(uint32_t userid, const char* col, uint32_t data)
{
	sprintf(this->sqlstr, "update %s set %s =%u where userid=%u " ,
			this->get_table_name(userid),col,data,userid);
	STD_SET_RETURN(this->sqlstr,userid ,USER_ID_NOFIND_ERR );	
}
int  Cuser_temp_team::get_teamname(uint32_t userid,char *data)
{
     sprintf(this->sqlstr, "select teamname from %s where userid = %u",this->get_table_name(userid),userid);
     STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
     BIN_CPY_NEXT_FIELD(data,16);
     STD_QUERY_ONE_END();
}
int  Cuser_temp_team::update(userid_t userid,const char*name,uint32_t id, uint32_t logo)
{
    sprintf(this->sqlstr, "update %s set  teamname = %s,teamid = %u,logo = %u  where userid = %u",this->get_table_name(userid),
			name,id, logo,userid);
	STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);
}
int Cuser_temp_team::update_badge(userid_t userid,uint32_t teamid,uint32_t badge)
{
	sprintf(this->sqlstr, "update %s set  badge = %u where userid = %u and teamid = %u",this->get_table_name(userid),badge,userid,teamid);
    STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);
}
int  Cuser_temp_team::remove(userid_t userid)
{
    sprintf(this->sqlstr, "delete from %s where userid = %u",this->get_table_name(userid),userid);
    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	  
}
int  Cuser_temp_team::get_teamid(userid_t userid,uint32_t* teamid)
{
	sprintf(this->sqlstr, "select teamid from %s where userid = %u",this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
	*teamid = atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END();
}
int  Cuser_temp_team::get_teaminfo(uint32_t userid, user_get_teaminfo_out_header *out)
{
	sprintf(this->sqlstr,"select teamname,teamid,badge,count,logo from %s where userid = %u",this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
	BIN_CPY_NEXT_FIELD(out->name,16);
	INT_CPY_NEXT_FIELD(out->teamid);
	INT_CPY_NEXT_FIELD(out->badge);
	INT_CPY_NEXT_FIELD(out->count);
	INT_CPY_NEXT_FIELD(out->logo);
	STD_QUERY_ONE_END();
}
