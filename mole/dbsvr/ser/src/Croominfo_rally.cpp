/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_gen_vip.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/08/2009 09:21:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_rally.h"
/* @brief 构造函数
 * @param db 数据库类指针
 */
Croominfo_rally :: Croominfo_rally(mysql_interface *db)
	              : CtableRoute10x10(db, "ROOMINFO", "t_roominfo_rally", "userid")
{

}

/* @brief 插入一条记录 
 * @param userid 用户的米米号
 * @param team 队的ID号
 */
int Croominfo_rally :: insert(userid_t userid, uint32_t team)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, 0, 0, 0, 0, 0, 0, 0)",
			this->get_table_name(userid),
			userid,
			team
		   );
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}
/* @brief 用户报名 
 * @param userid 用户的米米号
 * @param p_team 返回用户报名了哪个队
 */
int Croominfo_rally :: sign_up(userid_t userid, uint32_t *p_team)
{
	*p_team = rand() % 2 + 1; /* 随机生成报的队的ID号 */
	uint32_t ret = this->insert(userid, *p_team);
	if (ret == DB_ERR) {
		return YOU_HAVE_SIG_FIRE_CUP_ERR; 
	}
	return ret;
}

/* @brief 更新用户的分数，并且是参加的赛道数加一 
 * @param userid 用户的ID号
 * @param add_score 加的分数
 * @param index 赛道的ID号
 */
int Croominfo_rally :: update_score_race(userid_t userid, uint32_t add_score, uint32_t index)
{
	if (index <=0 || index > 4) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	sprintf(this->sqlstr, "update %s set score = score + %u, day_score = day_score + %u,\
						   race%u = race%u + 1 where userid = %u",
			this->get_table_name(userid),
			add_score,
			add_score,
			index,
			index,
			userid
			);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/* @brief 得到用户当天得到的分数  
 * @param userid 用户的米米号
 * @param p_day 保存当天得到的分数，作为返回值
 */
int Croominfo_rally :: get_today_score(userid_t userid, uint32_t *p_day)
{
	sprintf(this->sqlstr, "select day_score from %s where userid=%u",
			this->get_table_name(userid),
			userid
		   );

 	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
         INT_CPY_NEXT_FIELD(*p_day);
     STD_QUERY_ONE_END();
}

/* @brief 增加用户的分数，并且返回是否有四个赛道都跑过的标志 
 * @param userid 用户的米米号 
 * @param add_score 增加的分数
 * @param race_pos 参加的哪个赛道
 * @param p_finish 返回是否四个赛道都跑完的标志
 */
int Croominfo_rally :: add_score(userid_t userid, uint32_t add_score, uint32_t race_pos, uint32_t *p_finish)
{
	uint32_t ret = this->update_score_race(userid, add_score, race_pos);
	if (ret != SUCC) {
		return ret;
	}
	uint32_t race1 = 0;
	uint32_t race2 = 0;
	uint32_t race3 = 0;
	uint32_t race4 = 0;
	ret = get_races(userid, &race1, &race2, &race3, &race4);
	if (ret != SUCC) {
		return ret;
	}
	if ((race1 > 0) && (race2 > 0) && (race3 > 0) && (race4 > 0)) {
		*p_finish = 1;
	} else {
		*p_finish = 0;
	}
	return SUCC;
}

/* @breif 得到四个赛道分别参加的次数  
 * @param userid 用户的米米号
 * @param p_race1 第一赛道参赛的次数，作为返回值
 * @param p_race2 第二赛道参赛的次数，作为返回值
 * @param p_race3 第三赛道参赛的次数，作为返回值
 * @param p_race4 第四赛道参赛的次数，作为返回值
 */
int Croominfo_rally :: get_races(userid_t userid, uint32_t *p_race1, uint32_t *p_race2,
		               uint32_t *p_race3, uint32_t *p_race4)
{
	sprintf(this->sqlstr, "select race1, race2, race3, race4 from %s where userid=%u",
			this->get_table_name(userid),
			userid
		   );

 	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
         INT_CPY_NEXT_FIELD(*p_race1);
         INT_CPY_NEXT_FIELD(*p_race2);
         INT_CPY_NEXT_FIELD(*p_race3);
         INT_CPY_NEXT_FIELD(*p_race4);
     STD_QUERY_ONE_END();
}

/* @brief 使各个赛道的次数减一
 * @param userid 用户的米米号
 */
int Croominfo_rally :: change_race(userid_t userid)
{
	uint32_t race1 = 0;
	uint32_t race2 = 0;
	uint32_t race3 = 0;
	uint32_t race4 = 0;
	uint32_t ret = this->get_races(userid, &race1, &race2, &race3, &race4);
	if (ret != SUCC) {
		return ret;
	}
	/* 判断是否四个赛道的次数都是大于零 */
	if ((race1==0) || (race2 == 0) || (race3 == 0) || (race4 == 0)) {
		return YOU_HAVE_NOT_FISH_ALL_RACE_ERR;
	}
	ret = this->update_race(userid);
	return ret;
}

/* @brief 使各个赛道的次数减一 
 * @param userid 用户的米米号
 */
int Croominfo_rally :: update_race(userid_t userid)
{
	sprintf(this->sqlstr, "update %s set race1 = race1 - 1, race2 = race2 -1,\
						   race3 = race3 - 1, race4 = race4 - 1 where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}


/* @brief 得到报名的对和徽章的数目 
 * @param userid 用户的米米号
 * @param p_team 报的队名
 * @param p_medal 徽章数目
 */
int Croominfo_rally :: get_team_medal(userid_t userid, uint32_t *p_score, uint32_t *p_team, uint32_t *p_medal)
{
	sprintf(this->sqlstr, "select score, team, day_medal from %s where userid=%u",
			this->get_table_name(userid),
			userid
		   );

 	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
         INT_CPY_NEXT_FIELD(*p_score);
         INT_CPY_NEXT_FIELD(*p_team);
         INT_CPY_NEXT_FIELD(*p_medal);
     STD_QUERY_ONE_END();
}

int Croominfo_rally :: get_all_info(userid_t userid, roominfo_rally_get_user_all_info_web_out *p_out)
{
	sprintf(this->sqlstr, "select team, score, race1, race2, race3, race4, day_score from %s where userid=%u",
			this->get_table_name(userid),
			userid
		   );

 	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
         INT_CPY_NEXT_FIELD(p_out->team);
         INT_CPY_NEXT_FIELD(p_out->score);
         INT_CPY_NEXT_FIELD(p_out->race1);
         INT_CPY_NEXT_FIELD(p_out->race2);
         INT_CPY_NEXT_FIELD(p_out->race3);
         INT_CPY_NEXT_FIELD(p_out->race4);
         INT_CPY_NEXT_FIELD(p_out->day_score);
     STD_QUERY_ONE_END();
}


