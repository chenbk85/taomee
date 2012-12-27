#include <statistic_agent/msglog.h>
#include<libtaomee/project/utilities.h>
#include "proto.h"
#include "exclu_things.h"
#include "message.h"
#include "communicator.h"
#include "login.h"
#include "mole_class.h"
#include "pet_item.h"
#include "race_car.h"
#include "mole_pasture.h"
#include "mole_car.h"
#include "lamu_skill.h"
#include "small_require.h"
#include "mibiserver.h"
#include "event.h"
#include "street.h"
#include "mole_card.h"
//for animal follow
#include "mole_angel.h"
#include "mole_pasture.h"
#include "mole_cutepig.h"
//end
#include <openssl/md5.h>
#include <libtaomee/dataformatter/bin_str.h>


#define verify_buf_len 1024
#define verify_len 32

int defense_max[DEFENSE_TYPE_MAX] = {30, 40, 50, 50, 20};
static uint32_t gifts_rain_pos1 = 0;
static uint32_t gifts_rain_pos2 = 0;
static uint32_t gifts_rain_pos3 = 0;
static uint32_t gifts_rain_pos59 = 0;

#define db_get_ques_answer(p_, buf)\
		send_request_to_db(SVR_PROTO_GET_QUES_ANSWER, (p_), 8, buf, 0)
#define db_get_ques_num(p_)\
		send_request_to_db(SVR_PROTO_GET_QUES_NUM, (p_), 0, NULL, 0)
#define db_get_run_times(p_)\
		send_request_to_db(SVR_PROTO_GET_RUN_TIMES, (p_), 0, NULL, (p_)->id)
#define db_get_user_flag(p_, uid)\
		send_request_to_db(SVR_PROTO_GET_USER_FLAG, (p_), 0, NULL, uid)

uint32_t pos_dance[2] = {0};
/*
 *@ get all question and answer
 */
int get_ques_answer_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t buff[2]; // start index , and  count

	if (unpkg_uint32_uint32(body, bodylen, &buff[0], &buff[1]) == -1)
		return -1;

	DEBUG_LOG("GET QEST\t[%u %u]", p->id, buff[0]);
	return db_get_ques_answer(p, buff);
}

/*
 *@ get all question and answer, process db's reply
 */
int get_ques_answer_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
#define QUESTION_LEN	241
#define ANSWER_LEN 		361

	CHECK_BODY_LEN_GE(len, 4);

	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN (len - 4, count * (QUESTION_LEN + ANSWER_LEN));

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, count, j);

	DEBUG_LOG("GET QUES ANSWER\t[%u %s]", p->id, buf + 4);
	PKG_STR(msg, buf + 4, j, count * (QUESTION_LEN + ANSWER_LEN));

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

/*
 *@ get question number
 */
int get_ques_num_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	DEBUG_LOG("GET QEST NUM\t[%u]", p->id);
	return db_get_ques_num(p);
}

/*
 *@ get all question and answer, process db's reply
 */
int get_ques_num_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	uint32_t count = *(uint32_t*)buf;

	DEBUG_LOG("GET QUES NUM CALLBACK\t[%u %u]", p->id, count);
	response_proto_uint32(p, p->waitcmd, count, 0);
	return 0;
}

/*
 *@ get all question and answer
 */
int inc_run_times_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);

	if (!p->followed){
		ERROR_RETURN(("no pet follow %u", p->id), -1);
	}

	DEBUG_LOG("INC RUN TIME\t[%u]", p->id);
	return db_set_sth_done(p, 111, 1, p->id);
}

/*
 *@ get all question and answer, process db's reply
 */
int inc_run_times_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	uint32_t count = *(uint32_t*)buf;
	uint32_t itmid = 0;

	if(count == 5){
		uint32_t tmp_buf[5];
		tmp_buf[0] = 1;
		itmid = tmp_buf[1] = 160203;
		tmp_buf[2] = 1;
		tmp_buf[3] = 1;
		tmp_buf[4] = 0;
		send_request_to_db(SVR_PROTO_BUY_ITEM, 0, 20, tmp_buf, p->id);
	}

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, count, j);
	PKG_UINT32(msg, itmid, j);

	DEBUG_LOG("INC RUN TIME CALLBACK\t[%u %u %u]", p->id, count, itmid);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

/*
 *@ get all question and answer
 */
int get_run_times_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);

	DEBUG_LOG("GET RUN TIME\t[%u]", p->id);
	return db_get_run_times(p);
}

/*
 *@ get all question and answer, process db's reply
 */
int get_run_times_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	uint32_t count = *(uint32_t*)buf;

	DEBUG_LOG("GET RUN TIME CALLBACK\t[%u %u]", p->id, count);
	response_proto_uint32(p, p->waitcmd, count, 0);
	return 0;
}

/*
 *@ lahm play
 */
int lahm_play_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);

	if (!p->tiles || (p->tiles->id != 15 && p->tiles->id != 21 && p->tiles->id != 28 && p->tiles->id != 68)){
		ERROR_RETURN(("not in pet shop uid=%u", p->id), -1);
	}

	if (!p->followed){
		ERROR_RETURN(("no pet followed uid=%u", p->id), -1);
	}
	if (p->followed->spirit <= 95){
		p->followed->spirit += 5;
		//db_pet_op(0, p->followed, p->id);
		db_update_pet_attr(0, get_now_tv()->tv_sec, 0, 0, 0, 5, p->followed->id, p->id);
	}

	DEBUG_LOG("LAHM PLAY\t[%u]", p->id);
	response_proto_head(p,p->waitcmd, 0);
	return 0;
}

// ------------------------------for da rong shu SL party------------------------
int become_bigger_expeired(void* owner, void* data)
{
	int i;
	sprite_t* p = owner;

	if (p->action != WATER_BUCKET_ITEMNUM)
		return 0;

	p->action = 0;

	i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	init_proto_head(msg, PROTO_BECOME_BIGGER_EXPIRE, i);
	send_to_map(p, msg, i, 0);

	DEBUG_LOG("BIGGER EXPIRED\t[uid=%u]", p->id);
	return 0; // returns 0 means the `timer scanner` should remove this event
}

int become_bigger_cmd (sprite_t *p, const uint8_t *body, int len)
{
	CHECK_VALID_ID(p->id);

	uint32_t itmid = WATER_BUCKET_ITEMNUM; // the client's specification
	p->action = itmid;
	DEBUG_LOG("BECOME BIGGER\t[uid=%u]", p->id);

	add_event(&(p->timer_list), become_bigger_expeired, p, 0, get_now_tv()->tv_sec + 30, ADD_EVENT_REPLACE_UNCONDITIONALLY);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, itmid, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);

	return 0;
}

/*
 *@ add rand item
 */
int add_random_item(sprite_t* p)
{
	uint32_t itemid = *(uint32_t*)p->session;
	uint32_t itm_cnt = *(uint32_t*)(p->session + 4);
	uint32_t rand_flag = *(uint32_t*)(p->session + 8);
	uint32_t num_flag = 1;
	item_t* pitm = get_item_prop(itemid);
	if (!pitm)
		ERROR_RETURN(("bad itemid[%u %u]", p->id, itemid), -1);
	item_kind_t* kind = find_kind_of_item(itemid);
	if (!kind)
		ERROR_RETURN(("bad itemid[%u %u]", p->id, itemid), -1);

	if (rand_flag == 28) {
		db_single_item_op(0, p->id, 190437, 1, 0);
	}

	if (rand_flag == 74) {
		db_single_item_op(0, p->id, 190492, 1, 1);
		db_single_item_op(0, p->id, 190493, 1, 1);
	}
	if (rand_flag == 19) {
		if (rand() % 10 < 5) {
			db_single_item_op(0, p->id, 1230017, 1, 1);
			num_flag = 2;
		}
	}
	if (itemid == 1270009 && p->tiles && IS_PASTURE_MAP(p->tiles->id)) {
		db_single_item_op(0, GET_UID_IN_PASTURE(p->tiles->id), 190251, 1, 0);
		do_db_attr_op(0, GET_UID_IN_PASTURE(p->tiles->id), 0, 0, 0, 0, 2, 0, 0);
	}

	uint32_t flag = 0;
	if (kind->kind == HOME_ITEM_KIND) {
		flag = 1;
	} else if (kind->kind == HOMELAND_ITEM_KIND) {
		flag = 2;
	} else if (kind->kind == HAPPY_CARD_KIND) {
		flag = 6;
	}
	uint32_t tmp_buf[5];
	tmp_buf[0] = flag;
	tmp_buf[1] = itemid;
	tmp_buf[2] = itm_cnt;
	tmp_buf[3] = pitm->max;
	tmp_buf[4] = 0;
	send_request_to_db(SVR_PROTO_BUY_ITEM, 0, 20, tmp_buf, p->id);

	DEBUG_LOG("DISCOVER RAND ITM CALLBACK\t[%u %u]", p->id, itemid);
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, itemid, j);
	PKG_UINT32(msg, num_flag, j);
	PKG_UINT32(msg, itm_cnt, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

/*
 *@ discover random item
 */
int discover_random_item_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
typedef struct do_type{
	uint32_t type;
	uint32_t max;
	uint32_t itmid;
	uint32_t itm_cnt;
}do_type_t;
//118, 123, 122, 123, 125,

	do_type_t DuLuLu[1]				= {{130, 10, 1270005, 1}};
	do_type_t MoMoQiu[1]			= {{133, 12, 190390, 1}};
	do_type_t MoFaXing[1]			= {{134, 10, 190391, 1}};
	do_type_t HongZhuDuiHuanQuan[1]	= {{143, 1, 190229, 1}};
	do_type_t FlyBirdFruit[1]		= {{152, 5, 17010, 1}};
	do_type_t YinHuoCao[1]			= {{155, 3, 190351, 1}};
	do_type_t VipGetSheep[2]		= {{156, 2, 1270007, 1}, {156, 2, 1270006, 1}};
	do_type_t CanBaoBao[1]			= {{157, 3, 1270008, 1}};
	do_type_t english_lahm_or_not[6]= {{116, 9, 190223, 1}, {117, 9, 180057, 1}};
	do_type_t DriverDayTask[1]		= {{118, 5, 1230031, 1}};
	do_type_t Fishing_guaiguaiyu[1]	= {{122, 1, 1270009, 1}};
	do_type_t MuYang_Nangua[1]		= {{125, 3, 1230033, 1}};

	do_type_t Frank_momogift1[1]	= {{309, 5, 160521, 1}};
	do_type_t Frank_momogift2[1]	= {{309, 5, 160520, 1}};

	do_type_t Locus_yanjingtu[1]	= {{310, 10, 1270015, 1}};
	do_type_t Locus_shuaihuitu[1]	= {{310, 10, 1270016, 1}};
	do_type_t LaJiXiang_HuLuoBo[1]	= {{311, 5, 190028, 1}};
	do_type_t ShiWuCao_HuLuoBo[1]	= {{312, 5, 190028, 1}};

	do_type_t Heisenlin_guaiweiquan[1]	= {{313, 3, 180046, 1}};

	do_type_t Frank_applezhongzi[1]		= {{314, 1, 1230004, 1}};

	do_type_t Youyou_naiyouguilian[1]	= {{315, 99, 180067, 1}};

	do_type_t Frank_baimeigui[1]		= {{316, 20, 190477, 1}};

	do_type_t Locus_wayu[1]				= {{318, 1, 1270022, 1}};
	do_type_t Frank_special_color[4]	= {{323, 3, 190494, 1}, {323, 3, 190495, 1}, {323, 3, 190496, 1}, {323, 3, 190497, 1}};
	do_type_t Frank_justfish[3]			= {{324, 3, 1270023, 1}, {324, 3, 1270024, 1}, {324, 3, 1270025, 1}};

	do_type_t Frank_fi[1]				= {{325, 3, 190028, 1}};

	do_type_t HuangFengDie[1]			= {{305, 2, 1270026, 1}};
	do_type_t LanFengDie[1]				= {{326, 1, 1270027, 1}};
	do_type_t ShengDanShuLiWu[1]		= {{327, 5, 160606, 1}};
	do_type_t JuHuaiZhongZi[1]			= {{328, 1, 1230027, 2}};
	do_type_t LingLanZhongZi[1]			= {{329, 1, 1230030, 1}};
	do_type_t SiSeBaiHeHuaZhongZi[1]	= {{330, 3, 1230034, 1}};
	do_type_t chris_gift_packet[10]		= {{331, 10, 180015, 1}, {331, 10, 180016, 1}, {331, 10, 180027, 1}, \
										{331, 10, 180028, 1}, {331, 10, 180029, 1}, {331, 10, 180030, 1}, \
										{331, 10, 180067, 1}, {331, 10, 180068, 1}, {331, 10, 180069, 1}, \
										{331, 10, 180070, 1}};
	do_type_t xunlu[1]					= {{333, 5, 1270036, 1}};
	do_type_t xunlu_yinghuocao[1]		= {{334, 10, 190351, 2}};
	//do_type_t huafei[1]				= {{335, 1, 190574, 3}};
	do_type_t SongShu[1]				= {{336, 3, 1270037, 1}};
	do_type_t maomaodou[1]				= {{337, 1, 190196, 1}};

	CHECK_VALID_ID(p->id);

	CHECK_BODY_LEN(bodylen, 4);
	int i = 0;
	uint32_t rand_flag;
	UNPKG_UINT32(body, rand_flag, i);

	uint32_t indx = 0;

//	db_set_cnt(p,rand_flag + 10000,1);

	if (rand_flag == 3) {
			indx = 0;
	} else if (rand_flag == 10) {
		indx = 0;
	} else if (rand_flag == 12 || rand_flag == 13) {
		indx = 0;
	} else if (rand_flag == 15) {
		indx = 0;
	} else if (rand_flag == 18) {
		indx = 0;
	} else if (rand_flag == 19) {
		indx = 0;
	} else if (rand_flag == 20) {
		if(!ISVIP(p->flag)) {
			ERROR_RETURN(("not vip [%d]", p->id), -1);
		}
		indx = rand()%2;
	} else if (rand_flag == 21) {
		indx = 100;
		if(ISVIP(p->flag)) {
			indx = 0;
		} else if(p->followed) {
			int i;
			uint32_t task_id;
			for(i = 0; i < PET_TASK_MAX_CNT; i++) {
				task_id = p->followed->suppl_info.pti.taskinfo[i].taskid;
				if(task_id >= 101 && task_id <= 104) {
					indx = 0;
				}
			}
		}
		if(indx == 100) {
			ERROR_RETURN(("indx err [%d]", p->id), -1);
		}
	} else if (rand_flag == 22) {
		indx = rand() % 2;
		if(!p->followed) {
			ERROR_RETURN(("no pet follow [%d]", p->id), -1);
		}
		if (!pet_has_learned(p->followed, 15) && !PET_IS_SUPER_LAHM(p->followed)) {
			indx = 1;
		}
	} else if (rand_flag == 23) {
		indx = 0;
	} else if (rand_flag == 24) {
		indx = 0;
		if (!p->tiles || !IS_PASTURE_MAP(p->tiles->id))
			ERROR_RETURN(("not in right map\t[%u]", p->id), -1);
		db_single_item_op(0, GET_UID_IN_JY(p->tiles->id), 190251, 1, 0);
		db_add_pasture_visitors(p, GET_UID_IN_JY(p->tiles->id), PASTURE_VISITOR_FISHING);
	}  else if (rand_flag == 26) {
		indx = 0;
	} else if (rand_flag == 59 ||  rand_flag == 60 || rand_flag == 61 || rand_flag == 62 || rand_flag == 63 ||rand_flag == 64) {
		indx = 0;
	} else if (rand_flag == 65 ||rand_flag == 66) {
		indx = 0;
	} else if (rand_flag == 67 || rand_flag == 69 || rand_flag == 70) {
		indx = 0;
	} else if (rand_flag == 74) {
		indx = rand()%4;
	} else if (rand_flag == 75) {
		indx = rand()%3;
	} else if (rand_flag == 76 || rand_flag == 77) {
		indx = 0;
	} else if (rand_flag == 78 || rand_flag == 80 || rand_flag == 81) {
		if (!ISVIP(p->flag)) {
			return send_to_self_error(p, p->waitcmd, ERR_no_super_lahm, 1);
		}
		indx = 0;
	} else if (rand_flag == 79 || rand_flag == 82) {
		indx = 0;
	} else if (rand_flag == 83) {
		indx = rand() % 10;
	} else if (rand_flag == 85 || rand_flag == 86) {
		if (!ISVIP(p->flag)) {
			return send_to_self_error(p, p->waitcmd, ERR_no_super_lahm, 1);
		}
		indx = 0;
	} else if (rand_flag == 88 || rand_flag == 89){
		indx = 0;
	} else {
		ERROR_RETURN(("bad rand_type\t[%u %u]", p->id, rand_flag), -1);
	}


	*(uint32_t*)(p->session + 8) = rand_flag;
	if (rand_flag == 3) {
		*(uint32_t*)p->session = Heisenlin_guaiweiquan[indx].itmid;
			*(uint32_t*)(p->session + 4) = Heisenlin_guaiweiquan[indx].itm_cnt;
			DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, Heisenlin_guaiweiquan[indx].itmid);
			return db_set_sth_done(p, Heisenlin_guaiweiquan[indx].type, Heisenlin_guaiweiquan[indx].max, p->id);
	}
	if(rand_flag == 10) {

		*(uint32_t*)p->session = DuLuLu[indx].itmid;
		*(uint32_t*)(p->session + 4) = DuLuLu[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, DuLuLu[indx].itmid);

		return db_set_sth_done(p, DuLuLu[indx].type, DuLuLu[indx].max, p->id);
	}
	if(rand_flag == 12) {
		*(uint32_t*)p->session = MoMoQiu[indx].itmid;
		*(uint32_t*)(p->session + 4) = MoMoQiu[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, MoMoQiu[indx].itmid);
		return db_set_sth_done(p, MoMoQiu[indx].type, MoMoQiu[indx].max, p->id);
	}
	if(rand_flag == 13) {
		*(uint32_t*)p->session = MoFaXing[indx].itmid;
		*(uint32_t*)(p->session + 4) = MoFaXing[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, MoFaXing[indx].itmid);
		return db_set_sth_done(p, MoFaXing[indx].type, MoFaXing[indx].max, p->id);
	}
	if(rand_flag == 15) {
		*(uint32_t*)p->session = HongZhuDuiHuanQuan[indx].itmid;
		*(uint32_t*)(p->session + 4) = HongZhuDuiHuanQuan[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, HongZhuDuiHuanQuan[indx].itmid);
		return db_set_sth_done(p, HongZhuDuiHuanQuan[indx].type, HongZhuDuiHuanQuan[indx].max, p->id);
	} else if(rand_flag == 18) {
		*(uint32_t*)p->session = FlyBirdFruit[indx].itmid;
		*(uint32_t*)(p->session + 4) = FlyBirdFruit[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, FlyBirdFruit[indx].itmid);
		return db_set_sth_done(p, FlyBirdFruit[indx].type, FlyBirdFruit[indx].max, p->id);
	} else if(rand_flag == 19) {
		*(uint32_t*)p->session = YinHuoCao[indx].itmid;
		uint32_t itm_cnt = rand() % 3 + 3;
		//*(uint32_t*)(p->session + 4) = YinHuoCao[indx].itm_cnt;
		*(uint32_t*)(p->session + 4) = itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, YinHuoCao[indx].itmid);
		return db_set_sth_done(p, YinHuoCao[indx].type, YinHuoCao[indx].max, p->id);
	} else if(rand_flag == 20) {
		*(uint32_t*)p->session = VipGetSheep[indx].itmid;
		*(uint32_t*)(p->session + 4) = VipGetSheep[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, VipGetSheep[indx].itmid);
		return db_set_sth_done(p, VipGetSheep[indx].type, VipGetSheep[indx].max, p->id);
	} else if(rand_flag == 21) {
		*(uint32_t*)p->session = CanBaoBao[indx].itmid;
		*(uint32_t*)(p->session + 4) = CanBaoBao[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, CanBaoBao[indx].itmid);
		return db_set_sth_done(p, CanBaoBao[indx].type, CanBaoBao[indx].max, p->id);
	} else if(rand_flag == 22) {
		*(uint32_t*)p->session = english_lahm_or_not[indx].itmid;
		*(uint32_t*)(p->session + 4) = english_lahm_or_not[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, english_lahm_or_not[indx].itmid);
		return db_set_sth_done(p, english_lahm_or_not[indx].type, english_lahm_or_not[indx].max, p->id);
	} else if(rand_flag == 23) {
		*(uint32_t*)p->session = DriverDayTask[indx].itmid;
		*(uint32_t*)(p->session + 4) = DriverDayTask[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, DriverDayTask[indx].itmid);
		return db_set_sth_done(p, DriverDayTask[indx].type, DriverDayTask[indx].max, p->id);
	} else if(rand_flag == 24) {
		*(uint32_t*)p->session = Fishing_guaiguaiyu[indx].itmid;
		*(uint32_t*)(p->session + 4) = Fishing_guaiguaiyu[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, Fishing_guaiguaiyu[indx].itmid);
		return db_set_sth_done(p, Fishing_guaiguaiyu[indx].type, Fishing_guaiguaiyu[indx].max, p->id);
	} else if(rand_flag == 26) {
		*(uint32_t*)p->session = MuYang_Nangua[indx].itmid;
		*(uint32_t*)(p->session + 4) = MuYang_Nangua[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, MuYang_Nangua[indx].itmid);
		return db_set_sth_done(p, MuYang_Nangua[indx].type, MuYang_Nangua[indx].max, p->id);
	} else if (rand_flag == 59) {
		*(uint32_t*)p->session = Frank_momogift1[indx].itmid;
		*(uint32_t*)(p->session + 4) = Frank_momogift1[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, Frank_momogift1[indx].itmid);
		return db_set_sth_done(p, Frank_momogift1[indx].type, Frank_momogift1[indx].max, p->id);
	}  else if (rand_flag == 60) {
		*(uint32_t*)p->session = Frank_momogift2[indx].itmid;
		*(uint32_t*)(p->session + 4) = Frank_momogift2[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, Frank_momogift2[indx].itmid);
		return db_set_sth_done(p, Frank_momogift2[indx].type, Frank_momogift2[indx].max, p->id);
	} else if (rand_flag == 61) {
		*(uint32_t*)p->session = Locus_yanjingtu[indx].itmid;
		*(uint32_t*)(p->session + 4) = Locus_yanjingtu[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, Locus_yanjingtu[indx].itmid);
		return db_set_sth_done(p, Locus_yanjingtu[indx].type, Locus_yanjingtu[indx].max, p->id);
	} else if (rand_flag == 62) {
		*(uint32_t*)p->session = LaJiXiang_HuLuoBo[indx].itmid;
		*(uint32_t*)(p->session + 4) = LaJiXiang_HuLuoBo[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, LaJiXiang_HuLuoBo[indx].itmid);
		return db_set_sth_done(p, LaJiXiang_HuLuoBo[indx].type, LaJiXiang_HuLuoBo[indx].max, p->id);
	} else if (rand_flag == 63) {
		*(uint32_t*)p->session = ShiWuCao_HuLuoBo[indx].itmid;
		*(uint32_t*)(p->session + 4) = ShiWuCao_HuLuoBo[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, ShiWuCao_HuLuoBo[indx].itmid);
		return db_set_sth_done(p, ShiWuCao_HuLuoBo[indx].type, ShiWuCao_HuLuoBo[indx].max, p->id);
	}else if (rand_flag == 64) {
		*(uint32_t*)p->session = Frank_applezhongzi[indx].itmid;
		*(uint32_t*)(p->session + 4) = Frank_applezhongzi[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, Frank_applezhongzi[indx].itmid);
		return db_set_sth_done(p, Frank_applezhongzi[indx].type, Frank_applezhongzi[indx].max, p->id);
	} else if (rand_flag == 65) {
		*(uint32_t*)p->session = Youyou_naiyouguilian[indx].itmid;
		*(uint32_t*)(p->session + 4) = Youyou_naiyouguilian[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, Youyou_naiyouguilian[indx].itmid);
		return db_set_sth_done(p, Youyou_naiyouguilian[indx].type, Youyou_naiyouguilian[indx].max, p->id);
	}else if (rand_flag == 66) {
		*(uint32_t*)p->session = Locus_shuaihuitu[indx].itmid;
		*(uint32_t*)(p->session + 4) = Locus_shuaihuitu[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, Locus_shuaihuitu[indx].itmid);
		return db_set_sth_done(p, Locus_shuaihuitu[indx].type, Locus_shuaihuitu[indx].max, p->id);
	}else if (rand_flag == 67) {
		*(uint32_t*)p->session = Frank_baimeigui[indx].itmid;
		*(uint32_t*)(p->session + 4) = Frank_baimeigui[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, Frank_baimeigui[indx].itmid);
		return db_set_sth_done(p, Frank_baimeigui[indx].type, Frank_baimeigui[indx].max, p->id);
	} else if (rand_flag == 69) {
		*(uint32_t*)p->session = Locus_wayu[indx].itmid;
		*(uint32_t*)(p->session + 4) = Locus_wayu[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, Locus_wayu[indx].itmid);
		return db_set_sth_done(p, Locus_wayu[indx].type, Locus_wayu[indx].max, p->id);
	} else if (rand_flag == 74) {
		*(uint32_t*)p->session = Frank_special_color[indx].itmid;
		*(uint32_t*)(p->session + 4) = Frank_special_color[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, Frank_special_color[indx].itmid);
		return db_set_sth_done(p, Frank_special_color[indx].type, Frank_special_color[indx].max, p->id);
	} else if (rand_flag == 75) {
		*(uint32_t*)p->session = Frank_justfish[indx].itmid;
		*(uint32_t*)(p->session + 4) = Frank_justfish[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, Frank_justfish[indx].itmid);
		return db_set_sth_done(p, Frank_justfish[indx].type, Frank_justfish[indx].max, p->id);
	} else if (rand_flag == 76) {
		*(uint32_t*)p->session = Frank_fi[indx].itmid;
		*(uint32_t*)(p->session + 4) = Frank_fi[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, Frank_fi[indx].itmid);
		return db_set_sth_done(p, Frank_fi[indx].type, Frank_fi[indx].max, p->id);
	} else if (rand_flag == 77) {
		*(uint32_t*)p->session = HuangFengDie[indx].itmid;
		*(uint32_t*)(p->session + 4) = HuangFengDie[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, HuangFengDie[indx].itmid);
		return db_set_sth_done(p, HuangFengDie[indx].type, HuangFengDie[indx].max, p->id);
	} else if (rand_flag == 78) {
		*(uint32_t*)p->session = LanFengDie[indx].itmid;
		*(uint32_t*)(p->session + 4) = LanFengDie[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, LanFengDie[indx].itmid);
		return db_set_sth_done(p, LanFengDie[indx].type, LanFengDie[indx].max, p->id);
	} else if (rand_flag == 79) {
		*(uint32_t*)p->session = ShengDanShuLiWu[indx].itmid;
		*(uint32_t*)(p->session + 4) = ShengDanShuLiWu[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, ShengDanShuLiWu[indx].itmid);
		return db_set_sth_done(p, ShengDanShuLiWu[indx].type, ShengDanShuLiWu[indx].max, p->id);
	} else if (rand_flag == 80) {
		*(uint32_t*)p->session = JuHuaiZhongZi[indx].itmid;
		*(uint32_t*)(p->session + 4) = JuHuaiZhongZi[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, JuHuaiZhongZi[indx].itmid);
		return db_set_sth_done(p, JuHuaiZhongZi[indx].type, JuHuaiZhongZi[indx].max, p->id);
	} else if (rand_flag == 81) {
		*(uint32_t*)p->session = LingLanZhongZi[indx].itmid;
		*(uint32_t*)(p->session + 4) = LingLanZhongZi[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, LingLanZhongZi[indx].itmid);
		return db_set_sth_done(p, LingLanZhongZi[indx].type, LingLanZhongZi[indx].max, p->id);
	} else if (rand_flag == 82) {
		*(uint32_t*)p->session = SiSeBaiHeHuaZhongZi[indx].itmid;
		*(uint32_t*)(p->session + 4) = SiSeBaiHeHuaZhongZi[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, SiSeBaiHeHuaZhongZi[indx].itmid);
		return db_set_sth_done(p, SiSeBaiHeHuaZhongZi[indx].type, SiSeBaiHeHuaZhongZi[indx].max, p->id);
	} else if (rand_flag == 83) {
		*(uint32_t*)p->session = chris_gift_packet[indx].itmid;
		*(uint32_t*)(p->session + 4) = chris_gift_packet[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, chris_gift_packet[indx].itmid);
		return db_set_sth_done(p, chris_gift_packet[indx].type, chris_gift_packet[indx].max, p->id);
	} else if (rand_flag == 85) {
		*(uint32_t*)p->session = xunlu[indx].itmid;
		*(uint32_t*)(p->session + 4) = xunlu[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, xunlu[indx].itmid);
		return db_set_sth_done(p, xunlu[indx].type, xunlu[indx].max, p->id);
	} else if (rand_flag == 86) {
		*(uint32_t*)p->session = xunlu_yinghuocao[indx].itmid;
		*(uint32_t*)(p->session + 4) = xunlu_yinghuocao[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, xunlu_yinghuocao[indx].itmid);
		return db_set_sth_done(p, xunlu_yinghuocao[indx].type, xunlu_yinghuocao[indx].max, p->id);
	} else if (rand_flag == 88) {
		*(uint32_t*)p->session = SongShu[indx].itmid;
		*(uint32_t*)(p->session + 4) = SongShu[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, SongShu[indx].itmid);
		return db_set_sth_done(p, SongShu[indx].type, SongShu[indx].max, p->id);
	} else if (rand_flag == 89) {
		*(uint32_t*)p->session = maomaodou[indx].itmid;
		*(uint32_t*)(p->session + 4) = maomaodou[indx].itm_cnt;
		DEBUG_LOG("DISCOVER RAND ITM\t[%u %u]", p->id, maomaodou[indx].itmid);
		return db_set_sth_done(p, maomaodou[indx].type, maomaodou[indx].max, p->id);
	}

	return 0;
}

/*
 *@ get user flag
 */
int get_user_flag_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t userid;

	CHECK_BODY_LEN(bodylen, 4);
	int j = 0;
	UNPKG_UINT32(body, userid, j);
	CHECK_VALID_ID(userid);
	DEBUG_LOG("GET USER FLAG\t[%u %u]", p->id, userid);

	sprite_t* who = get_sprite(userid);
	if (who) {
		response_proto_uint32(p, p->waitcmd, who->flag, 0);
		return 0;
	}
	return db_get_user_flag(p, userid);
}

/*
 *@ get user flag from db
 */
int get_user_flag_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);

	uint32_t user_flag = *(uint32_t*)buf;

	DEBUG_LOG("GET USER FLAG CALLBACK\t[%u %u]", p->id, user_flag);
	response_proto_uint32(p, p->waitcmd, user_flag, 0);
	return 0;
}

int lahm_changed_expeired(void* owner, void* data)
{
	int i;
	sprite_t* p = owner;

	if(p->followed && PET_FORM_IS_CHANGED(p->followed)){
		PET_RECOVER_FORM(p->followed);
	} else {
		return 0;
	}

	i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	init_proto_head(msg, PROTO_PET_RECOVER_FORM, i);
	send_to_map(p, msg, i, 0);

	DEBUG_LOG("SL RECOVERED\t[%u]", p->id);
	return 0; // returns 0 means the `timer scanner` should remove this event
}

int lahm_form_change_cmd (sprite_t *p, const uint8_t *body, int len)
{
	if( !p->followed )
		ERROR_RETURN(("no pet follow %u", p->id), -1);
	if (!PET_IS_SUPER_LAHM(p->followed))
		ERROR_RETURN (("pet id %u is not super lahm", p->followed->id), -1);

	PET_CHANGE_FORM(p->followed);

	add_event(&(p->timer_list), lahm_changed_expeired, p, 0, get_now_tv()->tv_sec + 600, ADD_EVENT_REPLACE_UNCONDITIONALLY);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);

	DEBUG_LOG("SL CHANGE\t[pid=%u]", p->id);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);

	return 0;
}

/*
 *@ get question number
 */
int present_birthpet_hat_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	DEBUG_LOG("GET BIRTHPET HAT\t[%u]", p->id);
	CHECK_VALID_ID(p->id);

	CHECK_BODY_LEN(bodylen, 4);
	int i = 0;
	uint32_t itemid;
	UNPKG_UINT32(body, itemid, i);

	if (itemid < 12233 || itemid > 12244) {
		ERROR_RETURN(("bad item id\t[%u %u]", p->id, itemid), -1);
	}

	*(uint32_t*)p->session = itemid;

	int j = 0;
	uint8_t buff[13];
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, 12233, j);
	PKG_H_UINT32(buff, 12245, j);
	PKG_UINT8(buff, 2, j);

	return send_request_to_db(SVR_PROTO_GET_ITEM, p, j, buff, p->id);
}

/*
 *@ get user flag
 */
int get_item_from_baohe_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t itmid, userid;
	uint32_t del_id = 0;
	uint32_t baohe_itms[][2] = {{190190, 1250001}};

	CHECK_BODY_LEN(bodylen, 8);
	int j = 0;
	UNPKG_UINT32(body, userid, j);
	UNPKG_UINT32(body, itmid, j);
	CHECK_VALID_ID(userid);
	CHECK_VALID_ID(p->id);

	int loop;
	for (loop = 0; loop < sizeof(baohe_itms)/8; loop++) {
		if (baohe_itms[loop][0] == itmid) {
			del_id = baohe_itms[loop][1];
			break;
		}
	}

	if (del_id == 0)
		ERROR_RETURN(("bad itmid\t[%u %u %u]", p->id, itmid, userid), -1);

	*(uint32_t*)p->session = userid;
	*(uint32_t*)(p->session + 4) = del_id;

	DEBUG_LOG("GET ITM FRM BH\t[%u %u %u %u]", p->id, itmid, userid, del_id);
	return db_single_item_op(p, p->id, itmid, 1, 1);
}

/*
 *@ get user flag from db
 */
int del_item_from_baohe(sprite_t* p)
{
	uint32_t userid = *(uint32_t*)p->session;
	uint32_t del_id = *(uint32_t*)(p->session + 4);

	db_single_item_op(0, userid, del_id, 1, 0);
	db_add_op_to_baohe(p, userid, del_id);

	DEBUG_LOG("DEL ITM FRM BH\t[%u %u %u]", p->id, del_id, userid);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int get_baohe_op_list_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 0);

	if (!p->tiles || !IS_JY_MAP(p->tiles->id)) {
		ERROR_RETURN(("not in JY map\t[%u]", p->id), -1);
	}
	uint32_t uid = GET_UID_IN_JY(p->tiles->id);

	return db_get_baohe_op_list(p, uid);
}

int get_baohe_op_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + count * sizeof(op_baohe_t));

	op_baohe_t* op_t = (op_baohe_t*)(buf + 4);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, count, j);

	int loop;
	for (loop = 0; loop < count; loop++) {
		PKG_UINT32(msg, op_t->userid, j);
		PKG_UINT32(msg, op_t->itm_type, j);
		PKG_STR(msg, op_t->nick, j, USER_NICK_LEN);
		PKG_UINT32(msg, op_t->color, j);
		PKG_UINT8(msg, op_t->isvip, j);
		PKG_UINT32(msg, op_t->op_stamp, j);
		op_t++;
	}
	DEBUG_LOG("GET BH OPLIST\t[%u %u len=%u]", p->id, count, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

/*
 *@ set question naire
 */
int set_ques_naire_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint8_t* buff, count;
	uint32_t type = 0; // start index , and  count
	buff = p->session;

	CHECK_BODY_LEN_GE(bodylen, 16);

	int j = 0;
	p->sess_len = 0;
	UNPKG_UINT32(body, type, j);
	PKG_H_UINT32(buff, type, p->sess_len);
	UNPKG_UINT32(body, count, j);
	PKG_H_UINT32(buff, count, p->sess_len);
	CHECK_BODY_LEN(bodylen, count * 8 + 8);

	//type=26 has been used for feifei daren show
	if (count >= 99 || type > 25) {
		ERROR_RETURN(("too much question\t[%u %u]", p->id, count), -1);
	}

	int loop;
	for (loop = 0; loop < count; loop++) {
		uint32_t tmp;
		UNPKG_UINT32(body, tmp, j);
		PKG_H_UINT32(buff, tmp, p->sess_len);  // ques number
		UNPKG_UINT32(body, tmp, j);
		PKG_H_UINT32(buff, tmp, p->sess_len);  // ques answer
	}

	uint32_t day_limit = 1;
	DEBUG_LOG("SET QEST NAIRE\t[%u %u]", p->id, count);
	if (type <= 15)
	{
		return db_set_sth_done(p, 1000000080 + type, 1, p->id);//1000000080-1000000100 已经被使用
	}
	else if (type == 26)
	{
		day_limit = 3;
	}
	return db_set_sth_done(p, 39000 + type, day_limit, p->id);
	/*
	else
	{
		return db_set_sth_done(p, 39000 + type, 1, p->id);//39016-39025 已经被使用
	}
	*/
}

int get_ques_naire_info_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t type = 0;
	CHECK_BODY_LEN(bodylen, 4);
	int j = 0;
	UNPKG_UINT32(body, type, j);
	return send_request_to_db(SVR_PROTO_GET_QUES_NAIRE_INFO, p, 4, &type, p->id);

}

int get_ques_naire_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	struct ques_naire_info_hdr {
		uint32_t	count;
	}__attribute__((packed));

	struct ques_naire_info {
		uint32_t	maintype;
		uint32_t    subtype;
		uint32_t    count;
	}__attribute__((packed));

	struct ques_naire_info_hdr *p_hdr_info = (struct ques_naire_info_hdr*)buf;

	CHECK_BODY_LEN_GE(len,sizeof(struct ques_naire_info_hdr));
	int l = sizeof(protocol_t);;
	PKG_UINT32(msg, p_hdr_info->count, l);

	CHECK_BODY_LEN(len,sizeof(struct ques_naire_info_hdr) + p_hdr_info->count*sizeof(struct ques_naire_info));
	struct ques_naire_info *p_info = (struct ques_naire_info*)(buf + sizeof(struct ques_naire_info_hdr));

	int i = 0;
	for (i = 0; i < p_hdr_info->count; i++)
	{
		PKG_UINT32(msg, p_info->maintype, l);
		PKG_UINT32(msg, p_info->subtype, l);
		PKG_UINT32(msg, p_info->count, l);
		p_info++;
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}


int request_tmp_superlamn_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	return send_request_to_db(SVR_PROTO_REQ_TMP_SUPERLAMN, p, 0, NULL, p->id);
}

int request_tmp_superlamn_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct _req_vip {
		uint32_t channel_id;
		char verify_info[32];
		uint32_t client_ip;
	}req_vip_t;
	CHECK_BODY_LEN(len, 4);
	uint32_t result = *(uint32_t*)buf;
	DEBUG_LOG("REQ TMP SUPERLAMN [%d %d]", p->id, result);
	if(result == 1) {
		req_vip_t req_vip;
		req_vip.client_ip = p->fdsess->remote_ip;
		req_vip.channel_id = 90;
		return send_request_to_db(SVR_PROTO_ENABLE_TMP_SUPERLAMN, p, sizeof(req_vip_t), &req_vip, p->id);
	} else if (result == 2) {
		add_mibi_consume_coupon(p, 120);
		return 0;
	}
	response_proto_uint32(p, p->waitcmd, result, 0);
	return 0;
}

int req_tmp_superlamn_stat_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	return send_request_to_db(SVR_PROTO_REQ_TMP_SUPERLAMN_STAT, p, 0, NULL, p->id);
}

int req_tmp_superlamn_stat_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t result = *(uint32_t*)buf;
	DEBUG_LOG("REQ TMP SUPERLAMN STAT [%d %d]", p->id, result);
	response_proto_uint32(p, p->waitcmd, result, 0);
	return 0;
}


int enable_tmp_superlamn_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t trade_id = *(uint32_t*)buf;
	uint32_t result = *(uint32_t*)(buf + 4);
	DEBUG_LOG("ENABLE TMP SUPERLAMN [%d %d %d]", p->id, trade_id, result);
	response_proto_uint32(p, p->waitcmd, !result, 0);
	return 0;
}

/*
 *@ give JiJi Le card freely
 */
int give_jijile_card_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{

	uint32_t jijile_card[4] = {1290056, 1290057, 1290058, 1290059};
	CHECK_VALID_ID(p->id);

	CHECK_BODY_LEN(bodylen, 4);
	int i = 0;
	uint32_t rand_flag;
	UNPKG_UINT32(body, rand_flag, i);

	uint32_t indx;

	if (rand_flag == 1) {
		indx = rand() % 4;
		do_buy_item(p, jijile_card[indx], 1, 1, 0);
		response_proto_uint32(p,p->waitcmd, jijile_card[indx], 0);
		return 0;
	} else {
		ERROR_RETURN(("bad rand_type\t[%u %u]", p->id, rand_flag), -1);
	}

}

/*
 *@ give 1000 xiaome
 */
int get_500_xiaome_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	if (!p->tiles || p->tiles->id != 28) {
		ERROR_RETURN(("not in right map[%u %lu]", p->id, p->tiles->id), -1);
	}

	int itm_type = rand()%2;
	uint32_t itmid;
	uint32_t exid;
	int cnt;
	if (itm_type) {
		itmid = 17010;
		exid = 129;
		cnt = 3;
	} else {
		itmid = 0;
		exid = 119;
		cnt = 10;
	}

	*(uint32_t*)p->session = itmid;
	return db_set_sth_done(p, exid, cnt, p->id);
}

int get_lottery_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	return db_set_sth_done(p, 131, 1, p->id);
	//return db_chk_vip_info(p);
}

int calculate_lottery(sprite_t* p, uint8_t* buf, int count)
{
	if((ISVIP(p->flag) && count == 4) || (!ISVIP(p->flag) && count == 2)) {
		response_proto_uint32(p, p->waitcmd, 0, 0);
		return 0;
	}
	int cloth_info[4] = {0};
	static int cloth_id[4] = {12751, 12752, 12753, 12754};
	int i, k;

	int pos = 0;
	for(i = 0; i < 4; i++) {
		int exist = 0;
		for(k = 0; k < count; k++) {
			int tmp_id = *(uint32_t*)(buf + 8 * k);
			int tmp_cnt = *(uint32_t*)(buf + 8 * k + 4);
			if(cloth_id[i] == tmp_id && tmp_cnt) {
				exist = tmp_id;
				break;
			}
		}
		if((!exist) && (ISVIP(p->flag) || (i == 0 || i == 2))) {
			cloth_info[pos] = cloth_id[i];
			pos++;
		}
	}
	//DEBUG_LOG("CLOTH INFO [%d %d %d %d %d]", p->id, cloth_info[0], cloth_info[1], cloth_info[2], cloth_info[3]);
	int itmid = 0;
	int idx = rand()%100;
	if(pos) {
		if(ISVIP(p->flag)) {
			if(idx < 80) {
				itmid = 190381;
			} else {
				idx = rand()%pos;
				itmid = cloth_info[idx];
			}
		} else {
			if(idx < 5) {
				idx = rand()%pos;
				itmid = cloth_info[idx];
			} else if (idx < 30) {
				itmid = 190381;
			}
		}
		db_single_item_op(0, p->id, itmid, 1, 1);
	}
	DEBUG_LOG("LOTTERY [%d %d]", p->id, itmid);
	response_proto_uint32(p, p->waitcmd, itmid, 0);
	return 0;
}

int become_fail_expeired(void* owner, void* data)

{
	int i;
	sprite_t* p = owner;

	if (p->action != FAIL_ACTION_ITEMNUM)
		return 0;

	p->action = 0;

	i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	init_proto_head(msg, PROTO_BECOME_FAIL_EXPIRE, i);
	send_to_map(p, msg, i, 0);

	DEBUG_LOG("FAIL ACTION EXPIRED\t[uid=%u]", p->id);
	return 0; // returns 0 means the `timer scanner` should remove this event
}

int become_fail_cmd (sprite_t *p, const uint8_t *body, int len)
{
	CHECK_VALID_ID(p->id);

	uint32_t itmid = FAIL_ACTION_ITEMNUM; // the client's specification
	p->action = itmid;
	DEBUG_LOG("BECOME VICTORY ACTION\t[uid=%u]", p->id);

	add_event(&(p->timer_list), become_fail_expeired, p, 0, get_now_tv()->tv_sec + 600, ADD_EVENT_REPLACE_UNCONDITIONALLY);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, itmid, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);

	return 0;
}
int become_victory_expeired(void* owner, void* data)
{
	int i;
	sprite_t* p = owner;

	if (p->action != VICTORY_ACTION_ITEMNUM)
		return 0;

	p->action = 0;

	i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	init_proto_head(msg, PROTO_BECOME_VICTORY_EXPIRE, i);
	send_to_map(p, msg, i, 0);

	DEBUG_LOG("VICTORY ACTION EXPIRED\t[uid=%u]", p->id);
	return 0; // returns 0 means the `timer scanner` should remove this event
}

int become_victory_cmd (sprite_t *p, const uint8_t *body, int len)
{
	CHECK_VALID_ID(p->id);

	uint32_t itmid = VICTORY_ACTION_ITEMNUM; // the client's specification
	p->action = itmid;
	DEBUG_LOG("BECOME VICTORY ACTION\t[uid=%u]", p->id);

	add_event(&(p->timer_list), become_victory_expeired, p, 0, get_now_tv()->tv_sec + 600, ADD_EVENT_REPLACE_UNCONDITIONALLY);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, itmid, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);

	return 0;
}

int get_char_cloth_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	uint32_t itmid;
	int i = 0;
	UNPKG_UINT32(body, itmid, i);
	item_t* itm = get_item_prop(itmid);
	if (!itm || itmid < 12919 || itmid > 12944) {
		ERROR_RETURN(("bad itmid %u", itmid), -1);
	}

	*(uint32_t*)p->session = itmid;
	int j = 0;
	uint8_t buff[13];
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, 12919, j);
	PKG_H_UINT32(buff, 12944, j);
	PKG_UINT8(buff, 2, j);
	//return send_request_to_db(SVR_PROTO_GET_ITEM, p, j, buff, p->id);
	return send_request_to_db(SVR_PROTO_CHK_ITEM, p, j, buff, p->id);
}

int do_get_char_cloth(sprite_t* p, uint32_t count)
{
	uint32_t itmid = *(uint32_t*)p->session;
	if(count) {
		response_proto_uint32(p, p->waitcmd, 0, 0);
	} else {
		db_single_item_op(0, p->id, itmid, 1, 1);
		response_proto_uint32(p, p->waitcmd, 1, 0);
	}
	return 0;
}

int exchg_char_cloth_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	uint32_t itmid;
	int i = 0;
	UNPKG_UINT32(body, itmid, i);
	item_t* pitm = get_item_prop(itmid);
	if (!pitm || itmid < 12919 || itmid > 12944) {
		ERROR_RETURN(("bad itmid %u", itmid), -1);
	}

	*(uint32_t*)p->session = itmid;

	int j = 0;
	uint8_t buff[256];
	// Package For DB
	uint32_t rand_id = 190423;
	PKG_H_UINT32(buff, 1, j);
	PKG_H_UINT32(buff, 1, j);
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, 0, j);
	if (pkg_item_kind(p, buff, rand_id, &j) == -1)
		return -1;
	PKG_H_UINT32(buff, rand_id, j);
	PKG_H_UINT32(buff, 30, j);

	if (pkg_item_kind(p, buff, itmid, &j) == -1)
		return -1;

	PKG_H_UINT32(buff, itmid, j);
	PKG_H_UINT32(buff, 1, j);
	PKG_H_UINT32(buff, pitm->max, j);

	DEBUG_LOG("EXCHG CHAR CLOTH\t[%u %u]", p->id, itmid);
	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buff, p->id);
}

int get_class_score_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 0);

	if(!p->tiles || !IS_CLASS_MAP(p->tiles->id)) {
		ERROR_RETURN(("not in class [%u]", p->id), -1);
	}
	uint32_t uid = GET_UID_IN_CLASSID(p->tiles->id);

	return db_class_get_score(p, uid);
}

int get_class_score_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	uint32_t score = *(uint32_t*)buf;
	switch (p->waitcmd) {
		case PROTO_GET_CLASS_SCORE:
			response_proto_uint32(p, p->waitcmd, score, 0);
			DEBUG_LOG("GET CLASS SCORE\t[%u %u]", p->id, score);
			return 0;
		case PROTO_ENGLISH_CLASS:
		{
			uint32_t level;
			uint32_t jiangbei[] = {1260094, 1260095, 1260096, 1260109};
			uint32_t jinqi[] 	  = {1260097, 1260098, 1260099, 1260109};
			if (score > 2999) {
				level = 0;
			} else if (score > 999) {
				level = 1;
			} else if (score > 199) {
				level = 2;
			} else if (score > 99) {
				level = 3;
			} else {
				response_proto_uint32_uint32(p, p->waitcmd, 1, 0, 0);
				return 0;
			}

			*(uint32_t*)(p->session) = jiangbei[level];
			*(uint32_t*)(p->session + 4) = jinqi[level];
					uint32_t pkg[4];
					pkg[0] = 1;
					pkg[1] = jiangbei[level];
					pkg[2] = jiangbei[level];
					pkg[3] = jinqi[level];
					DEBUG_LOG("CLASS REWARD [%d %d %d]", p->id, pkg[2], pkg[3]);
					return send_request_to_db(SVR_PROTO_GET_CLASS_QUES_REWARD, p, 16, pkg, p->id);
		}

	}
	ERROR_RETURN(("BAD COMMAND\t[cmd=%u]", p->waitcmd), -1);
}

int get_tangguo_score_total_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);

	return db_get_tangguo_score_times(p, p->id);
}

int english_lahm_get_cloth_and_honor_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 0);
	if (!p->followed || (!pet_has_learned(p->followed, 15) && !PET_IS_SUPER_LAHM(p->followed))) {
		ERROR_RETURN(("%u no pet followed or pet not learn english", p->id), -1);
	}

	db_buy_pet_items(0, p->followed->id, 1210017, 1, 1, p->id);
	db_buy_pet_items(0, p->followed->id, 1200026, 1, 1, p->id);

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

static int is_vip_level_item(sprite_t* p, uint32_t itemid, int vip_month_level)
{
	item_t* pitm = get_item_prop(itemid);
	if (!pitm)
		ERROR_RETURN(("bad itemid[%u %u]", p->id, itemid), -1);

	DEBUG_LOG("VIP LEVEL[%u %u %u]", p->id, vip_month_level, pitm->vip_gift_level);
	if(((vip_month_level >= pitm->vip_gift_level) || (p->vip_level >= pitm->vip_gift_level)) && pitm->vip_gift_level)
		return 1;
	else
		return 0;
}



int calc_month_vip_level(int vip_month)
{
	if (vip_month <= 2) {
		return 1;
	} else if (vip_month <= 5) {
		return 2;
	}else if (vip_month <= 11) {
		return 3;
	} else if (vip_month <= 23) {
		return 4;
	} else if (vip_month <= 35) {
		return 5;
	} else if (vip_month <= 47){
		return 6;
	}else if (vip_month <= 59){
		return 7;
	}else{
		return 8;
	}
}


int vip_level_mole_gifts_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);

	uint32_t itemid, vip_level;
	int i = 0;
	UNPKG_UINT32(body, vip_level, i);
	UNPKG_UINT32(body, itemid, i);

	item_t* pitm = get_item_prop(itemid);
	if (!pitm)
		ERROR_RETURN(("bad itemid[%u %u]", p->id, itemid), -1);
	item_kind_t* kind = find_kind_of_item(itemid);
	if (!kind)
		ERROR_RETURN(("bad itemid[%u %u]", p->id, itemid), -1);

	int vip_month_level = calc_month_vip_level(p->vip_month);

	if ((vip_month_level < vip_level) && (p->vip_level < vip_level)) {
		ERROR_RETURN(("wrong level[%u %u %u]", p->id, vip_month_level, vip_level), -1);
	}

	if (!is_vip_level_item(p, itemid, vip_month_level)) {
		ERROR_RETURN(("itme not in this level[%u %u %u %u]", p->id, vip_month_level, vip_level, itemid), -1);
	}

	uint32_t flag = 0;
	if (kind->kind == HOME_ITEM_KIND) {
		flag = 1;
	} else if (kind->kind == HOMELAND_ITEM_KIND) {
		flag = 2;
	} else if (kind->kind == HAPPY_CARD_KIND) {
		flag = 6;
	} else if (kind->kind == CAR_KIND) {
		 *(uint32_t*)p->session = itemid;
		 return db_chk_itm_cnt(p, 1220097, 1220098);
	}
	uint32_t tmp_buf[5];
	tmp_buf[0] = flag;
	tmp_buf[1] = itemid;
	tmp_buf[2] = 1;
	tmp_buf[3] = pitm->max;
	tmp_buf[4] = 0;
	return send_request_to_db(SVR_PROTO_BUY_ITEM, p, 20, tmp_buf, p->id);
}

int vip_level_mole_gifts_car(sprite_t *p, int count)
{
	if (count == 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_have_garage, 1);
	}
	return db_add_car(p, *(uint32_t *)p->session, 0, p->id);
}


int vip_level_lahm_gifts_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);

	uint32_t itemid, vip_level;
	int i = 0;
	UNPKG_UINT32(body, vip_level, i);
	UNPKG_UINT32(body, itemid, i);

	item_t* pitm = get_item_prop(itemid);
	if (!pitm)
		ERROR_RETURN(("bad itemid[%u %u]", p->id, itemid), -1);
	if (!p->followed)
		ERROR_RETURN(("no pet follow[%u %u]", p->id, itemid), -1);

	item_kind_t* kind = find_kind_of_item(itemid);
	if (!kind || (kind->kind != PET_ITEM_CLOTH_KIND && kind->kind != PET_ITEM_HONOR_KIND))
		ERROR_RETURN(("bad itemid[%u %u]", p->id, itemid), -1);

	int vip_month_level = calc_month_vip_level(p->vip_month);


	if ((vip_month_level < vip_level) && (p->vip_level < vip_level)) {
		ERROR_RETURN(("wrong level[%u %u %u]", p->id, vip_month_level, vip_level), -1);
	}

	if (!is_vip_level_item(p, itemid, vip_month_level)) {
		ERROR_RETURN(("itme not in this level[%u %u %u]", p->id, vip_month_level, itemid), -1);
	}

	return db_buy_pet_items(p, p->followed->id, itemid, 1, 1, p->id);
}

int do_send_candy_packet(sprite_t * p)
{
typedef struct do_type{
	uint32_t type;
	uint32_t max;
	uint32_t itmid;
	uint32_t itm_cnt;
}do_type_t;

	do_type_t  candy_packet[10]  = {{123, 10, 1230001, 1},{123, 10, 190441, 1},{123, 10, 190441, 1}, {123, 10, 190441, 1},
								 {12, 10,190395,1},{12, 10,190441,1}, {12, 10,180065,1},{12, 10,180066,1},{12, 10,160513,1},
								 {12, 10,160513,1}};
	uint32_t type = *(uint32_t*)p->session;
	int j = sizeof(protocol_t);
	if(type == 1) {
		int idx1, idx2, idx3 ;
		idx1 = rand()%10;
		idx2 = rand()%10;
		while (idx2 == idx1) idx2 = rand()%10;
		idx3 = rand()%10;
		while (idx3 == idx1 ||idx3 == idx2) idx3 = rand()%10;
		PKG_UINT32(msg, 3, j);
		PKG_UINT32(msg, candy_packet[idx1].itmid, j);
		PKG_UINT32(msg, 1, j);
		db_buy_itm_freely(0, p->id, candy_packet[idx1].itmid, 1);
		PKG_UINT32(msg, candy_packet[idx2].itmid, j);
		PKG_UINT32(msg, 1, j);
		db_buy_itm_freely(0, p->id, candy_packet[idx2].itmid, 1);
		PKG_UINT32(msg, candy_packet[idx3].itmid, j);
		PKG_UINT32(msg, 1, j);
		db_buy_itm_freely(0, p->id, candy_packet[idx3].itmid, 1);
	} else
		PKG_UINT32(msg, 0, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);

}

int do_already_get_packet(sprite_t* p)
{
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, 0, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int send_candy_packet_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t type;
	int j = 0;
	UNPKG_UINT32(body, type, j);
	*(uint32_t*)p->session = type;
	return db_set_sth_done(p, 206, 5, p->id);
}

int become_nangua_expeired(void* owner, void* data)
{
	int i;
	sprite_t* p = owner;
	p->action = 0;

	i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	init_proto_head(msg, PROTO_BECOME_NANGUA_EXPIRE, i);
	send_to_map(p, msg, i, 0);

	return 0;
}

int do_change_self_nangua(sprite_t *p)//ddd
{
	uint32_t itmid = *(uint32_t*)p->session;
	p->action = itmid;
	if (itmid >= 10015 && itmid <= 10018) {
		add_event(&(p->timer_list), become_shibohui_expeired, p, 0, get_now_tv()->tv_sec + 20,
				ADD_EVENT_REPLACE_UNCONDITIONALLY);
	}
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, itmid, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);

	return 0;
}

/* @brief 食博会变身超时,20秒后
 */
int become_shibohui_expeired(void *owner, void *data)
{
	int i;
	sprite_t* p = owner;
	p->action = 0;
	i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, 0, i);
	init_proto_head(msg, PROTO_CHANGE_SELF_NANGUA, i);
	send_to_map(p, msg, i, 0);
	return 0;
}


int change_self_nangua_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	CHECK_VALID_ID(p->id);
	uint32_t op, change_flag;
	int j = 0;
	UNPKG_UINT32(body, change_flag, j);
	UNPKG_UINT32(body, op, j);
	uint32_t temp = get_now_tv()->tv_sec;
	if (temp < p->last_action_time + 2)
		return send_to_self_error(p, p->waitcmd, -ERR_client_not_proc, 1);
	p->last_action_time = temp;
	if (op != 0 && op != 1) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_op, 1);
	}
	//10000-nangua ;10001-  jiangshi;10002- youling
	//10004:dance1 10005:dance 2 10006:dance 3
	//10012 used for frozen mole; 10013-luokuang,10015~10018食博会变身, 150005 jiqibianshen
	if (change_flag < 10000 || change_flag > 10018)
		ERROR_RETURN(("invaild change id"), -1);
	if (change_flag == 10012 && (!p->tiles || p->tiles->id != 114))
		ERROR_RETURN(("not in right map\t[%u]", p->id), -1);
	if (op == 1 && ((change_flag == 10010 && pos_dance[0]) || (change_flag == 10011 && pos_dance[1]))) {
		DEBUG_LOG("OCCUPY BY OTHER\t[%u %u]", p->id, change_flag);
		return send_to_self_error(p, p->waitcmd, -ERR_candy_no_sit_ex_ex, 1);
	}
	if (op == 0 && ((change_flag == 10010 && pos_dance[0]) || (change_flag == 10011 && pos_dance[1]))) {
		pos_dance[change_flag == 10010 ? 0 : 1] = 0;
	}
	*(uint32_t*)p->session = change_flag;
	if (op == 1) {
		if (p->action == 10014) {
			return send_to_self_error(p, p->waitcmd, -ERR_you_have_change, 1);
		}
		if (change_flag == 10010 || change_flag == 10011)
			pos_dance[change_flag == 10010 ? 0 : 1] = 1;
		return do_change_self_nangua(p);
	}
	p->action = 0;
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, 0, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);

	return 0;
}

//PROTO_GET_BIBO_INFO
int get_bibo_date_info_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	int j = 0;
	uint32_t start, end;
	UNPKG_UINT32(body, start, j);
	UNPKG_UINT32(body, end, j);
	uint32_t dates[] = {start, end};
	return send_request_to_db(SVR_PROTO_GET_BIBO_INFO, p, 8, dates, p->id);
}

int get_bibo_date_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count;
	int j = 0;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN(len, 124*count);
	int i = sizeof(protocol_t), loop;
	PKG_UINT32(msg, count, i);
	for (loop = 0; loop < count; loop ++) {
		uint32_t date, start_time, end_time;
		char info[100] = {0};
		UNPKG_H_UINT32(buf, date, j);
		UNPKG_H_UINT32(buf, start_time, j);
		UNPKG_H_UINT32(buf, end_time, j);
		memcpy(info, buf, 100);
		buf += 100;
		PKG_UINT32(msg, date, i);
		PKG_UINT32(msg, start_time, i);
		PKG_UINT32(msg, end_time, i);
		PKG_STR(msg, info, i, 100);
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

//PROTO_SEND_ONE_OF_TWO
int send_gift_oneoftwo_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t itm, type;
	int j = 0;
	UNPKG_UINT32(body, itm, j);
	UNPKG_UINT32(body, type, j);
	if (type > 1) {
		ERROR_RETURN(("Invalid command type"), -1);
	}
	uint32_t items[][2] = {{13049, 13050}, {13105, 13106}};
	if (itm != items[type][0] && itm != items[type][1]) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}
	int  i = 0;
	uint8_t  buff[256];
	PKG_H_UINT32(buff, 2, i);
	PKG_H_UINT32(buff, items[type][0], i);
	PKG_H_UINT32(buff, items[type][1], i);
	*(uint32_t*)p->session = itm;
	return send_request_to_db(SVR_QUERY_ITEM_COUNT, p, i, buff, p->id);
}

int do_send_gift_oneoftwo(sprite_t * p, char * buf, int len)
{
	uint32_t count, itm;
	int j = 0;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN(len, 4 + count*8);
	int loop = 0;
	for (loop = 0; loop < count; loop++) {
		uint32_t itmid, itmcn;
		UNPKG_H_UINT32(buf, itmid, j);
		UNPKG_H_UINT32(buf, itmcn, j);
		if (count > 0) {
			return send_to_self_error(p, p->waitcmd, -ERR_already_has_one_of_two, 1);
		}
	}
	itm = *(uint32_t*)p->session;
	db_buy_itm_freely(0, p->id, itm, 1);
	response_proto_uint32(p, p->waitcmd, itm, 0);
	return 0;
}

int send_gift_twooffour_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t type;
	int j = 0;
	UNPKG_UINT32(body, type, j);
	uint32_t items[][2] = {
						{13056, 13064},
						{13055, 13057}
						};
	if (type != 0 && type != 1) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
	}
	int  i = 0;
	uint8_t  buff[256];
	PKG_H_UINT32(buff, 2, i);
	PKG_H_UINT32(buff, items[type][0], i);
	PKG_H_UINT32(buff, items[type][1], i);
	*(uint32_t*)p->session = type;
	return send_request_to_db(SVR_QUERY_ITEM_COUNT, p, i, buff, p->id);
}

int do_send_gift_twooffour(sprite_t * p, char * buf, int len)
{
	uint32_t count, type;
	int j = 0;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN(len, 4 + count*8);
	int loop = 0;
	for (loop = 0; loop < count; loop++) {
		uint32_t itmid, itmcn;
		UNPKG_H_UINT32(buf, itmid, j);
		UNPKG_H_UINT32(buf, itmcn, j);
		if (count > 0) {
			return send_to_self_error(p, p->waitcmd, -ERR_already_has_one_of_two, 1);
		}
	}
	type= *(uint32_t*)p->session;
	uint32_t items[][2] = {
						{13056, 13064},
						{13055, 13057}
						};
	db_buy_itm_freely(0, p->id, items[type][0], 1);
	db_buy_itm_freely(0, p->id, items[type][1], 1);
	response_proto_uint32_uint32(p, p->waitcmd, items[type][0], items[type][1], 0);
	return 0;
}

int set_photo_dress_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen, 8);
	int j = 0, i = 0, loop;
	uint32_t type, count;
	UNPKG_UINT32(body, type, j);
	UNPKG_UINT32(body, count, j);
	CHECK_BODY_LEN(bodylen, 8 + 4 * count);
	uint8_t type_8, count_8;
	type_8 = (uint8_t)type;
	count_8 = (uint8_t)count;
	PKG_H_UINT8(msg, type_8, i);
	PKG_H_UINT8(msg, count_8, i);
	for (loop = 0; loop < count; loop ++) {
		uint32_t tmp;
		UNPKG_UINT32(body, tmp, j);
		PKG_H_UINT32(msg, tmp, i);
	}
	for (; loop < 12; loop ++) {
		PKG_H_UINT32(msg, 0, i);
	}
	return send_request_to_db(SVR_PROTO_SET_PHOTO_DRESS, p, i, msg, p->id);
}
int set_photo_dress_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int get_photo_dress_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t uid;
	int j = 0;
	UNPKG_UINT32(body, uid, j);
	return send_request_to_db(SVR_PROTO_GET_PHOTO_DRESS, p, 0, NULL, uid);
}
int get_photo_dress_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 50);
	int j = 0, i = sizeof(protocol_t), loop;
	uint8_t type, count;
	UNPKG_H_UINT8(buf, type, j);
	UNPKG_H_UINT8(buf, count, j);
	uint32_t type_32, count_32;
	type_32 = (uint32_t)type;
	count_32 = (uint32_t)count;
	PKG_UINT32(msg, type_32, i);
	PKG_UINT32(msg, count_32, i);
	for (loop = 0; loop < count; loop ++) {
		uint32_t tmp;
		UNPKG_H_UINT32(buf, tmp, j);
		PKG_UINT32(msg, tmp, i);
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}


int echo_set_num_type_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 12);
	uint32_t num, type, succ;
	int j = 0, i = 0;
	UNPKG_UINT32(body, succ, j);
	UNPKG_UINT32(body, num, j);
	UNPKG_UINT32(body, type, j);
	*(uint32_t*)p->session = succ;
	PKG_H_UINT32(msg, succ, i);
	PKG_H_UINT32(msg, num, i);
	PKG_H_UINT32(msg, type, i);
	DEBUG_LOG("echo_set_num_type_cmd[uid=%u cmd=%u]", p->id, p->waitcmd);
	return send_request_to_db(SVR_PROTO_SET_NUM_TYPE, p, i, msg, p->id);

}

int echo_set_num_type_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	DEBUG_LOG("echo_set_num_type_callback[uid=%u cmd=%u flag=%u]", p->id, p->waitcmd, *(uint32_t*)p->session);
	response_proto_uint32(p, p->waitcmd, *(uint32_t*)p->session, 0);
	return 0;
}

int echo_get_num_type_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	return send_request_to_db(SVR_PROTO_GET_NUM_TYPE, p, 0, NULL, p->id);
}

int echo_get_num_type_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 12);
	uint32_t succ, num, type;
	int j = 0, i = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, succ, j);
	UNPKG_H_UINT32(buf, num, j);
	UNPKG_H_UINT32(buf, type, j);
	PKG_UINT32(msg, succ, i);
	PKG_UINT32(msg, num, i);
	PKG_UINT32(msg, type, i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int broadcast_value_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t type;
	int j = 0, i = sizeof(protocol_t);
	UNPKG_UINT32(body, type, j);
	PKG_UINT32(msg, type, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);
	return 0;
}

int get_all_tujian_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	return send_request_to_db(SVR_PROTO_GET_TUJIAN, p, 0, NULL, p->id);
}

int get_all_tujian_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count;
	int j = 0, i = sizeof(protocol_t), loop;
	UNPKG_H_UINT32(buf, count, j);
	PKG_UINT32(msg, count, i);
	DEBUG_LOG("get_all_tujian_callback[uid=%u cmd=%u flag=%u]", p->id, p->waitcmd, count);
	CHECK_BODY_LEN(len, 4 + 4 * count);
	for (loop = 0; loop < count; loop ++) {
		uint32_t itmid;
		UNPKG_H_UINT32(buf, itmid, j);
		PKG_UINT32(msg, itmid, i);
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int weighing_fish_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t itmid;
	int j = 0;
	UNPKG_UINT32(body, itmid, j);
	if (itmid != 190239 && itmid != 190250 && itmid != 190261 && itmid != 190263 && itmid != 190265 && itmid != 190267) {
		ERROR_RETURN(("weigh fish\t[%u %u]", p->id, itmid), -1);
	}

	DEBUG_LOG("WEIGH FISH\t[%u %u]", p->id, itmid);
	*(uint32_t*)p->session = itmid;
	return db_single_item_op(p, p->id, itmid, 1, 0);
}

int add_weight_to_total(sprite_t * p)
{
	uint32_t itmid = *(uint32_t*)p->session;
	uint32_t weight = 0;

	if (itmid == 190239) {
		weight = 10 + rand()%5;
	} else if (itmid == 190250) {
		weight = 5 + rand()%5;
	} else if (itmid == 190261) {
		weight = 1 + rand()%2;
	} else if (itmid == 190263) {
		weight = 2 + rand()%3;
	} else if (itmid == 190265) {
		weight = 5 + rand()%5;
	} else if (itmid == 190267) {
		weight = 20 + rand()%10;
	}
	*(uint32_t*)p->session = weight;
	uint32_t buff[2];
	buff[0] = itmid;
	buff[1] = weight;
	return send_request_to_db(SVR_PROTO_WEIGHTING, p, 8, buff, p->id);
}

int weighing_fish_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t total_weight = *(uint32_t*)buf;
	uint32_t old_weight = p->tmp_fish_weight;
	p->tmp_fish_weight = total_weight;
	uint32_t itmid = 0;
	if (old_weight < 30 && total_weight >= 30) {
		itmid = 160222;
	} else if (old_weight < 80 && total_weight >= 80) {
		itmid = 160216;
	} else if (old_weight < 150 && total_weight >= 150) {
		itmid = 160233;
	} else if (old_weight < 250 && total_weight >= 250) {
		itmid = 160252;
	} else if (old_weight < 400 && total_weight >= 400) {
		itmid = 160600;
	}
	if (itmid != 0) {
		db_buy_itm_freely(0, p->id, itmid, 1);
	}
	if (itmid == 160600 && p->tiles) {
		response_proto_uint32_uint32_map(p->tiles->id, PROTO_BROADCAST_NUM, 160600, p->id);
	}

	*(uint32_t*)(p->session + 4) = itmid;
	return send_request_to_db(SVR_PROTO_UPDATE_MAX_WEIGHT, p, 4, &total_weight, p->id);
}

int update_max_fish_weight_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t uid = *(uint32_t*)buf;
	uint32_t max_weight = *(uint32_t*)(buf + 4);
	uint32_t this_weight = *(uint32_t*)p->session;
	uint32_t itmid =  *(uint32_t*)(p->session + 4);

	DEBUG_LOG("WEIGHT FISH CALLBACK\t[%u %u %u %u %u %u]", p->id, itmid, this_weight, p->tmp_fish_weight, uid, max_weight);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, itmid, i);
	PKG_UINT32(msg, p->tmp_fish_weight, i);
	PKG_UINT32(msg, this_weight, i);
	PKG_UINT32(msg, uid, i);
	PKG_UINT32(msg, max_weight, i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}
int finshing_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	if (!p->tiles || (p->tiles->id != 5 && p->tiles->id != 112))
		ERROR_RETURN(("not in right map"), -1);
	uint32_t pro = rand()%100;
	uint32_t fish_id = 0;
	uint32_t fish_array[5][2] = {{190239, 1270003}, {190261, 1270022}, {190265, 1270024}, \
								 {190263, 1270023}, {190267, 1270025}};
	if (pro < 40) {
		//fish_id = fish_array[rand()%3][rand()%2];
		if (rand() % 2 < 1) {
			fish_id = 190265;
		} else {
			fish_id = fish_array[rand()%3][rand()%2];
		}
		//fish_id = fish_array[rand()%3][0];
	} else if (pro < 45) {
		fish_id = fish_array[3][rand()%2];
	} else if (pro < 50) {
		fish_id = fish_array[4][rand()%2];
	}
	else
	{
		fish_id = 0;
	}

	*(uint32_t*)p->session = fish_id;
	return db_set_sth_done(p, 212, 20, p->id);
}

int get_total_fish_weight_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	return send_request_to_db(SVR_PROTO_GET_ALL_FISH, p, 0, NULL, p->id);
}

int get_ring_from_other_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	if (!p->tiles || (!IS_HOUSE_MAP(p->tiles->id) && !IS_HOUSE1_MAP(p->tiles->id) && !IS_HOUSE2_MAP(p->tiles->id))) {
		ERROR_RETURN(("not in home[%u]", p->id), -1);
	}
	return send_request_to_db(SVR_PROTO_GET_RING_FRON_OTHER, p, 4, &p->id, GET_UID_IN_HOUSE(p->tiles->id));
}

int get_ring_from_other_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	db_single_item_op(0, p->id, 190515, 1, 1);
	response_proto_uint32(p, p->waitcmd, 190515, 0);
	return 0;
}

int pay_money_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen,4);

	int yxb,i = 0;
	UNPKG_UINT32(body,yxb,i);

	if(yxb <= 0 || yxb > 2000 )
		ERROR_RETURN(("[%u] payed money is %d", p->id,yxb), -1);

	*((int*)p->session) = yxb;

	return db_set_sth_done(p, 218, 5, p->id);
}

int get_penguin_egg_hatch_times_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	return send_request_to_db(SVR_PROTO_GET_EGG_HATCH_TIMES,p,0,NULL, p->id);
}

int get_penguin_egg_hatch_times_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len,8);

	response_proto_uint32_uint32(p, p->waitcmd, *(uint32_t*)(buf), *(uint32_t*)(buf+4), 0);

	return 0;
}

int set_penguin_egg_hatch_times_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen,8);
	int flag= 0,data = 0,i=0;

	UNPKG_UINT32(body,flag,i);
	UNPKG_UINT32(body,data,i);

	*(uint32_t*)p->session = flag;

	if (flag == 0) {
		return db_set_sth_done(p, 219, 1, p->id);
	}

	uint32_t buff[2] = {1,data};
	return send_request_to_db(SVR_PROTO_SET_EGG_HATCH_TIMES,p,8,buff, p->id);
}

int set_penguin_egg_hatch_times_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len,8);
	int times= 0,data = 0,i=0;

	UNPKG_H_UINT32(buf,times,i);
	UNPKG_H_UINT32(buf,data,i);

	uint32_t flag = *(uint32_t*)p->session;

	DEBUG_LOG("EGG [id=%d flag=%d times=%d data=%d]", p->id, flag, times, data);

	if (flag == 0 && times == 3) {
		db_single_item_op(0, p->id, 190580, 1, 0);
		db_single_item_op(0, p->id, 1270038, 1, 1);
		response_proto_uint32(p, p->waitcmd, 1270038, 0);
		return 0;
	}

	response_proto_uint32(p, p->waitcmd, 0, 0);
//	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int buy_some_cloths_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	typedef struct {
		uint32_t itm_id;
		uint32_t itm_cnt;
		uint32_t itm_max;
	}	buy_item_info_t;

	int i = 0, len = 0;
	char buf[2048]={0};
	uint32_t records = 0;
	uint32_t money = 0, total = 0;
	buy_item_info_t *pi = (buy_item_info_t*)(buf + 12);

	UNPKG_UINT32(body, records, i);

	if(records > 32) {
		ERROR_RETURN(("[%u] records %d > 32", p->id,records), -1);
	}

	CHECK_BODY_LEN(bodylen,records * 8 + 4);

	for (len = 0;len < records ; len++,pi++) {
		UNPKG_UINT32(body, pi->itm_id, i);
		UNPKG_UINT32(body, pi->itm_cnt, i);

		item_kind_t* kind = find_kind_of_item(pi->itm_id);
		if (!kind || kind->kind != SUIT_ITEM_KIND) {
			ERROR_LOG( "Item %u Kind Not SUIT_ITEM_KIND: uid=%u kind=%u", pi->itm_id, p->id, (kind ? kind->kind : 9999));
			return send_to_self_error(p, p->waitcmd, -ERR_cannot_buy_itm, 1);
		}

		item_t* itm_prop = get_item_prop(pi->itm_id);
		if (!itm_prop || !itm_buyable(p, itm_prop)) {
			ERROR_LOG( "Item %u not found or Unbuyable: uid=%u itmflag=0x%X",
						pi->itm_id, p->id, (itm_prop ? itm_prop->tradability : 0));
			return send_to_self_error(p, p->waitcmd, -ERR_cannot_buy_itm, 1);
		}

		pi->itm_max = itm_prop->max;
		if (pi->itm_cnt > itm_prop->max) {
			ERROR_LOG( "Item %u too Much: uid=%u cnt=%d max=%d", pi->itm_id, p->id, pi->itm_cnt,itm_prop->max);
			return send_to_self_error(p, p->waitcmd, -ERR_cannot_buy_itm, 1);
		}

		money = pi->itm_cnt * itm_prop->price;

		event_t* ev = get_event(5);
		if (ev && ev->status) {
			money *= itm_prop->discount;
		}

		total += money;
	}

	*(uint32_t*)buf = 0;
	*(uint32_t*)(buf + 4) = total;
	*(uint32_t*)(buf + 8) = records;

	len = 12 + records * 12;
	return send_request_to_db(SVR_PROTO_BUY_SOME_ITEMS,p,len,buf,p->id);
}

int buy_some_cloths_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len,4);

	int i = 0;
	uint32_t money;

	UNPKG_H_UINT32(buf,money,i);

	p->yxb = money;

	response_proto_uint32(p,p->waitcmd, p->yxb, 0);

	return 0;
}

int get_rand_times_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	return send_request_to_db(SVR_PROTO_RAND_TIMES, p, 0, NULL, p->id);
}

int get_rand_times_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len,8);
	DEBUG_LOG("RAND TIMES CALLBACK [id=%d times=%d data=%d]", p->id, *(uint32_t*)(buf), *(uint32_t*)(buf+4));
	response_proto_uint32_uint32(p, p->waitcmd, *(uint32_t*)(buf), *(uint32_t*)(buf+4), 0);
	return 0;
}

int rand_item_want_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t buff[1] = {190582};

	return send_request_to_db(SVR_PROTO_RAND_ITEM_WANT, p, 4, buff, p->id);
}

int rand_item_want_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len,4);

	int i = 0;
	uint32_t itmeid = 0;

	UNPKG_H_UINT32(buf,itmeid,i);

	if (itmeid != 0) {
		db_single_item_op(0, p->id, itmeid, 1, 1);
	}

	response_proto_uint32(p, p->waitcmd, itmeid, 0);
	return 0;
}

int get_pet_attires_count_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int i=0,n;
	uint32_t records=0;
	uint32_t buf[128] = {0};

	UNPKG_UINT32(body, records, i);

	if (records > 32) {
		ERROR_LOG("[%u] records %d > 32", p->id,records);
		return send_to_self_error(p, p->waitcmd, -ERR_record_count_limited, 1);
	}

	CHECK_BODY_LEN(bodylen, (records * 4 + 4));

	for(n = 0; n < records; n++) {
		UNPKG_UINT32(body, buf[n+1], i);
	}

	buf[0] = records;
	return send_request_to_db(SVR_PROTO_GET_PET_ATTIRE_CNT, p, records * 4 + 4, buf, p->id);
}

int get_pet_attries_count_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i=0;
	uint32_t records;

	int j = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, records, i);
	PKG_UINT32(msg, records, j);

	CHECK_BODY_LEN(len,records * 8 + 4);

	int n;
	for(n = 0; n < records; n++) {
		uint32_t itmid, cnt;
		UNPKG_H_UINT32(buf, itmid, i);
		UNPKG_H_UINT32(buf, cnt, i);

		DEBUG_LOG("ATTIRE COUNT [id=%d ITM=%d CNT=%d]", p->id,itmid, cnt);

		PKG_UINT32(msg, itmid, j);
		PKG_UINT32(msg, cnt, j);
	}

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int get_items_cnt_in_bag_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int i=0,n;
	uint32_t records=0;
	uint32_t buff[128] = {0};

	CHECK_VALID_ID(p->id);

	UNPKG_UINT32(body, records, i);

	if (records > 32) {
		ERROR_LOG("[%u] records %d > 32", p->id,records);
		return send_to_self_error(p, p->waitcmd, -ERR_record_count_limited, 1);
	}

	CHECK_BODY_LEN(bodylen, (records * 4 + 4));

	for(n = 0; n < records; n++) {
		UNPKG_UINT32(body, buff[n+1], i);
	}

	buff[0] = records;
	return send_request_to_db(SVR_PROTO_GET_ITEMS_CNT_IN_BAG, p, records * 4 + 4, buff, p->id);
}

int get_items_cnt_in_bag_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i=0;
	uint32_t records;

	int j = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, records, i);
	PKG_UINT32(msg, records, j);

	CHECK_BODY_LEN(len,records * 8 + 4);

	int n;
	for(n = 0; n < records; n++) {
		uint32_t itmid, cnt;
		UNPKG_H_UINT32(buf, itmid, i);
		UNPKG_H_UINT32(buf, cnt, i);

		PKG_UINT32(msg, itmid, j);
		PKG_UINT32(msg, cnt, j);
	}

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int get_10xiaomee_50times_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	return db_set_sth_done(p, 213, 100, p->id);
}

int get_christmas_gift_num_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	return send_request_to_db(SVR_PROTO_GET_GIFT_NUM, p, 0, NULL, p->id);
}

int get_christmas_gift_num_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	response_proto_uint32(p, p->waitcmd, *(uint32_t*)buf, 0); // gift num
	return 0;
}

int get_five_christmas_gifts_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	return db_set_sth_done(p, 215, 2, p->id);
}

int change_chris_gifts_num_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t remained_gift_cnt = *(uint32_t*)buf;
	DEBUG_LOG("REMAINED GIFT CNT\t[%u %u]", p->id, remained_gift_cnt);
	switch (p->waitcmd) {
	case PROTO_GET_CHRISTMAS_GIFTS:
		response_proto_uint32(p, p->waitcmd, remained_gift_cnt, 0); // gift num
		break;
	case PROTO_SEND_CHRIS_GIFTS_BILU:
		*(uint32_t*)p->session = remained_gift_cnt;
		db_add_op_to_chris(p, GET_UID_IN_HOUSE(p->tiles->id));
		break;
	case PROTO_SEND_CHRIS_GIFTS_TO_NPC:
		response_proto_uint32(p, p->waitcmd, remained_gift_cnt, 0); // gift num
		break;
	}
	return 0;
}

int send_chris_gift_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	if (!p->tiles || (!IS_HOUSE_MAP(p->tiles->id) && !IS_HOUSE1_MAP(p->tiles->id) && !IS_HOUSE2_MAP(p->tiles->id))) {
		ERROR_RETURN(("not in home[%u]", p->id), -1);
	}
	return db_change_gift_num(p, -1, p->id);
}

int add_op_to_chris_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	response_proto_uint32(p, p->waitcmd, *(uint32_t*)p->session, 0); // remained gift num
	return 0;
}

int send_chris_gift_to_npc_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	return db_change_gift_num(p, -1, p->id);
}

int get_chris_gift_from_bilu_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	return send_request_to_db(SVR_PROTO_GET_GIFT_FROM_BILU, p, 0, NULL, p->id);
}

int get_chris_gift_from_bilu_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t giftid[6] = {160446, 160447, 160448, 160449, 160624, 160625};
	CHECK_BODY_LEN(len, sizeof(chris_gift_list_t));

	uint32_t idx = rand() % 6;
	*(uint32_t*)p->session = giftid[idx];
	memcpy(p->session + 4, buf, sizeof(chris_gift_list_t));

	return db_buy_itm_freely(p, p->id, giftid[idx], 1);
}

int response_chris_gift(sprite_t* p)
{
	uint32_t itmid = *(uint32_t*)p->session;
	chris_gift_list_t* pcgl = (chris_gift_list_t*)(p->session + 4);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, itmid, j);
	PKG_UINT32(msg, pcgl->userid, j);
	PKG_STR(msg, pcgl->nick, j, 16);
	PKG_UINT32(msg, pcgl->color, j);
	PKG_UINT8(msg, pcgl->isvip, j);
	PKG_UINT32(msg, pcgl->op_stamp, j);
	init_proto_head(msg, p->waitcmd, j);

	return send_to_self(p, msg, j, 1);
}

int get_xiao_xiong_car_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	if(!p->driver_time) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_driver, 1);
	}
	/*
	   item_t* item = get_item_prop(1300009);
	if (!item|| !itm_buyable(p, item)) {
		ERROR_LOG( "Car 1300009 not found or Unbuyable: uid=%u itmflag=0x%X",
						p->id, (item? item->tradability : 0));
		return send_to_self_error(p, p->waitcmd, -ERR_cannot_buy_itm, 1);
	}
	*/
	DEBUG_LOG("GET CAR 1300010 [%u]", p->id);
	return db_single_item_op(p, p->id, 190533, 1, 0);
	//return db_add_car(p, 1300010, 0, p->id);
}

int gifts_rain(void* owner, void* data)
{
	map_t* mp = owner;
	//uint32_t wek_day = get_now_tm()->tm_wday;
	uint32_t hour = get_now_tm()->tm_hour;
	if (hour >= 18 && hour < 22) {
		if (mp->id == 1) {
			gifts_rain_pos1 = -1;
		} else if (mp->id == 2) {
			gifts_rain_pos2 = -1;
		} else if (mp->id == 3) {
			gifts_rain_pos3 = -1;
		} else {
			gifts_rain_pos59 = -1;
		}
		response_proto_head_map(mp->id, PROTO_GIFT_RAIN_BEGAIN);
	}
	//DEBUG_LOG("GIFT RAIN\t[%u %u]", hour, gifts_rain_pos1);
	ADD_TIMER_EVENT(mp, gifts_rain, 0, now.tv_sec + 120);
	return 0;
}

int collect_gift_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 4);
	if (!p->tiles) {
		ERROR_RETURN(("not in map\t[%u]", p->id), -1);
	}
	uint32_t itmid[4] = {190141, 190028, 180064, 180002};
	int i = 0;
	uint32_t posit;
	UNPKG_UINT32(body, posit, i);
	if (posit >= 4) {
		ERROR_RETURN(("bad gift position\t[%u]", p->id), -1);
	}
	switch (p->tiles->id) {
	case 1:
		if (!(gifts_rain_pos1 & (1 << posit))) {
			ERROR_LOG("position1 no gift\t[%u %u %u]", p->id, posit, gifts_rain_pos1);
			return send_to_self_error(p, p->waitcmd, -ERR_chris_gift_alrdy_collected, 1);
		}
		break;
	case 2:
		if (!(gifts_rain_pos2 & (1 << posit))) {
			ERROR_LOG("position2 no gift\t[%u %u %u]", p->id, posit, gifts_rain_pos2);
			return send_to_self_error(p, p->waitcmd, -ERR_chris_gift_alrdy_collected, 1);
		}
		break;
	case 3:
		if (!(gifts_rain_pos3 & (1 << posit))) {
			ERROR_LOG("position3 no gift\t[%u %u %u]", p->id, posit, gifts_rain_pos3);
			return send_to_self_error(p, p->waitcmd, -ERR_chris_gift_alrdy_collected, 1);
		}
		break;
	case 59:
		if (!(gifts_rain_pos59 & (1 << posit))) {
			ERROR_LOG("position59 no gift\t[%u %u %u]", p->id, posit, gifts_rain_pos59);
			return send_to_self_error(p, p->waitcmd, -ERR_chris_gift_alrdy_collected, 1);
		}
		break;
	default:
		ERROR_RETURN(("bad map id\t[%u]", p->id), -1);
	}

	uint32_t itm = itmid[rand()%4];
	*(uint32_t*)p->session = posit;
	*(uint32_t*)(p->session + 4) = itm;
	return db_set_sth_done(p, 217, 40, p->id);
}

int gift_disappear(sprite_t* p)
{
	uint32_t posit = *(uint32_t*)p->session;
	uint32_t itmid = *(uint32_t*)(p->session + 4);
	switch (p->tiles->id) {
	case 1:
		gifts_rain_pos1 &= ~(1 << posit);
		break;
	case 2:
		gifts_rain_pos2 &= ~(1 << posit);
		break;
	case 3:
		gifts_rain_pos3 &= ~(1 << posit);
		break;
	case 59:
		gifts_rain_pos59 &= ~(1 << posit);
		break;
	default:
		ERROR_RETURN(("bad map id\t[%u]", p->id), -1);
	}

	DEBUG_LOG("COLLECT GIFT\t[%u %u]", p->id, itmid);
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, posit, j);
	PKG_UINT32(msg, p->id, j);
	PKG_UINT32(msg, itmid, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int add_nengliangxing_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 4);

	int i = 0;
	uint32_t cnt;
	UNPKG_UINT32(body, cnt, i);
	if (cnt > 5) {
		ERROR_RETURN(("bad neng liang xing cnt\t[%u %u]", p->id, cnt), -1);
	}

	*(uint32_t*)p->session = cnt;
	uint32_t buff[2];
	buff[0] = cnt;
	buff[1] = 0;
	if (ISVIP(p->flag)) {
		buff[1] = 1;
	}
	return send_request_to_db(SVR_PROTO_ADD_NENGLIANG_CNT, p, 8, buff, p->id);
}

int add_nengliang_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 12);
	//uint32_t total_cnt = *(uint32_t*)buf;
	//uint32_t today_cnt = *(uint32_t*)(buf + 4);
	uint32_t real_cnt = *(uint32_t*)(buf + 8);
	//uint32_t cnt = *(uint32_t*)p->session;
	*(uint32_t*)(p->session + 4) = real_cnt;
	return db_single_item_op(p, p->id, 190531, real_cnt, 1);
}

int get_nengliang_cnt_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_GET_NENGLIANG_CNT, p, 0, NULL, p->id);
}

int get_nengliang_cnt_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t total_cnt = *(uint32_t*)buf;
	uint32_t today_cnt = *(uint32_t*)(buf + 4);

	response_proto_uint32_uint32(p, p->waitcmd, total_cnt, today_cnt, 0);
	return 0;
}

/*
 *@ response  lahm food
 */
int resp_lahm_food_mechine(sprite_t* p)
{
	uint32_t itemid = *(uint32_t*)p->session;
	uint32_t itm_cnt = *(uint32_t*)(p->session + 4);

	DEBUG_LOG("LAHM MACHINE CALLBACK\t[%u %u]", p->id, itemid);
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, itemid, j);
	PKG_UINT32(msg, 1, j);
	PKG_UINT32(msg, itm_cnt, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

/*
 *@ get  lahm food
 */
int do_lahm_food_mechine(sprite_t* p)
{
	uint32_t itemid = *(uint32_t*)p->session;
	uint32_t itm_cnt = *(uint32_t*)(p->session + 4);

	return db_buy_itm_freely(p, p->id, itemid, itm_cnt);
}

/*
 *@ get lahm food from lahm machine
 */
int lahm_food_machine_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
typedef struct do_type{
	uint32_t type;
	uint32_t max;
	uint32_t itmid;
	uint32_t itm_cnt;
}do_type_t;
	do_type_t lahm_food_machine[3] = {{216, 1, 180015, 1}, {216, 1, 180069, 1}, {216, 1, 180070, 1}};

	CHECK_VALID_ID(p->id);

	uint32_t indx = rand() % 3;

	*(uint32_t*)p->session = lahm_food_machine[indx].itmid;
	*(uint32_t*)(p->session + 4) = lahm_food_machine[indx].itm_cnt;
	DEBUG_LOG("LAHM FOOD MACH\t[%u %u]", p->id, lahm_food_machine[indx].itmid);
	return db_set_sth_done(p, lahm_food_machine[indx].type, lahm_food_machine[indx].max, p->id);
}

int get_dajiaoyin_reward_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_VALID_ID(p->id);

	return db_chk_itm_cnt(p, 190078, 190117);
}

int do_get_dajiaoyin_reward(sprite_t* p, uint32_t count)
{
	DEBUG_LOG("DAJIAOYIN CNT\t[%u %u]", p->id, count);
	if (count == 0) {
		response_proto_uint32(p, p->waitcmd, 0, 0);
	} else if (count >= 36) {
		db_single_item_op(0, p->id, 1270018, 1, 1);
		db_single_item_op(0, p->id, 190533, 1, 1);
		db_single_item_op(0, p->id, 190534, 1, 1);
		db_single_item_op(0, p->id, 13163, 1, 1);
		db_buy_itm_freely(0, p->id, 160635, 1);
		response_proto_uint32(p, p->waitcmd, 5, 0);
	} else if (count >= 30) {
		db_single_item_op(0, p->id, 1270018, 1, 1);
		db_single_item_op(0, p->id, 190533, 1, 1);
		db_single_item_op(0, p->id, 13163, 1, 1);
		db_buy_itm_freely(0, p->id, 160635, 1);
		response_proto_uint32(p, p->waitcmd, 4, 0);
	} else if (count >= 20) {
		db_single_item_op(0, p->id, 1270018, 1, 1);
		db_single_item_op(0, p->id, 190533, 1, 1);
		db_single_item_op(0, p->id, 13163, 1, 1);
		response_proto_uint32(p, p->waitcmd, 3, 0);
	} else if (count >= 10) {
		db_single_item_op(0, p->id, 1270018, 1, 1);
		db_single_item_op(0, p->id, 190533, 1, 1);
		response_proto_uint32(p, p->waitcmd, 2, 0);
	} else if (count >= 1) {
		db_single_item_op(0, p->id, 1270018, 1, 1);
		response_proto_uint32(p, p->waitcmd, 1, 0);
	}

	return db_del_multi_itm(0, 190078, 190116, p->id);
}

int set_photo_dahezhao_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen, 4);
	int j = 0, loop;
	uint32_t count;
	UNPKG_UINT32(body, count, j);
	CHECK_INT_LE(count, 12);
	CHECK_BODY_LEN(bodylen, 4 + 4 * count);

	uint32_t buff[13] = {0};
	buff[0] = count;
	for (loop = 0; loop < count; loop++) {
		UNPKG_UINT32(body, buff[loop+1], j);
	}

	return send_request_to_db(SVR_PROTO_SET_PHOTO_DAHEZHAO, p, (4+4*count), buff, p->id);
}

int set_photo_dahezhao_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int get_photo_dahezhao_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t uid;
	int j = 0;
	UNPKG_UINT32(body, uid, j);
	CHECK_VALID_ID(uid);

	return send_request_to_db(SVR_PROTO_GET_PHOTO_DAHEZHAO, p, 0, NULL, uid);
}

int get_photo_dahezhao_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	int j = 0;
	uint32_t count;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_INT_LE(count, 12);
	CHECK_BODY_LEN(len, 4 + 4 * count);

	int loop, i = sizeof(protocol_t);
	PKG_UINT32(msg, count, i);
	for (loop = 0; loop < count; loop ++) {
		uint32_t tmp;
		UNPKG_H_UINT32(buf, tmp, j);
		PKG_UINT32(msg, tmp, i);
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}


int set_type_photo_hezhao_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen, 8);
	int j = 0, loop;
	uint32_t type;
	uint32_t count;
	UNPKG_UINT32(body, type, j);
	UNPKG_UINT32(body, count, j);
	CHECK_INT_LE(count, 12);
	CHECK_BODY_LEN(bodylen, 8 + 4 * count);

	uint32_t buff[14] = {0};
	buff[0] = type;
	buff[1] = count;
	for (loop = 0; loop < count; loop++) {
		UNPKG_UINT32(body, buff[loop+2], j);
	}

	return send_request_to_db(SVR_PROTO_SET_TYPE_PHOTO_HEZHAO, p, (8+4*count), buff, p->id);
}

int set_type_photo_hezhao_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int get_type_photo_hezhao_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t uid = 0;
	uint32_t type = 0;
	int j = 0;
	UNPKG_UINT32(body, uid, j);
	CHECK_VALID_ID(uid);
	UNPKG_UINT32(body, type, j);

	return send_request_to_db(SVR_PROTO_GET_TYPE_PHOTO_HEZHAO, p, 4, &type, uid);
}

int get_type_photo_hezhao_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 8);

	int j = 0;
	uint32_t type;
	uint32_t count;
	UNPKG_H_UINT32(buf, type, j);
	UNPKG_H_UINT32(buf, count, j);
	CHECK_INT_LE(count, 12);
	CHECK_BODY_LEN(len, 8 + 4 * count);

	int loop, i = sizeof(protocol_t);
	PKG_UINT32(msg, type, i);
	PKG_UINT32(msg, count, i);
	for (loop = 0; loop < count; loop ++) {
		uint32_t tmp;
		UNPKG_H_UINT32(buf, tmp, j);
		PKG_UINT32(msg, tmp, i);
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}


int exchg_mibi_coupon_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_VALID_ID(p->id);

	DEBUG_LOG("EXCHG MIBI COUPON\t[%u]", p->id);
	return db_single_item_op(p, p->id, 190534, 1, 0);
}

int fetch_jinian_jiaoyin_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 4);

	int j = 0;
	uint32_t itmid;
	UNPKG_UINT32(body, itmid, j);
	if (itmid < 190078 || itmid > 190080) {
		ERROR_RETURN(("bad itm id\t[%u %u]", p->id, itmid), -1);
	}

	*(uint32_t*)p->session = itmid;
	//1000000018, 1000000019, 1000000020
	uint32_t type = 1000000018 + itmid - 190078;
	DEBUG_LOG("FETCH JINIAN JIAOYIN\t[%u %u]", p->id, itmid);
	return db_set_sth_done(p, type, 1, p->id);
}

/* @brief make request to db, get day and total carry num
 */
int mole_get_carry_rock_num_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_GET_CRRRY_ROCK_NUM, p, bodylen, body, p->id);
}

/* @brief get day and total carry num from db
 */
int mole_get_carry_rock_num_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);

	uint32_t day_num, total_num;

	day_num = *(uint32_t *)buf;
	total_num = *(uint32_t *)(buf + 4);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, day_num, i);
	PKG_UINT32(msg, total_num, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_self(p, msg, i, 1);
	return 0;
}

/* @brief set carry num to db
 */
int mole_set_carry_rock_num_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 0);

	return send_request_to_db(SVR_PROTO_SET_CRRRY_ROCK_NUM, p, bodylen, body, p->id);
}

/* @brief call this after db set carry num, if total num eq some times, give item
 */
int mole_set_carry_rock_num_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN(len, 8);

	uint32_t day_num, total_num, itemid = 0;

	day_num = *(uint32_t *)buf;
	total_num = *(uint32_t *)(buf + 4);

	if (total_num == 3) {
		itemid = 13245;
	} else if (total_num == 6) {
		itemid = 13244;
	} else if (total_num == 9) {
		itemid = 13241;
	} else if (total_num == 12) {
		itemid = 13243;
	} else if (total_num == 15) {
		itemid = 13242;
	}else {
		/*0 not give any thing*/
		itemid = 0;
	}

	if (itemid != 0) {
		db_single_item_op(0, p->id, itemid, 1, 1);
		DEBUG_LOG("ADD ITM\t[%u %u %u]", p->id, itemid, total_num);
	}
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, itemid, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_self(p, msg, i, 1);

	return 0;
}

/* @brief quit dog from jy
 */
int quit_machine_dog_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	int j = 0;
	uint32_t dog_id = 0;
	UNPKG_UINT32(body, dog_id, j);
	return send_request_to_db(SVR_PROTO_QUIT_MACHINE_DOG, p, 4, &dog_id, p->id);
}

/* @brief callback for quit dog
 */
int quit_machine_dog_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/* @brief feed battery to dog
 */
int feed_machine_dog_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 12);

	int j = 0;
	uint32_t dog_id, itemid, count;
	UNPKG_UINT32(body, dog_id, j);
	UNPKG_UINT32(body, itemid, j);
	UNPKG_UINT32(body, count, j);
	switch(dog_id) {
		case 1320001:
			{
				itemid = 190639;
				break;
				}
		case 1320002:
			{
				itemid = 190813;
				break;
			}
		default:
			ERROR_RETURN(("dog id is wrong %u %u", dog_id, p->id), -1);
	}
	uint32_t db_buf[] = {dog_id, itemid, count};
	return send_request_to_db(SVR_PROTO_FEED_MACHINE_DOG, p, sizeof(db_buf), db_buf, p->id);
}

/* @brief callback for feed dog
 */
int feed_machine_dog_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	int j = sizeof(protocol_t);
	uint32_t remain_time = *(uint32_t *)buf;
	PKG_UINT32(msg, remain_time, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
	return 0;
}

/* @brief get machine dog do thing info
 */
int get_machine_dog_do_thing_info_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	int j = 0;
	uint32_t id, dog_id;
	UNPKG_UINT32(body, id, j);
	UNPKG_UINT32(body, dog_id, j);
	CHECK_VALID_ID(id);
	return send_request_to_db(SVR_PROTO_GET_DOG_DO_THING_INFO, p, 4, &dog_id, id);
}

/* @brief callback for get machine do thing info
 */
int get_machine_dog_do_thing_info_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	typedef struct dog_do_info {
		uint32_t dog_id;
		uint32_t remain_time;
		uint32_t plant_water_count;
		uint32_t insect_kill_count;
		uint32_t animal_water_count;
		uint32_t animal_catch_count;
		uint32_t flag;
	} dog_do_info_t;

	CHECK_BODY_LEN(len, 28);
	dog_do_info_t *temp = (dog_do_info_t *)buf;

	if (temp->flag == 1)
	{
		send_postcard("善哉阿七",0,p->id,1000190,"",0);
	}

	if (temp->flag == 2)
	{
		send_postcard("善哉阿七",0,p->id,1000195,"",0);
	}

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, temp->dog_id, j);
	PKG_UINT32(msg, temp->remain_time, j);
	PKG_UINT32(msg, temp->plant_water_count, j);
	PKG_UINT32(msg, temp->insect_kill_count, j);
	PKG_UINT32(msg, temp->animal_water_count, j);
	PKG_UINT32(msg, temp->animal_catch_count, j);

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

/* @brief get architect exp
 */
int get_architect_exp_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	response_proto_uint32_uint32(p, p->waitcmd, p->profession[7], p->architect_exp, 0);
	return 0;
}

/* @brief get pipi douya
 */
int get_pipi_or_douya_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t j = 0, itemid;
	UNPKG_UINT32(body, itemid, j);
	uint32_t day_limit_type;
	if (rand() % 100 > 50) {
		response_proto_uint32(p, p->waitcmd, 0, 0);
		return 0;
	}
	*(uint32_t *)p->session = itemid;
	if (itemid == 1270046) { /*pipi*/
		day_limit_type = 10001;
	} else if (itemid == 1270047) { /*douya*/
		if (!ISVIP(p->flag)) {
			return send_to_self_error(p, p->waitcmd, -ERR_vip_npc_task, 1);
		}
		day_limit_type = 10002;
	} else {
		ERROR_RETURN(("WRONG TYPE %u %d", p->id, itemid), -1);
	}
	db_set_sth_done(p, day_limit_type, 3, p->id);
	return 0;
}

/* @brief add pipi or douya
 */
int add_pipi_or_douya(sprite_t *p)
{
	uint32_t itemid = *(uint32_t *)p->session;
	db_single_item_op(0, p->id, itemid, 1, 1);
	uint32_t animal_count = 1;
	msglog(statistic_logfile, 0x0201D000 + (itemid - 1270000), get_now_tv()->tv_sec, &animal_count, sizeof(uint32_t));
	response_proto_uint32(p, p->waitcmd, itemid, 0);
	return 0;
}

/* @brief 如果某位没设置，就设置这位，否则返回0
 */
int set_only_one_bit(sprite_t *p, uint32_t index)
{
	if( index > sizeof(p->only_one_flag)*8 )
	{
		ERROR_LOG( "Set Only One Bit Out of Range Value:[%d], Max:[%lu]", index, sizeof(p->only_one_flag)*8 );
		return 0;
	}
	int n = (index-1) / 32;
	int b = (index-1) % 32;
	if (p->only_one_flag[n] & (1 << b)) {
		return 0;
	}
	p->only_one_flag[n] |= (0x01 << b);

	send_request_to_db(SVR_PROTO_SET_ONLY_ONE_FLAG, NULL, sizeof(index), &index, p->id);
	return 1;
}

/* @brief 查询指定位的值
 */
int check_only_one_bit(sprite_t *p, uint32_t index)
{
	if( index > sizeof(p->only_one_flag)*8 )
	{
		ERROR_LOG( "check_only_one_bit Out of Range Value:[%d], Max:[%lu]", index, sizeof(p->only_one_flag)*8 );
		return 0;
	}
	int n = (index-1) / 32;
	int b = (index-1) % 32;
	return p->only_one_flag[n] & (1 << b);
}

/* @brief do something only do once
 */
int set_only_one_flag_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t j = 0, index;
	UNPKG_UINT32(body, index, j);
	/*1bit 表示是否领取了大礼包，2bit表示是否领取了离线大礼包，3bit表示选择依莲，4bit表示选择花嫂(屋委会)
	 *5bit表示是否完成了菲力餐厅的体验,6bit表示是否领取过食博奖励,7bit表示是否领取过建餐厅的礼包（5000豆豆）
	 *8bit表示拉姆运动会是否领取过奖杯,9bit表示是否从土林长老那边领取过卡牌,10bit表示是否领取过龙蛋
	 *11,12,13,14餐厅事件,15表示是否花了10000万摩尔豆开启了飞龙背包,16表示是否领取过开学大礼包,17夜煞龙蛋,18,19,20,21
	 *22黑龙在现送的五个礼包
	 *23表示领取过中秋月饼,swap.xml ID=33
	 *24拉姆教棒
	 *25皇家骑士派对，皇家骑士果实
	 *26小拉姆食盆任务奖励摩尔豆
	 *27小拉姆的食盆任务扣除拉姆食盆,奖励奇怪的小鸭子蛋
	 *28发威尔大礼包邮件
	 *29漂亮的新装,任务中送400摩尔豆
	 *30南瓜网发饰 id=33
	 *58
	 */
	switch(index) {
	case 1:
		if (p->followed == NULL) {
			ERROR_RETURN(("not followed pet %u", p->id), -1);
		}
		if (check_three_basic_skill(p->followed)) {
			return send_to_self_error(p, p->waitcmd, -ERR_not_study_basic_skills, 1);
		}
		if (!PET_IS_SUPER_LAHM(p->followed)) {
			return send_to_self_error(p, p->waitcmd, -ERR_no_super_lahm, 1);
		}
		if (p->only_one_flag[0] & 0x01) {
			return send_to_self_error(p, p->waitcmd, -ERR_have_got_five_stage_libo, 1);
		}
		p->only_one_flag[0] |= 0x01;
		send_request_to_db(SVR_PROTO_SET_ONLY_ONE_FLAG, NULL, sizeof(index), &index, p->id);
		return do_exchange_item(p, 1614, 0);
		break;

	case 3 ... 4:
		if (p->only_one_flag[0] & 0x0C) {
			return send_to_self_error(p, p->waitcmd, -ERR_you_have_chose_wo_wei_hui, 1);
		}
		p->only_one_flag[0] |= (0x01 << (index - 1));
		send_request_to_db(SVR_PROTO_SET_ONLY_ONE_FLAG, NULL, sizeof(index), &index, p->id);
		response_proto_head(p, p->waitcmd, 0);
		break;

	case 5:
		if (p->only_one_flag[0] & 0x10) {
			return send_to_self_error(p, p->waitcmd, -ERR_have_fini_tiyan_dinig_room, 1);
		}

		uint32_t ding_room[] = {0, p->id};
		msglog(statistic_logfile, 0x02020005, get_now_tv()->tv_sec, ding_room, sizeof(ding_room));

		p->only_one_flag[0] |= (0x01 << (index -1));
		send_request_to_db(SVR_PROTO_SET_ONLY_ONE_FLAG, NULL, sizeof(index), &index, p->id);
		uint32_t db_buf[] = {0, 3, 0, 0, 0, 190187, 10, 99999, 0, 190196, 10, 99999, 0, 190024, 10, 99999};
		send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf), db_buf, p->id);
		response_proto_head(p, p->waitcmd, 0);
		break;
	case 7:
		if(p->dining_flag == 0 ) {
			return send_to_self_error(p,p->waitcmd, -ERR_have_no_shop, 1);
		}
		if (p->only_one_flag[0] & (1 << 6)) {
			return send_to_self_error(p, p->waitcmd, -ERR_have_get_shop_packge, 1);
		}
		p->only_one_flag[0] |= (0x01 << 6);
		response_proto_head(p,p->waitcmd,0);
		send_request_to_db(SVR_PROTO_SET_ONLY_ONE_FLAG, NULL, sizeof(index), &index, p->id);
		return db_change_shop_money(NULL, 5000, p->id);

	default:
		ERROR_RETURN(("INDEX IS WRONG %u %u", p->id, index), -1);
	}
	return 0;
}

/* @brief 为屋委会的NPC加分数
 */
int add_score_for_wo_wei_hui_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t j = 0, score;
	UNPKG_UINT32(body, score, j);

	if (!(p->only_one_flag[0] & 0X0C)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_chose_wo_wei_hui, 1);
	}
	if (score > 100000) {
		ERROR_RETURN(("invalid game score %u %u", p->id, score), -1);
	}

	*(uint32_t *)p->session = score;
	switch (p->only_one_flag[0] & 0X0C) {
	case 4:/*依莲*/
		*(uint32_t *)(p->session + 4) = 131;
		break;

	case 8:/*花嫂*/
		*(uint32_t *)(p->session + 4) = 132;
		break;

	default:
		return send_to_self_error(p, p->waitcmd, -ERR_not_chose_wo_wei_hui, 1);
	}
	return send_request_to_db(SVR_PROTO_UPDATE_WO_WEI_HUI_SCORE, p, sizeof(uint32_t), &score, p->id);
}


/* @brief 判断是否送用户物品,并且为NPC加分数
 */
int add_score_for_wo_wei_hui_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t score = *(uint32_t *)buf;
	uint32_t flag = *(uint32_t *)(buf + 4);
	uint32_t add_score = *(uint32_t *)(buf + 8);
	uint32_t itemid = 0;
	if (flag == 1) {
		if (p->only_one_flag[0] & 0X04) {
			itemid = 13370;
		} else {
			itemid = 13369;
		}
		db_single_item_op(NULL, p->id, itemid, 1, 1);
	}

	uint32_t type = *(uint32_t *)(p->session + 4);
	db_set_cnt(p, type, (add_score / 10));

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, score, j);
	PKG_UINT32(msg, ((p->only_one_flag[0] & 0x0C) >> 2), j);
	PKG_UINT32(msg, itemid, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

/* @breif 得到某个用户为屋委会NPC增加的分数
 */
int get_score_for_wo_wei_hui_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	if (!(p->only_one_flag[0] & 0X0C)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_chose_wo_wei_hui, 1);
	}
	return send_request_to_db(SVR_PROTO_GET_WO_WEI_HUI_SCORE, p, 0, NULL, p->id);
}
/* @brief 得到为屋委会NPC增加的分数，并且返回是为哪个NPC增加的
 */
int get_score_for_wo_wei_hui_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t score = *(uint32_t *)buf;
	uint32_t flag = ((p->only_one_flag[0] & 0x0C) >> 2);
	response_proto_uint32_uint32(p, p->waitcmd, score, flag, 0);
	return 0;
}

/* @brief 喂兔子玉米得到樱桃树苗
 */
int feed_rabit_get_seed_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t j = 0, rabit_num;
	UNPKG_UINT32(body, rabit_num, j);
	if ((rabit_num > 3 || rabit_num < 1) || (!p->followed) || (!PET_IS_SUPER_LAHM(p->followed))) {
		ERROR_RETURN(("RABIT NUM WRONG %u %u", p->id, rabit_num), -1);
	}
	*(uint32_t *)p->session = rabit_num;
	return db_set_sth_done(p, 504, 2, p->id);
}

/* @brief 喂兔子玉米得到樱桃树苗
 */
int feed_rabit_get_seed_callback(sprite_t *p)
{
	uint32_t rabit_num = *(uint32_t *)p->session;
	uint32_t count = 0;
	if (rand() % 10 < 5) {
		count = 1;
	} else if (rand() % 10 < 7) {
		count = 2;
	} else {
		count = 3;
	}
	*(uint32_t *)p->session = count;

	uint32_t db_buf[11] = {1, 1, 0, 0, 0, 190419, rabit_num, 0, 1230044, count, 99};
	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, sizeof(db_buf), db_buf, p->id);
}

/* @brief  返回记录用户是否做过某件事的字段
 */
int get_support_npc_wo_wei_hui_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	int bytes = sizeof(protocol_t);
	int i;
	for( i = 0; i < sizeof(p->only_one_flag)/sizeof(int32_t); ++i )
	{
		PKG_UINT32(msg,p->only_one_flag[i],bytes);
	}
	init_proto_head(msg, p->waitcmd, bytes);
	return send_to_self(p, msg, bytes, 1);
}

/* @brief 得到登入时候的session
 */
int get_login_session_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	int j = sizeof(protocol_t);
	int ret = encrypt_login_session(msg + j, 16, p->id, p->fdsess->remote_ip);
	j += ret;
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

/* @brief 通过刮 刮刮卡得到物品, 先校验是否有刮刮卡，如有数目减一
 */
int get_money_by_gua_gua_card_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	db_single_item_op(p, p->id, 190669, 1, 0);
	return 0;
}

/* @brief 如果有刮刮卡，随机一定数目的摩尔豆
 */
int get_money_by_gua_gua_card_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t money[] = {0, 100, 500, 888, 2000};
	int rand_index = rand() % 5;
	if (money[rand_index] !=  0) {
		db_change_xiaome_not_limit(NULL, money[rand_index], p->id);
	}
	response_proto_uint32(p, p->waitcmd, money[rand_index], 0);
	return 0;
}

int get_type_data_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t type = 1;
	CHECK_BODY_LEN(bodylen,0);

	return send_request_to_db(SVR_PROTO_GET_TYPE_DATA,p,4,&type, p->id);
}

int get_type_data_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	int j = 0;
	uint32_t type;
	uint32_t uid;
	uint32_t gtime;

	CHECK_BODY_LEN_GE(len,12);

	UNPKG_H_UINT32(buf,type, j);
	UNPKG_H_UINT32(buf,uid, j);
	UNPKG_H_UINT32(buf,gtime,j);

	if(p->waitcmd == PROTO_SET_TYPE_DATA) {
		uint32_t gtime_up;
		gtime_up = *(uint32_t*)(p->session + 12);

		if(gtime != 0 && uid != 0 && gtime_up >= gtime) {//NOT NEED UPDATE
			response_proto_head(p, p->waitcmd, 0);
			return 0;
		}

		return send_request_to_db(SVR_PROTO_SET_TYPE_DATA, p, 108,p->session, p->id);
	}

	int bytes = sizeof(protocol_t);
	PKG_UINT32(msg,uid, bytes);
	PKG_UINT32(msg,gtime,bytes);
	init_proto_head(msg,p->waitcmd,bytes);
	return send_to_self(p, msg, bytes, 1);
}

int set_type_data_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen,4);

	int j = 0;
	uint32_t gtime;
	uint32_t type = 1;
	CHECK_BODY_LEN(bodylen,4);
	UNPKG_UINT32(body,gtime,j);

	p->sess_len = 0;
	PKG_H_UINT32(p->session,type,p->sess_len);
	PKG_H_UINT32(p->session,8,   p->sess_len);
	PKG_H_UINT32(p->session,p->id,p->sess_len);
	PKG_H_UINT32(p->session,gtime,p->sess_len);

	return send_request_to_db(SVR_PROTO_GET_TYPE_DATA, p, 4,p->session, p->id);
}

int set_type_data_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}


int get_session_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t game_id = 0;
	CHECK_BODY_LEN(bodylen,sizeof(game_id));
	unpack(body, sizeof(game_id), "L", &game_id);

	*(uint32_t *)p->session = game_id;

#ifdef TW_VER
	struct req_0B01 {
		uint32_t gameid;
		uint32_t ipaddr;
	} req;

	req.gameid = game_id;
	req.ipaddr = p->fdsess->remote_ip;
	return send_request_to_db(SVR_PROTO_GET_SESSION,p,sizeof(req),&req,p->id);
#else

	typedef struct _add_session {
		uint16_t channel_id;
		char verify_info[32];
		uint32_t gameid;
		uint32_t ipaddr;
	} __attribute__ ((packed)) add_session_t;
	add_session_t session_req;

	unsigned char src[100];
	unsigned char md_out[16], md[33];
	uint16_t channel_id = config_get_intval("channel_id", 0);
	uint32_t security_code = config_get_intval("security_code", 0);
	DEBUG_LOG("userid %d channel_id %d security_code %d", p->id, channel_id, security_code);

	session_req.channel_id = channel_id;
	session_req.gameid = game_id;
	session_req.ipaddr = p->fdsess->remote_ip;

	int len = snprintf((char*)src, sizeof src, "channelId=%d&securityCode=%u&data=", channel_id, security_code);
	*(uint32_t*)(src + len) = game_id;
	*(uint32_t*)(src + len + 4) = p->fdsess->remote_ip;
	MD5(src, len + 8, md_out);
	int i;
	for (i = 0; i != 16; ++i) {
		sprintf((char*)md + i * 2, "%.2x", md_out[i]);
	}

	memcpy(session_req.verify_info, md, 32);
	return send_request_to_db(SVR_PROTO_GET_SESSION, p, sizeof(session_req), &session_req, p->id);

#endif

}



int get_session_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN(len,16);
	response_proto_str(p,p->waitcmd,16,buf,0);

	uint32_t game_id = *(uint32_t *)p->session;
	if (game_id == 7)
	{
		DEBUG_LOG("1111 add hero list userid %d", p->id);
		map_t* tile = NULL;
		tile = p->tiles;
		if (tile)
		{
			if (((&p->hero_list)->next == &p->hero_list)&&((&p->hero_list)->prev == &p->hero_list))
			{
				DEBUG_LOG("add hero list userid %d", p->id);
				list_add_tail (&p->hero_list, &tile->hero_list_head);
			}
		}
		notify_enter_leave_hero_game(p, 1);
	}

	return 0;
}

int leave_hero_game_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t flag = 0;
	CHECK_BODY_LEN(bodylen,sizeof(flag));
	unpack(body, sizeof(flag), "L", &flag);
	if (flag != 0 || !(p->tiles))
	{
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	list_del_init(&p->hero_list);

	DEBUG_LOG("del hero list userid %d", p->id);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, flag, i);
	init_proto_head(msg, PROTO_LEAVE_HERO_GAME, i);
	send_to_map(p, msg, i, 1);
	return 0;
}

int notify_enter_leave_hero_game(sprite_t* p, uint32_t flag)
{
	uint8_t n_msg[128] = {0};
	int l = sizeof(protocol_t);
	PKG_UINT32(n_msg, p->id, l);
	PKG_UINT32(n_msg, flag, l);

	init_proto_head(n_msg, PROTO_LEAVE_HERO_GAME, l);
	send_to_map3(32, n_msg, l);
	return 0;

}

int get_in_hero_userid_list_cmd (sprite_t *p, const uint8_t *body, int len)
{
	if( !p->tiles )
	{
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	list_head_t* cur;
	int count = 0;
	int i, j;
	i = sizeof(protocol_t);
	j = i;
	PKG_UINT32(msg, count, i);

	list_for_each_prev(cur, &(p->tiles->hero_list_head))
	{
		sprite_t* sp = NULL;
		sp = list_entry(cur, sprite_t, hero_list);
		if (sp)
		{
			PKG_UINT32(msg, sp->id, i);
			count++;
		}
		if (count == 400)
		{
			break ;
		}
	}

	PKG_UINT32(msg, count, j);

	DEBUG_LOG("GET IN HERO USERID LIST\t[pid=%u]", p->id);
	init_proto_head(msg, p->waitcmd, i);
	send_to_self(p, msg, i, 1);

	return 0;
}


/* @brief 修复卡牌的BUG,如果经验值大于7000，没有红色训练服则加，如有有长矛，没红色训练服则加
 */
int add_red_clothe_accord_exp_lance_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return send_request_to_db(SVR_PROTO_CARD_GET_INFO, p, 0, NULL, p->id);
}

/* @brief 处理DB返回骑士卡牌的信息
 */
int get_card_game_info_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	#define WIN_BONUS	50
	#define LOST_BONUS	10
	#define RED_CLOTHE_SCORE 7000

	uint32_t flag = 0;
	uint32_t win_count = 0;
	uint32_t lost_count = 0;
	unpkg_host_uint32_uint32_uint32((uint8_t *)buf, &flag, &win_count, &lost_count);
	int scores = WIN_BONUS * win_count + LOST_BONUS * lost_count;
	if (scores >= RED_CLOTHE_SCORE) {
		db_single_item_op(NULL, p->id, 12364, 1, 1);
	}

	uint32_t db_buf[] = {1, 12367};
	return send_request_to_db(SVR_QUERY_ITEM_COUNT, p, sizeof(db_buf), db_buf, p->id);
}

/* @brief 对用户是否有骑士长矛进行处理
 */
int get_lance_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	uint32_t count = 0;
	uint32_t item_id = 0;
	uint32_t item_count = 0;
	unpkg_host_uint32_uint32_uint32((uint8_t *)buf, &count, &item_id, &item_count);
	if (item_id == 12367 && item_count) {
		db_single_item_op(NULL, p->id, 12364, 1, 1);
	}
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/* @brief 获取流浪商人卖出的物品
 */
int get_businessman_goods_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t npc_id = 0;
	CHECK_BODY_LEN(bodylen,sizeof(npc_id));
	unpack(body, sizeof(npc_id), "L", &npc_id);
	return send_request_to_db(SVR_PROTO_GET_BUSINESSMAN_GOODS, p, sizeof(npc_id), &npc_id, p->id);
}

/* @brief 流浪商人能够售卖的物品返回包
 */
int get_businessman_goods_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	struct businessman_goods {
		uint32_t item_id;
		uint32_t count;
	}__attribute__((packed)) *goods;

	uint32_t count = 0;
	unpkg_host_uint32((uint8_t *)buf, &count);
	goods = (struct businessman_goods *)(buf + sizeof(count));

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, count, j);
	int i = 0;
	for (i = 0; i < count; i++) {
		PKG_UINT32(msg, goods->item_id, j);
		PKG_UINT32(msg, goods->count, j);
		goods++;
	}
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

/* @brief 购买流动商人的物品
 */
int buy_businessman_goods_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	struct buy_businessman_goods {
		uint32_t npc_id;
		uint32_t item_id;
		uint32_t count;
	}__attribute__((packed)) buy_goods;

	unpack(buf, sizeof(buy_goods), "LLL", &buy_goods.npc_id, &buy_goods.item_id, &buy_goods.count);
	if (get_item_prop(buy_goods.item_id) == NULL) {
		ERROR_RETURN(("WRONG ITME ID %u %u", p->id, buy_goods.item_id), -1);
	}
	memcpy(p->session, &buy_goods, sizeof(buy_goods));
	return send_request_to_db(SVR_PROTO_BUY_BUSINESSMAN_GOODS, p, sizeof(buy_goods), &buy_goods, p->id);
}

/* @brief 购买流动商人物品的回调函数,修改注意偏移量
 */
int buy_businessman_goods_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	uint32_t remain_count = 0;
	unpkg_host_uint32((uint8_t *)buf, &remain_count);
	uint32_t item_id = *(uint32_t *)(p->session + 4);
	uint32_t count = *(uint32_t *)(p->session + 8);
	*(uint32_t *)(p->session + 12) = remain_count;

	DEBUG_LOG("BUY ITEM\t[uid=%u itm=%u cnt=%u]", p->id, item_id, count);
	item_t *itm = get_item_prop(item_id);
	return db_buy_items(p, itm, count, find_kind_of_item(item_id), 0, 1);
}

/* @brief 购买流动商人物品的
 */
int buy_businessman_add_bag_callback(sprite_t *p)
{
	uint32_t item_id = *(uint32_t *)(p->session + 4);
	uint32_t count = *(uint32_t *)(p->session + 8);
	uint32_t remain_count = *(uint32_t *)(p->session + 12);
	response_proto_uint32_uint32_uint32(p, p->waitcmd, item_id, remain_count, count, 0);
	return 0;
}

/* @brief 任务中获取龙蛋，只能获取一次
 */
int get_dragon_egg_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
    //?μ?	return send_to_self_error(p, p->waitcmd, -ERR_have_get_dragon_egg, 1);

	if (!set_only_one_bit(p, 10)) {
		return send_to_self_error(p, p->waitcmd, -ERR_have_get_dragon_egg, 1);
	}
	item_t *itm = get_item_prop(190681);
	db_buy_items(p, itm, 1, find_kind_of_item(190681), 1, 0);
	response_proto_uint32(p, p->waitcmd, 190681, 0);
	return 0;
}

/* @brief 用一个物品交换另一个物品
 */
int exchange_one_thing_to_another_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	struct exchange_one_to_another {
		uint32_t src_id;
		uint32_t desc_id;
		uint32_t div;
	};
	struct exchange_one_to_another info[] = {
											{190670, 17012, 2},
										  };
	uint32_t item_id = 0;
	uint32_t count = 0;
	unpack(buf, 8, "LL", &item_id, &count);
	int i = 0;
	for (i = 0; i < (sizeof(info) / sizeof(info[0])); i++) {
		if (info[i].src_id == item_id) {
			break;
		}
	}
	if (i == (sizeof(info) / sizeof(info[0]))) {
		ERROR_RETURN(("ITEM IS WRONG %u %u", p->id, item_id), -1);
	}
	*(uint32_t *)p->session = info[i].desc_id;
	*(uint32_t *)(p->session + 4) = count / info[i].div;
	uint32_t db_buf[] = {1, 1, 0, 0, 0, item_id, count, 0, info[i].desc_id, count / info[i].div, 9999};
	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, sizeof(db_buf), db_buf, p->id);
}

/* @brief用一个物品交换另一个物品的回调函数
 */
int exchange_one_thing_to_another_callback(sprite_t *p)
{
	uint32_t item_id = *(uint32_t *)p->session;
	uint32_t count = *(uint32_t *)(p->session + 4);
	response_proto_uint32_uint32(p, p->waitcmd, item_id, count, 0);
	return 0;
}

/*@brief 送开学大礼包
 */
int get_open_school_gift_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	if(!set_only_one_bit(p, 16)) {
		return send_to_self_error(p, p->waitcmd, -ERR_have_get_open_school_gift, 1);
	}
	uint32_t db_buf[] = {0, 1, 0, 0, 0, 190688, 10, 99999};
	send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf), db_buf, p->id);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int get_profession_exp_level_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	int j = sizeof(protocol_t);
	uint32_t count = 3;
	PKG_UINT32(msg, count, j);
	PKG_UINT32(msg, 7, j);
	PKG_UINT32(msg, p->profession[7], j);
	PKG_UINT32(msg, p->architect_exp, j);

	PKG_UINT32(msg, 9, j);
	PKG_UINT32(msg, p->profession[9], j);
	PKG_UINT32(msg, p->teach_exp, j);

	PKG_UINT32(msg, 10, j);
	PKG_UINT32(msg, p->profession[10], j);
	PKG_UINT32(msg, p->dietitian_exp, j);

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int look_sth_cnt_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	int j = 0;
	uint32_t type;

	CHECK_BODY_LEN(bodylen, 4);

	UNPKG_UINT32(buf,type,j);
	if(type < 20000 || type > 20001) {
		return -1;
	}
	*(uint32_t*)p->session = type;
	return db_set_sth_done(p,type,1,p->id);
}

int look_sth_cnt_callback_0(sprite_t *p,uint32_t type, uint32_t cnt)
{
	if(type < 20000 || type > 20001) {
		return -1;
	}
	int j = 0;
	int i = 0;
	int count = 1;
	uint8_t buf[pagesize];
	uint32_t itmcnt[2]={5,1};
	uint32_t itemid[2]={190028,1270002};

	if(cnt < 7) {
		db_set_cnt(p,type,1);
		if(cnt == 6) count = 2;

		if(type == 20001) {
			itemid[1] = 1230022;
		}

		PKG_H_UINT32(buf, 0, j);
		PKG_H_UINT32(buf, count, j);
		PKG_H_UINT32(buf, 0, j);
		PKG_H_UINT32(buf, 0, j);

		p->sess_len = 0;
		PKG_H_UINT32(p->session,count,p->sess_len);
		for(i=0;i<count;i++) {
			if (pkg_item_kind(p, buf, itemid[i], &j) == -1) {
				return -1;
			}
			item_t *itm = get_item_prop(itemid[i]);
			if(itm == NULL) {
				return -1;
			}
			PKG_H_UINT32(buf, itemid[i], j);
			PKG_H_UINT32(buf, itmcnt[i], j);
			PKG_H_UINT32(buf, itm->max, j);

			PKG_H_UINT32(p->session,itemid[i],p->sess_len);
			PKG_H_UINT32(p->session,itmcnt[i],p->sess_len);
		}
		return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buf, p->id);
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session,0,p->sess_len);
	return look_sth_cnt_callback_1(p);
}

int look_sth_cnt_callback_1(sprite_t *p)
{
	int j = 0;
	int i = 0;
	uint32_t count;
	uint32_t itemid;
	uint32_t itmcnt;

	int bytes = sizeof(protocol_t);
	UNPKG_H_UINT32(p->session,count,j);
	PKG_UINT32(msg,count,bytes);
	for(i=0; i<count;i++) {
		UNPKG_H_UINT32(p->session,itemid,j);
		UNPKG_H_UINT32(p->session,itmcnt,j);
		PKG_UINT32(msg,itemid,bytes);
		PKG_UINT32(msg,itmcnt,bytes);
	}
	init_proto_head(msg,p->waitcmd,bytes);
	return send_to_self(p, msg,bytes,1);
}

/*
 * @brief 得到章鱼扑克游戏每局的胜负
 */
int get_octopus_car_win_or_lose_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	uint32_t end_flag = 0; /* 1表示结束游戏 */
	unpack(buf, sizeof(end_flag), "L", &end_flag);
	/* 每个关卡累积的爆豆数目 */
	int32_t gate_num[] = {0, 1, 3, 6, 12, 24};

	if (!end_flag) {
		int32_t rand_num = rand() % 100;
		switch(p->game_gate) {
		case 0:
			if (rand_num < 70) {
				p->game_gate++;
				response_proto_uint32_uint32_uint32(p, p->waitcmd,  p->game_gate, 1, gate_num[p->game_gate], 0);
				return 0;
			}
			break;

		case 1 ... 4:
			if (rand_num < 50) {
				p->game_gate++;
				response_proto_uint32_uint32_uint32(p, p->waitcmd,  p->game_gate, 1, gate_num[p->game_gate], 0);
				return 0;
			}
			break;

		default:
			ERROR_LOG("THE GAME GATE IS WRONG [USER ID %u] [GAME GAME %u]", p->id, p->game_gate);
			response_proto_uint32_uint32_uint32(p, p->waitcmd,  p->game_gate, 0, 0, 0);
			p->game_gate = 0;
			return 0;
		}
	}

	/* 如果失败，则获取的爆豆数为零, 扣除玩游戏需要的爆豆 */
	if (!end_flag) {
		response_proto_uint32_uint32_uint32(p, p->waitcmd,  p->game_gate, 0, 0, 0);
		uint32_t db_buf[] = {1, 0, 0, 0, 0, 1230068, 2};
		send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf), db_buf, p->id);
		p->game_gate = 0;
		return 0;
	}

	/* 游戏退出处理 */
	if (p->game_gate && (gate_num[p->game_gate] > 2)) {
		uint32_t db_buf[] = {0, 1, 0, 0, 0, 1230068, gate_num[p->game_gate] - 2, 99999};
		send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf), db_buf, p->id);
		response_proto_uint32_uint32_uint32(p, p->waitcmd,  p->game_gate, 0, gate_num[p->game_gate], 0);
		p->game_gate = 0;
	} else {
		uint32_t db_buf[] = {1, 0, 0, 0, 0, 1230068, 2 - gate_num[p->game_gate]};
		send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf), db_buf, p->id);
		response_proto_uint32_uint32_uint32(p, p->waitcmd,  p->game_gate, 0, gate_num[p->game_gate], 0);
		p->game_gate = 0;
	}
	return 0;
}

int add_client_msg_log_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t msgid = 0;
	CHECK_BODY_LEN(bodylen,sizeof(msgid)+4);
	unpack(body, sizeof(msgid), "L", &msgid);

	uint32_t msgbuff[2]= {p->id, 1};
	msglog(statistic_logfile, msgid, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

//	DEBUG_LOG("userid:%u msgid:%u",p->id, msgid);
	response_proto_head(p, p->waitcmd, 0);
	return 0;

}

//肥肥达人秀投票领奖
int get_feifei_daren_prize_cmd(sprite_t* p, const uint8_t* body, int len)
{
#ifndef TW_VER
	if (get_today() < 20111020 || get_today() > 20111104)
#else
	if (get_today() < 20111101 || get_today() > 20111111)
#endif
	{
		return 0;
	}
	CHECK_BODY_LEN(len, 0);

	return send_request_to_db(SVR_PROTO_CP_GET_DAREN_VOTE, p, 0, NULL, p->id);
}

int get_feifei_daren_prize_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	enum DAREN_PRIZE
	{
		DP_OK		= 0,
		DP_NOT_VOTE,
		DP_HAS_GET,
		DP_FAILED,
	};
	uint32_t count = *(uint32_t*)buf;
	uint32_t ret;
	//need modify
	uint32_t first_pig_sign = 4;
	if (count == 0) {
		ret = DP_NOT_VOTE;
	}
	else if (count == 32) {
		ret = DP_HAS_GET;
	}
	else
	{
		if( count & first_pig_sign )
		{
			ret = DP_OK;
		}
		else
		{
			ret = DP_FAILED;
		}
	}
	int l = sizeof(protocol_t);
	PKG_UINT32( msg, ret, l );
	init_proto_head( msg, p->waitcmd, l );
	send_to_self( p, msg, l, 1 );
	uint32_t prize_pack_id = 1613133;
	uint32_t prize_pig_id = 1593008;
	if( ret == DP_OK )
	{
		uint8_t buff[128];
		int dblen = 0;
		PKG_H_UINT32( buff, 0, dblen );
		PKG_H_UINT32( buff, 2, dblen );
		PKG_H_UINT32( buff, 0, dblen );
		PKG_H_UINT32( buff, 0, dblen );
		pkg_item_kind( p, buff, prize_pack_id, &dblen );
		PKG_H_UINT32( buff, prize_pack_id, dblen );
		PKG_H_UINT32( buff, 1, dblen );
		PKG_H_UINT32( buff, 99999, dblen );

		pkg_item_kind( p, buff, prize_pig_id, &dblen );
		PKG_H_UINT32( buff, prize_pig_id, dblen );
		PKG_H_UINT32( buff, 1, dblen );
		PKG_H_UINT32( buff, 99999, dblen );
		send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, dblen, buff, p->id );
		uint32_t vote_id = 32;
		send_request_to_db( SVR_PROTO_CP_DAREN_VOTE, NULL, 4, &vote_id, p->id );
	}
	return 0;
}

int set_sth_follow( sprite_t* p, uint32_t ani_idx, uint32_t is_follow, uint32_t ani_type )
{
	uint32_t dbbuf[2];
	int db_proto;
	if ( 1 == is_follow && p->animal_nbr )
	{
		dbbuf[0] = p->animal_nbr;
		dbbuf[1] = 0;
		//cute pig
		if( p->animal_follow_info.tag_animal.itemid >= 1593000 )
		{
			db_proto = SVR_PROTO_CP_FOLLOW;
		}
		//angel
		else if( p->animal_follow_info.tag_animal.itemid >= 1353401 )
		{
			db_proto = SVR_PROTO_MAKE_ANGEL_FOLLOWED;
		}
		//animal
		else if( p->animal_follow_info.tag_animal.itemid > 0 )
		{
			db_proto = SVR_PROTO_ANIMAL_OUTGO;
		}

		send_request_to_db( db_proto, NULL, sizeof(dbbuf), dbbuf, p->id );
	}

	switch( ani_type )
	{
	case FAT_ANI:
		db_proto = SVR_PROTO_ANIMAL_OUTGO;
		break;
	case FAT_ANGEL:
		db_proto = SVR_PROTO_MAKE_ANGEL_FOLLOWED;
		break;
	case FAT_PIG:
		db_proto = SVR_PROTO_CP_FOLLOW;
		break;
	default:
		ERROR_RETURN( ( "Unknow follow animal type:[%u]", ani_type ), -1 );
	}
	dbbuf[0] = ani_idx;
	dbbuf[1] = is_follow;
	return send_request_to_db( db_proto, p, sizeof(dbbuf), dbbuf, p->id );
}

int get_day_type_count_cmd(sprite_t * p,const uint8_t * body,int len)
{
    uint32_t type = 0;
    int i = 0;
    uint32_t cnt = 0;
    CHECK_BODY_LEN_GE(len, 4);
	UNPKG_UINT32(body, cnt, i);
	CHECK_BODY_LEN(len, 4 + cnt * 4);
	if (cnt > 20){
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	uint8_t dbbuf[2048] = {};
	int l = 0;
	PKG_H_UINT32(dbbuf,cnt,l);
	int j = 0;
	for (j = 0; j < cnt; j++)
	{
	    UNPKG_UINT32(body, type, i);
	    PKG_H_UINT32(dbbuf, type, l);
	}

    return send_request_to_db(SVR_PROTO_SYSARG_GET_DAYTYPE_COUNT, p, l, dbbuf, p->id);
}

int get_day_type_count_callback(sprite_t* p, uint32_t id, char* buf, int len)
{	
	CHECK_BODY_LEN_GE(len, 4);
	CHECK_VALID_ID(p->id);
	uint32_t count = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN(len, 4 + count * 8);
	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, count, i);
	int loop = 0;
	for(loop = 0; loop < count; loop++){
		uint32_t type = 0;
		uint32_t cnt = 0;
		UNPKG_H_UINT32 (buf, type, j);
		PKG_UINT32 (msg, type, i);

		UNPKG_H_UINT32 (buf, cnt, j);
		PKG_UINT32 (msg, cnt, i);
	}
	
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}



