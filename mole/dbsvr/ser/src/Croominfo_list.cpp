/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_list.cpp
 *
 *    Description:  把roominfo表内的大字段分解出来
 *
 *        Version:  1.0
 *        Created:  01/19/2010 10:41:18 AM
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra Croominfo_list.cpp
 *
 *         Author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#include"Croominfo_list.h"

Croominfo_list::Croominfo_list(mysql_interface * db )
	:CtableRoute10x10( db, "ROOMINFO","t_roominfo_list","room_id")
{ 
}

int Croominfo_list::insert(userid_t userid)
{
	sprintf(this->sqlstr, "insert into %s values(%u, 0x00000000, 0x00000000, 0x00000000)",
		this->get_table_name(userid),
		userid
	   	);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

int Croominfo_list::pk_add(userid_t userid, roominfo_pk_add_in *p_in )
{
	int ret;
	stru_pk_list pklist = {	};	
	ret=this->get_pk_list(userid,&pklist );
	if(ret!=SUCC) return ret;
	pklist.count++;
	if (pklist.count>PK_LIST_MAX_COUNT){
		pklist.count=PK_LIST_MAX_COUNT;
	}
	for (uint32_t i = 0; i < pklist.count; i++) {
		if ((pklist.items[i].guestid == p_in->guestid) && (pklist.items[i].gameid == p_in->gameid)) {
			if (pklist.items[i].userid_score < p_in->userid_score) {
				pklist.items[i].userid_score = p_in->userid_score;
			}
			pklist.count--;
			return this->update_pk_list(userid, &pklist);
		}
	}
	for(uint32_t i=pklist.count-1;i>0;i-- ){
		memcpy(&(pklist.items[i]),&(pklist.items[i-1]),sizeof(pklist.items[0]));
	}
		

	//第一个数据填上
	memcpy(&(pklist.items[0]),p_in,sizeof(pklist.items[0]));
	//更新
	ret = this->update_pk_list(userid,&pklist);
	if (ret != SUCC) {
		this->insert(userid);
		return this->update_pk_list(userid,&pklist);
	}
	return 0;
}

int Croominfo_list::get_pk_list(userid_t userid, stru_pk_list *p_out )
{
	sprintf( this->sqlstr, "select pk_list \
			from %s where room_id=%d ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD (p_out, sizeof ( *p_out) );
	STD_QUERY_ONE_END();
}

int Croominfo_list::update_pk_list( userid_t userid, stru_pk_list *p_list ) 
{
	char mysql_list[mysql_str_len(sizeof (*p_list))];
	set_mysql_string(mysql_list,(char*)p_list, 
			4+sizeof(p_list->items[0])* p_list->count);
	sprintf( this->sqlstr, " update %s set pk_list ='%s' \
		where  room_id=%u " ,
		this->get_table_name(userid), 
		mysql_list, userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Croominfo_list::get_presentlist(userid_t userid, stru_presentlist *p_out )
{
	memset(p_out,0,sizeof(*p_out));
	sprintf( this->sqlstr, "select presentlist from %s where room_id=%d", 
			 this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD (p_out, sizeof ( *p_out) );
	STD_QUERY_ONE_END();
}

int Croominfo_list::update_presentlist(userid_t userid ,stru_presentlist * p_list )
{
	char mysql_list[mysql_str_len(sizeof (*p_list))];

	set_mysql_string(mysql_list,(char*)p_list, 
			4+sizeof(p_list->items[0])* p_list->count);
	sprintf( this->sqlstr, " update %s set presentlist ='%s' \
		where room_id=%u " ,
		this->get_table_name(userid), 
		mysql_list,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Croominfo_list::presentlist_add(userid_t userid ,stru_present * p_item )
{
	int ret;
	stru_presentlist list;
	ret=this->get_presentlist(userid,&list );
	if(ret!=SUCC) return ret;

	list.count++;
	if (list.count>20) list.count=20;

	uint32_t i=list.count; 
	if (i==20) i=19;
	for (;i>0;i--){
		memcpy(&(list.items[i]),&(list.items[i-1]),sizeof(list.items[0]));
	}
	memcpy(&(list.items[0]),p_item,sizeof(list.items[0]));
	ret = this->update_presentlist(userid,&list ); 
	if (ret != SUCC) {
		this->insert(userid);
		return this->update_presentlist(userid,&list );
	}
	return 0;
}


/**
 * @brief 更新心愿字节 
 * @param usrid 米米号
 * @param p_list 保存心愿的结构体 
 */
int Croominfo_list::update_wish_sql(userid_t userid, stru_roominfo_wish *p_list) 
{
	char mysql_list[mysql_str_len(sizeof (*p_list))];
	set_mysql_string(mysql_list, (char*)p_list, sizeof(*p_list));
	sprintf( this->sqlstr, " update %s set wish = '%s' where  room_id = %u" ,
			this->get_table_name(userid), 
			mysql_list,
			userid
		  );
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}


/**
 * @brief 得到心愿内容
 * @param userid 米米号
 * @param p_out 保存返回的内容
 */
int Croominfo_list::select_wish_sql(userid_t user_id, stru_roominfo_wish *p_out)
{
	sprintf( this->sqlstr, " select wish from %s where room_id=%d", 
			this->get_table_name(user_id),
			user_id
		   );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_out, sizeof(stru_roominfo_socks));
	STD_QUERY_ONE_END();
}


