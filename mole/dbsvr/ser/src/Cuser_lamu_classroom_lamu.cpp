/*
 * =====================================================================================
 *
 *       Filename:  Cuser_lamu_classroom_lamu.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/02/2010 11:22:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_lamu_classroom_lamu.h"
#include "common.h"

//心情的影响系数
const double g_lamu_mood[] = {2, 1.5, 1, 0.5, 0};
const double g_exam_fac[] = {4, 2.3, 1.12};//3次考试的系数
const double COURSE_HANDLE_FAC = 10;//课程掌握情况系数
const double ATTR_FAC_1 = 0.7;		//属性1的系数
const double ATTR_FAC_2 = 0.3;		//属性2的系数
const double EXAM_FAC_1 = 0.8;		//考试成绩中课程掌握情况比重
const double EXAM_FAC_2 = 0.2;		//考试成绩中属性比重
const uint32_t MAX_SCORE = 1000;	
const uint32_t MIN_SCORE = 250;		
const uint32_t MID_SCORE = 500;
/*
 * 不同性格下课后属性的变化量
 * 性格依次对应为马虎、勇敢、悠闲、淘气、胆小、急躁、认真、开朗、敏感、稳重
 * 改变量依次对应为潜力、德智体美劳
 */
const uint32_t lamu_change_val[][6] = {
	{60, 7, 0, 10, 0, 8},
	{100, 0, 0, 20, 0, 0},
	{60, 5, 0, 0, 10, 10},
	{60, 0, 10, 10, 0, 5},
	{120, 5, 5, 0, 5, 0},
	{100, 0, 0, 0, 0, 20},
	{100, 0, 20, 0, 0, 0},
	{60, 5, 5, 5, 5, 5},
	{100, 0, 0, 0, 20, 0},
	{100, 20, 0, 0, 0, 0}
};

//一维为考试数目3\4\5\6,二维为等级，exam_sum_score[i][j]表示有i+3门课程考试时，得到j+1等级的最小总分
const uint32_t exam_sum_score[][4] = {
	{2900, 2700, 2100, 1500},
	{3850, 3400, 2800, 1900},
	{4650, 4200, 3500, 2300},
	{5400, 4800, 4200, 2400}
};
#define MAX_EXAM_COURSE_CNT (3+sizeof(exam_sum_score)/sizeof(exam_sum_score[0]))

//返回值1/2/3/4/5对应S/A/B/C/D
inline uint32_t get_lamu_score_level(uint32_t score, uint32_t count)
{
	if(count < 3 || count > MAX_EXAM_COURSE_CNT){
		return 0;
	}
	uint32_t index = 0;
	for (; index < 4; ++index){
		if (score >= exam_sum_score[count - 3][index]) {
			break;
		}
	}
	return index + 1;
}

//精力和亲密度的影响因子
#define FAC_VAL(val) ((val) > 900 ? 1.0 : (1.0 * ((val) + 100) / 1000))
//活动后心情对潜力的影响系数
#define LATENT_CHANGE_VAL(mood) ((uint32_t)(5 - 2 * g_lamu_mood[(mood)]))
//#define MAX_VAL(val, _v) ((val) < (_v) ? (val) : (_v))
//#define MIN_VAL(val, _v) ((val) < (_v) ? 0 : (val))
//确定val在_a和_b之间，如果小于_a为0，大于_b为_b
//#define BETWEEN_VAL(val, _a, _b) MAX_VAL(MIN_VAL((val), (_a)), (_b)) 
//随机值val在_v范围内，即_v%几率
#define VAL_IN_RANDOM(val) (((val) % 100) < 30)
//学生数量对课程掌握情况的影响
#define STUDENT_CNT_AFFECT(cnt) (((cnt)>2)?(2*((cnt)-2)):0)
//德育与考试次数对考试的影响
#define MORAL_EXAM_AFFECT(val) ((val) * 15 + 15) 
/* 
 * @brief 类构造函数
 * @param db 数据库句柄
 */
Cuser_lamu_classroom_lamu::Cuser_lamu_classroom_lamu(mysql_interface * db) :
	CtableRoute100x10(db, "USER", "t_user_lamu_classroom_lamu", "userid")
{
}

/*
 * @breif 表中添加一列拉姆的值
 * @param lamu_id:
 */
int Cuser_lamu_classroom_lamu::add_lamu(userid_t userid, user_lamu_classroom_add_lamus_in_item *p_item, uint32_t lamu_id)
{
	/*
	char course_list_mysql[mysql_str_len(sizeof (*p_course_list) )];
    set_mysql_string(course_list_mysql, (char*)(&(p_course_list->count)),
			            p_course_list->count * (sizeof(p_course_list->course_score[0])) + 4);*/
	sprintf( this->sqlstr, "insert into %s (userid, \
						lamu_id, \
		   				name_id, \
						vip_flag, \
						latent, \
						nature, \
						mood, \
						moral, \
						intelligence, \
						sport, \
						art, \
						labor, \
						level, \
						color, \
						skill,\
						course_info) \
						values(%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, 0x00000000) ",
					this->get_table_name(userid), 
					userid, 
					lamu_id, 
					p_item->name_id,
		   			p_item->vip_flag,	
					p_item->latent, 
					p_item->nature, 
					p_item->mood, 
					p_item->moral,
					p_item->intelligence, 
					p_item->sport, 
					p_item->art, 
					p_item->labor,
					p_item->level,
					p_item->color,
					p_item->skill
					);

	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief 毕业或结业后，删除拉姆信息
 */
int Cuser_lamu_classroom_lamu::delete_lamu(userid_t userid)
{
	sprintf( this->sqlstr, "delete from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);
	STD_SET_RETURN_WITH_NO_AFFECTED(this->sqlstr);
}

/*
 * @brief 查看学生档案时，拉取学生信息
 */
int Cuser_lamu_classroom_lamu::get_lamus_info(userid_t userid, 
		user_lamu_classroom_get_lamus_info_out_item **pp_list, uint32_t *pcount)
{
	sprintf( this->sqlstr, "select \
			    		lamu_id, \
						name_id, \
						vip_flag, \
						latent, \
						nature, \
						mood, \
						moral, \
						intelligence, \
						sport, \
						art, \
						labor, \
						level, \
						color, \
						skill, \
						course_info \
						from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, pcount);
	    INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_info.lamu_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_info.name_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_info.vip_flag);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_info.latent);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_info.nature);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_info.mood);
		(*pp_list + i)->lamu_info.mood %= 5;
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_info.moral);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_info.intelligence);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_info.sport);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_info.art);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_info.labor);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_info.level);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_info.color);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_info.skill);
		BIN_CPY_NEXT_FIELD(&((*pp_list + i)->course_list), sizeof((*pp_list + i)->course_list));
	STD_QUERY_WHILE_END();
}

/*
 * @brief 进教室时拉取拉姆的名称、颜色、阶段、技能
 */
int Cuser_lamu_classroom_lamu::get_lamu_info_ex(userid_t userid, 
		user_lamu_classroom_get_class_info_out_item **pp_list, uint32_t *pcount)
{
	sprintf( this->sqlstr, "select lamu_id, \
						name_id, \
						level, \
						color, \
						skill, \
						vip_flag \
						from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, pcount);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->name_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->level);
		INT_CPY_NEXT_FIELD((*pp_list + i)->color);
		INT_CPY_NEXT_FIELD((*pp_list + i)->skill);
		INT_CPY_NEXT_FIELD((*pp_list + i)->vip_flag);
	STD_QUERY_WHILE_END();
}

/*
 * @brief 添加拉姆可以学习的课程
 */
/*
int Cuser_lamu_classroom_lamu::update_course(userid_t userid, uint32_t course_id)
{
	//得到拉姆的课程列表
	uint32_t count = 0;
	lamu_courses_list *p_list = NULL;
	uint32_t index;//如果没有出错，则返回的是course_id的位置index	
	int ret = this->is_courseid_in_list(userid, course_id, &p_list, &count, &index);

	if (ret != SUCC) {
		if (p_list != NULL) {
			free(p_list);
			p_list = NULL;
		}
		return ret;
	}	

	int course_count = p_list[0].course_list.count;
	for(uint32_t i = 0; i < count; ++i) {
		if (index == 0) { 	//添加新的课程
			p_list[i].course_list.count += 1;
			p_list[i].course_list.course_score[course_count].course_id = course_id;
		} else { 			//修改课程掌握情况
			p_list[i].course_list.course_score[ret - 1].score += 0;
		}
		ret = this->set_one_lamu_course_info(userid, p_list[i].lamu_id, p_list[i].course_list);
		if (ret != SUCC) {
			break;
		}
	}
	free(p_list);
	p_list = NULL;

	return ret;
}
*/

/*
 * @brief 下课或考试时，得到相关数据计算各属性的变化
 */
int Cuser_lamu_classroom_lamu::get_class_end_or_exam_info(userid_t userid, lamu_courses_list **pp_list, uint32_t *pcount)
{
	sprintf( this->sqlstr, "select lamu_id, nature, mood, latent, moral, intelligence, \
						sport, art, labor, course_info from %s where userid = %u", 
					this->get_table_name(userid),
					userid
					);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, pcount);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->nature);
		INT_CPY_NEXT_FIELD((*pp_list + i)->mood);
		INT_CPY_NEXT_FIELD((*pp_list + i)->latent);
		INT_CPY_NEXT_FIELD((*pp_list + i)->moral);
		INT_CPY_NEXT_FIELD((*pp_list + i)->intelligence);
		INT_CPY_NEXT_FIELD((*pp_list + i)->sport);
		INT_CPY_NEXT_FIELD((*pp_list + i)->art);
		INT_CPY_NEXT_FIELD((*pp_list + i)->labor);
		BIN_CPY_NEXT_FIELD(&((*pp_list + i)->course_list), sizeof(user_lamu_course_list));
	STD_QUERY_WHILE_END_WITHOUT_RETURN();

	if (*pcount == 0 || *pcount > 10) {
		return USER_LAMU_CLASSROOM_STUDENT_NUM_ERR;
	}
	return SUCC;
}

/*
 * @brief 修改单个lamu的课程掌握情况
 */
int Cuser_lamu_classroom_lamu::set_one_lamu_course_info(userid_t userid, uint32_t lamu_id, user_lamu_course_list lamu_course)
{
	char course_list_mysql[mysql_str_len(sizeof (user_lamu_course_list) )];
	set_mysql_string(course_list_mysql, (char*)(&(lamu_course.count)),
			lamu_course.count * (sizeof(lamu_course.course_score[0])) + 4);
	
	sprintf( this->sqlstr, "update %s set course_info = '%s' where userid = %u and lamu_id = %u",
					this->get_table_name(userid),
					course_list_mysql,
					userid,
					lamu_id
					);

	STD_SET_RETURN_EX( this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 判断学生是否有学习过course_id这门课
 */
int Cuser_lamu_classroom_lamu::is_courseid_in_course_list(uint32_t course_id, user_lamu_course_list course_list)
{
	int index = 0;
	for (uint32_t i = 0; i < course_list.count; ++i) {
		if (course_list.course_score[i].course_id == course_id) {
			index = i + 1;
			break;
		}
	}
	return index;
}

/*
 * @brief 一节课下后，修改所有拉姆的各项属性
 */
int Cuser_lamu_classroom_lamu::update_lamus_when_class_end(userid_t userid, 
		user_lamu_classroom_class_end_in *p_in, uint32_t energy, uint32_t lovely, uint32_t exam_times)
{
	lamu_courses_list *p_list = NULL;
	uint32_t lamu_cnt;
	int ret = this->get_class_end_or_exam_info(userid, &p_list, &lamu_cnt);

	if (ret != SUCC) {
		if (p_list != NULL) {
			free(p_list);
			p_list = NULL;
		}
		return ret;
	}

	user_lamu_course_list temp_course = {0};				//拉姆的所有课程掌握情况
	lamu_values_stru lamu_stru = {0};						//lamu自身的属性值
	uint32_t *p_in_attr = (uint32_t*)&(p_in->latent);//传入配置的属性值

 	for (uint32_t i = 0; i < lamu_cnt; ++i) {
		//课程掌握情况列表
		memcpy((char*)&temp_course, (char*)&((p_list + i)->course_list), sizeof(user_lamu_course_list));
		//lamu自身属性
		memcpy((char*)&lamu_stru, (char*)&(p_list + i)->latent, sizeof(lamu_stru));
		uint32_t tmp_nature = (p_list + i)->nature;
		uint32_t tmp_mood = (p_list + i)->mood;

		if (p_in->class_type == 1) {
			uint32_t *attr_values = (uint32_t*)&((p_list + i)->moral);//拉姆自身的属性值
			//课程掌握程度
			uint32_t course_score = cal_course_score_class_end(p_in->course_score,
				   	attr_values, p_in->attr_index, energy, exam_times, (p_list + i)->mood, lamu_cnt);

			int32_t index;	//返回course_id的位置index，如果index = 0, 则表示之前没有该课程
			index = this->is_courseid_in_course_list(p_in->course_id, temp_course);
			if (index == 0) {//如果是新学习的课程
				temp_course.course_score[temp_course.count].course_id = p_in->course_id;
				temp_course.course_score[temp_course.count].score = course_score;
				temp_course.count += 1;
			} else {
				temp_course.course_score[index - 1].score += course_score;
				temp_course.course_score[index - 1].score = BETWEEN_VAL(temp_course.course_score[index - 1].score, 0, MAX_SCORE);
			}
			//计算下课后属性变化
			this->cal_lamus_values(&lamu_stru, p_in_attr, energy, tmp_nature, tmp_mood);
		} else {
			//计算活动后属性变化
			this->cal_lamus_values_ex(&lamu_stru, p_in_attr, lovely, tmp_nature, tmp_mood);
		}

		ret = this->update_one_lamu_when_class_end(userid, (p_list + i)->lamu_id, lamu_stru, temp_course);
		memset(&temp_course, 0, sizeof(temp_course));
		memset(&lamu_stru, 0, sizeof(lamu_stru));
		if (ret != SUCC) {
			break;
		}
	}
	if (p_list != NULL) {
		free(p_list);
		p_list = NULL;
	}

	return ret;
}

/*
 * @brief 一节课下后，修改一个拉姆的属性
 */
int Cuser_lamu_classroom_lamu::update_one_lamu_when_class_end(userid_t userid, 
		uint32_t lamu_id, lamu_values_stru lamu_stru, user_lamu_course_list course_blob)
{
	char course_list_mysql[mysql_str_len(sizeof (user_lamu_course_list) )];
	set_mysql_string(course_list_mysql, (char*)(&(course_blob.count)),
			course_blob.count * (sizeof(course_blob.course_score[0])) + 4);
	
	sprintf( this->sqlstr, "update %s set \
						latent = %u,\
						moral = %u,\
						intelligence = %u,\
						sport = %u,\
						art = %u,\
						labor = %u,\
						mood = %u, \
						course_info = '%s' \
						where userid = %u and lamu_id = %u",
					this->get_table_name(userid),
					lamu_stru.latent,
					lamu_stru.moral,
					lamu_stru.intelligence,
					lamu_stru.sport,
					lamu_stru.art,
					lamu_stru.labor,
					lamu_stru.mood,
					course_list_mysql,
					userid,
					lamu_id
		   			);

    STD_SET_RETURN_EX( this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 得到本学期学生的人数
 * @param flag: 0表示得到的是普通拉姆的数，1表示超拉，2表示总数
 */
int Cuser_lamu_classroom_lamu::get_lamus_count(userid_t userid, uint32_t *count, uint32_t flag)
{
	if (flag == 2) {
		sprintf( this->sqlstr, "select count(*) from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);
	} else {
		sprintf( this->sqlstr, "select count(*) from %s where userid = %u and vip_flag = %u",
					this->get_table_name(userid),
					userid,
					flag
					);
	}

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();
}

/*
 * @brief 得到拉姆与老师的亲密度和心情值，满足一定条件可以安排一次补课
 */
int Cuser_lamu_classroom_lamu::get_lovely_mood(userid_t userid)
{
	//todo
	return 0;
}


/*
 * @brief 下课后，计算lamu的属性变化的计算公式
*/
int Cuser_lamu_classroom_lamu::cal_lamus_values(lamu_values_stru *p_lamu_stru, 
		uint32_t *p_in_attr, uint32_t energy, uint32_t nature, uint32_t mood)
{
	int32_t *p_values = (int32_t*)&p_lamu_stru->latent;
	//DEBUG_LOG("~~~~~lamus_value:%u,%u,%u,%u,%u,%u,%u~~~~~", energy, *(p_values), *(p_values + 1), *(p_values + 2), 
	//		*(p_values + 3), *(p_values + 4), *(p_values + 5));
	//防止性格和心情越界
	nature %= (sizeof(lamu_change_val) / sizeof(lamu_change_val[0]));
	mood %= (sizeof(g_lamu_mood) / sizeof(g_lamu_mood[0]));
	
	for (uint32_t i = 0; i < sizeof(lamu_change_val[0]) / sizeof(lamu_change_val[0][0]); ++i) {
		uint32_t change_val = (uint32_t)(lamu_change_val[nature][i] * g_lamu_mood[mood]);//性格和心情的影响
		int32_t tmp_val = *(p_in_attr + i);

		tmp_val = (int32_t)(FAC_VAL(energy) * FAC_VAL(energy) * tmp_val) + change_val;
		*(p_values + i) += tmp_val;
		*(p_values + i) = BETWEEN_VAL(*(p_values + i), 0, MAX_SCORE);
	}
	
	p_lamu_stru->mood = rand() % 5;
	//DEBUG_LOG("----lamus_value:%u,%u,%u,%u,%u,%u----", *(p_values), *(p_values + 1), *(p_values + 2), 
	//		*(p_values + 3), *(p_values + 4), *(p_values + 5));
	return SUCC;
}

/*
 * @brief 活动后，计算lamu属性变化的计算公式
 */
int Cuser_lamu_classroom_lamu::cal_lamus_values_ex(lamu_values_stru *p_lamu_stru, 
		uint32_t *p_in_attr, uint32_t lovely, uint32_t nature, uint32_t mood)
{
	int32_t *p_values = (int32_t*)&p_lamu_stru->latent;
	uint32_t latent = p_lamu_stru->latent;
	//防止性格和心情越界
	nature %= (sizeof(lamu_change_val) / (sizeof(lamu_change_val[0])));
	mood %= (sizeof(g_lamu_mood) / sizeof(g_lamu_mood[0]));
	for (uint32_t i = 1; i < 6; ++i) {
		uint32_t change_val = (uint32_t)(lamu_change_val[nature][i] * g_lamu_mood[mood]);//性格和心情的影响
		uint32_t tmp_val = (uint32_t)(FAC_VAL(lovely) * FAC_VAL(lovely) * (*(p_in_attr + i))) + change_val;//属性改变值
		
		if (latent >= tmp_val * LATENT_CHANGE_VAL(mood)) {
			if (*(p_values + i) + tmp_val >= 1000) {//如果相加已经大于最大值1000，避免潜力值浪费
				latent -= (1000 - *(p_values + i)) * LATENT_CHANGE_VAL(mood);
				*(p_values + i) = 1000;
			} else{
				latent = latent - tmp_val * LATENT_CHANGE_VAL(mood);
				*(p_values + i) += tmp_val;
			}
		} else if (latent >= 0) {
			uint32_t tmp_val = latent / LATENT_CHANGE_VAL(mood);
			*(p_values + i) += tmp_val;
			*(p_values + i) = BETWEEN_VAL(*(p_values + i), 0, MAX_SCORE);
			latent = latent - tmp_val * LATENT_CHANGE_VAL(mood);
			break;
		} else {
			break;
		}
		*(p_values + i) = BETWEEN_VAL(*(p_values + i), 0, MAX_SCORE);
	}
	p_lamu_stru->latent = BETWEEN_VAL(latent, 0, MAX_SCORE);
	p_lamu_stru->mood = rand() % 5;
	//DEBUG_LOG("----lamus_value: %u,%u,%u,%u,%u,%u----", *(p_values), *(p_values + 1), *(p_values + 2), 
	//		*(p_values + 3), *(p_values + 4), *(p_values + 5));
	return SUCC;
}

/*
 * @brief 一节课下后，计算课程掌握情况的改变量的计算公式
 */
uint32_t Cuser_lamu_classroom_lamu::cal_course_score_class_end(uint32_t conf_val,
	   	uint32_t *attr_vals, uint32_t attr[], uint32_t energy, uint32_t exam_times, uint32_t mood, uint32_t lamu_cnt)
{
	double change_val = 0;
	uint32_t attr_a = 0;
	uint32_t attr_b = 0;
	if (attr[0] != 0) {
		attr_a = attr_vals[attr[0] - 1];
		if (attr[1] != 0) {
			attr_b = attr_vals[attr[1] - 1];
		}
	}
	switch (exam_times) {
	case 0:
		attr_a = MAX_VAL(attr_a, MIN_SCORE);
		attr_b = MAX_VAL(attr_b, MIN_SCORE);
		break;
	case 1:
		attr_a = BETWEEN_VAL(attr_a, MIN_SCORE, MID_SCORE);
		attr_b = BETWEEN_VAL(attr_b, MIN_SCORE, MID_SCORE);
		break;
	case 2:
		attr_a = BETWEEN_VAL(attr_a, MID_SCORE, MAX_SCORE);
		attr_b = BETWEEN_VAL(attr_b, MID_SCORE, MAX_SCORE);
		break;
	}
	change_val = conf_val + attr_a * ATTR_FAC_1 + attr_b * ATTR_FAC_2;
	change_val = change_val * FAC_VAL(energy) * FAC_VAL(energy) 
		+ g_lamu_mood[mood] * COURSE_HANDLE_FAC + STUDENT_CNT_AFFECT(lamu_cnt);
	return (uint32_t)BETWEEN_VAL(change_val, 0, MAX_SCORE);
}

/*
 * @brief 考试结束后得到lamu的基本信息
 */
int Cuser_lamu_classroom_lamu::get_lamu_show_info(userid_t userid, user_lamu_classroom_get_exam_info_out_item *p_item)
{
	sprintf( this->sqlstr, "select name_id, \
						nature, \
						level, \
						color, \
						skill \
						from %s where userid = %u and lamu_id = %u",
					this->get_table_name(userid),
					userid,
					p_item->lamu_id
					);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR );
		INT_CPY_NEXT_FIELD(p_item->name_id);
		INT_CPY_NEXT_FIELD(p_item->nature);
		INT_CPY_NEXT_FIELD(p_item->level);
		INT_CPY_NEXT_FIELD(p_item->color);
		INT_CPY_NEXT_FIELD(p_item->skill);
	STD_QUERY_ONE_END();
}

/*
 * 考试时，计算学生每门课得分
 */
int Cuser_lamu_classroom_lamu::cal_exam_values(user_lamu_classroom_score_info *p_score_info, user_lamu_about_exam_list *p_list,
		user_lamu_classroom_begin_exam_in_item *p_item, uint32_t lovely, uint32_t exam_times)
{
	user_lamu_course_list tmp_course = {0};        //拉姆的课程掌握情况
	memcpy((char*)&tmp_course, (char *)&((p_list)->course_list), sizeof(user_lamu_course_list));
	uint32_t score_sum = 0;
	for (uint32_t i = 0; i < p_score_info->course_list.count; ++i) {
		uint32_t j = 0;
		for (; j < tmp_course.count; ++j) {
			if ((p_item + i)->course_id == tmp_course.course_score[j].course_id) {
				break;
			}
		}
		uint32_t tmp_score = (j >= tmp_course.count) ? 0 : tmp_course.course_score[j].score;
		uint32_t *p_values = (uint32_t*)&(p_list->moral);
		p_score_info->course_list.course_score[i].score = 
				this->cal_lamu_score(tmp_score, p_values, (p_item + i)->attr_index, lovely, exam_times) / 10 * 10;
		//DEBUG_LOG("----alexhe:exam score:%u,%u----", (p_item + i)->course_id, p_score_info->course_list.course_score[i].score);
		p_score_info->course_list.course_score[i].course_id = (p_item + i)->course_id;
		score_sum += p_score_info->course_list.course_score[i].score;
	}
	p_score_info->exam_times = exam_times + 1;
	p_score_info->lamu_id = p_list->lamu_id;
	//如果德育<(考试次数*20+10)时，有30%几率作弊
	if (p_list->moral < MORAL_EXAM_AFFECT(exam_times) && VAL_IN_RANDOM(rand())) {
		p_score_info->score = 6;//值为1/2/3/4/5/6对应S/A/B/C/D/作弊
		for (uint32_t i = 0; i < p_score_info->course_list.count; i++) {//作弊分数为0
			p_score_info->course_list.course_score[i].score = 0;
		}
	} else {
		p_score_info->score = get_lamu_score_level(score_sum, p_score_info->course_list.count);
		//DEBUG_LOG("----alexhe:student pingji:%u,%u----", score_sum, p_score_info->score);
	}

	return SUCC;
}

/*
 * @brief 课程得分的计算公式
 */
uint32_t Cuser_lamu_classroom_lamu::cal_lamu_score(uint32_t course_score, 
		uint32_t *attr_vals, uint32_t attr[], uint32_t lovely, uint32_t exam_times)
{
	uint32_t attr_a = 0;
	uint32_t attr_b = 0;
	if (attr[0] != 0) {
		attr_a = attr_vals[attr[0] - 1];
		if (attr[1] != 0) {
			attr_b = attr_vals[attr[1] - 1];
		}
	}

	double score = 0;
	score = (course_score * EXAM_FAC_1 + (attr_a * ATTR_FAC_1 + attr_b * ATTR_FAC_2) * EXAM_FAC_2);
	score = score * g_exam_fac[exam_times];//这儿考试次数还没有+1
	//如果第三次考试时，这门课的掌握情况为0，则计算的分数不能大于400
	if (exam_times == 2 && course_score == 0 && score > 400) {
		score = 400;
	}
	/*switch (exam_times) {
	case 0:
		score = score * g_exam_fac[0];
		break;
	case 1:
		score = score * g_exam_fac[1];
		break;
	case 2:
		score = score * g_exam_fac[2];
		//如果没有上过这门课程，则分数不能高于40分
		if (course_score == 0 && score > 400) {
			score = 400;
		}
		break;
	default:
		break;
	}*/
	//DEBUG_LOG("----alexhe:exam cal:%u,%u,%u,%u----",attr_a, attr_b,course_score,(uint32_t)score);
	return (uint32_t)BETWEEN_VAL(score, 0, MAX_SCORE);
}

int Cuser_lamu_classroom_lamu::insc_lamu_level(userid_t userid, uint32_t lamu_cnt)
{
	sprintf(this->sqlstr, "update %s set level = if(level = 101, level, if(level = 4, level, level + 1)) where userid = %u",
					this->get_table_name(userid),
					userid
					);

	STD_SET_LIST_RETURN_WITH_CHECK(this->sqlstr, (int)lamu_cnt, USER_ID_NOFIND_ERR);
}

/*
 * @brief 设置学生的心情
 */
int Cuser_lamu_classroom_lamu::set_one_lamu_mood(userid_t userid, user_lamu_classroom_set_lamu_mood_in *p_in)
{
	sprintf(this->sqlstr, "update %s set mood = %u where userid = %u and lamu_id = %u",
					this->get_table_name(userid),
					p_in->mood,
					userid,
					p_in->lamu_id
					);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief update学生的属性
 */
int Cuser_lamu_classroom_lamu::update_one_lamu_attr(userid_t userid, 
		user_lamu_classroom_update_lamu_attr_in *p_in, uint32_t course_id)
{
	if (p_in->type == 1) {//增加潜力
		sprintf(this->sqlstr, "update %s set latent = if(latent + %u > 1000, 1000, latent + %u) \
							where userid = %u and lamu_id = %u",
						this->get_table_name(userid),
						p_in->add_val,
						p_in->add_val,
						userid,
						p_in->lamu_id
						);
	} else if (p_in->type == 2) { // 增加课程掌握程度
		user_lamu_course_list course_list = {0};
		int ret = this->get_one_lamu_course_info(userid, p_in->lamu_id, &course_list);
		if (ret != SUCC) {
			return ret;
		}
		uint32_t index = this->is_courseid_in_course_list(course_id, course_list);
		if (index == 0) {
			course_list.course_score[course_list.count].course_id = course_id;
			course_list.course_score[course_list.count].score = p_in->add_val;
			course_list.count += 1;
		} else {
			course_list.course_score[index - 1].score += p_in->add_val;
			course_list.course_score[index - 1].score = BETWEEN_VAL(course_list.course_score[index - 1].score, 0, MAX_SCORE);
		}
		return this->set_one_lamu_course_info(userid, p_in->lamu_id, course_list);
	}
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 增加学生的课程掌握情况
 */
int Cuser_lamu_classroom_lamu::add_lamu_course_score(userid_t userid, user_lamu_classroom_add_course_score_in *p_in)
{
	lamu_course_score_stru *p_list = NULL;
	uint32_t lamu_cnt = 0;
	int ret = this->get_lamus_course_list(userid, &p_list, &lamu_cnt);
	if (ret != SUCC) {
		if (p_list != NULL) {
			free(p_list);
			p_list = NULL;
		}
		return ret;
	}
	if (lamu_cnt == 0) {
		return USER_LAMU_CLASSROOM_STUDENT_NUM_ERR;
	}

	user_lamu_course_list temp_course = {0};                //拉姆的所有课程掌握情况
	for (uint32_t i = 0; i < lamu_cnt; ++i) {
		int32_t index;	//返回course_id的位置index，如果index = 0, 则表示之前没有该课程
		memcpy((char*)&temp_course, (char*)&((p_list + i)->course_list), sizeof(user_lamu_course_list));
		index = this->is_courseid_in_course_list(p_in->course_id, temp_course);
		if (index == 0) {//如果之前没有上过这门课
			temp_course.course_score[temp_course.count].course_id = p_in->course_id;
			temp_course.course_score[temp_course.count].score = p_in->add_val;
			temp_course.count += 1;
		} else {
			temp_course.course_score[index - 1].score += p_in->add_val;
			temp_course.course_score[index - 1].score = BETWEEN_VAL(temp_course.course_score[index - 1].score, 0, MAX_SCORE);
		}
		ret = this->set_one_lamu_course_info(userid, (p_list + i)->lamu_id, temp_course);
		memset((char*)&temp_course, 0, sizeof(temp_course));
	}
	if (p_list != NULL) {
		free(p_list);
		p_list = NULL;
	}

	return ret;
}

/*
 * @brief 拉取所有学生的课程掌握情况
 */
int Cuser_lamu_classroom_lamu::get_lamus_course_list(uint32_t userid, lamu_course_score_stru **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select lamu_id, course_info from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_id);
		BIN_CPY_NEXT_FIELD(&((*pp_list + i)->course_list), sizeof(user_lamu_course_list));
	STD_QUERY_WHILE_END();
}

/*
 * @brief 拉取一个学生的课程掌握情况
 */
int Cuser_lamu_classroom_lamu::get_one_lamu_course_info(uint32_t userid, uint32_t lamu_id, 
		user_lamu_course_list *p_course_list)
{
	sprintf(this->sqlstr, "select course_info from %s where userid = %u and lamu_id = %u",
					this->get_table_name(userid),
					userid,
					lamu_id
					);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD((char*)p_course_list, sizeof(user_lamu_course_list));
	STD_QUERY_ONE_END();
}

/*
 * @brief 增加一个学生的各项属性
 */
int Cuser_lamu_classroom_lamu::add_one_lamu_attrs(uint32_t userid, user_lamu_classroom_add_lamu_attrs_in *p_in)
{
	sprintf(this->sqlstr, "select latent, moral, intelligence, sport, art, labor \
									from %s where userid = %u and lamu_id = %u",
					this->get_table_name(userid),
					userid,
					p_in->lamu_id
					);

	int32_t latent = 0;
	int32_t intelligence = 0;
	int32_t moral = 0;
	int32_t sport = 0;
	int32_t art = 0;
	int32_t labor = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(latent);
		INT_CPY_NEXT_FIELD(moral);
		INT_CPY_NEXT_FIELD(intelligence);
		INT_CPY_NEXT_FIELD(sport);
		INT_CPY_NEXT_FIELD(art);
		INT_CPY_NEXT_FIELD(labor);
	STD_QUERY_ONE_END_WITHOUT_RETURN();
	
	latent += p_in->latent;
	intelligence += p_in->intelligence;
	moral += p_in->moral;
	sport += p_in->sport;
	art += p_in->art;
	labor += p_in->labor;

	sprintf(this->sqlstr, "update %s set latent = %u, moral = %u, intelligence = %u, sport = %u, art = %u, labor = %u\
			                        where userid = %u and lamu_id = %u",
					this->get_table_name(userid),
					BETWEEN_VAL(latent, 0, 1000),
					BETWEEN_VAL(moral, 0, 1000),
					BETWEEN_VAL(intelligence, 0, 1000),
					BETWEEN_VAL(sport, 0, 1000),
					BETWEEN_VAL(art, 0, 1000),
					BETWEEN_VAL(labor, 0, 1000),
					userid,
					p_in->lamu_id
					);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_lamu_classroom_lamu::add_lamu_attrs(uint32_t userid, user_lamu_classroom_add_lamu_attrs_in *p_in)
{
	sprintf(this->sqlstr, "select lamu_id from %s where userid = %u",this->get_table_name(userid),userid);

	uint32_t count = 0;
	uint32_t *p_lamuid = NULL;

	uint32_t *p_count = &count;
	uint32_t **pp_lamuid = &p_lamuid;

    STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_lamuid, p_count);
    	INT_CPY_NEXT_FIELD(p_lamuid[i]);
    STD_QUERY_WHILE_END_WITHOUT_RETURN();

	if(p_lamuid != NULL) {
		int i;
		for(i=0; i < (int)count;i++) {
			p_in->lamu_id = p_lamuid[i];
			this->add_one_lamu_attrs(userid,p_in);
		}
		free(p_lamuid);
	}

	return SUCC;
}
