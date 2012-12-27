/*
 * =====================================================================================
 *
 *       Filename:  Cuser_connect_pet.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 13时46分59秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include "Cuser_connect.h"
//user_connect
Cuser_connect::Cuser_connect(mysql_interface * db ) 
	:CtableRoute100x10( db,"USER","t_user_connect","userid")
{ 

}

int Cuser_connect::init(userid_t userid, userid_t parentid, userid_t childid)
{
	uint32_t child_count;
	char child_list[CHILD_COUNT_MAX*sizeof(userid_t)];
	char mysql_child_list[mysql_str_len(CHILD_COUNT_MAX*sizeof(userid_t)) ];
	*((userid_t *)child_list)=childid;
	set_mysql_string(mysql_child_list ,child_list,1*sizeof(userid_t)  );

	if (childid==0) child_count=0;
	else child_count=1;


	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u,0,'%s')", 
		this->get_table_name(userid), userid ,parentid,child_count , mysql_child_list );
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_connect::get_info_from_db(userid_t userid, user_connect_get_info_out *p_out )
{

	sprintf( this->sqlstr, "select parentid, childcount,oldchildcount from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			p_out-> parentid=atoi_safe(NEXT_FIELD );
			p_out-> childcount=atoi_safe(NEXT_FIELD );
			p_out->old_childcount=atoi_safe(NEXT_FIELD );
			//当前大使数为总数-原有的大使数
			p_out->childcount-=p_out->old_childcount;
	STD_QUERY_ONE_END();
}
int Cuser_connect::get_info(userid_t userid, user_connect_get_info_out *p_out )
{
	int ret;
	ret=this->get_info_from_db(userid,p_out);
	//DEBUG_LOG("parentid: %u, old_childcount: %u, child_count: %u ", p_out->parentid, p_out->old_childcount, p_out->childcount);
	if (ret==USER_ID_NOFIND_ERR){
	 	p_out->old_childcount=0;		
	 	p_out->childcount=0;		
	 	p_out->parentid=0;
		ret=SUCC;
	}
	return ret;
}

int Cuser_connect::set_parentid(userid_t userid, userid_t parentid)
{
	sprintf( this->sqlstr, " update %s set \
		parentid=%u\
		where userid=%u " ,
		this->get_table_name(userid), 
		parentid, userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser_connect::add_childid(userid_t userid, userid_t childid,
		uint32_t *  p_childcount  )
{
	char child_list[CHILD_COUNT_MAX*sizeof(userid_t)];
	char mysql_child_list[mysql_str_len(CHILD_COUNT_MAX*sizeof(userid_t)) ];
	uint32_t oldchildcount;
	uint32_t childcount;
	sprintf( this->sqlstr, "select oldchildcount,childcount,childlist from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		oldchildcount=atoi_safe(NEXT_FIELD );
		childcount=atoi_safe(NEXT_FIELD );
		memcpy(child_list,NEXT_FIELD ,sizeof (child_list));
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	((userid_t *) child_list)[childcount%CHILD_COUNT_MAX]=childid;	
	childcount++;

	*p_childcount=childcount-oldchildcount ;
	set_mysql_string(mysql_child_list ,child_list, 
			get_valid_value(childcount,0,CHILD_COUNT_MAX)*sizeof(userid_t)  );

	sprintf( this->sqlstr, " update %s set \
		childcount=%u,\
		childlist='%s'\
		where userid=%u " ,
		this->get_table_name(userid), 
		childcount, 
		mysql_child_list,
		 userid  );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

