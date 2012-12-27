/*
 * =====================================================================================
 *
 *       Filename:  Cmsgboard.cpp
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
#include "Cmsgboard.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
//#define INIT_SHM_FLAG  -100 
//create
Cmsgboard::Cmsgboard(mysql_interface * db ) :Ctable( db, "MSGBOARD_DB","t_msgboard" )
{ 
	int key=config_get_intval("MSGBOARD_KEY",0);
	this->sem.init(key);
	if((shmid = shmget(key, sizeof (MSGBOARD_LIST) , IPC_CREAT | 0660 )) == -1){
		ERROR_LOG("MSGBOARD get shm err ");
	}
	this->cache_list=(MSGBOARD_LIST * )shmat(shmid ,NULL, 0);
	memset(this->cache_list, 0, sizeof (MSGBOARD_LIST) );
}

int  Cmsgboard::update_flag(userid_t msgid,  uint32_t  flag)
{
	sprintf( this->sqlstr, "update %s set flag=%u where id=%u " ,
		this->get_table_name(),flag,msgid );
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int  Cmsgboard::clear_by_delete_flag()
{
	sprintf( this->sqlstr, "delete from %s  where flag=%u " ,
		this->get_table_name(),FLAG_DELETE );
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int Cmsgboard::addhot(uint32_t boardid , uint32_t msgid )
{
	this->update_hot_in_cache(boardid,msgid );	
	sprintf( this->sqlstr, "update %s set hot=hot+1 where id=%u " ,
		this->get_table_name(),msgid);
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int Cmsgboard::check_boardid(uint32_t boardid)
{
	if (boardid<MSGBOARD_LIST_CACHE_MAX ){
		return SUCC; 
	}else{
		return VALUE_OUT_OF_RANGE_ERR; 
	}
}

int Cmsgboard::insert(uint32_t boardid,msgboard_item_max *msg,uint32_t msglen,uint32_t *p_msgid )
{
	char msg_mysql[mysql_str_len(1024)];
	char nick_mysql[mysql_str_len(NICK_LEN)];
	uint32_t flag;
	set_mysql_string(nick_mysql,(msg-> nick), NICK_LEN); 
	set_mysql_string(msg_mysql,(msg->msg),strlen(msg->msg)  ); 
	flag=FLAG_INPUT;
	if ((boardid != 2) && (boardid !=3) && (boardid != 4) && (boardid != 5) ){//不是图 3 母亲节活动
		if ( msglen<=150){
			//当文字少于150个字节，直接设置为删除状态
			flag=FLAG_DELETE;
		}
	}else{//是图片
		int ret;
		ret=this->check_msg(boardid,msg->userid,msg_mysql );
		if (ret==SUCC) return SUCC;   

	}
	
	sprintf( this->sqlstr, "insert into %s values (0,%u,%u,%u,%u,%u,%u,'%s','%s')", 
			this->get_table_name(), 
			flag,
			boardid,
			msg->logdate,
			msg->userid,
			msg->hot,
			msg->color,
			nick_mysql,		
			msg_mysql
	);
	STD_INSERT_GET_ID (this->sqlstr, EMAIL_EXISTED_ERR,*p_msgid );	
}


int Cmsgboard::addmsg_lock(uint32_t boardid, msgboard_item_max *msg,
		uint32_t msg_len, uint32_t *p_msgid  )
{
	int ret;
	this->sem.lock();
		ret=this->insert(boardid,msg, msg_len, p_msgid);
	this->sem.unlock();
	return ret;
}

int Cmsgboard::getmsglist_lock(uint32_t boardid,
		uint32_t pageid, msgboard_get_msglist_out *p_out)
{
	int ret;
	this->sem.lock();
		ret=this->getmsglist(boardid,pageid,p_out);
	this->sem.unlock();
	return ret;
}

int  Cmsgboard::getmsg_by_msgid(uint32_t msgid,uint32_t *p_boardid , msgboard_item_max *p_msg)
{
	memset(p_msg, 0, sizeof(*p_msg));

	sprintf( this->sqlstr,"select id,boardid, logdate,userid,hot,color,nick,msg from %s \
			where  id=%u and flag=%u " ,
		this->get_table_name(), msgid,FLAG_CHECK_OK); 
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, MSGID_NOFIND_ERR);
		p_msg-> msgid = atoi_safe(NEXT_FIELD);
		*p_boardid=atoi_safe(NEXT_FIELD);
		p_msg->logdate = atoi_safe(NEXT_FIELD);
		p_msg->userid= atoi_safe(NEXT_FIELD);
		p_msg->hot= atoi_safe(NEXT_FIELD);
		p_msg->color= atoi_safe(NEXT_FIELD);
		BIN_CPY_NEXT_FIELD(p_msg->nick,NICK_LEN);
		BIN_CPY_NEXT_FIELD(p_msg->msg,sizeof(p_msg->msg));
	STD_QUERY_ONE_END();
}

int  Cmsgboard::check_msg(uint32_t boardid ,uint32_t userid, char * mysql_msg)
{
	sprintf( this->sqlstr,"select id from %s \
			where   userid=%u and boardid=%u and msg='%s' limit 0,1 " ,
		this->get_table_name(), userid,boardid,mysql_msg ); 
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, MSGID_NOFIND_ERR);
	STD_QUERY_ONE_END();
}

int  Cmsgboard::get_msgcount(uint32_t boardid,uint32_t *p_count )
{
	sprintf( this->sqlstr,"select count(1) from %s \
			where  boardid=%u and flag=1 " ,
		this->get_table_name(),boardid ); 
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, MSGID_NOFIND_ERR);
		*p_count= atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END();
}

int Cmsgboard::getmsglist_from_db( uint32_t boardid,uint32_t flag,uint32_t start, uint32_t count, 
		uint32_t *p_count, stru_msgboard_item** pp_list )
{
	sprintf( this->sqlstr,"select id, logdate,userid,hot, color,nick,msg from %s \
			where boardid=%u and flag=%u  order by id desc limit %u,%u" ,
		this->get_table_name(), boardid, flag,start,count); 
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		(*pp_list+i)->msgid = atoi_safe(NEXT_FIELD);
		(*pp_list+i)->logdate =atoi_safe(NEXT_FIELD);
		(*pp_list+i)->userid= atoi_safe(NEXT_FIELD);
		(*pp_list+i)->hot= atoi_safe(NEXT_FIELD);
		(*pp_list+i)->color= atoi_safe(NEXT_FIELD);
		BIN_CPY_NEXT_FIELD((*pp_list+i)->nick,NICK_LEN);
		BIN_CPY_NEXT_FIELD((*pp_list+i)->msg,sizeof((*pp_list+i)->msg));
	STD_QUERY_WHILE_END();
}

int Cmsgboard::getmsglist_by_userid( userid_t userid, uint32_t boardid,
		uint32_t start, uint32_t count, uint32_t *p_count, stru_msgboard_item** pp_list )
{
	sprintf( this->sqlstr,"select id, logdate,userid,hot, color,nick,msg from %s \
			where  userid=%u and  boardid=%u and flag=1 order by id desc limit %u,%u" ,
		this->get_table_name(), userid, boardid ,start,count); 
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		(*pp_list+i)->msgid = atoi_safe(NEXT_FIELD);
		(*pp_list+i)->logdate =atoi_safe(NEXT_FIELD);
		(*pp_list+i)->userid= atoi_safe(NEXT_FIELD);
		(*pp_list+i)->hot= atoi_safe(NEXT_FIELD);
		(*pp_list+i)->color= atoi_safe(NEXT_FIELD);
		BIN_CPY_NEXT_FIELD((*pp_list+i)->nick,NICK_LEN);
		BIN_CPY_NEXT_FIELD((*pp_list+i)->msg,sizeof((*pp_list+i)->msg));
	STD_QUERY_WHILE_END();
}

int Cmsgboard::initlist(uint32_t boardid)
{
 	uint32_t count ; 
	int ret;
	stru_msgboard_item* p_list; 
	MSGBOARD * msgboard ; 
	
	if ((ret=this->check_boardid(boardid ))!=SUCC) return ret;
	msgboard = &(this->cache_list->item[boardid]);


	//得到总条数
	this->get_msgcount(boardid,&(msgboard->count_in_db)  );
	DEBUG_LOG("msg db count[%u]",msgboard->count_in_db);
	//得到cache数据
	ret =this->getmsglist_from_db(boardid,1,
			0, MSG_CACHE_MAX, &count,& p_list);
	if (ret==SUCC){ //cache 到 共享内存中。
		memcpy( msgboard->msgboard_item, p_list, sizeof (*p_list) * count );
		msgboard->startindex=0;
		msgboard->inited=true;
		free (p_list);
	}
	//
	return ret;
}

int Cmsgboard::set_cache_is_inited(uint32_t boardid, bool flag  )
{
	if ( this->check_boardid(boardid )==SUCC ){
		DEBUG_LOG("initflag[%u]",flag );
		this->cache_list->item[boardid].inited=flag;	
		if (!flag){
			DEBUG_LOG("initflag[%u]",flag );
			memset(&(this->cache_list-> item[boardid]), 0, 
					sizeof (this->cache_list-> item[boardid] )  );
		}
		return true;
	}else{
		return false;
	}
}

int Cmsgboard::getmsglist(uint32_t boardid,
		uint32_t pageid, msgboard_get_msglist_out *p_out)
{
	MSGBOARD * msgboard ; 
	int ret;
	int page_itemcount=6;

	if ((ret=this->check_boardid(boardid ))!=SUCC) return ret;
	msgboard = &(this->cache_list->item[boardid]);
	memset(p_out,0, sizeof (*p_out));
	if( pageid<3){//  get from  cache
		int start, end;
		if ( ! msgboard->inited){ //init cache
			if((ret= this->initlist(boardid))!=SUCC){
				return ret;	
			}	
		}

		start=(msgboard->startindex+pageid*page_itemcount)%20;
		end=(start+page_itemcount-1)%20;
		DEBUG_LOG("get list from cache start=%u  end=%u",start,end);
		if (start<end){ //一段复制
			DEBUG_LOG("get list from cache0  userid[%u]",msgboard-> msgboard_item[0].userid );
			memcpy(p_out->item_msgboard,msgboard->msgboard_item+start,
					sizeof (* (msgboard-> msgboard_item)) *page_itemcount );
		}else{//分段复制
			memcpy(p_out->item_msgboard , msgboard->msgboard_item+start,
					sizeof (* (msgboard-> msgboard_item)) *(20-start));
			memcpy(p_out->item_msgboard+(20-start),msgboard->msgboard_item,
					sizeof (* (msgboard-> msgboard_item)) *(end+1));
		}

		p_out->count= get_valid_value(msgboard->count_in_db-page_itemcount*pageid ,0,
				page_itemcount );
		ret=SUCC;	

	}else{ //get from db
		stru_msgboard_item* db_list; 
		ret =this->getmsglist_from_db(boardid,
				1, pageid*page_itemcount, page_itemcount ,&(p_out->count),& db_list);
		DEBUG_LOG("get list from db ret=%u,count=%u",ret,p_out->count);
		if (ret==SUCC){ 
			memcpy(p_out->item_msgboard, db_list ,sizeof (*db_list) * p_out->count );
			free (db_list);
		}
	}

	p_out->count_in_db =msgboard->count_in_db;
	DEBUG_LOG("get db msg count [%u]",p_out->count_in_db);

	return ret;
}

int	Cmsgboard::update_hot_in_cache(uint32_t boardid,uint32_t  msgid )
{
	MSGBOARD * p_msgboard ; 
	int ret;
	if ((ret=this->check_boardid(boardid ))!=SUCC) return ret;
	p_msgboard = &(this->cache_list->item[boardid]);
		
	for (int i=0;i<MSG_CACHE_MAX;i++){
		if (p_msgboard->msgboard_item[i].msgid==msgid){
			p_msgboard->msgboard_item[i].hot++;	
			break;
		}
	}
	return SUCC;
}
int  Cmsgboard::getmsg_by_msgid_nocheckflag(uint32_t msgid,
	   	msgboard_get_msg_nocheck_out *p_msg)
{
	memset(p_msg, 0, sizeof(*p_msg));

	sprintf( this->sqlstr,"select msg from %s \
			where  id=%u " ,
		this->get_table_name(), msgid); 
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, MSGID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_msg->msg,sizeof(p_msg->msg));
	STD_QUERY_ONE_END();
}

int  Cmsgboard::update_msg(uint32_t msgid, char *  msg )
{

	char msg_mysql[mysql_str_len(1024)];
	set_mysql_string(msg_mysql,(msg),strlen(msg)); 
	
	sprintf( this->sqlstr, "update %s set msg='%s' where id=%u " ,
		this->get_table_name(),msg_mysql, msgid );
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

