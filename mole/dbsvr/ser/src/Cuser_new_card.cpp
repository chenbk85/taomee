/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_new_card.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2010年06月21日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  tommychen, tommychen@taomee.com
 *
 * =====================================================================================
 */

#include "Cuser_new_card.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"
#include <msglog.h>


Cuser_new_card::Cuser_new_card(mysql_interface * db, Citem_change_log *p_log ) 
	:CtableRoute100x10( db,"USER","t_user_new_card","userid")
{ 
	this->msglog_file = config_get_strval("MSG_LOG_FILE");
	this->p_item_change_log = p_log;
}


int Cuser_new_card::init_new_card(userid_t userid, user_new_card_init_card_in *p_in)
{

	char cardlist_mysql[mysql_str_len(sizeof (*p_in) )];
	
	set_mysql_string(cardlist_mysql,(char*)(&(p_in->count)), 
			p_in->count*(sizeof(p_in->cards[0]))+4); 

	sprintf( this->sqlstr, "insert into  %s (userid, exp, cardlist)  values (%u, 0, '%s') ", 
			this->get_table_name(userid), userid, cardlist_mysql );
	
	STD_INSERT_RETURN(this->sqlstr, USER_NEW_CARD_IS_INITED_ERR);
}

int Cuser_new_card::get_new_card_info(userid_t userid, user_new_card_get_card_info_out *p_out, uint32_t& logdate)
{
	sprintf( this->sqlstr, "select exp, flag, dayexp, day_monster, logdate, cardlist\
		   	from %s where userid=%u ", 
			this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->exp);
		INT_CPY_NEXT_FIELD(p_out->flag);
		INT_CPY_NEXT_FIELD(p_out->day_exp);
		INT_CPY_NEXT_FIELD(p_out->day_monster);
		INT_CPY_NEXT_FIELD(logdate);
		BIN_CPY_NEXT_FIELD( &(p_out->cardList), sizeof(p_out->cardList));
	STD_QUERY_ONE_END();
}


int Cuser_new_card::add_new_card(userid_t userid, uint32_t cardid, uint32_t is_vip_opt_type)
{
	user_cardlist cardlist;
	int	ret;
	ret = this->get_new_card_list(userid, &cardlist);
	if (ret!=SUCC) return ret;

	ret = add_id_to_list((idlist*)&cardlist, cardid, CARDLIST_MAX );
	if ( ret != SUCC ) {
		if 	( ret == LIST_ID_MAX_ERR ) 
			return USER_NEW_CARDLIST_COUNT_MAX_ERR;
		else if ( ret == LIST_ID_EXISTED_ERR ) 
			return USER_NEW_CARDID_EXISTED_ERR;
		else
			return ret;
	}


	uint32_t buff[1] = {1};
	msglog(this->msglog_file,0x02024000 + cardid + 1,time(NULL),buff,sizeof(buff));

	ret = this->set_new_card_list(userid, &cardlist);
	/*
	 *114D协议,下面是道具增减统计支持 
	 */
	//if(ret == SUCC){
		//this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, cardid, 1);
	//}
	return ret;
}


int Cuser_new_card::get_new_card_list(userid_t userid, user_cardlist *p_cardlist)
{
	sprintf( this->sqlstr, "select cardlist \
		   	from %s where userid=%u ", 
			this->get_table_name(userid), userid);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_cardlist, sizeof(user_cardlist));
	STD_QUERY_ONE_END();
}


int Cuser_new_card::set_new_card_list(userid_t userid, user_cardlist *p_cardlist)
{
	char cardlist_mysql[mysql_str_len(sizeof (*p_cardlist) )];
	
	set_mysql_string(cardlist_mysql,(char*)(&(p_cardlist->count)), 
			p_cardlist->count*(sizeof(p_cardlist->cards[0]))+4); 

	sprintf( this->sqlstr, "update %s \
		   	set cardlist='%s'\
		   	where userid=%u ", 
			this->get_table_name(userid), cardlist_mysql, userid);
	
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}


int Cuser_new_card::update_exp_inc(userid_t userid, uint32_t exp_in)
{
	sprintf( this->sqlstr, "update %s  set exp= exp + %u, dayexp = dayexp + %u \
		   	where userid=%u ", 
			this->get_table_name(userid), 
			exp_in,
			exp_in,
			userid);

	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}


int Cuser_new_card::update_exp_inc_set_date(userid_t userid, uint32_t exp_in, uint32_t logdate)
{
	sprintf( this->sqlstr, "update %s  set exp= exp + %u, logdate = %u, dayexp = %u \
		   	where userid=%u ", 
			this->get_table_name(userid), 
			exp_in,
			logdate,
			exp_in,
			userid);

	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}


int Cuser_new_card::get_exp(userid_t userid, uint32_t &exp, uint32_t &flag)
{
	sprintf( this->sqlstr, "select exp,flag from %s where userid=%u ",
                        this->get_table_name(userid),userid);

        STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
                INT_CPY_NEXT_FIELD(exp);
				INT_CPY_NEXT_FIELD(flag);
        STD_QUERY_ONE_END();
}

int Cuser_new_card::get_dayexp_date(userid_t userid, uint32_t& dayexp, uint32_t& logdate)
{
	sprintf( this->sqlstr, "select logdate, dayexp  from %s where userid=%u ",
                        this->get_table_name(userid),userid);

        STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
                INT_CPY_NEXT_FIELD(logdate);
                INT_CPY_NEXT_FIELD(dayexp);
        STD_QUERY_ONE_END();
}

int Cuser_new_card::update_today_date(userid_t userid)
{
    uint32_t date = get_date(time(NULL));
    sprintf(this->sqlstr, "update %s set dayexp = 0, day_monster = 0, logdate = %u where userid=%u and logdate != %u",
        this->get_table_name(userid),
		date,
		userid,
		date);
    STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

int Cuser_new_card::update_monster_inc(userid_t userid, uint32_t count)
{
	sprintf( this->sqlstr, "update %s set  day_monster = day_monster + %u \
		   	where userid=%u ", 
			this->get_table_name(userid),
			count,
			userid);

	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

int Cuser_new_card::set_flag(userid_t userid, uint32_t flag)
{
	sprintf( this->sqlstr, "update %s set  flag = %u \
		   	where userid=%u ", 
			this->get_table_name(userid),
			flag,
			userid);

	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

int Cuser_new_card::set_exp(userid_t userid, uint32_t exp)
{
	sprintf( this->sqlstr, "update %s set  exp = %u \
		   	where userid=%u ", 
			this->get_table_name(userid),
			exp,
			userid);

	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

