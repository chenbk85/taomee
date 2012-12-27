/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_login_gifts.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/10/2011 02:35:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_login_gifts.h"

uint32_t date[] = {
	20110301,
	20110302,
	20110303,
	20110304,
	20110305,
	20110306,
	20110307,
	20110308,
	20110309,
	20110310,
	20110311,
	20110312,
	20110313,
	20110314,
	20110315,
	20110316,
	20110317,
	20110318,
	20110319,
	20110320,
	20110321,
	20110322,
	20110323,
	20110324,
	20110325,
	20110326,
	20110327,
	20110328,
	20110329,
	20110330,
	20110331,
	20110401,
	20110402,
	20110403,
	20110404,
	20110405,
	20110406,
	20110407,
	20110408,
	20110409,
	20110410,
	20110411,
	20110412,
	20110413,
	20110414,
	20110415,
	20110416,
	20110417,
	20110418,
	20110419,
	20110420,
	20110421,
	20110422,
	20110423,
};

Croominfo_login_gifts::Croominfo_login_gifts(mysql_interface *db):
	CtableRoute10x10(db, "ROOMINFO","t_roominfo_login_gifts", 
			"userid")
{

}

/*
 * @brief 插入记录
 */
int Croominfo_login_gifts::insert(userid_t userid, uint32_t count, uint32_t total,
		uint32_t round)
{

	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u)",
			this->get_table_name(userid), userid, count, total, round, get_date(time(NULL)));
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief 更新记录 
 */
int Croominfo_login_gifts::update(userid_t userid,  roominfo_query_login_gifts_out *p_out)
{	
	uint32_t today = get_date(time(NULL));
	int before = -1;
	for(int k = 0; k< (int)(sizeof(date)/sizeof(uint32_t)); ++k){
		if(date[k] == p_out->date){
			before = k;
			break;
		}
	}	
	roominfo_query_login_gifts_out temp={0};
	temp.count = p_out->count;
	temp.total = p_out->total;
	temp.round = p_out->round;
	if(before != -1 && date[before + 1] == today){//连续登陆
		p_out->count += 1;
		p_out->total += 1;
		temp.count += 1;
		temp.total += 1;
		if(p_out->count == 5){
			temp.count = 0;
			temp.round += 1;
		}
	}else{//不是连续登陆
		p_out->count = 1;
		p_out->total = 1;
		temp.count= 1;
		if(temp.total >= 50){
			p_out->round = 1;
			temp.round = 1;
		}
		temp.total= 1;
	}
	p_out->flag = 1;
	p_out->date = today;
	temp.date = today;

	sprintf(this->sqlstr, "update %s set count = %u, total = %u, round = %u,date = %u where userid = %u",
			this->get_table_name(userid), temp.count, temp.total,
			temp.round, temp.date, userid);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/*
 * @brief 查询记录 
 */
int Croominfo_login_gifts::query(userid_t userid, roominfo_query_login_gifts_out *p_out)
{
	sprintf(this->sqlstr, "select count, total, round, date  from %s where userid = %u",
			this->get_table_name(userid), userid);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->count);
		INT_CPY_NEXT_FIELD(p_out->total);
		INT_CPY_NEXT_FIELD(p_out->round);
		INT_CPY_NEXT_FIELD(p_out->date);
	STD_QUERY_ONE_END();
}
