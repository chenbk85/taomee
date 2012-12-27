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
#include "Cuser_ex.h"
#include <time.h> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include <algorithm>
//user 
Cuser_ex::Cuser_ex(mysql_interface * db):
	CtableRoute100x10( db , "USER" , "t_user_ex" , "userid") 
{ 

}

int Cuser_ex::get_jyaccess_list(userid_t userid, access_jylist *p_out )
{
	sprintf( this->sqlstr, "select jy_accesslist \
			from %s where  userid=%d ", 
			 this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD (p_out, sizeof ( *p_out) );
	STD_QUERY_ONE_END();
}

int Cuser_ex::init(userid_t userid, char *  nick )
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql,nick,NICK_LEN);

	sprintf( this->sqlstr, "insert into %s("
		"userid,"
		"flag,"
		"nick,"
		"friendlist,"
		"blacklist,"
		"jy_accesslist,"
		"tmpbuf,"
		"professionlist,"
		"classlist,"
		"class_firstid,"
		"diary_lock,"
		"level,"
		"driver,"
		"show_auto,"
		"pic_clothe,"
		"vip_mark_last,"
		"vip_start_time,"
		"vip_end_time"
	") values ("
		"%u,"
		"0,"
		"'%s',"
		"0x00000000,"
		"0x00000000,"
		"0x00000000 ,"
		"0x00,"
		"0x00000000,"
		"0x00000000,"
		"0,"
		"0,"
		"0,"
		"0,"
		"0,"
		"0x00000000,"//pic_clothe
		"0,"
		"0,"
		"0"//vip_end_time
		")", 
	this->get_table_name(userid), userid,nick_mysql );

	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_ex::access_jy (userid_t userid, user_access_jy_in *p_in )
{
	int ret;
	access_jylist jylist;	
	ret=this->get_jyaccess_list(userid,&jylist );
	if(ret!=SUCC) return ret;
	DEBUG_LOG("=opt type %u",p_in->opt_type);
	mms_opt_item * find_index, *idstart, *idend;

	//用于从指示从那里开始复制
	mms_opt_item 	 *cpy_index;	
	idstart=&(jylist.item[0]) ;
	idend=idstart+jylist.count;	
	find_index=std::find( idstart,idend, (*p_in)  );
	if (find_index!=idend ) {
		//找到,保留原有的浇水,除虫标志
		p_in->opt_type=find_index->opt_type;			
		cpy_index=find_index;
	}else{
		//没有找到
		p_in->opt_type=0;			
		jylist.count++;
		if (jylist.count>JY_ACCESS_MAX){
			jylist.count=JY_ACCESS_MAX;
			//指向最后一项数据
			cpy_index=find_index-1;
		}else{
			cpy_index=find_index;
		}
	}
	//复制数据
	while( cpy_index!=idstart ){
		memcpy(cpy_index,cpy_index-1,sizeof(*cpy_index));
		cpy_index--;	
	}
	DEBUG_LOG("opt type %u", p_in->opt_type);
	//第一个数据填上
	memcpy(idstart, p_in ,sizeof(*cpy_index));

	//更新
	return this->update_jy_accesslist(userid,&jylist);

}

int Cuser_ex::update_jy_accesslist( userid_t userid, access_jylist *p_list ) 
{
	char mysql_list[mysql_str_len(sizeof (*p_list))];
	set_mysql_string(mysql_list,(char*)p_list, 
			4+sizeof(p_list->item[0])* p_list->count);
	sprintf( this->sqlstr, " update %s set jy_accesslist ='%s' \
		where  userid=%u " ,
		this->get_table_name(userid), 
		mysql_list, userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser_ex::jy_accesslist_set_opt (userid_t userid, uint32_t opt_userid ,uint32_t opt_type )
{
	int ret;
	access_jylist jylist;	

	mms_opt_item item;
	item.userid=opt_userid;

	ret=this->get_jyaccess_list(userid,&jylist );
	if(ret!=SUCC) return ret;

	mms_opt_item * find_index, *idstart, *idend;
	//用于从指示从那里开始复制
	mms_opt_item 	 *cpy_index;	

	idstart=&(jylist.item[0]) ;
	idend=idstart+jylist.count;	
	find_index=std::find( idstart,idend, item  );
	if (find_index==idend ) {
		//没有找到就算了
		return SUCC ;
	}

	//找到 + 加入相关操作标志
	find_index->opt_type |= opt_type;			
	memcpy(&item ,find_index,sizeof(item) );	

	cpy_index=find_index;


	//偏移复制数据
	while( cpy_index!=idstart ){
		memcpy(cpy_index,cpy_index-1,sizeof(*cpy_index));
		cpy_index--;	
	}

	//第一个数据填上
	memcpy(idstart, &item ,sizeof(*idstart));

	//更新
	return this->update_jy_accesslist(userid,&jylist);

}

int Cuser_ex::add_friend(userid_t userid , userid_t id )
{
	uint32_t flag2;
	id_list friendlist={ };
	id_list blacklist={ };
	uint32_t * find_index, *idstart, *idend;	
	uint32_t allow_max_count=0;
	int ret;

	ret=this->get_friend_black_list(userid ,&flag2  , &friendlist, &blacklist);
	if(ret!=SUCC) return ret;

	//检查是否在blacklist中
	idstart=blacklist.item;
	idend=idstart+blacklist.count;	
	
	find_index=std::find( idstart ,  idend , id );
	if (find_index!=idend ) {
		//find 
		return ADD_FRIEND_EXISTED_BLACKLIST_ERR;
	}

	if ((flag2&USER_VIPFLAG_VIP )==USER_VIPFLAG_VIP ){ //vip
		allow_max_count=200;
	}else{
		allow_max_count=100;
	}

	idstart=friendlist.item;
	idend=idstart+friendlist.count;	
	
	find_index=std::find( idstart ,  idend ,  id );
	if (find_index!=idend ) {
		//find 
		return LIST_ID_EXISTED_ERR;
	}else if ( friendlist.count >= allow_max_count){
		//out of max value
		return LIST_ID_MAX_ERR;
	}else{
		friendlist.item[friendlist.count ]=id;
		friendlist.count++;
		return this->update_idlist(userid,FRIEND_LIST_STR ,&friendlist);
	}
}

int Cuser_ex::get_friend_black_list(userid_t userid ,uint32_t *p_flag2  , id_list *p_friendlist, 
	   id_list *  p_blacklist )
{
	sprintf( this->sqlstr, "select flag, friendlist, blacklist\
			from %s where userid=%u ", 
		 this->get_table_name(userid), userid);

	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		//copy idlist  
		 INT_CPY_NEXT_FIELD(*p_flag2 ) ;	
		BIN_CPY_NEXT_FIELD(p_friendlist , sizeof(*p_friendlist ));
		BIN_CPY_NEXT_FIELD(p_blacklist, sizeof(*p_blacklist ));
	STD_QUERY_ONE_END();
}

int Cuser_ex::update_idlist(userid_t userid , const char * id_flag_str ,  id_list * idlist )
{
	char mysql_idlist[mysql_str_len(sizeof(*idlist ) )];
	set_mysql_string(mysql_idlist,(char*)idlist,
			ID_LIST_HEADER_LEN +sizeof (userid_t)*(idlist->count));
	sprintf( this->sqlstr, " update %s set \
		%s ='%s'\
		where userid=%u " ,
		this->get_table_name(userid), 
		id_flag_str,
		mysql_idlist,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser_ex::del_id(userid_t userid ,const char * id_flag_str , userid_t id )
{
	int ret;
	id_list idlist={ };
	uint32_t * new_idend, *idstart, *idend;	
	ret=this->get_idlist(userid,id_flag_str,&idlist);
	if (ret==SUCC){
		idstart=idlist.item;
		idend=idstart+idlist.count;	
		new_idend=std::remove( idstart ,  idend ,  id );
		if (new_idend != idend) {
			idlist.count=new_idend-idstart;	
			return this->update_idlist(userid,id_flag_str ,&idlist);
		}else{
			return LIST_ID_NOFIND_ERR ;
		}

	}else{
		return ret;
	}
}

int Cuser_ex::get_idlist(userid_t userid ,const  char * id_flag_str , id_list * p_idlist )
{
	memset(p_idlist,0,sizeof(*p_idlist) );
	sprintf( this->sqlstr, "select  %s \
			from %s where userid=%u ", 
		id_flag_str,this->get_table_name(userid), userid);

	//copy idlist  
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_idlist, sizeof(*p_idlist ));
	STD_QUERY_ONE_END();
}

int Cuser_ex::set_flag(userid_t userid  ,const  char * flag_type  ,  uint32_t flag_bit ,  bool is_true )
{
	uint32_t flag;
	int ret;
	bool old_is_true;//曾经是VIP会员？
	ret =this->get_flag(userid, flag_type ,&flag);
	if (ret!=SUCC)
	{
			return ret;
	}	

	DEBUG_LOG("---flag[%X]--bit[%X]--&[%X]-",flag,flag_bit,flag & flag_bit );

	if ( (flag & flag_bit) == flag_bit ) old_is_true=true;
	else old_is_true =false; 

	if (old_is_true==is_true) //已经设置了
	{
		return FLAY_ALREADY_SET_ERR;	
	}
		
	if (is_true )
	{
		flag+=flag_bit;  		
	} else {
		flag-=flag_bit;
	}

	return this->update_flag(userid, flag_type ,flag);
}

int Cuser_ex::get_flag(userid_t userid ,const char * flag_type   ,  uint32_t * flag)
{
	sprintf( this->sqlstr, "select  %s from %s where userid=%u ", 
		flag_type, this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
			*flag=atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END();
}

int Cuser_ex::update_flag(userid_t userid ,const char * flag_type  , uint32_t  flag)
{
	sprintf( this->sqlstr, "update %s set %s =%u where userid=%u " ,
		this->get_table_name(userid), flag_type,flag,userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser_ex::check_black(userid_t userid , userid_t friendid, uint32_t *p_is_existed)
{
	int ret;
	id_list idlist={ };
	uint32_t * find_index, *idstart, *idend;	
	ret=this->get_idlist(userid,"blacklist",&idlist);
	if (ret==SUCC){
		idstart=idlist.item;
		idend=idstart+idlist.count;	

		find_index=std::find( idstart ,  idend , friendid  );
		if (find_index!=idend ) {
			*p_is_existed=1;
		}else{
			*p_is_existed=0;
		}
		return SUCC;
	}else{
		return ret;
	}
}

int Cuser_ex::check_friend(userid_t userid , userid_t friendid, uint32_t *p_is_existed  )
{
	int ret;
	id_list idlist={ };
	uint32_t * find_index, *idstart, *idend;	
	ret=this->get_idlist(userid,FRIEND_LIST_STR,&idlist);
	if (ret==SUCC){
		idstart=idlist.item;
		idend=idstart+idlist.count;	

		find_index=std::find( idstart ,  idend , friendid  );
		if (find_index!=idend ) {
			//find 
			*p_is_existed=1;
		}else{
			*p_is_existed=0;
		}
		return SUCC;
	}else{
		return ret;
	}
}

int Cuser_ex::add_id(userid_t userid ,const  char * id_flag_str , userid_t id, 
		uint32_t allow_max_count )
{
	int ret;
	id_list idlist={ };
	uint32_t * find_index, *idstart, *idend;	

	ret=this->get_idlist(userid,id_flag_str,&idlist);
	if (ret==SUCC){

		idstart=idlist.item;
		idend=idstart+idlist.count;	
		
		find_index=std::find( idstart ,  idend ,  id );
		if (find_index!=idend ) {
			//find 
			return LIST_ID_EXISTED_ERR;
		}else if ( idlist.count >= allow_max_count){
			//out of max value
			return LIST_ID_MAX_ERR;
		}else{
			idlist.item[idlist.count ]=id;
			idlist.count++;
			return this->update_idlist(userid,id_flag_str ,&idlist);
		}
	}else{
		return ret;
	}
}

int Cuser_ex::get_nick_fiendlist(userid_t userid , char * nick , id_list * idlist )
{
	sprintf( this->sqlstr, "select  nick, friendlist \
			from %s where userid=%u ", 
		this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(nick, NICK_LEN );
		BIN_CPY_NEXT_FIELD(idlist, sizeof(id_list ));
	STD_QUERY_ONE_END();
}

int Cuser_ex::login(userid_t userid , user_login_ex_out  *p_out  )
{
	sprintf( this->sqlstr, "select flag &0x01, friendlist  \
			from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->vip_flag);
		BIN_CPY_NEXT_FIELD(&(p_out->friendcount),4+sizeof(userid_t)*FRIEND_COUNT_MAX);
	STD_QUERY_ONE_END();
}

int Cuser_ex::get_classlist(userid_t userid ,stru_user_class_list *p_out ) 
{
	memset(p_out,0,sizeof(*p_out));
	sprintf( this->sqlstr, "select  class_firstid, classlist \
		  	from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->class_firstid);
		BIN_CPY_NEXT_FIELD(&(p_out->classlist), sizeof(p_out->classlist) );
	STD_QUERY_ONE_END();
}

int Cuser_ex::get_info(userid_t userid ,stru_user_other *p_out ) 
{
	uint32_t login_vip_mark = 0;
	sprintf( this->sqlstr, "select \
			flag,\
			login_vip_mark,\
			vip_mark_last,\
			vip_start_time,\
			vip_end_time,\
			vip_pet_level,\
			vip_is_auto_charge,\
			nick,\
			class_firstid,\
			classlist,\
			level,\
			driver, \
			build_level, \
			nourish_level, \
			professionlist, \
			event_flag, \
			event_flag_ex, \
			sendmail_flag \
		from %s where userid=%u ", 
			this->get_table_name(userid),userid);

	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			INT_CPY_NEXT_FIELD(p_out->flag);
			INT_CPY_NEXT_FIELD(login_vip_mark);
			INT_CPY_NEXT_FIELD(p_out->vip_mark_last);
			p_out->vip_mark_last += login_vip_mark;
			INT_CPY_NEXT_FIELD(p_out->vip_start_time);
			INT_CPY_NEXT_FIELD(p_out->vip_end_time);
			INT_CPY_NEXT_FIELD(p_out->vip_pet_level);
			INT_CPY_NEXT_FIELD(p_out->vip_is_auto_charge);
			BIN_CPY_NEXT_FIELD(p_out->nick, NICK_LEN );
			INT_CPY_NEXT_FIELD(p_out->class_firstid);
			BIN_CPY_NEXT_FIELD(&(p_out->classlist), sizeof(p_out->classlist) );
			INT_CPY_NEXT_FIELD(p_out->level);
			INT_CPY_NEXT_FIELD(p_out->driver);
			INT_CPY_NEXT_FIELD(p_out->build_level);
			INT_CPY_NEXT_FIELD(p_out->nourish_level);
			BIN_CPY_NEXT_FIELD(p_out->professionlist, 200);
			INT_CPY_NEXT_FIELD(p_out->event_flag[0]);
			BIN_CPY_NEXT_FIELD(&(p_out->event_flag[1]),28);
			INT_CPY_NEXT_FIELD(p_out->sendmail_flag);
	STD_QUERY_ONE_END();

}

int Cuser_ex::get_vip_nick(userid_t userid ,user_get_vip_nick_out  *p_out )
{
	sprintf( this->sqlstr, "select  flag&0x03, nick \
		  	from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			INT_CPY_NEXT_FIELD(p_out->vipflag	);
			BIN_CPY_NEXT_FIELD(p_out->nick, NICK_LEN );
	STD_QUERY_ONE_END();

}

int Cuser_ex::get_vip(userid_t userid ,bool * is_vip )
{
	uint32_t flag;
	int ret;
	ret=this->get_int_value(userid,USER_EX_FLAG_STR,&flag );
	if(ret!=SUCC) return ret;
	*is_vip=((flag&0x01)==0x01);
	return ret;
}

int Cuser_ex::get_nick(userid_t userid , char  *p_nick)
{
	sprintf( this->sqlstr, "select   nick \
		  	from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD(p_nick, NICK_LEN );
	STD_QUERY_ONE_END();
}

int Cuser_ex::get_user_color_nick(userid_t userid, char* p_nick, uint32_t *color)
{
	sprintf(this->sqlstr, "select nick, petcolor from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD(p_nick, NICK_LEN );
			INT_CPY_NEXT_FIELD(*color);
	STD_QUERY_ONE_END();
}

int Cuser_ex::get_info(userid_t userid , user_get_user_all_out *p_out )
{
	sprintf( this->sqlstr, "select"
		" flag,"
		" nick,"
		" driver,"
		" level,"
		" vip_mark_last,"
		" vip_start_time,"
		" vip_end_time,"
		" vip_is_auto_charge,"
		" vip_charge_changel_last, "
		" build_level, "
		" nourish_level "
		"from %s where userid=%u ", 
	this->get_table_name(userid),userid);

	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			INT_CPY_NEXT_FIELD(p_out->flag2);
			BIN_CPY_NEXT_FIELD(p_out->nick, NICK_LEN );
			INT_CPY_NEXT_FIELD(p_out->driver);
			INT_CPY_NEXT_FIELD(p_out->level);
			INT_CPY_NEXT_FIELD(p_out->vip_mark_last);
			INT_CPY_NEXT_FIELD(p_out->vip_start_time);
			INT_CPY_NEXT_FIELD(p_out->vip_end_time);
			INT_CPY_NEXT_FIELD(p_out->vip_is_auto_charge);
			INT_CPY_NEXT_FIELD(p_out->vip_charge_changel_last);
			INT_CPY_NEXT_FIELD(p_out->build_level);
			INT_CPY_NEXT_FIELD(p_out->nourish_level);
	STD_QUERY_ONE_END();
}

int Cuser_ex::get_tmpbuf(userid_t userid ,user_get_tempbuf_out *p_out )
{
	sprintf( this->sqlstr, "select   tmpbuf \
		  	from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD(p_out , sizeof(*p_out ) );
	STD_QUERY_ONE_END();
}

int Cuser_ex::set_professionlist(userid_t userid  ,  user_profession_set_in*p_in )
{
	stru_user_profession_list list;
	int ret;
	ret=this->get_professionlist(userid,&list );
	if (ret!=SUCC) return ret;
	if (p_in->profession_id>=PROFESSION_COUNT){
		return ENUM_OUT_OF_RANGE_ERR;
	}
	list.profession_value[p_in->profession_id]=p_in->profession_value;
	
	return this->update_professionlist(userid,&list);
}

int Cuser_ex::update_professionlist(userid_t userid  ,  stru_user_profession_list *p_in )
{
	char professionlist_mysql[mysql_str_len(sizeof(*p_in) )];
	set_mysql_string(professionlist_mysql, (char*)p_in , sizeof(*p_in) );
	sprintf( this->sqlstr, " update %s set professionlist='%s' \
		   			where userid=%u " ,
				this->get_table_name(userid), 
				 professionlist_mysql , userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser_ex::get_professionlist(userid_t userid ,stru_user_profession_list *p_out )
{
	memset(p_out,0,sizeof(*p_out ) );
	sprintf( this->sqlstr, "select   professionlist \
		  	from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD(p_out , sizeof(*p_out ) );
	STD_QUERY_ONE_END();
}

int Cuser_ex::update_tmpbuf(userid_t userid  ,  user_set_tempbuf_in *p_in )
{
	char tmpbuf_mysql[mysql_str_len(sizeof(*p_in) )];
	set_mysql_string(tmpbuf_mysql, (char*)p_in , sizeof(*p_in) );
	sprintf( this->sqlstr, " update %s set tmpbuf='%s' \
		   			where userid=%u " ,
				this->get_table_name(userid), 
				 tmpbuf_mysql , userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser_ex::update_nick(userid_t userid  ,  char * nick )
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql,
			set_space_end(nick, NICK_LEN ), NICK_LEN);
	sprintf( this->sqlstr, " update %s set nick='%s' \
		   			where userid=%u " ,
				this->get_table_name(userid), 
				 nick_mysql , userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser_ex::set_lock(userid_t userid, uint32_t lock) 
{
	if (lock > 2 || lock < 1) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	uint32_t state = 0;
	uint32_t ret = this->get_lock(userid, &state);
	if (ret != SUCC) {
		return ret;
	}
	if ((lock == 2) && (lock == state)) {
		return DIARY_IS_LOCKED_ERR;
	}

	if ((lock == 1) && (lock == state)) {
		return DIARY_IS_OPEN_ERR;
	}

	ret = update_lock(userid, lock);
	return ret;
}

int Cuser_ex::update_lock(userid_t userid  ,  uint32_t lock)
{
	sprintf( this->sqlstr, " update %s set diary_lock=%u \
		   			where userid=%u",
			this->get_table_name(userid), 
			lock ,
			userid
		);
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}

int Cuser_ex::get_lock(userid_t userid, uint32_t *p_out )
{
	sprintf( this->sqlstr, "select diary_lock from %s where userid=%u ", 
			this->get_table_name(userid),
			userid
		  );
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			INT_CPY_NEXT_FIELD(*p_out);
	STD_QUERY_ONE_END();
}

int Cuser_ex::update_driver(userid_t userid  ,  uint32_t onlinetime)
{
	sprintf( this->sqlstr, " update %s set driver = driver + if(driver > 0, %u, 0) \
		   			where userid = %u",
			this->get_table_name(userid), 
			onlinetime,
			userid
		);
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}

int Cuser_ex::get_pic_clothe(userid_t userid, user_ex_pic_clothe  *p_out)
{
	memset(p_out,0,sizeof(*p_out ) );
	sprintf( this->sqlstr, "select pic_clothe from %s where userid = %u ", 
			this->get_table_name(userid),
			userid
		   );
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD(p_out , sizeof(*p_out));
	STD_QUERY_ONE_END();
}

int Cuser_ex::update_pic_clothe(userid_t userid, user_ex_pic_clothe *p_in)
{
	char tmpbuf_mysql[mysql_str_len(sizeof(*p_in) )];
	set_mysql_string(tmpbuf_mysql, (char*)p_in , sizeof(*p_in) );
	sprintf(this->sqlstr, " update %s set pic_clothe = '%s' where userid = %u " ,
			this->get_table_name(userid), 
			tmpbuf_mysql,
			userid
			);
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}

int Cuser_ex::vip_start_end_time_set(const user_set_vip_flag_in& in)
{
	sprintf(this->sqlstr, "update %s set"
				" vip_start_time=%u,"
				" vip_end_time=%u,"
				" vip_is_auto_charge=%u,"
				" vip_charge_changel_last=%u"
			" where userid=%u",
		this->get_table_name(_userid),
		in.vip_start_time,
		in.vip_end_time,
		in.vip_is_auto_charge,
		in.vip_charge_changel_last,
		_userid
	);
	STD_SET_RETURN(this->sqlstr,_usrid, USER_ID_NOFIND_ERR);
}

int Cuser_ex::vip_mark_last_set(const uint32_t vip_mark_last)
{
	sprintf(this->sqlstr, "update %s set  vip_mark_last=%u where userid=%u",
		this->get_table_name(_userid),
		vip_mark_last,
		_userid
	);
	STD_SET_RETURN(this->sqlstr,_usrid, USER_ID_NOFIND_ERR);
}

int Cuser_ex::user_vip_set_flag_no_check(const user_set_vip_flag_in& in)
{
	sprintf(this->sqlstr, "update %s set"
		" level=%u,"
		" vip_start_time=%u,"
		" vip_end_time=%u,"
		" vip_is_auto_charge=%u,"
		" vip_charge_changel_last=%u"
		" where userid=%u",
		this->get_table_name(_userid),
		in.level,
		in.vip_start_time,
		in.vip_end_time,
		in.vip_is_auto_charge,
		in.vip_charge_changel_last,
		_userid
	);
	STD_SET_RETURN(this->sqlstr,_usrid, USER_ID_NOFIND_ERR);
}

int Cuser_ex::update_build_level_inc(userid_t userid, uint32_t build_level)
{
	sprintf( this->sqlstr, " update %s set build_level = build_level + %u \
		   			where userid = %u",
			this->get_table_name(userid), 
			build_level,
			userid
		);
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}

int Cuser_ex::reset_event_flag(userid_t userid, uint32_t index)
{
	uint32_t bits[8]={0};

        if(index < 1 || index > sizeof(bits) * 8) {
                DEBUG_LOG("reset_event_flag %u not in [1,%lu]",index,sizeof(bits) * 8);
                return SUCC;
        }

        sprintf(this->sqlstr, "select event_flag,event_flag_ex from %s where userid=%u",
                this->get_table_name(userid),userid);
        STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
                INT_CPY_NEXT_FIELD(bits[0]);
                BIN_CPY_NEXT_FIELD(&bits[1],sizeof(bits) - 4);
        STD_QUERY_ONE_END_WITHOUT_RETURN();

        int n = (--index) / 32;
        uint32_t val = (1 << (index % 32));

        if(!(bits[n] & val)) {
                DEBUG_LOG("reset_event_flag[%d] & %u not 1",n,val);
                return SUCC;
        }

        bits[n] &= ~val;
        char sql_bits[mysql_str_len(sizeof(bits) - 4)];
        set_mysql_string(sql_bits, (char*)&bits[1] , sizeof(bits) - 4);

        sprintf(this->sqlstr, "update %s set event_flag = %u,event_flag_ex = '%s' where userid = %u",
                this->get_table_name(userid),bits[0],sql_bits,userid);
        STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

int Cuser_ex::set_event_flag(userid_t userid, uint32_t index)
{
	uint32_t bits[8]={0};
	
	if(index < 1 || index > sizeof(bits) * 8) {
		DEBUG_LOG("set_event_flag %u not in [1,%lu]",index,sizeof(bits) * 8);
		return SUCC;
	}

	sprintf(this->sqlstr, "select event_flag,event_flag_ex from %s where userid=%u",
		this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
        	INT_CPY_NEXT_FIELD(bits[0]);
        	BIN_CPY_NEXT_FIELD(&bits[1],sizeof(bits) - 4);
        STD_QUERY_ONE_END_WITHOUT_RETURN();
	
	int n = (--index) / 32;
	uint32_t val = (1 << (index % 32));

	if(bits[n] & val) {
		DEBUG_LOG("set_event_flag[%d] & %u not 0",n,val);
		return SUCC;
	}
 
	bits[n] |= val;
	char sql_bits[mysql_str_len(sizeof(bits) - 4)];
        set_mysql_string(sql_bits, (char*)&bits[1] , sizeof(bits) - 4);

	sprintf(this->sqlstr, "update %s set event_flag = %u,event_flag_ex = '%s' where userid = %u",
		this->get_table_name(userid),bits[0],sql_bits,userid);
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}

int Cuser_ex::set_sendmail_flag(userid_t userid, uint32_t index)
{
	sprintf(this->sqlstr, " update %s set sendmail_flag = (1 << %u) | (sendmail_flag & ~(1 << %u)) where userid = %u",
		this->get_table_name(userid), index - 1,(index + 15) & 31,userid);
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}

int Cuser_ex::set_sendmail_flag(userid_t userid, uint32_t index, uint32_t email_flag)
{
	sprintf(this->sqlstr, "update %s set sendmail_flag = (1 << %u) | (sendmail_flag & %u) where userid = %u",
			this->get_table_name(userid), index - 1, email_flag, userid);
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

int Cuser_ex::get_board_info(userid_t userid ,user_get_user_board_info_out_header *p_out ) 
{
	sprintf( this->sqlstr, "select \
			flag,\
			nick,\
			professionlist \
		from %s where userid=%u ", 
			this->get_table_name(userid),userid);

	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			INT_CPY_NEXT_FIELD(p_out->flag);
			BIN_CPY_NEXT_FIELD(p_out->nick, NICK_LEN );
			BIN_CPY_NEXT_FIELD(p_out->professionlist, 200);
	STD_QUERY_ONE_END();

}

int Cuser_ex::get_months(userid_t userid, user_ex_get_month_out *p_out)
{
	sprintf(this->sqlstr, "select months,expire,bonus_used from %s where userid=%u ", 
		this->get_table_name(userid),userid);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->months);
		INT_CPY_NEXT_FIELD(p_out->expire);
		INT_CPY_NEXT_FIELD(p_out->bonus_used);
	STD_QUERY_ONE_END();
}

int Cuser_ex::update_months(userid_t userid, user_ex_add_month_in *p_in)
{
	int ret = this->reset_event_flag(userid,56);
	if(ret != SUCC) return ret;
	sprintf(this->sqlstr, "update %s set months=%u,expire=%u where userid = %u",
                this->get_table_name(userid),p_in->months,p_in->expire,userid);
        STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

int Cuser_ex::inc_months(userid_t userid, uint32_t months)
{
	sprintf(this->sqlstr, "update %s set months = months + %u where userid = %u",
				this->get_table_name(userid), months, userid);
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

int Cuser_ex::add_bonus_used(userid_t userid, uint32_t value)
{
	user_ex_get_month_out out = {};

	int ret = this->get_months(userid,&out);	
	if(ret != SUCC) return ret;
	if(out.months < out.bonus_used + value) return ATTIRE_COUNT_NO_ENOUGH_ERR;
        sprintf(this->sqlstr, "update %s set bonus_used=bonus_used+%u where userid = %u",
                this->get_table_name(userid),value,userid);
        STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

/*
 * @brief 增加营养师熟练度
 */
int Cuser_ex::update_nourish_level_inc(userid_t userid, uint32_t nourish_level)
{
	sprintf( this->sqlstr, " update %s set nourish_level = nourish_level + %u \
		   			where userid = %u",
			this->get_table_name(userid), 
			nourish_level,
			userid
		);
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}


int Cuser_ex::clear_bonus_used(userid_t userid)
{

	sprintf(this->sqlstr, "update %s set bonus_used = if(bonus_used > months, 0, bonus_used) where userid = %u",
                this->get_table_name(userid),userid);
    
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

int Cuser_ex::get_vip_pet_level(userid_t userid, uint32_t& vip_pet_level)
{
    sprintf( this->sqlstr, "select vip_pet_level\
        from %s where userid=%u",this->get_table_name(userid),userid );

    STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR );
        vip_pet_level = atoi_safe(NEXT_FIELD );
    STD_QUERY_ONE_END();
}

int Cuser_ex::get_nick_clothe(userid_t userid, char *nick,  user_ex_pic_clothe  *p_out)
{
	memset(p_out,0,sizeof(*p_out ) );
    sprintf( this->sqlstr, "select nick, pic_clothe from %s where userid = %u ",
            this->get_table_name(userid),
            userid
           );

    STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD(nick, NICK_LEN );
            BIN_CPY_NEXT_FIELD(p_out , sizeof(*p_out));
    STD_QUERY_ONE_END();
}
int Cuser_ex::get_two_cols(userid_t userid, const char* col_1, const char* col_2, uint32_t &value_1,
		uint32_t &value_2)
{
    sprintf( this->sqlstr, "select %s, %s from %s where userid=%u",
			col_1,
			col_2,
			this->get_table_name(userid),
			userid 
			);

    STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR );
        value_1 = atoi_safe(NEXT_FIELD );
        value_2 = atoi_safe(NEXT_FIELD );
    STD_QUERY_ONE_END();
}
int Cuser_ex::update_two_cols(userid_t userid, const char* col_1, const char* col_2, uint32_t value_1,
		uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set %s = %u, %s = %u where userid = %u",
			this->get_table_name(userid),
			col_1,
			value_1,
			col_2,
			value_2,
			userid
			);

	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

int Cuser_ex::batch_del_friends(userid_t userid, const char* id_flag_str, uint32_t in_count, 
		user_batch_del_friends_in_item* p_in_item, uint32_t *real_del_count)
{
	*real_del_count = 0;
	int ret;
	id_list idlist={ };
	uint32_t * new_idend, *idstart, *idend;	
	ret=this->get_idlist(userid,id_flag_str,&idlist);
	if(ret==SUCC){
		uint32_t k = 0;
		for(; k < in_count; ++k){
			idstart = idlist.item;
			idend = idstart+idlist.count;	
			DEBUG_LOG("!!!!!!!!!!!!!!!friendid: %u", (p_in_item+k)-> id);
			new_idend = std::remove( idstart ,  idend , (p_in_item+k)-> id );
			if (new_idend != idend) {
				idlist.count = new_idend-idstart;	
				++(*real_del_count);
			}
		}//for
		return this->update_idlist(userid,id_flag_str ,&idlist);
	}

	return ret;

}

int Cuser_ex::inc_login_vip_mark(userid_t userid, uint32_t add_vip_mark)
{
	sprintf(this->sqlstr, "update %s set login_vip_mark = login_vip_mark + %u where userid = %u",
		this->get_table_name(userid),
		add_vip_mark,
		userid
		);

	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

int Cuser_ex::update_one_col(userid_t userid, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u",
			this->get_table_name(userid),
			col,
			value,
			userid
			);

	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}
