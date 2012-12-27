/*
 * =====================================================================================
 *
 *       Filename:  Cuser_info.cpp
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
#include "Cuser_info.h"
#include <time.h> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "msglog.h"
#include "login_info_submitter.h"
using namespace std;
//user 
Cuser_info::Cuser_info(mysql_interface * db ):CtableRoute( db , "USER_INFO" , "t_user_info" , "userid") 
{ 
	this->msglog_file = config_get_strval("MSG_LOG_FILE");
}

int Cuser_info::del(userid_t userid )
{
	sprintf( this->sqlstr, "delete from %s where userid=%u " ,
			this->get_table_name(userid),  userid); 
	STD_REMOVE_RETURN (this->sqlstr,userid,  USER_ID_NOFIND_ERR);
}

int Cuser_info::insert(userid_t userid, user_info_item * u)
{
	uint32_t gameflag=0;
	char passwd_mysql[mysql_str_len(PASSWD_LEN)];
	char paypasswd_mysql[mysql_str_len(PASSWD_LEN)];
	char email_mysql[mysql_str_len(EMAIL_LEN)];
	char parent_email_mysql[mysql_str_len(EMAIL_LEN)];
	char passwd_email_mysql[mysql_str_len(EMAIL_LEN)];
	char question_mysql[mysql_str_len(QUESTION_LEN)];
	char answer_mysql[mysql_str_len(ANSWER_LEN)];
	char signature_mysql[mysql_str_len(SIGNATURE_LEN)];

	set_mysql_string(passwd_mysql,u->passwd,sizeof(u->passwd) );
	set_mysql_string(paypasswd_mysql,u->paypasswd,sizeof(u->paypasswd));
//	set_mysql_string(nick_mysql,u->nick,sizeof(u->nick) );
	
	set_mysql_string(email_mysql,set_space_end( u->email,EMAIL_LEN ), EMAIL_LEN);
	set_mysql_string(parent_email_mysql,
			set_space_end( u->parent_email,EMAIL_LEN),
			sizeof(u->parent_email));

	set_mysql_string(passwd_email_mysql,
			set_space_end( u->passwdemail,EMAIL_LEN),
			sizeof(u->passwdemail));

	set_mysql_string(question_mysql,u->question,sizeof(u->question));
	set_mysql_string(answer_mysql,u->answer,sizeof(u->answer));
	set_mysql_string(signature_mysql,u->signature,SIGNATURE_LEN);
	uint32_t mee_fans=0;

	sprintf( this->sqlstr, "insert into %s values (\
		%u,%u,%u,%u,%u,0,'%s','%s','%s','%s','%s','%s','%s', '%s',0x00,0x00,0x00,0x00,0 )", 
			this->get_table_name(userid), 
			userid,
			u->flag1,
			mee_fans,
			gameflag,
			u->regflag,
			passwd_mysql, 
			paypasswd_mysql, 
			email_mysql,
			parent_email_mysql,
			passwd_email_mysql,
			question_mysql,
			answer_mysql,
			signature_mysql
	   	);
		STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}

int	Cuser_info::check_existed( userid_t userid)
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

int Cuser_info::get_email_by_fieldname(userid_t userid,
		const char * fieldname ,char * email )
{
	sprintf( this->sqlstr, "select  %s \
		  	from %s where userid=%u ", 
			fieldname, this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD(email,EMAIL_LEN);
	STD_QUERY_ONE_END();
}

int Cuser_info::get_email(userid_t userid ,char * email )
{
	return this->get_email_by_fieldname(userid ,"email", email  );
}

int Cuser_info::get_passwdemail(userid_t userid ,char * email )
{
	return this->get_email_by_fieldname(userid ,"passwdemail", email  );
}



int Cuser_info::check_question(userid_t userid , char*  question, char * answer )
{
	char question_mysql[mysql_str_len(QUESTION_LEN)];
	char answer_mysql[mysql_str_len(ANSWER_LEN)];
	if (question[0]=='\0' ){
		return USER_QUESTION_NOSET_ERR; 
	}
	set_mysql_string(question_mysql,question,QUESTION_LEN );
	set_mysql_string(answer_mysql,answer,ANSWER_LEN );
	sprintf( this->sqlstr, "select  userid from %s \
			where userid=%u  and question='%s'  and  answer='%s' ", 
			this->get_table_name(userid),userid,question_mysql,answer_mysql );
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_CHECK_QUESTION_ERR );
	STD_QUERY_ONE_END();
}

/*
int Cuser_info::get_nick(userid_t userid ,char *nick )
{
	sprintf( this->sqlstr, "select   nick \
		  	from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD(nick,NICK_LEN );
	STD_QUERY_ONE_END();
}

*/
int Cuser_info::isset_question(userid_t userid)
{
	sprintf( this->sqlstr, "select  userid \
		  	from %s where userid=%u and question<>'' ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_QUESTION_NOSET_ERR );
	STD_QUERY_ONE_END();
}
int Cuser_info::get_security_flag(userid_t userid,uint32_t *p_flag )
{
	char parent_email[EMAIL_LEN];
	char question[QUESTION_LEN];
	memset(parent_email,0,sizeof(parent_email) );
	memset(question,0,sizeof(question) );
	sprintf( this->sqlstr, "select  question,parent_email\
		  	from %s where userid=%u", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_QUESTION_NOSET_ERR );
		BIN_CPY_NEXT_FIELD(question,sizeof(question ));
		BIN_CPY_NEXT_FIELD(parent_email,sizeof(parent_email));
	STD_QUERY_ONE_END_WITHOUT_RETURN();
	*p_flag=0;
	if (question[0]!='\0'){
		DEBUG_LOG("%d",question[0] );
		*p_flag|=0x01;
	}
	if (parent_email[0]!='\0'){
		DEBUG_LOG("%d",parent_email[0]);
		*p_flag|=0x02;
	}
	return SUCC;
}

/*
int Cuser_info::get_info (userid_t userid ,USERINFO_GET_INFO_OUT *p_out )
{
	memset(p_out,0,sizeof(*p_out));
	sprintf( this->sqlstr, "select  flag1,flag2, nick \
		  	from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			INT_CPY_NEXT_FIELD(p_out->flag1);
			INT_CPY_NEXT_FIELD(p_out->flag2);
			BIN_CPY_NEXT_FIELD(p_out->nick, NICK_LEN );
	STD_QUERY_ONE_END();
}
*/

int Cuser_info::get_info_all (userid_t userid ,user_info_item *p_out )
{
	memset(p_out,0,sizeof(*p_out));
	sprintf( this->sqlstr, "select flag1, regflag, passwd, paypasswd, \
			 email, parent_email,passwdemail, question, answer, signature \
		  	from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->flag1);
		INT_CPY_NEXT_FIELD(p_out->regflag);
		BIN_CPY_NEXT_FIELD(p_out->passwd,sizeof(p_out->passwd));
		BIN_CPY_NEXT_FIELD(p_out->paypasswd,sizeof(p_out->paypasswd));
		BIN_CPY_NEXT_FIELD(p_out->email,sizeof(p_out->email));
		BIN_CPY_NEXT_FIELD(p_out->parent_email,sizeof(p_out->parent_email));
		BIN_CPY_NEXT_FIELD(p_out->passwdemail,sizeof(p_out->passwdemail));
		BIN_CPY_NEXT_FIELD(p_out->question,sizeof(p_out->question));
		BIN_CPY_NEXT_FIELD(p_out->answer,sizeof(p_out->answer));
		BIN_CPY_NEXT_FIELD(p_out->signature,sizeof(p_out->signature));
	STD_QUERY_ONE_END();
}

int Cuser_info::update_email(userid_t userid  ,  char * email )
{
	char email_mysql[mysql_str_len(EMAIL_LEN)];
	set_mysql_string(email_mysql,
			set_space_end(email, EMAIL_LEN ), EMAIL_LEN);
	sprintf( this->sqlstr, " update %s set \
					email='%s' \
		   			where userid=%u " ,
				this->get_table_name(userid), 
				 email_mysql , userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser_info::update_pemail(userid_t userid  ,  char * pemail )
{
	char pemail_mysql[mysql_str_len(EMAIL_LEN)];
	set_mysql_string(pemail_mysql,
			set_space_end(pemail, EMAIL_LEN ), EMAIL_LEN);
	sprintf( this->sqlstr, " update %s set \
					parent_email='%s' \
		   			where userid=%u " ,
				this->get_table_name(userid), 
				 pemail_mysql , userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

/*
int Cuser_info::update_nick(userid_t userid  ,  char * nick )
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
*/

int Cuser_info::update_for_pay(userid_t userid  , userinfo_set_payinfo_in *p_in )
{
	char parent_email_mysql[mysql_str_len(EMAIL_LEN)];
	char question_mysql[mysql_str_len(QUESTION_LEN)];
	char answer_mysql[mysql_str_len(ANSWER_LEN)];

	set_mysql_string(parent_email_mysql,
			set_space_end( p_in->parent_email,sizeof(p_in->parent_email)),
			sizeof(p_in->parent_email));
	set_mysql_string(question_mysql,p_in->question,sizeof(p_in->question));
	set_mysql_string(answer_mysql,p_in->answer,sizeof(p_in->answer));
	sprintf( this->sqlstr, " update %s set parent_email='%s' , \
					question='%s',\
					answer='%s'\
		   			where userid=%u " ,
				this->get_table_name(userid), 
				parent_email_mysql,
				question_mysql,
				answer_mysql,
				  userid );

	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser_info::update_qa(userid_t userid , userinfo_set_qa_in *p_in )
{
	char question_mysql[mysql_str_len(QUESTION_LEN)];
	char answer_mysql[mysql_str_len(ANSWER_LEN)];

	set_mysql_string(question_mysql,p_in->question,sizeof(p_in->question));
	set_mysql_string(answer_mysql,p_in->answer,sizeof(p_in->answer));
	sprintf( this->sqlstr, " update %s set  \
					question='%s',\
					answer='%s'\
		   			where userid=%u " ,
				this->get_table_name(userid), 
				question_mysql,
				answer_mysql,
				  userid );

	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}


int Cuser_info::get_passwd(userid_t userid ,  char * p_passwd)
{

	sprintf( this->sqlstr, "select passwd \
			from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_passwd,PASSWD_LEN);
	STD_QUERY_ONE_END();
}


int Cuser_info::user_check(userid_t userid ,  char * passwd)
{
	sprintf( this->sqlstr, "select passwd \
			from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	char  db_passwd[16] ;
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(db_passwd,PASSWD_LEN);
	STD_QUERY_ONE_END_WITHOUT_RETURN();
	if (memcmp(db_passwd ,passwd,PASSWD_LEN)!=0) return CHECK_PASSWD_ERR;
	return SUCC;
}
/*
int Cuser_info::login(userid_t userid ,  char * passwd, userinfo_login_out  *p_out  )
{
	sprintf( this->sqlstr, "select passwd ,flag2&0x01, friendlist  \
			from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	char  db_passwd[16] ;
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(db_passwd,PASSWD_LEN);
		INT_CPY_NEXT_FIELD(p_out->vip_flag);
		BIN_CPY_NEXT_FIELD(&(p_out->friendcount),4+sizeof(userid_t)*FRIEND_COUNT_MAX);
	STD_QUERY_ONE_END_WITHOUT_RETURN();
	if (memcmp(db_passwd ,passwd,PASSWD_LEN)!=0) return CHECK_PASSWD_ERR;
	return SUCC;
}
*/
int Cuser_info::login_ex_with_email(userid_t userid ,  char * passwd, uint32_t* p_game_flag, char *email)
{
	sprintf( this->sqlstr, "select passwd ,gameflag, email\
			from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	char  db_passwd[16] ;
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(db_passwd,PASSWD_LEN);
		INT_CPY_NEXT_FIELD(*p_game_flag );
		BIN_CPY_NEXT_FIELD(email, EMAIL_LEN);
	STD_QUERY_ONE_END_WITHOUT_RETURN();
	if (memcmp(db_passwd ,passwd,PASSWD_LEN)!=0) return CHECK_PASSWD_ERR;
	return SUCC;
}


int Cuser_info::login_ex(userid_t userid ,  char * passwd, uint32_t* p_game_flag )
{
	sprintf( this->sqlstr, "select passwd ,gameflag ,flag1\
			from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	char  db_passwd[16] ;
	uint32_t flag1;
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(db_passwd,PASSWD_LEN);
		INT_CPY_NEXT_FIELD(*p_game_flag );
		INT_CPY_NEXT_FIELD(flag1);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	//检查账号是否被冻结了
	if  ((flag1& USER_FLAG1_DISABLE_ACCOUNT)>0 ){
		return   USER_NOT_ENABLED_ERR;
	}

	if (memcmp(db_passwd ,passwd,PASSWD_LEN)!=0) {

		char hex_passwd[33]	;
		char hex_passwd2[33]	;
		ASC2HEX_2_lower(hex_passwd,db_passwd ,PASSWD_LEN );
		ASC2HEX_2_lower(hex_passwd2,passwd,PASSWD_LEN );
		DEBUG_LOG("PASSWD:ERR:db:%s,input:%s",hex_passwd,hex_passwd2 );
		return CHECK_PASSWD_ERR;
	}
	return SUCC;
}

int Cuser_info::login_ex_with_stat(userid_t userid, userinfo_log_with_stat_in *p_in, uint32_t *p_game_flag)
{
	int ret = this->login_ex(userid, p_in->passwd, p_game_flag);
	if (ret != SUCC) { 
		ERROR_LOG("CHECK_ERR: %u %u %08X",userid, p_in->ip,p_in->login_channel );
		return ret; 
	}

	uint32_t now = time(NULL);
	//ret = this->update_flag(userid, "last_login", now);
	if (p_in->login_channel == 4)
	{
		haqi_town_login haqi_login;
		haqi_login.index = 1;
		haqi_login.userid = userid;
		msglog(this->msglog_file, 0x0605001F, now, &haqi_login, sizeof(haqi_town_login));
	}
	return SUCC;
}


int Cuser_info::change_passwd(userid_t userid ,  char * oldpasswd , char *newpasswd  )
{
	int ret;	
	ret=this->user_check(userid, oldpasswd);
	if(ret==SUCC ){
		ret=this->update_passwd(userid, newpasswd  );
	}
	return ret;
}

int Cuser_info::update_passwd(userid_t userid  ,char * newpasswd)
{
	int ret;
	ret=this->set_history_passwd(userid);
	if(ret!=SUCC) return ret;
	DEBUG_LOG("=update===ret:%d",ret)	;
	char mysql_passwd[mysql_str_len(PASSWD_LEN)];
	set_mysql_string(mysql_passwd,newpasswd,PASSWD_LEN);
	sprintf( this->sqlstr, " update %s set  passwd='%s' where userid=%u " ,
			this->get_table_name(userid), mysql_passwd, userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser_info::check_flag(userid_t userid,const char *  flag_type,  uint32_t flag, bool * p_existed ) 
{
	int ret=0;
	uint32_t db_flag;
	ret =this->get_flag(userid, flag_type ,&db_flag);
	if (ret!=SUCC){
		return ret;
	}
	if ( (db_flag & flag ) == flag )  *p_existed=true;
	else *p_existed=false;
	DEBUG_LOG("exist:%d",*p_existed);
	return SUCC;
}

int Cuser_info::set_flag ( userid_t userid  ,const  char * flag_type  ,  uint32_t flag_bit ,  bool is_true )
{
	uint32_t flag;
	int ret;
	bool old_is_true;
	ret =this->get_flag(userid, flag_type ,&flag);
	if (ret!=SUCC){
			return ret;
	}	

	DEBUG_LOG("---flag[%X]--bit[%X]--&[%X]-",flag,flag_bit,flag & flag_bit );
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

int Cuser_info::get_bit_flag(userid_t userid, const char *flag_type, uint32_t flag_bit, uint32_t *p_flag)
{
	uint32_t flag;
	int ret;
	ret =this->get_flag(userid, flag_type ,&flag);
	if (ret!=SUCC) return ret;

	if ( (flag & flag_bit) == flag_bit ) *p_flag=1;
	else *p_flag =0; 
	return SUCC;
}


int Cuser_info::get_flag(userid_t userid ,const char * flag_type   ,  uint32_t * flag)
{
	sprintf( this->sqlstr, "select  %s from %s where userid=%u ", 
		flag_type, this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
			*flag=atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END();
}
int Cuser_info::get_gameflag(userid_t userid ,uint32_t * p_gameflag)
{
	sprintf( this->sqlstr, "select  gameflag from %s where userid=%u ", 
		 this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_gameflag);
	STD_QUERY_ONE_END();
}



int Cuser_info::update_flag(userid_t userid ,const char * flag_type  , uint32_t  flag)
{
	sprintf( this->sqlstr, "update %s set %s =%u where userid=%u " ,
		this->get_table_name(userid), flag_type,flag,userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

/*
int Cuser_info::add_friend(userid_t userid , userid_t id )
{
	uint32_t flag2;
	ID_LIST friendlist;
	ID_LIST blacklist;
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

	if ((flag2&USER_FLAG2_VIP)==USER_FLAG2_VIP ){ //vip
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
*/

/*
int Cuser_info::add_id(userid_t userid ,const  char * id_flag_str , userid_t id )
{
	int ret;
	uint32_t allow_max_count=100;
	ID_LIST idlist;
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

int Cuser_info::del_id(userid_t userid ,const char * id_flag_str , userid_t id )
{
	int ret;
	ID_LIST idlist;
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

int Cuser_info::get_idlist(userid_t userid ,const  char * id_flag_str , ID_LIST * idlist )
{
	sprintf( this->sqlstr, "select  %s \
			from %s where userid=%u ", 
		id_flag_str,this->get_table_name(userid), userid);

	//copy idlist  
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(idlist, sizeof(ID_LIST ));
	STD_QUERY_ONE_END();
}

int Cuser_info::get_friend_black_list(userid_t userid ,uint32_t *p_flag2  , ID_LIST *p_friendlist, 
	   ID_LIST *  p_blacklist )
{
	sprintf( this->sqlstr, "select flag2, friendlist, blacklist\
			from %s where userid=%u ", 
		 this->get_table_name(userid), userid);

	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		//copy idlist  
		 INT_CPY_NEXT_FIELD(*p_flag2 ) ;	
		BIN_CPY_NEXT_FIELD(p_friendlist , sizeof(ID_LIST ));
		BIN_CPY_NEXT_FIELD(p_blacklist, sizeof(ID_LIST ));
	STD_QUERY_ONE_END();
}


int Cuser_info::get_nick_fiendlist(userid_t userid , char * nick , ID_LIST * idlist )
{
	sprintf( this->sqlstr, "select  nick, friendlist \
			from %s where userid=%u ", 
		this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(nick, NICK_LEN );
		BIN_CPY_NEXT_FIELD(idlist, sizeof(ID_LIST ));
	STD_QUERY_ONE_END();
}

int Cuser_info::update_idlist(userid_t userid , const char * id_flag_str ,  ID_LIST * idlist )
{
	char mysql_idlist[mysql_str_len(sizeof(ID_LIST) )];
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
*/
/*
int Cuser_info::check_friend(userid_t userid , userid_t friendid, uint32_t *p_is_existed  )
{
	int ret;
	ID_LIST idlist;
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
			*p_is_existed=1;
		}
		return SUCC;
	}else{
		return ret;
	}
}
*/

int Cuser_info::update_passwdemail(userid_t userid , char*  passwdemail )
{
	char passwdemail_mysql[mysql_str_len(EMAIL_LEN)];
	set_mysql_string(passwdemail_mysql ,passwdemail ,EMAIL_LEN );
	sprintf( this->sqlstr, " update %s set \
					passwdemail='%s' \
		   			where userid=%u " ,
				this->get_table_name(userid), 
				 passwdemail_mysql , userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	

}

int Cuser_info::set_paypasswd(userid_t userid,char* paypasswd  )
{
	int ret;
	ret=this->update_paypasswd(userid,paypasswd );
	if (ret!=SUCC) return ret;

	//设置：已经设置过支付密码了	
	this->set_flag(userid,FLAG1_STR,USER_FLAG1_ISSET_PAYPASSWD ,true );
	return SUCC;
}
int Cuser_info::user_check_paypasswd(userid_t userid ,  char * paypasswd)
{
	char  init_passwd[PASSWD_LEN]={};
	char  db_passwd[PASSWD_LEN] ;
	sprintf( this->sqlstr, "select paypasswd \
			from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(db_passwd,PASSWD_LEN);
	STD_QUERY_ONE_END_WITHOUT_RETURN();
	//密码是初始
	if (memcmp(init_passwd, paypasswd,PASSWD_LEN)==0) 
		return CHECK_PAYPASSWD_ERR;

	if (memcmp(db_passwd, paypasswd,PASSWD_LEN)!=0) 
		return CHECK_PAYPASSWD_ERR;

	return SUCC;
}

int Cuser_info::update_paypasswd(userid_t userid  ,  char * paypasswd)
{
	char paypasswd_mysql[mysql_str_len(PASSWD_LEN)];
	set_mysql_string(paypasswd_mysql ,paypasswd ,PASSWD_LEN );
	sprintf( this->sqlstr, " update %s set \
					paypasswd='%s' \
		   			where userid=%u " ,
				this->get_table_name(userid), 
				 paypasswd_mysql , userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}


int Cuser_info::get_question(userid_t userid , char*  question  )
{
	sprintf( this->sqlstr, "select question  from %s \
			where userid=%u  ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(question,QUESTION_LEN);
	STD_QUERY_ONE_END();
}


int Cuser_info::set_passwdemail(userid_t userid , char*  passwdemail )
{
	int ret ;
	bool existed;
	ret=this->check_flag(userid, FLAG1_STR, USER_FLAG1_ISCONFIRM_PASSWDEMAIL ,&existed);	
	if (ret!=SUCC) return ret;
	if (existed)  
		return USER_PASSWDEMAIL_CONFIRMED_ERR;
	ret=this->update_passwdemail(userid,passwdemail);	
	if (ret==SUCC){
		this->set_flag(userid , FLAG1_STR, 
		              USER_FLAG1_ISSET_PASSWDEMAIL ,true);
	}
	return ret;
}
int Cuser_info::add_game(userid_t userid , uint32_t  gameid_flag)
{
	uint32_t gameflag_bit=(0x01<< (gameid_flag-1 ) );
	sprintf( this->sqlstr, "update %s set gameflag=gameflag|%u  where userid=%u " ,
		this->get_table_name(userid),  gameflag_bit ,userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser_info::get_history_passwd(userid_t userid, history_passwd *p_history_passwd )
{
	sprintf( this->sqlstr, "select  history_passwd \
		from %s where userid=%u",
		this->get_table_name(userid),userid );

	STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR );
		BIN_CPY_NEXT_FIELD(p_history_passwd ,sizeof (*p_history_passwd));	
	STD_QUERY_ONE_END();
}

int Cuser_info::update_history_passwd(userid_t userid, history_passwd *p_history_passwd )
{

	char history_passwd_mysql[mysql_str_len(sizeof(*p_history_passwd))];
	set_mysql_string(history_passwd_mysql, 
			(char*)(p_history_passwd),sizeof(*p_history_passwd));
	sprintf( this->sqlstr, "update %s  set history_passwd='%s'\
			where userid=%u ",
		this->get_table_name(userid), 
		history_passwd_mysql, userid
		);
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	

}

int Cuser_info::set_history_passwd(userid_t userid )
{
	int ret;

	char passwd[PASSWD_LEN];
	//得到之前的密码
	ret=this->get_passwd(userid, passwd );	
	if(ret!=SUCC) return ret;


	history_passwd history_lastpasswd={};		
	ret=this->get_history_passwd(userid,&history_lastpasswd );
	if(ret!=SUCC) return SUCC;

	history_passwd_item *_start, *_end,*_find  ; 
	history_passwd_item item;  
	memcpy( item.passwd, passwd, PASSWD_LEN);
	item.logtime=time(NULL);

	if (ret!=SUCC) return ret;
	_start= &(history_lastpasswd.passwditem[0]);
	_end=_start+3;	
	_find=std::find( _start ,  _end , item );
	if (_find!=_end ) {
		_find->logtime=time(NULL);
	}else{
		//没有找到
		memcpy( &(history_lastpasswd.passwditem[2]), &(history_lastpasswd.passwditem[1]),
				sizeof (history_passwd_item ) );

		memcpy( &(history_lastpasswd.passwditem[1]), &(history_lastpasswd.passwditem[0]),
				sizeof (history_passwd_item ) );
		memcpy( &(history_lastpasswd.passwditem[0]), &(item),
				sizeof (history_passwd_item) );
	}	
	
	return this->update_history_passwd(userid,&history_lastpasswd );
}

int Cuser_info::get_login_info(userid_t userid ,userinfo_get_login_info_out *p_out )
{
	memset(p_out,0,sizeof(*p_out));
	sprintf( this->sqlstr, "select passwd, gameflag from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_out->passwd,sizeof(p_out->passwd));
		INT_CPY_NEXT_FIELD( p_out->gameflag);
	STD_QUERY_ONE_END();
}

int Cuser_info::set_question2(userid_t userid  , userinfo_set_question2_in *p_in )
{
	char question2_mysql[mysql_str_len(sizeof(p_in->question2 ))];
	char answer2_mysql[mysql_str_len(sizeof(p_in->answer2 ))];
	set_mysql_string(question2_mysql,(char*)&(p_in->question2) , sizeof((p_in->question2)) );
	set_mysql_string(answer2_mysql,(char*)&(p_in->answer2) , sizeof((p_in->answer2)) );
	sprintf( this->sqlstr, " update %s set \
					question2='%s', \
					answer2='%s', \
					question2_set_time=%u \
		   			where userid=%u " ,
				this->get_table_name(userid), 
						question2_mysql,
						answer2_mysql,
						p_in->set_time,
				 	 userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}
int Cuser_info::get_question2(userid_t userid ,userinfo_get_question2_out *p_out )
{
	memset(p_out,0,sizeof(*p_out));
	sprintf( this->sqlstr, "select question2,  answer2,question2_set_time from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(&(p_out->question2) , sizeof(p_out->question2));
		BIN_CPY_NEXT_FIELD(&(p_out->answer2) , sizeof(p_out->answer2));
		INT_CPY_NEXT_FIELD(p_out->set_time );
	STD_QUERY_ONE_END();
}
int Cuser_info::get_question2_set_flag(userid_t userid ,uint32_t  *p_is_setd)
{
	stru_question2 question2;
	sprintf( this->sqlstr, "select question2  from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(&question2 , sizeof(question2));
		if (question2.question_1[0]=='\0'  
		    || question2.question_2[0]=='\0'  
		    || question2.question_3[0]=='\0'  
		   ){//有一个为NULL
			*p_is_setd=0;		
		}else{
			*p_is_setd=1;		
		}
	STD_QUERY_ONE_END();
}



int Cuser_info::get_answer2(userid_t userid , stru_answer2 *p_out )
{
	memset(p_out,0,sizeof(*p_out));
	sprintf( this->sqlstr, "select   answer2 from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_out , sizeof(*p_out));
	STD_QUERY_ONE_END();
}



int Cuser_info::verify_question2(userid_t userid , stru_answer2 *p_in)
{
	char answer2_mysql[mysql_str_len(sizeof(*p_in))];
	set_mysql_string(answer2_mysql,(char*)p_in , sizeof(*p_in) );
	sprintf( this->sqlstr, "select   1 from %s where userid=%u  and answer2='%s'", 
			this->get_table_name(userid),userid,answer2_mysql );
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USERINFO_VERIFY_QUESTION2_ERR);
	STD_QUERY_ONE_END();
};

int Cuser_info::login_by_md5_two(userid_t userid, userinfo_login_by_md5_two_in*p_in, uint32_t *p_game_flag)
{
	int ret = this->login_passwd_md5_two(userid, p_in->passwd_md5_two, p_game_flag);
	if (ret != SUCC) { 
		ERROR_LOG("CHECK_ERR:%u %u %08X",userid, p_in->ip,p_in->login_channel );
		return ret; 
	}

	uint32_t now = time(NULL);
	//ret = this->update_flag(userid, "last_login", now);
	if (p_in->login_channel == 4)
	{
		haqi_town_login haqi_login;
		haqi_login.index = 1;
		haqi_login.userid = userid;
		msglog(this->msglog_file, 0x0605001F, now, &haqi_login, sizeof(haqi_town_login));
	}
	return SUCC;
}
int Cuser_info::login_passwd_md5_two(userid_t userid ,  char * passwd_md5_two, uint32_t* p_game_flag )
{
	uint32_t flag1;
	sprintf( this->sqlstr, "select passwd ,gameflag ,flag1\
			from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	char  db_passwd[16] ;
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(db_passwd,PASSWD_LEN);
		INT_CPY_NEXT_FIELD(*p_game_flag );
		INT_CPY_NEXT_FIELD(flag1 );
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	//检查账号是否被冻结了
	if  ((flag1& USER_FLAG1_DISABLE_ACCOUNT)>0 ){
		return   USER_NOT_ENABLED_ERR;
	}

	//对DB中的密码再次加密
	//得到hex 小写的密码	
	char hex_db_passwd[33]	;
	ASC2HEX_2_lower(hex_db_passwd,db_passwd ,PASSWD_LEN );
	DEBUG_LOG("PASSWD:DB:%s",hex_db_passwd );
	this->md5.reset();
	this->md5.update(hex_db_passwd );


	if (memcmp(this->md5.digest(), passwd_md5_two,PASSWD_LEN)!=0) {
		char hex_passwd2[33]	;
		ASC2HEX_2_lower(hex_passwd2,passwd_md5_two,PASSWD_LEN );
		DEBUG_LOG("PASSWD:ERR:db:%s,input:%s",this->md5.toString().c_str() ,hex_passwd2 );
		return CHECK_PASSWD_ERR;
	}
	return SUCC;
}



int Cuser_info::get_meefan_gameflag(userid_t userid ,userinfo_get_meefan_gameflag_out *p_out )
{
	memset(p_out,0,sizeof(*p_out));
	sprintf( this->sqlstr, "select  mee_fans, gameflag from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( p_out->mee_fans);
		INT_CPY_NEXT_FIELD( p_out->gameflag);
	STD_QUERY_ONE_END();
}
