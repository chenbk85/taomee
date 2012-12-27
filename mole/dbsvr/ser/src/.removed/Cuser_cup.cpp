/*
 * =====================================================================================
 *
 *       Filename:  Cuser_cup.cpp
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
#include "Cuser_cup.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
//#define INIT_SHM_FLAG  -100 
//create
Cuser_cup::Cuser_cup(mysql_interface * db ) :CtableDate( db, "CUP_DB","t_user_cup","userid" )
{ 

	this->cup1_max=config_get_intval("CUP1_MAX",2);
	this->cup2_max=config_get_intval("CUP2_MAX",10);
	this->cup3_max=config_get_intval("CUP3_MAX",20);

	DEBUG_LOG("CUP MAX config%u,%u,%u",this->cup1_max,this->cup2_max,this->cup3_max);
	int key=config_get_intval("CUP_LIST_KEY",19004);
	int shmid;
	if((shmid = shmget(key, sizeof (*(this->cup_list) ) , IPC_CREAT | 0660 )) == -1){
        ERROR_LOG("USER_CUP shm err ");
    }
    this->cup_list=(CUP_MSG_LIST * )shmat(shmid ,NULL, 0);
	memset(this->cup_list,0,sizeof(*(this->cup_list)));



}

int Cuser_cup::update(userid_t userid, CUP_STU *p_cup_stu  )
{
	sprintf( this->sqlstr, "update %s set cup1=%u, cup2=%u, cup3=%u where userid=%u",
	this->get_table_name(time(NULL) ), p_cup_stu->cup1,p_cup_stu->cup2,p_cup_stu->cup3, userid);

	STD_SET_RETURN_EX(this->sqlstr, SUCC);	
}

int Cuser_cup::init(userid_t userid, int cup_type,int value )
{
	CUP_STU cup;
	memset(&cup,0,sizeof(cup));
	this->cal_cup(&cup,cup_type,value);		
	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u,%u)",
	this->get_table_name(time(NULL)),userid,cup.cup1,cup.cup2,cup.cup3 );
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}

int Cuser_cup::get_last_userlist(CUP_MSG_LIST * p_cup_list  )
{
	memcpy(p_cup_list,this->cup_list, sizeof(*p_cup_list ) );
	p_cup_list->index=10;		
	return SUCC;
}

int Cuser_cup::get_cup(userid_t userid, CUP_STU *p_cup_stu  )
{
	memset(p_cup_stu,0,sizeof(*p_cup_stu));
	sprintf( this->sqlstr,"select cup1,cup2,cup3 from %s \
			where  userid=%u " ,
		this->get_table_name(time(NULL)), userid ); 
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_cup_stu->cup1);
		INT_CPY_NEXT_FIELD(p_cup_stu->cup2);
		INT_CPY_NEXT_FIELD(p_cup_stu->cup3);
	STD_QUERY_ONE_END();
}


int Cuser_cup::add_cup(userid_t userid,uint32_t groupid , 
		uint32_t gameid, uint32_t cup_type )
{
	CUP_STU cup;
	int ret;
	//得到原有的值
	memset(&cup,0,sizeof(cup));
	ret=this->get_cup(userid,&cup );			
	
	switch ( ret ){
	case USER_ID_NOFIND_ERR ://没用记录
		ret=this->init(userid,cup_type,1 );
		break;
	case SUCC ://更新原有的记录
		if (this->cal_cup(&cup,cup_type,1)==false){
			return CUP_VALUE_MAX_ERR;	
		}
		ret=this->update(userid, &cup);
		break;
	default :
		return ret;	
		break;
	}
	if (ret==SUCC && cup_type==1){
		DEBUG_LOG("index:%u",this->cup_list->index);
		this->cup_list->index=(this->cup_list->index+1)%10;
		this->cup_list->cup_item[this->cup_list->index].userid=userid;
		this->cup_list->cup_item[this->cup_list->index].gameid=gameid;
		this->cup_list->cup_item[this->cup_list->index].groupid=groupid;
	}
	return ret;
}

inline bool Cuser_cup::cal_cup (CUP_STU *p_cup_stu, int cup_type,int changevalue )
{
	switch ( cup_type ){
		case 1 :
			p_cup_stu->cup1+=changevalue;
			DEBUG_LOG("cup1:%u",p_cup_stu->cup1);
			if (p_cup_stu->cup1>this->cup1_max){ return false; }
			break;
		case 2 :
			p_cup_stu->cup2+=changevalue;
			DEBUG_LOG("cup2:%u",p_cup_stu->cup2);
			if (p_cup_stu->cup2>this->cup2_max){ return false; }
			break;
		case 3 :
			p_cup_stu->cup3+=changevalue;
			DEBUG_LOG("cup3:%u",p_cup_stu->cup3);
			if (p_cup_stu->cup3>this->cup3_max){ return false; }
			break;
		default :
			break;
	}
	return true;
}
