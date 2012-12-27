#ifndef ONLINE_PET_H_
#define ONLINE_PET_H_

// C99
#include <stdint.h>
// self-define
#include "benchapi.h"
#include "sprite.h"
#include <libtaomee/timer.h>
#include "util.h"

#define	SVR_PROTO_GET_PET_ITEM_INFO		0x1006

#define	SVR_PROTO_GET_PET		0x1050
#define	SVR_PROTO_GET_ONE_PET   0x106B

#define	SVR_PROTO_ADD_PET		0x1151
//#define	SVR_PROTO_SET_PET		0x1152
#define	SVR_PROTO_SET_PET2		0x1153
#define	SVR_PROTO_SET_PETS_POS	0x1154
#define SVR_PROTO_DEL_PET		0x1155
#define SVR_PROTO_SET_PET_SKILL	0x1157

#define SVR_PROTO_SET_PET_FLAG		0x11D0
#define SVR_PROTO_SET_PET_FLAG2		0x11D1
#define SVR_PROTO_SET_PET_BIRTH		0x11D2
#define SVR_PROTO_SET_PET_NICK		0x11D3
#define SVR_PROTO_SET_PET_COLOR		0x11D4
#define SVR_PROTO_SET_PET_SICKTIME	0x11D5
#define SVR_PROTO_SET_PET_END_TIME	0x11D6
#define SVR_PROTO_UPDATE_PET_ATTR	0x11D7

#define SVR_PROTO_SET_FLAG_ENDTIME	0x11DB
#define SVR_PROTO_FEED_PET			0x11D8
#define SVR_PROTO_TRUST_PET			0x11D9
#define SVR_PROTO_PET_TASK			0x11DA
#define SVR_PROTO_GET_PET_TASK_ATTIRE_INFO			0x11E5
#define SVR_PROTO_SET_PET_SKILL_EX	0x11EA
#define SVR_PROTO_SET_PET_SKILL_TYPE	0x3156
#define SVR_PROTO_SET_PET_HOT_SKILL  0x3169

#define SVR_PROTO_GET_LAHM_TASK_STATE_EX	0x3053
#define SVR_PROTO_SET_LAHM_TASK_STATE_EX	0x3152
#define SVR_PROTO_GET_LAHM_TASK_DATA_EX		0x3055
#define SVR_PROTO_SET_LAHM_TASK_DATA_EX		0x3154
#define SVR_PROTO_LAHM_BUILD_TEMPLE         0xB570
#define SVR_PROTO_GET_USER_DATA				0xB469
#define SVR_PROTO_SET_USER_DATA				0xB56A
#define SVR_PROTO_GET_CLIENT_OBJ				0xB481
#define SVR_PROTO_SET_CLIENT_OBJ				0xB580

#define PET_NICK_LEN		16
#define PET_CLOTH_MAX_CNT	1
#define PET_HONOR_MAX_CNT	1
#define PET_TASK_MAX_CNT	20


enum pet_arg {
	PET_FLAG	= 0,
	PET_FLAG2,
	PET_BIRTH,
	PET_COLOR,
	PET_SICKTIME,
	PET_ENDTIME,
	PET_STAMP,
	PET_SKILL_FLAG
};

typedef struct task_info {
	uint8_t	taskid;
	uint8_t	task_flag;
} __attribute__ ((packed)) task_info_t;

//pet task info
typedef struct PetTaskInfo {
	uint8_t	count;       // user of the pet
	task_info_t	taskinfo[PET_TASK_MAX_CNT];
} __attribute__ ((packed)) pet_task_info_t;

// supplemental info
typedef struct PetSupplInfo {
	userid_t	uid;       // user of the pet
	uint32_t	flag;
	uint32_t	cur_form;  // current formation of the pet
	map_id_t	mapid;     // map where the pet is located currently

	uint8_t		pet_cloth_cnt;
	uint32_t	pet_clothes[PET_CLOTH_MAX_CNT];
	uint8_t		pet_honor_cnt;
	uint32_t	pet_honor[PET_HONOR_MAX_CNT];
	pet_task_info_t		pti;
} __attribute__ ((packed)) pet_suppl_info_t;

typedef struct pet {
	uint32_t	id;
	uint32_t	flag;
	uint32_t	flag2;
	uint32_t	birth;
	uint32_t	logic_birthday;
	char		nick[PET_NICK_LEN];
	uint32_t	color;
	uint32_t	sicktime; // indicate sick time if is sick or dead time if is dead
	/*the head 40 byte can not change*/
	uint8_t		posX;
	uint8_t		posY;

	uint8_t		hungry;
	uint8_t		thirsty;
	uint8_t		sanitary;
	uint8_t		spirit;
	//time_t		trust_end_tm; // indicate trust/task end time
	//time_t		start_tm; //  indicate task start time
	uint32_t	trust_end_tm; // indicate trust/task end time
	uint32_t	start_tm; //  indicate task start time
	uint32_t	taskid;   //  task id
	uint32_t	skill;
	//time_t		stamp;
	uint32_t	stamp;
	uint32_t	sick_type;
	uint32_t	skill_ex[3];//0 :fire, 1:water, 2:wood
	uint32_t	skill_bits; //skill selecter
	uint32_t	skill_value;//skill points
	uint32_t	skill_hot[3];//hot skill
	uint32_t	bit_flag;//sth done flag
	pet_suppl_info_t	suppl_info;
} __attribute__ ((packed)) pet_t;

typedef struct PetPos {
	uint32_t id;
	uint8_t	x;
	uint8_t y;
} __attribute__((packed)) pet_pos_t;

enum {
	PET_GO_HOME	= 0,
	PET_OUT	= 1
};

enum {
	PET_STATUS_NORMAL			= 0,
	PET_STATUS_SICKY			= 1,
	PET_STATUS_DEAD			= 2,
	PET_STATUS_TRUSTED		= 4,
	PET_STATUS_ON_TASK		= 8,
	PET_STATUS_GO_OUT			= 16,
	PET_STATUS_VISIT_OTHER	= 32,
	PET_STATUS_EMPLOYED		= 64,
	PET_STATUS_MAX
};

#define PET_NATURAL(pet_)		!((pet_)->flag)
#define PET_SICK(pet_)			!!((pet_)->flag & PET_STATUS_SICKY)
#define PET_DEAD(pet_)			!!((pet_)->flag & PET_STATUS_DEAD)
#define PET_TRUSTED(pet_)		!!((pet_)->flag & PET_STATUS_TRUSTED)
#define PET_ON_TASK(pet_)		!!((pet_)->flag & PET_STATUS_ON_TASK)
#define PET_GO_OUT(pet_)		!!((pet_)->flag & PET_STATUS_GO_OUT)
#define PET_VISIT(pet_)		!!((pet_)->flag & PET_STATUS_VISIT_OTHER)
#define PET_EMPLOYED(pet_)		!!((pet_)->flag & PET_STATUS_EMPLOYED)

#define SET_PET_SICK(pet_)			((pet_)->flag = PET_STATUS_SICKY)
#define SET_PET_DEAD(pet_)			((pet_)->flag = PET_STATUS_DEAD)
#define SET_PET_TRUSTED(pet_)		((pet_)->flag = PET_STATUS_TRUSTED)
#define SET_PET_ON_TASK(pet_)		((pet_)->flag = PET_STATUS_ON_TASK)
#define SET_PET_GO_OUT(pet_)		((pet_)->flag = PET_STATUS_GO_OUT)
#define SET_PET_VISIT_OTHER(pet_)	((pet_)->flag = PET_STATUS_VISIT_OTHER)
#define SET_PET_EMPLOYED(pet_)	((pet_)->flag = PET_STATUS_EMPLOYED)

#define RESET_PET_SICK(pet_)		((pet_)->flag &= ~PET_STATUS_SICKY)
#define RESET_PET_DEAD(pet_)		((pet_)->flag &= ~PET_STATUS_DEAD)
#define RESET_PET_TRUSTED(pet_)	((pet_)->flag &= ~PET_STATUS_TRUSTED)
#define RESET_PET_ON_TASK(pet_)	((pet_)->flag &= ~PET_STATUS_ON_TASK)
#define RESET_PET_GO_OUT(pet_)	((pet_)->flag &= ~PET_STATUS_GO_OUT)
#define RESET_PET_VISIT(pet_)		((pet_)->flag &= ~PET_STATUS_VISIT_OTHER)
#define RESET_PET_EMPLOYED(pet_)	((pet_)->flag &= ~PET_STATUS_EMPLOYED)

#define PET_GOOD_MOOD(pet_)		((pet_)->sprite > 69)
#define PET_NORMAL_MOOD(pet_)		((pet_)->sprite > 30)
#define PET_BAD_MOOD(pet_)			((pet_)->sprite > 0)

enum {
	PALASEN_SICK		= 0,
	FEIWUTANGYAN_SICK	= 1,
	PIYANGYANG_SICK		= 2,
	PET_SICK_MAX		= 3
};

static inline void infect_pet(pet_t* pet, int sicktype)
{
	int idx = sicktype * 2;
	pet->sick_type |= 1 << idx;
	pet->sick_type &= ~(1 << (idx + 1));
}

static inline void cure_pet(pet_t* pet, int sicktype)
{
	int idx = sicktype * 2;
	pet->sick_type &= ~(1 << idx);
	pet->sick_type |= 1 << (idx + 1);
}

static inline int is_pet_infected(pet_t* pet, int sicktype)
{
	int idx = sicktype * 2;
	if (pet->sick_type & (1 << idx))
		return 1;

	return 0;
}

// Super Lahm
enum {
	PET_SUPER_LAHM		= 1,
};

enum {
	PET_FORM_CHANGED_BAT	= 1
};

#define PET_IS_SUPER_LAHM(pet_) \
		!!((pet_)->flag2 & PET_SUPER_LAHM)
#define PET_SET_SUPER_LAHM(pet_) \
		((pet_)->flag2 |= PET_SUPER_LAHM)
#define PET_RESET_SUPER_LAHM(pet_) \
		((pet_)->flag2 &= ~PET_SUPER_LAHM)

//
pet_t*  get_pet(userid_t uid, uint32_t petid);
// calc level of the given @pet
uint8_t get_pet_level(const pet_t* pet);

//
int  calc_pet_attr_full(uint32_t id, pet_t* pet, int hungry, int thirsty, int sanitary, int spirit, int nosick);
#define calc_pet_attr(id_, pet_, hungry_, thirsty_, sanitary_, spirit_) \
		calc_pet_attr_full((id_), (pet_), (hungry_), (thirsty_), (sanitary_), (spirit_), 0)
void cancel_super_lahm(const sprite_t* p, pet_t* pet, int cnt);
void set_all_pets_attr(uint32_t mapid);

// callbacks for dealing with DB return
int add_pet_to_db(sprite_t *p, pet_t *pet);
int add_pet_return(sprite_t* p, uint32_t id, char* buf, int len);
int feed_pet_callback(sprite_t* p);
int get_pet_return(sprite_t* p, uint32_t id, char* buf, int len);
int set_pet_return(sprite_t* p, uint32_t id, char* buf, int len);
int set_pet2_return(sprite_t* p, uint32_t id, char* buf, int len);
int set_pets_pos_return(sprite_t* p, uint32_t id, char* buf, int len);
int set_pet_skill_callback(sprite_t* p, uint32_t id, char* buf, int len);

//DB-related funcs/macros
#define db_add_pet(p, pet) \
		add_pet_to_db(p,pet)
#define db_set_pet_skill(p_, pkg_) \
		send_request_to_db(SVR_PROTO_SET_PET_SKILL, (p_), 8, (pkg_), (p_)->id)
/*
	this protocol is now not used.
	and the data send to db is error format after the struct of pet_t has been changed.
	if you want to use it need re-coding it.
#define db_set_pet(p, pet, id) \
		send_request_to_db(SVR_PROTO_SET_PET, p, 40, pet, id)
*/
#define db_get_pet_and_item_info(p, id) \
		send_request_to_db(SVR_PROTO_GET_PET_ITEM_INFO, p, 0, NULL, id)

#define db_set_pet_nick(p, buf, id) \
		send_request_to_db(SVR_PROTO_SET_PET_NICK, p, 20, buf, id)
#define db_get_one_pet(p, petid, id) \
		send_request_to_db(SVR_PROTO_GET_ONE_PET, p, 4, &petid, id)

//
int db_pet_op(sprite_t* p, const pet_t *pet, uint32_t userid);
int db_set_flag_endtime(sprite_t* p, uint32_t flag, uint32_t endtime, uint32_t petid, userid_t uid);
int db_update_pet_attr(sprite_t * p, uint32_t stamp, uint32_t hungry, uint32_t thirsty, uint32_t sanitary, uint32_t spirit, uint32_t petid, userid_t uid);
int db_feed_pet(sprite_t* p, uint32_t flag, uint32_t sicktime, uint32_t stamp, uint32_t hungry, uint32_t thirsty, uint32_t sanitary, uint32_t spirit, uint32_t petid, userid_t uid);
int db_trust_pet(sprite_t* p, uint32_t flag, uint32_t endtime, uint32_t stamp, uint32_t hungry, uint32_t thirsty, uint32_t sanitary, uint32_t spirit, uint32_t petid, userid_t uid);
int db_pet_task(sprite_t* p, uint32_t flag, uint32_t endtime, uint32_t starttime, uint32_t taskid, uint32_t stamp, uint32_t hungry, uint32_t thirsty, uint32_t sanitary, uint32_t spirit, uint32_t petid, userid_t uid);
int db_set_pet_arg(sprite_t* p, uint8_t type, uint32_t arg, uint32_t petid, userid_t uid);
int db_infect_pet(sprite_t *p, pet_t *pet, userid_t id);


// chk if the given user @p has already had a super lahm
int chk_has_super_lahm(sprite_t* p);
//
int do_buy_pet(sprite_t* p, uint32_t itemid, uint32_t color, const char* nick, int charge, uint32_t flag);
int do_get_pet(sprite_t* p, uint32_t petid, uint32_t userid, uint8_t type);
int do_trust_pet(sprite_t* p);
//
int do_pick_pet_task(sprite_t* p);
int do_terminate_pet_task(sprite_t* p, userid_t uid, uint32_t petid, int fulltime);

// funcs for proto.c
int ask_for_a_pet_cmd(sprite_t* p, const uint8_t* body, int bodylen);
//
int buy_pet_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int feed_item_cmd(sprite_t *p, const uint8_t *body, int len);
int follow_pet_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int follow_outside_pet_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_pet_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_pet_cnt_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int learn_pet_skill_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int play_with_pet_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_pet_nick_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_pets_pos_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int do_set_pet_color(sprite_t *p, uint32_t petid, uint32_t color);
int discolor_pill_modify_pet_color_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_one_pet_callback(sprite_t* p, uint32_t id, char* buf, int len);
int pick_pet_magic_task_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int fin_pet_magic_task_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int cancel_pet_magic_task_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int set_magic_task_data_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_magic_task_data_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_pet_magic_task_list_cmd(sprite_t* p, const uint8_t* body, int bodylen);

int set_pet_magic_task_callback(sprite_t* p, uint32_t id, char* buf, int len);
int fin_pet_magic_task_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cancel_pet_magic_task_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_magic_task_data_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_magic_task_data_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_pet_magic_task_list_callback(sprite_t* p, uint32_t id, char* buf, int len);
int do_send_pet_home(sprite_t* p);
int send_pet_home_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int call_super_lahm_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_all_pet_info_cmd(sprite_t *p, const uint8_t *body, int bodylen);

//-----------------------------------------------------
// Trust Pet
//
int trust_pets_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int withdraw_pets_cmd(sprite_t* p, const uint8_t* body, int bodylen);
static inline int
get_trusted_pets_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 0);

	//DEBUG_LOG("GET TRUSTED PET\t[uid=%u]", p->id);
	return do_get_pet(p, 0, p->id, 0);
}

//-----------------------------------------------------
// Pet Task
//
int pick_pet_task_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int terminate_pet_task_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int set_pet_task_fin_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_pet_task_list_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int response_proto_get_pets(sprite_t* p, pet_t* pets, uint32_t uid, int count, int followed, uint8_t type);
void notify_pet_status(sprite_t* p, const pet_t* pets, int cnt);

static inline int
get_pets_on_task_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 0);

	//DEBUG_LOG("GET ON-TASK PETS\t[uid=%u]", p->id);
	return do_get_pet(p, 0, p->id, 0);
}

static inline int
pet_has_learned(pet_t* pet, uint32_t taskid)
{
	uint32_t loop;
	for(loop = 0; loop < pet->suppl_info.pti.count && loop < PET_TASK_MAX_CNT; loop++) {
		if (pet->suppl_info.pti.taskinfo[loop].taskid == taskid && pet->suppl_info.pti.taskinfo[loop].task_flag == 3) {
			return 1;
		}
	}
	return 0;
}

//-----------------------------------------------------
// Super Lahm
//
int become_super_lahm_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int has_super_lahm_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int lahm_housework_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int set_pet_arg_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_pet_task_attire_info_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_pet_task_attire_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int add_lamu_growth_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int add_lamu_growth_callback(sprite_t* p, uint32_t id, char* buf, int len);
struct pet *get_pet_by_id(uint32_t uid, uint32_t pet_id);

int set_pet_skill_ex_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_pet_skill_ex_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_build_temple_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_build_temple_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_lahm_task_state_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_lahm_task_state_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_lahm_task_state_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_lahm_task_state_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_lahm_task_data_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_lahm_task_data_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_lahm_task_data_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_lahm_task_data_callback(sprite_t* p, uint32_t id, char* buf, int len);
int build_temple_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int build_temple_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_pet_list_by_task_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_pet_list_by_task_callback(sprite_t* p, uint32_t id, char* buf, int len);
int exchange_item_to_rand_item_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int do_send_one_of_enum_items(sprite_t *p, char *buf, int len);

int set_pet_skill_type_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_pet_skill_type_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_pet_hot_skill_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_pet_hot_skill_callback(sprite_t* p, uint32_t id, char* buf, int len);
int db_set_pet_hot_skill(sprite_t *p, uint32_t petid, uint32_t hot1, uint32_t hot2, uint32_t hot3, uint32_t id);

int get_user_data_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_user_data_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_user_data_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_user_data_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_client_object_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_client_object_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_client_object_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_client_object_callback(sprite_t* p, uint32_t id, char* buf, int len);
int add_lamu_growth_use_tick_callback(sprite_t *p, uint32_t id, char *buf, int len);

int get_lahm_count_accord_state_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_lahm_count_accord_state_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int del_dead_lahm_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int del_pet_cmd(sprite_t *p, const uint8_t *body, int bodylen);

#endif // ONLINE_PET_H_
