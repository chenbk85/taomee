/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_sports_pet_info.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/10/2010 07:44:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_sports_pet_info.h"
/*
 * brief: 拉姆运动会20100513
 *
 *
 */
//using namespace std;

Croominfo_sports_pet_info::Croominfo_sports_pet_info(mysql_interface * db)
	:CtableRoute10x10(db, "ROOMINFO", "t_roominfo_sports_pet_info", "userid")
{

}



int Croominfo_sports_pet_info::reward_flag_select(uint32_t userid, uint32_t petid, uint32_t *reward_flag)
{
	sprintf(this->sqlstr, "select reward_flag from %s where petid = %u and userid= %u",
		   this->get_table_name(userid),
		   petid,
		   userid
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr, ROOMINFO_SPORTS_PET_INFO_GET_ERR);
		INT_CPY_NEXT_FIELD(*reward_flag);
	STD_QUERY_ONE_END();
}


int Croominfo_sports_pet_info::reward_flag_update(uint32_t userid, uint32_t petid)
{
	sprintf(this->sqlstr, "update %s set reward_flag = 1 where petid = %u and userid = %u",
		   this->get_table_name(userid),
		   petid,
		   userid
		   );

	STD_SET_RETURN_EX(this->sqlstr, ROOMINFO_SPORTS_PET_INFO_SET_ERR);
}

int Croominfo_sports_pet_info::reward_flag_insert(uint32_t userid, uint32_t petid)
{
	sprintf(this->sqlstr, "insert %s(petid, userid, reward_flag) values(%u, %u, 1)",
		   this->get_table_name(userid),
		   petid,
		   userid
		   );

	STD_SET_RETURN_EX(this->sqlstr, ROOMINFO_SPORTS_PET_INFO_SET_ERR);
}
