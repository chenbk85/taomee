/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_fire.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/11/2009 04:14:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include "CtableRoute10x10.h"
#include "benchapi.h"
#include "proto.h"

/* 
 * brief 对ROOMINFO库中的t_jy_fire表进行操作，包括插入记录，更新记录和获取记录信息
 */
class Croominfo_fire : public CtableRoute10x10 {
	public:
		/* 信息以十个库保存，没个库中包含十个表 */
		Croominfo_fire(mysql_interface *db);
		int die_first(uint32_t &die_flag, uint32_t fire, const userid_t userid);
		/* 设置家园的着火状态 */
		//int set_state(const uint32_t state, const uint32_t fire, const uint32_t pos, const userid_t userid);
		/* 设置家园的着火大小 */
		//int set_fire(const uint32_t fire, const userid_t userid);
		/* 得到家园着火状态 */
		int get(const userid_t userid, roominfo_set_jy_fire_out &out);
		/* 得到家园着火记录*/
		int get_record(roominfo_set_jy_fire_out &out, uint32_t &alive, const userid_t userid);
	private:
		/* 对火势进行减少 */	
		int set_fire(const uint32_t fire, const userid_t userid);
		/* 得到火势的大小和是否第一次熄灭 */
		int get_die(uint32_t &fire, uint32_t &die, const userid_t userid);
		/* 更新状态和熄灭状态 */
		int update_die_alive(const uint32_t alive, const uint32_t state, const userid_t userid);
		/* 得到家园着火记录*/
		//int get_record(roominfo_set_jy_fire_out &out, uint32_t &alive, const userid_t userid);
		/* 当用户的记录还不存在，插入一条记录 */
		int insert(const userid_t userid);
};
