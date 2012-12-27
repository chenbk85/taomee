/*
 * =====================================================================================
 *
 *       Filename:  Cuser_lamu_classroom_remembrance.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/07/2010 10:22:54 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_lamu_classroom_remembrance.h"
#include "common.h"
#include <msglog.h>

/* 
 * 计算学生去向，一维：去向数量
 * 二维：依次对应为：去向id、德、智、体、美、劳、考评、(课程id、分数 ..........)
 */
const uint32_t g_student_direct[][13] = {
	{1,600,900,0,0,500,4,3,1000},
	{2,800,800,0,0,500,4,3,800},
	{3,600,800,0,0,500,4,4,800},
	{4,0,900,0,0,500,4,4,950},
	{5,900,0,0,500,0,4,1,1000},
	{6,800,800,0,400,0,4,1,800},
	{7,0,900,500,0,600,4,5,1000},
	{8,0,800,600,0,600,4,2,850},
	{9,800,0,500,0,500,4,2,950},
	{11,0,300,0,0,300,4,3,500},
	{12,400,0,500,0,0,4,1,600,13,500},
	{13,0,0,300,0,300,4,4,400},
	{14,400,0,400,0,500,4,1,500,4,600},
	{15,300,300,0,200,0,4,1,500},
	{16,500,0,600,0,600,3,13,900},
	{17,0,0,300,0,300,4,5,400},
	{18,0,400,0,0,500,4,3,600,5,600},
	{19,300,300,0,0,200,4,2,500},
	{20,400,500,0,0,500,4,1,600,2,600},
	{21,0,900,0,0,900,3,15,900},
	{22,500,600,0,0,500,4,15,600},
	{23,600,0,0,0,600,4,16,600},
	{24,800,0,0,0,800,3,16,800,13,800},
	{25,900,900,0,0,900,2,1,900,16,900},
};

Cuser_lamu_classroom_remembrance::Cuser_lamu_classroom_remembrance(mysql_interface * db)
	:CtableRoute100x10(db, "USER", "t_user_lamu_classroom_remembrance", "userid")
{
	this->msglog_file = config_get_strval("MSG_LOG_FILE");
}

/*
 * @brief 添加毕业去向
 */
int Cuser_lamu_classroom_remembrance::insert(userid_t userid, uint32_t remem_id, uint32_t remem_cnt, uint32_t got_time)
{
	sprintf( this->sqlstr, "insert into %s (userid, \
						remem_id, \
						remem_cnt, \
						got_time) \
   						values(%u, %u, %u, %u)",
					this->get_table_name(userid),
					userid,
					remem_id,
					remem_cnt,
					got_time
					);
	
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief 查看毕业去向时拉取所有
 */
int Cuser_lamu_classroom_remembrance::get_remembrances(userid_t userid, 
		user_lamu_classroom_get_remembrance_out_item **pp_list, uint32_t *pcount)
{
	sprintf( this->sqlstr, "select remem_id, got_time, remem_cnt from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, pcount);
		INT_CPY_NEXT_FIELD((*pp_list + i)->remem_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->got_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->remem_cnt);
	STD_QUERY_WHILE_END();
}

/*
 * @brief 修改毕业去向的人数
 */
int Cuser_lamu_classroom_remembrance::update_remem_cnt(userid_t userid, uint32_t remem_id, uint32_t remem_cnt)
{
	sprintf(this->sqlstr, "update %s set remem_cnt = remem_cnt + %u where userid = %u and remem_id = %u",
					this->get_table_name(userid),
					remem_cnt,
					userid,
					remem_id
					);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief 计算每个学生的毕业去向，得到优秀毕业生人数
 */
int Cuser_lamu_classroom_remembrance::cal_student_graduate(userid_t userid,
		user_lamu_classroom_graduate_direction_info &graduate_info,
		user_lamu_classroom_get_lamus_info_out_item *p_lamu_list, uint32_t lamu_cnt,
		user_lamu_classroom_get_remembrance_out_item *p_remem_list, uint32_t remem_cnt,
		user_lamu_classroom_get_exam_info_out_item *p_score_list,
		uint32_t &great_cnt)
{
	for (uint32_t i = 0; i < lamu_cnt; ++i) {
		uint32_t direct_id = cal_student_direct(userid, p_lamu_list + i, p_score_list, lamu_cnt);//计算去向
		if (direct_id != 10) {
		   	++great_cnt;
		}

		uint32_t j;
		for (j = 0; j < graduate_info.count; j++) {//判断direct_id是否在graduate_info中，在累计，不在添加
			if (direct_id == graduate_info.direct_info[j].direct_id) {
				break;
			}
		}
		if (j == graduate_info.count) {
			graduate_info.direct_info[j].direct_id = direct_id;
			graduate_info.direct_info[j].direct_cnt = 1;
			++(graduate_info.count);

			uint32_t k = 0;
			for (; k < remem_cnt; ++k) { //判断是否为是新的去向
				if ((p_remem_list + k)->remem_id == graduate_info.direct_info[j].direct_id) {
					break;
				}
			}
			if (k == remem_cnt) {
				graduate_info.direct_info[j].is_new = 1;
			} else {
				graduate_info.direct_info[j].is_new = 0;
			}
		} else {
			++(graduate_info.direct_info[j].direct_cnt);
		}
	}

	uint32_t buff[2] = {userid, lamu_cnt};
	msglog(this->msglog_file, 0x02102040, time(NULL), buff, sizeof(buff));
	buff[1] = great_cnt;
	msglog(this->msglog_file, 0x02102041, time(NULL), buff, sizeof(buff));

	for (uint32_t i = 0; i < graduate_info.count; ++i) {
		buff[1] = graduate_info.direct_info[i].direct_cnt;
		msglog(this->msglog_file, 0x02103100 + graduate_info.direct_info[i].direct_id, 
								time(NULL), buff, sizeof(buff));
	}
	return SUCC;
}

/*
 * @breif 计算毕业去向
 */
uint32_t Cuser_lamu_classroom_remembrance::cal_student_direct(userid_t userid, 
		user_lamu_classroom_get_lamus_info_out_item *p_lamu, 
		user_lamu_classroom_get_exam_info_out_item *p_score_list, uint32_t lamu_cnt)
{
	uint32_t direct_id = 10;
	uint32_t direct_cnt = 0;
	uint32_t direct_array[20];
	uint32_t *p_value = (uint32_t*)&(p_lamu->lamu_info.moral);
	//计算可行的毕业去向
	for (uint32_t i = 0; i < sizeof(g_student_direct) / sizeof(g_student_direct[0]); ++i) {
		uint32_t flag = 1;
		uint32_t j = 0;
		for (; j < 5; j++) {//5项属性比较
			if (p_value[j] < g_student_direct[i][j+1]) {
				flag = 0;
				break;
			}
		}
		
		for (j = 0; j < lamu_cnt && flag == 1; ++j) {
			if (p_lamu->lamu_info.lamu_id == (p_score_list + j)->lamu_id) {//找到学生
				if ((p_score_list + j)->score > g_student_direct[i][6]) { //学生的考评不合格
					flag = 0;
					break;
				}
				//比较配置的分数
				for (uint32_t k = 7; k < sizeof(g_student_direct[0]) / sizeof(g_student_direct[0][0]) && flag == 1; k += 2) {
					if (g_student_direct[i][k] == 0) {//k后面没有配课程
						break;
					}
					uint32_t b_find = 0;
					for (uint32_t l = 0; l < (p_score_list + j)->score_list.count; ++l) {
						//毕业成绩中找到该课程
						if (g_student_direct[i][k] == (p_score_list + j)->score_list.course_score[l].course_id) {
							b_find = 1;
							//比较分数
							if ((p_score_list + j)->score_list.course_score[l].score < g_student_direct[i][k+1]) {
								flag = 0;
								break;
							}
						}
					}
					if (b_find == 0) {
						flag = 0;
					}
				}
				break;
			}
		}
		if (flag == 1) {
			direct_array[direct_cnt++] = i;
		}
	}
	//DEBUG_LOG("----alexhe:graduate direct_cnt = %u, direct_id = %u----", direct_cnt, direct_id);
	if (direct_cnt != 0) {//如果存在毕业去向，随机生成其中的一种
		uint32_t index = direct_array[rand() % direct_cnt];
		direct_id = g_student_direct[index][0];
	}
	return direct_id;
}
