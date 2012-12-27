/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_fire.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/11/2009 04:19:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_fire.h"
/* 
 * @brief 构造函数，十个库每个库中十个表
 */
Croominfo_fire :: Croominfo_fire(mysql_interface *db) : CtableRoute10x10(db, "ROOMINFO", "t_jy_fire", "userid")
{

}

 /*
  * @brief 设置家园的着火状态
  * @param state 家园的着火状态，state为1表示处于着火状态，state为2表示已经着过火
  * @param fire 如果家园有正常状态变为着火状态，设置家园刚开始着火的火势的大小
  * @return VALUE_OUT_OF_RANGE_ERR  超出范围
//  */
//int Croominfo_fire :: set_state(const uint32_t state, const uint32_t fire, const uint32_t pos, const userid_t userid)
//{
//	DEBUG_LOG("===state %u", state);
//	if (state == 1) {
//		sprintf(this->sqlstr, "update %s set state = %u, fire_power = %u, pos = %u where userid=%u",
//				this->get_table_name(userid),
//				state,
//				fire,
//				pos,
//				userid
//			   );
//
//	} else if(state == 2) {
//		sprintf(this->sqlstr, "update %s set state = %u where userid=%u",
//				this->get_table_name(userid),
//				state,
//				userid
//			);
//	} else {
//		return VALUE_OUT_OF_RANGE_ERR;
//	}
//
//	STD_SET_RETURN_EX(this->sqlstr, SUCC);
//}
//

/* 
 * @brief 设置家园的火势
 * @param fire 家园的火势应该减少多少
 */
inline int Croominfo_fire :: set_fire(const uint32_t fire, const userid_t userid)
{
	sprintf(this->sqlstr, "update %s set fire_power = if(fire_power > %u ,\
						  fire_power - %u, 0) where userid = %u",
			this->get_table_name(userid),
			fire,
			fire,
			userid
			);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}
/* 
 * @brief 设置家园的火势，并判断是否第一次熄灭
 *
 */
int Croominfo_fire :: die_first(uint32_t &die_flag, uint32_t fire, const userid_t userid)
{
	uint32_t die;

	this->set_fire(fire, userid);
	int ret = this->get_die(fire, die, userid);
	if (ret != SUCC) {
		return ret;
	}
	DEBUG_LOG("===die %u", die);
	if ((fire == 0) && ((die & 0x02) == 0)) {
		die |= 0x02;
		ret = update_die_alive(die, 2, userid);
		if (ret != SUCC) {
			return ret;
		}
		die_flag = 2;
	}
	return 0;
}

/* 
 * @brief 更新着火状态和是否是第一次着火和第一次熄灭
 *
 */

int Croominfo_fire :: update_die_alive(const uint32_t alive, const uint32_t state, const userid_t userid)
{
	if (state == 1) {
		uint32_t pos = rand() % 4 + 1;
		uint32_t fire = rand() % 2;
		if (fire == 1){
			fire = 30;
		} else {
			fire = 50;
		}
		sprintf(this->sqlstr, "update %s set state = %u, fire_power = %u, pos = %u, alive_die = %u\
							  where userid = %u",
							  this->get_table_name(userid),
							  state,
							  fire,
							  pos,
							  alive,
							  userid
			   );
	} else {
		sprintf(this->sqlstr, "update %s set state = %u, alive_die = %u where userid = %u",
				this->get_table_name(userid),
				state,
				alive,
				userid
				);

	}
	STD_SET_RETURN_EX(this->sqlstr, RECORD_NOT_EXIST_ERR);
}

/*
 * @brief 得到火的大小和是否第一次熄灭的标志
 *
 */
int Croominfo_fire :: get_die(uint32_t &fire, uint32_t &die, const userid_t userid)
{
	sprintf(this->sqlstr, "select fire_power, alive_die from %s where userid=%u",
			this->get_table_name(userid),
			userid
		   );

 	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
         INT_CPY_NEXT_FIELD(fire);
		 INT_CPY_NEXT_FIELD(die);
     STD_QUERY_ONE_END();
}


/* 
 * @param 得到记录的信息，如果记录不存在，就插入新的记录
 * @param out 返回家园的着火状态信息，state和fire
 */
int Croominfo_fire :: get(const userid_t userid, roominfo_set_jy_fire_out &out) 
{
	uint32_t alive = 0;

	int ret = this->get_record(out, alive, userid);
	if (ret != SUCC) {
		ret = this->insert(userid);
		if (ret != SUCC) {
			return ret;
		}
	}
	if ((alive & 0x01) == 0) {
		if (rand() % 100 < 5) {
			alive |= 0x01;
			ret = this->update_die_alive(alive, 1, userid);
			if (ret != SUCC) {
				return ret;
			}
			ret  = this->get_record(out, alive, userid);
			if (ret != SUCC) {
				return ret;
			}
			out.alive_die = 1;
			return 0;
		}
		//out.alive_die = 1;
		//return 0;
	}
	out.alive_die = 0;
	return 0;
}

/*
 * @param 得到用户的家园着火状态的信息
 * @param out 返回家园的着火状态信息
 */ 
int Croominfo_fire :: get_record(roominfo_set_jy_fire_out &out, uint32_t &alive, const uint32_t userid)
{
	sprintf(this->sqlstr, "select state, fire_power, pos, alive_die from %s where userid=%u",
			this->get_table_name(userid),
			userid
		   );

 	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
         INT_CPY_NEXT_FIELD(out.state);
         INT_CPY_NEXT_FIELD(out.fire);
		 INT_CPY_NEXT_FIELD(out.pos);
		 INT_CPY_NEXT_FIELD(alive);
     STD_QUERY_ONE_END();
}


/* 
 * @brief 如不存在，插入一条记录
 */
inline int Croominfo_fire :: insert(const userid_t userid)
{
	sprintf(this->sqlstr, "insert into %s values(%u, 0, 0, 0, 0)",
			this->get_table_name(userid),
			userid
			);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}
