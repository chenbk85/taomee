/*
 * =====================================================================================
 *
 *       Filename:  Cuser_group_pet.cpp
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

#include "Cuser_group.h"
#include <algorithm>
//user_group
Cuser_group::Cuser_group(mysql_interface * db ) 
	:CtableRoute100x10( db,"USER","t_user_group","userid")
{ 

}
int Cuser_group::insert (userid_t userid, creategroupid_list *p_clist,  group_list *p_list )
{

	char memberlist_mysql[mysql_str_len(sizeof (*p_list) )];
	set_mysql_string(memberlist_mysql,(char*)(&(p_list->count)), 
			p_list->count*(sizeof(p_list->groupidlist[0]))+4); 

	char clist_mysql[mysql_str_len(sizeof (*p_clist) )];
	set_mysql_string(clist_mysql,(char*)p_clist,sizeof(*p_clist)); 

	sprintf( this->sqlstr, "insert into %s values (\
		%u,'%s','%s')", 
			this->get_table_name(userid), 
			userid,
			clist_mysql,
			memberlist_mysql
	   	);
	STD_INSERT_RETURN(this->sqlstr,SUCC );	
}

int Cuser_group::update_grouplist_db(userid_t userid,creategroupid_list *p_clist,  group_list *p_list )
{
	char memberlist_mysql[mysql_str_len(sizeof (*p_list) )];
	set_mysql_string(memberlist_mysql,(char*)(&(p_list->count)), 
			p_list->count*(sizeof(p_list->groupidlist[0]))+4); 

	char clist_mysql[mysql_str_len(sizeof (*p_clist) )];
	set_mysql_string(clist_mysql,(char*)p_clist,sizeof(*p_clist)); 

	sprintf( this->sqlstr, " update %s set \
		creategrouplist='%s', \
		grouplist='%s' \
		where userid=%u " ,
		this->get_table_name(userid), 
		clist_mysql,	
		memberlist_mysql,	
		userid
		);

	STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);	
}

int Cuser_group::update_grouplist(userid_t userid,creategroupid_list *p_clist,  group_list *p_list )
{
	int ret;
	ret=this->update_grouplist_db(userid,p_clist,p_list );
	if ( ret == USER_ID_NOFIND_ERR ){//没有记录
		return  this->insert(userid, p_clist,p_list);
	}else{
		return ret;
	}
}

int Cuser_group::get_grouplist_from_db(userid_t userid ,
	creategroupid_list *p_clist	,group_list *p_list )
{
	int clist_len ,list_len ;
	sprintf( this->sqlstr, "select creategrouplist, grouplist\
		   	from %s where userid=%u ", 
			this->get_table_name(userid ), userid );
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR );
		BIN_CPY_NEXT_FIELD(p_clist,sizeof(*p_clist)  );
		clist_len=res->lengths[0];
		BIN_CPY_NEXT_FIELD(p_list,sizeof (*p_list));
		list_len=res->lengths[1];
	STD_QUERY_ONE_END_WITHOUT_RETURN();
	DEBUG_LOG("%d,%d",clist_len,list_len);
	if (clist_len<=16){
		//旧有的，没有flag字段
		id_list	*p_idlist=(id_list*) p_clist;
		if (p_clist->count>3) p_clist->count=3; 
		for(int32_t i=p_clist->count-1;i>=0;i--){
				p_clist->creategroupid[i].groupid=p_idlist->item[i]	;
				p_clist->creategroupid[i].flag=0;
		}

	}
	DEBUG_LOG("grouplist.count:%u",p_list->count );
	if (p_list->count>0){
		if ((list_len-4)/p_list->count!=8){
			//旧有的，没有flag字段
			id_list	*p_idlist=(id_list*) p_list;
			for(int32_t i=p_list->count-1;i>=0;i--){
					p_list->groupidlist[i].groupid=p_idlist->item[i]	;
					p_list->groupidlist[i].flag=0;
			}
		}
	}
	return SUCC;
}

int Cuser_group::get_grouplist(userid_t userid ,
	   creategroupid_list *p_clist	,group_list * p_list )
{
	memset(p_clist,0,sizeof(p_clist) );
	p_list->count=0;
	int ret;
	ret=this->get_grouplist_from_db(userid,p_clist ,p_list );
	if (ret==DB_ERR){
		return ret;	
	}else{
		return SUCC;	
	}
}

int Cuser_group::jion_group(userid_t userid , uint32_t  groupid )
{
	group_list list;
	creategroupid_list createlist;
	stru_group_item item;

	int ret;
	ret=this->get_grouplist(userid, &createlist,&list);	
	if (ret!=SUCC) return ret;
	item.groupid=groupid;

	ret=add_group_item_to_list(&list, &item , 7 );

	if (ret!=SUCC){
		if 	(ret==LIST_ID_MAX_ERR ) 
			return GROUP_COUNT_MAX_ERR;
		else if (ret==LIST_ID_EXISTED_ERR) 
			return GROUPID_EXISTED_ERR;
		else
			return ret;
	}

	return this->update_grouplist(userid,&createlist,&list ); 
}

int Cuser_group::del_group(userid_t userid, uint32_t  groupid )
{
	group_list list;
	stru_group_item item={};
	creategroupid_list createlist;
	int ret;
	ret=this->get_grouplist(userid, &createlist,&list);	
	if (ret!=SUCC) return ret;
	item.groupid=groupid;

	del_group_item_from_list (&createlist,&item );
	del_group_item_from_list (&list,&item );
	return this->update_grouplist(userid,&createlist ,&list ); 
}


int Cuser_group::add_group(userid_t userid , uint32_t  groupid )
{
	group_list list;
	creategroupid_list createlist;
	stru_group_item item={};
	int ret;
	ret=this->get_grouplist(userid, &createlist,&list);	
	if (ret!=SUCC) return ret;
	item.groupid=groupid;
	/*
	for (uint32_t i=0;i<createlist.count; i++  ) {
		DEBUG_LOG("===%u",createlist.creategroupid[i].groupid )	;
	}
	*/

	ret=add_group_item_to_list(&createlist, &item , CREATE_GROUP_MAX );
	if (ret!=SUCC){
		if 	(ret==LIST_ID_MAX_ERR ) 
			return  GROUP_CREATE_COUNT_MAX_ERR;
		else if (ret==LIST_ID_EXISTED_ERR) 
			return GROUPID_EXISTED_ERR;
		else
			return ret;
	}

	return this->update_grouplist(userid,&createlist,&list ); 
}

int Cuser_group::get_creategroup_list( userid_t userid,  creategroupid_list *p_clist )
{
	int clist_len ;
	sprintf( this->sqlstr, "select creategrouplist \
		   	from %s where userid=%u ", 
			this->get_table_name(userid ), userid );
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR );
		BIN_CPY_NEXT_FIELD(p_clist,sizeof(*p_clist)  );
		clist_len=res->lengths[0];
	STD_QUERY_ONE_END_WITHOUT_RETURN();
	if (clist_len<=16){
		//旧有的，没有flag字段
		id_list	*p_idlist=(id_list*) p_clist;
		if (p_clist->count>3) p_clist->count=3; 
		for(int32_t i=p_clist->count-1; i>=0;i--){
				p_clist->creategroupid[i].groupid=p_idlist->item[i]	;
				p_clist->creategroupid[i].flag=0;
		}
	}
	return SUCC;
}

int Cuser_group::set_group_flag(userid_t userid, stru_group_item *p_group_item )
{
	group_list list;
	creategroupid_list createlist;
	int ret;
	ret=this->get_grouplist(userid, &createlist,&list);	
	if (ret!=SUCC) return ret;
	set_group_item_to_list (&createlist,p_group_item );
	set_group_item_to_list (&list,p_group_item );
	return this->update_grouplist(userid,&createlist,&list ); 
}
