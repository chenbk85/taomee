#include <time.h> 
#include <algorithm>
#include <set>
#include "proto.h"
#include "db_error.h"
#include "Cgongfu.h"

//dudu 
Cgongfu::Cgongfu(mysql_interface * db)
	:CtableRoute100x10( db , "GF" , "t_gongfu" , "userid") 
{ 

}

int Cgongfu::init(userid_t userid,   gf_reg_in  *p_in )
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql,p_in->nick ,sizeof(p_in->nick) );
	uint32_t init_xioamee=2000;
	if (p_in->sex!=1 && p_in->sex!=2  )
	{
		return  ENUM_OUT_OF_RANGE_ERR;
	}
	uint32_t exp = 0;
 	
	GEN_SQLSTR( this->sqlstr, "insert into %s values (\
		%u,0,%u,%u,%u,'%s',%u,0,0,0x00000000,0x00000000,%u,'',0x00000000,0x00000000)",
			this->get_table_name(userid),userid, p_in->sex, 
			(uint32_t )time(NULL), p_in->birthday,
			 nick_mysql, init_xioamee,exp);
	return this->exec_insert_sql(this->sqlstr, USER_ID_EXISTED_ERR);
}


/**  
 * @fn 取得用户全局信息
 * @brief  cmd route interface
 */
int Cgongfu::get_user_info(userid_t userid, uint32_t role_regtime, stru_gf_info_out *p_out, 
    uint32_t* forbid_flag, stru_msg_list* p_msg)
{
	GEN_SQLSTR(this->sqlstr,"select flag, regtime, forbid_flag, vip,vip_month_count,start_time,end_time, x_value, \
        sword_value, vip_exwarehouse, Ol_count, Ol_today, Ol_last, Ol_time,\
        msglist,amb_status, parentid, child_cnt, achieve_cnt, flag_bit, act_record, forbiden_add_friend_flag  from %s where userid = %u ",
        this->get_table_name(userid),userid);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->flag);
		INT_CPY_NEXT_FIELD(p_out->regtime);
		INT_CPY_NEXT_FIELD(*forbid_flag);
		INT_CPY_NEXT_FIELD(p_out->vip);
		INT_CPY_NEXT_FIELD(p_out->vip_month_cnt);
		INT_CPY_NEXT_FIELD(p_out->vip_start_tm);
		INT_CPY_NEXT_FIELD(p_out->vip_end_tm);
		INT_CPY_NEXT_FIELD(p_out->vip_x_val);
		INT_CPY_NEXT_FIELD(p_out->sword_value);
		INT_CPY_NEXT_FIELD(p_out->vip_exwarehouse);
//		INT_CPY_NEXT_FIELD(p_out->skill_point);
//		INT_CPY_NEXT_FIELD(p_out->xiaomee);
		INT_CPY_NEXT_FIELD(p_out->Ol_count);
		INT_CPY_NEXT_FIELD(p_out->Ol_today);
		INT_CPY_NEXT_FIELD(p_out->Ol_last);
		INT_CPY_NEXT_FIELD(p_out->Ol_time);
		BIN_CPY_NEXT_FIELD(p_msg, sizeof (typeof(*p_msg)));
		INT_CPY_NEXT_FIELD(p_out->amb_status);
		INT_CPY_NEXT_FIELD(p_out->parentid);
		INT_CPY_NEXT_FIELD(p_out->child_cnt);
		INT_CPY_NEXT_FIELD(p_out->achieve_cnt);
		BIN_CPY_NEXT_FIELD(p_out->flag_bit, sizeof(p_out->flag_bit));
		BIN_CPY_NEXT_FIELD(p_out->act_record, sizeof(p_out->act_record));
		INT_CPY_NEXT_FIELD(p_out->forbiden_add_friend_flag);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	return SUCC;
}

int Cgongfu::get_user_base_info(userid_t userid, gf_get_user_base_info_out_header *p_out)
{
	GEN_SQLSTR(this->sqlstr,"select flag,regtime,forbid_flag, deadline, \
        vip, vip_month_count,start_time,end_time, \
        auto_incr, x_value+sword_value, Ol_count,Ol_today,Ol_last,Ol_time, \
        amb_status, parentid, child_cnt, achieve_cnt from %s where userid = %u ",
        this->get_table_name(userid),userid);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->flag);
		INT_CPY_NEXT_FIELD(p_out->regtime);
		INT_CPY_NEXT_FIELD(p_out->forbid_flag);
		INT_CPY_NEXT_FIELD(p_out->deadline);
		INT_CPY_NEXT_FIELD(p_out->vip);
		INT_CPY_NEXT_FIELD(p_out->vip_month_count);
		INT_CPY_NEXT_FIELD(p_out->start_time);
		INT_CPY_NEXT_FIELD(p_out->end_time);
		INT_CPY_NEXT_FIELD(p_out->auto_incr);
		INT_CPY_NEXT_FIELD(p_out->x_value);
		INT_CPY_NEXT_FIELD(p_out->Ol_count);
		INT_CPY_NEXT_FIELD(p_out->Ol_today);
		INT_CPY_NEXT_FIELD(p_out->Ol_last);
		INT_CPY_NEXT_FIELD(p_out->Ol_time);
        INT_CPY_NEXT_FIELD(p_out->amb_status);
		INT_CPY_NEXT_FIELD(p_out->parentid);
		INT_CPY_NEXT_FIELD(p_out->child_cnt);
		INT_CPY_NEXT_FIELD(p_out->achieve_cnt);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	return SUCC;
}

int Cgongfu::update_user_base_info(userid_t userid, gf_set_user_base_info_in * p_in)
{
	GEN_SQLSTR(this->sqlstr, "update %s set flag=%u,regtime=%u,vip=%u,Ol_count=%u,\
			Ol_today=%u,Ol_last=%u,Ol_time=%u,amb_status=%u, parentid=%u, child_cnt=%u, \
            achieve_cnt=%u where userid=%u",
			this->get_table_name(userid), p_in->flag, p_in->regtime, p_in->vip, 
			p_in->Ol_count, p_in->Ol_today, p_in->Ol_last, p_in->Ol_time, p_in->amb_status,
            p_in->parentid, p_in->child_cnt, p_in->achieve_cnt, userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cgongfu::update_nick(userid_t userid, char* nick)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql, nick, NICK_LEN);
	GEN_SQLSTR(this->sqlstr,"update %s set nick='%s' \
			where userid=%u",
			this->get_table_name(userid), nick, userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR );
}

int Cgongfu::update_idlist(userid_t userid, const char* id_flag_str, struct stru_id_list* p_idlist)
{
	char mysql_idlist[mysql_str_len(sizeof(*p_idlist))];
	set_mysql_string(mysql_idlist, (char*)p_idlist, 
			4+sizeof(userid_t)*(p_idlist->count));

	GEN_SQLSTR(this->sqlstr,"update %s set %s='%s' \
			where userid=%u ",
			this->get_table_name(userid), id_flag_str, mysql_idlist, userid);

	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
	
}

int Cgongfu::del_id(userid_t userid, const char* id_flag_str, userid_t id)
{
	int ret;
	struct stru_id_list idlist = {};
	uint32_t *new_end, *id_start, *id_end;

	ret = this->get_idlist(userid, id_flag_str, &idlist);
	if(ret != SUCC)
		return ret;

	id_start = idlist.item;
	id_end = id_start + idlist.count;
	new_end = std::remove(id_start, id_end, id);
	if(new_end != id_end)
	{
		idlist.count = new_end - id_start;
		return this->update_idlist(userid, id_flag_str, &idlist);
	}
	else
	{
		return LIST_ID_NOFIND;
	}
}

int Cgongfu::get_idlist(userid_t userid, const char* id_flag_str, struct stru_id_list* p_idlist)
{
	memset(p_idlist,0,sizeof(*p_idlist) );
	GEN_SQLSTR(this->sqlstr,"select %s \
			from %s where userid=%u ",
			id_flag_str, this->get_table_name(userid), userid);

	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_idlist, sizeof(stru_id_list));
	STD_QUERY_ONE_END();
	
}


int Cgongfu::get_vip_type(userid_t userid,uint32_t* p_vip_type)
{
	GEN_SQLSTR(this->sqlstr,"select vip \
			from %s where userid=%u ",
			this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( (*p_vip_type) );
	STD_QUERY_ONE_END();	
}

int Cgongfu::update_list_nickname(uint32_t count, gf_get_friendlist_out_item* p_list)
{
	while (count--)
	{
	//	if ( (ret=this->get_nick_name((p_list)->id,(p_list)->nick)) != SUCC )
		{
			DEBUG_LOG("update_list_nickname,cannot select nick name, userid:[%u]",p_list->id );
		}
		p_list++;
	}
	return SUCC;
}

int Cgongfu::get_nick_name(userid_t userid, char* nick)
{
	GEN_SQLSTR(this->sqlstr,"select nick \
			from %s where userid=%u ",
			this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(nick,sizeof(nick));
	STD_QUERY_ONE_END();		
}

int Cgongfu::add_offline_msg(userid_t userid ,  stru_msg_item *item )
{
	int ret;
	stru_msg_list msglist={0};
	
	ret=this->get_msglist(userid, &msglist);
	DEBUG_LOG("-----------add_offline_msg: %d :  %d", msglist.len, msglist.count);
	if (ret!=SUCC) 
	{
		return ret;
	}

	if (msglist.len < msg_list_head_len)
	{
		msglist.len = msg_list_head_len;
	}
	
	if (msglist.len+item->msglen> MSG_LIST_BUF_SIZE)
		return  GF_OFFLINE_MSG_MAX_ERR ;

	memcpy (((char*)(&msglist))+msglist.len, (char*)item,item->msglen);
	msglist.len+=item->msglen;
	msglist.count++;
	DEBUG_LOG("-----------add_offline_msg-: %d :  %d", msglist.len, msglist.count);
	return this->update_msglist(userid,&msglist);
}

int Cgongfu::get_msglist(userid_t userid ,  stru_msg_list * msglist )
{
	sprintf( this->sqlstr, "select msglist from %s where userid=%u ",
			this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD(msglist, sizeof(stru_msg_list) );
	STD_QUERY_ONE_END();
}

int Cgongfu::update_msglist(userid_t userid ,  stru_msg_list * msglist )
{   
	char mysql_msglist[mysql_str_len(sizeof (stru_msg_list))];
	DEBUG_LOG("-----------update-: %d :  %d", msglist->len, msglist->count);
	
	set_mysql_string(mysql_msglist,(char*)msglist, msglist->len );
	sprintf( this->sqlstr, " update %s set \
		msglist='%s'\
		where userid=%u ",
		this->get_table_name(userid),
		mysql_msglist,
		userid );
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}   

int Cgongfu::is_user_existed(userid_t userid,bool* is_existed, uint32_t* regtime)
{
//	bool tmp_existed = false;
//	int tmp_ret = is_user_existed_ex(userid, &tmp_existed);

//	if (tmp_existed) {
		*is_existed = false;

		sprintf( this->sqlstr, "select regtime from %s where userid=%u",
				this->get_table_name(userid),userid);
		STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_EXISTED_ERR);
				INT_CPY_NEXT_FIELD((*regtime));
				*is_existed = true;
		STD_QUERY_ONE_END();
//	}
}


int Cgongfu::is_user_existed_for_boss(userid_t userid,bool* is_existed)
{
	sprintf( this->sqlstr, "select count(*) from %s where userid=%u",
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_EXISTED_ERR);
			INT_CPY_NEXT_FIELD((*is_existed));
	STD_QUERY_ONE_END();
}

int Cgongfu::add_user(userid_t userid, uint32_t parentid)
{
	GEN_SQLSTR( this->sqlstr, "insert into %s (userid,flag,regtime,vip,Ol_count,Ol_today \
		,Ol_last,Ol_time,msglist, amb_status, parentid, child_cnt, achieve_cnt, flag_bit) \
        values (%u,%u,%u,%u,%u,%u,%u,%u,'',%u, %u, %u, %u, '')",
		this->get_table_name(userid),userid,0,uint32_t(time(NULL)),0,0,0,0,0,0,parentid,0,0);
	return this->exec_insert_sql(this->sqlstr, DB_ERR);
}

int Cgongfu::add_user_for_boss(userid_t userid)
{
	bool  is_existed = false;
	int rettmp = is_user_existed_for_boss(userid, &is_existed);
	if (rettmp != SUCC) {
		return rettmp;
	}
	if (!is_existed) {
		GEN_SQLSTR( this->sqlstr, "replace into %s (userid) values (%u)",
			this->get_table_name(userid),userid);
		return this->exec_insert_sql(this->sqlstr, DB_ERR);
	}
	return SUCC;
}

int Cgongfu::update_onlinetime(userid_t userid  , uint32_t onlinetime )
{
	uint32_t  Ol_count,Ol_today,Ol_last,Ol_time;
	time_t now = time (NULL);

	sprintf( this->sqlstr, "select Ol_count,Ol_today,Ol_last,Ol_time \
			from %s where userid=%u ", 
			this->get_table_name(userid),userid);

	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
			Ol_count=atoi_safe(NEXT_FIELD );
			Ol_today=atoi_safe(NEXT_FIELD );
			Ol_last=atoi_safe(NEXT_FIELD );
			Ol_time=atoi_safe(NEXT_FIELD );
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	if(onlinetime>0)
		Ol_count++;

/*
	sprintf( this->sqlstr, " update %s set \
					Ol_count=Ol_count+%u,\
					Ol_today=case when DATE_FORMAT(FROM_UNIXTIME(Ol_last),'%%Y%%m%%d')=DATE_FORMAT(FROM_UNIXTIME(%u),'%%Y%%m%%d') then \
					Ol_today+%u else \
					%u end,\
					Ol_last=%u,\
					Ol_time=Ol_time+%u\
		   			where userid=%u " ,
				this->get_table_name(userid), 
					Ol_count,
					(uint32_t)now,
					onlinetime,
					onlinetime,
					(uint32_t)now,
					onlinetime,
				 	userid );
	return this->exec_update_sql(this->sqlstr,USER_ID_NOFIND_ERR );	
*/
	
	// same day 
	if (get_date(now)==get_date(Ol_last )){
		Ol_today+=onlinetime;		
	}else{
		Ol_today=onlinetime;		
	}
	Ol_last=now;
	Ol_time+=onlinetime;
	

	sprintf( this->sqlstr, " update %s set \
					Ol_count=%u,\
					Ol_today=%u,\
					Ol_last=%u,\
					Ol_time=%u\
		   			where userid=%u " ,
				this->get_table_name(userid), 
					Ol_count,
					Ol_today,
					Ol_last,
					Ol_time,
				 	userid );
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cgongfu::get_amb_status(userid_t userid, uint8_t* amb_status)
{
    sprintf(this->sqlstr, "select amb_status from %s where userid=%u",
        this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_EXISTED_ERR);
			INT_CPY_NEXT_FIELD((*amb_status));
	STD_QUERY_ONE_END();
}

int Cgongfu::get_user_gfcoin(userid_t userid, uint32_t* gfcoin)
{
    sprintf(this->sqlstr, "select gold_coin from %s where userid=%u",
        this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_EXISTED_ERR);
			INT_CPY_NEXT_FIELD((*gfcoin));
	STD_QUERY_ONE_END();
}

int Cgongfu::get_amb_achieve_count(userid_t userid, uint32_t* cnt)
{
    sprintf(this->sqlstr, "select achieve_cnt from %s where userid=%u",
        this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_EXISTED_ERR);
			INT_CPY_NEXT_FIELD((*cnt));
	STD_QUERY_ONE_END();
}

int Cgongfu::update_user_count(userid_t userid, const char* row_name)
{
#ifndef VERSION_TAIWAN
    uint8_t amb_status = 0;
    int ret = this->get_amb_status(userid, &amb_status);
    if (ret != SUCC) {
        return ret;
    }
    if (amb_status != 1) {
        return USER_ID_ABSENT_AMB;
    }
#endif
	sprintf( this->sqlstr, " update %s set %s=%s+1 where userid=%u ",
		this->get_table_name(userid), row_name, row_name, userid );
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cgongfu::update_amb_status(userid_t userid, uint8_t status)
{
	sprintf( this->sqlstr, " update %s set amb_status=%u where userid=%u ",
		this->get_table_name(userid), status, userid );
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cgongfu::update_amb_reward_flag(userid_t userid, uint8_t buf[40])
{
	char str_mysql[mysql_str_len(40)];

	set_mysql_string(str_mysql, (char*)(buf), 40);
	
	GEN_SQLSTR( this->sqlstr, "update %s set flag_bit='%s'where userid=%u",
		this->get_table_name(userid), str_mysql, userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cgongfu::get_act_record(userid_t userid, uint8_t *buf)
{
    GEN_SQLSTR(this->sqlstr,"select act_record from %s where userid = %u ",
        this->get_table_name(userid),userid);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        BIN_CPY_NEXT_FIELD(buf, 40);
    STD_QUERY_ONE_END();
}

int Cgongfu::set_act_record(userid_t userid, uint8_t buf[40])
{
	char str_mysql[mysql_str_len(40)];

	set_mysql_string(str_mysql, (char*)(buf), 40);
	
	GEN_SQLSTR( this->sqlstr, "update %s set act_record='%s'where userid=%u",
		this->get_table_name(userid), str_mysql, userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cgongfu::get_vip_info(userid_t userid, gf_get_user_vip_out* out)
{
    GEN_SQLSTR(this->sqlstr,"select vip, x_value+sword_value, vip_month_count, start_time, end_time, \
        auto_incr, chn_id from %s where userid = %u ",
        this->get_table_name(userid),userid);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(out->vip_is);
		INT_CPY_NEXT_FIELD(out->x_value);
		INT_CPY_NEXT_FIELD(out->vip_month_cnt);
		INT_CPY_NEXT_FIELD(out->start_tm);
		INT_CPY_NEXT_FIELD(out->end_tm);
		INT_CPY_NEXT_FIELD(out->auto_incr);
		INT_CPY_NEXT_FIELD(out->method);
    STD_QUERY_ONE_END_WITHOUT_RETURN();
    return SUCC;
}

int Cgongfu::get_necessary_info_for_login(userid_t userid, gf_user_self_define* out)
{
    GEN_SQLSTR(this->sqlstr,"select forbid_flag, forbid_time,deadline, \
        vip, x_value+sword_value,start_time,end_time \
        from %s where userid = %u ",
        this->get_table_name(userid),userid);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(out->forbid_flag);
		INT_CPY_NEXT_FIELD(out->forbid_time);
		INT_CPY_NEXT_FIELD(out->deadline);
		INT_CPY_NEXT_FIELD(out->vip_is);
		INT_CPY_NEXT_FIELD(out->x_value);
		INT_CPY_NEXT_FIELD(out->start_time);
		INT_CPY_NEXT_FIELD(out->end_time);
    STD_QUERY_ONE_END_WITHOUT_RETURN();
    return SUCC;
}

int Cgongfu::set_vip(userid_t userid, gf_set_vip_in* p_in)
{
    if (p_in->vip_is == 9) {
        GEN_SQLSTR(this->sqlstr, "update %s set vip=9, vip_month_count=%u, \
            start_time=%u, end_time=%u, auto_incr=%u where userid=%u",
            this->get_table_name(userid), p_in->vip_month_cnt,
            p_in->start_tm, p_in->end_tm, p_in->auto_incr, userid);
    } else if (p_in->vip_is == 0) {
        GEN_SQLSTR(this->sqlstr, "update %s set vip=2, auto_incr=%u where userid=%u",
            this->get_table_name(userid), p_in->auto_incr, userid);
    } else {
        GEN_SQLSTR(this->sqlstr, "update %s set vip=3, vip_month_count=%u, \
            start_time=%u, end_time=%u, auto_incr=%u where userid=%u",
            this->get_table_name(userid), p_in->vip_month_cnt,
            p_in->start_tm, p_in->end_tm, p_in->auto_incr, userid);
    }

    return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cgongfu::set_int_add_value(userid_t userid, uint32_t add_value, const char* row_name)
{
    GEN_SQLSTR(this->sqlstr, "update %s set %s=%s+%u where userid=%u",
        this->get_table_name(userid), row_name, row_name, add_value, userid);
    return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cgongfu::get_int_value(userid_t userid, const char* row_name, uint32_t* value)
{
    GEN_SQLSTR(this->sqlstr,"select %s from %s where userid = %u ",
        row_name, this->get_table_name(userid),userid);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD((*value));
    STD_QUERY_ONE_END();
}

int Cgongfu::set_int_value(userid_t userid, uint32_t value, const char* row_name)
{
    GEN_SQLSTR(this->sqlstr, "update %s set %s=%u where userid=%u",
        this->get_table_name(userid), row_name, value, userid);
    return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cgongfu::set_vip_exvalue(userid_t userid, gf_set_vip_exinfo_in* p_in)
{
    GEN_SQLSTR(this->sqlstr, "update %s set vip_exbag=%u,vip_exwarehouse=%u where userid=%u",
        this->get_table_name(userid), p_in->exbag, p_in->exwarehouse, userid);
    return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cgongfu::update_account_forbid(userid_t userid, uint32_t flag, uint32_t limit_tm)
{
    uint32_t curtime = time(NULL);
    GEN_SQLSTR(this->sqlstr, "update %s set forbid_flag=%u, forbid_time=%u, deadline=%u where userid=%u",
        this->get_table_name(userid), flag, curtime, limit_tm, userid);
    return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cgongfu::get_parent_id(userid_t userid, userid_t* p_id)
{
	sprintf( this->sqlstr, "select parentid from %s where userid=%u ", 
		this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_id );
	STD_QUERY_ONE_END();
}
