#include <statistic_agent/msglog.h>

#include "proto.h"
#include "exclu_things.h"
#include "message.h"
#include "communicator.h"
#include "school_bar_cmd.h"
#include "lahm_diary.h"

typedef struct directory_item {
	uint32_t 	diary_id;
	uint32_t	write_date;
	uint32_t	lock_state;
	char		title[31];
}__attribute__((packed)) directory_item_t;

typedef struct diary_content {
	uint32_t 	mood;
	userid_t	flower;
	char		lahm_surprise[61];
	char 		whisper[31];
	char		star[61];
	char 		ABC[145];
	char		content[601];
}__attribute__((packed)) diary_content_t;

typedef struct directory_list {
	uint32_t 			count;
	directory_item_t 	di[];
}__attribute__((packed)) directory_list_t;

/*
 *@ edit diary
 */
int edit_diary_cmd(sprite_t *p, uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 942);
	uint8_t school_bar_buf[1024] = { };
	uint8_t* pc = p->session;
	int i = 0;
	UNPKG_UINT32(body, *((uint32_t*)pc), i);     // diary id
	UNPKG_UINT32(body, *((uint32_t*)(pc + 4)), i); // mood
	UNPKG_UINT32(body, *((uint32_t*)(pc + 8)), i); // state

	pc += 12;
	UNPKG_STR(body, pc, i, MAX_DIARY_TITLE_LEN);  // diary title
	pc[MAX_DIARY_TITLE_LEN - 1] = '\0';
	CHECK_DIRTYWORD(p, pc);
	memcpy(school_bar_buf, pc, MAX_DIARY_TITLE_LEN);

	pc += MAX_DIARY_TITLE_LEN;
	UNPKG_STR(body, pc, i, MAX_LAHM_SURPRISE_LEN);  // lahm surprise
	pc[MAX_LAHM_SURPRISE_LEN - 1] = '\0';
	CHECK_DIRTYWORD(p, pc);

	pc += MAX_LAHM_SURPRISE_LEN;
	UNPKG_STR(body, pc, i, MAX_WHISPER_LEN);
	pc[MAX_WHISPER_LEN - 1] = '\0';
	CHECK_DIRTYWORD(p, pc);

	pc += MAX_WHISPER_LEN;
	UNPKG_STR(body, pc, i, MAX_STAR_LEN);
	pc[MAX_STAR_LEN - 1] = '\0';
	CHECK_DIRTYWORD(p, pc);

	pc += MAX_STAR_LEN;
	UNPKG_STR(body, pc, i, MAX_ABC_LEN);
	pc[MAX_ABC_LEN - 1] = '\0';
	CHECK_DIRTYWORD(p, pc);

	pc += MAX_ABC_LEN;
	UNPKG_STR(body, pc, i, MAX_CONTENT_LEN);
	pc[MAX_CONTENT_LEN - 1] = '\0';
	CHECK_DIRTYWORD(p, pc);
	memcpy(school_bar_buf + MAX_DIARY_TITLE_LEN, pc, MAX_CONTENT_LEN);
	uint32_t diary_id = *(uint32_t*)p->session;
	DEBUG_LOG("EDIT DIARY\t[%u %u %u]", p->id, diary_id, *(uint32_t*)(p->session + 4));
	if (diary_id) {
		return db_set_lahm_diary(p , p->session, p->id);
	} else {
		//send_to_school_bar(LAHM_DIARY_CMD, MAX_DIARY_TITLE_LEN + MAX_CONTENT_LEN, school_bar_buf, p->id);
		return db_set_sth_done(p, 141, 2, p->id);
	}
}

/*
 *@ leave message for sb, process db's reply
 */
int edit_diary_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	response_proto_uint32(p, p->waitcmd, *(uint32_t*)buf, 0);
	return 0;
}

/*
 *@ get diarys'  title
 */
int get_diarys_title_cmd(sprite_t *p, uint8_t *body, int bodylen)
{

	uint32_t buff[3];
	uint32_t uid;

	CHECK_BODY_LEN(bodylen, 12);
	int j = 0;
    UNPKG_UINT32(body, uid, j);
	buff[0] = p->id;
	UNPKG_UINT32(body, buff[1], j);  // start index
	UNPKG_UINT32(body, buff[2], j);  // end index
	if (buff[2] - buff[1] > 20)
		ERROR_RETURN (("error id=%u, start=%u end=%u", p->id, buff[1], buff[2]), -1);

	DEBUG_LOG("GET DIARY\t[%u %u %u %u]", p->id, uid, buff[1], buff[2]);

	return db_get_lahm_diarys_title(p, buff, uid);
}

/*
 *@ get diarys'  title  reply
 */
int get_diarys_title_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, sizeof(directory_list_t));

	directory_list_t* dl = (void*)buf;

	CHECK_BODY_LEN (len - sizeof(directory_list_t), sizeof(directory_item_t) * dl->count);
	if (dl->count > 20)
		ERROR_RETURN (("error id=%u, diary_num=%d", p->id, dl->count), -1);

	DEBUG_LOG("DB SEND TITLE\t[%u %u]", p->id, dl->count);
	int loop;
	for (loop = 0; loop < dl->count; loop++) {
		DEBUG_LOG("id=%u diary_id=%u write_date=%u lock=%u tilte=%s", p->id,
			dl->di[loop].diary_id,
			dl->di[loop].write_date,
			dl->di[loop].lock_state,
			dl->di[loop].title);
		dl->di[loop].diary_id    = htonl(dl->di[loop].diary_id);
		dl->di[loop].write_date  = htonl(dl->di[loop].write_date);
		dl->di[loop].lock_state  = htonl(dl->di[loop].lock_state);
	}

	dl->count = htonl(dl->count);

	response_proto_str (p, p->waitcmd, len, buf, 0);
	return 0;
}

/*
 *@ delete diary
 */
int delete_diary_cmd(sprite_t *p, uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t diary_id;

	int i = 0;
	UNPKG_UINT32(body, diary_id, i);

	DEBUG_LOG("DELETE DIARY\t[%u %u]", p->id, diary_id);
	return db_del_lahm_diary(p, &diary_id, p->id);
}

/*
 *@ delete diary callback
 */
int delete_diary_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/*
 *@ set diary lock
 */
int set_diary_lock_state_cmd(sprite_t *p, uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);

	uint32_t buff[2];
	int i = 0;
	UNPKG_UINT32(body, buff[0], i);  // diary id
	UNPKG_UINT32(body, buff[1], i);  // lock state
	if (buff[1] != 1 && buff[1] != 2) {
		ERROR_RETURN(("bad lock state\t[%u %u %u]", p->id, buff[0], buff[1]), -1);
	}

	DEBUG_LOG("SET LOCK STATE\t[%u %u %u]", p->id, buff[0], buff[1]);
	return db_set_diary_lock(p, buff, p->id);
}

/*
 *@ set diary lock callback
 */
int set_diary_lock_state_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/*
 *@ send diary flower
 */
int send_diary_flower_cmd(sprite_t *p, uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);

	uint32_t uid;
	uint32_t diary_id;
	int j = 0;
    UNPKG_UINT32(body, uid, j);
	UNPKG_UINT32(body, diary_id, j);
	if (uid == p->id)
		ERROR_RETURN(("can't send flower to your self\t[%u %u %u]", p->id, uid, diary_id), -1);

	DEBUG_LOG("SEND DIARY FLOWER\t[%u %u %u]", p->id, uid, diary_id);
	*(uint32_t*)p->session = diary_id;
	*(uint32_t*)(p->session + 4) = uid;

	return db_set_sth_done(p, 145, 1, p->id);
}

/*
 *@ send flower diary callback
 */
int send_diary_flower_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}
/*
 *@ send diary flower
 */
int get_diary_content_cmd(sprite_t *p, uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);

	uint32_t uid;
	uint32_t diary_id;
	int j = 0;
    UNPKG_UINT32(body, uid, j);
	UNPKG_UINT32(body, diary_id, j);
	*(uint32_t*)p->session = diary_id;

	uint32_t buff[2];
	buff[0] = diary_id;
	buff[1] = p->id;
	DEBUG_LOG("GET DIARY CONTENT\t[%u %u %u]", p->id, uid, diary_id);
	return db_get_diary_content(p, buff, uid);
}

/*
 *@ send flower diary callback
 */
int get_diary_content_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 677);

	diary_content_t* dc = (diary_content_t*)buf;

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, *(uint32_t*)p->session, j);
	PKG_UINT32(msg, dc->mood, j);
	PKG_UINT32(msg, dc->flower, j);
	PKG_STR(msg, dc->lahm_surprise, j, MAX_LAHM_SURPRISE_LEN);
	PKG_STR(msg, dc->whisper, j, MAX_WHISPER_LEN);
	PKG_STR(msg, dc->star, j, MAX_STAR_LEN);
	PKG_STR(msg, dc->ABC, j, MAX_ABC_LEN);
	PKG_STR(msg, dc->content, j, MAX_CONTENT_LEN);

	DEBUG_LOG("GET DIARY CONTENT CALLBACK\t[%u %u %u]", p->id, *(uint32_t*)p->session, dc->mood);
	init_proto_head(msg ,p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

/*
 *@ set big lock
 */
int set_big_lock_cmd(sprite_t *p, uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	uint32_t big_lock;
	int j = 0;
	UNPKG_UINT32(body, big_lock, j);

	DEBUG_LOG("SET BIG LOCk\t[%u %u]", p->id, big_lock);
	return db_set_big_lock(p, &big_lock, p->id);
}

/*
 *@ send big lock callback
 */
int set_big_lock_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/*
 *@ get big lock
 */
int get_big_lock_cmd(sprite_t *p, uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	uint32_t uid;
	int j = 0;
	UNPKG_UINT32(body, uid, j);

	DEBUG_LOG("GET BIG LOCK\t[%u %u]", p->id, uid);
	return db_get_big_lock(p, uid);
}

/*
 *@ send big lock callback
 */
int get_big_lock_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	uint32_t big_lock = *(uint32_t*)buf;
	response_proto_uint32(p, p->waitcmd, big_lock, 0);
	return 0;
}

/*
 *@ get big lock
 */
int get_diary_num_cmd(sprite_t *p, uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	uint32_t uid;
	int j = 0;
	UNPKG_UINT32(body, uid, j);
	CHECK_VALID_ID(uid);

	DEBUG_LOG("GET DIARY NUM\t[%u %u]", p->id, uid);
	return db_get_diary_num(p, uid);
}

/*
 *@ send big lock callback
 */
int get_diary_num_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	uint32_t num = *(uint32_t*)buf;

	DEBUG_LOG("GET DIARY NUM CALLBACK\t[%u %u]", p->id, num);
	response_proto_uint32(p, p->waitcmd, num, 0);
	return 0;
}

