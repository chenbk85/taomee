#include <assert.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include <glib.h>
#include <libxml/tree.h>

#include <libtaomee/list.h>
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>
#include <statistic_agent/msglog.h>

#include "communicator.h"
#include "proto.h"
#include "util.h"
#include <libtaomee/log.h>
#include "logic.h"
#include "sprite.h"
#include "shmq.h"
#include "map.h"
#include "item.h"
#include "tasks.h"
#include "dll.h"
#include "service.h"
#include "cache_svr.h"

#include "event.h"
#include "message.h"
#include "exclu_things.h"
#include "lahm_classroom.h"

#define LAHM_CLASSROOM_GRADE_MAX    3
#define LAHM_CLASSROOM_EXAM_NUM     5
#define CLASSROOM_BONUS_TABLE_SIZE 	5
#define MAX_ITEMS 20
#define LAHM_CLASSROOM_LESSON_NUM     30
#define LAHM_CLASSROOM_FELLOWSHIP_NUM     20


static classroom_bonus_t classroom_bonus[CLASSROOM_BONUS_TABLE_SIZE];
static students_info_t students_info[LAHM_CLASSROOM_GRADE_MAX];
static exam_lessons_t  lessons_info[LAHM_CLASSROOM_EXAM_NUM];
static lesson_questions_t   questions_info[LAHM_CLASSROOM_LESSON_NUM];
static fellowship_info_t    fellowship_info[LAHM_CLASSROOM_FELLOWSHIP_NUM];
static uint32_t fellowship_num = 0;




int get_classroom_last_grid_cmd(sprite_t * p,const uint8_t * body,int len)
{
    int i = 0;
	uint32_t uid = 0;
	CHECK_BODY_LEN(len,4);
	UNPKG_UINT32(body,uid,i);

	return send_request_to_db(SVR_PROTO_GET_LAST_FRAMEID, p, 0, NULL, uid);
}

int get_classroom_last_grid_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 12);

	int i = 0;
	uint32_t end_grid;
	uint32_t start_grid;
	uint32_t my_grid;

	UNPKG_H_UINT32(buf,end_grid,i);
	UNPKG_H_UINT32(buf,start_grid,i);
	UNPKG_H_UINT32(buf,my_grid,i);

	if(my_grid != 0) my_grid--;
	if(start_grid != 0) start_grid--;

	int l = sizeof(protocol_t);
	PKG_UINT32(msg,start_grid,l);
	PKG_UINT32(msg,end_grid,l);
	PKG_UINT32(msg,my_grid,l);

	init_proto_head(msg,p->waitcmd,l);
	return send_to_self(p,msg,l,1);
}


int get_grid_classroom_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	uint32_t map_grid;
    CHECK_BODY_LEN(len, 4);
	UNPKG_UINT32(body,map_grid,i);

	map_grid++;
	p->sess_len = 0;
	PKG_H_UINT32(p->session,map_grid,p->sess_len);
	return send_request_to_db(SVR_PROTO_GET_FRAME_CLASSROOM, p, 4, &map_grid, p->id);
}

int get_grid_classroom_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i = 0;
	uint32_t count = 0;
	uint32_t is_last_grid = 0;

	uint8_t name[16];
	uint32_t owner;
	uint32_t style;
	uint32_t roomid;

    int l = 0;
	l = sizeof(protocol_t);

	UNPKG_H_UINT32(buf,count,i);
	UNPKG_H_UINT32(buf,is_last_grid,i);
	PKG_UINT32(msg, is_last_grid, l);
	PKG_UINT32(msg, count, l);
	while(count)
	{
		UNPKG_H_UINT32(buf,roomid,i);
		UNPKG_H_UINT32(buf,owner,i);
		UNPKG_STR(buf, name, i, 16);
		UNPKG_H_UINT32(buf,style,i);

		PKG_UINT32(msg,style,l);
		PKG_UINT32(msg,owner,l);
		PKG_UINT32(msg,roomid,l);
		PKG_STR(msg, name, l, 16);
		count--;
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int set_classroom_name_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_BODY_LEN(len, 20);

	if(!p->tiles || p->id != GET_UID_CLASSROOM_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_classroom_not_yours, 1);
	}

	p->sess_len = len;
	memcpy(p->session, body, len);
	*(uint32_t*)p->session = ntohl(*(uint32_t*)p->session);

	p->session[len] = 0;
	CHECK_DIRTYWORD(p, p->session + 4);
	return send_request_to_db(SVR_PROTO_SET_CLASSROOM_NAME, p, len, p->session, p->id);
}

int set_classroom_name_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	response_proto_str(p, p->waitcmd, 16, p->session + 4, 0);
	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_SET_NAME, 0, p->sess_len, p->session, p->id);
}

int create_classroom_cmd(sprite_t * p,const uint8_t * body,int len)
{
	uint32_t room_style = 1330026;
	CHECK_BODY_LEN(len, 16);

	uint8_t buf[20] = {0};
	memcpy(buf, body, 16);
	CHECK_DIRTYWORD(p, buf);

	item_t *itm = get_item_prop(room_style);
	if( !itm )
	{
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}

	p->sess_len = 0;
	PKG_STR(p->session,buf, p->sess_len,16);
	*(uint32_t*)((uint8_t*)buf + 16) = room_style;
	return send_request_to_db(SVR_PROTO_CREATE_CLASSROOM, p, sizeof(buf), buf, p->id);

}


int create_lahm_classroom(sprite_t * p)
{
	uint32_t room_style = 1330026;
	uint8_t buf[20] = {0};
	memcpy(buf, p->nick, 16);

	item_t *itm = get_item_prop(room_style);
	if( !itm )
	{
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}

	p->sess_len = 0;
	PKG_STR(p->session,buf, p->sess_len,16);
	*(uint32_t*)((uint8_t*)buf + 16) = room_style;
	return send_request_to_db(SVR_PROTO_CREATE_CLASSROOM, p, sizeof(buf), buf, p->id);

}


int create_classroom_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i = 0;
	uint32_t roomid;
	uint32_t inner_style = 1330025;

	CHECK_BODY_LEN(len, 4);
	UNPKG_H_UINT32(buf,roomid,i);

	uint8_t dbbuff[24] = {0};
	i = 0;
	PKG_H_UINT32(dbbuff, roomid, i);
	PKG_STR(dbbuff, p->session, i,16);
	PKG_H_UINT32(dbbuff, inner_style, i);

	int j = sizeof(protocol_t);
	j += pack_user_tasks(p, msg + j);
	init_proto_head(msg, PROTO_SET_TASK, j);
	send_to_self(p, msg, j, 1);

	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_CREATE_CLASSROOM, 0, sizeof(dbbuff), dbbuff, p->id);
}

int set_classroom_inner_style_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t roomid,style;

	if(!p->tiles || p->id != GET_UID_CLASSROOM_MAP(p->tiles->id)) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_classroom_not_yours, 1);
	}

	if (p->tiles->mmdu[0].lahm_class.course_id)
	{
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	p->sess_len = 0;
	UNPKG_UINT32(body,roomid,p->sess_len);
	UNPKG_UINT32(body,style,p->sess_len);

	item_kind_t *ik_style = find_kind_of_item(style);
	if(!ik_style || ik_style->kind != STREET_ITEM_KIND) {
		return send_to_self_error(p, p->waitcmd, -ERR_style_item_err, 1);
	}

	item_t *itm = get_item(ik_style, style);
	if(!itm || itm->layer != 2) {
		return send_to_self_error(p, p->waitcmd, -ERR_style_item_err, 1);
	}

	if(itm_vip_only(itm) && !ISVIP((p)->flag)) {
		return send_to_self_error(p, p->waitcmd, -ERR_style_item_err, 1);
	}

	int enable = 0;
	enable = check_inner_style_condtion(p, itm->u.shop_tag.channel, itm->u.shop_tag.condition);
	if (!enable)
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_style_item_err, 1);
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session,roomid,p->sess_len);
	PKG_H_UINT32(p->session,style,p->sess_len);
	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_SET_INNER_STYLE, p, p->sess_len, p->session, p->id);
}

int set_classroom_inner_style_callback(sprite_t* p, uint32_t id, char* buf, int len)
{

    uint32_t itemid = *(uint32_t*)(p->session + 4);
	response_proto_uint32(p, p->waitcmd, itemid, 0);
	msglog(statistic_logfile, 0x02102301 + itemid - 1330027,get_now_tv()->tv_sec, &(p->id), 4);
	return 0;
}

int get_classroom_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	uint32_t uid;
	uint32_t owner_flag;

	CHECK_BODY_LEN(len,4);
	UNPKG_UINT32(body,uid,i);

	p->sess_len = 0;
	owner_flag = (uid == p->id);
	PKG_H_UINT32(p->session, uid, p->sess_len);
	PKG_H_UINT32(p->session, owner_flag, p->sess_len);

	if (owner_flag)
	{
	    return send_request_to_db(SVR_PROTO_USER_CLASSROOM_GET_COURSE_INFO, p, 0, NULL, p->id);
    }
    else
    {
        return send_request_to_db(SVR_PROTO_USER_CLASSROOM_GET_CLASS_INFO, p, p->sess_len, p->session, uid);
    }


}

int get_classroom_course_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
     struct course_info {
		uint32_t    class_flag;
		uint32_t	course_id;
		uint32_t    course_time;
	}__attribute__((packed));

    CHECK_BODY_LEN(len, sizeof(struct course_info));
	struct course_info *p_info = (struct course_info*)buf;
	if ((p_info->class_flag == 0 || p_info->class_flag == 1) && (p_info->course_id != 0))
	{
	    int exam_num = p->exam_num;
	    if (exam_num >=  LAHM_CLASSROOM_EXAM_NUM)
	    {
	        exam_num = 0;
	    }
        uint32_t j = 0;
	    for (j = 0; j < lessons_info[exam_num].lesson_count; j++)
        {
            if (lessons_info[exam_num].lessons[j].id == p_info->course_id)
            {
                if (lessons_info[exam_num].lessons[j].time <= p_info->course_time)
                {
                    classroom_class_end_mgr(p, p_info->course_id, 1, 0);
                    return 0;
                }
            }
        }
	}

	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_GET_CLASS_INFO, p, p->sess_len, p->session, p->id);

}


int get_classroom_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    struct classroom_info_hdr {
        uint32_t	userid;
        uint32_t	roomid;
		char		room_name[16];
		uint32_t	inner_style;
		uint32_t	exam_num;
		uint32_t	exp;
		uint32_t	great_cnt;
		uint32_t	energy;
		uint32_t	lovely;
		uint32_t	class_flag;
		uint32_t	course_id;
		uint32_t	course_time;
		uint32_t	exam_flag;
		uint32_t	graduate_all;
		uint8_t	    honors[4];
		uint32_t	event_cnt;
		uint32_t	class_all;
		uint32_t	count;
	}__attribute__((packed));

    struct lahm_info {
        uint32_t	petid;
		uint32_t    nameid;
		uint32_t	stage;
		uint32_t	color;
		uint32_t	skill;
		uint32_t	sl_flag;
	}__attribute__((packed));

	uint32_t time_left = 0;

	struct classroom_info_hdr *p_hdr_info = (struct classroom_info_hdr*)buf;

	CHECK_BODY_LEN_GE(len,sizeof(struct classroom_info_hdr));

    int exam_num = p->exam_num;
	if (exam_num >=  LAHM_CLASSROOM_EXAM_NUM)
	{
	    exam_num = 0;
	}

    uint32_t j = 0;
	for (j = 0; j < lessons_info[exam_num].lesson_count; j++)
    {
        if (lessons_info[exam_num].lessons[j].id == p_hdr_info->course_id)
        {
            if (lessons_info[exam_num].lessons[j].time <= p_hdr_info->course_time)
            {
                time_left = 0;
            }
            else
            {
                time_left = lessons_info[exam_num].lessons[j].time - p_hdr_info->course_time;
            }
        }
    }

    int tm_hour = get_now_tm()->tm_hour;
	if(tm_hour < 6)
	{
		p_hdr_info->class_flag = 3;
	}

    int l = sizeof(protocol_t);;
    PKG_UINT32(msg, p_hdr_info->userid, l);
    PKG_UINT32(msg, p_hdr_info->roomid, l);
    PKG_STR(msg, p_hdr_info->room_name, l,16);
    PKG_UINT32(msg, p_hdr_info->inner_style, l);
    PKG_UINT32(msg, p_hdr_info->exp, l);
    PKG_UINT32(msg, p_hdr_info->energy, l);
    PKG_UINT32(msg, p_hdr_info->lovely, l);
    PKG_UINT32(msg, p_hdr_info->class_flag, l);
    PKG_UINT32(msg, p_hdr_info->course_id, l);
    PKG_UINT32(msg, time_left, l);
    PKG_UINT32(msg, p_hdr_info->event_cnt, l);

    p->tiles->mmdu[0].lahm_class.course_id = p_hdr_info->course_id;

    if(p->id == GET_UID_CLASSROOM_MAP(p->tiles->id))
    {
        uint32_t buff[2] = {0};
        buff[0] = p_hdr_info->exp;
        buff[1] = p_hdr_info->great_cnt;
        send_request_to_db(SVR_PROTO_CLASSROOM_SET_EXP_AND_OUTSTAND_SUM, NULL, 8, buff, p->id);
    }

    CHECK_BODY_LEN(len,sizeof(struct classroom_info_hdr) + p_hdr_info->count*sizeof(struct lahm_info));
    struct lahm_info *p_lahm_info = (struct lahm_info*)(buf + sizeof(struct classroom_info_hdr));
	PKG_UINT32(msg, p_hdr_info->count, l);
	int i = 0;
	for (i = 0; i < p_hdr_info->count; i++)
	{
	    PKG_UINT32(msg, p_lahm_info->petid, l);
	    PKG_UINT32(msg, p_lahm_info->nameid, l);
	    PKG_UINT32(msg, p_lahm_info->stage, l);
	    PKG_UINT32(msg, p_lahm_info->color, l);
	    PKG_UINT32(msg, p_lahm_info->skill, l);
	    PKG_UINT32(msg, p_lahm_info->sl_flag, l);
	    p_lahm_info++;
	}

	init_proto_head(msg, p->waitcmd, l);
	send_to_self(p, msg, l, 1);

	if(p_hdr_info->userid == GET_UID_CLASSROOM_MAP(p->tiles->id)) {
		p->tiles->mmdu[0].lahm_class.roomid = p_hdr_info->roomid;
		p->tiles->mmdu[0].lahm_class.energy = p_hdr_info->energy;
		p->tiles->mmdu[0].lahm_class.lovely = p_hdr_info->lovely;
		p->tiles->mmdu[0].lahm_class.graduate_all = p_hdr_info->graduate_all;
		p->tiles->mmdu[0].lahm_class.class_all = p_hdr_info->class_all;
		memcpy(p->tiles->mmdu[0].lahm_class.honors, p_hdr_info->honors, sizeof(p_hdr_info->honors));

		set_all_graduate_cnt_honor(p, p_hdr_info->graduate_all);

		msglog(statistic_logfile, 0x04040102,get_now_tv()->tv_sec, &(p->id), 4);

//		set_all_class_cnt_honor(p, p_hdr_info->class_all);
	}

    if (p_hdr_info->exam_flag > 0)
	{
	    notify_classroom_event_info(p, p_hdr_info->exam_flag);
	}

	return 0;

}


static int parse_level_single_student(student_info_t* iut, uint32_t* cnt, xmlNodePtr cur)
{
	int j = 0;
	while (cur)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Item")))
		{
			DECODE_XML_PROP_INT (iut[j].id, cur, "PetID");
			DECODE_XML_PROP_INT (iut[j].latent, cur, "Latent");
			DECODE_XML_PROP_INT (iut[j].moral, cur, "Moral");
			DECODE_XML_PROP_INT (iut[j].iq, cur, "IQ");
			DECODE_XML_PROP_INT (iut[j].sport, cur, "Sport");
			DECODE_XML_PROP_INT (iut[j].art, cur, "Art");
			DECODE_XML_PROP_INT (iut[j].labor, cur, "Labor");
			DECODE_XML_PROP_INT (iut[j].sl_flag, cur, "Flag");
			j++;
		}
		cur = cur->next;
	}

	*cnt = j;

	return 0;
}

static int parse_grade_students(students_info_t *git, uint32_t* cnt, xmlNodePtr chl)
{
	xmlNodePtr cur;
	int i = 0;
	int levelid = 0;

	while (chl)
	{
		if (!xmlStrcmp(chl->name, (const xmlChar *)"Level"))
		{
			DECODE_XML_PROP_INT(levelid, chl, "LevelID");
			git->level_students[levelid-1].level = levelid;

			cur = chl->xmlChildrenNode;
			if (parse_level_single_student(git->level_students[levelid-1].students, &(git->level_students[levelid-1].student_count), cur) != 0)
			{
				return -1;
			}

			i++;
		}
		chl = chl->next;
	}

	*cnt = i;
	return 0;

}

int load_lahm_students_conf(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int i, err = -1;
	int ex_count;

	ex_count = 0;
	memset(students_info, 0, sizeof (students_info));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Grade"))) {
			DECODE_XML_PROP_INT(i, cur, "GradeID");
			if (ex_count >= LAHM_CLASSROOM_GRADE_MAX - 1 || i <= 0) {
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, ex_count, i);
				goto exit;
			}
			students_info[i-1].grade = i;
			if (students_info[i-1].grade > LAHM_CLASSROOM_GRADE_MAX) {
				ERROR_LOG ("parse %s failed, Count=%d, grade=%d", file, ex_count, students_info[i-1].grade);
				goto exit;
			}

			chl = cur->xmlChildrenNode;
			if ( (parse_grade_students(&students_info[i-1], &(students_info[i-1].level_count), chl) != 0) )
				goto exit;

			ex_count++;
		}
		cur = cur->next;
	}

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load lahm students item file %s", file);

}

int query_all_students_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
    return send_request_to_db(SVR_PROTO_USER_CLASSROOM_GET_BASE_TEACH_INFO, p, 0, NULL, p->id);
}

int query_all_students_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 20);

    struct teacher_info_t{
        uint32_t	teach_exp;
        uint32_t	student_num;
        uint32_t	sl_student_num;
        uint32_t	exam_num;
        uint32_t	difficult;
	}__attribute__((packed));

	struct teacher_info_t *p_info = (struct teacher_info_t*)buf;

    int grade = 1;
    int level = rand()%6;
    if (level == 0)
    {
        level = 1;
    }

	float latent_rate = (rand() % 100)*0.01 + 1;
	float moral_rate = (rand() % 100)*0.01 + 1;
	float sport_rate = (rand() % 100)*0.01 + 1;
	float art_rate = (rand() % 100)*0.01 + 1;
	float labor_rate = (rand() % 100)*0.01 + 1;
	float iq_rate = (rand() % 100)*0.01 + 1;

	level_students_t level_info = {};
	memcpy(&level_info, &(students_info[grade-1].level_students[level-1]), sizeof(level_students_t));

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, p_info->teach_exp, l);
	PKG_UINT32(msg, p_info->student_num, l);
	PKG_UINT32(msg, p_info->sl_student_num, l);
	PKG_UINT32(msg, p_info->exam_num, l);
	PKG_UINT32(msg, p_info->difficult, l);
	PKG_UINT32(msg, level_info.student_count, l);

	int j = 0;
	for (j = 0; j < level_info.student_count; j++)
	{
	    p->students[j].id = (uint8_t)(level_info.students[j].id) ;
	    p->students[j].latent = (uint8_t)(level_info.students[j].latent * latent_rate) ;
	    p->students[j].moral = (uint8_t)(level_info.students[j].moral * moral_rate);
	    p->students[j].iq = (uint8_t)(level_info.students[j].iq * iq_rate);
	    p->students[j].sport = (uint8_t)(level_info.students[j].sport * sport_rate);
	    p->students[j].art = (uint8_t)(level_info.students[j].art * art_rate);
	    p->students[j].labor = (uint8_t)(level_info.students[j].labor * labor_rate);
	    p->students[j].sl_flag = (uint8_t)(level_info.students[j].sl_flag);

	    PKG_UINT32(msg, p->students[j].id, l);
        PKG_UINT32(msg, p->students[j].latent*2, l);
        PKG_UINT32(msg, p->students[j].moral, l);
        PKG_UINT32(msg, p->students[j].iq, l);
        PKG_UINT32(msg, p->students[j].sport, l);
        PKG_UINT32(msg, p->students[j].art, l);
        PKG_UINT32(msg, p->students[j].labor, l);
        PKG_UINT32(msg, p->students[j].sl_flag, l);

	}

	p->student_count = level_info.student_count;

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}


int add_lahm_students_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
    uint32_t petid = 0;
    uint32_t nameid = 0;
    uint32_t nature = 0;
    uint32_t mood = 1;
    uint32_t stage = 0;
    uint32_t color = 0;
    uint32_t skill = 0;
    int i = 0;
    uint32_t cnt = 0;
    CHECK_BODY_LEN_GE(len, 4);
	UNPKG_UINT32(body, cnt, i);

	int flag = 0;
	uint8_t dbbuf[2048] = {};
	int l = 0;
	PKG_H_UINT32(dbbuf,cnt,l);

	int j = 0;
	for (j = 0; j < cnt; j++)
	{
	    UNPKG_UINT32(body, petid, i);
	    UNPKG_UINT32(body, nameid, i);
	    UNPKG_UINT32(body, nature, i);
	    UNPKG_UINT32(body, stage, i);
	    UNPKG_UINT32(body, color, i);
	    UNPKG_UINT32(body, skill, i);

	    PKG_H_UINT32(dbbuf,petid,l);
	    PKG_H_UINT32(dbbuf,nameid,l);
	    PKG_H_UINT32(dbbuf,nature,l);
	    PKG_H_UINT32(dbbuf,mood,l);
	    PKG_H_UINT32(dbbuf,stage,l);
	    PKG_H_UINT32(dbbuf,color,l);
	    PKG_H_UINT32(dbbuf,skill,l);

        flag = 0;
	    int k = 0;
	    for (k = 0; k < p->student_count; k++)
	    {
	        if (p->students[k].id == petid)
	        {
	            PKG_H_UINT32(dbbuf,(uint32_t)p->students[k].latent*2,l);
	            PKG_H_UINT32(dbbuf,(uint32_t)p->students[k].moral,l);
	            PKG_H_UINT32(dbbuf,(uint32_t)p->students[k].iq,l);
	            PKG_H_UINT32(dbbuf,(uint32_t)p->students[k].sport,l);
	            PKG_H_UINT32(dbbuf,(uint32_t)p->students[k].art,l);
	            PKG_H_UINT32(dbbuf,(uint32_t)p->students[k].labor,l);
	            PKG_H_UINT32(dbbuf,(uint32_t)p->students[k].sl_flag,l);
	            flag = 1;
	            break ;
	        }
	    }

	    if (!flag)
	    {
            return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	    }

	}

    return send_request_to_db(SVR_PROTO_USER_LAMU_CLASSROOM_ADD_LAMUS, p, l, dbbuf, p->id);

}

int add_lahm_students_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 4);
	int i = 0;
	uint32_t lahm_num = 0;
	UNPKG_H_UINT32(buf,lahm_num,i);
	p->classroom_student = lahm_num;

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int del_lahm_students_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
    CHECK_BODY_LEN(len, 0);
    return send_request_to_db(SVR_PROTO_USER_CLASSROOM_ABANDON_LAMUS, p, 0, NULL, p->id);
}

int del_lahm_students_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 4);
	int i = 0;
	uint32_t student_num = 0;
	UNPKG_H_UINT32(buf,student_num,i);
	p->classroom_student = 0;

	response_proto_uint32(p, p->waitcmd, student_num, 0);
	return 0;
}

int get_lahm_teacher_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	uint32_t uid;

	CHECK_BODY_LEN(len, 4);
	UNPKG_UINT32(body,uid,i);

	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_GET_TEACHER_INFO, p, 0, NULL, uid);

}

int get_lahm_teacher_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    struct teacher_info_hdr {
        uint32_t	userid;
        uint32_t	exp;
        uint32_t	difficulty;
		uint32_t	s_evaluate;
		uint32_t	graduate_sum;
		uint32_t	outstand_sum;
		uint32_t	evaluate;
		uint32_t	class_sum;
		uint32_t	course_sum;
		uint32_t	count;
	}__attribute__((packed));

    struct course_info {
        uint32_t	course_id;
		uint32_t    num;
	}__attribute__((packed));

	struct teacher_info_hdr *p_hdr_info = (struct teacher_info_hdr*)buf;

	CHECK_BODY_LEN_GE(len,sizeof(struct teacher_info_hdr));
    int l = sizeof(protocol_t);;
    PKG_UINT32(msg, p_hdr_info->userid, l);
    PKG_UINT32(msg, p_hdr_info->exp, l);
    PKG_UINT32(msg, p_hdr_info->difficulty, l);
    PKG_UINT32(msg, p_hdr_info->s_evaluate, l);
    PKG_UINT32(msg, p_hdr_info->graduate_sum, l);
    PKG_UINT32(msg, p_hdr_info->outstand_sum, l);
    PKG_UINT32(msg, p_hdr_info->evaluate, l);
    PKG_UINT32(msg, p_hdr_info->class_sum, l);
    PKG_UINT32(msg, p_hdr_info->course_sum, l);
    PKG_UINT32(msg, p_hdr_info->count, l);

    CHECK_BODY_LEN(len,sizeof(struct teacher_info_hdr) + p_hdr_info->count*sizeof(struct course_info));
    struct course_info *p_info = (struct course_info*)(buf + sizeof(struct teacher_info_hdr));

	int i = 0;
	for (i = 0; i < p_hdr_info->count; i++)
	{
	    PKG_UINT32(msg, p_info->course_id, l);
	    PKG_UINT32(msg, p_info->num, l);
	    p_info++;
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}


int get_lahm_students_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	uint32_t uid;

	CHECK_BODY_LEN(len, 4);
	UNPKG_UINT32(body,uid,i);

	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_GET_LAMUS_INFO, p, 0, NULL, uid);

}

int get_lahm_students_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    struct students_info_hdr {
        uint32_t	userid;
        uint32_t	exam_time;
		uint32_t	count;
	}__attribute__((packed));

    struct course_info {
        uint32_t	course_id;
		uint32_t    lesson_degree;
	}__attribute__((packed));

	struct students_info {
        uint32_t	lamu_id;
		uint32_t    name_id;
		uint32_t	nature;
		uint32_t    mood;
		uint32_t	level;
		uint32_t    color;
		uint32_t	skill;
		uint32_t    latent;
		uint32_t	moral;
		uint32_t    intelligence;
		uint32_t	sport;
		uint32_t    art;
		uint32_t	labor;
		uint32_t    vip_flag;
		uint32_t    count;
		struct course_info s_course_info[20];
	}__attribute__((packed));

	struct students_info_hdr *p_hdr_info = (struct students_info_hdr*)buf;

	CHECK_BODY_LEN_GE(len, sizeof(struct students_info_hdr));
    int l = sizeof(protocol_t);;
    PKG_UINT32(msg, p_hdr_info->userid, l);
    PKG_UINT32(msg, p_hdr_info->exam_time, l);
    PKG_UINT32(msg, p_hdr_info->count, l);

    CHECK_BODY_LEN_GE(len, sizeof(struct students_info_hdr) + p_hdr_info->count*sizeof(struct students_info));
    struct students_info *p_students_info = (struct students_info*)(buf + sizeof(struct students_info_hdr));

	int i = 0;
	for (i = 0; i < p_hdr_info->count; i++)
	{
	    PKG_UINT32(msg, p_students_info->lamu_id, l);
	    PKG_UINT32(msg, p_students_info->name_id, l);
	    PKG_UINT32(msg, p_students_info->nature, l);
	    PKG_UINT32(msg, p_students_info->mood, l);
	    PKG_UINT32(msg, p_students_info->level, l);
	    PKG_UINT32(msg, p_students_info->color, l);
	    PKG_UINT32(msg, p_students_info->skill, l);
	    PKG_UINT32(msg, p_students_info->latent, l);
	    PKG_UINT32(msg, p_students_info->moral, l);
	    PKG_UINT32(msg, p_students_info->intelligence, l);
	    PKG_UINT32(msg, p_students_info->sport, l);
	    PKG_UINT32(msg, p_students_info->art, l);
	    PKG_UINT32(msg, p_students_info->labor, l);
	    PKG_UINT32(msg, p_students_info->vip_flag, l);
	    PKG_UINT32(msg, p_students_info->count, l);

        int j = 0;
	    for(j = 0; j < p_students_info->count; j++)
	    {
	        PKG_UINT32(msg, p_students_info->s_course_info[j].course_id, l);
	        PKG_UINT32(msg, p_students_info->s_course_info[j].lesson_degree, l);
	    }

	    p_students_info++;
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}

int check_lesson_condtion(sprite_t * p, uint32_t channel, uint32_t condtion)
{
	switch(channel){
	case 0:
		return 1;
	case 1:
		if(condtion <= p->teach_exp)
		{
		    return 1;
		}
	default:
		break;
	}

	return 0;
}


int get_teach_plan_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
    CHECK_BODY_LEN(len, 0);
	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_GET_TEACH_PLAN, p, 0, NULL, p->id);
}

int get_teach_plan_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    struct plan_info_hdr {
        uint32_t	today_left;
        uint32_t	term_left;
        uint32_t	course_cur;
	}__attribute__((packed));

	struct plan_info_hdr *p_hdr_info = (struct plan_info_hdr*)buf;

	CHECK_BODY_LEN_GE(len, sizeof(struct plan_info_hdr));
    int l = sizeof(protocol_t);;
    PKG_UINT32(msg, p_hdr_info->today_left, l);
    PKG_UINT32(msg, p_hdr_info->term_left, l);
    PKG_UINT32(msg, p_hdr_info->course_cur, l);

    int exam_num = p->exam_num;
	if (exam_num >=  LAHM_CLASSROOM_EXAM_NUM)
	{
	    exam_num = 0;
	}

	uint32_t isunlock = 0;
    uint32_t j = 0;
    PKG_UINT32(msg, lessons_info[exam_num].lesson_count, l);
	for (j = 0; j < lessons_info[exam_num].lesson_count; j++)
    {
        isunlock = check_lesson_condtion(p, lessons_info[exam_num].lessons[j].channel,
                    lessons_info[exam_num].lessons[j].condition);

        PKG_UINT32(msg,lessons_info[exam_num].lessons[j].id,l);
        PKG_UINT32(msg,lessons_info[exam_num].lessons[j].viponly,l);
        PKG_UINT32(msg,isunlock,l);
        PKG_UINT32(msg,lessons_info[exam_num].lessons[j].time,l);
        PKG_UINT32(msg,lessons_info[exam_num].lessons[j].class_hour,l);
        PKG_UINT32(msg,lessons_info[exam_num].lessons[j].type,l);
    }

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}

int set_classroom_class_begin_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_BODY_LEN(len, 4);
	int i = 0;
	uint32_t course_id = 0;
	uint32_t course_type = 0;
	UNPKG_UINT32(body,course_id,i);

	int tm_hour = get_now_tm()->tm_hour;
	if(tm_hour < 6)
	{
		return send_to_self_error(p, p->waitcmd, -ERR_lahm_classroom_closed, 1);
	}

	int exam_num = p->exam_num;
	if (exam_num >=  LAHM_CLASSROOM_EXAM_NUM)
	{
	    exam_num = 0;
	}

	int flag = 0;
	int course_time = 0;
	uint32_t channel = 0;
	uint32_t condition = 0;

    int j = 0;
    for (j = 0; j < lessons_info[exam_num].lesson_count; j++)
    {
        if (lessons_info[exam_num].lessons[j].id == course_id)
        {
            course_time = lessons_info[exam_num].lessons[j].time;
            course_type = lessons_info[exam_num].lessons[j].type;
            channel = lessons_info[exam_num].lessons[j].channel;
            condition = lessons_info[exam_num].lessons[j].condition;
            flag = 1;
            break ;
        }
    }

    if (!flag)
    {
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

    uint32_t isunlock = 0;
	isunlock = check_lesson_condtion(p, channel, condition);
	if (!isunlock)
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

    *(uint32_t*)p->session  = course_id;
    *(uint32_t*)(p->session + 4) = course_time;


	uint32_t dbbuf[2] = {};
	dbbuf[0] = course_id;
	dbbuf[1] = course_type;

	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_CLASS_BEGIN, p, sizeof(dbbuf), dbbuf, p->id);

}

int set_classroom_class_begin_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 8);
    uint32_t class_flag = 0;
    uint32_t event_count = 0;
    unpkg_host_uint32_uint32((uint8_t *)buf, &class_flag, &event_count);

    uint32_t course_id = 0;
    uint32_t course_time = 0;
    course_id = *(uint32_t*)p->session;
    course_time = *(uint32_t*)(p->session + 4);
    p->tiles->mmdu[0].lahm_class.course_id = course_id;

    int l = sizeof(protocol_t);
	PKG_UINT32(msg,course_id,l);
	PKG_UINT32(msg,course_time,l);
	PKG_UINT32(msg,class_flag,l);
	PKG_UINT32(msg,event_count,l);

	init_proto_head(msg,p->waitcmd,l);
	return send_to_self(p,msg,l,1);

}

int set_classroom_class_mode_cmd(sprite_t * p,const uint8_t * body,int len)
{
    CHECK_BODY_LEN(len, 0);
	uint32_t class_flag = 3;
	*(uint32_t*)p->session  = class_flag;
	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_SET_TEACH_MODE, p, sizeof(class_flag), &class_flag, p->id);

}

int set_classroom_class_mode_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    uint32_t class_flag = 0;
    class_flag = *(uint32_t*)p->session;
	response_proto_uint32(p, p->waitcmd,  class_flag, 0);
	return 0;
}

static int parse_single_lesson(lesson_info_t* iut, uint32_t* cnt, xmlNodePtr cur)
{
	int j = 0;
	while (cur)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Lesson")))
		{
			DECODE_XML_PROP_INT (iut[j].id, cur, "ID");
			DECODE_XML_PROP_INT_DEFAULT(iut[j].class_hour, cur, "Class_hour", 0);
			DECODE_XML_PROP_INT_DEFAULT(iut[j].time, cur, "Time", 0);
			DECODE_XML_PROP_INT_DEFAULT(iut[j].channel, cur, "Channel", 0);
			DECODE_XML_PROP_INT_DEFAULT(iut[j].condition, cur, "Condition", 0);
			DECODE_XML_PROP_INT_DEFAULT(iut[j].energy, cur, "Energy", 0);
			DECODE_XML_PROP_INT_DEFAULT(iut[j].lovely, cur, "Lovely", 0);
			DECODE_XML_PROP_INT_DEFAULT(iut[j].latent, cur, "Latent", 0);
			DECODE_XML_PROP_INT_DEFAULT (iut[j].moral, cur, "Moral", 0);
			DECODE_XML_PROP_INT_DEFAULT (iut[j].iq, cur, "IQ", 0);
			DECODE_XML_PROP_INT_DEFAULT (iut[j].sport, cur, "Sport", 0);
			DECODE_XML_PROP_INT_DEFAULT (iut[j].art, cur, "Art", 0);
			DECODE_XML_PROP_INT_DEFAULT (iut[j].labor, cur, "Labor", 0);
			DECODE_XML_PROP_INT_DEFAULT (iut[j].lesson_degree, cur, "Lesson_degree", 0);
			decode_xml_prop_arr_int_default ((int32_t *)iut[j].level_exp, 5, cur, "Level_exp", 0);
			decode_xml_prop_arr_int_default ((int32_t *)iut[j].attr, 2, cur, "Attr", 0);
			DECODE_XML_PROP_INT_DEFAULT (iut[j].type, cur, "Type", 0);
			DECODE_XML_PROP_INT_DEFAULT (iut[j].viponly, cur, "VipOnly", 0);

			j++;
		}
		cur = cur->next;
	}

	*cnt = j;

	return 0;
}


int load_lahm_lessons_conf(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int i, err = -1;
	int ex_count;

	ex_count = 0;
	memset(lessons_info, 0, sizeof (lessons_info));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Exam")))
		{
			DECODE_XML_PROP_INT(i, cur, "Num");
			if (ex_count >= LAHM_CLASSROOM_EXAM_NUM - 1 || i >= LAHM_CLASSROOM_EXAM_NUM)
			{
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, ex_count, i);
				goto exit;
			}

			lessons_info[i].exam_num = i;

			chl = cur->xmlChildrenNode;
			if ( (parse_single_lesson(lessons_info[i].lessons, &(lessons_info[i].lesson_count), chl) != 0) )
				goto exit;

			ex_count++;
		}
		cur = cur->next;
	}

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load lahm lessons item file %s", file);

}

int set_classroom_class_end_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_BODY_LEN(len, 4);
	int i = 0;
	uint32_t course_id = 0;
	UNPKG_UINT32(body,course_id,i);
	classroom_class_end_mgr(p, course_id, 1, 0);

	return 0;

}


int classroom_class_end_mgr(sprite_t * p, uint32_t course_id, uint32_t is_return, uint32_t end_flag)
{
	uint8_t dbbuf[1024] = {0};
	uint32_t l = 0;
	int exam_num = p->exam_num;
	if (exam_num >=  LAHM_CLASSROOM_EXAM_NUM)
	{
	    exam_num = 0;
	}

	lesson_info_t lesson_info = {0};

	int flag = 0;
    int i = 0;
    for (i = 0; i < lessons_info[exam_num].lesson_count; i++)
    {
        lesson_info = lessons_info[exam_num].lessons[i];
        if (lesson_info.id == course_id)
        {
            PKG_H_UINT32(dbbuf,lesson_info.id,l);
            PKG_H_UINT32(dbbuf,lesson_info.class_hour,l);
            PKG_H_UINT32(dbbuf,lesson_info.time,l);
            PKG_H_UINT32(dbbuf,lesson_info.energy,l);
            PKG_H_UINT32(dbbuf,lesson_info.lovely,l);
            PKG_H_UINT32(dbbuf,lesson_info.latent,l);
            PKG_H_UINT32(dbbuf,lesson_info.moral,l);
            PKG_H_UINT32(dbbuf,lesson_info.iq,l);
            PKG_H_UINT32(dbbuf,lesson_info.sport,l);
            PKG_H_UINT32(dbbuf,lesson_info.art,l);
            PKG_H_UINT32(dbbuf,lesson_info.labor,l);
            PKG_H_UINT32(dbbuf,lesson_info.lesson_degree,l);
            PKG_H_UINT32(dbbuf,lesson_info.level_exp[0],l);
            PKG_H_UINT32(dbbuf,lesson_info.level_exp[1],l);
            PKG_H_UINT32(dbbuf,lesson_info.level_exp[2],l);
            PKG_H_UINT32(dbbuf,lesson_info.level_exp[3],l);
            PKG_H_UINT32(dbbuf,lesson_info.level_exp[4],l);
            PKG_H_UINT32(dbbuf,lesson_info.attr[0],l);
            PKG_H_UINT32(dbbuf,lesson_info.attr[1],l);
            PKG_H_UINT32(dbbuf,lesson_info.type,l);

            flag = 1;

            uint32_t info_m[2] = {0};
            info_m[0] = p->id;
            info_m[1] = lesson_info.class_hour;
	        msglog(statistic_logfile, 0x02102001, now.tv_sec, info_m, sizeof(info_m));

            uint32_t info_msg = 1;
	        msglog(statistic_logfile, 0x02103200+lesson_info.id, now.tv_sec, &info_msg, sizeof(info_msg));

            break ;
        }
    }

    PKG_H_UINT32(dbbuf, end_flag, l);

    if (!flag)
    {
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	if (is_return)
	{
	    return send_request_to_db(SVR_PROTO_USER_CLASSROOM_CLASS_END, p, l, dbbuf, p->id);
	}
	else
	{
	    return send_request_to_db(SVR_PROTO_USER_CLASSROOM_CLASS_END, NULL, l, dbbuf, p->id);
	}

}



int set_classroom_class_end_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    if (p->waitcmd == PROTO_GET_USER_LAHM_CLASSROOM_INFO)
    {
        return send_request_to_db(SVR_PROTO_USER_CLASSROOM_GET_CLASS_INFO, p, p->sess_len, p->session, p->id);
    }

    struct class_end_info {
        uint32_t	course_id;
		uint32_t    class_flag;
		uint32_t    teach_exp;
		uint32_t    energy;
		uint32_t    lovely;
		uint32_t    exam_flag;
	}__attribute__((packed));

    CHECK_BODY_LEN(len, sizeof(struct class_end_info));
	struct class_end_info *p_info = (struct class_end_info*)buf;

	set_teach_profession_level_change(p, p_info->teach_exp);

	if (p->waitcmd == PROTO_SET_CLASSROOM_CLASS_END)
	{
	    int l = sizeof(protocol_t);
	    PKG_UINT32(msg, p_info->course_id, l);
	    PKG_UINT32(msg, p_info->class_flag, l);
        init_proto_head(msg, p->waitcmd, l);
        send_to_self(p, msg, l, 1);
    }
    else if (p->waitcmd == PROTO_USE_LAHM_CLASSROOM_ITEMS)
    {
        int itemid = 0;
        itemid = *(uint32_t*)p->session;
        int petid = *(uint32_t*)(p->session + 4);
        response_proto_uint32_uint32(p, p->waitcmd, itemid, petid, 0);
        notify_classroom_class_end(p, p_info->course_id, p_info->class_flag);
    }

    p->teach_exp= p_info->teach_exp;
    p->tiles->mmdu[0].lahm_class.energy = p_info->energy;
    p->tiles->mmdu[0].lahm_class.lovely = p_info->lovely;
    p->tiles->mmdu[0].lahm_class.course_id = 0;
    p->tiles->mmdu[0].lahm_class.class_all = p->tiles->mmdu[0].lahm_class.class_all + 1;
 //   int bitindex = get_class_cnt_honor(p->tiles->mmdu[0].lahm_class.class_all);
 //   set_lahm_classroom_honor_bit(p, bitindex);

    notify_classroom_teacher_info(p, p_info->teach_exp, p_info->energy, p_info->lovely);
    if (p_info->exam_flag > 0)
	{
	    notify_classroom_event_info(p, p_info->exam_flag);
	}

	return 0;

}

int notify_classroom_teacher_info(sprite_t* p, uint32_t exp, uint32_t energy, uint32_t lovely)
{
    uint8_t n_msg[128] = {0};
	int l = sizeof(protocol_t);
	PKG_UINT32(n_msg, exp, l);
	PKG_UINT32(n_msg, energy, l);
	PKG_UINT32(n_msg, lovely, l);

    init_proto_head(n_msg, PROTO_NOTIFY_CLASSROOM_TEACHER_INFO, l);
    return send_to_self(p, n_msg, l, 0);

}

int notify_classroom_class_end(sprite_t* p, uint32_t course_id, uint32_t class_flag)
{
    uint8_t n_msg[128] = {0};
	int l = sizeof(protocol_t);
	PKG_UINT32(n_msg, course_id, l);
	PKG_UINT32(n_msg, class_flag, l);

    init_proto_head(n_msg, PROTO_SET_CLASSROOM_CLASS_END, l);
    return send_to_self(p, n_msg, l, 0);

}


int notify_classroom_event_info(sprite_t* p, uint32_t eventid)
{
    uint8_t n_msg[128] = {0};
	int l = sizeof(protocol_t);
	PKG_UINT32(n_msg, eventid, l);

    init_proto_head(n_msg, PROTO_NOTIFY_CLASSROOM_EVENT_INFO, l);
    return send_to_self(p, n_msg, l, 0);

}


int get_classroom_course_level_cmd(sprite_t * p,const uint8_t * body,int len)
{
	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_GET_COURSE_NUM, p, 0, NULL, p->id);
}

int get_classroom_course_level_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    struct course_info_hdr {
		uint32_t	count;
	}__attribute__((packed));

    struct course_info {
        uint32_t	course_id;
		uint32_t    num;
	}__attribute__((packed));

	struct course_info_hdr *p_hdr_info = (struct course_info_hdr*)buf;

	CHECK_BODY_LEN_GE(len,sizeof(struct course_info_hdr));
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, p_hdr_info->count, l);

    CHECK_BODY_LEN(len,sizeof(struct course_info_hdr) + p_hdr_info->count*sizeof(struct course_info));
    struct course_info *p_info = (struct course_info*)(buf + sizeof(struct course_info_hdr));

	int i = 0;
	for (i = 0; i < p_hdr_info->count; i++)
	{
	    PKG_UINT32(msg, p_info->course_id, l);
	    PKG_UINT32(msg, p_info->num, l);
	    p_info++;
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}

int lahm_classroom_exam_cmd(sprite_t * p,const uint8_t * body,int len)
{
    uint32_t course_id = 0;
    int i = 0;
    uint32_t cnt = 0;
    CHECK_BODY_LEN_GE(len, 4);
	UNPKG_UINT32(body, cnt, i);
	CHECK_BODY_LEN(len, 4+cnt*4);

	if ((cnt > 6) || (cnt < 3))
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	int exam_num = p->exam_num;
	if (exam_num >=  LAHM_CLASSROOM_EXAM_NUM)
	{
	    exam_num = 0;
	}

    int32_t flag = 0;
	uint8_t dbbuf[1024] = {};
	int l = 0;
	PKG_H_UINT32(dbbuf,cnt,l);

	int j = 0;
	for (j = 0; j < cnt; j++)
	{
	    UNPKG_UINT32(body, course_id, i);

	    PKG_H_UINT32(dbbuf, course_id, l);
	    int k = 0;
        for (k = 0; k < lessons_info[exam_num].lesson_count; k++)
        {
            if (lessons_info[exam_num].lessons[k].id == course_id)
            {
                if (lessons_info[exam_num].lessons[k].type == 1)
                {
                    PKG_H_UINT32(dbbuf, lessons_info[exam_num].lessons[k].attr[0], l);
                    PKG_H_UINT32(dbbuf, lessons_info[exam_num].lessons[k].attr[1], l);
                    flag = 1;
                }
            }
        }

        if (!flag)
        {
            return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
        }

	}

    return send_request_to_db(SVR_PROTO_USER_LAMU_CLASSROOM_EXAM, p, l, dbbuf, p->id);

}

int lahm_classroom_exam_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 12);
	int i = 0;
	uint32_t evaluate = 0;
	uint32_t exam_times = 0;
	uint32_t teach_exp = 0;
	UNPKG_H_UINT32(buf,evaluate,i);
	UNPKG_H_UINT32(buf,exam_times,i);
	UNPKG_H_UINT32(buf,teach_exp,i);

	uint32_t info_m[] = {1};
	msglog(statistic_logfile, 0x02102001 + exam_times, now.tv_sec, info_m, sizeof(info_m));

    p->teach_exp = teach_exp;
	set_teach_profession_level_change(p, teach_exp);

	notify_classroom_teacher_info(p, teach_exp, p->tiles->mmdu[0].lahm_class.energy,
	    p->tiles->mmdu[0].lahm_class.lovely);

	if (exam_times < 1 || exam_times > 3)
	{
        ERROR_RETURN(("----exam_times:%d---evaluate:%d ", exam_times, evaluate), -1);
	}
    p->exam_num = exam_times;
	classroom_bonus_t lahm_bonus = classroom_bonus[exam_times-1];
	uint8_t buff[1024] = {};
	int j = 0;
	int k = 0;
	int count = 0;
	int count_low = 0;
	int count_high = 0;
    switch (evaluate)
    {
        case 1: //S
        {
            count_high = 3;
            count_low = 0;
	        break ;
        }
        case 2: //A
        {
            count_high = 2;
            count_low = 1;
	        break ;
        }
        case 3: //B
        {
            count_high = 1;
            count_low = 1;
	        break ;
        }
        case 4: //C
        {
            count_high = 0;
            count_low = 2;
	        break ;
        }
        case 5: //D
        {
            count_high = 0;
            count_low = 1;
	        break ;
        }
        default:
        {
            break ;
        }
    }

    count = count + count_high + count_low;
    if (ISVIP(p->flag))
	{
	    count = count + 1;
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session, count, p->sess_len);

    PKG_H_UINT32(buff, 0, j);
    PKG_H_UINT32(buff, count, j);
    PKG_H_UINT32(buff, 202, j);
    PKG_H_UINT32(buff, 0, j);
    for (k = 0; k < count_high; k++)
    {
        int index_v = rand()%(lahm_bonus.item_level[1].item_count);
        if (pkg_item_kind(p, buff, lahm_bonus.item_level[1].item[index_v].itm->id, &j) == -1)
        {
            return -1;
        }
        PKG_H_UINT32(buff, lahm_bonus.item_level[1].item[index_v].itm->id, j);
        PKG_H_UINT32(buff, lahm_bonus.item_level[1].item[index_v].count, j);
        PKG_H_UINT32(buff, lahm_bonus.item_level[1].item[index_v].itm->max, j);

        PKG_H_UINT32(p->session, lahm_bonus.item_level[1].item[index_v].itm->id, p->sess_len);
        PKG_H_UINT32(p->session, lahm_bonus.item_level[1].item[index_v].count, p->sess_len);
    }
    for (k = 0; k < count_low; k++)
    {
        int index_v = rand()%(lahm_bonus.item_level[0].item_count);
        if (pkg_item_kind(p, buff, lahm_bonus.item_level[0].item[index_v].itm->id, &j) == -1)
        {
            return -1;
        }
        PKG_H_UINT32(buff, lahm_bonus.item_level[0].item[index_v].itm->id, j);
        PKG_H_UINT32(buff, lahm_bonus.item_level[0].item[index_v].count, j);
        PKG_H_UINT32(buff, lahm_bonus.item_level[0].item[index_v].itm->max, j);

        PKG_H_UINT32(p->session, lahm_bonus.item_level[0].item[index_v].itm->id, p->sess_len);
        PKG_H_UINT32(p->session, lahm_bonus.item_level[0].item[index_v].count, p->sess_len);
    }
	if (ISVIP(p->flag))
    {
        for (k = 0; k < 1; k++)  //super lahm
    	{
    	    int index_v = rand()%(lahm_bonus.item_level[2].item_count);
    	    if (pkg_item_kind(p, buff, lahm_bonus.item_level[2].item[index_v].itm->id, &j) == -1)
    		{
    	        return -1;
    		}
    		PKG_H_UINT32(buff, lahm_bonus.item_level[2].item[index_v].itm->id, j);
    		PKG_H_UINT32(buff, lahm_bonus.item_level[2].item[index_v].count, j);
    		PKG_H_UINT32(buff, lahm_bonus.item_level[2].item[index_v].itm->max, j);

    		PKG_H_UINT32(p->session, lahm_bonus.item_level[2].item[index_v].itm->id, p->sess_len);
            PKG_H_UINT32(p->session, lahm_bonus.item_level[2].item[index_v].count, p->sess_len);
    	 }
	}

	send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buff, p->id);
	return 0;
}

int get_course_id_type(sprite_t * p, uint32_t course_id)
{
    int exam_num = p->exam_num;
	if (exam_num >=  LAHM_CLASSROOM_EXAM_NUM)
	{
	    exam_num = 0;
	}

    int j = 0;
    for (j = 0; j < lessons_info[exam_num].lesson_count; j++)
    {
        if (lessons_info[exam_num].lessons[j].id == course_id)
        {
            return  lessons_info[exam_num].lessons[j].type;
        }
    }

    return 0;

}


int get_lahm_teacher_exam_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
    CHECK_BODY_LEN(len, 4);
	int i = 0;
	uint32_t exam_times = 0;
	UNPKG_UINT32(body, exam_times, i);
	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_GET_EXAM_INFO, p, 4, &exam_times, p->id);

}

int get_lahm_teacher_exam_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    struct prize_item_info {
        uint32_t	itemid;
		uint32_t    cnt;
	}__attribute__((packed));

	struct course_info {
        uint32_t	course_id;
		uint32_t    score;
	}__attribute__((packed));

    struct teacher_exam_info_hdr {
        uint32_t	quality;
        uint32_t	score;
        uint32_t	difficulty;
		uint32_t	level_s;
		uint32_t	level_a;
		uint32_t	level_b;
		uint32_t	exp;
		uint32_t	exp_cur;
		uint32_t	evaluate;
		uint32_t	item_cnt;
		struct  prize_item_info s_item_info[10];
		uint32_t	lahm_count;
	}__attribute__((packed));

	struct lahm_exam_info {
        uint32_t	lahm_id;
        uint32_t	nameid;
        uint32_t	nature;
        uint32_t	stage;
        uint32_t	color;
        uint32_t	skill;
        uint32_t	total_score;
        uint32_t	course_count;
		struct     course_info  s_course_info[10];
	}__attribute__((packed));

	struct teacher_exam_info_hdr *p_hdr_info = (struct teacher_exam_info_hdr*)buf;

	CHECK_BODY_LEN_GE(len,sizeof(struct teacher_exam_info_hdr));
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, p_hdr_info->quality, l);
    PKG_UINT32(msg, p_hdr_info->score, l);
    PKG_UINT32(msg, p_hdr_info->difficulty, l);
    PKG_UINT32(msg, p_hdr_info->level_s, l);
    PKG_UINT32(msg, p_hdr_info->level_a, l);
    PKG_UINT32(msg, p_hdr_info->level_b, l);
    PKG_UINT32(msg, p_hdr_info->exp, l);
    PKG_UINT32(msg, p_hdr_info->exp_cur, l);
    PKG_UINT32(msg, p_hdr_info->evaluate, l);
    PKG_UINT32(msg, p_hdr_info->item_cnt, l);

    int j = 0;
    for (j = 0; j < p_hdr_info->item_cnt; j++)
	{
	    PKG_UINT32(msg, p_hdr_info->s_item_info[j].itemid, l);
	    PKG_UINT32(msg, p_hdr_info->s_item_info[j].cnt, l);
	}

    PKG_UINT32(msg, p_hdr_info->lahm_count, l);

    CHECK_BODY_LEN(len,sizeof(struct teacher_exam_info_hdr) + p_hdr_info->lahm_count*sizeof(struct lahm_exam_info));
    struct lahm_exam_info *p_lahm_info = (struct lahm_exam_info*)(buf + sizeof(struct teacher_exam_info_hdr));

	int i = 0;
	for (i = 0; i < p_hdr_info->lahm_count; i++)
	{
	    PKG_UINT32(msg, p_lahm_info->lahm_id, l);
	    PKG_UINT32(msg, p_lahm_info->nameid, l);
	    PKG_UINT32(msg, p_lahm_info->nature, l);
	    PKG_UINT32(msg, p_lahm_info->stage, l);
	    PKG_UINT32(msg, p_lahm_info->color, l);
	    PKG_UINT32(msg, p_lahm_info->skill, l);
	    PKG_UINT32(msg, p_lahm_info->total_score, l);
	    PKG_UINT32(msg, p_lahm_info->course_count, l);
	    int k = 0;
	    for (k = 0; k < p_lahm_info->course_count; k++)
	    {
	        PKG_UINT32(msg, p_lahm_info->s_course_info[k].course_id, l);
	        PKG_UINT32(msg, p_lahm_info->s_course_info[k].score, l);
	    }

	    p_lahm_info++;
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}

int get_lahm_classroom_count_cmd(sprite_t * p,const uint8_t * body,int len)
{
    int i = 0;
	uint32_t uid;

	CHECK_BODY_LEN(len, 4);
	UNPKG_UINT32(body,uid,i);

	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_GET_CLASSROOM_COUNT, p, 0, NULL, uid);
}

int get_lahm_classroom_count_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 8);

	int i = 0;
	uint32_t userid = 0;
	uint32_t count = 0;
	UNPKG_H_UINT32(buf,userid,i);
	UNPKG_H_UINT32(buf,count,i);

	int l = sizeof(protocol_t);
	PKG_UINT32(msg,userid,l);
	PKG_UINT32(msg,count,l);

	init_proto_head(msg,p->waitcmd,l);
	return send_to_self(p,msg,l,1);
}


int get_classroom_rand_userid_cmd(sprite_t * p,const uint8_t * body,int len)
{
	return send_request_to_db(SVR_PROTO_GET_RAND_USERID, p, 0, NULL, p->id);
}

int get_classroom_rand_userid_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 4);

	int i = 0;
	uint32_t userid = 0;
	UNPKG_H_UINT32(buf,userid,i);

	int l = sizeof(protocol_t);
	PKG_UINT32(msg,userid,l);

	init_proto_head(msg,p->waitcmd,l);
	return send_to_self(p,msg,l,1);
}


static int parse_lahm_classroom_single_item(item_unit_t* iut, uint32_t* cnt, xmlNodePtr cur)
{
	int j = 0;
	int id = 0;
	while (cur)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Item")))
		{
			if (j == MAX_ITEMS)
			{
				ERROR_RETURN(("too many items"), -1);
			}
			DECODE_XML_PROP_INT (id, cur, "ID");
			if (!(iut[j].itm = get_item_prop(id)))
			{
				ERROR_RETURN(("can't find item=%d", id), -1);
			}

			DECODE_XML_PROP_INT (iut[j].count, cur, "Count");
			j++;
		}
		cur = cur->next;
	}

	*cnt = j;
	return 0;
}

static int parse_lahm_classroom_levels(classroom_bonus_t *git, uint32_t* cnt, xmlNodePtr chl)
{
	xmlNodePtr cur;
	int i = 0;
	int level = 0;

	while (chl)
	{
		if (!xmlStrcmp(chl->name, (const xmlChar *)"Level"))
		{
			DECODE_XML_PROP_INT(level, chl, "LevelID");
			git->item_level[level-1].level = level;

			cur = chl->xmlChildrenNode;
			if (parse_lahm_classroom_single_item(git->item_level[level-1].item, &(git->item_level[level-1].item_count), cur) != 0)
			{
				return -1;
			}
			i++;
		}
		chl = chl->next;
	}

	*cnt = i;
	return 0;

}

int load_lahm_classroom_bonus(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int i, err = -1;
	int ex_count;

	ex_count = 0;
	memset(classroom_bonus, 0, sizeof (classroom_bonus));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Exam"))) {
			DECODE_XML_PROP_INT(i, cur, "ExamTime");
			if (ex_count >= CLASSROOM_BONUS_TABLE_SIZE - 1 || i <= 0) {
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, ex_count, i);
				goto exit;
			}
			classroom_bonus[i-1].exam_time = i;
			if (classroom_bonus[i-1].exam_time > LAHM_CLASSROOM_EXAM_NUM) {
				ERROR_LOG ("parse %s failed, Count=%d, exam_time=%d", file, ex_count, classroom_bonus[i-1].exam_time);
				goto exit;
			}

			chl = cur->xmlChildrenNode;
			if ( (parse_lahm_classroom_levels(&classroom_bonus[i-1], &(classroom_bonus[i-1].level_count), chl) != 0) )
				goto exit;

			ex_count++;
		}
		cur = cur->next;
	}

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load lahm classroom bonus item file %s", file);

}

int get_classroom_class_flag_cmd(sprite_t * p,const uint8_t * body,int len)
{
	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_GET_CLASS_FLAG, p, 0, NULL, p->id);
}

int get_classroom_class_flag_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 4);

	int i = 0;
	uint32_t class_flag = 0;
	UNPKG_H_UINT32(buf,class_flag,i);

	int l = sizeof(protocol_t);
	PKG_UINT32(msg,class_flag,l);

	init_proto_head(msg,p->waitcmd,l);
	return send_to_self(p,msg,l,1);
}

int lahm_classroom_class_graduate_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_BODY_LEN(len, 0);
	if(GET_UID_CLASSROOM_MAP(p->tiles->id) != p->id)
	{
		return send_to_self_error(p, p->waitcmd, -ERR_the_classroom_not_yours, 1);
	}
	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_CLASS_GRADUATE, p, 0, NULL, p->id);

}

int lahm_classroom_class_graduate_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    struct graduate_info_hdr {
        uint32_t	class_flag;
        uint32_t	time;
        uint32_t	cnt;
        uint32_t	great_cnt;
        uint32_t	great_all;
		uint32_t	count;
	}__attribute__((packed));

    struct direct_info {
        uint32_t	direct_id;
		uint32_t    direct_cnt;
		uint32_t    flag;
	}__attribute__((packed));

	struct graduate_info_hdr *p_hdr_info = (struct graduate_info_hdr*)buf;

	CHECK_BODY_LEN_GE(len,sizeof(struct graduate_info_hdr));
    p->exam_num = 0;

    p->tiles->mmdu[0].lahm_class.graduate_all = p->tiles->mmdu[0].lahm_class.graduate_all + p_hdr_info->cnt;
    int bit_index = get_graduate_cnt_honor(p->tiles->mmdu[0].lahm_class.graduate_all);
    set_lahm_classroom_honor_bit(p, bit_index);

    int l = sizeof(protocol_t);
    PKG_UINT32(msg, p_hdr_info->class_flag, l);
    PKG_UINT32(msg, p_hdr_info->time, l);
    PKG_UINT32(msg, p_hdr_info->cnt, l);
    PKG_UINT32(msg, p_hdr_info->great_cnt, l);
    PKG_UINT32(msg, p_hdr_info->count, l);

    send_request_to_db(SVR_PROTO_CLASSROOM_SET_OUTSTAND_SUM, NULL, 4, &(p_hdr_info->great_all), p->id);

    CHECK_BODY_LEN(len,sizeof(struct graduate_info_hdr) + p_hdr_info->count*sizeof(struct direct_info));
    struct direct_info *p_info = (struct direct_info*)(buf + sizeof(struct graduate_info_hdr));

	int i = 0;
	for (i = 0; i < p_hdr_info->count; i++)
	{
	    PKG_UINT32(msg, p_info->direct_id, l);
	    PKG_UINT32(msg, p_info->direct_cnt, l);
	    PKG_UINT32(msg, p_info->flag, l);
	    p_info++;
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}


int lahm_classroom_teach_remembrance_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	uint32_t uid = 0;
	CHECK_BODY_LEN(len, 4);
	UNPKG_UINT32(body,uid,i);

	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_GET_REMEMBRANCE, p, 0, NULL, uid);

}

int lahm_classroom_teach_remembrance_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    struct rem_info_hdr {
		uint32_t	count;
	}__attribute__((packed));

    struct rem_info {
        uint32_t	rem_id;
        uint32_t    time;
		uint32_t    rem_cnt;
	}__attribute__((packed));

	struct rem_info_hdr *p_hdr_info = (struct rem_info_hdr*)buf;

	CHECK_BODY_LEN_GE(len,sizeof(struct rem_info_hdr));
    int l = sizeof(protocol_t);;
    PKG_UINT32(msg, p_hdr_info->count, l);

    CHECK_BODY_LEN(len,sizeof(struct rem_info_hdr) + p_hdr_info->count*sizeof(struct rem_info));
    struct rem_info *p_info = (struct rem_info*)(buf + sizeof(struct rem_info_hdr));

	int i = 0;
	for (i = 0; i < p_hdr_info->count; i++)
	{
	    PKG_UINT32(msg, p_info->rem_id, l);
	    PKG_UINT32(msg, p_info->time, l);
	    PKG_UINT32(msg, p_info->rem_cnt, l);
	    p_info++;
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}

int lahm_classroom_get_items_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_BODY_LEN(len, 0);
	int j = 0;
	uint8_t buff[13];
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, 190750, j);
	PKG_H_UINT32(buff, 190800, j);
	PKG_UINT8(buff, 2, j);
	return send_request_to_db(SVR_PROTO_GET_ITEM, p, j, buff, p->id);

}

int lahm_classroom_use_item_cmd(sprite_t * p,const uint8_t * body,int len)
{
    int i = 0;
    int itemid = 0;
    int petid = 0;

	CHECK_BODY_LEN(len, 8);
	UNPKG_UINT32(body,itemid,i);
	UNPKG_UINT32(body,petid,i);

//	DEBUG_LOG("---petid:%d---", petid);

    if(!p->tiles || p->id != GET_UID_CLASSROOM_MAP(p->tiles->id))
    {
		return send_to_self_error(p, p->waitcmd, -ERR_the_classroom_not_yours, 1);
	}

	if (itemid < 190750 || itemid > 190800)
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	*(uint32_t*)p->session  = itemid;
	*(uint32_t*)(p->session + 4)  = petid;

	if (itemid == 190750 || itemid == 190751)
	{
	    uint32_t class_hour = 0;
	    if (itemid == 190750)
	    {
	        class_hour = 3;
	    }
	    else
	    {
	        class_hour = 10;
	    }

	    return send_request_to_db(SVR_PROTO_USER_CLASSROOM_CHECK_ADD_COURSE, p, 4, &class_hour, p->id);
	}
	else
	{
	    return db_single_item_op(p, p->id, itemid, 1, 0);
	 }


}

int lahm_classroom_check_add_course_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    int itemid = 0;
    itemid = *(uint32_t*)p->session;
    return db_single_item_op(p, p->id, itemid, 1, 0);
}


int lahm_classroom_use_item_callback(sprite_t * p, uint32_t itemid, uint32_t petid)
{
    uint32_t info_m[2] = {0};
    info_m[0] = 0;
    info_m[1] = 1;
	msglog(statistic_logfile, 0x02102100+itemid-190750, now.tv_sec, info_m, sizeof(info_m));

    uint32_t class_hour = 0;
    switch (itemid)
    {
        case 190750:
        {
            class_hour = 3;
            send_request_to_db(SVR_PROTO_USER_CLASSROOM_ADD_COURSE_CNT, NULL, 4, &class_hour, p->id);
            break ;
        }
        case 190751:
        {
            class_hour = 10;
            send_request_to_db(SVR_PROTO_USER_CLASSROOM_ADD_COURSE_CNT, NULL, 4, &class_hour, p->id);
            break ;
        }
        case 190752:
        {
            classroom_class_end_mgr(p, p->tiles->mmdu[0].lahm_class.course_id, 1, 1);
            break ;
        }
        case 190753:
        {
            change_teacher_attr_item(p, 1, 100);
            break ;
        }
        case 190754:
        {
            change_teacher_attr_item(p, 2, 100);
            break ;
        }
        case 190755:
        {

            uint32_t buff[2] = {0};
            buff[0] = petid;
            int val = rand()%10;
            if (val < 5)
            {
                buff[1] = 1;
            }
            else
            {
                buff[1] = 2;
            }
            send_request_to_db(SVR_PROTO_USER_CLASSROOM_SET_PET_MOOD, NULL, 8, buff, p->id);
            break ;

        }
        case 190756:
        {
            uint32_t buff[2] = {0};
            buff[0] = petid;
            buff[1] = rand()%5;
            if (buff[1] == 2)
            {
                buff[1] = 1;
            }
            send_request_to_db(SVR_PROTO_USER_CLASSROOM_SET_PET_MOOD, NULL, 8, buff, p->id);
            break ;
        }
        case 190757:
        {
            uint32_t buff[3] = {0};
            buff[0] = petid;
            buff[1] = 1;
            buff[2] = 100 + rand()%201;
            send_request_to_db(SVR_PROTO_USER_CLASSROOM_UPDATE_LAMU_ATTR, NULL, 12, buff, p->id);
            break ;
        }
        default:
        {
            break ;
        }

    }

    if (itemid != 190752)
    {
        response_proto_uint32_uint32(p, p->waitcmd, itemid, petid, 0);
    }

	return 0;
}

int calc_teach_exp_to_level(uint32_t exp)
{
    if (exp <= 2399)
    {
        return 1;
    }
    else if (exp <= 20249)
    {
        return 2;
    }
    else if (exp <= 71949)
    {
        return 3;
    }
    else if (exp <= 148649)
    {
        return 4;
    }
    else
    {
        return 5;
    }

}

int set_teach_profession_level_change(sprite_t * p, uint32_t exp_end)
{
    uint32_t info_m[2] = {0};
    info_m[0] = p->id;
    info_m[1] = exp_end;
	msglog(statistic_logfile, 0x02102000, now.tv_sec, info_m, sizeof(info_m));

	send_request_to_db(SVR_PROTO_CLASSROOM_SET_TEACHER_EXP, NULL, 4, &exp_end, p->id);

	uint32_t level_begin = p->profession[9];
	uint32_t level_end = calc_teach_exp_to_level(exp_end);
    if (level_end > level_begin)
    {
        uint32_t buff[2] = {0};
        buff[0] = 9;
        buff[1] = level_end;
        p->profession[buff[0]] = buff[1];
        db_set_profession(0, buff, p->id);
    }

    return 0;
}

int change_classroom_teacher_info(sprite_t* p, uint32_t type, uint32_t add_value)
{
    if (type == 1)
    {
        p->tiles->mmdu[0].lahm_class.energy = p->tiles->mmdu[0].lahm_class.energy + add_value;
        if (p->tiles->mmdu[0].lahm_class.energy > 1000)
        {
            p->tiles->mmdu[0].lahm_class.energy = 1000;
        }
    }
    else if (type == 2)
    {
        p->tiles->mmdu[0].lahm_class.lovely= p->tiles->mmdu[0].lahm_class.lovely + add_value;
        if (p->tiles->mmdu[0].lahm_class.lovely > 1000)
        {
            p->tiles->mmdu[0].lahm_class.lovely = 1000;
        }

    }

    return  0;

}

int change_teacher_attr_item(sprite_t * p, uint32_t type, uint32_t add_value)
{
    uint32_t buff[2] = {0};
    buff[0] = type;
    buff[1] = add_value;
    change_classroom_teacher_info(p, type, add_value);
    notify_classroom_teacher_info(p, p->teach_exp, p->tiles->mmdu[0].lahm_class.energy,
        p->tiles->mmdu[0].lahm_class.lovely);
    send_request_to_db(SVR_PROTO_USER_CLASSROOM_ADD_TEACHER_ATTR, NULL, 8, buff, p->id);
    return 0;
}

int get_friend_classroom_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	int count = 0;
	uint32_t buff[1024] = {0};

	if (IS_GUEST_ID (p->id) || IS_NPC_ID (p->id))
	{
		ERROR_RETURN(("not a register user\t[%u]", p->id), -1);
	}

	CHECK_BODY_LEN_GE(len, 4);

	int i = 0;
	UNPKG_UINT32(body, count, i);
	buff[0] = count;
	int cnt = 1;
	if (count > 250) {
		ERROR_RETURN(("too many users, count=%d", count), -1);
	}

	CHECK_BODY_LEN(len, count * 4 + 4);
	int j = 0;
	uint32_t uid = 0;
	for (; j != count; ++j)
	{
		UNPKG_UINT32(body, uid, i);
		if(!uid || IS_NPC_ID(uid) || IS_GUEST_ID(uid))
		{
		    continue;
		}

		buff[cnt++] = uid;
	}

	buff[0] = cnt - 1;
	len = (cnt - 1) * 4 + 4;

	return send_request_to_db(SVR_PROTO_GET_FRIEND_CLASSROOM_INFO, p, len, buff, p->id);

}

int get_friend_classroom_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct classroom_info{
			uint32_t	uid;
			uint32_t	teach_exp;
			uint32_t	great_student;
	} classroom_info_t;

	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + 12 * count);

	int loop = 0;
	int cnt = 0;
	int j = sizeof(protocol_t) + 4;
	classroom_info_t* p_info = (classroom_info_t*)(buf + 4);
	for (loop = 0; loop < count; loop++,p_info++)
	{
		if(p_info->uid != p->id && p_info->teach_exp == 0 && p_info->great_student == 0)
		{
			continue;
		}

		cnt++;
		PKG_UINT32(msg, p_info->uid, j);
		PKG_UINT32(msg, p_info->teach_exp, j);
		PKG_UINT32(msg, p_info->great_student, j);
	}
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cnt, l);
	j = sizeof(protocol_t) + 4 + 12 * cnt;
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int add_lahm_classroom_game_score_cmd(sprite_t * p,const uint8_t * body,int len)
{
    int j = 0;
    int score = 0;

    CHECK_BODY_LEN(len, 4);
    UNPKG_UINT32(body,score,j);

    int exam_num = p->exam_num;
	if (exam_num >=  LAHM_CLASSROOM_EXAM_NUM)
	{
	    exam_num = 0;
	}

	int course_id = 0;
	course_id = p->tiles->mmdu[0].lahm_class.course_id;
    int degree = 0;
    degree = score/10;
    if (degree > 80)
    {
        degree = 80;
    }

    if (course_id != 13)//only sport lesson
    {
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	uint32_t buff[2] = {0};
	buff[0] = course_id;
	buff[1] = degree;

	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_ADD_COURSE_SCORE, p, 8, buff, p->id);
}

int add_lahm_classroom_game_score_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    response_proto_head(p, p->waitcmd, 0);
    return 0;
}


static int parse_single_lesson_question(question_info_t* iut, uint32_t* cnt, xmlNodePtr cur)
{
	int j = 0;
	int q_id = 0;
	while (cur)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Question")))
		{
			DECODE_XML_PROP_INT (q_id, cur, "ID");
		    if (j >= 50 || q_id <= 0 || q_id > 50)
			{
				ERROR_RETURN(("error count=%d, questionid=%d ", j, q_id), -1);
			}

            iut[q_id-1].id = q_id;

			DECODE_XML_PROP_INT_DEFAULT(iut[q_id-1].answer, cur, "Answer", 0);
			DECODE_XML_PROP_INT_DEFAULT(iut[q_id-1].value, cur, "Value", 0);
			DECODE_XML_PROP_INT_DEFAULT(iut[q_id-1].lovely, cur, "Lovely", 0);
			DECODE_XML_PROP_INT_DEFAULT(iut[q_id-1].moral, cur, "Moral", 0);
			DECODE_XML_PROP_INT_DEFAULT(iut[q_id-1].iq, cur, "IQ", 0);
			DECODE_XML_PROP_INT_DEFAULT(iut[q_id-1].sport, cur, "Sport", 0);
			DECODE_XML_PROP_INT_DEFAULT(iut[q_id-1].art, cur, "Art", 0);
			DECODE_XML_PROP_INT_DEFAULT(iut[q_id-1].labor, cur, "Labor", 0);
			DECODE_XML_PROP_INT_DEFAULT(iut[q_id-1].latent, cur, "Latent", 0);
			j++;
		}
		cur = cur->next;
	}

	*cnt = j;

	return 0;
}


int load_lahm_lesson_questions_conf(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int i, err = -1;
	int ex_count;

	ex_count = 0;
	memset(questions_info, 0, sizeof (questions_info));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Lesson")))
		{
			DECODE_XML_PROP_INT(i, cur, "ID");
			if (ex_count >= LAHM_CLASSROOM_LESSON_NUM - 1 || i >= LAHM_CLASSROOM_LESSON_NUM || i <= 0)
			{
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, ex_count, i);
				goto exit;
			}

			questions_info[i-1].lesson_id = i;

			chl = cur->xmlChildrenNode;
			if ( (parse_single_lesson_question(questions_info[i-1].questions, &(questions_info[i-1].question_count), chl) != 0) )
				goto exit;

			ex_count++;
		}
		cur = cur->next;
	}

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load lahm lesson questions conf file %s", file);

}

int answer_lahm_classroom_lesson_question_cmd(sprite_t * p,const uint8_t * body,int len)
{
    int j = 0;
    int petid = 0;
    int questionid = 0;
    int user_answer = 0;

    CHECK_BODY_LEN(len, 12);
    UNPKG_UINT32(body,petid,j);
    UNPKG_UINT32(body,questionid,j);
    UNPKG_UINT32(body,user_answer,j);

    DEBUG_LOG("---petid=%d--questionid=%d--user_anwer=%d--", petid, questionid, user_answer);

	int course_id = p->tiles->mmdu[0].lahm_class.course_id;
    if (!course_id || questionid > 50 || questionid <= 0 || questions_info[course_id-1].questions[questionid-1].id != questionid)
    {
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	*(uint32_t*)p->session  = petid;
	*(uint32_t*)(p->session + 4)  = questionid;
	*(uint32_t*)(p->session + 8)  = user_answer;

	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_ADD_LESSON_EVENT, p, 4, &course_id, p->id);
}

int add_lahm_classroom_lesson_event_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    int petid = *(uint32_t*)p->session;
    int questionid = *(uint32_t*)(p->session + 4);
    int user_answer = *(uint32_t*)(p->session + 8);
    int course_id = p->tiles->mmdu[0].lahm_class.course_id;
    int type = get_course_id_type(p, course_id);
    lesson_questions_t* p_questions = &questions_info[course_id-1];
    int answer =  p_questions->questions[questionid-1].answer ;
    int result = 0;
    if (course_id == 14)
    {
        int32_t buff[8] = {0};
        buff[0] = petid;
        if (answer != user_answer)
        {
            result = 0;
            buff[1] = p_questions->questions[questionid-1].lovely*(-0.5);
            buff[2] = p_questions->questions[questionid-1].latent*(-0.5);
            buff[3] = p_questions->questions[questionid-1].moral*(-0.5);
            buff[4] = p_questions->questions[questionid-1].iq*(-0.5);
            buff[5] = p_questions->questions[questionid-1].sport*(-0.5);
            buff[6] = p_questions->questions[questionid-1].art*(-0.5);
            buff[7] = p_questions->questions[questionid-1].labor*(-0.5);

        }
        else
        {
            result = 1;
            buff[1] = p_questions->questions[questionid-1].lovely;
            buff[2] = p_questions->questions[questionid-1].latent;
            buff[3] = p_questions->questions[questionid-1].moral;
            buff[4] = p_questions->questions[questionid-1].iq;
            buff[5] = p_questions->questions[questionid-1].sport;
            buff[6] = p_questions->questions[questionid-1].art;
            buff[7] = p_questions->questions[questionid-1].labor;

        }

        int lovely = p->tiles->mmdu[0].lahm_class.lovely;
        lovely = lovely + buff[1];
        if (lovely < 0)
        {
            lovely = 0;
        }

        if (lovely > 1000)
        {
            lovely = 1000;
        }

        p->tiles->mmdu[0].lahm_class.lovely = lovely;
        notify_classroom_teacher_info(p, p->teach_exp, p->tiles->mmdu[0].lahm_class.energy, lovely);

        send_request_to_db(SVR_PROTO_USER_LAMU_CLASSROOM_ADD_LAMU_ATTRS, NULL, sizeof(buff), buff, p->id);

    }
    else
    {
        if (answer != user_answer)
        {
            result = 0;
        }
        else
        {
            result = 1;
            uint32_t buff[3] = {0};
	        buff[0] = petid;
	        if (type == 1)
	        {
	            buff[1] = 2; //lesson degree
	        }
	        else
	        {
	            buff[1] = 1; //pet latent
	        }
	        buff[2] = p_questions->questions[questionid-1].value;
	        send_request_to_db(SVR_PROTO_USER_CLASSROOM_UPDATE_LAMU_ATTR, NULL, 12, buff, p->id);
        }
    }

    response_proto_uint32_uint32_uint32(p, p->waitcmd, petid, type, result, 0);
    return 0;
}

int get_lahm_classroom_honor_list_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i=0;
	CHECK_BODY_LEN(len,0);

	if(!p->tiles )
	{
		return send_to_self_error(p, p->waitcmd, -ERR_not_resturant_type, 1);
	}

	int bytes = sizeof(protocol_t);
	PKG_UINT32(msg,5,bytes);//now only send first 5 honors
	for(i=1; i <= 5; i++) {
		PKG_UINT32(msg,i,bytes);
		if(test_bit_on(p->tiles->mmdu[0].lahm_class.honors,4,i)) {
			PKG_UINT32(msg,1,bytes);
		} else {
			PKG_UINT32(msg,0,bytes);
		}
	}

	init_proto_head(msg,p->waitcmd,bytes);
	return send_to_self(p,msg,bytes,1);
}

int  set_lahm_classroom_honor_bit(sprite_t * p,int bit)
{
	if (bit  <= 0)
	{
		return 0;
	}

    if(p->id != GET_UID_CLASSROOM_MAP(p->tiles->id))
    {
        return 0;
    }
	if(test_bit_on(p->tiles->mmdu[0].lahm_class.honors,4,bit))
	{
	    return 0;
    }

	uint32_t buff[4] = {0};
	buff[0] = bit;
	switch(bit) {
	case 1:
		p->yxb += 500;
		buff[1] = 500;
		buff[2] = 500;
		buff[3] = 0;
		p->teach_exp = p->teach_exp + 500;
		set_teach_profession_level_change(p, p->teach_exp);
		break;
	case 2:
	    p->yxb += 1000;
		buff[1] = 0;
		buff[2] = 1000;
		buff[3] = 0;
		break;
	case 3:
	    p->yxb += 2000;
		buff[1] = 2000;
		buff[2] = 2000;
		buff[3] = 0;
	    p->teach_exp = p->teach_exp + 2000;
		set_teach_profession_level_change(p, p->teach_exp);
		break;
	case 4:
		buff[1] = 4000;
		buff[2] = 0;
		buff[3] = 10;
	    p->teach_exp = p->teach_exp + 4000;
		set_teach_profession_level_change(p, p->teach_exp);
		break;
	case 5:
		buff[1] = 0;
		buff[2] = 0;
		buff[3] = 20;
		break;
	case 6:
		p->yxb += 500;
		buff[1] = 200;
		buff[2] = 500;
		buff[3] = 0;
		p->teach_exp = p->teach_exp + 200;
		set_teach_profession_level_change(p, p->teach_exp);
		break;
	case 7:
	    p->yxb += 1000;
		buff[1] = 600;
		buff[2] = 1000;
		buff[3] = 0;
		p->teach_exp = p->teach_exp + 600;
		set_teach_profession_level_change(p, p->teach_exp);
		break;
	case 8:
	    p->yxb += 2000;
		buff[1] = 0;
		buff[2] = 2000;
		buff[3] = 0;
		break;
	case 9:
		buff[1] = 3000;
		buff[2] = 0;
		buff[3] = 10;
	    p->teach_exp = p->teach_exp + 3000;
		set_teach_profession_level_change(p, p->teach_exp);
		break;
	case 10:
		buff[1] = 0;
		buff[2] = 0;
		buff[3] = 20;
		break;
	default:
	    {
	        return 0;
	    }
	}

	if (buff[1] > 0)
	{
	    notify_classroom_teacher_info(p, p->teach_exp, p->tiles->mmdu[0].lahm_class.energy,
            p->tiles->mmdu[0].lahm_class.lovely);
	}

	msglog(statistic_logfile, 0x02102200 + bit,get_now_tv()->tv_sec, &(p->id), 4);

	set_bit_on(p->tiles->mmdu[0].lahm_class.honors, 4, bit);
	//DEBUG_LOG("---bit=%d--", bit);
	send_request_to_db(SVR_PROTO_USER_CLASSROOM_SET_HONOR_BIT, NULL, 16, buff, p->id);

	int i =  sizeof(protocol_t);
	uint8_t p_msg[128] ={0};
	PKG_UINT32(p_msg,bit,i);
	init_proto_head(p_msg,PROTO_NOTIFY_LAHM_CLASSROOM_HONOR_ID,i);
	send_to_self(p, p_msg, i, 0);

	char txt[256];
    int txtlen;

#ifdef TW_VER
	switch(bit) {
	case 1:
		txtlen=snprintf(txt, sizeof txt, "恭喜你作爲拉姆小導師獲得了【諄諄教導】榮譽，給你500點導師經驗、500摩爾豆作爲獎勵。\n");
		break;
	case 2:
		txtlen=snprintf(txt, sizeof txt, "恭喜你作爲拉姆小導師獲得了【誨人不倦】榮譽，給你一套全新的教室裝潢、1000摩爾豆作爲獎勵。\n");
		break;
	case 3:
		txtlen=snprintf(txt, sizeof txt, "恭喜你作爲拉姆小導師獲得了【春風化雨】榮譽，給你2000點導師經驗、2000摩爾豆作爲獎勵。\n");
		break;
	case 4:
		txtlen=snprintf(txt, sizeof txt, "恭喜你作爲拉姆小導師獲得了【碩果滿堂】榮譽，給你4000點導師經驗、10點點豆作爲獎勵。\n");
		break;
	case 5:
		txtlen=snprintf(txt, sizeof txt, "恭喜你作爲拉姆小導師獲得了【桃李滿天下】榮譽，給你一套全新的教室裝潢、20點點豆作爲獎勵。\n");
		break;
	default:
	    {
	        break;
	    }
	}
#else
	switch(bit) {
	case 1:
		txtlen=snprintf(txt, sizeof txt, "恭喜你作为拉姆小导师获得了【谆谆教导】荣誉，给你500点导师经验、500摩尔豆作为奖励。\n");
		break;
	case 2:
		txtlen=snprintf(txt, sizeof txt, "恭喜你作为拉姆小导师获得了【诲人不倦】荣誉，给你一套全新的教室装潢、1000摩尔豆作为奖励。\n");
		break;
	case 3:
		txtlen=snprintf(txt, sizeof txt, "恭喜你作为拉姆小导师获得了【春风化雨】荣誉，给你2000点导师经验、2000摩尔豆作为奖励。\n");
		break;
	case 4:
		txtlen=snprintf(txt, sizeof txt, "恭喜你作为拉姆小导师获得了【硕果满堂】荣誉，给你4000点导师经验、10点点豆作为奖励。\n");
		break;
	case 5:
		txtlen=snprintf(txt, sizeof txt, "恭喜你作为拉姆小导师获得了【桃李满天下】荣誉，给你一套全新的教室装潢、20点点豆作为奖励。\n");
		break;
	default:
	    {
	        break;
	    }
	}
#endif

	notify_system_message(p, txt, txtlen);

	return 1;

}

int set_all_graduate_cnt_honor(sprite_t * p, uint32_t graduate_cnt)
{
    if (graduate_cnt >= 3)
    {
        set_lahm_classroom_honor_bit(p, 1);
    }
    if (graduate_cnt >= 10)
    {
        set_lahm_classroom_honor_bit(p, 2);
    }
    if (graduate_cnt >= 30)
    {
        set_lahm_classroom_honor_bit(p, 3);
    }

    if (graduate_cnt >= 60)
    {
        set_lahm_classroom_honor_bit(p, 4);
    }

    if (graduate_cnt >= 200)
    {
        set_lahm_classroom_honor_bit(p, 5);
    }

    return 0;
}

int set_all_class_cnt_honor(sprite_t * p, uint32_t class_cnt)
{
    if (class_cnt >= 20)
    {
        set_lahm_classroom_honor_bit(p, 6);
    }
    if (class_cnt >= 80)
    {
        set_lahm_classroom_honor_bit(p, 7);
    }
    if (class_cnt >= 200)
    {
        set_lahm_classroom_honor_bit(p, 8);
    }

    if (class_cnt >= 500)
    {
        set_lahm_classroom_honor_bit(p, 9);
    }

    if (class_cnt >= 2000)
    {
        set_lahm_classroom_honor_bit(p, 10);
    }

    return 0;
}



int get_graduate_cnt_honor(uint32_t graduate_cnt)
{
    if (graduate_cnt >= 200)
    {
        return 5;
    }
    else if (graduate_cnt >= 60)
    {
        return 4;
    }
    else if (graduate_cnt >= 30)
    {
        return 3;
    }
    else if (graduate_cnt >= 10)
    {
        return 2;
    }
    else if (graduate_cnt >= 3)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}

int get_class_cnt_honor(uint32_t class_cnt)
{
    if (class_cnt >= 2000)
    {
        return 10;
    }
    else if (class_cnt >= 500)
    {
        return 9;
    }
    else if (class_cnt >= 200)
    {
        return 8;
    }
    else if (class_cnt >= 80)
    {
        return 7;
    }
    else if (class_cnt >= 20)
    {
        return 6;
    }
    else
    {
        return 0;
    }

}


int check_inner_style_condtion(sprite_t * p, uint32_t channel, uint32_t condtion)
{
	switch(channel){
	case 0:
		break;
	case 1:
		return 1;
	case 2:
		break;
	case 3:
		if(test_bit_on(p->tiles->mmdu[0].lahm_class.honors,4,condtion))
		{
			return 1;
		}
		break;
	default:
		break;
	}

	return 0;
}


int get_classroom_inner_style_list_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i=0;
	uint32_t kind_id = 25;
	uint32_t kind_layer = 2;
	CHECK_BODY_LEN(len, 0);
	if(!p->tiles ) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	if(GET_UID_CLASSROOM_MAP(p->tiles->id) != p->id) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_classroom_not_yours, 1);
	}

	int enable = 0;
	int bytes = sizeof(protocol_t);
	item_kind_t *ik = get_item_kind(kind_id);

	int cnt = 0;
	int pos = bytes;
	PKG_UINT32(msg,ik->count,bytes);
	for(i=0; i < ik->count; i++)
	{
		if( ik->items[i].layer != kind_layer || ik->items[i].u.shop_tag.type != 32)
		{
			continue;
		}
		cnt++;
		enable = check_inner_style_condtion(p, ik->items[i].u.shop_tag.channel, ik->items[i].u.shop_tag.condition);
		PKG_UINT32(msg,ik->items[i].id,bytes);
		PKG_UINT32(msg,enable,bytes);

		//DEBUG_LOG("---enable=%d--itemid=%d", enable, ik->items[i].id);
	}

	PKG_UINT32(msg,cnt,pos);

	init_proto_head(msg,p->waitcmd,bytes);
	return send_to_self(p,msg,bytes,1);
}


int load_classroom_fellowship_conf(const char* file)
{
    fellowship_num = 0;
	int i, err = -1;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile(file);
	if (!doc) {
		ERROR_RETURN(("load lahm classroom fellowship config failed"), -1);
	}

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	i = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Fellowship"))){
			DECODE_XML_PROP_INT(fellowship_info[i].id, cur, "ID");

			if (i >= LAHM_CLASSROOM_FELLOWSHIP_NUM || fellowship_info[i].id <= 0 || fellowship_info[i].id > LAHM_CLASSROOM_FELLOWSHIP_NUM)
			{
				ERROR_RETURN(("error count=%d, id=%d ", i, fellowship_info[i].id), -1);
			}

			DECODE_XML_PROP_INT_DEFAULT(fellowship_info[i].lovely, cur, "Lovely", 0);
			DECODE_XML_PROP_INT_DEFAULT(fellowship_info[i].moral, cur, "Moral", 0);
			DECODE_XML_PROP_INT_DEFAULT(fellowship_info[i].iq, cur, "IQ", 0);
			DECODE_XML_PROP_INT_DEFAULT(fellowship_info[i].sport, cur, "Sport", 0);
			DECODE_XML_PROP_INT_DEFAULT(fellowship_info[i].art, cur, "Art", 0);
			DECODE_XML_PROP_INT_DEFAULT(fellowship_info[i].labor, cur, "Labor", 0);
			DECODE_XML_PROP_INT_DEFAULT(fellowship_info[i].latent, cur, "Latent", 0);
			fellowship_info[i].bonus_cnt = decode_xml_prop_arr_int_default(fellowship_info[i].itembonus, 10, cur, "ItemBonus", 0);
//			DEBUG_LOG("--bonus_cnt: %d--", fellowship_info[i].bonus_cnt);
			i++;
		}

		cur = cur->next;
	}

    fellowship_num = i;
    DEBUG_LOG("--fellowship_num: %d--", fellowship_num);

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load lahm classroom fellowship %s", file);
}

int on_lahm_classroom_fellowship_cmd(sprite_t * p,const uint8_t * body,int len)
{
    int j = 0;
    int userid = 0;
    CHECK_BODY_LEN(len, 4);
    uint32_t info_m[2] = {p->id, 1};
	msglog(statistic_logfile, 0x02102050, now.tv_sec, info_m, sizeof(info_m));

    UNPKG_UINT32(body,userid,j);
	return send_request_to_db(SVR_PROTO_USER_CLASSROOM_DO_FELLOWSHIP, p, 4, &userid, p->id);
}

int on_lahm_classroom_fellowship_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    int index = 0;
    index = rand()%fellowship_num;
    fellowship_info_t* p_fellowship = &fellowship_info[index];

    int32_t buff[8] = {0};
    buff[0] = 0;
    buff[1] = p_fellowship->lovely;
    buff[2] = p_fellowship->latent;
    buff[3] = p_fellowship->moral;
    buff[4] = p_fellowship->iq;
    buff[5] = p_fellowship->sport;
    buff[6] = p_fellowship->art;
    buff[7] = p_fellowship->labor;
    send_request_to_db(SVR_PROTO_USER_LAMU_CLASSROOM_ADD_LAMU_ATTRS, NULL, sizeof(buff), buff, p->id);

	uint8_t db_buff[1024] = {};
	int j = 0;
	int k = 0;
	PKG_H_UINT32(db_buff, 0, j);
	PKG_H_UINT32(db_buff, p_fellowship->bonus_cnt, j);
	PKG_H_UINT32(db_buff, 202, j);
	PKG_H_UINT32(db_buff, 0, j);
	for (k = 0; k < p_fellowship->bonus_cnt; k++)
	{
		if (pkg_item_kind(p, db_buff, p_fellowship->itembonus[k], &j) == -1)
		{
			return -1;
		}

		item_t* p_item = get_item_prop(p_fellowship->itembonus[k]);
		if (!p_item)
		{
		    return -1;
		}
		PKG_H_UINT32(db_buff, p_item->id, j);
		PKG_H_UINT32(db_buff, 1, j);
		PKG_H_UINT32(db_buff, p_item->max, j);
	}

	send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, j, db_buff, p->id);

    int lovely = p->tiles->mmdu[0].lahm_class.lovely;
    lovely = lovely + buff[1];
    if (lovely < 0)
    {
        lovely = 0;
    }
    if (lovely > 1000)
    {
        lovely = 1000;
    }

    p->tiles->mmdu[0].lahm_class.lovely = lovely;
    notify_classroom_teacher_info(p, p->teach_exp, p->tiles->mmdu[0].lahm_class.energy, lovely);

    response_proto_uint32(p, p->waitcmd, p_fellowship->id, 0);
    return 0;
}




