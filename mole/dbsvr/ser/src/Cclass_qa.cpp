/*
 * =====================================================================================
 *
 *       Filename:  Ccup.cpp
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
#include "Cclass_qa.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
#include <algorithm>
using namespace std;
//#define INIT_SHM_FLAG  -100 
//createDB_ROOMINFO_0
Cclass_qa::Cclass_qa (mysql_interface * db ) :CtableRoute10x10( db, "ROOMINFO","t_class_qa","classid" )
{ 


}


int Cclass_qa::init(userid_t classid)
{
	sprintf( this->sqlstr, "insert into %s values (%u,0,0,0,0x00000000,0x00000000, 0x0000)", 
		this->get_table_name(classid),classid);
	STD_SET_RETURN_EX (this->sqlstr, CLASSID_EXISTED_ERR);
}
int Cclass_qa::get_db(userid_t classid, stru_class_qa *p_out)
{
	sprintf( this->sqlstr, "select score,count,logdate,member_list ,member_score_list from %s where classid=%d", 
			this->get_table_name(classid),classid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,CLASSID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->score);
		INT_CPY_NEXT_FIELD(p_out->count);
		INT_CPY_NEXT_FIELD(p_out->logdate);
		BIN_CPY_NEXT_FIELD (&(p_out->member_list), sizeof (p_out->member_list ) );
		BIN_CPY_NEXT_FIELD (&(p_out->member_score_list), sizeof (p_out->member_score_list) );
	STD_QUERY_ONE_END();
}


int Cclass_qa::get(userid_t classid, stru_class_qa *p_out)
{
	int ret;	
	ret=this->get_db(classid,p_out );
	if (ret!=SUCC){
		this->init(classid);
		return this->get_db(classid,p_out );
	}
	return ret;
}
int Cclass_qa::get_score_info(userid_t classid, stru_class_score_info *p_out)
{
//	int ret;	
//	if (ret!=SUCC) return ret;
//	p_out->score=item.count;
//	p_out->count=item.score;
//	if (item.member_score_list.count>200) 
//		item.member_score_list.count=200; 
//
//	memcpy (&(p_out->member_score_list),&(item.member_score_list),
//			4+item.member_score_list.count*sizeof (item.member_score_list.items[0] )  );

	return SUCC;
}
	

int Cclass_qa::update(userid_t classid, stru_class_qa *p_list)
{
	char mysql_list[mysql_str_len(sizeof (p_list->member_list))];
	char mysql_member_score_list[mysql_str_len(sizeof (p_list->member_score_list))];

	set_mysql_string(mysql_list,(char*)(&(p_list->member_list)) , 
			4+sizeof(p_list->member_list.items[0])* p_list->member_list.count);

	set_mysql_string(mysql_member_score_list,(char*)(&(p_list->member_score_list)) , 
			4+sizeof(p_list->member_score_list.items[0])* p_list->member_score_list.count);



	sprintf( this->sqlstr, " update %s set  score=%u,count=%u,\
			logdate=%u, member_list='%s', member_score_list='%s' \
		where classid=%u " ,
		this->get_table_name(classid), 
		p_list->score,
		p_list->count,
		p_list->logdate,
		mysql_list,
		mysql_member_score_list,
		classid );
	STD_SET_RETURN(this->sqlstr,classid ,CLASSID_NOFIND_ERR );	
}

int Cclass_qa::add_score(userid_t classid, roominfo_class_qa_add_score_in *p_in,roominfo_class_qa_add_score_out *p_out )
{
	int ret;	
	stru_class_qa item; 
	ret=this->get(classid,&item );
	if(ret!=SUCC) return ret;
	
	uint32_t today=get_date(time(NULL))	;
	if (today!=item.logdate ){
		//不是同一天的
		item.logdate=today;			
		item.member_list.count=0;
	}
	attire_count_with_max opt_item;
	opt_item.attireid=p_in->memberid;
	opt_item.count=1;
	opt_item.maxcount=3;
	ret=add_attire_count((noused_homeattirelist*)&(item.member_list),&opt_item ,100 );
	if (ret!=SUCC){
		//已做过 
		return CLASS_QA_IS_ADD_ERR ; 
	}

	//总分
	item.count++;		
	item.score+=p_in->add_score;		
		 
	//个人总分
	bool find_flag=false;	
	for (uint32_t i=0;i<item.member_score_list.count;i++ ){
		if(item.member_score_list.items[i].memberid==p_in->memberid){
			item.member_score_list.items[i].score+=p_in->add_score;
			p_out->member_score=item.member_score_list.items[i].score;
			find_flag=true;
			break;
		}
	}
	if (! find_flag ){
		if (item.member_score_list.count>=200){
			item.member_score_list.count=200;							
		}else{// <200
			item.member_score_list.items[item.member_score_list.count ].memberid=p_in->memberid;
			item.member_score_list.items[item.member_score_list.count ].score=p_in->add_score;
			item.member_score_list.count++;							
		}
		p_out->member_score=p_in->add_score;
	}

	return this->update(classid,&item);
}

int Cclass_qa::check_add(userid_t classid, roominfo_class_qa_check_add_in *p_in,
	roominfo_class_qa_check_add_out *p_out	)
{
	int ret;	
	stru_class_qa item; 
	p_out->is_existed=0;
	ret=this->get(classid,&item );
	
	if(ret!=SUCC) return ret;
	
	uint32_t today=get_date(time(NULL))	;
	if (today!=item.logdate ){
		//不是同一天的
		item.logdate=today;			
		item.member_list.count=0;
	}

	for (uint32_t i=0;i<item.member_list.count;i++ ){
		if(item.member_list.items[i].memberid==p_in->memberid){
			if (item.member_list.items[i].score>=3)
				p_out->is_existed=1;
			break;
		}
	}
	return SUCC;
}

int Cclass_qa::class_get_flag(userid_t userid, stru_class_medal_flag *p_in)
{
	                   
	sprintf(this->sqlstr, "select class_flag from %s where classid = %u",
	       this->get_table_name(userid),
	       userid
	       );
	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
	    BIN_CPY_NEXT_FIELD(p_in, sizeof(stru_class_medal_flag));
	STD_QUERY_ONE_END();

}

int Cclass_qa::class_update_flag(userid_t userid, stru_class_medal_flag *p_in)
{
	char class_flag[mysql_str_len(sizeof(stru_class_medal_flag))];
	set_mysql_string(class_flag, (char*)(p_in), sizeof(stru_class_medal_flag));
	sprintf(this->sqlstr, "update %s set class_flag = '%s' where classid = %u",
	        this->get_table_name(userid),
	         class_flag,
	         userid
	        );
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cclass_qa::set_class_flag(userid_t userid, uint32_t classid, uint32_t index)
{
	stru_class_medal_flag temp = {};
	int ret = this->class_get_flag(userid, &temp);
	if (ret != SUCC) {
	       return ret;
	}
	DEBUG_LOG("===class count %u", temp.count);
	if (temp.count > 199) {
		return SUCC;
	}
	uint32_t i;
	for (i = 0; i < temp.count; i++) {
		if (temp.class_flag[i].memberid == classid) {
			break;
		}
	}
	if (i == temp.count) {
		temp.class_flag[i].memberid = classid;
		temp.class_flag[i].flag = 0;
		temp.count++;
	}
	if ((temp.class_flag[i].flag & (0x01 << index)) == 1) {
		return HAVE_SET_THIS_BIT_ERR;
	} else {
		temp.class_flag[i].flag |= (0x01 << index);
	}
	ret = this->class_update_flag(userid, &temp);
	return ret;
}
