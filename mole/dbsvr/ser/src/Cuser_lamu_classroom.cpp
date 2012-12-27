/*
 * =====================================================================================
 *
 *       Filename:  Cuser_lamuclass.cpp
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  09/01/2010 05:11:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_lamu_classroom.h"
#include "common.h"

//3次考试时课程数
const uint32_t g_exam_count[] = {25, 55, 100};
//根据经验得到每日的最大上课数，从5开始
const uint32_t g_course_today[] = {150, 3750, 9000, 21750, 32010, 40970, 54870, 68150, 90150, 126450};
//根据考评计算每日最大课时数SABCD分别对应53210,默认为1(第一次招收学生还没有考评时)
const uint32_t g_evaluate_today[] = {1, 5, 3, 2, 1, 0};
//根据经验得到最大学生数,从2开始
const uint32_t g_max_student_cnt[] = {500, 7050, 16050, 26550, 48950, 76050, 106950, 148650};
//教学质量评分标准
const uint32_t g_quality_level[] = {45, 40, 30, 20, 0};
//根据亲密度计算学生对老师的评分
const uint32_t g_student_score[] = {950, 900, 800, 600, 0};
//计算教学质量时，不同等级的学生的系数不同
const uint32_t g_student_level_fac[] = {50, 40, 30, 20, 10};
//SABCD不同等级得到的经验不同
const uint32_t g_exp_level[] = {100, 90, 80, 60, 10};
//学生人数不同时，考评计算不同
const uint32_t g_student_cnt[] = {3, 6, 9};
//计算老师考评，不同学生数时经验值的划分
//一维为学生人数的区别，二维为经验区间
const uint32_t g_evaluate_exp[][5] = {
	{360, 260, 220, 150, 0},
	{440, 400, 350, 260, 0},
	{510, 480, 420, 340, 0},
	{600, 570, 520, 430, 0},
	{680, 620, 570, 490, 0},
	{750, 680, 620, 540, 0},
	{800, 720, 660, 580, 0},
	{860, 780, 710, 620, 0},
	{920, 830, 770, 670, 0},
	{1000, 900, 840, 740, 0}
};

#define GET_EXP_FROM_DIFFICULTY(val) (uint32_t)(100.0*(1000-(val))/1000)
#define MAX_EVENT_CNT 1

/*
 * @brief 类构造函数
 * @param db 数据库句柄
 */ 
Cuser_lamu_classroom::Cuser_lamu_classroom(mysql_interface * db)
	:CtableRoute100x10( db, "USER", "t_user_lamu_classroom", "userid")
{
}

/* 
 * @brief t_user_lamu_class表中创建拉姆教室
 */
int Cuser_lamu_classroom::insert(userid_t userid, user_lamu_classroom_create_classroom_in* p_in) 
{  
	uint32_t date = get_date(time(0));
	sprintf(this->sqlstr, "insert into %s (userid, room_id, room_name, inner_style, class_flag, last_date) \
						values(%u, %u, '拉姆教室', %u, 2, %u)",
					this->get_table_name(userid),
					userid,
					p_in->room_id,
					p_in->inner_style,
					date
					);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief 修改拉姆教室名称
 */
int Cuser_lamu_classroom::set_lamu_classroom_name(userid_t userid, char room_name[])
{
	char room_name_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(room_name_mysql, room_name, NICK_LEN);

	sprintf( this->sqlstr, "update %s set room_name = '%s' where userid = %u", 
					this->get_table_name(userid),
					room_name_mysql,
					userid
					);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

/* 
 * @brief 设置拉姆教室内部装扮
 */
int Cuser_lamu_classroom::set_lamu_classroom_inner_style(userid_t userid, uint32_t inner_style)
{
	sprintf( this->sqlstr, "update %s set inner_style = %u where userid = %u",
					this->get_table_name(userid),
					inner_style,
					userid
					);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 每学期第一次招生后，修改教学困难度、精力、亲密度、年级 
 */
int Cuser_lamu_classroom::new_lamu_classroom(userid_t userid, uint32_t difficulty)
{
	uint32_t evaluate = 0;
    this->get_teacher_evaluate(userid, evaluate);
    uint32_t today_add = g_evaluate_today[evaluate];	

	sprintf( this->sqlstr, "update %s set difficulty = %u, \
						term_sum = 100, \
						term_left = 100, \
						energy = 1000, \
						lovely = 1000, \
						today_add = %u \
						where userid = %u",
					this->get_table_name(userid),
					difficulty,
					today_add,
					userid
					);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 得到困难度
 */
int Cuser_lamu_classroom::get_class_difficulty(userid_t userid, uint32_t &difficulty)
{
	sprintf( this->sqlstr, "select difficulty from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(difficulty);
	STD_QUERY_ONE_END();
}

/*
 * @brief 设置困难度
 */
int Cuser_lamu_classroom::set_class_difficulty(userid_t userid, uint32_t difficulty)
{
	sprintf( this->sqlstr, "update %s set difficulty = %u where userid = %u",
					this->get_table_name(userid),
					difficulty,
					userid
					);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/* 
 * @brief 放弃当前学生,教室状态、考试次数、困难度、精力、亲密度、等要清0
 */
int Cuser_lamu_classroom::abandon_lamu_classroom(userid_t userid)
{
	sprintf( this->sqlstr, "update %s set exam_times = 0,\
		   				class_flag = 2, \
						difficulty = 0, \
						energy = 0, \
						lovely = 0, \
						term_sum = 0,\
						term_left = 0 \
					   	where userid = %u",
					this->get_table_name(userid),
					userid
					);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

/* 
 * @brief 得到拉姆教室的信息
 */
int Cuser_lamu_classroom::get_lamu_classroom_info(userid_t userid, user_lamu_classroom_get_class_info_out_header *p_out)
{
	uint32_t start_time = time(0);
	uint32_t term_sum;
	uint32_t term_left;
	uint32_t exam_times;
	sprintf( this->sqlstr, "select \
						userid, \
						room_id, \
						room_name, \
						inner_style, \
						exam_times, \
						exp, \
						graduate_sum, \
						outstand_sum, \
						energy, \
						lovely, \
		   				class_flag, \
						cur_course_id, \
						(%u - start_time) as course_time, \
						term_sum, \
						term_left, \
						honor, \
						exam_times, \
						event_cnt, \
						course_sum \
						from %s where userid = %u",
					start_time,
					this->get_table_name(userid),
					userid
					);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->userid);
		INT_CPY_NEXT_FIELD(p_out->room_id);
		MEM_CPY_NEXT_FIELD(p_out->room_name, NICK_LEN);
		INT_CPY_NEXT_FIELD(p_out->inner_style);
		INT_CPY_NEXT_FIELD(p_out->exam_times);
		INT_CPY_NEXT_FIELD(p_out->exp);
		INT_CPY_NEXT_FIELD(p_out->graduate_sum);
		INT_CPY_NEXT_FIELD(p_out->outstand_sum);
		INT_CPY_NEXT_FIELD(p_out->energy);
		INT_CPY_NEXT_FIELD(p_out->lovely);
		INT_CPY_NEXT_FIELD(p_out->class_flag);
		INT_CPY_NEXT_FIELD(p_out->cur_course_id);
		INT_CPY_NEXT_FIELD(p_out->course_time);
		INT_CPY_NEXT_FIELD(term_sum);
		INT_CPY_NEXT_FIELD(term_left);
		INT_CPY_NEXT_FIELD(p_out->honor);
		INT_CPY_NEXT_FIELD(exam_times);
		INT_CPY_NEXT_FIELD(p_out->event_cnt);
		INT_CPY_NEXT_FIELD(p_out->course_sum);
	STD_QUERY_ONE_END_WITHOUT_RETURN();
	
	//判断是否要考试
    if (term_sum - term_left >= g_exam_count[exam_times]) {
		p_out->exam_flag = exam_times + 1;
	} else {
		p_out->exam_flag = 0;
	}

	return SUCC;
}

/*
 * @brief 得到教室档案
 */ 
int Cuser_lamu_classroom::get_lamu_classroom_record(userid_t userid, user_lamu_classroom_get_record_out_header *p_out)
{
	sprintf( this->sqlstr, "select \
						userid,\
						exp, \
			            difficulty, \
						level_s, \
						graduate_sum, \
						outstand_sum, \
						evaluate, \
						class_sum, \
						course_sum \
						from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->userid);
		INT_CPY_NEXT_FIELD(p_out->exp);
		INT_CPY_NEXT_FIELD(p_out->difficulty);
		INT_CPY_NEXT_FIELD(p_out->level_s);
		INT_CPY_NEXT_FIELD(p_out->graduate_sum);
		INT_CPY_NEXT_FIELD(p_out->outstand_sum);
		INT_CPY_NEXT_FIELD(p_out->evaluate);
		INT_CPY_NEXT_FIELD(p_out->class_sum);
		INT_CPY_NEXT_FIELD(p_out->course_sum);
	STD_QUERY_ONE_END();
}

/*
 * @brief 拉取教学计划信息
 */
int Cuser_lamu_classroom::get_lamu_classroom_teach_plan(userid_t userid, 
		user_lamu_classroom_get_teach_plan_out *p_out)
{
	sprintf( this->sqlstr, "select today_sum, today_add, term_left, term_sum, exp from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);

	int exp = 0;
	int today_sum;
	int today_add;
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(today_sum);
		INT_CPY_NEXT_FIELD(today_add);
		INT_CPY_NEXT_FIELD(p_out->term_left);
		INT_CPY_NEXT_FIELD(p_out->term_sum);
		INT_CPY_NEXT_FIELD(exp);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

//	p_out->today_left = get_value_index_insc(exp, g_course_today, sizeof(g_course_today) / sizeof(g_course_today[0]), 5) 
//			+ today_add - today_sum;

	int today_enable = 0;	
	today_enable = get_value_index_insc(exp, g_course_today, sizeof(g_course_today) / sizeof(g_course_today[0]), 5) 
			+ today_add;
	if (today_enable >= today_sum)
	{
		p_out->today_left = today_enable - today_sum;
	}
	else
	{
		p_out->today_left = 0;
	}

	p_out->today_left = MAX_VAL(p_out->today_left, p_out->term_left);//今日剩余课时数不能大于学期剩余课时数
	return SUCC;
}

/*
 * @brief 开始上课，设置上课方式，开始时间，课程id
 */
int Cuser_lamu_classroom::set_class_begin(userid_t userid, uint32_t course_id, uint32_t class_flag)
{
	uint32_t start_time = time(0);
	sprintf( this->sqlstr, "update %s set class_flag = %u, start_time = %u, cur_course_id = %u where userid = %u",
					this->get_table_name(userid),
					class_flag,
					start_time,
					course_id,
					userid
					);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief  设置上课模式class_flag
 */
int Cuser_lamu_classroom::set_class_mode(userid_t userid, uint32_t class_flag)
{
	sprintf( this->sqlstr, "update %s set class_flag = %u where userid = %u",
					this->get_table_name(userid),
					class_flag,
					userid
					);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief  得到上课模式,判断是否正常上课
 */
int Cuser_lamu_classroom::get_class_flag(uint32_t userid, uint32_t &class_flag)
{
	sprintf( this->sqlstr, "select class_flag from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(class_flag);
	STD_QUERY_ONE_END();
}

/*
 * @brief 下课后，更新教室经验、精力、亲密度、class_flag等
 */
int Cuser_lamu_classroom::update_when_class_end(userid_t userid, 
			user_lamu_classroom_class_end_out class_end, uint32_t course_cnt)
{
	sprintf( this->sqlstr, "update %s set \
		   				energy = %u, \
						lovely = %u, \
						exp = %u, \
						class_flag = %u, \
						cur_course_id = 0, \
						start_time = 0, \
						course_sum = course_sum + %u, \
						term_left = if(term_left < %u, 0, term_left - %u), \
						today_sum = today_sum + %u, \
						cur_course_flag = 0, \
						event_cnt = 0 \
						where userid = %u",
					this->get_table_name(userid),
					class_end.energy, 
					class_end.lovely, 
					class_end.exp,
					class_end.class_flag,
					course_cnt,
					course_cnt, course_cnt,
					course_cnt,
					userid
					);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 下课时，得到亲密度和精力，用与计算学生的各项属性的该变
 */
int Cuser_lamu_classroom::get_class_end_info(userid_t userid, user_lamu_classroom_class_end_out *p_out, uint32_t course_cnt)
{
	uint32_t term_sum;
	uint32_t term_left;
	uint32_t exam_times;

	sprintf( this->sqlstr, "select class_flag, exp, energy, lovely, term_sum, term_left, exam_times from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->class_flag);
		INT_CPY_NEXT_FIELD(p_out->exp);
		INT_CPY_NEXT_FIELD(p_out->energy);
		INT_CPY_NEXT_FIELD(p_out->lovely);
		INT_CPY_NEXT_FIELD(term_sum);
		INT_CPY_NEXT_FIELD(term_left);
		INT_CPY_NEXT_FIELD(exam_times);
	STD_QUERY_ONE_END_WITHOUT_RETURN();
	
	//判断是否要考试
	if (term_sum - term_left + course_cnt >= g_exam_count[exam_times]) {
		p_out->exam_flag = exam_times + 1;
	} else {
		p_out->exam_flag = 0;
	}

	return SUCC;
}

/* 
 * @brief 设置拉姆教室荣誉标识
 */
int Cuser_lamu_classroom::set_honor_flag(userid_t userid, uint32_t index)
{
	if (index < 1) {
		return SUCC;
	}

	uint32_t honor_val = 0;
	this->get_int_value(userid, "honor", &honor_val);
	if (honor_val & 0x01<<(index - 1)) {
		return USER_LAMU_CLASSROOM_CHECK_HONOR_ERR;
	}

	sprintf(this->sqlstr, "update %s set honor = honor|(0x01<<(%u-1)) where userid = %u",
				this->get_table_name(userid), index, userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 得到好友的经验和level_s级别的学生人数
 */
int Cuser_lamu_classroom::get_exp_levels(userid_t userid, uint32_t &exp_out, uint32_t &level_s)
{
	sprintf( this->sqlstr, "select exp, level_s from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(exp_out);
		INT_CPY_NEXT_FIELD(level_s);
	STD_QUERY_ONE_END();
}

/*
 * @brief 得到经验和本学期的考试次数
 */ 
int Cuser_lamu_classroom::get_exp_course_cnt(userid_t userid, user_lamu_classroom_get_exp_course_out *p_out)
{
	sprintf( this->sqlstr, "select exp, difficulty, exam_times from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->exp);
		INT_CPY_NEXT_FIELD(p_out->difficulty);
		INT_CPY_NEXT_FIELD(p_out->exam_times);
	STD_QUERY_ONE_END();
}

/*
 * @brief 下课时判断是否合法下课
 */
int Cuser_lamu_classroom::is_class_end_right(userid_t userid, uint32_t course_id, uint32_t course_time)
{
	uint32_t end_time = time(0);
	sprintf( this->sqlstr, "select cur_course_id, start_time as course_time from %s where userid = %u",
					this->get_table_name(userid),
					userid
		   			);

	uint32_t tmp_course_id;
	uint32_t start_time;
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(tmp_course_id);
		INT_CPY_NEXT_FIELD(start_time);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	if (course_id != tmp_course_id) { //课程id不对
		return USER_LAMU_CLASSROOM_CLASS_END_ERR;
	}
	if (end_time - start_time + 30 < course_time) {//提前下课
		return USER_LAMU_CLASSROOM_CLASS_END_EARLY_ERR;
	}
	return SUCC;
}

/*
 * @brief 进教室前判断是否正在上课 
 */
int Cuser_lamu_classroom::is_class_time(userid_t userid, user_lamu_classroom_is_class_time_out *p_out)
{
	uint32_t start_time = time(0);
	sprintf( this->sqlstr, "select class_flag, cur_course_id, (%u - start_time) as course_time from %s where userid = %u",
					start_time,
					this->get_table_name(userid),
					userid
					);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->class_flag);
		INT_CPY_NEXT_FIELD(p_out->cur_course_id);
		INT_CPY_NEXT_FIELD(p_out->cur_course_time);
	STD_QUERY_ONE_END();
}

/*
 * @brief 考试结束前得到困难度，class_flag，考试次数，用于计算成绩
 */
int Cuser_lamu_classroom::get_about_exam_info(userid_t userid, 
		uint32_t &lovely, uint32_t &difficulty, uint32_t &exam_times, uint32_t &exp)
{
	uint32_t class_flag;
	sprintf( this->sqlstr, "select class_flag, lovely, difficulty, exam_times, exp from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(class_flag);
		INT_CPY_NEXT_FIELD(lovely);
		INT_CPY_NEXT_FIELD(difficulty);
		INT_CPY_NEXT_FIELD(exam_times);
		INT_CPY_NEXT_FIELD(exp);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	if (class_flag != 4) {
		return USER_LAMU_CLASSROOM_EXAM_ERR;
	}
	return SUCC;
}

/*
 * @brief 判断用户是否拥有lamu教室
 */
int Cuser_lamu_classroom::is_user_have_class(userid_t userid, uint32_t *pflag)
{
	sprintf( this->sqlstr, "select count(*) from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);

	*pflag = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*pflag);
	STD_QUERY_ONE_END();
}

/*
 * @brief 考试后修改相应的状态
 */
int Cuser_lamu_classroom::set_classroom_after_exam(userid_t userid, user_lamu_classroom_exam_info exam_info)
{
	uint32_t class_flag = 2;
	if (exam_info.exam_times == 3) {//毕业
	//	exam_times = 0;
		class_flag = 5;
	}
	
	sprintf( this->sqlstr, "update %s set exam_times = %u, \
						exp = exp + %u, \
						class_flag = %u, \
						evaluate = %u, \
						level_s = if(%u = 1, level_s + 1, level_s)\
						where userid = %u",
					this->get_table_name(userid),
					exam_info.exam_times,
					exam_info.exp,
					class_flag,
					exam_info.evaluate,
					exam_info.evaluate,
					userid
					);


	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 得到当前教学考评
 */
int Cuser_lamu_classroom::get_teacher_evaluate(userid_t userid, uint32_t &evaluate)
{
	sprintf(this->sqlstr, "select evaluate from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(evaluate);
	STD_QUERY_ONE_END();
}

/*
 * @brief 每天第一次进教室前设置
 */
int Cuser_lamu_classroom::update_classroom_per_day(userid_t userid)
{
	uint32_t date = get_date(time(0));
	uint32_t evaluate = 0;
	this->get_teacher_evaluate(userid, evaluate);
	uint32_t today_add = g_evaluate_today[evaluate];
/* 	switch (evaluate) {
	case 1:
		today_add = 5;
		break;
	case 2:
		today_add = 3;
		break;
	case 3:
		today_add = 2;
		break;
	case 0:
	case 4:
		today_add = 1;
		break;
	default:
		today_add = 0;
		break;
	}
	*/
	sprintf( this->sqlstr, "update %s set today_sum = 0, \
						today_add = %u, \
						energy = if(energy + 20 > 1000, 1000, energy + 20), \
						last_date = %u \
						where userid = %u and last_date != %u",
					this->get_table_name(userid),
					today_add,
					date,
					userid,
					date
					);

	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/*
 * @brief 得到考试数
 */
int Cuser_lamu_classroom::get_exam_times(userid_t userid, uint32_t &exam_times)
{
	sprintf( this->sqlstr, "select exam_times from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(exam_times);
	STD_QUERY_ONE_END();
}

/*
 * @brief 判断是否有过考试
 */
int Cuser_lamu_classroom::is_exam_state(userid_t userid, uint32_t &exam_times)
{
	int ret = this->get_exam_times(userid, exam_times);
	if (ret == SUCC && exam_times == 0) {
		ret = USER_LAMU_CLASSROOM_GET_EXAM_INFO_ERR;
	}
	return ret;
}

/*
 * @brief 毕业
 */
int Cuser_lamu_classroom::class_graduate(userid_t userid, uint32_t lamu_cnt, uint32_t great_cnt)
{
	sprintf( this->sqlstr, "update %s set \
						exam_times = 0, \
						class_flag = 2, \
						difficulty = 0, \
						energy = 0, \
						lovely = 0, \
						term_sum = 0, \
						term_left = 0, \
						today_sum = 0, \
						today_add = 0, \
						graduate_sum = graduate_sum + %u, \
						outstand_sum = outstand_sum + %u, \
						class_sum = class_sum + 1 \
						where userid = %u",
					this->get_table_name(userid),
					lamu_cnt,
					great_cnt,
					userid
					);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 得到优秀毕业生人数
 */
int Cuser_lamu_classroom::get_outstand_sum(userid_t userid, uint32_t *p_outstand_sum)
{
	sprintf(this->sqlstr, "select outstand_sum from %s where userid = %u", 
					this->get_table_name(userid), 
					userid
					);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_outstand_sum);
	STD_QUERY_ONE_END();
}

/*
 * @brief 判断是否可以毕业
 */
int Cuser_lamu_classroom::is_class_graduate_right(userid_t userid)
{
	uint32_t class_flag;
	int ret = this->get_class_flag(userid, class_flag);
	if (ret == SUCC && class_flag != 5) {
		ret = USER_LAMU_CLASSROOM_GRADUATE_ERR; 
	}
	return ret;
}

/*
 * @brief 道具增加每日课时数
 */
int Cuser_lamu_classroom::update_today_add(userid_t userid, uint32_t today_add)
{
	sprintf(this->sqlstr, "update %s set today_add = today_add + %u where userid = %u",
					this->get_table_name(userid),
					today_add,
					userid
					);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 判断是否能够用道具添加课时数
 */
int Cuser_lamu_classroom::check_add_course(userid_t userid, uint32_t add_cnt)
{
	uint32_t term_left;
	uint32_t today_sum;
	uint32_t today_add;
	uint32_t exp;
	sprintf(this->sqlstr, "select term_left, today_sum, today_add, exp from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(term_left);
		INT_CPY_NEXT_FIELD(today_sum);
		INT_CPY_NEXT_FIELD(today_add);
		INT_CPY_NEXT_FIELD(exp);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	uint32_t today_left = get_value_index_insc(exp, g_course_today, sizeof(g_course_today) / sizeof(g_course_today[0]), 5)
			+ today_add;
	today_left = (today_left > today_sum) ? (today_left - today_sum) : 0;
	//if (today_left + add_cnt > term_left) {
	if (today_left >= term_left) {
		return USER_LAMU_CLASSROOM_ADD_COURSE_ERR;
	}
	return SUCC;
}

/*
 * @brief 考试后计算教学质量、学生评分、困难度、考试次数、经验、考评等
 */
int Cuser_lamu_classroom::cal_teacher_after_exam(user_lamu_classroom_exam_info &exam_info, 
		uint32_t lamu_cnt, uint32_t lovely, uint32_t difficulty, uint32_t exam_times)
{
	uint32_t add_exp = 0;
	double quality = 1.0 * (g_student_level_fac[0] * exam_info.level_s + g_student_level_fac[1] * exam_info.level_a 
						+ g_student_level_fac[2] * exam_info.level_b + g_student_level_fac[3] * exam_info.level_c 
						+ g_student_level_fac[4] * exam_info.level_d) / lamu_cnt;
	exam_info.quality = get_value_index_desc((uint32_t)quality, 
			g_quality_level, sizeof(g_quality_level) / sizeof(g_quality_level[0]));
	exam_info.score = get_value_index_desc(lovely, g_student_score, sizeof(g_student_score) / sizeof(g_student_score[0])); 
	exam_info.difficulty = (difficulty + 99) / 100;//困难度返回0~10之间的值
	exam_info.exam_times = exam_times + 1;
	add_exp += g_exp_level[exam_info.quality - 1];//教学质量转化为经验
	add_exp += g_exp_level[exam_info.score - 1];//学生评分转化为经验
	add_exp += GET_EXP_FROM_DIFFICULTY(difficulty);//困难度转化为经验
	//SAB级学生每人可加经验分别为80\60\40
	add_exp += (80 * exam_info.level_s + 60 * exam_info.level_a + 40 * exam_info.level_b);
	exam_info.exp = add_exp;//与等级人数、学生评分、教学质量、教学难度有关
	//教学考评与经验有关,学生人数不同,对考评的影响不同,值为12345对应SABCD
	exam_info.evaluate = get_value_index_desc(add_exp, g_evaluate_exp[lamu_cnt - 1], sizeof(g_evaluate_exp[0]));
	return SUCC;
}

/*
 * @brief 每招收一个学生，改变困难度
 */
double Cuser_lamu_classroom::cal_class_difficulty(user_lamu_classroom_add_lamus_in_item *p_item, 
		double difficulty, uint32_t lamu_cnt)
{
	difficulty = 3.0 * (difficulty * lamu_cnt / 3.0 
					+ (p_item->latent + p_item->moral + p_item->intelligence 
					+ p_item->sport + p_item->art + p_item->labor) / 6.0) 
					/ (lamu_cnt + 1);
	difficulty = BETWEEN_VAL(difficulty, 0, 1000);
	return difficulty;
}

/*
 * @brief 得到老师经验
 */
int Cuser_lamu_classroom::get_teacher_exp(userid_t userid, uint32_t &exp)
{
	sprintf(this->sqlstr, "select exp from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(exp);
	STD_QUERY_ONE_END();
}
/*
 * @brief 增加老师经验
 */
int Cuser_lamu_classroom::add_teacher_exp(userid_t userid, uint32_t exp)
{
	sprintf(this->sqlstr, "update %s set exp=exp+%u where userid = %u",
					this->get_table_name(userid),
					exp,userid
					);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}


/*
 * @brief 校验时否到达今天上课的上限
 */
int Cuser_lamu_classroom::check_lamu_classroom_class_begin(userid_t userid)
{
	sprintf( this->sqlstr, "select today_sum, today_add, exp from %s where userid = %u",
			        this->get_table_name(userid),
					userid
					);

	int exp = 0;
	int today_sum = 0;
	int today_add = 0;
	int today_enable = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(today_sum);
		INT_CPY_NEXT_FIELD(today_add);
		INT_CPY_NEXT_FIELD(exp);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	today_enable = get_value_index_insc(exp, g_course_today, sizeof(g_course_today) / sizeof(g_course_today[0]), 5)
					+ today_add ;
	if (today_enable > today_sum) {
		return  1;
	} else {
		return  0; 
	}
}

/*
 * @brief 恢复老师精力或亲密度
 */
int Cuser_lamu_classroom::recover_teacher_attr(userid_t userid, user_lamu_classroom_recover_teacher_attr_in *p_in)
{
	char field[10] = {0};
	if (p_in->type == 1) {
		strcpy(field, "energy");
	} else if (p_in->type == 2) {
		strcpy(field, "lovely");
	}
	uint32_t field_val = 0;
	this->get_int_value(userid, field, &field_val);
	field_val = field_val + p_in->add_val;
	this->set_int_value(userid, field, BETWEEN_VAL((int32_t)field_val, 0, 1000));

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 判断正在上course_id这门课
 * @param type : 0课程游戏加分，1课程中处理事件
 */
int Cuser_lamu_classroom::check_class_course_id(userid_t userid, uint32_t course_id, uint32_t type)
{
	uint32_t class_flag = 2;
	uint32_t cur_course_id = 0;
	uint32_t cur_course_flag = 0;
	sprintf( this->sqlstr, "select class_flag, cur_course_id, cur_course_flag from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(class_flag);
		INT_CPY_NEXT_FIELD(cur_course_id);
		INT_CPY_NEXT_FIELD(cur_course_flag);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	switch (type) {
	case 0:	//上体育课
		if (class_flag != 0 || cur_course_id != course_id || cur_course_flag == 1) {
			return USER_LAMU_CLASSROOM_CHECK_COURSE_ID_ERR;
		}
		break;
	case 1: //课程或活动中处理事件,不需要判断cur_course_flag
		if (!(class_flag == 0 || class_flag == 1) || cur_course_id != course_id ) {
			return USER_LAMU_CLASSROOM_CHECK_COURSE_ID_ERR;
		}
		break;
	default:
		break;
	}
	return SUCC;
}

/*
 * @brief 设置cur_course_flag
 */
int Cuser_lamu_classroom::set_cur_course_flag(userid_t userid)
{
	sprintf(this->sqlstr, "update %s set cur_course_flag = 1 where userid = %u", this->get_table_name(userid), userid);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 检查班级学生人数
 */
int Cuser_lamu_classroom::check_class_student_cnt(uint32_t userid, uint32_t count)
{
	uint32_t exp = 0;
	this->get_teacher_exp(userid, exp);
	uint32_t index = get_value_index_insc(exp, g_max_student_cnt, sizeof(g_max_student_cnt), 2);
	if (count > index) {
		return USER_LAMU_CLASSROOM_MAX_STUDENT_ERR;
	}
	return SUCC;
}

/*
 * @brief 设置事件次数
 */
int Cuser_lamu_classroom::set_event_cnt(userid_t userid, uint32_t course_id, uint32_t lamu_cnt)
{
	uint32_t event_cnt = 0;
	int ret = this->get_int_value(userid, "event_cnt", &event_cnt);
	uint32_t max_event_cnt = MAX_EVENT_CNT;
	if (course_id == 14) { //随堂测试
		max_event_cnt = lamu_cnt * 2;
	}

	if (event_cnt >= max_event_cnt) {
		return USER_LAMU_CLASSROOM_MAX_EVENT_ERR;
	} else {
		ret = this->set_int_value(userid, "event_cnt", event_cnt + 1);
	}
	return ret;
}

/*
 * @brief 设置今天联谊的userid
 */
int Cuser_lamu_classroom::set_day_networking_class(userid_t userid,userid_t other)
{
	struct networking {
		uint32_t count;
		uint32_t today;
		userid_t other[3];
	} __attribute__((packed));

	struct networking network = {};	

	sprintf(this->sqlstr,"select networking from %s where userid=%u",this->get_table_name(userid), userid);
        STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
                BIN_CPY_NEXT_FIELD(&network,sizeof(network));
        STD_QUERY_ONE_END_WITHOUT_RETURN();	

	if(network.today == (uint32_t)get_date(time(NULL))) {
		if(network.count == 3) return USER_NETWORK_COUNT_LIMIT_ERR;
		int i = 0;
		while(i < (int)network.count) {
			if(network.other[i] == other) return USER_NETWORK_ALREADY_IN_ERR;
			i++;		
		}
		network.other[network.count++] = other;
	} else {
		network.count = 1;
		network.other[0] = other;
		network.today = get_date(time(NULL));
	}

	char sql_net[mysql_str_len(sizeof(network))];
	set_mysql_string(sql_net,(char*)&network,sizeof(network));
	sprintf(this->sqlstr,"update %s set networking='%s' where userid=%u",this->get_table_name(userid),sql_net,userid);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
