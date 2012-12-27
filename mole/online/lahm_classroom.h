#ifndef _LAHM_CLASSROOM_H_
#define  _LAHM_CLASSROOM_H_

#include "sprite.h"
#include "item.h"

typedef struct {
	uint32_t    id;
	uint32_t    latent;
	uint32_t    moral;
	uint32_t    iq;
	uint32_t    sport;
	uint32_t    art;
	uint32_t    labor;
	uint32_t    sl_flag;
}__attribute__(( packed ))student_info_t;

typedef struct {
	uint32_t		level;
	uint32_t		student_count;
	student_info_t  students[30];
}__attribute__(( packed ))level_students_t;

typedef struct {
	uint32_t		    grade;
	uint32_t		    level_count;
	level_students_t    level_students[10];
}__attribute__(( packed )) students_info_t;

typedef struct s_lesson_info{
	uint32_t    id;
	uint32_t    class_hour;
	uint32_t    time;
	uint32_t    channel;
	uint32_t    condition;
	uint32_t    energy;
	uint32_t    lovely;
	uint32_t    latent;
	uint32_t    moral;
	uint32_t    iq;
	uint32_t    sport;
	uint32_t    art;
	uint32_t    labor;
	uint32_t    lesson_degree;
	uint32_t    level_exp[5];
	uint32_t    attr[2];
	uint32_t    type;
	uint32_t    viponly;
	}__attribute__(( packed ))lesson_info_t;

typedef struct {
	uint32_t		exam_num;
	uint32_t		lesson_count;
	lesson_info_t   lessons[30];
}__attribute__(( packed ))exam_lessons_t;

typedef struct {
	uint32_t		level;
	uint32_t		item_count;
	item_unit_t  	item[20];
}__attribute__(( packed ))level_item_t;

typedef struct {
	uint32_t		exam_time;
	uint32_t		level_count;
	level_item_t	item_level[5];
}__attribute__(( packed )) classroom_bonus_t;

typedef struct s_question_info{
	uint32_t    id;
	uint32_t    answer;
	uint32_t    value;
	uint32_t    lovely;
	uint32_t    moral;
	uint32_t    iq;
	uint32_t    sport;
	uint32_t    art;
	uint32_t    labor;
	uint32_t    latent;
}__attribute__(( packed ))question_info_t;

typedef struct {
	uint32_t		    lesson_id;
	uint32_t		    question_count;
	question_info_t     questions[50];
}__attribute__(( packed ))lesson_questions_t;

typedef struct s_fellowship_info{
	uint32_t    id;
	uint32_t    lovely;
	uint32_t    moral;
	uint32_t    iq;
	uint32_t    sport;
	uint32_t    art;
	uint32_t    labor;
	uint32_t    latent;
	int	        itembonus[10];
	uint32_t	bonus_cnt;
}__attribute__(( packed ))fellowship_info_t;



int get_classroom_last_grid_cmd(sprite_t * p,const uint8_t * body,int len);
int get_classroom_last_grid_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_grid_classroom_info_cmd(sprite_t * p,const uint8_t * body,int len);
int get_grid_classroom_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

int create_classroom_cmd(sprite_t * p,const uint8_t * body,int len);
int create_classroom_callback(sprite_t* p, uint32_t id, char* buf, int len);

int set_classroom_name_cmd(sprite_t * p,const uint8_t * body,int len);
int set_classroom_name_callback(sprite_t* p, uint32_t id, char* buf, int len);

int set_classroom_inner_style_cmd(sprite_t * p,const uint8_t * body,int len);
int set_classroom_inner_style_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_classroom_info_cmd(sprite_t * p,const uint8_t * body,int len);
int get_classroom_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

int load_lahm_students_conf(const char *file);

int query_all_students_info_cmd(sprite_t * p,const uint8_t * body,int len);
int query_all_students_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

int add_lahm_students_info_cmd(sprite_t * p,const uint8_t * body,int len);
int add_lahm_students_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int del_lahm_students_info_cmd(sprite_t * p,const uint8_t * body,int len);
int del_lahm_students_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_lahm_teacher_info_cmd(sprite_t * p,const uint8_t * body,int len);
int get_lahm_teacher_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_lahm_students_info_cmd(sprite_t * p,const uint8_t * body,int len);
int get_lahm_students_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_teach_plan_info_cmd(sprite_t * p,const uint8_t * body,int len);
int get_teach_plan_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_classroom_class_begin_cmd(sprite_t * p,const uint8_t * body,int len);
int set_classroom_class_begin_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_classroom_class_mode_cmd(sprite_t * p,const uint8_t * body,int len);
int set_classroom_class_mode_callback(sprite_t* p, uint32_t id, char* buf, int len);

int load_lahm_lessons_conf(const char *file);
int set_classroom_class_end_cmd(sprite_t * p,const uint8_t * body,int len);
int set_classroom_class_end_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_classroom_course_level_cmd(sprite_t * p,const uint8_t * body,int len);
int get_classroom_course_level_callback(sprite_t* p, uint32_t id, char* buf, int len);
int classroom_class_end_mgr(sprite_t * p, uint32_t course_id, uint32_t is_return, uint32_t end_flag);
int notify_classroom_teacher_info(sprite_t* p, uint32_t exp, uint32_t energy, uint32_t lovely);
int notify_classroom_event_info(sprite_t* p, uint32_t eventid);
int get_classroom_course_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int lahm_classroom_exam_cmd(sprite_t * p,const uint8_t * body,int len);
int lahm_classroom_exam_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_course_id_type(sprite_t * p, uint32_t course_id);
int get_lahm_teacher_exam_info_cmd(sprite_t * p,const uint8_t * body,int len);
int get_lahm_teacher_exam_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_lahm_classroom_count_cmd(sprite_t * p,const uint8_t * body,int len);
int get_lahm_classroom_count_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_classroom_rand_userid_cmd(sprite_t * p,const uint8_t * body,int len);
int get_classroom_rand_userid_callback(sprite_t* p, uint32_t id, char* buf, int len);
int load_lahm_classroom_bonus(const char *file);
int create_lahm_classroom(sprite_t * p);
int get_classroom_class_flag_cmd(sprite_t * p,const uint8_t * body,int len);
int get_classroom_class_flag_callback(sprite_t* p, uint32_t id, char* buf, int len);
int check_lesson_condtion(sprite_t * p, uint32_t channel, uint32_t condtion);
int lahm_classroom_class_graduate_cmd(sprite_t * p,const uint8_t * body,int len);
int lahm_classroom_class_graduate_callback(sprite_t* p, uint32_t id, char* buf, int len);
int lahm_classroom_teach_remembrance_cmd(sprite_t * p,const uint8_t * body,int len);
int lahm_classroom_teach_remembrance_callback(sprite_t* p, uint32_t id, char* buf, int len);
int lahm_classroom_get_items_cmd(sprite_t * p,const uint8_t * body,int len);
int lahm_classroom_use_item_cmd(sprite_t * p,const uint8_t * body,int len);
int lahm_classroom_use_item_callback(sprite_t * p, uint32_t itemid, uint32_t petid);
int calc_teach_exp_to_level(uint32_t exp);
int set_teach_profession_level_change(sprite_t * p, uint32_t exp_end);
int lahm_classroom_check_add_course_callback(sprite_t* p, uint32_t id, char* buf, int len);
int change_classroom_teacher_info(sprite_t* p, uint32_t type, uint32_t add_value);
int change_teacher_attr_item(sprite_t * p, uint32_t type, uint32_t add_value);
int get_friend_classroom_info_cmd(sprite_t* p, const uint8_t* body, int len);
int get_friend_classroom_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int notify_classroom_class_end(sprite_t* p, uint32_t course_id, uint32_t class_flag);
int add_lahm_classroom_game_score_cmd(sprite_t * p,const uint8_t * body,int len);
int add_lahm_classroom_game_score_callback(sprite_t* p, uint32_t id, char* buf, int len);
int load_lahm_lesson_questions_conf(const char *file);
int answer_lahm_classroom_lesson_question_cmd(sprite_t * p,const uint8_t * body,int len);
int add_lahm_classroom_lesson_event_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_lahm_classroom_honor_list_cmd(sprite_t * p,const uint8_t * body,int len);
int get_graduate_cnt_honor(uint32_t graduate_cnt);
int set_lahm_classroom_honor_bit(sprite_t * p,int bit) ;
int check_inner_style_condtion(sprite_t * p, uint32_t channel, uint32_t condtion);
int get_classroom_inner_style_list_cmd(sprite_t * p,const uint8_t * body,int len);
int set_all_graduate_cnt_honor(sprite_t * p, uint32_t graduate_cnt);
int load_classroom_fellowship_conf(const char* file);
int on_lahm_classroom_fellowship_cmd(sprite_t * p,const uint8_t * body,int len);
int on_lahm_classroom_fellowship_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_all_class_cnt_honor(sprite_t * p, uint32_t class_cnt);
int get_class_cnt_honor(uint32_t class_cnt);





#endif

