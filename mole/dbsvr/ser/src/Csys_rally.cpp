/*
 * =====================================================================================
 *
 *       Filename:  Csys_arg.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include "Csys_rally.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
#include <algorithm>

/* @brief 构造函数
 * @param db DB的类指针
 */
Csys_rally::Csys_rally(mysql_interface * db ) : Ctable( db, "SYSARG_DB","t_sysarg_rally")
{ 

}
/* @brief 插入队的记录 
 * @team 插入队记录的ID号 
 */
int Csys_rally :: insert(uint32_t team)
{
	sprintf(this->sqlstr, "insert into %s values(%u, 0, 0, 0, 0, 0, 0)",
			this->get_table_name(),
			team
		   );
	STD_SET_RETURN_EX(this->sqlstr, SUCC);	
}

/* @brief 更新每个队的分数和当天获取的分数 
 * @param team 组的ID号
 * @param value 要增加的值
 */
int Csys_rally :: update_add(uint32_t team, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set score = score + %u, day_score = day_score + %u\
			 where team = %u ",
			this->get_table_name(),
			value,
			value,
			team
		   );
	STD_SET_RETURN_EX(this->sqlstr, KEY_NOFIND_ERR);	
}
 
/* @brief 更新每个赛道的最短时间 
 * @param team 组的ID号
 * @param index 哪个赛道（1~4表示）
 * @param value 要更新的值
 */
int Csys_rally :: update_race(uint32_t team, uint32_t index, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set race%u = if(((race%u < %u) && (race%u != 0)), race%u, %u)\
			 where team = %u ",
			this->get_table_name(),
			index,
			index,
			value,
			index,
			index,
			value,
			team
		   );
	STD_SET_RETURN_EX(this->sqlstr, KEY_NOFIND_ERR);	
}

/* @brief 得到各个对的各个赛道的最短时间值
 * @param pp_list 各个对的时间值返回值
 * @param p_count 对的数目
 */
int Csys_rally :: get_list_race(sys_get_team_race_list_out_item **pp_list, uint32_t *p_count )
{
	sprintf( this->sqlstr, " select team, race1, race2, race3, race4 from %s",
			this->get_table_name()
			);   

	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		(*pp_list+i)->team = atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->race1 = atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->race2 = atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->race3 = atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->race4 = atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}

/* @brief 得到各个对的分数 
 * @param pp_list 返回各个对的分数
 * @param p_count 返回队的个数
 */
int Csys_rally :: get_list_score(sys_get_team_score_list_out_item **pp_list, uint32_t *p_count )
{
	sprintf( this->sqlstr, " select team, score, day_score from %s",
			this->get_table_name()
			);   

	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		(*pp_list+i)->team = atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->score = atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->day_score = atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}

