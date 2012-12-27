/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_diningroom.cpp
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2010年03月09日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  tommychen, tommychen@taomee.com
 *
 * =====================================================================================
 */

#include "Cuser_diningroom.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

/* @brief 类构造函数
 * @param db 数据库句柄
 */
Cuser_diningroom::Cuser_diningroom(mysql_interface * db ) 
	:CtableRoute100x10( db, "USER", "t_user_diningroom", "userid")
{ 
}


int Cuser_diningroom::insert(userid_t userid, user_diningroom_create_room_in* p_in) 
{

	char room_name_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(room_name_mysql,p_in->room_name,NICK_LEN);

	sprintf(this->sqlstr, "insert into %s (userid, roomid, room_name, style_id, level, evaluate, type_id, inner_style) values(%u, %u, '%s', %u, 1, 200, %u, %u)",
			this->get_table_name(userid),
			userid,
			p_in->roomid,
			room_name_mysql,
			p_in->style_id,
			p_in->type_id,
			p_in->inner_style
			);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_diningroom::set_room_name(userid_t userid, user_diningroom_set_room_name_in *p_in)
{
	char room_name_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(room_name_mysql,p_in->room_name,NICK_LEN);
	sprintf( this->sqlstr, "update %s set room_name = '%s' where userid = %u and roomid = %u",
		this->get_table_name(userid),
   		room_name_mysql,
		userid,
		p_in->roomid );
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}


int Cuser_diningroom::set_room_style(userid_t userid, user_diningroom_set_room_style_in *p_in)
{
	sprintf( this->sqlstr, "update %s set style_id = %u where userid = %u and roomid = %u",
		this->get_table_name(userid),
   		p_in->style_id,
		userid,
		p_in->roomid );
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cuser_diningroom::set_room_inner_style(userid_t userid, user_diningroom_set_room_inner_style_in *p_in)
{
	sprintf( this->sqlstr, "update %s set inner_style = %u where userid = %u and roomid = %u",
		this->get_table_name(userid),
   		p_in->inner_style,
		userid,
		p_in->roomid );
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cuser_diningroom::get_diningroom_info(userid_t userid, uint32_t type_id, user_get_diningroom_out_header *p_out)
{
	sprintf(this->sqlstr, "select userid, roomid, room_name, type_id, style_id, exp, xiaomee, evaluate,level, inner_style, honor_flag\
						   from %s where userid = %u and type_id = %u",
			this->get_table_name(userid),
			userid,
			type_id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->userid);
		INT_CPY_NEXT_FIELD(p_out->roomid);
		BIN_CPY_NEXT_FIELD(&(p_out->room_name), NICK_LEN);
		INT_CPY_NEXT_FIELD(p_out->type_id);
		INT_CPY_NEXT_FIELD(p_out->out_style);
		INT_CPY_NEXT_FIELD(p_out->exp);
		INT_CPY_NEXT_FIELD(p_out->xiaomee);
		INT_CPY_NEXT_FIELD(p_out->evaluate);
		INT_CPY_NEXT_FIELD(p_out->level);
		INT_CPY_NEXT_FIELD(p_out->inner_style);
		BIN_CPY_NEXT_FIELD(&(p_out->honor_flag), sizeof(p_out->honor_flag));
	STD_QUERY_ONE_END();
}

int Cuser_diningroom::get_board_info(userid_t userid, uint32_t type_id, user_dining_get_user_board_info_out& out)
{
	sprintf(this->sqlstr, "select  off_xiaomee, off_evaluate  from %s where userid = %u and type_id = %u",
			this->get_table_name(userid),
			userid,
			type_id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(out.off_xiaomee);
		INT_CPY_NEXT_FIELD(out.off_evaluate);
	STD_QUERY_ONE_END();
}

int Cuser_diningroom::update_exp_inc(userid_t userid, uint32_t exp)
{
	sprintf( this->sqlstr, "update %s set exp = exp + %u  where userid = %u ",
		this->get_table_name(userid),
   		exp,
		userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}


int Cuser_diningroom::update_xiaomee_inc(userid_t userid, uint32_t xiaomee)
{
	sprintf( this->sqlstr, "update %s set xiaomee = xiaomee + %u  where userid = %u ",
		this->get_table_name(userid),
   		xiaomee,
		userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cuser_diningroom::update_xiaomee_dec(userid_t userid, uint32_t xiaomee)
{
	sprintf( this->sqlstr, "update %s set xiaomee = xiaomee - %u  where userid = %u ",
		this->get_table_name(userid),
   		xiaomee,
		userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cuser_diningroom::update_evaluate_dec(userid_t userid, uint32_t evaluate)
{
	sprintf( this->sqlstr, "update %s set evaluate = evaluate - %u  where userid = %u ",
		this->get_table_name(userid),
   		evaluate,
		userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cuser_diningroom::update_evaluate_inc(userid_t userid, uint32_t evaluate)
{
	sprintf( this->sqlstr, "update %s set evaluate = evaluate + %u  where userid = %u ",
		this->get_table_name(userid),
   		evaluate,
		userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cuser_diningroom::update_time(userid_t userid, uint32_t cur_time)
{
	sprintf( this->sqlstr, "update %s set time = %u  where userid = %u ",
		this->get_table_name(userid),
   		cur_time,
		userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cuser_diningroom::update_evaluate_xiaomee_time_inc(userid_t userid, uint32_t evaluate, uint32_t xiaomee, uint32_t time)
{
	sprintf( this->sqlstr, "update %s set evaluate = evaluate + %u, xiaomee = xiaomee + %u, time = %u  where userid = %u ",
		this->get_table_name(userid),
   		evaluate,
		xiaomee,
		time,
		userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}


int Cuser_diningroom::get_time(userid_t userid, uint32_t& eat_time)
{
	sprintf(this->sqlstr, "select time  from %s where userid = %u ",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(eat_time);
	STD_QUERY_ONE_END();
}

int Cuser_diningroom::set_evaluate(userid_t userid, uint32_t evaluate)
{
	sprintf( this->sqlstr, "update %s set evaluate = %u  where userid = %u ",
		this->get_table_name(userid),
   		evaluate,
		userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cuser_diningroom::update_off_evaluate_dec(userid_t userid, uint32_t evaluate)
{
	sprintf( this->sqlstr, "update %s set evaluate = evaluate - %u, off_evaluate = %u  where userid = %u ",
		this->get_table_name(userid),
   		evaluate,
		evaluate,
		userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cuser_diningroom::update_off_evaluate_inc(userid_t userid, uint32_t evaluate)
{
	sprintf( this->sqlstr, "update %s set evaluate = evaluate + %u, off_evaluate = %u  where userid = %u ",
		this->get_table_name(userid),
   		evaluate,
		evaluate,
		userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cuser_diningroom::update_off_xiaomee_inc(userid_t userid, uint32_t xiaomee)
{
	sprintf( this->sqlstr, "update %s set xiaomee = xiaomee + %u, off_xiaomee = %u  where userid = %u ",
		this->get_table_name(userid),
   		xiaomee,
		xiaomee,
		userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cuser_diningroom::get_level_evaluate(userid_t userid, uint32_t& level, uint32_t& evaluate)
{
	sprintf(this->sqlstr, "select  evaluate,level from %s where userid = %u ",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(evaluate);
		INT_CPY_NEXT_FIELD(level);
	STD_QUERY_ONE_END();
}

int Cuser_diningroom::get_user_diningroom_count(userid_t userid, uint32_t type_id, uint32_t& count)
{
	sprintf(this->sqlstr, "select  count(*) as num from %s where userid = %u and type_id = %u ",
			this->get_table_name(userid),
			userid,
			type_id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

int Cuser_diningroom::set_honor_flag(userid_t userid, uint32_t index)
{
	if (index < 1)
	{
		return SUCC;
	}

	char honor_mysql[mysql_str_len(8)];

	uint8_t honor[8]={ };
	sprintf(this->sqlstr, "select  honor_flag from %s where userid = %u ",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(honor, 8);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	if ( (honor[7]&0x01) != 0x01)
	{
		for (uint32_t i = 1; i < 8; i++)
		{
			honor[i] = 0;
		}

		honor[0] &= 0xDF;

		honor[7] |= 0x01;
	}
	
	uint32_t num = (index-1)/8;
	honor[num] = honor[num]|(0x01<<((index-1)&7));

	set_mysql_string(honor_mysql,(char*)(honor),8);
	
	sprintf( this->sqlstr, " update %s set honor_flag = '%s' \
		   			where userid = %u",
			this->get_table_name(userid), 
			honor_mysql,
			userid
		);
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}

int Cuser_diningroom::event_num_set(userid_t userid)
{
	sprintf( this->sqlstr, "update %s set event_num = event_num + 1  where userid = %u ",
		this->get_table_name(userid),
		userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cuser_diningroom::event_num_get(userid_t userid, uint32_t &count)
{
	sprintf(this->sqlstr, "select event_num from %s where userid = %u ",
                        this->get_table_name(userid),
                        userid
                        );

        STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
                INT_CPY_NEXT_FIELD(count);
        STD_QUERY_ONE_END();
}

int Cuser_diningroom::update_exp_dec(userid_t userid, uint32_t exp)
{
	sprintf( this->sqlstr, "update %s set exp = exp - %u  where userid = %u ",
		this->get_table_name(userid),
   		exp,
		userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cuser_diningroom::get_exp(userid_t userid, uint32_t& exp_out)
{
	sprintf(this->sqlstr, "select exp from %s where userid = %u ",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(exp_out);
	STD_QUERY_ONE_END();
}




