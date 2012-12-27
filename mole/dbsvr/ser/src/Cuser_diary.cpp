/* room_msg 模块*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
#include "Cuser_diary.h"

Cuser_diary::Cuser_diary(mysql_interface * db ) :
	CtableRoute10x10( db, "USERMSG", "t_user_diary", "userid")
{

}
/*
int Cuser_diary::add_msg(userid_t userid, USER_DIARY_ADD_MSG_IN * p_msg)
{
    uint32_t msg_count;
	if (userid >=50000){
		sprintf(this->sqlstr, "select count(*) from %s where userid=%u", 
				this->get_table_name(userid), userid);
		STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_EXISTED_ERR);
		INT_CPY_NEXT_FIELD(msg_count);
		STD_QUERY_ONE_END_WITHOUT_RETURN();
		if (msg_count>=1000) // 留言数太多了 提示用户删一些留言
		{
			DEBUG_LOG(">>>>>>>>>>>>>USER_ID %d<<<<<<<<<<<<", userid);
			return USER_DIARY_MSG_NUM_OUT_ERR;
		}
        
	}

    char tmp2[200];
    char tmp1[401];
	char nick_mysql[mysql_str_len(NICK_LEN)];

	set_mysql_string(nick_mysql,p_msg->guest_nick,sizeof(p_msg->guest_nick));

    strncpy(tmp2, p_msg->msg_buf, sizeof(tmp2));
    set_mysql_string(tmp1, tmp2, 200);

    // set_mysql_string(tmp1, p_msg->msg_buf, 200);
    if (userid==p_msg->guest_id)
    {
        sprintf( this->sqlstr, "insert into %s \
				(userid, guest_id, guest_nick, edit_time, flag, msg_buf)\
			   	values (%u, %u, '%s', %u, %u, '%s' )" , 
				this->get_table_name(userid), userid, 
			p_msg->guest_id, nick_mysql,
		   	p_msg->edit_time, (uint32_t)time(NULL), tmp1);
    }
    else
    {
        sprintf( this->sqlstr, "insert into %s \
				(userid, guest_id, guest_nick, edit_time, msg_buf) \
			   	values (%u, %u, '%s', %u, '%s')", 
				this->get_table_name(userid), userid, p_msg->guest_id, 
		 	nick_mysql, p_msg->edit_time, tmp1);
    }
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_diary::show_msg(userid_t userid, USER_DIARY_SHOW_MSG_IN * p_msg)
{
    if (p_msg->is_show!=0) {
        sprintf(this->sqlstr,
			   	"update %s set flag=1  where msg_id=%u and userid=%u" ,
			   	this->get_table_name(userid),  p_msg->msg_id, userid);
    } else {
        sprintf(this->sqlstr, "update %s set flag=%u where msg_id=%u and userid=%u"
            , this->get_table_name(userid), 0, p_msg->msg_id, userid);
    }
    STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_diary::list_msg(userid_t userid, USER_DIARY_LIST_MSG_IN * p_msgin, uint32_t* p_total, uint32_t* p_count, USER_DIARY_LIST_MSG_OUT_ITEM** pp_list)
{
    if (p_msgin->msg_count==0) {
        if (userid==p_msgin->request_id) {
            sprintf(this->sqlstr, "select count(*) from %s where userid=%u",
				   	this->get_table_name(userid), userid);
        } else {
            sprintf(this->sqlstr, "select count(*) from %s where userid=%u and flag=1",
				   	this->get_table_name(userid), userid);
        }
        STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_EXISTED_ERR);
        INT_CPY_NEXT_FIELD(*p_total);
        STD_QUERY_ONE_END_WITHOUT_RETURN();
    }
    else
        *p_total=1001;
    
    //DEBUG_LOG("userid=%u:p_msgin->request_id=%u", userid, p_msgin->request_id);
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
    BIN_CPY_NEXT_FIELD((*pp_list+i)->msg_buf, 200); // 留言长度200
    BIN_CPY_NEXT_FIELD((*pp_list+i)->res_buf, 100); // 回复长度100
    STD_QUERY_WHILE_END();
}

int Cuser_diary::del_msg(userid_t userid, USER_DIARY_DEL_MSG_IN * p_msg)
{
    if (p_msg->flag==0)
    {
        sprintf(this->sqlstr, "delete from %s where msg_id=%u and userid=%u"
           , this->get_table_name(userid), p_msg->msg_id, userid);
    }
    else
    {
        sprintf(this->sqlstr, "update %s set res_buf='' where msg_id=%u"
            , this->get_table_name(userid), p_msg->msg_id);
    }
    
    STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}
*/
