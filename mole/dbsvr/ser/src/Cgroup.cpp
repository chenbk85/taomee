/*
 * =====================================================================================
 *
 *       Filename:  Cgroup.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include "Cgroup.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

Cgroup::Cgroup(mysql_interface * db ) 
		:CtableRoute10x10( db,"GROUP","t_group","groupid")
{ 

}
int Cgroup:: insert(uint32_t groupid,group_item_without_id *p_item )
{
	char groupmsg_mysql[mysql_str_len(sizeof (p_item->groupmsg)) ];
	char groupname_mysql[mysql_str_len(sizeof (p_item->groupname) )];
	char memberlist_mysql[mysql_str_len(sizeof (p_item->memberlist)+4)];

	set_mysql_string(groupmsg_mysql,(char*)(p_item->groupmsg),sizeof (p_item->groupmsg)); 
	set_mysql_string(groupname_mysql,(char*)(p_item->groupname),sizeof (p_item->groupname)); 
	set_mysql_string(memberlist_mysql,(char*)(&(p_item->membercount)), 
			p_item->membercount*(sizeof(p_item->memberlist[0]))+4 ); 

	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u,'%s','%s','%s'  )", 
			this->get_table_name(groupid),  
			groupid,
			p_item->type,
			p_item->groupownerid,	
			groupname_mysql,
			groupmsg_mysql,
			memberlist_mysql
	);
	STD_INSERT_RETURN(this->sqlstr,GROUPID_EXISTED_ERR);
}

int Cgroup::get(uint32_t groupid,group_item_without_id *p_out)
{
	sprintf( this->sqlstr, "select type, ownerid, groupname,groupmsg,memberlist \
		   	from %s where groupid=%u ", 
			this->get_table_name(groupid), groupid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, GROUPID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->type);
		INT_CPY_NEXT_FIELD(p_out->groupownerid);
		BIN_CPY_NEXT_FIELD(p_out->groupname,sizeof (p_out->groupname ));
		BIN_CPY_NEXT_FIELD(p_out->groupmsg,sizeof (p_out->groupmsg ));
		BIN_CPY_NEXT_FIELD(&(p_out->membercount),sizeof (p_out->memberlist )+4 );
	STD_QUERY_ONE_END();
}

int Cgroup::get_ownerid(uint32_t groupid, userid_t *p_ownerid)
{
	sprintf( this->sqlstr, "select ownerid \
		   	from %s where groupid=%u ", 
			this->get_table_name(groupid), groupid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, GROUPID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( *p_ownerid );
	STD_QUERY_ONE_END();
}

int Cgroup::get_member(uint32_t groupid,stru_group_member *p_out,userid_t *p_ownerid )
{
	sprintf( this->sqlstr, "select ownerid,memberlist \
		   	from %s where groupid=%u ", 
			this->get_table_name(groupid), groupid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, GROUPID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_ownerid );
		BIN_CPY_NEXT_FIELD(p_out,sizeof (*p_out));
	STD_QUERY_ONE_END();
}



int Cgroup::set_msg(uint32_t groupid, group_msg_item*p_item )
{
	char groupmsg_mysql[mysql_str_len(sizeof (p_item->groupmsg)) ];
	char groupname_mysql[mysql_str_len(sizeof (p_item->groupname) )];

	set_mysql_string(groupmsg_mysql,(char*)(p_item->groupmsg),sizeof (p_item->groupmsg)); 
	set_mysql_string(groupname_mysql,(char*)(p_item->groupname),sizeof (p_item->groupname)); 


	sprintf( this->sqlstr, "update %s set groupname='%s' , groupmsg='%s' \
		   	where groupid=%u  ", 
			this->get_table_name(groupid),  
			groupname_mysql,
			groupmsg_mysql,
			groupid
	);
	STD_SET_RETURN(this->sqlstr,groupid , GROUPID_NOFIND_ERR );	
}

int Cgroup::add_member( uint32_t groupid, userid_t groupownerid, uint32_t memberid,
	  stru_group_member * p_group_memberlist 	)
{
	int ret;
	uint32_t db_groupownerid;
	uint32_t allow_max_count=20; 

	ret=this->get_member( groupid, p_group_memberlist,& db_groupownerid);
	if (ret!=SUCC) return ret;
	if (groupownerid!=db_groupownerid){
		return GROUP_ISNOT_OWNER_ERR;
	}


	ret=add_id_to_list((idlist*)p_group_memberlist, memberid, allow_max_count );
	if (ret!=SUCC){
		if 	(ret==LIST_ID_MAX_ERR ) 
			return  GROUP_MEMBER_COUNT_MAX_ERR;
		else if (ret==LIST_ID_EXISTED_ERR) 
			return GROUP_MEMBERID_EXISTED_ERR;
		else
			return ret;
	}
	
	return this->set_memberlist(groupid, p_group_memberlist );
}

int Cgroup::set_memberlist(uint32_t groupid, stru_group_member *p_item)
{
	char memberlist_mysql[mysql_str_len(sizeof (*p_item) )];
	set_mysql_string(memberlist_mysql,(char*)(&(p_item->membercount)), 
			p_item->membercount*(sizeof(p_item->memberlist[0]))+4); 

	sprintf( this->sqlstr, "update %s set memberlist='%s' \
		   	where groupid= %u  ", 
			this->get_table_name(groupid),  
			memberlist_mysql,
			groupid
	);
	STD_SET_RETURN(this->sqlstr,groupid , GROUPID_NOFIND_ERR );	

}

int Cgroup::del_member( uint32_t groupid, userid_t groupownerid, uint32_t memberid,
	stru_group_member * p_group_memberlist  )
{

	int ret;
	uint32_t db_groupownerid;

	ret=this->get_member( groupid, p_group_memberlist,& db_groupownerid);
	if (ret!=SUCC) return ret;
	if (groupownerid!=db_groupownerid && groupownerid!=0 ){
		return GROUP_ISNOT_OWNER_ERR;
	}

	ret=del_id_from_list((idlist*) p_group_memberlist,memberid );
	if (ret!=SUCC) 
		return GROUP_MEMBERID_NOFIND_ERR;

	return this->set_memberlist(groupid, p_group_memberlist );
}

int Cgroup::del( uint32_t groupid, userid_t groupownerid )
{
	
	sprintf( this->sqlstr, "delete from  %s \
		   	where groupid= %u  and ownerid =%u ", 
			this->get_table_name(groupid),  
			groupid,groupownerid 
	);
	STD_SET_RETURN(this->sqlstr,groupid , GROUP_ISNOT_OWNER_ERR );	
}


