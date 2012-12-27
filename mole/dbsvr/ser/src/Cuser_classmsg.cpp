/* room_msg 模块*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
#include "Cuser_classmsg.h"

Cuser_classmsg::Cuser_classmsg(mysql_interface * db ) :CtableRoute100x10( db, "USER", "t_user_classmsg", "userid")
{

}


int Cuser_classmsg::get_count(userid_t userid, uint32_t  * p_count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid=%u", 
				this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, DB_ERR );
		INT_CPY_NEXT_FIELD(*p_count );
	STD_QUERY_ONE_END();
}


int Cuser_classmsg::add_msg(userid_t userid, user_classmsg_add_in * p_msg)
{
	int ret;
    uint32_t msg_count;
	ret=this->get_count(userid,&msg_count);
	if(ret!=SUCC) return ret;
	if (msg_count>=1000) {
			return CLASSMSG_MAX_ERR;
	}
	p_msg->msg_buf[sizeof(p_msg->msg_buf)-1]='\0';

	char nick_mysql[mysql_str_len(NICK_LEN)];
	char msg_mysql[mysql_str_len(sizeof (p_msg->msg_buf))];

	set_mysql_string(nick_mysql,p_msg->guest_nick,sizeof(p_msg->guest_nick));
	set_mysql_string(msg_mysql,p_msg->msg_buf,strlen(p_msg->msg_buf) );
	uint32_t flag;
    if (userid==p_msg->guest_id){
		flag=1;
	}else{
		flag=0;
	}
	

     sprintf( this->sqlstr, "insert into %s  values (0,%u, %u, '%s', %u, %u, '%s','' )" , 
				this->get_table_name(userid), userid, 
				p_msg->guest_id, nick_mysql,
			   	p_msg->edit_time, flag , msg_mysql );
    
	STD_SET_RETURN_EX (this->sqlstr, DB_ERR);
}

int Cuser_classmsg::show_msg(userid_t userid, user_classmsg_set_show_flag_in * p_msg)
{
	if ( p_msg->is_show>0) p_msg->is_show=1;
    sprintf(this->sqlstr, "update %s set flag=%u  where msg_id=%u and userid=%u" ,
	this->get_table_name(userid), p_msg->is_show, p_msg->msg_id, userid);
    STD_SET_RETURN_EX (this->sqlstr, MSGID_NOFIND_ERR );
}
int Cuser_classmsg::get_list(userid_t userid,  user_classmsg_get_list_in * p_msgin, uint32_t* p_total, uint32_t* p_count, user_classmsg_get_list_out_item  ** pp_list)
{
    if (p_msgin->msg_count==0) {
        if (userid==p_msgin->request_id) {
            sprintf(this->sqlstr, "select count(*) from %s where userid=%u",
				   	this->get_table_name(userid), userid);
        } else {
            sprintf(this->sqlstr, "select count(*) from %s where userid=%u and flag=1",
				   	this->get_table_name(userid), userid);
        }
        STD_QUERY_ONE_BEGIN(this->sqlstr,MSGID_NOFIND_ERR );
      		 INT_CPY_NEXT_FIELD(*p_total);
        STD_QUERY_ONE_END_WITHOUT_RETURN();
    }
    else
        *p_total=1001;
    
    if (userid==p_msgin->request_id) {
        sprintf(this->sqlstr, "select msg_id, guest_id, guest_nick, edit_time, flag, msg_buf, res_buf from %s where userid=%u order by msg_id  desc  limit %u,6"
            , this->get_table_name(userid), userid, p_msgin->msg_count);
    } else {
        sprintf(this->sqlstr, "select msg_id, guest_id, guest_nick, edit_time, flag, msg_buf, res_buf from %s where userid=%u and flag=1  order by msg_id  desc  limit %u,6 "
            , this->get_table_name(userid), userid, p_msgin->msg_count);
    }

    STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_list, p_count);
	    INT_CPY_NEXT_FIELD((*pp_list+i)->msg_id);
	    INT_CPY_NEXT_FIELD((*pp_list+i)->guest_id);
	    BIN_CPY_NEXT_FIELD((*pp_list+i)->guest_nick, NICK_LEN);
	    INT_CPY_NEXT_FIELD((*pp_list+i)->edit_time);
	    INT_CPY_NEXT_FIELD((*pp_list+i)->show_time);
	    BIN_CPY_NEXT_FIELD((*pp_list+i)->msg_buf, sizeof ((*pp_list+i)->msg_buf ) );
	    BIN_CPY_NEXT_FIELD((*pp_list+i)->res_buf, sizeof((*pp_list+i)->res_buf) );
    STD_QUERY_WHILE_END();
}

int Cuser_classmsg::del_msg(userid_t userid,  user_classmsg_del_in* p_msg)
{
    if (p_msg->flag==0)
    {
        sprintf(this->sqlstr, "delete from %s where msg_id=%u "
           , this->get_table_name(userid), p_msg->msg_id );
    }
    else
    {
        sprintf(this->sqlstr, "update %s set res_buf='' where msg_id=%u"
            , this->get_table_name(userid), p_msg->msg_id);
    }
    
    STD_SET_RETURN_EX (this->sqlstr, MSGID_NOFIND_ERR );
}

int Cuser_classmsg::del_class(userid_t userid )
{
	sprintf(this->sqlstr, "delete from %s where userid=%u " , 
			this->get_table_name(userid), userid);
    STD_SET_RETURN_EX (this->sqlstr, SUCC);
}

int Cuser_classmsg::res_msg(userid_t userid, user_classmsg_report_in * p_msg)
{

	char res_mysql[mysql_str_len(sizeof (p_msg->res_buf))];
    set_mysql_string(res_mysql,p_msg->res_buf,sizeof(p_msg->res_buf));
    sprintf(this->sqlstr, "update %s set res_buf='%s' where msg_id=%u", 
			this->get_table_name(userid),res_mysql , p_msg->msg_id);
    STD_SET_RETURN_EX (this->sqlstr,MSGID_NOFIND_ERR );
}
