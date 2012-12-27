/*
 * =====================================================================================
 *
 *       Filename:  Clamuclass.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/31/2010 05:17:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Clamu_classroom.h"
#include "proto.h"
#include "benchapi.h"

Clamu_classroom::Clamu_classroom(mysql_interface * db ):Ctable(db, "DININGROOM","t_lamu_classroom" )
{
}

/* 
 * @brief 创建拉姆教室
 */
int Clamu_classroom::insert(userid_t userid, lamu_classroom_user_create_classroom_in *p_in, 
		lamu_classroom_user_create_classroom_out& out)
{
	sprintf( this->sqlstr, "insert into %s (room_id, userid, room_name, style_id) values(NULL, %u, '拉姆教室', %u)",
			this->get_table_name(),
			userid,
			p_in->style_id
			);
	STD_INSERT_GET_ID(this->sqlstr, USER_ID_EXISTED_ERR, out.room_id);
}

/*
 * @brief 设置拉姆教室的名称 
 */
int Clamu_classroom::set_lamu_classroom_name(lamu_classroom_user_set_classroom_name_in *p_in)
{
	char room_name_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(room_name_mysql, p_in->room_name, NICK_LEN);
	sprintf( this->sqlstr, "update %s set room_name = '%s' where room_id = %u",
			this->get_table_name(),
			room_name_mysql,
			p_in->room_id 
			);
	STD_SET_RETURN_EX(this->sqlstr, LAMU_CLASSROOM_ROOMID_NOFIND_ERR);
}

/*
 * @brief 得到名称
 */
int Clamu_classroom::get_lamu_classroom_exp_and_name(userid_t userid, lamu_classroom_tw_get_exp_and_name_out *p_out)
{
	sprintf(this->sqlstr, "select exp, room_name from %s where userid = %u", this->get_table_name(), userid);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->exp);
		BIN_CPY_NEXT_FIELD(p_out->room_name, NICK_LEN);
	STD_QUERY_ONE_END();
}

/* 
 * @brief 得到一格4间拉姆教室的信息
 */
int Clamu_classroom::get_lamu_classroom_frame(uint32_t frame_id ,
		uint32_t *p_count, lamu_classroom_user_get_frame_classroom_out_item** pp_list)
{
	uint32_t roomid_end = frame_id * 4;
	uint32_t roomid_begin = roomid_end - 3;
	sprintf( this->sqlstr, "select room_id, userid, room_name, style_id from %s \
			where room_id >= %u and room_id <= %u",
			this->get_table_name(), roomid_begin, roomid_end);
	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_list, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_list+i)->room_id);
		INT_CPY_NEXT_FIELD((*pp_list+i)->userid);
		BIN_CPY_NEXT_FIELD((*pp_list+i)->room_name, NICK_LEN);
		INT_CPY_NEXT_FIELD((*pp_list+i)->style_id);
	STD_QUERY_WHILE_END();
}
 
/*
 * @brief 拉姆教室的最大room_id
 */
int Clamu_classroom::get_lamu_classroom_count(uint32_t& count)
{
	sprintf( this->sqlstr, "select max(room_id) as count from %s",
			this->get_table_name()
			);
	STD_QUERY_ONE_BEGIN( this->sqlstr, DB_ERR) ;
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

/*
 * @brief 通过米米号得到拉姆教室的room_id 
 */
int Clamu_classroom::get_lamu_classroom_roomid(userid_t userid,  uint32_t& room_id)
{
	sprintf( this->sqlstr, "select room_id from %s where userid = %u ",
			this->get_table_name(),
			userid
			);
	STD_QUERY_ONE_BEGIN( this->sqlstr, DB_ERR);
		INT_CPY_NEXT_FIELD(room_id);
	STD_QUERY_ONE_END();
}

/*
 * @brief 通过教室id得到米米号
 */
int Clamu_classroom::get_lamu_classroom_userid(userid_t room_id,  uint32_t& userid)
{
	sprintf( this->sqlstr, "select userid from %s where room_id = %u ",
			this->get_table_name(),
			room_id
			);
	STD_QUERY_ONE_BEGIN( this->sqlstr, DB_ERR);
		INT_CPY_NEXT_FIELD(userid);
	STD_QUERY_ONE_END();
}

/*
 * @brief 设置教室经验
 */
int Clamu_classroom::set_teacher_exp(userid_t userid, uint32_t teacher_exp)
{
	sprintf(this->sqlstr, "update %s set exp = %u where userid = %u", 
					this->get_table_name(), teacher_exp, userid);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 设置优秀学生人数
 */
int Clamu_classroom::set_outstand_sum(userid_t userid, uint32_t outstand_sum)
{
	sprintf(this->sqlstr, "update %s set outstand_sum = %u where userid = %u", 
					this->get_table_name(), outstand_sum, userid);
	
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 得到好友的经验和优秀学生人数
 */
int Clamu_classroom::get_friend_rank_info(uint32_t count, lamu_classroom_get_friend_rank_in_item* p_in_item,
		uint32_t *p_out_cnt, lamu_classroom_get_friend_rank_out_item **pp_out_item)
{
	std::ostringstream sql_in_string;
	for(int i = 0; i < (int)count; ++i) {
		sql_in_string << (p_in_item + i)->userid;
		if (i < (int)count - 1) {
			sql_in_string << ',';
		}
	}
	sprintf(this->sqlstr, "select userid, exp, outstand_sum from %s where userid in (%s) order by exp desc",
					this->get_table_name(),
					sql_in_string.str().c_str()
					);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, p_out_cnt) ;
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->userid);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->exp);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->outstand_sum);
	STD_QUERY_WHILE_END();
}

/*
 * @brief 设置经验和优秀学生人数
 */
int Clamu_classroom::set_exp_and_outstand_sum(userid_t userid, uint32_t exp, uint32_t outstand_sum)
{
	sprintf(this->sqlstr, "update %s set exp = %u, outstand_sum = %u where userid = %u",
					this->get_table_name(), exp, outstand_sum, userid);
	
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}


