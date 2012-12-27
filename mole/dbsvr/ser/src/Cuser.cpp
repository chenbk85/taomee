/*
 * =====================================================================================
 *
 *       Filename:  Cuser.cpp
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
 *  common.h
 * =====================================================================================
 */

#include "Cuser.h"
#include <time.h> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "msglog.h"



using namespace std;
//user 
Cuser::Cuser(mysql_interface * db, Csend_log *p_send_log, Citem_change_log * p_log ) 
	:CtableRoute100x10(db , "USER" , "t_user" , "userid"), mole_bank(db) 
{ 
	this->p_send_log=p_send_log;
	this->p_item_change_log = p_log;

	uint32_t week_day = get_week_day(time(0));
	if(week_day == 0 || week_day == 6 || week_day == 5){
		this->xiamee_a_day_max=20000;
	}
	else{
		this->xiamee_a_day_max=10000;
	}
	this->sale_xiamee_a_day_max=20000;

	this->log_xiaomee_flag=config_get_intval("LOG_XIAOMEE",0);
	if  (this->log_xiaomee_flag==1){
	 	strncpy (this->msglog_file,config_get_strval("MSG_LOG_FILE"),
				sizeof(this->msglog_file )-1 );
		this->msglog_file[sizeof(this->msglog_file )-1]='\0';
		DEBUG_LOG("log.file..%s",this->msglog_file);
	}else{

	}
}

int Cuser::del(userid_t userid )
{
	sprintf( this->sqlstr, "delete from %s where userid=%u " ,
			this->get_table_name(userid),  userid); 
	STD_REMOVE_RETURN (this->sqlstr,userid,  USER_ID_NOFIND_ERR);
}


int Cuser::insert(userid_t userid, user_without_id  * p_in )
{
	char homelist[200];
	char homelist_mysql[401];
	char jylist[200];
	char jylist_mysql[401];
	char tasklist_mysql[mysql_str_len(sizeof (p_in->tasklist))];

	memset(homelist,0,sizeof(homelist) );
	uint32_t lastip=0;

	//小屋初始内容
	home_attirelist	*p_attirelist=(home_attirelist*)homelist; 
	p_attirelist->count=1;	
	p_attirelist->item[0].attireid=160030;	
	p_attirelist->item[0].value[6]=6;	
	p_attirelist->item[0].value[7]=3;	
	p_attirelist->item[0].value[5]=1;	
	set_mysql_string(homelist_mysql ,(char *)homelist,20);

	//家园初始内容
	home_attirelist	*p_jyattirelist=(home_attirelist*)jylist; 
	p_jyattirelist->count=1;	
	p_jyattirelist->item[0].attireid=1220001;	
	set_mysql_string(jylist_mysql ,(char *)jylist,20);
	memset(&(p_in->tasklist),0,sizeof(p_in->tasklist) );
	//设置已完成新手任务设置300和301号任务
	p_in->tasklist.list[75]=0x05;

	uint32_t recv_task[2] = {1,0};
	msglog(this->msglog_file, 0x02022000 + 300, time(NULL), recv_task, sizeof(recv_task));
	msglog(this->msglog_file, 0x02022000 + 301, time(NULL), recv_task, sizeof(recv_task));

	set_mysql_string(tasklist_mysql, 
			(char*)(&(p_in->tasklist)),sizeof(p_in->tasklist));

	sprintf( this->sqlstr, "insert into %s"
	"(userid ,\
	vip ,\
	flag ,\
	petcolor ,\
	petbirthday ,\
	xiaomee ,\
	xiaomee_max ,\
	sale_xiaomee_max ,\
	exp ,\
	strong ,\
	iq ,\
	charm ,\
	game ,\
	work ,\
	fashion ,\
	Ol_count ,\
	Ol_today ,\
	Ol_last ,\
	Ol_time ,\
	lastip ,\
	history_lastip ,\
	birthday ,\
	lockid_logintime ,\
	lockid_endtime ,\
	tmp_flag ,\
	tmp_value ,\
	homeattirelist_1 ,\
	homeattirelist_2 ,\
	homeattirelist_3 ,\
	noused_homeattirelist ,\
	jyattirelist ,\
	noused_jyattirelist ,\
	tasklist, \
	msglist)"
	"values (\
		%u, \
		%u, \
		%u, \
		%u, \
		%u, \
		%u, \
		%u, \
		%u, \
		%u, \
		%u, \
		%u, \
		%u, \
		0x00, \
		0, \
		0, \
		%u, \
		%u, \
		%u, \
		%u, \
		%u, \
		0x00, \
		%u, \
		2145888000, \
		0, \
		0, \
		0, \
		'%s', \
		0x00000000, \
		0x00000000, \
		0x00000000, \
		'%s' , \
		0x00000000, \
		'%s' , \
		0x0800000000000000)", 
			this->get_table_name(userid), 
			userid,
			p_in->vip,
			p_in->flag,
			p_in->petcolor,
			p_in->petbirthday,
			p_in->xiaomee,
			0,
			0,
			p_in->exp,
			p_in->strong,
			p_in->iq,
			p_in->charm,
			p_in->Ol_count,
			p_in->Ol_today,
			p_in->Ol_last,
			p_in->Ol_time,
			lastip,
			p_in->birthday,
			homelist_mysql,
			jylist_mysql,
			tasklist_mysql
	   	);
		STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}

int Cuser::get_noused_home_attirelist(const char * type_str, userid_t userid , 
		noused_homeattirelist * p_list  )
{
	sprintf( this->sqlstr, "select noused_%sattirelist from %s where userid=%u ", 
			type_str,this->get_table_name(userid),userid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD(p_list,sizeof (*p_list));
	STD_QUERY_ONE_END();
}

int	Cuser::check_existed( userid_t userid)
{
	bool existed;
	int ret=this->id_is_existed(userid,&existed);
	if (ret!=SUCC) return ret;
	if (existed ){
		return SUCC;
	}else {
		return USER_ID_NOFIND_ERR;
	}
}

int Cuser::update_noused_homeattirelist(const char * type_str, userid_t userid ,
		noused_homeattirelist * p_list )
{
	char mysql_attirelist[mysql_str_len(sizeof(noused_homeattirelist))];
	set_mysql_string(mysql_attirelist,(char*)p_list, 
			4 +sizeof(attire_noused_item  )* p_list->count);
	sprintf( this->sqlstr, " update %s set \
		noused_%sattirelist='%s' \
		where userid=%u " ,
		this->get_table_name(userid), type_str ,
		mysql_attirelist,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser::home_attire_change(const char * type_str, userid_t userid , user_home_attire_change_in *p_in )
{
	if ( p_in->changeflag==2){//减少使用中
		return this->del_home_attire_used(type_str, userid, p_in->attireid );	
	}else if(p_in->changeflag==0 ){//减少未使用
		attire_count_with_max item;
		item.attireid=p_in->attireid;
		item.count=p_in->value;
		item.maxcount=p_in->maxvalue;
		return this->del_home_attire_noused(type_str, userid, &item );		
	}else if(p_in->changeflag==1 ){//增加未使用
		attire_count_with_max item;
		item.attireid=p_in->attireid;
		item.count=p_in->value;
		item.maxcount=p_in->maxvalue;
		return this->add_home_attire(type_str, userid, &item );		
	}else 
		return  ENUM_OUT_OF_RANGE_ERR;
}

int Cuser::del_home_attire_used(const char * type_str, userid_t userid , uint32_t attireid )
{
	noused_homeattirelist noused_list;
	home_attirelist  used_list;
	home_attire_item item;  
	item.attireid=attireid;
	int ret;
	ret=this->get_homeattirelist(type_str,userid,&used_list,&noused_list);
	if (ret!=SUCC) return ret;
	ret=reduce_used_attire (&used_list,attireid );
	if (ret!=SUCC) return ret;
	return this->update_used_homeattirelist(type_str,userid,&used_list);
	/*
	_start=used_list.item ;
	_end=_start+used_list.count;	
	_find=std::find( _start ,  _end , item );
	if (_find!=_end ) {
		//去掉这一项
		_tmp=_find+1;	
		for (;_tmp!=_end; _tmp++){
			memcpy(_tmp-1, _tmp, sizeof(* _tmp));
		}
		used_list.count--;
	}else{
		return USER_ATTIRE_ID_NOFIND_ERR;
	}	  */
}

/**
 * @brief 把指定的装扮设置为不使用状态，假设attireid正在被使用
 * @param const char* type_str 使用于表内的字符，此外设计很差
 * @param userid 指定用户
 * @param attireid 指定物品
 */
int Cuser::home_set_attire_noused(const char * type_str, userid_t userid , uint32_t attireid )
{
	noused_homeattirelist noused_list;
	home_attirelist  used_list;

	int ret=this->get_homeattirelist(type_str,userid,&used_list,&noused_list);
	if (ret!=SUCC) return ret;
	ret=reduce_used_attire (&used_list,attireid );
	if (ret!=SUCC) return ret;

	attire_count_with_max  noused_item;
	noused_item.attireid=attireid;
	noused_item.count=1;
	noused_item.maxcount=0xFFFFFFFF;
	ret=this->add_home_attire(type_str, userid,&noused_item);
	if (ret!=SUCC) return ret; 
	return this->update_used_homeattirelist(type_str,userid,&used_list);
}

int Cuser::del_home_attire_noused(const char * type_str, userid_t userid , attire_count_with_max  * p_item,
		uint32_t is_vip_opt_type)
{
	noused_homeattirelist list;
	home_attirelist  used_list;
	attire_noused_item *_start, *_end,*_find , *_tmp ; 

	int ret=this->get_homeattirelist(type_str, userid,&used_list,&list);
	if (ret!=SUCC) return ret;

	_start=list.item ;
	_end=_start+list.count;	
	_find=std::find( _start ,  _end ,  *((attire_noused_item*) p_item) );
	if (_find!=_end ) {
		//find  ,update
		if (_find->count>p_item->count){
			_find->count-=p_item->count;
		} else if (_find->count==p_item->count){
			//去掉这一项
			_tmp=_find+1;	
			for (;_tmp!=_end; _tmp++){
				memcpy(_tmp-1, _tmp, sizeof(* _tmp));
			}
			list.count--;
		}
		else 
			return ATTIRE_COUNT_NO_ENOUGH_ERR;
	}else{
		return USER_ATTIRE_ID_NOFIND_ERR;
	}	
	ret = this->update_noused_homeattirelist(type_str, userid,&list);
	if(ret == SUCC){
		this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, p_item->attireid, -p_item->count);
	}
	return ret;
}

int Cuser::add_home_attire(const char * type_str, userid_t userid , attire_count_with_max  * p_item, 
		uint32_t is_vip_opt_type)
{
	noused_homeattirelist list;
	home_attirelist  used_list;
	uint32_t used_count=0;

	int ret=this->get_homeattirelist(type_str,userid,&used_list,&list);
	if (ret!=SUCC) return ret;
	//在使用列表中的个数
	used_count=0;
	for (uint32_t i=0;i<used_list.count;i++){
		if (used_list.item[i].attireid==p_item->attireid){
			used_count++;
		}
	}	
	if (p_item->maxcount>=used_count){
		p_item->maxcount-=used_count;		
	}else{
		return VALUE_OUT_OF_RANGE_ERR;
	}

	ret=add_attire_count( &list, p_item,HOME_NOUSE_ATTIRE_ITEM_MAX );
	if (ret!=SUCC) return ret;

	this->add_attire_msglog(userid, p_item->attireid, p_item->count);

	ret = this->update_noused_homeattirelist( type_str, userid,&list);
	if(ret == SUCC){
		return this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, p_item->attireid, p_item->count);
	}
	return ret;

}

int Cuser::get_user_ex(userid_t userid  , get_user_out * p_out )
{
		sprintf( this->sqlstr, "select \
				vip, \
				flag, \
				petcolor,\
				petbirthday,\
				xiaomee,\
				exp,\
				strong,\
				iq,\
				charm,\
				game,\
				work,\
				fashion,\
				Ol_count,\
				Ol_today,\
				Ol_last,\
				Ol_time,\
				birthday,\
				tasklist,teamid, msglist, game_exp from %s where userid=%u",this->get_table_name(userid),userid );

		STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR );
			p_out->vip=atoi_safe(NEXT_FIELD );
			p_out->flag=atoi_safe(NEXT_FIELD );
			p_out->petcolor=atoi_safe(NEXT_FIELD );
			p_out->petbirthday=atoi_safe(NEXT_FIELD );
			p_out->xiaomee=atoi_safe(NEXT_FIELD );
			p_out->exp=atoi_safe(NEXT_FIELD );
			p_out->strong=atoi_safe(NEXT_FIELD );
			p_out->iq=atoi_safe(NEXT_FIELD );
			p_out->charm=atoi_safe(NEXT_FIELD );
			p_out->game=atoi_safe(NEXT_FIELD );
			p_out->work=atoi_safe(NEXT_FIELD );
			p_out->fashion=atoi_safe(NEXT_FIELD );
			p_out->Ol_count=atoi_safe(NEXT_FIELD );
			p_out->Ol_today=atoi_safe(NEXT_FIELD );
			p_out->Ol_last=atoi_safe(NEXT_FIELD );
			p_out->Ol_time=atoi_safe(NEXT_FIELD );
			p_out->birthday=atoi_safe(NEXT_FIELD );
			memcpy(&(p_out-> tasklist),NEXT_FIELD,sizeof (p_out-> tasklist ));	
			p_out->mvp_team=atoi_safe(NEXT_FIELD );
			memcpy (&(p_out->msglist), NEXT_FIELD, sizeof (stru_msg_list));
			p_out->game_exp=atoi_safe(NEXT_FIELD );
		STD_QUERY_ONE_END_WITHOUT_RETURN();

		if ( p_out->msglist.count!=0 ){
			//init msglist
			stru_msg_list t_msglist;
			t_msglist.count=0;
			t_msglist.len=8;
			this->update_msglist(userid ,&t_msglist);		
		}

		return SUCC;	
}

int Cuser::get_user_all(userid_t userid  , user_get_user_all_out * p_out )
{
		memset(p_out,0,sizeof (*p_out)  );
		sprintf( this->sqlstr, "select \
				vip, \
				flag, \
				petcolor,\
				petbirthday,\
				xiaomee,\
				xiaomee_max,\
				sale_xiaomee_max, \
				exp,\
				strong,\
				iq,\
				charm,\
				game,\
				work,\
				fashion,\
				Ol_count,\
				Ol_today,\
				Ol_last,\
				Ol_time,\
				lastip,\
				birthday,\
				tasklist \
		from %s where userid=%u",this->get_table_name(userid),userid );

		STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR );
			INT_CPY_NEXT_FIELD(p_out->vip);
			INT_CPY_NEXT_FIELD(	p_out->flag);
			INT_CPY_NEXT_FIELD(	p_out->petcolor);
			INT_CPY_NEXT_FIELD(	p_out->petbirthday);
			INT_CPY_NEXT_FIELD(	p_out->xiaomee);
			INT_CPY_NEXT_FIELD(	p_out->xiaomee_max);
			INT_CPY_NEXT_FIELD(	p_out->sale_xiaomee_max);
			INT_CPY_NEXT_FIELD(	p_out->exp);
			INT_CPY_NEXT_FIELD(	p_out->strong);
			INT_CPY_NEXT_FIELD(	p_out->iq);
			INT_CPY_NEXT_FIELD(	p_out->charm);
			INT_CPY_NEXT_FIELD(	p_out->game);
			INT_CPY_NEXT_FIELD(	p_out->work);
			INT_CPY_NEXT_FIELD(	p_out->fashion);
			INT_CPY_NEXT_FIELD(	p_out->Ol_count);
			INT_CPY_NEXT_FIELD(	p_out->Ol_today);
			INT_CPY_NEXT_FIELD(	p_out->Ol_last);
			INT_CPY_NEXT_FIELD(	p_out->Ol_time);
			INT_CPY_NEXT_FIELD(	p_out->last_ip);
			INT_CPY_NEXT_FIELD(	p_out->birthday);
			BIN_CPY_NEXT_FIELD(&(p_out->tasklist), sizeof(p_out->tasklist));		
		STD_QUERY_ONE_END();
}

int Cuser::get_tasklist(userid_t userid  , stru_tasklist * p_tasklist)
{

		sprintf( this->sqlstr, "select  tasklist\
		from %s where userid=%u",this->get_table_name(userid),userid );

		STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR );
			BIN_CPY_NEXT_FIELD(p_tasklist,sizeof (*p_tasklist));	
		STD_QUERY_ONE_END();

}

int Cuser::get_user(userid_t userid  , get_user_out * p_out )
{
		sprintf( this->sqlstr, "select \
				vip, \
				flag, \
				petcolor,\
				petbirthday,\
				xiaomee,\
				exp,\
				strong,\
				iq,\
				charm,\
				game,\
				work,\
				fashion,\
				Ol_count,\
				Ol_today,\
				Ol_last,\
				Ol_time,\
				tasklist,\
				teamid, \
				game_exp from %s where userid=%u",this->get_table_name(userid),userid );

		STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR );
			p_out->vip=atoi_safe(NEXT_FIELD );
			p_out->flag=atoi_safe(NEXT_FIELD );
			p_out->petcolor=atoi_safe(NEXT_FIELD );
			p_out->petbirthday=atoi_safe(NEXT_FIELD );
			p_out->xiaomee=atoi_safe(NEXT_FIELD );
			p_out->exp=atoi_safe(NEXT_FIELD );
			p_out->strong=atoi_safe(NEXT_FIELD );
			p_out->iq=atoi_safe(NEXT_FIELD );
			p_out->charm=atoi_safe(NEXT_FIELD );
			p_out->game=atoi_safe(NEXT_FIELD );
			p_out->work=atoi_safe(NEXT_FIELD );
			p_out->fashion=atoi_safe(NEXT_FIELD );
			p_out->Ol_count=atoi_safe(NEXT_FIELD );
			p_out->Ol_today=atoi_safe(NEXT_FIELD );
			p_out->Ol_last=atoi_safe(NEXT_FIELD );
			p_out->Ol_time=atoi_safe(NEXT_FIELD );
			memcpy(&(p_out-> tasklist),NEXT_FIELD,sizeof (p_out-> tasklist ));	
			p_out->mvp_team = atoi_safe(NEXT_FIELD );
			p_out->game_exp = atoi_safe(NEXT_FIELD );
		STD_QUERY_ONE_END();
}
int Cuser::get_mole_color(userid_t userid, uint32_t &color)
{
	sprintf( this->sqlstr, " select petcolor from %s where userid=%u " ,
                        this->get_table_name(userid),userid);
                STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR );
                        color = atoi_safe(NEXT_FIELD );
                STD_QUERY_ONE_END();
}

int Cuser::update_petcolor(userid_t userid ,  user_set_petcolor_in * in)
{	
	sprintf( this->sqlstr, " update %s set \
			petcolor=%u \
			where userid=%u " ,
			this->get_table_name(userid),  
				in->petcolor,	
		   	userid); 
	STD_SET_RETURN (this->sqlstr,userid,  USER_ID_NOFIND_ERR);
}

int Cuser::get_attirelist( const char * type_str, userid_t userid , home_attirelist * p_home_attirelist)
{
	sprintf( this->sqlstr, "select  %sattirelist from %s where userid=%u ", 
			type_str, this->get_table_name(userid),userid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD (p_home_attirelist  , sizeof(*p_home_attirelist ));
	STD_QUERY_ONE_END();
}

int Cuser::user_check(userid_t userid )
{
	sprintf(this->sqlstr, "select vip,Ol_last,lockid_endtime, lockid_logintime\
			from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	uint32_t vip;
	uint32_t Ol_last;
	uint32_t endtime;
	uint32_t logintime;
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		vip=atoi_safe(NEXT_FIELD);
		Ol_last=atoi_safe(NEXT_FIELD);
		endtime=atoi_safe(NEXT_FIELD);
		logintime=atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END_WITHOUT_RETURN();
	uint32_t now = time(NULL);
	if (now < endtime) {
		if (now > logintime) {
			return ANXIN_SERVIECE_OUT_OF_RANGE_ERR;
		}
	}
	if ((vip&USER_FLAG_UNUSED)!=0) return USER_NOUSED_ERR; 

	if ((vip & USER_FLAG_OFFLINE24)!=0 && (time(NULL)-Ol_last)<24*3600 ) 
		return USER_OFFLIN24_ERR; 

	if ((vip & USER_FLAG_OFFLINE24)!=0){
		this->set_flag(userid,
			VIP_FLAG_STR,USER_FLAG_OFFLINE24 ,0);
	}

	if ((vip & USER_FLAG_UNUSED_SEVEN_DAY) != 0 && (time(NULL) - Ol_last) < 24*3600*7) {
		return USER_OFFLINE_SEVEN_DAY_ERR;
	}
	if ((vip & USER_FLAG_UNUSED_SEVEN_DAY) != 0) {
		this->set_flag(userid, VIP_FLAG_STR, USER_FLAG_UNUSED_SEVEN_DAY, 0);
	}

	if ((vip & USER_FLAG_UNUSED_FOURTEEN_DAY) != 0 && (time(NULL) - Ol_last) < 24*3600*14) {
		return USER_OFFLINE_FOURTEEN_DAY_ERR;
	}

	if ((vip & USER_FLAG_UNUSED_FOURTEEN_DAY) != 0) {
		this->set_flag(userid, VIP_FLAG_STR, USER_FLAG_UNUSED_FOURTEEN_DAY, 0);
	}
	return SUCC;
}

int Cuser::set_flag ( userid_t userid  ,const  char * flag_type  ,  uint32_t flag_bit ,  bool is_true )
{
	uint32_t flag;
	int ret;
	bool old_is_true;
	if ((ret =this->get_flag(userid, flag_type ,&flag))!=SUCC){
			return ret;
	}	
	if ( (flag & flag_bit) == flag_bit ) old_is_true=true;
	else old_is_true =false; 

	if (old_is_true==is_true){
		//已经设置了
		return FLAY_ALREADY_SET_ERR;	
	}
		
	if (is_true ) flag+=flag_bit;  		
	else flag-=flag_bit;
	return this->update_flag(userid, flag_type ,flag);
}


int  Cuser::get_homeattirelist(const char * type_str,  userid_t userid ,
		home_attirelist * p_usedlist ,  noused_homeattirelist* p_nousedlist)
{
	sprintf( this->sqlstr, "select %sattirelist, noused_%sattirelist \
			from %s where userid=%u ", 
			type_str,type_str ,this->get_table_name(userid),userid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD (p_usedlist,sizeof (*p_usedlist ) );
			BIN_CPY_NEXT_FIELD (p_nousedlist, sizeof (*p_nousedlist) );
	STD_QUERY_ONE_END();
}

int Cuser::reset_noused_homeattirelist(const char * type_str,  userid_t userid ,
				noused_homeattirelist* p_nousedlist)
{
	uint32_t i;
	for(i=0;i<p_nousedlist->count;i++)	{
		if (p_nousedlist->item[i].attireid==12839){
			p_nousedlist->item[i].attireid=160376;
			return this->update_noused_homeattirelist(type_str,userid,p_nousedlist );
		}
	}
	return SUCC;

}

int Cuser::add_offline_msg(userid_t userid ,  msg_item *item )
{
	int ret;
	stru_msg_list msglist;
	ret=this->get_msglist(userid, &msglist);
	if (ret!=SUCC) return ret;
	
	if (msglist.len+item->itemlen > MSG_LIST_BUF_SIZE) 
		return  VALUE_OUT_OF_RANGE_ERR;


	memcpy (((char*)(&msglist))+msglist.len, (char*)item,item->itemlen);
	msglist.len+=item->itemlen;
	msglist.count++;
	return this->update_msglist(userid,&msglist);
}

int Cuser::update_msglist(userid_t userid ,  stru_msg_list * msglist )
{
	char mysql_msglist[mysql_str_len(sizeof (stru_msg_list))];
	set_mysql_string(mysql_msglist,(char*)msglist, msglist->len );
	sprintf( this->sqlstr, " update %s set \
		msglist='%s'\
		where userid=%u " ,
		this->get_table_name(userid), 
		mysql_msglist,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser::get_msglist(userid_t userid ,  stru_msg_list * msglist )
{
	sprintf( this->sqlstr, "select msglist from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD (msglist, sizeof (stru_msg_list) );
	STD_QUERY_ONE_END();
}

void Cuser::get_home_attire_total(ATTIRE_MAPITEM * p_itemmap,
			   	home_attirelist * p_usedlist,
				noused_homeattirelist * p_nousedlist )
{
	uint32_t attireid;
	//set used count
	for (uint32_t i=0;i<p_usedlist->count;i++ )	{
		attireid=p_usedlist->item[i].attireid;
		if ( p_itemmap->find(attireid)==p_itemmap->end()) {
			//(*p_itemmap).insert(map<uint32_t,uint32_t> :: value_type(attireid,1));
			(*p_itemmap)[attireid]=1;
		}else{
			(*p_itemmap)[attireid]++;
		}
	}

	//set noused count
	for (uint32_t i=0;i<p_nousedlist->count;i++ ){
		attireid=p_nousedlist->item[i].attireid;
		if ( p_itemmap->find(attireid)==p_itemmap->end()) {
			//(*p_itemmap).insert(map<uint32_t,uint32_t> :: value_type(attireid,
			//p_nousedlist->item[i].count));
			(*p_itemmap)[attireid]= p_nousedlist->item[i].count;
			
		}else{
			(*p_itemmap)[attireid]+=p_nousedlist->item[i].count;
		}
	}
}

int Cuser::get_attire_count( const char * type_str, userid_t userid, uint32_t  attireid,
		uint32_t count_flag,  uint32_t *p_count )
{
	home_attirelist 	old_usedlist;
	noused_homeattirelist old_nousedlist;
	int ret;	
	//get old data
	if ((ret=this->get_homeattirelist( type_str,
		userid,&old_usedlist, & old_nousedlist))!=SUCC){
		return ret;
	}
	*p_count=0;
	if ( count_flag==0 ||count_flag==2 ){//未使用
		for (uint32_t i=0;i<old_nousedlist.count;i++ )	{
			//DEBUG_LOG("attireid [%u][%u] ",i,old_nousedlist.item[i].attireid );
			if(old_nousedlist.item[i].attireid==attireid){
				(*p_count)+=old_nousedlist.item[i].count;
				break;
			}
		}
	}

	if ( count_flag==1 ||count_flag==2 ){//已使用
		for (uint32_t i=0;i<old_usedlist.count;i++ )	{
			//DEBUG_LOG("attireid [%u][%u] ",i,old_usedlist.item[i].attireid );
			if( old_usedlist.item[i].attireid==attireid){
				(*p_count)++;
			}
		}
	}

	return SUCC;
}

int Cuser::update_homeattire_all(const char * type_str,  userid_t userid ,  
			home_attirelist * p_usedlist , 
			noused_homeattirelist * p_nousedlist )
{
	home_attirelist 	old_usedlist;
	noused_homeattirelist old_nousedlist;
	ATTIRE_MAPITEM olditemmap; 	
	ATTIRE_MAPITEM newitemmap; 	
	ATTIRE_MAPITEM::iterator oldit;
	ATTIRE_MAPITEM::iterator newit;
	int ret;	
	
	//get old data
	if ((ret=this->get_homeattirelist( type_str,
		userid,&old_usedlist, & old_nousedlist))!=SUCC){
		return ret;
	}

	//check
	this->get_home_attire_total(&olditemmap,&old_usedlist, &old_nousedlist  );

	this->get_home_attire_total(&newitemmap,p_usedlist , p_nousedlist );		
	//check size    
	if (olditemmap.size()!= newitemmap.size()){
			DEBUG_LOG("type count err db[%lu] send[%lu]",
					olditemmap.size(),newitemmap.size() );
			return USER_SET_ATTIRE_DATA_ERR;
	}		

	for( oldit=olditemmap.begin();oldit!=olditemmap.end();++oldit){
		newit=newitemmap.find(oldit->first);
		if (newit !=newitemmap.end() && newit->second==oldit->second ){
			// find  and count is equal   
			continue;	
		}else{
			if (newit ==newitemmap.end()){
				DEBUG_LOG("sendlist no find attireid[%u]", oldit->first );
			}else{
				DEBUG_LOG("sendlist attireid [%u],  count err :db[%u] send[%u]", 
					oldit->first, oldit->second,newit->second);
			}
			return USER_SET_ATTIRE_DATA_ERR;
		}
	}

	return this->update_homeattirelist(type_str, userid,p_usedlist, p_nousedlist );	

}

int Cuser::update_homeattirelist(const char * type_str, userid_t userid ,   
			home_attirelist * p_usedlist,noused_homeattirelist * p_nousedlist )
{
	char mysql_usedlist[mysql_str_len(sizeof (home_attirelist))];
	char mysql_nousedlist[mysql_str_len(sizeof (noused_homeattirelist ))];

	set_mysql_string(mysql_usedlist,(char*)p_usedlist, 
			4+sizeof(home_attire_item)* p_usedlist->count);

	set_mysql_string(mysql_nousedlist,(char*)p_nousedlist, 
			4+sizeof(attire_noused_item)* p_nousedlist->count);

	sprintf( this->sqlstr, " update %s set \
		%sattirelist='%s', \
		noused_%sattirelist ='%s' \
		where userid=%u " ,
		 this->get_table_name(userid), 
		 type_str,mysql_usedlist,
		type_str, mysql_nousedlist,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser::update_used_homeattirelist(const char * type_str, userid_t userid ,   
			home_attirelist * p_usedlist )
{
	char mysql_usedlist[mysql_str_len(sizeof (home_attirelist))];
	set_mysql_string(mysql_usedlist,(char*)p_usedlist, 
			4+sizeof(home_attire_item)* p_usedlist->count);

	sprintf( this->sqlstr, " update %s set \
		%sattirelist='%s' \
		where userid=%u " ,
		 this->get_table_name(userid), type_str,
		mysql_usedlist,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser::get_flag(userid_t userid ,const char * flag_type   ,  uint32_t * flag)
{
	sprintf( this->sqlstr, "select  %s from %s where userid=%u ", 
		flag_type, this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
			*flag=atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END();
}
int Cuser::get_flags(userid_t userid ,user_get_flag_out *p_out )
{
	sprintf( this->sqlstr, "select  flag,vip  from %s where userid=%u ", 
		 this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->flag );
		INT_CPY_NEXT_FIELD(p_out->vip);
	STD_QUERY_ONE_END();
}

int Cuser::get_xiaomee(userid_t userid ,uint32_t *p_xiaomee)
{
	sprintf( this->sqlstr, "select  xiaomee from %s where userid=%u ", 
		 this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_xiaomee);
	STD_QUERY_ONE_END();
}

int Cuser::update_flag(userid_t userid ,const char * flag_type  , uint32_t  flag)
{
	sprintf( this->sqlstr, "update %s set %s =%u where userid=%u " ,
		this->get_table_name(userid), flag_type,flag,userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}
/*
 * 计算摩尔豆不超过五亿（包括银行，兜里),所有加魔豆的地方
 * 都要调用此函数
 */
int Cuser::cal_total_xiaomee(userid_t userid, int *change_value, int* ret_xiaomee)
{
   //1,表示原有总的摩尔豆大于5亿，扣掉多出部分
   //2,表示要增加的数量和原有总的数量之和大于5亿，加一部分	
	if(*change_value > 0){
		uint32_t bank_xiaomee = 0;
		mole_bank.get_total_money_in_bank(userid, &bank_xiaomee);
		uint32_t pocket_xiaomee = 0;
		this->get_xiaomee(userid, &pocket_xiaomee);
		
		uint32_t total = bank_xiaomee + pocket_xiaomee;	
		if(total > MOLE_XIAOMEE_MAX){
			uint32_t remainder = total- MOLE_XIAOMEE_MAX ;
			uint32_t sub_xiaomee = pocket_xiaomee > remainder ? (pocket_xiaomee-remainder):0;
			this->update_xiaomee(userid, sub_xiaomee);
			*ret_xiaomee = sub_xiaomee;
			return 1;
		}
		else{
			if(total+ (*change_value) > MOLE_XIAOMEE_MAX){
				uint32_t surplus = total+ (*change_value) - MOLE_XIAOMEE_MAX;	
				*change_value = (*change_value) - surplus;
				return 2;
			}
		}//else
		
	}

	return 0;
}

int Cuser::change_xiaomee(userid_t userid , int change_value ,
		uint32_t reason,uint32_t reason_ex , int * p_xiaomee)
{
	user_change_user_value_in uci; 	 
	user_change_user_value_out uco; 	 
	//要返回xiaomee最终值
//	if (change_value==0) return SUCC;
	DEBUG_LOG("need update xiaomee: %d", change_value);
	memset(&uci, 0,sizeof(uci)  );
	memset(&uco, 0,sizeof(uco)  );
	int m_ret = this->cal_total_xiaomee(userid, &change_value, p_xiaomee);
	if(m_ret != 1){
		uci.addxiaomee=change_value;
		uci.reason=reason;
		uci.reason_ex=reason_ex;
		int ret=this->change_user_value( userid, &uci,  &uco );
		if (ret!=SUCC) return ret;
		*p_xiaomee=	uco.xiaomee;

	}

	return SUCC;
}

int Cuser::update_onlinetime(userid_t userid  , time_t onlinetime,uint32_t lastip )
{
	uint32_t  Ol_count,Ol_today,Ol_last,Ol_time,db_lastip;
	time_t now = time (NULL);
	const char *xiaomee_max_fmt;
	const char *xiaomee_max_fmt1="";
	const char *xiaomee_max_fmt2=" xiaomee_max=0, sale_xiaomee_max=0, ";

	sprintf( this->sqlstr, "select Ol_count,Ol_today,Ol_last,Ol_time ,lastip \
			from %s where userid=%u ", 
			this->get_table_name(userid),userid);

	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
			Ol_count=atoi_safe(NEXT_FIELD );
			Ol_today=atoi_safe(NEXT_FIELD );
			Ol_last=atoi_safe(NEXT_FIELD );
			Ol_time=atoi_safe(NEXT_FIELD );
			db_lastip=atoi_safe(NEXT_FIELD );
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	if (db_lastip!=lastip ){
	 	this->set_history_ip(userid,db_lastip );
	}

	if(onlinetime>0)
		Ol_count++;

	// same day 
	if (get_date(now)==get_date(Ol_last )){
		Ol_today+=onlinetime;		
		xiaomee_max_fmt=xiaomee_max_fmt1;
	}else{
		Ol_today=onlinetime;		
		xiaomee_max_fmt=xiaomee_max_fmt2;
	}
	Ol_last=now;
	Ol_time+=onlinetime;
	

	sprintf( this->sqlstr, " update %s set \
					%s \
					Ol_count=%u,\
					Ol_today=%u,\
					Ol_last=%u,\
					Ol_time=%u,\
					lastip=%u\
		   			where userid=%u " ,
				this->get_table_name(userid), 
					xiaomee_max_fmt,
					Ol_count,
					Ol_today,
					Ol_last,
					Ol_time,
					lastip,
				 	userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

/* @breif 更新当天在线时长和最后一次登入时间
 * @param userid 米米号
 * @param ol_tody 当天在线时长
 * @param ol_last 最后登入时间
 */
int Cuser::update_last_onlinetime(userid_t userid, uint32_t ol_today, uint32_t ol_last)
{
	const char *xiaomee_max_clear=" xiaomee_max=0, sale_xiaomee_max=0, ";
	sprintf(this->sqlstr, " update %s set %s Ol_today=%u, Ol_last=%u where userid=%u",
			this->get_table_name(userid), 
			xiaomee_max_clear,
			ol_today,
			ol_last,
		 	userid
		  );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}



int Cuser::change_user_value(userid_t userid , user_change_user_value_in *in  ,  
				user_change_user_value_out *out )
{

	uint32_t sale_xiaomee_max,xiaomee_max,Ol_last;
	int ret=SUCC;
	uint32_t now=time(NULL);	
	char  Ol_last_fmt[100];
	uint32_t old_xiaomee  = 0;
	//if (in->addxiaomee>10000) return XIAOMEE_MAX_A_DAY_ERR; 

	sprintf( this->sqlstr, "select Ol_last, xiaomee , xiaomee_max, sale_xiaomee_max, exp ,strong, iq, charm \
			from %s  where userid=%u " ,
		this->get_table_name(userid),userid );
	
	//uint32_t tmp_xiao = 0;		
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR );
		Ol_last=atoi_safe(NEXT_FIELD);
		old_xiaomee=atoi_safe(NEXT_FIELD);
		out->xiaomee = old_xiaomee + in->addxiaomee;
		xiaomee_max=atoi_safe(NEXT_FIELD);
		INT_CPY_NEXT_FIELD(sale_xiaomee_max);
		out->exp=atoi_safe(NEXT_FIELD) + in->addexp;
		out->strong=atoi_safe(NEXT_FIELD)+in->addstrong ;
		out->iq=atoi_safe(NEXT_FIELD)+in ->addiq;
		out->charm=atoi_safe(NEXT_FIELD)+in->addcharm ;
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	if (in->addcharm==0&& in->addexp==0 && in->addiq==0 
			&& in->addstrong==0 && in->addxiaomee==0  )
	{
		//不调整
		return SUCC;
	}

		
	if (in->addxiaomee!=0 && get_date(Ol_last)!= get_date(now)){
		//不在同一天,则将xiaomee_max设置为0
		sprintf(Ol_last_fmt, " Ol_last=%u, Ol_today=0," ,now   );
		xiaomee_max=0;
		sale_xiaomee_max=0;
	}else{
		strcpy(Ol_last_fmt, "" );
	}


//	DEBUG_LOG("in reason%u",in->reason );
	if (in->addxiaomee>0  //是增加xiaomee
			&& in->reason != XIAOMEE_SALE_PIGLET_ITEM//卖系统道具
			&& in->reason!=XIAOMEE_USE_ADD_SU//管理员设置
			&& in->reason!=XIAOMEE_USE_ADD_SALE //卖出
			&& in->reason!=XIAOMEE_USE_ADD_BACK //回滚
			&& in->reason!=XIAOMEE_USE_ADD_BANK //银行取款
			&& in->reason!=XIAOMEE_USE_ADD_NPC_TASK //NPC任务
			)
	{
		xiaomee_max+=in->addxiaomee;
		if (xiaomee_max>this->xiamee_a_day_max)
		{
			DEBUG_LOG("XIAOMEE_A_DAY_ENOUGH_ERR:userid[%d] xiaomee [%d]", userid,xiaomee_max);
			return XIAOMEE_MAX_A_DAY_ERR;

		}
	}

	
	if (out->xiaomee<0){
		return XIAOMEE_NOT_ENOUGH_ERR;
	}

	//卖出限制
	if(in->reason==XIAOMEE_USE_ADD_SALE){//卖出
		sale_xiaomee_max += in->addxiaomee;
		if (sale_xiaomee_max>this->sale_xiamee_a_day_max){
			DEBUG_LOG("SALE XIAOMEE MAX:userid[%d] xiaomee [%d]",
					userid,sale_xiaomee_max);
			return SALE_XIAOMEE_MAX_A_DAY_ERR;

		}
	}


	//check value, VALUE_MAX	  
	//添加5亿摩尔豆上限后，由于一些玩家的原有摩尔豆个数已经超出上限，所以增加摩尔豆时，只是将其置为原有
	//摩尔豆个数；另一些玩家原有摩尔豆个数小于等于摩尔豆上限，增加摩尔豆时，如果个数超过上限，将摩尔
	//豆个数置为上限
	if (out->xiaomee > MOLE_XIAOMEE_MAX && old_xiaomee < MOLE_XIAOMEE_MAX && in->addxiaomee > 0){
		out->xiaomee =  MOLE_XIAOMEE_MAX ;
	}
	else if(out->xiaomee > MOLE_XIAOMEE_MAX && old_xiaomee >= MOLE_XIAOMEE_MAX && in->addxiaomee > 0){
		out->xiaomee= old_xiaomee;
	}
	if(out->exp<0 || out->iq<0 || out->strong<0 || out->charm<0 ){
		DEBUG_LOG("err: value out of [%d][%d][%d][%d][%d] ",
			out->xiaomee, out->exp, out->strong , out->iq,
			out->charm);
		return  VALUE_OUT_OF_RANGE_ERR;
	}
	

	//update  xiaomee
	sprintf( this->sqlstr, "update %s  set %s  xiaomee=%d,xiaomee_max=%u, sale_xiaomee_max=%u, \
			exp=%d, strong=%d, iq=%d, charm=%d  	\
			where userid=%u " ,
		this->get_table_name(userid), 
		Ol_last_fmt,
		out->xiaomee,
		xiaomee_max,
		sale_xiaomee_max,
		out->exp,
		out->strong ,
		out->iq,
		out->charm,
		userid);
	STD_SET_WITHOUT_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR);	

	if (in->addxiaomee!=0){
		//原有的统计
		TXIAOMEE_USE xu;	
		xu.logtime=time(NULL);
		xu.usecount=abs( in->addxiaomee);
		xu.reason=in->reason;
		xu.reason_ex=in->reason_ex;
		SAVE_PROTOLOG_XIAOMEE_USE(xu);

		//统一统计
		uint32_t logid=0;

		switch (in->reason){
			case XIAOMEE_USE_ADD_SALE :
				logid=0x020B1200;//+:卖出
				break;
			case XIAOMEE_USE_ADD_BY_GAME:
				logid=0x020B1100;//+:游戏
				break;
			case XIAOMEE_USE_ADD_BY_PARTY:
				logid=0x020B1300;//+:活动
				break;
			case XIAOMEE_USE_ADD_BY_TASK:
				logid=0x020B1400;//+:任务
				break;
			case XIAOMEE_USE_ADD_BY_USER_GET:
				logid=0x020B1500;//+:客户端直接索取
				break;
			case XIAOMEE_USE_DEL_BUY_ATTIRE:
				logid=0x020B0100;//-:买东西
				break;
			case XIAOMEE_USE_DEL_BY_PET:
				logid=0x020B0200;//-:买宠物
				break;
			default :
				break;
		}
		if (logid==0 ){//未定义
			if (in->addxiaomee>0) {
				logid=0x020B1000;//+
			}else{
				logid=0x020B0000;//-
			}
		}
		uint32_t change_xiaomee;
		if (in->addxiaomee>0) {
			change_xiaomee=in->addxiaomee;		
		}else{
			change_xiaomee=-in->addxiaomee;
			//msglog(this->msglog_file, 0x04040105,time(NULL), &userid, 4);
		}

		if (this->log_xiaomee_flag==1){
			msglog(this->msglog_file,logid ,time(NULL),
				&(change_xiaomee ), sizeof(change_xiaomee));	
		}

	}
	//修改明细 
	if (ret==SUCC){
		//log_change
		change_log_item c_item;
		c_item.userid=userid;
		c_item.change_type=5;
		c_item.logtime=time(NULL);
		c_item.change_value=in->addxiaomee;
		c_item.change_v1=0;
		c_item.change_v2=out->xiaomee;
		this->p_send_log->send(&c_item );
	}


 
	return ret;
} 

int Cuser::set_tasklist(userid_t userid , stru_tasklist *p_tasklist)
{
	char tasklist_mysql[mysql_str_len(sizeof(*p_tasklist))];
	set_mysql_string(tasklist_mysql, 
			(char*)(p_tasklist),sizeof(*p_tasklist));
	sprintf( this->sqlstr, "update %s  set tasklist='%s'\
			where userid=%u ",
		this->get_table_name(userid), 
		tasklist_mysql, userid
		);
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
} 

int Cuser::del_homeattire_noused( noused_homeattirelist *p_list, 
				attire_noused_item *p_item )
{
	attire_noused_item *_start, *_end,*_find , *_tmp ; 

	_start=p_list->item ;
	_end=_start+p_list->count;	
	_find=std::find( _start ,  _end ,  * p_item );
	if (_find!=_end ) {
		//find  ,update
			
		if (_find->count>p_item->count){
			_find->count-=p_item->count;
			DEBUG_LOG("p_list del item id:%d count:%d",p_item->attireid,p_item->count );
			return p_item->count;
		}else{
			int ret_count=_find->count;
			//去掉这一项
			_tmp=_find+1;	
			for (;_tmp!=_end; _tmp++){
				memcpy(_tmp-1, _tmp, sizeof(* _tmp));
			}
			(p_list->count)--;
			return ret_count;  
		}
	}else{
		return 0;
	}	
}

int Cuser::del_homeattire_used( home_attirelist *p_list , home_attire_item * p_item )
{
	home_attire_item *_start, *_end,*_find , *_tmp ; 
	_start=p_list->item ;
	_end=_start+p_list->count;	
	_find=std::find( _start ,  _end , *p_item );
	if (_find!=_end ) {
		//去掉这一项
		_tmp=_find+1;	
		for (;_tmp!=_end; _tmp++){
			memcpy(_tmp-1, _tmp, sizeof(* _tmp));
		}
		(p_list->count)--;
		return 1;
	}else{
		return 0;
	}	
}

int Cuser::swap_homeattire(const char *   type_str, userid_t userid , 
				uint32_t oldcount,attire_count* oldlist,
				uint32_t newcount,attire_count_with_max * newlist )
{
	noused_homeattirelist noused_list;
	home_attirelist  used_list;
	attire_count * p_noused_item;
	int real_del_count;
	int ret;
	uint32_t i,j;
	ret=this->get_homeattirelist(type_str, userid,&used_list,&noused_list);
	if (ret!=SUCC) return ret;
	//删除旧有的
	for (i=0;i<oldcount;i++){
 		p_noused_item=oldlist+i;
		real_del_count=this->del_homeattire_noused(&noused_list, p_noused_item );
		//从已使用中删除
		for(j=0;j<p_noused_item ->count-real_del_count;j++){
			if( this->del_homeattire_used(&used_list, 
						(home_attire_item* )p_noused_item  )!=1 ) {
				return   USER_ATTIRE_ID_NOFIND_ERR;
			}
		}
	}

	for (i=0;i<newcount;i++){
		ret=add_attire_count(&noused_list,  (newlist+i),HOME_NOUSE_ATTIRE_ITEM_MAX );
		if ( ret!=SUCC) return ret;
	}
	return this->update_homeattirelist(type_str,userid,&used_list,&noused_list  );
}

int Cuser::update_used_homeattirelist_with_check(const char * type_str, userid_t userid ,   
			home_attirelist * p_new_usedlist )
{
	int ret;
	home_attirelist old_usedlist;  
	ret=this->get_attirelist( type_str , userid, &old_usedlist);
	if(ret!=SUCC) return ret;
	if(old_usedlist.count!=p_new_usedlist->count)		{
		DEBUG_LOG("count:err %u %u",old_usedlist.count,p_new_usedlist->count  );
		return USER_SET_ATTIRE_DATA_ERR;
	}	
	for (uint32_t i=0;i<old_usedlist.count;i++ ){
		if (p_new_usedlist->item[i]!=old_usedlist.item[i]){
			DEBUG_LOG("item:err %u %u",p_new_usedlist->item[i].attireid,
				   old_usedlist.item[i].attireid	);
			return  USER_SET_ATTIRE_DATA_ERR;
		}
	}
	return this->update_used_homeattirelist( type_str,userid,p_new_usedlist);	
}

int Cuser::isset_birthday(userid_t userid,stru_is_seted *p_out) 
{
	int ret;
	uint32_t	db_birthday	;
	ret=this->get_birtyday(userid,&db_birthday );
	if (ret!=SUCC) return ret;
	if (db_birthday==0){
		p_out->is_seted=0;
	}else{
		p_out->is_seted=1;
	}
	return SUCC;
}

int Cuser::set_birthday(userid_t userid , uint32_t birthday )
{
	/*
	IS_SETED_STRU  is_set; 
	int ret;	
	ret=this->isset_birthday(userid,&is_set);
	if(ret!=SUCC) return ret;
	if (is_set.is_seted ==1 ) {
		return  USER_BIRTHDAY_IS_SETED_ERR;
	}
	*/
	
	sprintf( this->sqlstr, " update %s set \
			birthday=%u \
			where userid=%u " ,
			this->get_table_name(userid),  
				birthday,	
		   	userid); 
	STD_SET_RETURN (this->sqlstr,userid,  USER_ID_NOFIND_ERR);
}

int Cuser::get_birtyday(userid_t userid,uint32_t *p_birthday )
{
	sprintf( this->sqlstr, "select birthday from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_birthday);
	STD_QUERY_ONE_END();
}

int Cuser::get_tmp_value(userid_t userid,uint32_t *p_flag, uint32_t *p_value)
{
	sprintf( this->sqlstr, "select tmp_flag,tmp_value from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_flag );
		INT_CPY_NEXT_FIELD(*p_value);
	STD_QUERY_ONE_END( );
}

int Cuser::set_tmp_value(userid_t userid,uint32_t flag,uint32_t value)
{
	sprintf( this->sqlstr, " update %s set \
			tmp_flag=%u, \
			tmp_value=%u \
			where userid=%u " ,
			this->get_table_name(userid),  
			flag,value, userid); 
	STD_SET_RETURN (this->sqlstr,userid,  USER_ID_NOFIND_ERR);
}

int Cuser::add_tmp_value(userid_t userid, uint32_t flag, uint32_t *p_value )
{
	uint32_t db_flag, db_value;	
	int ret;
	ret=this->get_tmp_value(userid, &db_flag,&db_value );
	if (ret!=SUCC) return ret;
	
	if (db_flag==flag){
		*p_value+=db_value+1;
	}else{//是之前活动的标示
		*p_value=1;
	}
	return this->set_tmp_value(userid, flag, *p_value );
}

int Cuser::set_task_value(userid_t userid ,user_set_task_in *p_in )
{
	stru_tasklist tasklist; 
	if (p_in->taskid>=600 || p_in->value>2 ) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	int ret;
	uint32_t value;
	ret=this->get_tasklist(userid, &tasklist );
	if (ret!=SUCC) return ret;
	uint32_t id=p_in->taskid/4;
	uint32_t posx=p_in->taskid%4;
	value=tasklist.list[id];
	//DEBUG_LOG("-------p_in->task_id: %u,------- id: %u,------- posx: %u--------", p_in->taskid, id, posx);
	uint32_t maskvalue=(0xFF-(3<<(posx*2)));

	//得到原有的值
	uint32_t oldvalue=(value-(value & maskvalue))>>(posx*2);
	DEBUG_LOG("old_task_value:%u",oldvalue);
	if (oldvalue>= p_in->value){
		ERROR_LOG("TASK ERR: userid:%u taskid:%u oldvalue:%u  value:%u ",
				userid, p_in->taskid, oldvalue, p_in->value );
	}

	//清空原有的值
	value=(value & maskvalue);
	
	value=value+(p_in->value<<(posx*2));
	tasklist.list[id]=value;
	return  this->set_tasklist(userid, &tasklist );
}

int Cuser::get_hometypeid(userid_t userid ,uint32_t *p_hometypeid)
{
	stru_home_used_attirelist usedlist;  
	int ret;
	ret=this->home_get_used_attirelist(  userid,1, &usedlist);
	if(ret!=SUCC) return ret;
	*p_hometypeid =usedlist.item[0].attireid;
	return SUCC;
}

int Cuser::get_history_ip(userid_t userid, history_ip *p_history_ip )
{
	sprintf( this->sqlstr, "select  history_lastip \
		from %s where userid=%u",
		this->get_table_name(userid),userid );

	STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR );
		BIN_CPY_NEXT_FIELD(p_history_ip ,sizeof (*p_history_ip));	
	STD_QUERY_ONE_END();
}

int Cuser::update_history_ip(userid_t userid, history_ip *p_history_ip )
{

	char history_ip_mysql[mysql_str_len(sizeof(*p_history_ip))];
	set_mysql_string(history_ip_mysql, 
			(char*)(p_history_ip),sizeof(*p_history_ip));
	sprintf( this->sqlstr, "update %s  set history_lastip='%s'\
			where userid=%u ",
		this->get_table_name(userid), 
		history_ip_mysql, userid
		);
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	

}

int Cuser::set_history_ip(userid_t userid, uint32_t lastip )
{
	history_ip history_lastip={};		
	int ret;
	ret=this->get_history_ip(userid,&history_lastip );
	if(ret!=SUCC) return SUCC;

	history_ip_item *_start, *_end,*_find  ; 
	history_ip_item item;  
	item.ip=lastip;
	item.logtime=time(NULL);

	if (ret!=SUCC) return ret;
	_start= &(history_lastip.ipitem[0]);
	_end=_start+3;	
	_find=std::find( _start ,  _end , item );
	if (_find!=_end ) {
		_find->logtime=time(NULL);
	}else{
		//没有找到
		memcpy( &(history_lastip.ipitem[2]), &(history_lastip.ipitem[1]),
				sizeof (history_ip_item ) );

		memcpy( &(history_lastip.ipitem[1]), &(history_lastip.ipitem[0]),
				sizeof (history_ip_item ) );
		memcpy( &(history_lastip.ipitem[0]), &(item),
				sizeof (history_ip_item) );
	}	
	return this->update_history_ip(userid,&history_lastip );
}

int Cuser::set_xiaomee_max_info(userid_t userid ,  user_set_xiaomee_max_info_in * in)
{	
	sprintf( this->sqlstr, " update %s set \
			xiaomee_max=%u, sale_xiaomee_max=%u,Ol_last=%u \
			where userid=%u " ,
			this->get_table_name(userid),  
				in->xiaomee_max,	
				in->sale_xiaomee_max,	
				in->Ol_last,	
		   	userid); 
	STD_SET_RETURN (this->sqlstr,userid,  USER_ID_NOFIND_ERR);
}

int Cuser::add_game(userid_t userid, uint32_t score)
{
		sprintf( this->sqlstr, " update %s set game=game+%u	where userid=%u " ,
			this->get_table_name(userid),  
			score,	
		   	userid
		); 
	
	STD_SET_RETURN (this->sqlstr,userid,  USER_ID_NOFIND_ERR);
}

int Cuser::update_work(userid_t userid, uint32_t work)
{
		sprintf( this->sqlstr, " update %s set work = %u	where userid=%u " ,
			this->get_table_name(userid),  
			work,	
		   	userid
		); 
	
	STD_SET_RETURN (this->sqlstr,userid,  USER_ID_NOFIND_ERR);
}

//小屋相关协议
int Cuser::home_get_noused_attirelist( userid_t userid, stru_noused_attirelist * p_list )
{
	sprintf( this->sqlstr, "select noused_homeattirelist from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD(p_list,sizeof (*p_list));
	STD_QUERY_ONE_END();
}

int  Cuser::home_get_used_attirelist( userid_t userid, uint32_t homeid,
		stru_home_used_attirelist * p_usedlist)
{
	p_usedlist->count=0;
	sprintf( this->sqlstr, "select  homeattirelist_%u from %s where userid=%u ", 
			homeid, this->get_table_name(userid),userid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD (p_usedlist , sizeof(*p_usedlist));
	STD_QUERY_ONE_END();
}

int  Cuser::home_get_edit_list( userid_t userid,uint32_t homeid , 
		stru_noused_attirelist * p_noused_list ,
			stru_home_used_attirelist * p_usedlist )
{
	p_usedlist->count=0;
	sprintf( this->sqlstr, "select noused_homeattirelist , homeattirelist_%u \
			from %s where userid=%u ", 
			 homeid, this->get_table_name(userid),userid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_noused_list,sizeof (*p_noused_list));
		BIN_CPY_NEXT_FIELD (p_usedlist, sizeof(*p_usedlist));
	STD_QUERY_ONE_END();
}

int  Cuser::home_get_all( userid_t userid,stru_noused_attirelist * p_noused_list ,
			stru_home_used_attirelist * p_usedlist_1,
			stru_home_used_attirelist * p_usedlist_2,
			stru_home_used_attirelist * p_usedlist_3)
{
	p_usedlist_2->count=0;
	p_usedlist_3->count=0;
	sprintf( this->sqlstr, "select noused_homeattirelist , homeattirelist_1 ,\
			homeattirelist_2 , homeattirelist_3 \
			from %s where userid=%u ", 
			 this->get_table_name(userid),userid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_noused_list,sizeof (*p_noused_list));
		BIN_CPY_NEXT_FIELD (p_usedlist_1 , sizeof(*p_usedlist_1));
		BIN_CPY_NEXT_FIELD (p_usedlist_2 , sizeof(*p_usedlist_2));
		BIN_CPY_NEXT_FIELD (p_usedlist_3 , sizeof(*p_usedlist_3));
	STD_QUERY_ONE_END();
}

int Cuser::home_update_noused_attirelist( userid_t userid ,
		stru_noused_attirelist	 * p_list )
{
	char mysql_attirelist[mysql_str_len(sizeof(*p_list))];
	set_mysql_string(mysql_attirelist,(char*)p_list, 
			4 +sizeof(stru_attire_noused )* p_list->count);
	sprintf( this->sqlstr, " update %s set \
		noused_homeattirelist='%s' \
		where userid=%u " ,
		this->get_table_name(userid),
		mysql_attirelist,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser::home_add_attire( userid_t userid , attire_count_with_max  * p_item, bool is_vip, uint32_t is_vip_opt_type )
{
	stru_noused_attirelist noused_list ;
	stru_home_used_attirelist usedlist_1={ };
	stru_home_used_attirelist usedlist_2={ };
	stru_home_used_attirelist usedlist_3={ };
	int ret;

	//vip 是1500 非vip 是500
	uint32_t max_item_count;
	if (is_vip){
		max_item_count = HOME_NOUSE_ATTIRE_ITEM_MAX_EX;
	}else{
		max_item_count = 500+500;//HOME_NOUSE_ATTIRE_ITEM_MAX_EX/2;
	}

	ret=this->home_get_all(userid,&noused_list,&usedlist_1,&usedlist_2,&usedlist_3 );
	if(ret!=SUCC)return ret;

	
	ATTIRE_MAPITEM  itemmap;//使用map 临时保存数据
	home_all_to_map(itemmap,noused_list,
			usedlist_1, usedlist_2, usedlist_3 );

	//类型个数
	uint32_t all_type_count=itemmap.size();
	uint32_t attireid=p_item->attireid;

	//得到当前物品个数	
	uint32_t cur_attire_count=0;
	if ( itemmap.find(attireid)!=itemmap.end()) {
		cur_attire_count=itemmap[attireid];
	}
	if (cur_attire_count==0){
		//原来还没有这个物品,检查类型的个数是否超过限制
		if (all_type_count>=max_item_count){
			return USER_ATTIRE_MAX_ERR;
		}
	}

	//检查当前个数是否超过限制
	if (cur_attire_count+p_item->count>p_item->maxcount){
			return VALUE_OUT_OF_RANGE_ERR;
	}

	//增加物品
	ret=add_attire_count( (noused_homeattirelist *) &noused_list , 
			p_item,max_item_count );
	if (ret!=SUCC) return ret;

	DEBUG_LOG("itemid: [ %u ], count:  [%u ]", p_item->attireid, p_item->count);
	this->add_attire_msglog(userid, p_item->attireid, p_item->count);
	
	ret = this->home_update_noused_attirelist( userid,&noused_list);
	/*
	 * 114D协议,下面是道具增减统计支持
	 */
	if(ret == SUCC){
		return this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, p_item->attireid, p_item->count);
	}
	return ret;
	
}

int Cuser::get_home_attire_count(userid_t userid, bool is_vip, user_get_home_attire_count_out* p_out)
{
	stru_noused_attirelist noused_list ;
	stru_home_used_attirelist usedlist_1={ };
	stru_home_used_attirelist usedlist_2={ };
	stru_home_used_attirelist usedlist_3={ };
	int ret;

	//vip 是1500 非vip 是500
	if (is_vip) {
		p_out->max_count = HOME_NOUSE_ATTIRE_ITEM_MAX_EX;
	}else{
		p_out->max_count = 500 + 500;
	}

	ret = this->home_get_all(userid, &noused_list, &usedlist_1, &usedlist_2, &usedlist_3 );
	if (ret != SUCC) {
		return ret;
	}
	
	ATTIRE_MAPITEM  itemmap;//使用map 临时保存数据
	home_all_to_map(itemmap, noused_list, usedlist_1, usedlist_2, usedlist_3);

	//类型个数
	p_out->count = itemmap.size();
/*	uint32_t attireid = p_item->attireid;

	//得到当前物品个数	
	uint32_t cur_attire_count=0;
	if ( itemmap.find(attireid)!=itemmap.end()) {
		cur_attire_count=itemmap[attireid];
	}
	if (cur_attire_count==0){
		//原来还没有这个物品,检查类型的个数是否超过限制
		if (all_type_count>=max_item_count){
			return USER_ATTIRE_MAX_ERR;
		}
	}
	//检查当前个数是否超过限制
	if (cur_attire_count+p_item->count>p_item->maxcount){
			return VALUE_OUT_OF_RANGE_ERR;
	}
*/
	return ret;
}

int Cuser::home_reduce_attire( userid_t userid , attire_count_with_max  * p_item, uint32_t is_vip_opt_type)
{
	stru_noused_attirelist noused_list ;
	int ret;
	ret=this->home_get_noused_attirelist(userid,&noused_list);
	if(ret!=SUCC)return ret;
	ret=reduce_attire_count( (noused_homeattirelist *) &noused_list , 
			p_item);
	if (ret!=SUCC) return ret;
	ret = this->home_update_noused_attirelist( userid,&noused_list);
	if(ret == SUCC){
		return this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, p_item->attireid, -p_item->count);
	}
	return ret;
}

int Cuser::home_edit(  userid_t userid, uint32_t homeid, stru_home_used_attirelist * p_usedlist  )
{
	int ret;
	stru_noused_attirelist old_nousedlist;
	stru_home_used_attirelist old_usedlist;

	if ((ret=this->home_get_edit_list(userid, 
		homeid,& old_nousedlist ,&old_usedlist ))!=SUCC){
		return ret;
	}
	if (p_usedlist->count >=HOME_ATTIRE_ITEM_MAX){ //最多99个
		return VALUE_OUT_OF_RANGE_ERR; 
	}

	if(homeid==1){//第一个小屋,要加入小屋类型字段
		//将原有数据向后偏移
		for(uint32_t i=p_usedlist->count;i>0;i-- ){
			memcpy( &(p_usedlist->item[i]), &(p_usedlist->item[i-1]),
					sizeof(p_usedlist->item[0]));
		}
		//将小屋类型加入
		memcpy( &(p_usedlist->item[0]), &(old_usedlist.item[0]),
					sizeof(p_usedlist->item[0]));
		p_usedlist->count++;
	}

	stru_noused_attirelist new_nousedlist; 
	ret= cal_home_attire_list(&old_usedlist,(noused_homeattirelist* )&old_nousedlist,
			p_usedlist, HOME_NOUSE_ATTIRE_ITEM_MAX_EX,
			(noused_homeattirelist*) &new_nousedlist  )	;
	if (ret!=SUCC){
		return  USER_SET_ATTIRE_DATA_ERR;
	}

	return this->home_edit_update(userid,homeid,&new_nousedlist ,p_usedlist );	
}

int Cuser::home_edit_update( userid_t userid ,uint32_t homeid,
		stru_noused_attirelist	 * p_noused_list, 
		stru_home_used_attirelist * p_used_list )
{
	char mysql_attirelist[mysql_str_len(sizeof(*p_noused_list))];
	set_mysql_string(mysql_attirelist,(char*)p_noused_list, 
			4 +sizeof(stru_attire_noused )* p_noused_list->count);

	char mysql_usedlist[mysql_str_len(sizeof (*p_used_list))];
	set_mysql_string(mysql_usedlist,(char*)p_used_list, 
			4+sizeof(p_used_list->item[0])* p_used_list->count);

	sprintf( this->sqlstr, " update %s set \
		noused_homeattirelist='%s', \
		homeattirelist_%u='%s' \
		where userid=%u " ,
		this->get_table_name(userid),
		mysql_attirelist,
		homeid,
		mysql_usedlist,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser::home_reduce_used_attire( userid_t userid ,uint32_t homeid, uint32_t attireid )
{
	stru_home_used_attirelist used_list;
	int ret;
	ret=this->home_get_used_list(userid,homeid,&used_list );
	if (ret!=SUCC) return ret;

	home_attire_item item ;
	item.attireid=attireid;
	home_attire_item *_start, *_end,*_find , *_tmp ; 
	_start=used_list.item ;
	_end=_start+used_list.count;	
	_find=std::find( _start ,  _end , item );
	if (_find!=_end ) {
		//去掉这一项
		_tmp=_find+1;	
		for (;_tmp!=_end; _tmp++){
			memcpy(_tmp-1, _tmp, sizeof(* _tmp));
		}
		used_list.count--;
	}else{
		return USER_ATTIRE_ID_NOFIND_ERR;
	}	
	return this->home_update_used_list(userid,homeid,&used_list);
}

int Cuser::home_update_used_list( userid_t userid ,uint32_t homeid,
		stru_home_used_attirelist * p_used_list )
{
	char mysql_usedlist[mysql_str_len(sizeof (*p_used_list))];
	set_mysql_string(mysql_usedlist,(char*)p_used_list, 
			4+sizeof(p_used_list->item[0])* p_used_list->count);

	sprintf( this->sqlstr, " update %s set \
		homeattirelist_%u='%s' \
		where userid=%u " ,
		this->get_table_name(userid),
		homeid,
		mysql_usedlist,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int  Cuser::home_get_used_list( userid_t userid, uint32_t homeid,
			stru_home_used_attirelist * p_usedlist)
{
	p_usedlist->count=0;
	sprintf( this->sqlstr, "select homeattirelist_%u \
			from %s where userid=%u ", 
			 homeid, this->get_table_name(userid),userid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD (p_usedlist, sizeof(*p_usedlist));
	STD_QUERY_ONE_END();
}

int Cuser::home_edit_reset_all( userid_t userid, stru_home_used_attirelist * p_new_usedlist,
	stru_noused_attirelist	*p_out_noused_attireidlist )
{
	stru_home_used_attirelist usedlist_1;
	stru_home_used_attirelist usedlist_2;
	stru_home_used_attirelist usedlist_3;
	int ret;
	ret=this->home_get_all(userid,p_out_noused_attireidlist ,&usedlist_1,&usedlist_2,&usedlist_3 );
	if(ret!=SUCC)return ret;
	//将所有物品放回
	
	ATTIRE_MAPITEM  itemmap;//使用map 临时保存数据
	home_all_to_map(itemmap,*p_out_noused_attireidlist,
			usedlist_1, usedlist_2, usedlist_3 );

	DEBUG_LOG("all====:%lu",itemmap.size() );
	if(p_new_usedlist->item[0].attireid==0){
		memcpy(&(p_new_usedlist->item[0]),&(usedlist_1.item[0]) ,
				sizeof(p_new_usedlist->item[0]) );
	}
	uint32_t attireid;
	//set used count
	for (uint32_t i=0;i<p_new_usedlist->count;i++ )	{
		attireid=p_new_usedlist->item[i].attireid;
		if ( itemmap.find(attireid)==itemmap.end()) {
			return  USER_SET_ATTIRE_DATA_ERR;
		}else{
			if (itemmap[attireid]>1 ) {
				itemmap[attireid]--;
			}else if  (itemmap[attireid]==1 ) {
				itemmap.erase(attireid );
			}else {
				return  USER_SET_ATTIRE_DATA_ERR;
			}
		}
	}

	//复制数据未使用
	 p_out_noused_attireidlist->count=itemmap.size();
	 DEBUG_LOG("NO====used:%u",p_out_noused_attireidlist->count);

	if (p_out_noused_attireidlist->count > HOME_NOUSE_ATTIRE_ITEM_MAX_EX ) {
		return  USER_SET_ATTIRE_DATA_ERR;
	}

	ATTIRE_MAPITEM::iterator it;
	uint32_t i=0;
	for( it=itemmap.begin();it!=itemmap.end();++it){
		p_out_noused_attireidlist-> item[i].attireid=it->first;
		p_out_noused_attireidlist-> item[i].count=it->second;
		i++;
	}
	//更新
	return this->home_edit_reset_all_update(userid,p_out_noused_attireidlist , p_new_usedlist );
}

int Cuser::home_edit_reset_all_update( userid_t userid ,
		stru_noused_attirelist	 * p_noused_list, 
		stru_home_used_attirelist * p_used_list )
{
	char mysql_attirelist[mysql_str_len(sizeof(*p_noused_list))];
	set_mysql_string(mysql_attirelist,(char*)p_noused_list, 
			4 +sizeof(stru_attire_noused )* p_noused_list->count);

	char mysql_usedlist[mysql_str_len(sizeof (*p_used_list))];
	set_mysql_string(mysql_usedlist,(char*)p_used_list, 
			4+sizeof(p_used_list->item[0])* p_used_list->count);

	sprintf( this->sqlstr, " update %s set \
		noused_homeattirelist='%s', \
		homeattirelist_1='%s', \
		homeattirelist_2=0x00000000, \
		homeattirelist_3=0x00000000 \
		where userid=%u " ,
		this->get_table_name(userid),
		mysql_attirelist,
		mysql_usedlist,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}


int Cuser::home_get_attire_count( userid_t userid, uint32_t  attireid,
		uint32_t count_flag,  uint32_t *p_count )
{
	stru_noused_attirelist noused_list ;
	stru_home_used_attirelist usedlist_1;
	stru_home_used_attirelist usedlist_2;
	stru_home_used_attirelist usedlist_3;
	int ret;
	ret=this->home_get_all(userid,&noused_list,&usedlist_1,&usedlist_2,&usedlist_3 );
	if(ret!=SUCC)return ret;
	*p_count=0;
	
	if ( count_flag==0 ||count_flag==2 ){//未使用
	 	*p_count+=noused_list.get_attire_count(attireid);
	}

	if ( count_flag==1 ||count_flag==2 ){//已使用
		*p_count+=home_get_attire_used_count(attireid, 
			&usedlist_1,&usedlist_2,&usedlist_3 );
	}

	return SUCC;
}

int Cuser::home_update_used_attirelist_with_check( userid_t userid ,uint32_t  homeid, 
		stru_home_used_attirelist	* p_new_usedlist )
{
	int ret;
	stru_home_used_attirelist old_usedlist;  
	ret=this->home_get_used_list( userid,homeid, &old_usedlist);
	if(ret!=SUCC) return ret;
	if(old_usedlist.count!=p_new_usedlist->count)		{
		DEBUG_LOG("count:err %u %u",old_usedlist.count,p_new_usedlist->count  );
		return USER_SET_ATTIRE_DATA_ERR;
	}	
	for (uint32_t i=0;i<old_usedlist.count;i++ ){
		if (p_new_usedlist->item[i]!=old_usedlist.item[i]){
			DEBUG_LOG("item:err %u %u",p_new_usedlist->item[i].attireid,
				   old_usedlist.item[i].attireid	);
			return  USER_SET_ATTIRE_DATA_ERR;
		}
	}
	return this->home_update_used_list( userid,homeid,p_new_usedlist);	
}

int Cuser::home_swap_attirelist( userid_t userid , 
				uint32_t oldcount,attire_count* oldlist,
				uint32_t newcount,attire_count_with_max * newlist )
{
	stru_noused_attirelist noused_list ;
	int ret;
	ret=this->home_get_noused_attirelist(userid,&noused_list);
	if(ret!=SUCC)return ret;
	
	uint32_t i;
	//删除旧有的
	for (i=0;i<oldcount;i++){
		ret=reduce_attire_count( (noused_homeattirelist *) &noused_list , 
			 (attire_count_with_max*)(oldlist+i) );
		if(ret!=SUCC) return ret;
	}

	for (i=0;i<newcount;i++){
		ret=add_attire_count( (noused_homeattirelist *)  &noused_list, 
				(newlist+i),HOME_NOUSE_ATTIRE_ITEM_MAX_EX );
		if ( ret!=SUCC) return ret;
	}
	return this->home_update_noused_attirelist(userid,&noused_list  );
}

int  Cuser::home_get( userid_t userid, uint32_t homeid,
			stru_home_used_attirelist * p_usedlist_1, stru_home_used_attirelist * p_usedlist )
{

	p_usedlist->count=0;
	sprintf( this->sqlstr, "select homeattirelist_1, homeattirelist_%u \
			from %s where userid=%u ", 
			 homeid, this->get_table_name(userid),userid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD (p_usedlist_1, sizeof(*p_usedlist_1));
		BIN_CPY_NEXT_FIELD (p_usedlist, sizeof(*p_usedlist));
	STD_QUERY_ONE_END();
}

int Cuser::home_reduce_used_attire(userid_t userid, uint32_t attireid  )
{
	int ret;
	stru_noused_attirelist noused_list ;
	stru_home_used_attirelist usedlist_1={ };
	stru_home_used_attirelist usedlist_2={ };
	stru_home_used_attirelist usedlist_3={ };
	ret=this->home_get_all(userid,&noused_list,&usedlist_1,&usedlist_2,&usedlist_3 );
	if(ret!=SUCC) return ret;
	uint32_t homeid=0;
	stru_home_used_attirelist* p_used_list=NULL;
	ret=reduce_used_attire(&usedlist_1,attireid);	
	if(ret!=SUCC){
		ret=reduce_used_attire(&usedlist_2,attireid);	
		if(ret!=SUCC){
			ret=reduce_used_attire(&usedlist_3,attireid);	
			if(ret!=SUCC) return ret;
			homeid=3;
			p_used_list=&usedlist_3;
		}else{
			homeid=2;
			p_used_list=&usedlist_2;
		}
	}else{
		homeid=1;
		p_used_list=&usedlist_1;
	}
	
	//增加物品
	attire_count_with_max item;
	item.attireid=attireid;
	item.count=1;
	item.maxcount=9999;
	ret=add_attire_count( (noused_homeattirelist *) &noused_list , 
			&item,HOME_NOUSE_ATTIRE_ITEM_MAX_EX );
	if (ret!=SUCC) return ret;

	return this->home_edit_update(userid,homeid,&noused_list ,p_used_list );	
}

int Cuser::check_lock_time(userid_t userid)
{
	uint32_t end_time = 0;
	uint32_t login_time = 0;
	sprintf(this->sqlstr, "select end_time, login_time from  %s where userid=%u",
		 	this->get_table_name(userid),
			 userid
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR );
		INT_CPY_NEXT_FIELD(end_time);
		INT_CPY_NEXT_FIELD(login_time);
	STD_QUERY_ONE_END_WITHOUT_RETURN();
	uint32_t now = time(NULL);
	if (now < end_time) {
		if (now < login_time) {
			return SUCC;
		} else {
			return 1;
		}
	} else {
		return 1;
	}
}

/* 得到用户没有的模特 */
int Cuser::home_get_mode_index( userid_t userid, uint32_t *p_mode_index) 
{
	stru_noused_attirelist noused_list ;
	stru_home_used_attirelist usedlist_1;
	stru_home_used_attirelist usedlist_2;
	stru_home_used_attirelist usedlist_3;
	int ret;
	ret=this->home_get_all(userid,&noused_list,&usedlist_1,&usedlist_2,&usedlist_3 );
	if(ret!=SUCC)return ret;
	uint32_t count = 0;
	for (uint32_t attireid = 160549; attireid <= 160598; attireid++) {
		count = 0;
	 	count+=noused_list.get_attire_count(attireid);
		count+=home_get_attire_used_count(attireid, &usedlist_1, &usedlist_2, &usedlist_3);
		if (count == 0) {
			*p_mode_index = attireid;
			break;
		}
	}
	return SUCC;
}

int Cuser::home_dress_in_photo_set(const userid_t userid, const db_utility::DressingUnit& dress)
{
	user_without_id  t={};
	insert(userid, &t);

	char mysql_list[mysql_str_len(sizeof(dress))];
	set_mysql_string(mysql_list, (char*)(&dress), sizeof(dress));

	sprintf(this->sqlstr, "update %s set dress_in_photo='%s' where userid=%u", 
			 this->get_table_name(userid),
			 mysql_list,
			 userid);
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

int Cuser::home_dress_in_photo_get(const userid_t userid, db_utility::DressingUnit& dress)
{
	sprintf(this->sqlstr, " select dress_in_photo from %s where userid=%d", 
			this->get_table_name(userid),
			userid
		   );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(&dress, sizeof(dress));
	STD_QUERY_ONE_END();
}

int Cuser::get_noah_info(userid_t userid, user_get_noah_info_out& out )
{
		sprintf( this->sqlstr, "select \
				xiaomee,\
				exp,\
				strong,\
				iq,\
				charm,\
				petbirthday\
		from %s where userid=%u",this->get_table_name(userid),userid );

		STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR );
			out.xiaomee=atoi_safe(NEXT_FIELD );
			out.exp=atoi_safe(NEXT_FIELD );
			out.strong=atoi_safe(NEXT_FIELD );
			out.iq=atoi_safe(NEXT_FIELD );
			out.charm=atoi_safe(NEXT_FIELD );
			out.birthday=atoi_safe(NEXT_FIELD );
		STD_QUERY_ONE_END();
}

int Cuser::add_attire_msglog(userid_t userid, uint32_t attireid, uint32_t count)
{
	uint32_t log_type = 0;
	if (attireid >= 12001 && attireid <=15999)//服饰
	{
		log_type = 0x02010000+attireid-12000;

	}
	else if(attireid >= 160001 && attireid <= 169999)//小屋道具
	{
		log_type = 0x02011000+attireid-160000;
	}
	else if(attireid >= 180001 && attireid <= 189999)//喂养道具
	{
		log_type = 0x02004000+attireid-180000;
	}
	else if(attireid >= 190001 && attireid <= 199999)//收集品
	{
		log_type = 0x02014000+attireid-190000;
	}

	else if(attireid >= 1220001 && attireid <= 1229999)//家园装扮
	{
		log_type = 0x02017000+attireid-1220000;
	}

	else if(attireid >= 1230001 && attireid <= 1239999)//种子
	{
		log_type = 0x0201a000+attireid-1230000;
	}

	DEBUG_LOG("itemid_1: [ %u ], count_1:  [%u ], log_type: [ %x]", attireid, count, log_type);

	if (log_type > 0)
	{
		struct USERID_NUM{
			uint32_t id;
			uint32_t num;
		};

		USERID_NUM s_userid_num = {};
		s_userid_num.id = userid ;
		s_userid_num.num = count;

		msglog(this->msglog_file, log_type, time(NULL), &s_userid_num, sizeof(s_userid_num));

	}
	
	return SUCC;
}

int Cuser::update_xiaomee_inc(userid_t userid, int32_t xiaomee_inc)
{
	int pocket_xiaomee = 0;
	int m_ret = cal_total_xiaomee(userid, &xiaomee_inc, &pocket_xiaomee);
	if(m_ret != 1){
		sprintf( this->sqlstr, "update %s set xiaomee = if(xiaomee + %u > %u, %u, xiaomee + %u) \
				where userid=%u " ,
			this->get_table_name(userid), 
			xiaomee_inc,
			MOLE_XIAOMEE_MAX,
			MOLE_XIAOMEE_MAX,
			xiaomee_inc,
			userid
			);

		STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR);	
	}
	return SUCC;

}
/*
 * 此函数禁止用
 */
int Cuser::update_xiaomee(userid_t userid, uint32_t xiaomee)
{
	sprintf( this->sqlstr, "update %s set xiaomee = %u where userid=%u " ,
		this->get_table_name(userid), 
		xiaomee,
		userid
		);

	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR);	

}

int Cuser::update_noah_info_inc(userid_t userid, user_noah_inc_user_info_in* p_in)
{
	sprintf( this->sqlstr, "update %s set \
		xiaomee = if(xiaomee + %u > %u, %u, xiaomee + %u),\
		exp = exp + %u,\
		strong = strong + %u,\
		iq = iq + %u,\
		charm = charm + %u\
		where userid=%u",
		this->get_table_name(userid),
		p_in->inc_xiaomee,
		MOLE_XIAOMEE_MAX,
		MOLE_XIAOMEE_MAX,
		p_in->inc_xiaomee,
		p_in->inc_exp,
		p_in->inc_strong,
		p_in->inc_iq,
		p_in->inc_charm,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR);

}

int Cuser::get_field_uint32(userid_t userid, const char *field, uint32_t &value)
{
	sprintf(this->sqlstr, "select %s from %s where userid=%u",
		field, this->get_table_name(userid),userid);

        STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
                INT_CPY_NEXT_FIELD(value);
        STD_QUERY_ONE_END();
}

int Cuser::get_user_board_info(userid_t userid, user_get_user_board_info_out_header * p_out )
{
		sprintf( this->sqlstr, "select \
				petcolor,\
				petbirthday,\
				xiaomee,\
				exp,\
				strong,\
				iq,\
				charm,\
				game,\
				work,\
				fashion\
		from %s where userid=%u",this->get_table_name(userid),userid );

		STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR );
			p_out->petcolor=atoi_safe(NEXT_FIELD );
			p_out->petbirthday=atoi_safe(NEXT_FIELD );
			p_out->xiaomee=atoi_safe(NEXT_FIELD );
			p_out->exp=atoi_safe(NEXT_FIELD );
			p_out->strong=atoi_safe(NEXT_FIELD );
			p_out->iq=atoi_safe(NEXT_FIELD );
			p_out->charm=atoi_safe(NEXT_FIELD );
			p_out->game=atoi_safe(NEXT_FIELD );
			p_out->work=atoi_safe(NEXT_FIELD );
			p_out->fashion=atoi_safe(NEXT_FIELD );
		STD_QUERY_ONE_END();
}

int Cuser::get_user_exp(userid_t userid, uint32_t& exp)
{
	sprintf( this->sqlstr, "select exp\
		from %s where userid=%u",this->get_table_name(userid),userid );

	STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR );
		exp=atoi_safe(NEXT_FIELD );
	STD_QUERY_ONE_END();
}

int Cuser::get_user_exp_color(userid_t userid, uint32_t& exp, uint32_t &color)
{
	sprintf( this->sqlstr, "select exp, petcolor\
		from %s where userid=%u",this->get_table_name(userid),userid );

	STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR );
		exp=atoi_safe(NEXT_FIELD );
		color=atoi_safe(NEXT_FIELD );
	STD_QUERY_ONE_END();
}


int Cuser::update_exp_inc(userid_t userid, uint32_t exp_inc, uint32_t strong_inc)
{
	sprintf( this->sqlstr, "update  %s  set exp= exp + %u, strong=strong + %u \
			where userid=%u " ,
		this->get_table_name(userid), 
		exp_inc,
		strong_inc,
		userid);

	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR);	

}

int Cuser::get_teamid(userid_t userid,uint32_t* data)
{
    sprintf(this->sqlstr, "select teamid from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		*data=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}
int Cuser::get_one_col(userid_t userid, const char* col, uint32_t *value)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u",
			col,
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		*value=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}
int Cuser::update_one_col(userid_t userid, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u",
			this->get_table_name(userid),
			col,
			value,
			userid
			);

	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR);	
}
