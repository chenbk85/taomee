/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_stone_move.h
 *
 *    Description:  摩尔搬石头，需要记录每天搬的石头和总共搬的石头
 *
 *        Version:  1.0
 *        Created:  01/25/2010 02:54:59 PM
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra Croominf_stone_move.h
 *
 *         Author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#ifndef CROOMINFO_STONE_MOVE_INC
#define CROOMINFO_STONE_MOVE_INC

#include <cstring>
#include "CtableRoute10x10.h"
#include "proto.h"
#include "benchapi.h"

class Croominfo_stone_move: public CtableRoute10x10
{
public:
	Croominfo_stone_move(mysql_interface * db )
		:CtableRoute10x10( db, "ROOMINFO","t_roominfo_stone_move","userid" )
	{
	
	}

	void userid(const userid_t uid)
	{
		_userid=uid;
	}

	/// 把插入一个空的记录，此时所有值都是默认值，除了键值userid
	int insert();

	/**
	 * @brief 取得指定用户当天搬石头数据和总搬石头数
	 * @param uint32_t& 用户返回今天的搬石头数
	 * @param uint32_t& total 用于返回全部的石头数
	 */
	int stone_today_total_get(uint32_t& today, uint32_t& total);

	/**
	 * @brief 设置指定用户搬石头数
	 * @param uint32_t 用户传入今天的搬石头数
	 * @param uint32_t total 用于传入全部的石头数
	 */
	int stone_today_total_set(uint32_t today, uint32_t total);

	/**
	 * @brief 用于设置用户搬石头数，如果数据库中不存在记录，则插入
	 * @param uint32_t 用户传入今天的搬石头数
	 * @param uint32_t total 用于传入全部的石头数
	 */
	int stone_today_total_replace(uint32_t today, uint32_t total);

	int penguin_egg_get(uint32_t& egg_num, uint32_t& from_client)
	{
		sprintf( this->sqlstr, "select penguin_egg_count, penguin_from_client from %s where userid=%d", 
			this->get_table_name(_userid),
			_userid
		);

		STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
			INT_CPY_NEXT_FIELD(egg_num);
			INT_CPY_NEXT_FIELD(from_client);
		STD_QUERY_ONE_END();
	}

	int warrior_draw_get(uint32_t& warrior_draw_times, uint32_t& warrior_luk_times)
	{
		sprintf( this->sqlstr, "select warrior_lucky_draw_times, warrior_lucky_draw_is_luck_times from %s where userid=%d", 
			this->get_table_name(_userid),
			_userid
		);

		STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
			INT_CPY_NEXT_FIELD(warrior_draw_times);
			INT_CPY_NEXT_FIELD(warrior_luk_times);
		STD_QUERY_ONE_END();
	}

	int penguin_egg_set_client(uint32_t from_client)
	{
		sprintf(this->sqlstr, "update %s set  penguin_from_client=%u where userid=%d", 
			this->get_table_name(_userid),
			from_client,
			_userid
		);

		STD_SET_RETURN(this->sqlstr,_userid,USER_ID_NOFIND_ERR);	
	}

	int penguin_egg_set()
	{
		//每次只能增加，并且上限是3
		sprintf(this->sqlstr, "update %s set penguin_egg_count=if(penguin_egg_count>=3, 1, penguin_egg_count+1) where userid=%d", 
			this->get_table_name(_userid),
			_userid
		);

		STD_SET_RETURN(this->sqlstr,_userid,USER_ID_NOFIND_ERR);	
	}

	int warrior_draw_set(uint32_t warrior_draw_times, uint32_t warrior_luk_times)
	{
		sprintf(this->sqlstr, "update %s set"
		" warrior_lucky_draw_times=%u,"
		" warrior_lucky_draw_is_luck_times=%u"
		" where userid=%d", 
			this->get_table_name(_userid),
			warrior_draw_times,
			warrior_luk_times,
			_userid
		);

		STD_SET_RETURN(this->sqlstr,_userid,USER_ID_NOFIND_ERR);	
	}

private:
	userid_t _userid; ///< 内存缓存的userid
};

#endif //CROOMINFO_STONE_MOVE_INC
