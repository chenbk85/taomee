#include "communicator.h"
#include "dbproxy.h"

#include "central_online.h"
#include "exclu_things.h"
#include "logic.h"
#include "message.h"
#include "proto.h"
#include "tasks.h"
#include "pet_item.h"
#include "spring.h"
#include "small_require.h"
#include "cache_svr.h"
#include "pet.h"
#include "homeinfo.h"
#include "lamu_skill.h"
#include "school_bar_cmd.h"


// typedefs
// For do_trust_pet
typedef struct TrustPetSess {
	pet_t* pet;
	uint32_t duration;
} trust_pet_sess_t;
// For do_pick_task
typedef struct PickPetTaskSess {
	pet_t*		pet;
	task_t*		task;
	uint16_t	days;
} pick_pet_task_sess_t;

// For pick magic task
typedef struct PickPetMagicTaskSess {
	pet_t*		pet;
	task_t*		task;
	uint16_t	stage;
} pick_pet_magic_task_sess_t;

// For db_pet_op
typedef struct PetBasicInfo {
	uint32_t id, flag, flag2, sicktime;
	uint8_t  hungry, thirsty, sanitary, spirit;
	uint32_t   trust_end_tm, start_tm;
	uint32_t taskid;
	uint32_t   stamp;
} __attribute__ ((packed)) pet_basic_info_t;

// DB-related funcs/macros
#define db_get_pet(p, petid, id) \
		send_request_to_db(SVR_PROTO_GET_PET, p, 4, &petid, id)
#define db_set_pets_pos(p, c, buf) \
		send_request_to_db(SVR_PROTO_SET_PETS_POS, p, c * 6 + 4, buf, p->id)
#define db_del_pet(p_, petid_, id_) \
		send_request_to_db(SVR_PROTO_DEL_PET, (p_), 4, &(petid_), (id_))

#define db_inc_trusted_pet_cnt() db_set_cnt(0, 0, 1)
#define db_dec_trusted_pet_cnt() db_set_cnt(0, 0, -1)
#define db_inc_ontask_pet_cnt() db_set_cnt(0, 1, 1)
#define db_dec_ontask_pet_cnt() db_set_cnt(0, 1, -1)
//
static inline uint32_t del_pet(uint32_t uid, uint32_t petid);
static inline int terminate_pet_task(sprite_t* p, userid_t uid, pet_t* pet, int fulltime);
//
static inline void notify_pet_disappear(sprite_t* p, const pet_t* pet);
//static inline void notify_pet_home(sprite_t* p, const pet_t* pet, uint32_t itmid, uint32_t type);
static int  notify_trusted_pets(sprite_t* p, pet_t* pets, int count);
static inline int  response_follow_pet(sprite_t *p, pet_t *pet);
static inline void response_proto_play_pet(sprite_t *p, uint32_t userid, pet_t *pet, int action);
//
static int do_feed_item(sprite_t *p, uint32_t uid,uint32_t petid, uint32_t itemid);
static int do_follow_pet(sprite_t *p, uint32_t petid, uint32_t status);
static int do_play_with_pet(sprite_t *p, uint32_t userid, uint32_t petid);
static int do_set_pet_nick(sprite_t *p, uint32_t petid, const char *nick);
static int do_set_pets_pos(sprite_t *p, int count, pet_pos_t *pos);
static inline int do_withdraw_pet(sprite_t* p, uint32_t petid);
int db_set_lahm_task_state(sprite_t *p, uint32_t id, uint32_t petid, uint32_t taskid, uint32_t state);
//----------------------------------------------------
/**
 * chk_has_super_lahm - chk if the given user @p has already had a super lahm
 * @p: pointer of type sprite_t
 *
 * chk if the given user @p has already had a super lahm,
 * returns 1 if has, 0 if has none, or -1 on error.
 */
int chk_has_super_lahm(sprite_t* p)
{
	CHECK_VALID_ID(p->id);

	map_t* map = get_map(p->id);
	if (map && map->pet_loaded) {
		int i, has = 0;
		for (i = 0; i != map->pet_cnt; ++i) {
			if ( PET_IS_SUPER_LAHM(&(map->pets[i])) ) {
				has = 1;
				break;
			}
		}
		return has;
	}

	ERROR_RETURN(("pet not loaded yet: uid=%u", p->id), -1);
}

/**
  * del_pet: del the given pet @petid of @uid
  * return: @petid if the given pet is deleted, otherwise 0
  */
static inline uint32_t
del_pet(uint32_t uid, uint32_t petid)
{
	db_del_pet(0, petid, uid);

	map_t* map = get_map(uid);
	if (map && map->pet_loaded) {
		int i = 0;
		for ( ; (i != map->pet_cnt) && (map->pets[i].id != petid); ++i ) ;

		if ( i != map->pet_cnt ) {
			if ( i != --(map->pet_cnt) ) {
				map->pets[i] = map->pets[map->pet_cnt];
			}
			return petid;
		}
	}
	return 0;
}

int add_pet_to_db(sprite_t *p, pet_t *pet)
{
	int len = 0;
	char buff[sizeof(pet_t)];

	PKG_H_UINT32(buff,pet->flag,len);
	PKG_H_UINT32(buff,pet->flag2,len);
	PKG_H_UINT32(buff,pet->birth,len);
	PKG_H_UINT32(buff,pet->logic_birthday,len);
	memcpy(buff + len,pet->nick,sizeof(pet->nick));
	len += sizeof(pet->nick);
	PKG_H_UINT32(buff,pet->color,len);
	PKG_H_UINT32(buff,pet->sicktime,len);
	PKG_H_UINT8(buff,pet->posX,len);
	PKG_H_UINT8(buff,pet->posY,len);
	PKG_H_UINT8(buff,pet->hungry,len);
	PKG_H_UINT8(buff,pet->thirsty,len);
	PKG_H_UINT8(buff,pet->sanitary,len);
 	PKG_H_UINT8(buff,pet->spirit,len);
	PKG_H_UINT32(buff,pet->trust_end_tm,len);
	PKG_H_UINT32(buff,pet->start_tm,len);
	PKG_H_UINT32(buff,pet->taskid,len);
	PKG_H_UINT32(buff,pet->skill,len);
	PKG_H_UINT32(buff,pet->stamp,len);
	PKG_H_UINT32(buff,pet->sick_type,len);

	return send_request_to_db(SVR_PROTO_ADD_PET, p, len, buff, p->id);
}

pet_t* get_pet(userid_t userid, uint32_t petid)
{
	int i;

	map_t* map = get_map(userid);
	if (!map || !map->pet_loaded || !map->pet_cnt) {
		//DEBUG_LOG("get_pet1\t[%u]", userid);
		return NULL;
	}

	for (i = 0; i < map->pet_cnt; i++) {
		if (map->pets[i].id == petid) {
			//DEBUG_LOG("get_pet2\t[%u %u %u %u]", userid, map->pets[i].id, petid, map->pet_cnt);
			return &map->pets[i];
		}
		//DEBUG_LOG("get_pet\t[%u %u %u %u]", userid, map->pets[i].id, petid, map->pet_cnt);
	}

	return NULL;
}

/**
 * get_pet_level - calc level of the given @pet
 * @pet: pointer of type const pet_t
 *
 * calc level of the given @pet and
 * returns 0xFF if @pet is a 'super lahm',
 * or corresponding level of @pet.
 */
uint8_t get_pet_level(const pet_t* pet)
{
	if (PET_IS_SUPER_LAHM(pet)) {
		return 101;
	}

	if(pet->skill_bits) return 5;

	time_t sec_now = get_now_tv()->tv_sec;
	if (sec_now < pet->logic_birthday) {
		ERROR_RETURN(("error time, sec_now=%ld, logic_birthday=%u", sec_now, pet->logic_birthday), 0);
	}

	int n = (sec_now - pet->logic_birthday) / (3600 * 24);
	if (n >= 90) {
		return 4;
	}
	if (n >= 30) {
		return 3;
	}
	if (n >= 1) {
		return 2;
	}
	return 1;
}
//----------------------------------------------------

//----------------------------------------------------
int calc_pet_attr_full(uint32_t id, pet_t* pet, int hungry, int thirsty, int sanitary, int spirit, int nosick)
{
#define ATTR_ADD(a, v) \
		a = a + v; \
		if (v > 0) a = ((a) > 100 ? 100 : (a)); \
		else a = ((a) > 100 ? 0 : (a));

	if (PET_TRUSTED(pet) || PET_ON_TASK(pet) || PET_GO_OUT(pet) || PET_VISIT(pet)) {
		sprite_t* p = get_sprite(id);
		if ( (get_now_tv()->tv_sec < pet->trust_end_tm) || !p ) {
			return 0;
		} else if (PET_TRUSTED(pet)) {
			// Send a Postcard to Inform the Player
#ifdef TW_VER
			send_postcard("彩虹姐姐", 0, id, 1000018, "親愛的小摩爾，已經過了託管時間咯！你的拉姆已經自己回家，它看起來很不高興哦，趕快回家看看吧！", 0);
#else
			send_postcard("彩虹姐姐", 0, id, 1000018, "亲爱的小摩尔，已经过了托管时间咯！你的拉姆已经自己回家，它看起来很不高兴哦，赶快回家看看吧！", 0);
#endif
			DEBUG_LOG("PET TRUST TIME EXPIRED\t[uid=%u pid=%u]", id, pet->id);
			//
			spirit = -50;
			RESET_PET_TRUSTED(pet);
			pet->stamp        = pet->trust_end_tm;
			pet->trust_end_tm = 0;
			//db_pet_op(0, pet, id);
			db_set_flag_endtime(0, pet->flag, pet->trust_end_tm, pet->id, p->id);
			db_dec_trusted_pet_cnt();
			//
			if (p->waitcmd != PROTO_LOGIN) {
				send_attr_update_noti(0, p, id, 3);
			}
		} else if (PET_ON_TASK(pet)) {
			DEBUG_LOG("PET ON-TASK TIME EXPIRED\t[uid=%u pid=%u tid=%u]", id, pet->id, pet->taskid);
			terminate_pet_task(0, id, pet, 1);
			do_terminate_pet_task(0, id, pet->id, 1);
			//
			if (p->waitcmd != PROTO_LOGIN) {
				send_attr_update_noti(0, p, id, 3);
			}
		} else if (PET_GO_OUT(pet)) {
			static const uint32_t  itms[]    = { 180001, 180002, 180003 };
			static const uint32_t  strong_itms[]    = {190022, 190120, 190119};
#ifdef TW_VER
			static const char*     itmname[] = {"巧克力豆", "麥香牛奶", "泡泡浴" };
			static const char*     strong_itmname[] = {"一個南瓜", "一些紅棗", "一些糯米" };
#else
			static const char*     itmname[] = { "巧克力豆", "麦香牛奶", "泡泡浴" };
			static const char*     strong_itmname[] = { "一个南瓜", "一些红枣", "一些糯米" };
#endif

			spirit = 20;
			RESET_PET_GO_OUT(pet);
			pet->trust_end_tm  = 0;
			respond_pet_to_map(pet, PET_GO_HOME);
			rm_pet_from_map(p, pet->id, pet->suppl_info.mapid);
			pet->suppl_info.mapid = 0;
			//db_pet_op(0, pet, id);
			db_set_flag_endtime(0, pet->flag, pet->trust_end_tm, pet->id, p->id);
			//
			char txt[128];
			int out_reason = 0;
			int loop;
			for (loop = 0; loop < pet->suppl_info.pti.count && loop < PET_TASK_MAX_CNT; loop++) {
				if (pet->suppl_info.pti.taskinfo[loop].taskid == 102) {
					out_reason = 102;
					break;
				}
				if (pet->suppl_info.pti.taskinfo[loop].taskid == 104) {
					out_reason = 104;
					break;
				}
			}
			int idx = rand() % 3;
			if (out_reason == 102)
				do_buy_item(p, strong_itms[idx], 1, 1, 0);
			else if (pet->suppl_info.pti.count != 0)
				do_buy_item(p, itms[idx], 1, 1, 0);

			if (out_reason == 102) {
			#ifdef TW_VER
				snprintf(txt, sizeof txt, "　　bibobibo！快去百寶箱看看啊！我搬了%s回來", strong_itmname[idx]);
			#else
				snprintf(txt, sizeof txt, "　　bibobibo！快去百宝箱看看啊！我搬了%s回来", strong_itmname[idx]);
			#endif
				send_postcard(pet->nick, 0, p->id, 1000087, txt, 0);
			} else if (out_reason == 104) {
			#ifdef TW_VER
				snprintf(txt, sizeof txt, "　　我回來了，今天的探險十分的開心啊！看！我還意外的發現了1個%s", itmname[idx]);
			#else
				snprintf(txt, sizeof txt, "　　我回来了，今天的探险十分的开心啊！看！我还意外的发现了1个%s", itmname[idx]);
			#endif
				send_postcard(pet->nick, 0, p->id, 1000085, txt, 0);
			} else if (pet->suppl_info.pti.count != 0) {
			#ifdef TW_VER
				snprintf(txt, sizeof txt, "　　你的拉姆運氣真不錯，在外閒逛時意外的獲得1個%s", itmname[idx]);
			#else
				snprintf(txt, sizeof txt, "　　你的拉姆运气真不错，在外闲逛时意外的获得1个%s", itmname[idx]);
			#endif
				send_postcard(pet->nick, 0, id, 1000024, txt, 0);
			} else {
			#ifdef TW_VER
				snprintf(txt, sizeof txt, "　　拉姆學院上課好有趣啊！可以學到好厲害的本領呢！我也想去拉姆學院學習學習，可以嗎？");
			#else
				snprintf(txt, sizeof txt, "　　拉姆学院上课好有趣啊！可以学到好厉害的本领呢！我也想去拉姆学院学习学习，可以吗？");
			#endif
				send_postcard(pet->nick, 0, id, 1000081, txt, 0);
			}
			DEBUG_LOG("PET RETURNS HOME\t[uid=%u pid=%u itm=%u]", id, pet->id, itms[idx]);
		} else if (PET_VISIT(pet)) {
			spirit = -20;
			RESET_PET_VISIT(pet);
			pet->trust_end_tm  = 0;
			respond_pet_to_map(pet, PET_GO_HOME);
			rm_pet_from_map(p, pet->id, pet->suppl_info.mapid);
			pet->suppl_info.mapid = 0;
			//db_pet_op(0, pet, id);
			db_set_flag_endtime(0, pet->flag, pet->trust_end_tm, pet->id, p->id);
			DEBUG_LOG("PET RETURNS HOME\t[uid=%u pid=%u]", id, pet->id);
		}
	}

	//const float rate = 0.01388;
	const float rate = 0.00694;
	int diff = ((now.tv_sec - pet->stamp) / 60 * rate);

	if (!hungry && !thirsty && !sanitary && !spirit && diff <= 0) {
		return 0;
	}
	//DEBUG_LOG ("petid=%d, %d,%d,%d,%d,%d,%d", pet->id, pet->hungry, pet->thirsty,pet->sanitary,pet->spirit, pet->stamp, now.tv_sec);

	ATTR_ADD(pet->hungry, hungry - diff);
	ATTR_ADD(pet->thirsty, thirsty - diff);
	ATTR_ADD(pet->sanitary, sanitary - diff);
	ATTR_ADD(pet->spirit, spirit- diff);
	pet->stamp = now.tv_sec;
	//DEBUG_LOG ("petid=%d, %d,%d,%d,%d,%d,%d", pet->id, pet->hungry, pet->thirsty,pet->sanitary,pet->spirit, pet->stamp, now.tv_sec);
	db_update_pet_attr(0, pet->stamp, hungry - diff, thirsty - diff, sanitary - diff, spirit- diff, pet->id, id);

	// Super Lahm is Immortal
	if (PET_IS_SUPER_LAHM(pet) || nosick) {
		return 0;
	}

//	DEBUG_LOG ("petid=%d, %d,%d,%d,%d", pet->id, pet->hungry, pet->thirsty,pet->sanitary,pet->spirit);
	//to be sick
	if (!pet->sicktime) {
		if (pet->hungry < 15 || pet->thirsty < 15 || pet->sanitary < 15 || pet->spirit < 15) {
			if ((1 + (int) (100.0 * (rand() / (RAND_MAX + 1.0)))) < 10) {
				pet->sicktime = now.tv_sec;
				SET_PET_SICK(pet);
                pet->sick_type = 0;
                db_infect_pet(NULL, pet, id);
				//db_set_pet(0, pet, id);
				db_set_pet_arg(0, PET_FLAG, pet->flag, pet->id, id);
				db_set_pet_arg(0, PET_SICKTIME, pet->sicktime, pet->id, id);
				DEBUG_LOG("PET SICK\t[uid=%u pid=%u]", id, pet->id);
				//cs_set_jy_and_pet_state(0, 2, 1, id);
			}
		}
	} else if ((now.tv_sec - pet->sicktime) / 3600 > 168) {
		if ( PET_SICK(pet) ) { // to be dead
			SET_PET_DEAD(pet);
			pet->sicktime = now.tv_sec;
			pet->hungry   = 0;
			pet->sanitary = 0;
			pet->spirit   = 0;
			pet->thirsty  = 0;
			//db_set_pet(0, pet, id);
			db_set_pet_arg(0, PET_FLAG, pet->flag, pet->id, id);
			db_set_pet_arg(0, PET_SICKTIME, pet->sicktime, pet->id, id);
			db_update_pet_attr(0, get_now_tv()->tv_sec, -100, -100, -100, -100, pet->id, id);
			DEBUG_LOG("PET DEAD\t[uid=%u pid=%u]", id, pet->id);
			//cs_set_jy_and_pet_state(0, 2, 1, id);
		}
/*
		else if ( PET_DEAD(pet) ) { // to be disappear
			char txt[256];
#ifdef TW_VER
			int len = snprintf(txt, sizeof txt, "你的拉姆寶寶%s已經永遠離你而去，看來你不是一個合格的好主人哦！", pet->nick);
#else
			int len = snprintf(txt, sizeof txt, "你的拉姆宝宝%s已经永远离你而去，看来你不是一个合格的好主人哦！", pet->nick);
#endif
			len = text_notify_pkg(msg + 4, 0, 0, 0, 0, admin, 0, len, txt);

			sprite_t* recvr = get_sprite(id);
			if (recvr) {
				send_to_self(recvr, msg + 4, len, 0);
			} else {
				*(uint32_t*)msg = len + 4;
				db_send_mail(0, id, msg, len + 4);
			}
			// del pet
			if (del_pet(id, pet->id) == pet->id) {
				notify_pet_disappear(recvr, pet);
				if ( recvr && recvr->followed && (pet->id == recvr->followed->id) ) {
					recvr->followed = 0;
				}
			}
			DEBUG_LOG("PET DISAPPEAR\t[uid=%u pid=%u]", id, pet->id);
		} */
	}

	return 0;

#undef ATTR_ADD
}

void cancel_super_lahm(const sprite_t* p, pet_t* pet, int cnt)
{
	if (!ISVIP(p->flag)) {
		int i;
		for (i = 0; i != cnt; ++i) {
			DEBUG_LOG("SET PET %u SKILL FLAG TO 0, OLD FLAG %x", pet[i].id, pet[i].skill_bits);
			if (PET_IS_SUPER_LAHM(&pet[i])) {
				PET_RESET_SUPER_LAHM(&pet[i]);
				if (enter_five_stage(&pet[i])) {
					pet[i].skill_bits = 0;
					db_set_pet_hot_skill(NULL, pet[i].id, 1, 2, 3, p->id);
					db_set_pet_arg(NULL, PET_SKILL_FLAG, 0, pet[i].id, p->id);
#ifndef TW_VER
				send_postcard("系统邮件", 0, p->id, 1000119, "你的超级拉姆已经离开，无法继续使用水、火、木3系技能。如果想继续使用技能，带上拉姆去拉姆世界神殿里重新选择吧！", 0);
#else
				send_postcard("系統郵件", 0, p->id, 1000119, "你的超級拉姆已經離開，無法繼續使用水、火、木3系技能。如果想繼續使用技能，帶上拉姆去拉姆世界神殿裡重新選擇吧！", 0);
#endif
				}
				db_set_pet_arg(0, PET_FLAG2, pet->flag2, pet[i].id, p->id);
			}
		}
	}
}

void set_all_pets_attr(uint32_t mapid)
{
	int i;
	map_t* map = get_map(mapid);

	if (!map || !map->pet_loaded || !map->pet_cnt)
		return;

	for (i = 0; i < map->pet_cnt; i++) {
		calc_pet_attr(mapid, &map->pets[i], 0, 0, 0, 0);
		db_pet_op(0, &map->pets[i], mapid);
	}
}

static inline int
terminate_pet_task(sprite_t* p, userid_t uid, pet_t* pet, int fulltime)
{
	const task_t* p_task = get_task(pet->taskid);
	int days = ((fulltime ? pet->trust_end_tm : get_now_tv()->tv_sec) - pet->start_tm)/86400;
//	int days = ((fulltime ? pet->trust_end_tm : get_now_tv()->tv_sec) - pet->start_tm);

	task_elem_upd_t task;
	task.petid			  = pet->id;
	task.taskid 		  = pet->taskid;
	task.flag			  = 1;
	task.add_tm 		  = days * 86400;
	task.start_tm		  = 0;
	task.end_tm 		  = 0;
	task.max_accu_tm	  = p_task->task_time;
	task.max_accu_tm_flag = p_task->task_end_flag;

	DEBUG_LOG("TERMINATE PET TASK\t[uid=%u pid=%u task=%u accu_days=%d]",(p ? p->id : 0), pet->id, pet->taskid, days);
	return db_set_task2(p, uid, task);
}
//----------------------------------------------------

//----------------------------------------------------
static inline void
notify_pet_cnt(sprite_t* p, int cnt)
{
	int i = sizeof(protocol_t);

	PKG_UINT8(msg, cnt, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_self(p, msg, i, 1);
}

static inline void
notify_pet_disappear(sprite_t* p, const pet_t* pet)
{
	if (p) {
		int i = sizeof(protocol_t);
		PKG_UINT32(msg, pet->id, i);
		PKG_STR(msg, pet->nick, i, sizeof p->nick);
		init_proto_head(msg, PROTO_PET_DISAPPEAR_NOTI, i);
		send_to_self(p, msg, i, 0);
	}
}
/*
static inline void
notify_pet_home(sprite_t* p, const pet_t* pet, uint32_t itmid, uint32_t type)
{
	uint8_t buf[sizeof(protocol_t) + 13];

	int i = sizeof(protocol_t);
	PKG_UINT32(buf, type, i);
	PKG_UINT32(buf, pet->id, i);
	PKG_UINT8(buf, 1, i);
	PKG_UINT32(buf, itmid, i);
	init_proto_head(buf, PROTO_PET_RETURN_HOME, i);
	send_to_self(p, buf, i, 0);
}*/

void notify_pet_status(sprite_t* p, const pet_t* pets, int cnt)
{
	char txt[256];

	int cmd = 0;

	int i = 0, len = 0;
	for ( ; i != cnt; ++i ) {
#ifdef TW_VER
		if ( PET_DEAD(&pets[i]) ) {
			cmd = 5;
			len = snprintf(txt, sizeof txt, "告訴你個不幸的消息，你的拉姆因久病不治目前已經死亡，看來你是一個完全不合格的主人，趕快回家看看吧，說不定還有什麼方法可以救活它哦！");
			break;
		} else if ( PET_SICK(&pets[i]) ) {
			len = snprintf(txt, sizeof txt, "你的拉姆現在已經病倒了，看來你還不是一個稱職的好主人哦，趕快回家看看吧！");
			break;
		} else if ( pets[i].hungry < 30 ) {
			len = snprintf(txt, sizeof txt, "你的拉姆現在非常饑餓，看來你沒有好好照顧它哦，趕快回家看看吧！");
			break;
		} else if ( pets[i].thirsty < 30 ) {
			len = snprintf(txt, sizeof txt, "你的拉姆現在非常口渴，看來你沒有好好照顧它哦，趕快回家看看吧！");
			break;
		} else if ( pets[i].sanitary < 30 ) {
			len = snprintf(txt, sizeof txt, "你的拉姆現在非常髒了，看來你沒有好好照顧它哦，趕快回家看看吧！");
			break;
		} else if ( pets[i].spirit < 30 ) {
			len = snprintf(txt, sizeof txt, "你的拉姆現在心情非常糟糕，看來你沒有好好照顧它哦，趕快回家看看吧！");
			break;
		}
#else
		if ( PET_DEAD(&pets[i]) ) {
			cmd = 5;
			len = snprintf(txt, sizeof txt, "告诉你个不幸的消息，你的拉姆因久病不治目前已经死亡，看来你是一个完全不合格的主人，赶快回家看看吧，说不定还有什么方法可以救活它哦！");
			break;
		} else if ( PET_SICK(&pets[i]) ) {
			len = snprintf(txt, sizeof txt, "你的拉姆现在已经病倒了，看来你还不是一个称职的好主人哦，赶快买瓶万能药水治疗它吧！");
			break;
		} else if ( pets[i].hungry < 30 ) {
			len = snprintf(txt, sizeof txt, "你的拉姆现在非常饥饿，看来你没有好好照顾它哦，赶快回家看看吧！");
			break;
		} else if ( pets[i].thirsty < 30 ) {
			len = snprintf(txt, sizeof txt, "你的拉姆现在非常口渴，看来你没有好好照顾它哦，赶快回家看看吧！");
			break;
		} else if ( pets[i].sanitary < 30 ) {
			len = snprintf(txt, sizeof txt, "你的拉姆现在非常脏了，看来你没有好好照顾它哦，赶快回家看看吧！");
			break;
		} else if ( pets[i].spirit < 30 ) {
			len = snprintf(txt, sizeof txt, "你的拉姆现在心情非常糟糕，看来你没有好好照顾它哦，赶快回家看看吧！");
			break;
		}
#endif
	}

	if (len) {
	    new_notify_system_message(p, txt, len, cmd);
		//notify_system_message(p, txt, len);
	}
}

static int
notify_trusted_pets(sprite_t* p, pet_t* pets, int count)
{
	time_t tmnow = get_now_tv()->tv_sec;
	int len = sizeof(protocol_t) + 4, i, cnt = 0;

	for ( i = 0; i != count; ++i ) {
		pet_t* pet = pets + i;
		if (PET_TRUSTED(pet)) {
			PKG_UINT32(msg, pet->id, len);
			PKG_STR(msg, pet->nick, len, PET_NICK_LEN);
			PKG_UINT32(msg, pet->color, len);
			PKG_UINT8(msg, get_pet_level(pet), len);
			PKG_UINT32(msg, pet->trust_end_tm - tmnow, len);
			PKG_UINT32(msg,pet->skill_bits,len);
			PKG_UINT32(msg,pet->skill_value,len);

			PKG_UINT32(msg, pet->suppl_info.pet_clothes[0], len);
			++cnt;
		}
	}

	*(uint32_t*)(msg + sizeof(protocol_t)) = htonl(cnt);
	init_proto_head(msg, p->waitcmd, len);
	return send_to_self(p, msg, len, 1);
}

static int
notify_on_task_pets(sprite_t* p, pet_t* pets, int count)
{
	time_t tmnow = get_now_tv()->tv_sec;
	int len = sizeof(protocol_t) + 1, i, cnt = 0;

	for ( i = 0; i != count; ++i ) {
		pet_t* pet = pets + i;
		if (PET_ON_TASK(pet)) {
			PKG_UINT32(msg, pet->id, len);
			PKG_STR(msg, pet->nick, len, PET_NICK_LEN);
			PKG_UINT32(msg, pet->color, len);
			PKG_UINT8(msg, get_pet_level(pet), len);
			PKG_UINT16(msg, (pet->trust_end_tm - tmnow)/86400 + 1, len);
			PKG_UINT32(msg,pet->skill_bits,len);
			PKG_UINT32(msg,pet->skill_value,len);
			PKG_UINT32(msg, pet->suppl_info.pet_clothes[0], len);
			++cnt;
		}
	}

	*(msg + sizeof(protocol_t)) = cnt;
	init_proto_head(msg, p->waitcmd, len);
	return send_to_self(p, msg, len, 1);
}

/*
affect client protocol 215 235 251
*/
static inline int
response_follow_pet(sprite_t *p, pet_t *pet)
{
	int i = sizeof(protocol_t);
	uint8_t lvl;

	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, pet->id, i);
	PKG_STR(msg, pet->nick, i, sizeof(pet->nick));
	PKG_UINT32(msg, pet->color, i);
	PKG_UINT32(msg, p->followed == pet, i);
	PKG_UINT8(msg, pet->hungry, i);
	PKG_UINT8(msg, pet->thirsty, i);
	PKG_UINT8(msg, pet->sanitary, i);
	PKG_UINT8(msg, pet->spirit, i);
	lvl = get_pet_level(pet);
	PKG_UINT8(msg, lvl, i);
	PKG_UINT32(msg, pet->skill, i);
	PKG_UINT32(msg, pet->skill_ex[0], i);
	PKG_UINT32(msg, pet->skill_ex[1], i);
	PKG_UINT32(msg, pet->skill_ex[2], i);
	PKG_UINT32(msg, pet->skill_bits, i);
	PKG_UINT32(msg, pet->skill_value, i);
	PKG_UINT8(msg, pet->skill_hot[0], i);
	PKG_UINT8(msg, pet->skill_hot[1], i);
	PKG_UINT8(msg, pet->skill_hot[2], i);

	PKG_UINT32(msg, pet->suppl_info.pet_clothes[0], i);
	PKG_UINT32(msg, pet->suppl_info.pet_honor[0], i);

	init_proto_head(msg, p->waitcmd, i);

	send_to_map(p, msg, i, 1);
	return 0;
}

/*
affect client protocol 212 240
*/

int response_proto_get_pets(sprite_t* p, pet_t* pets, uint32_t uid, int count, int followed, uint8_t type)
{
	int k, j;
	uint8_t lvl;

	k = sizeof (protocol_t);

	PKG_UINT32(msg, uid, k);
	PKG_UINT32 (msg, (followed&&(!type)) ? count - 1 : count, k);
	for (j = 0; j < count; j++) {
		pet_t *pet = pets + j;

		if (followed && (pet->id == followed) && (!type)) {
			continue;
		}

		PKG_UINT32(msg, pet->id, k);
		PKG_UINT32(msg, pet->flag, k);
		PKG_UINT32(msg, pet->birth, k);
		PKG_UINT32(msg, now.tv_sec - pet->logic_birthday, k);
		PKG_STR(msg, pet->nick, k, 16);
		PKG_UINT32(msg, pet->color, k);
		PKG_UINT32(msg, pet->sicktime, k);
		PKG_UINT8(msg, pet->posX, k);
		PKG_UINT8(msg, pet->posY, k);
		PKG_UINT8(msg, pet->hungry, k);
		PKG_UINT8(msg, pet->thirsty, k);
		PKG_UINT8(msg, pet->sanitary, k);
		PKG_UINT8(msg, pet->spirit, k);

		lvl = get_pet_level(pet);
		PKG_UINT8(msg, lvl, k);
		PKG_UINT32(msg, pet->skill, k);
		PKG_UINT32(msg, pet->sick_type, k);
		PKG_UINT32(msg, pet->skill_ex[0], k);
		PKG_UINT32(msg, pet->skill_ex[1], k);
		PKG_UINT32(msg, pet->skill_ex[2], k);
		PKG_UINT32(msg, pet->skill_bits, k);
		PKG_UINT32(msg, pet->skill_value, k);
		PKG_UINT8(msg, pet->skill_hot[0], k);
		PKG_UINT8(msg, pet->skill_hot[1], k);
		PKG_UINT8(msg, pet->skill_hot[2], k);

		PKG_UINT32(msg, pet->suppl_info.pet_clothes[0], k);
		PKG_UINT32(msg, pet->suppl_info.pet_honor[0], k);
	}

	DEBUG_LOG("RSP PET\t[%u %u %u len=%u]", p->id, uid, (followed&&(!type)) ? count - 1 : count, k);
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

static inline void
response_proto_play_pet(sprite_t *p, uint32_t userid, pet_t *pet, int action)
{
	int i = sizeof (protocol_t);

	PKG_UINT32 (msg, userid, i);
	PKG_UINT32 (msg, pet->id, i);
	PKG_UINT32 (msg, action, i);
	PKG_UINT8 (msg, pet->hungry, i);
	PKG_UINT8 (msg, pet->thirsty, i);
	PKG_UINT8 (msg, pet->sanitary, i);
	PKG_UINT8 (msg, pet->spirit, i);
	init_proto_head (msg, PROTO_PLAY_WITH_PET, i);

	send_to_map (p, msg, i, 1);
}
//----------------------------------------------------

//------------------------- DB-related functions ---------------------------
int db_pet_op(sprite_t* p, const pet_t* pet, uint32_t userid)
{
	pet_basic_info_t petinfo;
	petinfo.id       = pet->id;
	petinfo.flag     = pet->flag;
	petinfo.flag2    = pet->flag2;
	petinfo.sicktime = pet->sicktime;
	petinfo.hungry   = pet->hungry;
	petinfo.thirsty  = pet->thirsty;
	petinfo.sanitary = pet->sanitary;
	petinfo.spirit   = pet->spirit;
	petinfo.trust_end_tm = pet->trust_end_tm;
	petinfo.start_tm = pet->start_tm;
	petinfo.taskid   = pet->taskid;
	petinfo.stamp    = pet->stamp;
	//DEBUG_LOG("pid=%u flag=%u h=%u t=%u s=%u s=%u",
	//			pet->id, pet->flag, pet->hungry, pet->thirsty, pet->sanitary, pet->spirit);
	return send_request_to_db(SVR_PROTO_SET_PET2, p, sizeof petinfo, &petinfo, userid);
}
//----------------------------------------------------------------------

//------------------------- callbacks for dealing with DB return ---------------------------
int add_pet_return(sprite_t* p, uint32_t id, char* buf, int len)
{
	if ( (len != 4) || (id != p->id) ) {
		ERROR_RETURN(("error len=%d, id=%u,%u, waitcmd=%d", len, p->id, id, p->waitcmd), -1);
	}

	assert(p->sess_len == sizeof(pet_t));

	uint32_t petid = *(uint32_t*)buf;
	map_t*   map   = get_map(id);
	if (map && map->pet_loaded) {
		memcpy(&map->pets[map->pet_cnt], p->session, sizeof(pet_t));
		map->pets[map->pet_cnt].id = petid;
		map->pets[map->pet_cnt].suppl_info.uid = p->id;
		++(map->pet_cnt);
	}
	p->sess_len = 0;

	int i = sizeof(protocol_t);
	switch (p->waitcmd) {
	case PROTO_BUY_PET:
	case PROTO_ASK_FOR_A_PET:
	{
		char txt[192] = {0};
		#ifdef TW_VER
		snprintf(txt, sizeof txt, "   嘿！你可以帶小拉姆去拉姆學院學習嘍，那裡的課程十分豐富，可以學習到各種本領哦！");
		send_postcard("菩提校長", 0, p->id, 1000082, txt, 0);
		#else
		snprintf(txt, sizeof txt, "   嘿！你可以带小拉姆去拉姆学院学习喽，那里的课程十分丰富，可以学习到各种本领哦！");
		send_postcard("菩提校长", 0, p->id, 1000082, txt, 0);
		#endif
		send_attr_update_noti(0, 0, p->id, 3);
		PKG_UINT32(msg, petid, i);
		break;
	}
	case PROTO_FETCH_REALIZED_WISH:
		break;
	default:
		ERROR_LOG("Unexpected WaitCmd %d", p->waitcmd);
		return send_to_self_error(p, p->waitcmd, -ERR_system_error, 1);
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int feed_pet_callback(sprite_t* p)
{
	assert (p->sess_len == 16);

	int i = *(uint32_t*)(p->session + 8);
	uint32_t uid = *(uint32_t*)(p->session + 12);
	item_t* pItm = *(item_t **)p->session;
	pet_t*  pPet;
	if (!(pPet = get_pet (uid, i)))
		ERROR_RETURN(("can't find pet=%u, id=%u", i, p->id), -1);

	p->sess_len = 0;
	i = sizeof (protocol_t);
	PKG_UINT32(msg, uid, i);
	PKG_UINT32(msg, pItm->id, i);
	PKG_UINT8(msg, pItm->u.feed_attr.type, i);
	PKG_UINT32(msg, pPet->flag, i);
	PKG_UINT32(msg, pPet->id, i);
	PKG_UINT8(msg, pPet->hungry, i);
	PKG_UINT8(msg, pPet->thirsty, i);
	PKG_UINT8(msg, pPet->sanitary, i);
	PKG_UINT8(msg, pPet->spirit, i);
	init_proto_head (msg, PROTO_FEED_ITEM_USE, i);
	send_to_map(p, msg, i, 1);
	return 0;
}

int get_pet_return(sprite_t* p, uint32_t id, char* buf, int len)
{
	int count;
	//sprite_t* who;
	map_t* map;

	if ( (len < 4) || (p->waitcmd != PROTO_GET_PET && p->waitcmd != PROTO_GET_ALL_PET_INFO) ) {
		ERROR_RETURN(("error len=%d, id=%u,%u, waitcmd=%d", len, p->id, id, p->waitcmd), -1);
	}

	int i = 0;
	UNPKG_H_UINT32(buf, count, i);
	CHECK_BODY_LEN(len - 4, count * (sizeof(pet_t) - sizeof(pet_suppl_info_t)));
	if (count > MAX_PETS_PER_SPRITE) {
		ERROR_RETURN(("too many pets, id=%u, count=%d", id, count), -1);
	}
	uint8_t buff[2048] = {0};
	switch (p->waitcmd) {
	case PROTO_GET_PET:
		map = get_map(id);
		if (!map) {
			map = alloc_map(id);
		}
		break;
	case PROTO_GET_ALL_PET_INFO:
		map = get_map(id);
		if (!map) {
			map = (map_t*)buff;
		}
		break;
	}
	for (i = 0; i != count; ++i) {
		//DEBUG_LOG("PET INFO1\t[%u %u %u %u]", p->id, id, map->pets[i].id, *(uint32_t*)(buf + 4 + i * (sizeof(pet_t) - sizeof(pet_suppl_info_t))));
		memcpy(&(map->pets[i]), buf + 4 + i * (sizeof(pet_t) - sizeof(pet_suppl_info_t)), sizeof(pet_t) - sizeof(pet_suppl_info_t));
		map->pets[i].suppl_info.uid = id;
        if((PET_IS_SUPER_LAHM(&(map->pets[i])) || PET_SICK((&map->pets[i])) || PET_DEAD(&(map->pets[i]))) && map->pets[i].sick_type) {
            map->pets[i].sick_type = 0;
            db_infect_pet(NULL, &(map->pets[i]), id);
        }
	}
	map->pet_loaded = 1;
	map->pet_cnt = count;
	if (p->waitcmd == PROTO_GET_PET) {
		for (i = 0; i != count; i++) {
			calc_pet_attr(id, &(map->pets[i]), 0, 0, 0, 0);
		}
	}
	DEBUG_LOG("PET NUM\t[%u %u %u]", p->id, id, map->pet_cnt);
	/*
	who = get_sprite(id);
	i = ( (who && who->followed) ? who->followed->id : 0 );
	*/
	switch (p->waitcmd) {
	case PROTO_GET_PET:
		return db_get_all_pets_weared(p, id);
	case PROTO_GET_ALL_PET_INFO:
		return response_proto_get_pets(p, map->pets, id, map->pet_cnt, 0, 0);
	default:
		ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}

	return 0;
}

int set_pet_return(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);

	switch (p->waitcmd){
	case PROTO_SET_PET_NICK:
		response_proto_head(p, p->waitcmd, 0);
		break;
	case PROTO_MODIFY_PET_COLOR:
	{
		uint32_t petid = *(uint32_t*)p->session;
		uint32_t color = *(uint32_t*)(p->session + 4);
		uint32_t itemid = *(uint32_t*)(p->session + 8);
		pet_t* ppet = get_pet(p->id, petid);
		if (!ppet)
			ERROR_RETURN(("bad petid %u", petid), -1);

		item_t* pitem = get_item_prop(itemid);
		if (!pitem)
			ERROR_RETURN(("bad itemid %u", itemid), -1);

		int j = sizeof(protocol_t);
		PKG_UINT32(msg, itemid, j);
		PKG_UINT32(msg, pitem->u.feed_attr.type, j);
		PKG_UINT32(msg, ppet->flag, j);
		PKG_UINT32(msg, petid, j);
		PKG_UINT32(msg, color, j);
		init_proto_head(msg, p->waitcmd, j);
		send_to_map(p, msg, j, 1);
		break;
	}
	case PROTO_SET_STH_DONE:
	{
		const exclu_things_t* exclu = *((const exclu_things_t**)(p->session));

		int i = sizeof(protocol_t);
		PKG_UINT32(msg, exclu->exclu_id, i);
		PKG_UINT32(msg, 1, i);
		PKG_UINT32(msg, 1001, i);
		PKG_UINT32(msg, 1, i);
		init_proto_head(msg, p->waitcmd, i);
		send_to_self(p, msg, i, 1);
		break;
	}

	}
	return 0;
}

int set_pet2_return(sprite_t* p, uint32_t id, char *buf, int len)
{
	item_t *itm;

	CHECK_BODY_LEN (len, 0);
	switch (p->waitcmd) {
	case PROTO_FEED_ITEM_USE:
		assert (p->sess_len == 8);
		itm = *(item_t **)p->session;
		if (itm->u.feed_attr.type != 2) {
			return db_single_item_op(p, p->id, itm->id, 1, 0);
		} else {
			return feed_pet_callback(p);
		}
	case PROTO_FOLLOW_PET:
		return response_follow_pet(p, p->followed);
	case PROTO_BECOME_SUPER_LAHM:
#ifdef TW_VER
		send_postcard("克勞神父", 0, p->id, 1000026, "恭喜！你的拉姆已經變成超級拉姆了！超級拉姆是莊園的守護精靈。你將有更大的責任做一名樂於助人的小摩爾。", 0);
#else
		send_postcard("克劳神父", 0, p->id, 1000026, "恭喜！你的拉姆已经变成超级拉姆了！超级拉姆是庄园的守护精灵。你将有更大的责任做一名乐于助人的小摩尔。", 0);
#endif
		DEBUG_LOG("BECOME SUPER LAHM\t[uid=%u pid=%u]", p->id, p->followed->id);
		// become 'super lahm' successfully, response `uint8_t 1`
		response_proto_uint8(p, p->waitcmd, 1, 0);
		return 0;
	case PROTO_PET_TRUST:
		p->followed = 0;
		response_proto_uint32(p, p->waitcmd, p->id, 1);
		db_inc_trusted_pet_cnt();
		break;
	case PROTO_PICK_PET_TASK:
	{
		//
		pick_pet_task_sess_t* tasksess = (void*)(p->session);
		const pet_t* pet = tasksess->pet;
		p->followed = 0;
		response_proto_uint32(p, p->waitcmd, p->id, 1);
		//
		char msgbuf[256];
#ifdef TW_VER
		snprintf(msgbuf, sizeof msgbuf,
					"　　親愛的%.16s，你為拉姆報了%d天的%s。%d天後，我就會安排它回家了。",
					p->nick, tasksess->days, tasksess->task->name, tasksess->days);
#else
		snprintf(msgbuf, sizeof msgbuf,
					"　　亲爱的%.16s，你为拉姆报了%d天的%s。%d天后，我就会安排它回家了。",
					p->nick, tasksess->days, tasksess->task->name, tasksess->days);
#endif
		send_postcard("菩提大伯", 0, p->id, 1000022, msgbuf, 0);
		//
		db_inc_ontask_pet_cnt();
		//
		task_elem_upd_t task;
		task.petid            = pet->id;
		task.taskid           = pet->taskid;
		task.flag             = 2;
		task.add_tm           = 0;
		task.start_tm         = pet->start_tm;
		task.end_tm           = pet->trust_end_tm;
		task.max_accu_tm      = tasksess->task->task_time;
		task.max_accu_tm_flag = 2;
		db_set_task2(0, p->id, task);
		//
		send_attr_update_noti(0, p, p->id, 3);
		break;
	}
	case PROTO_PET_WITHDRAW:
		do_exchange_item(p, 1601, 0);
		db_dec_trusted_pet_cnt();
		break;
	/*
	case PROTO_SET_STH_DONE:
	{
		const exclu_things_t* exclu = *((const exclu_things_t**)(p->session));

		int i = sizeof(protocol_t);
		PKG_UINT32(msg, exclu->exclu_id, i);
		PKG_UINT32(msg, 1, i);
		PKG_UINT32(msg, 1001, i);
		PKG_UINT32(msg, 1, i);
		init_proto_head(msg, p->waitcmd, i);
		send_to_self(p, msg, i, 1);
		break;
	}
	*/
	default:
		ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}

	return 0;
}

int set_pets_pos_return (sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN (len, 0);
	if (p->waitcmd != PROTO_SET_PET_POS)
		ERROR_RETURN (("error waitcmd=%d, id=%u", p->waitcmd, p->id), -1);
	assert (p->sess_len > 4);
	init_proto_head (p->session, p->waitcmd, p->sess_len);
	send_to_map (p, p->session, p->sess_len, 1);
	p->sess_len = 0;

	return 0;
}

int set_pet_skill_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	p->followed->skill = *((uint32_t*)buf);

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
int do_buy_pet(sprite_t* p, uint32_t itemid, uint32_t color, const char* nick, int charge,
    uint32_t flag)
{
	item_kind_t* kind;
	item_t*      itm;
	pet_t*       pet;

	itm  = get_item_prop(itemid);
	kind = find_kind_of_item(itemid);
	if (!itm || !kind || (kind->kind != PET_ITEM_KIND)) {
		ERROR_RETURN(("error itemid=%u, uin=%u", itemid, p->id), -1);
	}

	p->sess_len   = sizeof(pet_t);
	pet = (pet_t *)p->session;
	memset(pet, 0, p->sess_len);
	pet->color    = color;
	pet->birth    = now.tv_sec;
	pet->logic_birthday = now.tv_sec - 26*60*60;
	pet->stamp    = now.tv_sec;
	pet->hungry   = 100;
	pet->thirsty  = 100;
	pet->sanitary = 100;
	pet->spirit   = 100;
	strncpy(pet->nick, nick, sizeof (pet->nick) - 1);

	return (charge ?
			db_attr_op(p, -itm->price, 0, 0, 0, 0, ATTR_CHG_buy_pet, itm->id) :
			db_add_pet(p, pet));
}

static int
do_feed_item(sprite_t *p, uint32_t uid, uint32_t petid, uint32_t itemid)
{
	item_kind_t* kind = find_kind_of_item(itemid);
	item_t*      itm  = get_item_prop(itemid);
	pet_t*       pet;

	if ( !itm || !kind || (kind->kind != FEED_ITEM_KIND) ) {
		ERROR_RETURN(("error itemid=%u, id=%u", itemid, p->id), -1);
	}
	if ( !(pet = get_pet(uid, petid)) ) {
		//ERROR_RETURN(("no pet, petid=%u, id=%u", petid, p->id), -1);
		return send_to_self_error(p, p->waitcmd, -ERR_pet_infected_cannot_feed, 1);
	}

	//if (is_pet_infected(pet, PALASEN_SICK)
	//	|| is_pet_infected(pet, FEIWUTANGYAN_SICK)
	//	|| is_pet_infected(pet, PIYANGYANG_SICK)) {
	//	return send_to_self_error(p, p->waitcmd, -ERR_pet_infected_cannot_feed, 1);
	//}

	*(unsigned long *)p->session = (unsigned long)itm;
	*(unsigned int *)(p->session + 8) = petid;
	*(unsigned int *)(p->session + 12) = uid;
	p->sess_len = 16;

	if ( itm->u.feed_attr.curability && PET_SICK(pet) ) {
		RESET_PET_SICK(pet);
		pet->sicktime = 0;
	}
	if ( itm->u.feed_attr.rebirth && PET_DEAD(pet) ) {
		RESET_PET_DEAD(pet);
		pet->sicktime = 0;
	}

	if (itemid == 180005){
	    uint32_t buff[2] = {0};
	    buff[0] = petid;
	    buff[1] = 0;
	    send_request_to_db(SVR_PROTO_SET_PET_SICKTYPE , NULL, 8, buff, p->id);
	}

	//calc_pet_attr(p->id, pet, itm->u.feed_attr.hungry, itm->u.feed_attr.thirsty, itm->u.feed_attr.sanitary, itm->u.feed_attr.spirit);

	//return db_pet_op(p, pet, p->id);
	return db_feed_pet(p, pet->flag, pet->sicktime, get_now_tv()->tv_sec, itm->u.feed_attr.hungry,	itm->u.feed_attr.thirsty, itm->u.feed_attr.sanitary, itm->u.feed_attr.spirit, petid, uid);
}

static int
do_follow_pet(sprite_t *p, uint32_t petid, uint32_t status)
{
	CHECK_INT_LE(status, 1);

	pet_t* pet = get_pet(p->id, petid);
	if (!pet) {
		ERROR_RETURN(("can't find pet=%u, uid=%u", petid, p->id), -1);
	}

	if (p->followed)
		PET_RECOVER_FORM(p->followed);

	if (status) {
		p->followed = pet;
		calc_pet_attr_full(p->id, pet, -1, -1, -2, 1, 1);
		//db_update_pet_attr(0, pet->stamp, -1, -1, -2, 1, pet->id, p->id);
	} else {
		p->followed = 0;
	}
	broadcast_pet_info(p, status);
	return response_follow_pet(p, pet);
}

static inline int
do_set_pet_nick(sprite_t *p, uint32_t petid, const char *nick)
{
	pet_t *pet = get_pet(p->id, petid);

	if (!pet) {
		ERROR_RETURN(("can't find petid=%d, id=%u", petid, p->id), -1);
	}
	uint8_t buff[20];
	*(uint32_t*)buff = petid;
	memcpy(&buff[4], nick, sizeof pet->nick);
	return db_set_pet_nick(p, buff, p->id);
}

int do_set_pet_color(sprite_t *p, uint32_t petid, uint32_t color)
{
	pet_t *pet = get_pet(p->id, petid);

	if (!pet) {
		ERROR_RETURN(("can't find petid=%d, id=%u", petid, p->id), -1);
	}

	return db_set_pet_arg(p, PET_COLOR, color, petid, p->id);
}

static int
do_set_pets_pos(sprite_t *p, int count, pet_pos_t *pos)
{
	char buf[6 * count + 4];
	int i;

	for (i = 0; i < count; i++) {
		pet_t *pet = get_pet (p->id, pos[i].id);
		if (!pet) ERROR_RETURN (("not exists pet=%u, id=%u", pos[i].id, p->id), -1);
		pet->posX = pos[i].x;
		pet->posY = pos[i].y;
	}
	//for reply to client
	*(int*)buf = count;
	memcpy (buf + 4, pos, 6 * count);

	return db_set_pets_pos (p, count, buf);
}

static inline int
do_play_with_pet(sprite_t *p, uint32_t userid, uint32_t petid)
{
	int action;
	pet_t *pet = get_pet(userid, petid);

	if (!pet) ERROR_RETURN (("can't pet=%d, id=%u", userid, petid), -1);

	action = 1 + (int) (2.0 * (rand() / (RAND_MAX + 1.0)));
	response_proto_play_pet (p, userid, pet, action);
	return 0;
}

int do_get_pet(sprite_t* p, uint32_t petid, uint32_t userid, uint8_t type)
{
	map_t *map;

	map = get_map(userid);
	if (map && map->pet_loaded && p->waitcmd != PROTO_GET_PET) {
		//get all pets
		if (petid == 0) {
			switch (p->waitcmd) {
			case PROTO_GET_PET_CNT:
				notify_pet_cnt(p, map->pet_cnt);
				return 0;
			case PROTO_GET_TRUSTED_PET:
				return notify_trusted_pets(p, map->pets, map->pet_cnt);
			case PROTO_GET_PETS_ON_TASK:
				return notify_on_task_pets(p, map->pets, map->pet_cnt);
			case PROTO_GET_ALL_PET_INFO:
				return response_proto_get_pets(p, map->pets, userid, map->pet_cnt, 0, 0);
			default:
				ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
			}
		}
	}

	if (petid != 0)
		return db_get_one_pet(p, petid, userid);

	return db_get_pet(p, petid, userid);
}

int do_trust_pet(sprite_t* p)
{
	trust_pet_sess_t* petsess = (void*)(p->session);
	pet_t* pet = petsess->pet;
	SET_PET_TRUSTED(pet);
	pet->sicktime = 0;
	pet->hungry   = 100;
	pet->thirsty  = 100;
	pet->sanitary = 100;
	pet->spirit   = 100;
	pet->stamp    = get_now_tv()->tv_sec;
	pet->trust_end_tm = pet->stamp + petsess->duration;
	p->sess_len   = 0;

	//return db_pet_op(p, pet, p->id);
	return db_trust_pet(p,pet->flag, pet->trust_end_tm, pet->stamp, 100, 100, 100, 100, pet->id, p->id);
}

int do_pick_pet_task(sprite_t* p)
{
	pick_pet_task_sess_t* tasksess = (void*)(p->session);
	pet_t* pet = tasksess->pet;
	SET_PET_ON_TASK(pet);
	pet->hungry   = 100;
	pet->thirsty  = 100;
	pet->sanitary = 100;
	pet->spirit   = 100;
	pet->stamp    = get_now_tv()->tv_sec;
	pet->trust_end_tm = pet->stamp + (tasksess->days * 86400);
	//pet->trust_end_tm = pet->stamp + (tasksess->days * 1);
	pet->start_tm = pet->stamp;
	pet->taskid   = tasksess->task->id;

	//return db_pet_op(p, pet, p->id);
	return db_pet_task(p, pet->flag, pet->trust_end_tm, pet->start_tm, pet->taskid, pet->stamp, 100, 100, 100, 100, pet->id, p->id);
}

int do_pick_magic_pet_task(sprite_t* p)
{
	pick_pet_magic_task_sess_t* tasksess = (void*)(p->session);
	const pet_t* pet = tasksess->pet;
	task_t* task = tasksess->task;
	//db_inc_ontask_pet_cnt();

	magic_task_elem_upd_t mteu;
	mteu.petid			  = pet->id;
	mteu.taskid 		  = task->id;
	mteu.flag			  = 2;
	mteu.add_tm 		  = 0;
	mteu.start_tm		  = 0;
	mteu.end_tm 		  = 0;
	mteu.max_accu_tm	  = 0;
	mteu.max_accu_tm_flag = 0;
	mteu.stage 			  = tasksess->stage;
	return db_set_magic_task(p, p->id, mteu);
}

int do_terminate_pet_task(sprite_t* p, userid_t uid, uint32_t petid, int fulltime)
{
	pet_t* pet = get_pet(uid, petid);

	RESET_PET_ON_TASK(pet);
	if (fulltime) {
		const task_t* task = get_task(pet->taskid);
		// Send a Postcard to Inform the Player
		char msgbuf[256];
		if (pet->taskid == 15) {
#ifdef TW_VER
		snprintf(msgbuf, sizeof msgbuf, "　　親愛的小摩爾，你的拉姆已經完成了%u天的%s學習。我已安排它回家，記得帶它去拉姆課堂參加英語考核哦。", (pet->trust_end_tm - pet->start_tm)/86400, task->name);
#else
		snprintf(msgbuf, sizeof msgbuf, "　　亲爱的小摩尔，你的拉姆已经完成了%u天的%s学习。我已安排它回家，记得带它去拉姆课堂参加英语考核哦。", (pet->trust_end_tm - pet->start_tm)/86400, task->name);
#endif
		} else if (pet->taskid == 16) {
#ifdef TW_VER
		snprintf(msgbuf, sizeof msgbuf, "　　親愛的小摩爾，你的拉姆已經完成了%u天的%s學習。我已安排它回家，完成課程后，記得帶它去拉姆課堂參加考核哦。", (pet->trust_end_tm - pet->start_tm)/86400, task->name);
#else
		snprintf(msgbuf, sizeof msgbuf, "　　亲爱的小摩尔，你的拉姆已经完成了%u天的%s学习。我已安排它回家，完成课程后，记得带它去拉姆课堂参加考核哦。", (pet->trust_end_tm - pet->start_tm)/86400, task->name);
#endif
		} else {
#ifdef TW_VER
		snprintf(msgbuf, sizeof msgbuf, "　　親愛的小摩爾，你的拉姆已經完成了%u天的%s學習。我已安排它回家，快回小屋和它相聚吧。", (pet->trust_end_tm - pet->start_tm)/86400, task->name);
#else
		snprintf(msgbuf, sizeof msgbuf, "　　亲爱的小摩尔，你的拉姆已经完成了%u天的%s学习。我已安排它回家，快回小屋和它相聚吧。", (pet->trust_end_tm - pet->start_tm)/86400, task->name);
#endif
		}
		send_postcard("菩提大伯", 0, uid, 1000023, msgbuf, 0);
		//
		pet->stamp = pet->trust_end_tm;
	} else {
		pet->stamp = get_now_tv()->tv_sec;
	}
	pet->trust_end_tm = 0;
	pet->start_tm     = 0;
	pet->taskid       = 0;

	db_dec_ontask_pet_cnt();
	//db_pet_op(0, pet, uid);
	db_pet_task(0, pet->flag, pet->trust_end_tm, pet->start_tm, pet->taskid, pet->stamp, 0, 0, 0, 0, pet->id, uid);

	if (p) {
		response_proto_head(p, p->waitcmd, 0);
	}
	return 0;
}

static inline int
do_withdraw_pet(sprite_t* p, uint32_t petid)
{
	pet_t* pet = get_pet(p->id, petid);
	if (pet && PET_TRUSTED(pet)) {
		RESET_PET_TRUSTED(pet);
		pet->stamp        = get_now_tv()->tv_sec;
		pet->trust_end_tm = 0;
		//return db_pet_op(p, pet, p->id);
		return db_trust_pet(p, pet->flag, pet->trust_end_tm, pet->stamp, 0, 0, 0, 0, petid, p->id);
	}

	ERROR_RETURN(("fail to withdraw pet: uid=%u petid=%u flag=0x%X",
					p->id, petid, pet ? pet->flag : 0), -1);
}

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
static inline int
unpkg_set_pet_pos (const uint8_t body[], int bodylen, int* count, pet_pos_t* p)
{
	int j, i = 0;
	if (bodylen <= 4)
		ERROR_RETURN (("error len=%d", bodylen), -1);

	UNPKG_UINT32(body, *count, i);
	CHECK_BODY_LEN(bodylen - 4, (*count) * 6);
	if (*count > MAX_PETS_PER_SPRITE)
		ERROR_RETURN (("error count=%d", *count), -1);

	for (j = 0; j < *count; j++) {
		pet_pos_t *pos = p + j;
		UNPKG_UINT32 (body, pos->id, i);
		UNPKG_UINT8 (body, pos->x, i);
		UNPKG_UINT8 (body, pos->y, i);
	}
	return 0;
}

static inline int
unpkg_pet_nick (const uint8_t body[], int bodylen, uint32_t* petid, char* nick)
{
	int i = 0;
	CHECK_BODY_LEN(bodylen, 20);

	UNPKG_UINT32(body, *petid, i);
	UNPKG_STR(body, nick, i, 16);

	return 0;
}

static inline int
unpkg_buy_pet(const uint8_t body[], int bodylen, uint32_t* itemid, uint32_t* color, char *nick)
{
	int i = 0;
	CHECK_BODY_LEN (bodylen, 24);

	UNPKG_UINT32(body, *itemid, i);
	UNPKG_UINT32(body, *color, i);
	UNPKG_STR(body, nick, i, 16);
	return 0;
}
//-------------------------------------------------------------------------------

//--------------- funcs for proto.c -------------------------------------------------
int ask_for_a_pet_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	DEBUG_LOG("ASK FOR A PET\t[uid=%u]", p->id);

    if(!set_only_one_bit(p, 102))
	{
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

#ifdef TW_VER
	return do_buy_pet(p, 170002, 2, "小黄黄", 0, 1);
#else
	return do_buy_pet(p, 170002, 2, "小黄黄", 0, 1);
#endif
}

int buy_pet_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t itemid, color;
	char nick[16];

	if (unpkg_buy_pet(body, bodylen, &itemid, &color, nick) == -1)
		return -1;

	DEBUG_LOG("BUY PET\t[%u %u %u]", p->id, itemid, color);
	return do_buy_pet(p, itemid, color, nick, 1, 0);
}

int feed_item_cmd(sprite_t *p, const uint8_t *body, int len)
{
	uint32_t petid, itemid, uid;
	CHECK_BODY_LEN(len, 12);

	int j = 0;
	UNPKG_UINT32(body, uid, j);
	UNPKG_UINT32(body, petid, j);
	UNPKG_UINT32(body, itemid, j);

	DEBUG_LOG("FEED PET\t[%u %u %u %u]", p->id, uid, petid, itemid);
	msglog(statistic_logfile, 0x0404010B,get_now_tv()->tv_sec, &(p->id), 4);
	return do_feed_item(p, uid, petid, itemid);
}

int follow_pet_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t petid, status;
	if (unpkg_uint32_uint32(body, bodylen, &petid, &status) == -1)
		return -1;

	DEBUG_LOG("FOLLOW PET\t[%u %u %d]", p->id, petid, status);
	return do_follow_pet(p, petid, status);
}

int follow_outside_pet_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t userid, petid;
	CHECK_BODY_LEN(bodylen, 8);

	int i = 0;
	UNPKG_UINT32(body, userid, i);
	UNPKG_UINT32(body, petid, i);

	CHECK_VALID_ID(userid);
	if ( p->id != userid) {
		ERROR_RETURN( ("Invalid Req: uid=%u puid=%u petid=%u ",
					p->id, userid, petid), -1 );
	}

	DEBUG_LOG("FOLLOW OUTSIDE PET\t[userid=%u petid=%u]", userid, petid);

	pet_t* pet = get_pet(p->id, petid);
	if (!pet) {
		ERROR_RETURN(("can't find pet=%u, uid=%u", petid, p->id), -1);
	}
	if(pet->suppl_info.mapid != p->tiles->id){
		ERROR_LOG("Can not follow pet in other map, pet in map %lu, sprite in map %lu", pet->suppl_info.mapid, p->tiles->id);
		return send_to_self_error(p, p->waitcmd, -ERR_can_not_follow_pet, 1);
	}

	p->followed = pet;
	if(PET_VISIT(pet)||PET_GO_OUT(pet)){
		if(PET_GO_OUT(pet)){
			RESET_PET_GO_OUT(pet);
		}else if(PET_VISIT(pet)){
			RESET_PET_VISIT(pet);
		}
		pet->trust_end_tm  = 0;
		rm_pet_from_map(p, petid, p->tiles->id);
		pet->suppl_info.mapid = 0;
		db_set_flag_endtime(0, pet->flag, pet->trust_end_tm, pet->id, p->id);
		calc_pet_attr_full(p->id, pet, 0, 0, 0, 20, 1);
		//db_update_pet_attr(0, pet->stamp, 0, 0, 0, 20, pet->id, p->id);
	}
	return response_follow_pet(p, pet);
}

int do_send_pet_home(sprite_t* p)
{
	uint32_t userid, petid;
	userid = *(uint32_t*)p->session;
	petid = *(uint32_t*)(p->session + 4);

	pet_t* pet = get_pet(userid, petid);
	sprite_t* ptmp = get_sprite(userid);
	if (!pet || !ptmp) {
		return send_to_self_error(p, p->waitcmd, -ERR_pet_already_in_home, 1);
	}

	RESET_PET_GO_OUT(pet);
	pet->trust_end_tm  = 0;
	respond_pet_to_map(pet, PET_GO_HOME);
	rm_pet_from_map(ptmp, pet->id, pet->suppl_info.mapid);
	pet->suppl_info.mapid = 0;
	db_set_flag_endtime(0, pet->flag, pet->trust_end_tm, pet->id, userid);

	p->lovely += 2;
	do_db_attr_op(NULL, p->id, 0, 0, 0, 0, 2, ATTR_CHG_roll_back, 0);

	char txt[128];
#ifdef TW_VER
	snprintf(txt, sizeof txt, "親愛的%s，你的小拉姆外出遊玩迷路了。我已經把它送回家了，記得好好照看它哦！", ptmp->nick);
#else
	snprintf(txt, sizeof txt, "亲爱的%s，你的小拉姆外出游玩迷路了。我已经把它送回家了，记得好好照看它哦！", ptmp->nick);
#endif
	send_postcard(p->nick, 0, userid, 1000088, txt, 0);
	response_proto_head(p, p->waitcmd, 0);

	return 0;
}

int send_pet_home_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t userid, petid;
	CHECK_BODY_LEN(bodylen, 8);

	int i = 0;
	UNPKG_UINT32(body, userid, i);
	UNPKG_UINT32(body, petid, i);

	CHECK_VALID_ID(userid);
	if ( p->id == userid) {
		ERROR_RETURN(("cannot send your self pet\t[%u %u %u]",	p->id, userid, petid), -1 );
	}
	DEBUG_LOG("SEND PET HOME\t[pid=%u uid=%u petid=%u]", p->id, userid, petid);
	pet_t* pet = get_pet(userid, petid);
	if (!pet) {
		return send_to_self_error(p, p->waitcmd, -ERR_pet_already_in_home, 1);
	}
	if(!PET_GO_OUT(pet)) {
		return send_to_self_error(p, p->waitcmd, -ERR_pet_already_in_home, 1);
	}

	DEBUG_LOG("SEND PET HOME\t[pid=%u uid=%u petid=%u]", p->id, userid, petid);
	*(uint32_t*)p->session = userid;
	*(uint32_t*)(p->session + 4) = petid;

	return db_set_sth_done(p, 201, 10, p->id);
}

int get_pet_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 9);

	uint8_t  type;
	uint32_t petid, userid;

	int i = 0;
	UNPKG_UINT32(body, userid, i);
	UNPKG_UINT32(body, petid, i);
	UNPKG_UINT8(body, type, i);

	CHECK_VALID_ID(userid);
	if ( type && ((p->id != userid) || petid) ) {
		ERROR_RETURN( ("Invalid Req: uid=%u puid=%u petid=%u type=%d",
				p->id, userid, petid, type), -1 );
	}

	*(uint32_t*)p->session = type;
	DEBUG_LOG("GET PET\t[uid=%u puid=%u pid=%u type=%d]", p->id, userid, petid, type);
	return do_get_pet(p, petid, userid, type);
}

int get_all_pet_info_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	uint32_t userid;

	int i = 0;
	UNPKG_UINT32(body, userid, i);
	CHECK_VALID_ID(userid);

	DEBUG_LOG("GET ALL PET\t[uid=%u puid=%u]", p->id, userid);
	return do_get_pet(p, 0, userid, 0);
}

int get_pet_cnt_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	uint32_t userid = ntohl(*(uint32_t*)body);

	CHECK_VALID_ID(userid);

	DEBUG_LOG("GET PET CNT\t[chker=%u chkee=%u]", p->id, userid);
	return do_get_pet(p, 0, userid, 0);
}

int learn_pet_skill_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	uint32_t type, done = 10000001;

	if (!(p->followed)) {
		ERROR_RETURN(("No Pets Followed Now: uid=%u", p->id), -1);
	}

	if ((unpkg_uint32(body, bodylen, &type) == 0) && (type < 3)) {
		p->sess_len = 0;
		PKG_H_UINT32(p->session, type, p->sess_len);

		return db_set_sth_done(p, done, 1, p->id);
	}

	return -1;
}

int play_with_pet_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t userid, petid;

	if (unpkg_uint32_uint32 (body, bodylen, &userid, &petid) == -1)
		return -1;

	CHECK_VALID_ID (userid);
	DEBUG_LOG ("PLAY PET\t[%u %u %u]", p->id, userid, petid);
	return do_play_with_pet(p, userid, petid);
}

int set_pet_nick_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t petid;
	char nick[17];
	nick[16] = '\0';

	if (unpkg_pet_nick (body, bodylen, &petid, nick) == -1)
		return -1;

	CHECK_DIRTYWORD(p, nick);
	char* bad_char = "\n\v\t\f\r";
	if (strpbrk(nick, bad_char)) {
		return send_to_self_error(p, p->waitcmd, -ERR_dirty_word, 1);
	}

	DEBUG_LOG("SET NICK\t[%u %u]", p->id, petid);
	return do_set_pet_nick (p, petid, nick);
}

int set_pets_pos_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int count;
	pet_pos_t pos[4];

	if (unpkg_set_pet_pos(body, bodylen, &count, pos) == -1)
		return -1;

	p->sess_len = bodylen + sizeof(protocol_t);
	memcpy(p->session + sizeof(protocol_t), body, bodylen);

	DEBUG_LOG("SET PET_POS\t[%u %u]", p->id, count);
	return do_set_pets_pos(p, count, pos);
}

int trust_pets_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	static const int price[]    = { -120, -300, -600 };
	static const int duration[] = { 86400 * 3, 86400 * 7, 86400 * 14 };

	CHECK_BODY_LEN(bodylen, 9);

	uint8_t  cnt;
	uint32_t petid, trust_type;
	int i = 0;
	UNPKG_UINT8(body, cnt, i);
	UNPKG_UINT32(body, petid, i);
	UNPKG_UINT32(body, trust_type, i);

	CHECK_INT_LE(trust_type, 2);

	pet_t* pet = get_pet(p->id, petid);
	if (pet && PET_NATURAL(pet)) {
		trust_pet_sess_t* petsess = (void*)(p->session);
		petsess->pet      = pet;
		petsess->duration = duration[trust_type];
		p->sess_len       = sizeof *petsess;
		DEBUG_LOG("TRUST PET\t[uid=%u petid=%u type=%u]", p->id, petid, trust_type);

		/*拉姆托管统计*/
		uint32_t data[] = {p->id, 1};
		msglog(statistic_logfile, 0x202000b, now.tv_sec, data, sizeof(data));

		return db_attr_op(p, price[trust_type], 0, 0, 0, 0, ATTR_CHG_trust_pet, 0);
	}

	if (pet && PET_SICK(pet)) {
		return send_to_self_error(p, p->waitcmd, -ERR_lahm_is_sick, 1);
	}

	ERROR_RETURN(("pet %u status 0x%X", petid, pet ? pet->flag : 0), -1);
}

int withdraw_pets_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 5);

	int i = 0;
	uint8_t  cnt;
	uint32_t petid;

	UNPKG_UINT8(body, cnt, i);
	UNPKG_UINT32(body, petid, i);

	DEBUG_LOG("WITHDRAW PETS\t[uid=%u pid=%u]", p->id, petid);
	return do_withdraw_pet(p, petid);
}

int pick_pet_task_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 11);

	int i = 1;
	uint32_t petid, taskid;
	pick_pet_task_sess_t* tasksess = (void*)(p->session);

	UNPKG_UINT32(body, petid, i);
	UNPKG_UINT32(body, taskid, i);
	UNPKG_UINT16(body, tasksess->days, i);

	tasksess->task = get_task(taskid);
	tasksess->pet  = get_pet(p->id, petid);

	if (tasksess->pet && tasksess->task) {
		if ( ((tasksess->task->pet_is_vip && PET_IS_SUPER_LAHM(tasksess->pet))
				|| !tasksess->task->pet_is_vip)
			 && PET_NATURAL(tasksess->pet)) {
			p->sess_len = sizeof *tasksess;
			DEBUG_LOG("PICK PET TASK\t[uid=%u pid=%u task=%u days=%u]", p->id, petid, taskid, tasksess->days);
			return db_attr_op(p, (tasksess->task->coins_per_day * tasksess->days), 0, 0, 0, 0, ATTR_CHG_pet_task, 0);
		}
		if(PET_SICK(tasksess->pet)) {
			return send_to_self_error(p, p->waitcmd, -ERR_lahm_is_sick, 1);
		}
	}

	ERROR_RETURN(("pet %u status 0x%X task %u", petid, (tasksess->pet ? tasksess->pet->flag : 0), taskid), -1);
}

int pick_pet_magic_task_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 10);

	int i = 0;
	uint32_t petid, taskid;
	pick_pet_magic_task_sess_t* tasksess = (void*)(p->session);

	UNPKG_UINT32(body, petid, i);
	UNPKG_UINT32(body, taskid, i);
	UNPKG_UINT16(body, tasksess->stage, i);

	tasksess->pet  = get_pet(p->id, petid);
	tasksess->task = get_task(taskid);
	if (tasksess->pet && tasksess->task && (taskid > 100 && taskid < 120)) {
		if ((!PET_IS_SUPER_LAHM(tasksess->pet) && taskid == 101)
			|| (PET_IS_SUPER_LAHM(tasksess->pet) && taskid != 101)) {
			return send_to_self_error(p, p->waitcmd, -ERR_no_right_attend_magic, 1);
		}

		p->sess_len = sizeof *tasksess;
		DEBUG_LOG("PICK MAGIC TASK\t[uid=%u pid=%u task=%u stage=%u]", p->id, petid, taskid, tasksess->stage);
		return do_pick_magic_pet_task(p);
	}

	ERROR_RETURN(("pet %u status 0x%X task %u", petid, (tasksess->pet ? tasksess->pet->flag : 0), taskid), -1);
}

int fin_pet_magic_task_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 10);

	int i = 0;
	uint32_t petid, taskid, stage;

	uint32_t buff[3];
	UNPKG_UINT32(body, petid, i);  // pet id
	UNPKG_UINT32(body, taskid, i);  // task id
	UNPKG_UINT16(body, stage, i);  // stage

	*(uint32_t*)p->session = petid;
	*(uint32_t*)(p->session + 4) = taskid;
	*(uint32_t*)(p->session + 8) = stage;
	buff[0] = petid;
	buff[1] = taskid;
	buff[2] = stage;
	pet_t* pet  = get_pet(p->id, petid);
	if (pet && (taskid > 100 && taskid < 120) && stage < 4) {

		DEBUG_LOG("FIN MAGIC TASK\t[uid=%u pid=%u task=%u stage=%u]", p->id, petid, taskid, stage);
		return db_fin_magic_task(p, buff, p->id);
	}

	ERROR_RETURN(("pet %u task %u", petid, taskid), -1);
}

int cancel_pet_magic_task_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);

	int i = 0;
	uint32_t petid, taskid;

	uint32_t buff[2];
	UNPKG_UINT32(body, petid, i);  // pet id
	UNPKG_UINT32(body, taskid, i);  // task id

	*(uint32_t*)p->session = petid;
	buff[0] = petid;
	buff[1] = taskid;
	pet_t* pet  = get_pet(p->id, petid);
	if (pet && (taskid > 100 && taskid < 120)) {

		DEBUG_LOG("CANCEL MAGIC TASK\t[uid=%u pid=%u task=%u]", p->id, petid, taskid);
		return db_cancel_magic_task(p, buff, p->id);
	}

	ERROR_RETURN(("pet %u task %u", petid, taskid), -1);
}

int set_magic_task_data_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 12);

	int i = 0;
	uint32_t petid, taskid;

	uint32_t buff[3];
	UNPKG_UINT32(body, petid, i);  // pet id
	UNPKG_UINT32(body, taskid, i);  // task id
	UNPKG_UINT32(body, buff[2], i);  //value

	buff[0] = petid;
	buff[1] = taskid;
	pet_t* pet  = get_pet(p->id, petid);
	if (pet && (taskid > 100 && taskid < 120)) {

		DEBUG_LOG("SET MAGIC TASK DATA\t[uid=%u pid=%u task=%u value=%u]", p->id, petid, taskid, buff[2]);
		return db_set_magic_task_data(p, buff, p->id);
	}

	ERROR_RETURN(("pet %u task %u", petid, taskid), -1);
}

int get_magic_task_data_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);

	int i = 0;
	uint32_t petid, taskid;

	uint32_t buff[2];
	UNPKG_UINT32(body, petid, i);  // pet id
	UNPKG_UINT32(body, taskid, i);  // task id

	*(uint32_t*)p->session = buff[0] = petid;
	*(uint32_t*)(p->session + 4) = buff[1] = taskid;
	pet_t* pet  = get_pet(p->id, petid);
	if (pet && (taskid > 100 && taskid < 120)) {

		DEBUG_LOG("GET MAGIC TASK DATA\t[uid=%u pid=%u task=%u]", p->id, petid, taskid);
		return db_get_magic_task_data(p, buff, p->id);
	}

	ERROR_RETURN(("pet %u task %u", petid, taskid), -1);
}

int get_pet_magic_task_list_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 12);

	int i = 0;
	uint32_t buff[3];
	UNPKG_UINT32(body, buff[0], i);  //petid
	UNPKG_UINT32(body, buff[1], i);  // start id
	UNPKG_UINT32(body, buff[2], i);  // end id

	if (buff[1] > buff[2] || buff[1] < 101 || buff[1] > 104 || buff[2] < 101 || buff[2] > 104) {
		ERROR_RETURN(("bad start id or end id\t[%u %u %u %u]", p->id, buff[0], buff[1], buff[2]), -1);
	}

	DEBUG_LOG("GET PET MAGIC TASK LIST\t[%u %u %u %u]", p->id, buff[0], buff[1], buff[2]);
	return db_get_magic_task(p, buff);
}

int terminate_pet_task_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint8_t  cnt;
	uint32_t petid = 0;
	pet_t*   pet = NULL;
	if ( (unpkg_uint8_uint32(body, bodylen, &cnt, &petid) == 0)
			&& (pet = get_pet(p->id, petid)) && PET_ON_TASK(pet) ) {
		return terminate_pet_task(p, p->id, pet, 0);
	}

	ERROR_RETURN(("petid %u flag 0x%X", petid, (pet ? pet->flag : 0)), -1);
}

int set_pet_task_fin_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	const task_t*   task;
	task_elem_upd_t task_upd;
	pet_t*   pet = NULL;

	memset(&task_upd, 0, sizeof task_upd);
	if ( (unpkg_uint32_uint32(body + 1, bodylen - 1, &(task_upd.petid), &(task_upd.taskid)) == 0)
			&& (task = get_task(task_upd.taskid)) ) {
		pet = get_pet(p->id, task_upd.petid);
		if (pet) {

			task_upd.flag             = 1;
			task_upd.max_accu_tm      = task->task_time;
			task_upd.max_accu_tm_flag = 3;

			DEBUG_LOG("SET PET TASK FIN\t[uid=%u pid=%u task=%u]", p->id, task_upd.petid, task_upd.taskid);
			return db_set_task2(p, p->id, task_upd);
		}
	}

	return -1;
}

int get_pet_task_attire_info_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	pet_t *pet = p->followed;
	if (!pet)
		ERROR_RETURN(("no pet follow\t[%u]", p->id), -1);

	uint32_t pet_task_id;
	int i = 0;
	UNPKG_UINT32(body, pet_task_id, i);

	task_t *ptask = get_task(pet_task_id);
	if (!ptask)
		ERROR_RETURN(("error pet task\t[%u %u]", p->id, pet_task_id), -1);

//	if (ptask->pet_is_vip && !PET_IS_SUPER_LAHM(pet))
//		ERROR_RETURN(("error the task need your pet is super lahm \t[spriteid=%u taskid=%u petid=%u]", p->id, pet_task_id,pet->id), -1);

	*(uint32_t*)p->session = ptask->award_item_id;

	uint32_t buff[3] = {0};
	buff[0] = pet->id;
	buff[1] = pet_task_id;
	buff[2] = ptask->award_item_id;
	return send_request_to_db(SVR_PROTO_GET_PET_TASK_ATTIRE_INFO,p,12,buff,p->id);
}

int get_pet_task_attire_info_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	response_proto_uint32(p, p->waitcmd, *(uint32_t*)p->session, 0);

	return 0;
}

int get_pet_task_list_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 16);

	int i = 0;
	task_list_query_t query;
	UNPKG_UINT32(body, query.petid, i);
	UNPKG_UINT32(body, query.start_taskid, i);
	UNPKG_UINT32(body, query.end_taskid, i);
	UNPKG_UINT32(body, query.flag, i);
	//DEBUG_LOG("GET PET TASK LIST\t[%u %u]", p->id, p->waitcmd);
	p->sess_len = 0;
	PKG_H_UINT32(p->session, query.petid, p->sess_len);
	return db_get_task2_list(p, query);
}

//-----------------------------------------------------
// Super Lahm
//
/**
 * become_lahm_cmd - makes the pet that followed @p the 'super lahm'
 * @p: pointer of type sprite_t
 * @body: pkg from client
 * @bodylen: pkg len
 *
 * makes the pet that followed @p the 'super lahm',
 * response `uint8_t 1` if @p is a vip and none of
 * his/her pet is a super lahm, otherwise `uint8_t 0`.
 */
int become_super_lahm_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	int has = chk_has_super_lahm(p);
	if (p->followed && !has) {
		// chk if @p is a vip before making his/her pet the 'super lahm'
		return db_get_sprite(p, p->id);
	}

	ERROR_RETURN( ("cannot become super lahm: uid=%u pet=%p has=%d",
					p->id, p->followed, has), -1);
}

/**
 * has_super_lahm_cmd - chk if the given user @p has already had a super lahm
 * @p: pointer of type sprite_t
 * @body: pkg from client
 * @bodylen: pkg len
 *
 * chk if the given user @p has already had a super lahm,
 * response `uint8_t 1` if has, otherwise `uint8_t 0`.
 */
int has_super_lahm_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	int has = chk_has_super_lahm(p);
	//DEBUG_LOG("HAS SUPER LAHM\t[uid=%u has=%d]", p->id, has);
	if (has != -1) {
		response_proto_uint8(p, p->waitcmd, has, 1);
		return 0;
	}

	return -1;
}

int lahm_housework_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t petid, itemid;
	if (unpkg_uint32_uint32(body, bodylen, &petid, &itemid) == -1)
		return -1;

	DEBUG_LOG ("LAHM HOUSEWORK\t[%u %u %u]", p->id, petid, itemid);

	item_kind_t* kind = find_kind_of_item(itemid);
	item_t*      itm  = get_item_prop(itemid);
	pet_t*       pet;

	if ( !itm || !kind || (kind->kind != FEED_ITEM_KIND) ) {
		ERROR_RETURN(("error itemid=%u, id=%u", itemid, p->id), -1);
	}
	if ( !(pet = get_pet(p->id, petid)) ) {
		ERROR_RETURN(("no pet, petid=%u, id=%u", petid, p->id), -1);
	}


	calc_pet_attr(p->id, pet, itm->u.feed_attr.hungry,
			itm->u.feed_attr.thirsty, itm->u.feed_attr.sanitary, itm->u.feed_attr.spirit);
	db_clean_mud(0, NULL, 0, p->id);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, itm->id, i);
	PKG_UINT8(msg, itm->u.feed_attr.type, i);
	PKG_UINT32(msg, pet->flag, i);
	PKG_UINT32(msg, pet->id, i);
	PKG_UINT8(msg, pet->hungry, i);
	PKG_UINT8(msg, pet->thirsty, i);
	PKG_UINT8(msg, pet->sanitary, i);
	PKG_UINT8(msg, pet->spirit, i);
	init_proto_head (msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);
	return 0;
}

/**
 * modify_pet_color_cmd - modify the color of pet that followed @p
 * @p: pointer of type sprite_t
 * @body: pkg from client
 * @bodylen: pkg len
 */
int discolor_pill_modify_pet_color_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 12);

	item_kind_t *ik;
	pet_t* pet;
	uint32_t color, itemid, petid;
	int j = 0;

	UNPKG_UINT32(body, petid, j);
	UNPKG_UINT32(body, itemid, j);
	UNPKG_UINT32(body, color, j);

	pet = get_pet(p->id, petid);
	if ( !pet )
		ERROR_RETURN (("pet id %u is not valid", petid), -1);
	if (!PET_IS_SUPER_LAHM(pet))
		ERROR_RETURN (("pet id %u is not super lahm", petid), -1);
	*(uint32_t*)p->session = petid;
	*(uint32_t*)(p->session + 4) = color;
	*(uint32_t*)(p->session + 8) = itemid;

	ik = find_kind_of_item (itemid);
	if (!ik || ik->kind != FEED_ITEM_KIND)
		ERROR_RETURN (("error item=%u, color=%u", itemid, color), -1);

	DEBUG_LOG ("SET PET COLOR\t[uid=%u petid=%u color=%u itemid=%u]", p->id, petid, color, itemid);
	return db_single_item_op(p, p->id, itemid, 1, 0);
}

int set_pet_arg_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(pet_t) - sizeof(pet_suppl_info_t));

	uint32_t petid = *(uint32_t*)buf;
	pet_t* pet = get_pet(id, petid);
	if (pet == NULL) {
		ERROR_RETURN(("PET IS NULL %u", p->id), 0);
	}
	memcpy(pet, buf, len);

	switch (p->waitcmd) {
	case PROTO_SET_PET_NICK:
		response_proto_head(p, p->waitcmd, 0);
		break;
	case PROTO_MODIFY_PET_COLOR:
	{
		uint32_t petid = *(uint32_t*)p->session;
		uint32_t color = *(uint32_t*)(p->session + 4);
		uint32_t itemid = *(uint32_t*)(p->session + 8);
		pet_t* ppet = get_pet(p->id, petid);
		if (!ppet)
			ERROR_RETURN(("bad petid %u", petid), -1);

		item_t* pitem = get_item_prop(itemid);
		if (!pitem)
			ERROR_RETURN(("bad itemid %u", itemid), -1);

		int j = sizeof(protocol_t);
		PKG_UINT32(msg, itemid, j);
		PKG_UINT32(msg, pitem->u.feed_attr.type, j);
		PKG_UINT32(msg, ppet->flag, j);
		PKG_UINT32(msg, petid, j);
		PKG_UINT32(msg, color, j);
		init_proto_head(msg, p->waitcmd, j);
		send_to_map(p, msg, j, 1);
		break;
	}

	case PROTO_FEED_ITEM_USE:
	{
		assert (p->sess_len == 16);
		item_t* itm = *(item_t **)p->session;
		if (itm->u.feed_attr.type != 2) {
			return db_single_item_op(p, p->id, itm->id, 1, 0);
		} else {
			return feed_pet_callback(p);
		}
	}
	case PROTO_FOLLOW_PET:
		return response_follow_pet(p, p->followed);
	case PROTO_BECOME_SUPER_LAHM:
#ifdef TW_VER
		send_postcard("克勞神父", 0, p->id, 1000026, "恭喜！你的拉姆已經變成超級拉姆了！超級拉姆是莊園的守護精靈。你將有更大的責任做一名樂於助人的小摩爾。", 0);
#else
		send_postcard("克劳神父", 0, p->id, 1000026, "恭喜！你的拉姆已经变成超级拉姆了！超级拉姆是庄园的守护精灵。你将有更大的责任做一名乐于助人的小摩尔。", 0);
#endif
		DEBUG_LOG("BECOME SUPER LAHM\t[uid=%u pid=%u]", p->id, p->followed->id);
		// become 'super lahm' successfully, response `uint8_t 1`
		item_t* pitm = get_item_prop(p->followed->suppl_info.pet_clothes[0]);
		if (pitm && IS_NOMAL_LAHM_ITM(pitm)) {
			pet_unwear_cloth(p, pet, pitm->id);
		}
		response_proto_uint8(p, p->waitcmd, 1, 0);
		return 0;
	case PROTO_PET_TRUST:
		p->followed = 0;
		response_proto_uint32(p, p->waitcmd, p->id, 1);
		db_inc_trusted_pet_cnt();
		break;
	case PROTO_PICK_PET_TASK:
	{
		//
		pick_pet_task_sess_t* tasksess = (void*)(p->session);
		const pet_t* pet = tasksess->pet;
		p->followed = 0;
		response_proto_uint32(p, p->waitcmd, p->id, 1);
		//
		char msgbuf[256];
#ifdef TW_VER
		snprintf(msgbuf, sizeof msgbuf,
					"　　親愛的%.16s，你為拉姆報了%d天的%s。%d天後，我就會安排它回家了。",
					p->nick, tasksess->days, tasksess->task->name, tasksess->days);
#else
		snprintf(msgbuf, sizeof msgbuf,
					"　　亲爱的%.16s，你为拉姆报了%d天的%s。%d天后，我就会安排它回家了。",
					p->nick, tasksess->days, tasksess->task->name, tasksess->days);
#endif
		send_postcard("菩提大伯", 0, p->id, 1000022, msgbuf, 0);
		//
		db_inc_ontask_pet_cnt();
		//
		task_elem_upd_t task;
		task.petid			  = pet->id;
		task.taskid 		  = pet->taskid;
		task.flag			  = 2;
		task.add_tm 		  = 0;
		task.start_tm		  = pet->start_tm;
		task.end_tm 		  = pet->trust_end_tm;
		task.max_accu_tm	  = tasksess->task->task_time;
		task.max_accu_tm_flag = 2;
		db_set_task2(0, p->id, task);
		//
		send_attr_update_noti(0, p, p->id, 3);
		break;
	}
	case PROTO_PET_WITHDRAW:
		do_exchange_item(p, 1601, 0);
		db_dec_trusted_pet_cnt();
		break;
	default:
		ERROR_RETURN(("bad msg\t[%u]", p->id), -1);
	}
	return 0;
}

int db_set_flag_endtime(sprite_t* p, uint32_t flag, uint32_t endtime, uint32_t petid, userid_t uid)
{
	uint32_t buff[3];
	buff[0] = petid;
	buff[1] = flag;
	buff[2] = endtime;
	return send_request_to_db(SVR_PROTO_SET_FLAG_ENDTIME, p, 12, buff, uid);
}

int db_update_pet_attr(sprite_t* p, uint32_t stamp, uint32_t hungry, uint32_t thirsty, uint32_t sanitary, uint32_t spirit, uint32_t petid, userid_t uid)
{
	uint32_t buff[6];
	buff[0] = petid;
	buff[1] = hungry;
	buff[2] = thirsty;
	buff[3] = sanitary;
	buff[4] = spirit;
	buff[5] = stamp;
	return send_request_to_db(SVR_PROTO_UPDATE_PET_ATTR, p, 24, buff, uid);
}

int db_feed_pet(sprite_t* p, uint32_t flag, uint32_t sicktime, uint32_t stamp, uint32_t hungry, uint32_t thirsty, uint32_t sanitary, uint32_t spirit, uint32_t petid, userid_t uid)
{
	uint32_t buff[9];
	buff[0] = petid;
	buff[1] = flag;
	buff[2] = sicktime;
	buff[3] = stamp;
	buff[4] = hungry;
	buff[5] = thirsty;
	buff[6] = sanitary;
	buff[7] = spirit;
	buff[8] = p->id;
	return send_request_to_db(SVR_PROTO_FEED_PET, p, 36, buff, uid);
}

int db_trust_pet(sprite_t* p, uint32_t flag, uint32_t endtime, uint32_t stamp, uint32_t hungry, uint32_t thirsty, uint32_t sanitary, uint32_t spirit, uint32_t petid, userid_t uid)
{
	uint32_t buff[8];
	buff[0] = petid;
	buff[1] = flag;
	buff[2] = endtime;
	buff[3] = stamp;
	buff[4] = hungry;
	buff[5] = thirsty;
	buff[6] = sanitary;
	buff[7] = spirit;
	return send_request_to_db(SVR_PROTO_TRUST_PET, p, 32, buff, uid);
}

int db_pet_task(sprite_t* p, uint32_t flag, uint32_t endtime, uint32_t starttime, uint32_t taskid, uint32_t stamp, uint32_t hungry, uint32_t thirsty, uint32_t sanitary, uint32_t spirit, uint32_t petid, userid_t uid)
{
	uint32_t buff[10];
	buff[0] = petid;
	buff[1] = flag;
	buff[2] = endtime;
	buff[3] = starttime;
	buff[4] = taskid;
	buff[5] = stamp;
	buff[6] = hungry;
	buff[7] = thirsty;
	buff[8] = sanitary;
	buff[9] = spirit;
	return send_request_to_db(SVR_PROTO_PET_TASK, p, 40, buff, uid);
}

int db_set_pet_arg(sprite_t* p, uint8_t type, uint32_t arg, uint32_t petid, userid_t uid)
{
	uint32_t buff[2];
	buff[0] = petid;
	buff[1] = arg;
	switch (type) {
	case  PET_FLAG:
		return send_request_to_db(SVR_PROTO_SET_PET_FLAG, p, 8, buff, uid);
	case  PET_FLAG2:
		return send_request_to_db(SVR_PROTO_SET_PET_FLAG2, p, 8, buff, uid);
	case  PET_COLOR:
		return send_request_to_db(SVR_PROTO_SET_PET_COLOR, p, 8, buff, uid);
	case  PET_SICKTIME:
		return send_request_to_db(SVR_PROTO_SET_PET_SICKTIME, p, 8, buff, uid);
	case  PET_ENDTIME:
		return send_request_to_db(SVR_PROTO_SET_PET_END_TIME, p, 8, buff, uid);
	case  PET_STAMP:
		return send_request_to_db(SVR_PROTO_SET_PET_FLAG, p, 8, buff, uid);
	case  PET_SKILL_FLAG:
		if(p) memcpy(p->session, buff, 8);
		DEBUG_LOG("SET PET ARG PETID=%u, ARG=%u",buff[0],buff[1]);
		return send_request_to_db(SVR_PROTO_SET_PET_SKILL_TYPE, p, 8, buff, uid);
	default:
		ERROR_RETURN(("set pet arg error\t[id=%u uid=%u type=%u]", uid, uid, type), -1);
	}
}

int get_one_pet_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, sizeof(pet_t) - sizeof(pet_suppl_info_t) + 4);

	pet_t pet;
	memcpy(&pet, buf, sizeof(pet_t) - sizeof(pet_suppl_info_t));
	memset(&(pet.suppl_info), 0, sizeof(pet_suppl_info_t));
	pet.suppl_info.uid = id;
	int count, i = sizeof(pet_t) - sizeof(pet_suppl_info_t);
	UNPKG_H_UINT32(buf, count, i);
	CHECK_BODY_LEN(len, sizeof(pet_t) - sizeof(pet_suppl_info_t) + 4 + count * 8);

	int loop;
	uint32_t itmid;
	for (loop = 0; loop < count; loop++) {
		UNPKG_H_UINT32(buf, itmid, i);
		i += 4;
		DEBUG_LOG("ONE PET INFO\t[%u %u %u %u]", p->id, id, pet.id, itmid);
		item_kind_t* kind = find_kind_of_item(itmid);
		if (kind && kind->kind == PET_ITEM_CLOTH_KIND) {
			pet.suppl_info.pet_clothes[0] = itmid;
		}
		if (kind && kind->kind == PET_ITEM_HONOR_KIND) {
			pet.suppl_info.pet_honor[0] = itmid;
		}
	}

	return response_proto_get_pets(p, &pet, id, 1, 0, 0);
}

int db_infect_pet(sprite_t *p, pet_t *pet, userid_t id)
{
	uint32_t buff[2];
	buff[0] = pet->id;
	buff[1] = pet->sick_type;

	return send_request_to_db(SVR_PROTO_SET_PET_SICKTYPE , p, 8, buff, id);
}

int set_pet_magic_task_callback(sprite_t* p, uint32_t id, char* buf, int len)
{

	CHECK_BODY_LEN(len, 8);

	response_proto_uint32(p, p->waitcmd, p->id, 1);
	return 0;
}

int fin_pet_magic_task_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	static uint32_t clothes[4] = 	{1200025, 1200023, 1200024, 1200022};
	static uint32_t honor[4] = 		{1210016, 1210014, 1210013, 1210015};

	CHECK_BODY_LEN(len, 4);
	uint32_t petid = *(uint32_t*)p->session;
	uint32_t taskid = *(uint32_t*)(p->session + 4);
	uint32_t stage = *(uint32_t*)(p->session + 8);
	pet_t* pet = get_pet(p->id, petid);
	if (stage == 3 && pet && (taskid >= 101 && taskid <= 104)) {
		uint32_t buff[5];
		buff[0] = petid;
		buff[1] = clothes[taskid - 101];
		buff[2] = 1;
		buff[3] = 1;
		buff[4] = 0;

		DEBUG_LOG("MAGIC TASK FINISH\t[%u %u %u]", p->id, petid, taskid);
		send_request_to_db(SVR_PROTO_BUY_PET_ITEM, 0, 20, buff, p->id);
		buff[1] = honor[taskid - 101];
		send_request_to_db(SVR_PROTO_BUY_PET_ITEM, 0, 20, buff, p->id);
	}

	response_proto_uint32(p, p->waitcmd, *(uint32_t*)buf, 0);  // 1, some pet is learning; 0 no pet learning
	return 0;
}

int cancel_pet_magic_task_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	response_proto_uint32(p, p->waitcmd, *(uint32_t*)buf, 0);
	return 0;
}

int set_magic_task_data_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int get_magic_task_data_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t tmp_data = *(uint32_t*)buf;
	uint32_t petid = *(uint32_t*)p->session;
	uint32_t taskid = *(uint32_t*)(p->session + 4);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, petid, j);
	PKG_UINT32(msg, taskid, j);
	PKG_UINT32(msg, tmp_data, j);

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int get_pet_magic_task_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
typedef struct magic_task_info {
	uint32_t		petid;
	uint32_t		taskid;
	uint32_t		done_time;
	uint32_t		stage;
	uint32_t		client_data;
} __attribute__((packed)) magic_task_info_t;

	CHECK_BODY_LEN_GE(len, 4);

	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + count * sizeof(magic_task_info_t));

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, count, j);
	int loop;
	for (loop = 0; loop < count; loop++) {
		magic_task_info_t* mti = (magic_task_info_t*)(buf + 4 + loop * sizeof(magic_task_info_t));
		pet_t* pet = get_pet(p->id, mti->petid);
		if (!pet)
			continue;
		PKG_UINT32(msg, pet->id, j);
		PKG_STR(msg, pet->nick, j, PET_NICK_LEN);
		PKG_UINT32(msg, pet->color, j);
		PKG_UINT8(msg, get_pet_level(pet), j);
		PKG_UINT32(msg, mti->taskid, j);
		PKG_UINT32(msg, mti->stage, j);
		PKG_UINT32(msg, mti->client_data, j);
	}

	DEBUG_LOG("GET MAGIC TASK CALLBACK\t[%u %u]", p->id, count);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int call_super_lahm_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);

	pet_t* pet = NULL;
	map_t* map = get_map(p->id);
	if (map && map->pet_loaded) {
	    int i;
		for (i = 0; i != map->pet_cnt; ++i) {
			if (PET_IS_SUPER_LAHM(&(map->pets[i]))){
				pet = &(map->pets[i]);
				if (PET_TRUSTED(pet)) {
					return send_to_self_error(p, p->waitcmd, -ERR_super_lahm_on_trust, 1);
				} else {
					if(PET_ON_TASK(pet)) {
						return send_to_self_error(p, p->waitcmd, -ERR_super_lahm_on_task, 1);
					}
					if(PET_EMPLOYED(pet)) {
						return send_to_self_error(p, p->waitcmd, -ERR_lahm_is_employed, 1);
					}
				}
				break;
			}
		}
	}
	if (pet) {

	    uint32_t msgbuff[2] = {p->id, 1};
	    msglog(statistic_logfile, 0x0408B3CC, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

		return do_follow_pet(p, pet->id, 1);
	}

	return send_to_self_error(p, p->waitcmd, -ERR_no_super_lahm, 1);
}

/* @brief add lamu growth by xunfuxing or yupiao
 */
int add_lamu_growth_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 4);

	if (p->followed == NULL) {
		ERROR_RETURN(("not followed pet %u", p->id), -1);
	}

	/*save petid, itemid, count, hour*/
	uint32_t db_buf[4] = { };
	uint32_t j = 0;

	db_buf[0] = p->followed->id;
	UNPKG_UINT32(body, db_buf[1], j);

	if (db_buf[1] == 190594) {
		return db_set_sth_done(p, 117, 1, p->id);
	}

	/*save item count and hour*/
	if (db_buf[1] == 190592) {
		*(uint32_t *)p->session = 24;
		db_buf[2] = 20;
		db_buf[3] = 24 * 3600;
	} else if (db_buf[1] == 190594) {
		*(uint32_t *)p->session = 24 * 10;
		db_buf[2] = 1;
		db_buf[3] = 24 * 10 * 3600;
	} else {
		ERROR_RETURN(("wrong add type %u", db_buf[0]), -1);
	}
	/*if pet level gt 4, add 0 hour*/
	if (p->followed == NULL) {
		ERROR_RETURN(("not followed pet %u", p->id), -1);
	}
	int level = get_pet_level(p->followed);
	if (level >= 4) {
		DEBUG_LOG("you pet greater 4 %u %u", p->id, p->followed->id);
		db_buf[3] = 0;
	}
	return send_request_to_db(SVR_PROTO_ADD_LAMU_GROWTH, p, 16, db_buf, p->id);
}

int add_lamu_growth_use_tick_callback(sprite_t *p, uint32_t id, char *buf, int len)
{

	if (p->followed == NULL) {
		ERROR_RETURN(("not followed pet %u", p->id), -1);
	}

	/*save petid, itemid, count, hour*/
	uint32_t db_buf[4] = { };

	db_buf[0] = p->followed->id;
	db_buf[1] = 190594;

	/*save item count and hour*/
	*(uint32_t *)p->session = 24 * 10;
	db_buf[2] = 1;
	db_buf[3] = 24 * 10 * 3600;
	int level = get_pet_level(p->followed);
	if (level >= 4) {
		DEBUG_LOG("you pet greater 4 %u %u", p->id, p->followed->id);
		db_buf[3] = 0;
	}
	return send_request_to_db(SVR_PROTO_ADD_LAMU_GROWTH, p, 16, db_buf, p->id);
}


/* @brief the callback for add lamu growth
 */
int add_lamu_growth_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t add_hour = *(uint32_t *)p->session;
	/*update online data*/
	if (p->followed == NULL) {
		ERROR_RETURN(("not followed pet %u", p->id), -1);
	}
	p->followed->logic_birthday -= add_hour * 3600;

	uint32_t level = get_pet_level(p->followed);
	response_proto_uint32(p, p->waitcmd, level, 0);
	return 0;
}

int set_pet_skill_type_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int i = 0;
	uint32_t buff[2] = {0};
	CHECK_BODY_LEN(bodylen,8);

	UNPKG_UINT32(body, buff[0], i);//petid
	UNPKG_UINT32(body, buff[1], i);//skill type

	if (!p->followed || p->followed->id != buff[0]) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_lahm_not_followed, 1);
	}

	if (p->followed->skill_ex[0] == 0 ||
		p->followed->skill_ex[1] == 0 ||
		p->followed->skill_ex[2] == 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_study_basic_skills, 1);
	}

	int pet_level = get_pet_level(p->followed);

	DEBUG_LOG("SET PET SKILL TYPE %u %u %d %d ",p->id, p->followed->id, pet_level, buff[1]);

	if (pet_level < 4) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_lahm_level_too_low, 1);
	} else if(PET_IS_SUPER_LAHM(p->followed)) {
		if(buff[1] != 7) {
			return send_to_self_error(p, p->waitcmd, -ERR_client_not_proc, 1);
		}
	} else {
		if((p->followed->skill_bits != 0) || (buff[1] != 1 && buff[1] != 2 && buff[1] != 4)) {
			return send_to_self_error(p, p->waitcmd, -ERR_client_not_proc, 1);
		}
	}

	*(uint32_t*)p->session = buff[0];
	*(uint32_t*)(p->session + 4) = buff[1];
	db_set_lahm_task_state(NULL, p->id, p->followed->id, 1, 2);

	return send_request_to_db(SVR_PROTO_GET_TEMPLE, p, 0, NULL, p->id);
}

int set_pet_skill_type_callback(sprite_t* p, uint32_t id, char* buf, int len)
{

    typedef struct five_stage_msg
    {
        uint32_t tm;
        char     nick[USER_NICK_LEN];
        char	 m_nick[USER_NICK_LEN];
        uint32_t skill_bits;
    }__attribute__((packed)) five_stage_msg_t;
    five_stage_msg_t msglog = {};
    msglog.tm = time(NULL);

    if(p->followed)
    {
        memcpy(msglog.nick,p->followed->nick,USER_NICK_LEN);
     }
     memcpy(msglog.m_nick,p->nick,USER_NICK_LEN);
     msglog.skill_bits = *(uint32_t*)(p->session + 4);
    send_msglog_to_bus(LAMU_GET_FIVE_STAGE_CMD,sizeof(msglog),&msglog,p->id);

	p->followed->skill_bits = *(uint32_t*)(p->session + 4);
	response_proto_uint32(p, p->waitcmd, p->followed->skill_bits, 0);
	return 0;
}

int set_pet_skill_ex_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int i=0;
	uint32_t buff[4] = {0,0,0,1};

	CHECK_BODY_LEN(bodylen,12);

	UNPKG_UINT32(body, buff[0], i);//petid
	UNPKG_UINT32(body, buff[1], i);//skill type 1 fire,2 water,3 wood
	UNPKG_UINT32(body, buff[2], i);//skill level 0:basic,1 --- 5:level1 --- level5;

	buff[1] -= 1;//client use 1,2,3,so sub 1 here

	if (buff[1] > 2) { //now fire & wood & water skill opened
		ERROR_LOG("id=%u pet %u skill Type = %d", p->id, buff[0], buff[1]);
		return send_to_self_error(p, p->waitcmd, -ERR_client_not_proc, 1);
	}

	if (!p->followed || p->followed->id != buff[0]) {
		ERROR_LOG("id=%u pet %u not Follow", p->id, buff[0]);
		return send_to_self_error(p, p->waitcmd, -ERR_the_lahm_not_followed, 1);
	}

	DEBUG_LOG("SET PET SKILL EX %u %u %u %u %u",p->id, p->followed->id, buff[0], buff[1],buff[2]);

	int pet_level = get_pet_level(p->followed);
	if (pet_level < 2) {
		ERROR_LOG("id=%u pet %u Level = %d < 2", p->id, buff[0], pet_level);
		return send_to_self_error(p, p->waitcmd, -ERR_the_lahm_level_too_low, 1);
	}

	uint32_t skill = p->followed->skill_ex[buff[1]];
	if(buff[2] == 0) {
		if (skill) {
			response_proto_uint32_uint32(p, p->waitcmd, buff[1] + 1, skill, 0);
			return 0;
		}
	} else {
		if((p->followed->skill_bits & (1 << buff[1])) == 0) {
			response_proto_uint32_uint32(p, p->waitcmd, buff[1] + 1, skill, 0);
			return 0;
		}
		struct skill_item *skitm = get_lahm_skill(buff[1], buff[2]);
		if(!skitm || skitm->need_change > p->followed->skill_value) {
			response_proto_uint32_uint32(p, p->waitcmd, buff[1] + 1, skill, 0);
			return 0;
		}
	}

	p->sess_len = 8;
	memcpy(p->session, (buff + 1), 8);
	DEBUG_LOG("id=%u pet=%u type=%x skill=%x",p->id, buff[0], buff[1],buff[2]);
	return send_request_to_db(SVR_PROTO_SET_PET_SKILL_EX, p, 16, buff, p->id);
}

int set_pet_skill_ex_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t type = 0, bit = 0, skill =0;

	type = *(uint32_t*)p->session;
	bit = *(uint32_t*)(p->session + 4);

	if (p->followed && type < 3 && bit < 32) {
		uint32_t skill_item[] = {190593,190628,190595};
		p->followed->skill_ex[type] |= (1 << bit);
		skill = p->followed->skill_ex[type];
		db_single_item_op(0, p->id, skill_item[type], 1, 0);
	}

	response_proto_uint32_uint32(p, p->waitcmd, type + 1, skill, 0);

	if( (p->followed->skill_ex[0] == 1) &&
		(p->followed->skill_ex[1] == 1) &&
		(p->followed->skill_ex[2] == 1)) {

#ifndef TW_VER
		send_postcard("公民管理处",0,p->id,1000137,"你的拉姆刚刚学会了水、火、木技能，可以去藏宝神殿内进化成神力拉姆了哦！",0);
#else
		send_postcard("公民管理處",0,p->id,1000137,"你的拉姆剛剛學會了水、火、木技能，可以去藏寶神殿內進化成神力拉姆了哦！",0);
#endif
	}

	return 0;
}

int db_set_pet_hot_skill(sprite_t *p, uint32_t petid, uint32_t hot1, uint32_t hot2, uint32_t hot3, uint32_t id)
{
	uint32_t local_buff[4] = {petid, hot1, hot2, hot3};
	return send_request_to_db(SVR_PROTO_SET_PET_HOT_SKILL, p, 16, local_buff, id);
}

int set_pet_hot_skill_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen,7);

	int i=0;
	uint32_t petid;
	uint8_t	 hotsk[3];

	UNPKG_UINT32(body, petid, i);//petid
	UNPKG_UINT8(body,hotsk[0],i);
	UNPKG_UINT8(body,hotsk[1],i);
	UNPKG_UINT8(body,hotsk[2],i);

	DEBUG_LOG("SET HOT SKILL id=%u pet=%u skill=%d %d %d",p->id, petid, hotsk[0], hotsk[1], hotsk[2]);

	if (hotsk[0] > 18 || hotsk[1] > 18 || hotsk[2] > 18) {
		ERROR_LOG("id=%u pet %u skill Type = %d %d %d", p->id, petid, hotsk[0], hotsk[1], hotsk[2]);
		return send_to_self_error(p, p->waitcmd, -ERR_client_not_proc, 1);
	}

	for(i=0; i < 3; i++) {
		if(hotsk[i] && hotsk[i] == hotsk[(i+1) % 3]) {
			ERROR_LOG("id=%u pet=%u hot skill[%d] = skill[%d] = %d", p->id, petid, i, (i+1) % 3, hotsk[i]);
			return send_to_self_error(p, p->waitcmd, -ERR_client_not_proc, 1);
		}
	}

	if (!p->followed || p->followed->id != petid) {
		ERROR_LOG("id=%u pet %u not Follow", p->id, petid);
		return send_to_self_error(p, p->waitcmd, -ERR_the_lahm_not_followed, 1);
	}

	for(i=0; i < 3; i++) {
		if(hotsk[i]) {
			int type = (hotsk[i] - 1) % 3;
			int bitx = (hotsk[i] - 1) / 3;
			if(bitx > 31 || (p->followed->skill_ex[type] & (1 << bitx)) == 0) {
				hotsk[i] = 0;
			}
		}
	}

	DEBUG_LOG("BIND HOT SKILL id=%u pet=%u skill=%d %d %d",p->id, petid, hotsk[0], hotsk[1], hotsk[2]);

	p->sess_len = 0;
	PKG_H_UINT32(p->session, petid,    p->sess_len);
	PKG_H_UINT32(p->session, hotsk[0], p->sess_len);
	PKG_H_UINT32(p->session, hotsk[1], p->sess_len);
	PKG_H_UINT32(p->session, hotsk[2], p->sess_len);
	return send_request_to_db(SVR_PROTO_SET_PET_HOT_SKILL, p, p->sess_len, p->session, p->id);
}

int set_pet_hot_skill_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint8_t buff[3]={0};

	p->followed->skill_hot[0] = buff[0] = *(uint32_t*)(p->session + 4);
	p->followed->skill_hot[1] = buff[1] = *(uint32_t*)(p->session + 8);
	p->followed->skill_hot[2] = buff[2] = *(uint32_t*)(p->session + 12);

	memset(p->session, 0, p->sess_len);
	p->sess_len = 0;

	response_proto_str(p, p->waitcmd, 3, buff, 0);
	return 0;
}

/* @brief get pet task stat
 */
int get_lahm_task_state_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int i = 0;
	uint32_t buff[2] = {0};

	CHECK_BODY_LEN(bodylen,8);

	UNPKG_UINT32(body, buff[0], i);//petid
	UNPKG_UINT32(body, buff[1], i);//taskid

	if (!get_pet(p->id, buff[0])) {
		ERROR_RETURN(("uid=%u pet=%u task=%u pet not exsit", p->id, buff[0], buff[1]),-1);
	}

	if (!get_lahm_task(buff[1])) {
		ERROR_RETURN(("uid=%u pet=%u task=%u task not exsit", p->id, buff[0], buff[1]),-1);
	}

	return send_request_to_db(SVR_PROTO_GET_LAHM_TASK_STATE_EX, p, 8, buff, p->id);
}

/* @brief get pet task stat callback
 */
int get_lahm_task_state_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len,4);

	response_proto_uint32(p, p->waitcmd, *((uint32_t*)buf), 0);
	return 	0;
}

int db_set_lahm_task_state(sprite_t *p, uint32_t id, uint32_t petid, uint32_t taskid, uint32_t state)
{
	uint32_t buff[12];
	buff[0] = petid;
	buff[1] = 1;
	buff[2] = 2;
	return send_request_to_db(SVR_PROTO_SET_LAHM_TASK_STATE_EX, 0, 12, buff, id);
}

/* @brief set pet task stat
 */
int set_lahm_task_state_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int i = 0;
	uint32_t buff[3] = {0};

	CHECK_BODY_LEN(bodylen,12);

	UNPKG_UINT32(body, buff[0], i);//petid
	UNPKG_UINT32(body, buff[1], i);//taskid
	UNPKG_UINT32(body, buff[2], i);//state

	if (buff[2] == 0 || buff[2] > 2) {
		ERROR_LOG("uid=%u pet=%u task=%u state=%u illegal state", p->id, buff[0], buff[1],buff[2]);
		return send_to_self_error(p, p->waitcmd, -ERR_want_set_task_state_0, 1);
	}

	if (!get_pet(p->id, buff[0])) {
		ERROR_RETURN(("uid=%u pet=%u task=%u pet not exsit", p->id, buff[0], buff[1]),-1);
	}

	if (!get_lahm_task(buff[1])) {
		ERROR_RETURN(("uid=%u pet=%u task=%u task not exsit", p->id, buff[0], buff[1]),-1);
	}

	DEBUG_LOG("LAHM TASK SET uid=%u pet=%u task=%u,state=%u", p->id, buff[0], buff[1], buff[2]);

	return send_request_to_db(SVR_PROTO_SET_LAHM_TASK_STATE_EX, p, 12, buff, p->id);
}

/* @breif set pet task stat callback
 */
int set_lahm_task_state_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	DEBUG_LOG("LAHM TASK RETURN uid=%u len=%d state=%u", p->id, len, *(uint32_t*)buf);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/* @brief set client data
 */
int set_lahm_task_data_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int i = 0;
	uint32_t buff[16] = {0};
	/*petid, taskid, data 50 bytes*/
	CHECK_BODY_LEN(bodylen,58);

	UNPKG_UINT32(body, buff[0], i);//petid
	UNPKG_UINT32(body, buff[1], i);//taskid

	if (!get_pet(p->id, buff[0])) {
		ERROR_RETURN(("uid=%u pet=%u task=%u pet not exsit", p->id, buff[0], buff[1]),-1);
	}

	if (!get_lahm_task(buff[1])) {
		ERROR_RETURN(("uid=%u pet=%u task=%u task not exsit", p->id, buff[0], buff[1]),-1);
	}

	memcpy((buff + 2), body + 8, 50);//data

	return send_request_to_db(SVR_PROTO_SET_LAHM_TASK_DATA_EX, p, 58, buff, p->id);
}

/* @breif set data callback
 */
int set_lahm_task_data_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/* @brief get pet task client data
 */
int get_lahm_task_data_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int i = 0;
	uint32_t buff[2] = {0};
	/*save petid, taskid*/
	CHECK_BODY_LEN(bodylen,8);

	UNPKG_UINT32(body, buff[0], i);//petid
	UNPKG_UINT32(body, buff[1], i);//taskid

	if (!get_pet(p->id, buff[0])) {
		ERROR_RETURN(("uid=%u pet=%u task=%u pet not exsit", p->id, buff[0], buff[1]),-1);
	}

	if (!get_lahm_task(buff[1])) {
		ERROR_RETURN(("uid=%u pet=%u task=%u task not exsit", p->id, buff[0], buff[1]),-1);
	}

	return send_request_to_db(SVR_PROTO_GET_LAHM_TASK_DATA_EX, p, 8, buff, p->id);
}

/* @brief client data
 */
int get_lahm_task_data_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 50);
	response_proto_str(p, p->waitcmd, len, buf,0);
	return 0;
}

/* @brief sub material, add temple height
 */
int build_temple_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int i = 0;
	uint32_t buff[2] = {0,0};

	CHECK_BODY_LEN(bodylen,8);
	/*save temple type, add height*/
	UNPKG_UINT32(body, buff[0], i);
	/*always 1*/
	UNPKG_UINT32(body, buff[1], i);
	/*this week just fire temple, should change next week*/
	if (buff[0] > 2) {
		ERROR_RETURN(("temple type error %u", buff[0]), -1);
	}

	if (buff[1] != 1) {
		ERROR_LOG("temple height %u != 1, set to 1 and ignore this error", buff[1]);
		buff[1] = 1;
	}

	DEBUG_LOG("temple %u %u", buff[0], buff[1]);
	p->sess_len = 8;
	memcpy(p->session, buff, 8);
//state need confirm
	int db_len[] = {13 * 4, 13 * 4, 13 * 4};
	uint32_t db_buf[3][16] = {
			{3,0,0,0, 0,190590,4, 0,190591,4, 0,190592,4},
			{3,0,0,0, 0,190623,4, 0,190624,4, 0,190625,4},
			{3,0,0,0, 0,190610,4, 0,190611,4, 0,190612,4}
			};

	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, db_len[buff[0]], db_buf[buff[0]], p->id);
}

/* @brief get now temple height
 */
int build_temple_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len,4);
	response_proto_uint32(p, p->waitcmd, *(uint32_t*)buf, 0);
	return 0;
}

/* @brief send request to db, get fire, water, wood temple height
 */
int get_build_temple_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	return send_request_to_db(SVR_PROTO_GET_TEMPLE, p, 0, 0, p->id);
}

/* @brief get fire, water, wood temple now height
 */
int get_build_temple_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 12);
	uint32_t fire,water,wood;
	fire = *(uint32_t *)buf;
	water= *(uint32_t *)(buf + 4);
	wood = *(uint32_t *)(buf + 8);

	switch(p->waitcmd) {
		case PROTO_GET_BUILD_TEMPLE:
		{
			int j = sizeof(protocol_t);
			PKG_UINT32(msg, fire, j);
			PKG_UINT32(msg, water, j);
			PKG_UINT32(msg, wood, j);
			init_proto_head(msg, p->waitcmd, j);
			return send_to_self(p, msg, j, 1);
		}
		case PROTO_SET_PET_SKILL_TYPE:
			if (fire < 1 || water < 1 || wood < 1) {
//				return send_to_self_error(p, p->waitcmd, -ERR_not_build_all_temple, 1);
				DEBUG_LOG("SET_PET_SKILL_TYPE,%u %d %d %d",p->id,fire , water ,wood );
			}
			return send_request_to_db(SVR_PROTO_SET_PET_SKILL_TYPE, p, 8, p->session, p->id);
		default:
			break;
	}
	ERROR_LOG("BUG:CAN'T GET HERE, uid=%u, waitcmd=%d",id, p->waitcmd);
	return -1;
}

/* @breif get petid and data if one task not finish
 */
int get_pet_list_by_task_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	/*save taskid, taskstat*/
	uint32_t buff[2] = {0};
	int i = 0;

	UNPKG_UINT32(body, buff[0], i);
	buff[1] = 1;

	return send_request_to_db(SVR_PROTO_GET_PET_TASK_LIST_EX, p, 8, buff, p->id);
}

/* @breif give pet info which not fini task
 */
int get_pet_list_by_task_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct pet_task_info {
		uint32_t	petid;
		uint8_t		data[50];
	}__attribute__((packed)) pet_task_info_t;

	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + count * sizeof(pet_task_info_t));

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, count, j);
	int loop;
	for (loop = 0; loop < count; loop++) {
		pet_task_info_t* mti = (pet_task_info_t*)(buf + 4 + loop * sizeof(pet_task_info_t));
		pet_t* pet = get_pet(p->id, mti->petid);
		if (!pet) {
			continue;
		}
		PKG_UINT32(msg, pet->id, j);
		PKG_STR(msg, pet->nick, j, PET_NICK_LEN);
		PKG_UINT32(msg, pet->color, j);
		PKG_UINT8(msg, get_pet_level(pet), j);
		PKG_STR(msg, mti->data, j, 50);
	}

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int do_send_one_of_enum_items(sprite_t *p, char *buf, int len)
{
	int i = 0,t = 0;
	uint32_t records;
	uint32_t itmid,itmcn;

	CHECK_BODY_LEN_GE(len,4);
	UNPKG_H_UINT32(buf, records, i);

	DEBUG_LOG("send one of items id=%u records=%u", p->id, records);

	if (records == 0) {
		uint32_t rand_items[] = {13248,13249};
		itmid = rand_items[rand() % (sizeof(rand_items) / sizeof(uint32_t))];
	} else {
		CHECK_BODY_LEN(len, records * 8 + 4);
		for ( t = 0; t < records; t++) {
			UNPKG_H_UINT32(buf,itmid,i);
			UNPKG_H_UINT32(buf,itmcn,i);
			DEBUG_LOG("send one of items id=%u item=%u itmcn=%u", p->id, itmid,itmcn);

			if (itmcn == 0) {
				break;
			}
		}
	}

	if(records == 0 || t < records) {
		uint32_t buff[32] = {0};
		uint32_t ownitmid = *((uint32_t*)p->session);

		DEBUG_LOG("send one of items id=%u exchange %u -> %u", p->id, ownitmid, itmid);

		item_kind_t * itmkind = find_kind_of_item(itmid);
		if (!itmkind) {
			ERROR_LOG("id=%u itemid=%u kind err", p->id, itmid);
			return send_to_self_error(p, p->waitcmd, ERR_invalid_item_id, 1);
		}

		item_t *item = get_item(itmkind,itmid);
		if (!item) {
			ERROR_LOG("id=%u itemid=%u not exsit", p->id, itmid);
			return send_to_self_error(p, p->waitcmd, ERR_invalid_item_id, 1);
		}

		buff[0] = 1;
		buff[1] = 1;
		buff[2] = 0;
		buff[3] = 0;

		buff[4] = 0;
		buff[5] = ownitmid;
		buff[6] = 1;

		buff[7] = 0;
		buff[8] = itmid;
		buff[9] = 1;
		buff[10] = item->max;

		*((uint32_t*)p->session) = itmid;
		return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, 44, buff, p->id);

	}

	return send_to_self_error(p, p->waitcmd, -ERR_already_get_item, 1);
}


int exchange_item_to_rand_item_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int i = 0;
	uint32_t type = 0;
	uint32_t itemid,randid;
	uint32_t buff[64] = {0};

	CHECK_BODY_LEN(bodylen, 4);
	UNPKG_UINT32(body, itemid, i);

	if (itemid == 190586) {//fire
		uint32_t rand_items[] = {1230009, 1230018, 1230016, 180046, 180027, 180028, 180029, 180030};
		randid = rand_items[rand() % (sizeof(rand_items) / sizeof(uint32_t))];
	} else if (itemid == 190587) {
		uint32_t rand_items[] = {160671,160672,160673};
		randid = rand_items[rand() % (sizeof(rand_items) / sizeof(uint32_t))];
		type = 1;
	} else if (itemid == 190588) {
		uint32_t rand_items[] = {1200036};
		if (!p->followed) {
			return send_to_self_error(p, p->waitcmd, ERR_the_lahm_not_followed, 1);
		}
		randid = rand_items[rand() % (sizeof(rand_items) / sizeof(uint32_t))];
		*((uint32_t*)p->session) = randid;
		return db_single_item_op(p, p->id, itemid, 1, 0);
	} else if (itemid == 190589) {
//		uint32_t rand_items[] = {13248,13249};
//		randid = rand_items[rand() % (sizeof(rand_items) / sizeof(uint32_t))];

		buff[0] = 2;
		buff[1] = 13248;
		buff[2] = 13249;
		*((uint32_t*)p->session) = itemid;
		return send_request_to_db(SVR_QUERY_ITEM_COUNT, p, 12, buff, p->id);
	} else if (itemid == 190606) { // wood
		uint32_t rand_items[] = {1200038};//lahm cloth
		if (!p->followed) {
			return send_to_self_error(p, p->waitcmd, ERR_the_lahm_not_followed, 1);
		}
		randid = rand_items[rand() % (sizeof(rand_items) / sizeof(uint32_t))];
		*((uint32_t*)p->session) = randid;
		return db_single_item_op(p, p->id, itemid, 1, 0);
	} else if (itemid == 190607) {
		uint32_t rand_items[] = {160701,160702,160703};//room items
		randid = rand_items[rand() % (sizeof(rand_items) / sizeof(uint32_t))];
		type = 1;
	} else if (itemid == 190608) {
		uint32_t rand_items[] = {180046,180027,180028,1230018};//lahm food & zhongzi
		randid = rand_items[rand() % (sizeof(rand_items) / sizeof(uint32_t))];
	} else if (itemid == 190609) {
		uint32_t rand_items[] = {13277,13281};//mole cloth
		randid = rand_items[rand() % (sizeof(rand_items) / sizeof(uint32_t))];
	} else if (itemid == 190619) {//water
		uint32_t rand_items[] = {1200039};//lahm cloth
		if (!p->followed) {
			return send_to_self_error(p, p->waitcmd, ERR_the_lahm_not_followed, 1);
		}
		randid = rand_items[rand() % (sizeof(rand_items) / sizeof(uint32_t))];
		*((uint32_t*)p->session) = randid;
		return db_single_item_op(p, p->id, itemid, 1, 0);
	} else if (itemid == 190620) {
		uint32_t rand_items[] = {180056,180045,1230009,1230018};//lahm food & zhongzi
		randid = rand_items[rand() % (sizeof(rand_items) / sizeof(uint32_t))];
	} else if (itemid == 190621) {
		uint32_t rand_items[] = {160706,160707,160708};//room items
		randid = rand_items[rand() % (sizeof(rand_items) / sizeof(uint32_t))];
		type = 1;
	} else if (itemid == 190622) {
		uint32_t rand_items[] = {13282,13285};//mole cloth
		randid = rand_items[rand() % (sizeof(rand_items) / sizeof(uint32_t))];
	} else {
		ERROR_LOG("id=%u itemid=%u", p->id, itemid);
		return send_to_self_error(p, p->waitcmd, ERR_invalid_item_id, 1);
	}

	item_kind_t * itmkind = find_kind_of_item(randid);
	if (!itmkind) {
		ERROR_LOG("id=%u itemid=%u kind err", p->id, randid);
		return send_to_self_error(p, p->waitcmd, ERR_invalid_item_id, 1);
	}

	item_t *item = get_item(itmkind,randid);
	if (!item) {
		ERROR_LOG("id=%u itemid=%u not exsit", p->id, randid);
		return send_to_self_error(p, p->waitcmd, ERR_invalid_item_id, 1);
	}
	buff[0] = 1;
	buff[1] = 1;
	buff[2] = 0;
	buff[3] = 0;

	buff[4] = 0;
	buff[5] = itemid;
	buff[6] = 1;

	buff[7] = type;
	/*
	0:װ°媱:Сϝ,2:¼Ӕ°, 4:Ł³¡, 6:¼ª¼ªV, 7:°༶, 8:ǻ³µ)
	*/
	buff[8] = randid;
	buff[9] = 1;
	buff[10] = item->max;

	*((uint32_t*)p->session) = randid;
	DEBUG_LOG("id=%u itemid=%u exchange to randid=%u", p->id, itemid, randid);

	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, 44, buff, p->id);
}

int get_user_data_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int i = 0;
	uint32_t key;

	CHECK_BODY_LEN(bodylen, 4);
	UNPKG_UINT32(body, key, i);

	return send_request_to_db(SVR_PROTO_GET_USER_DATA, p, 4, &key, p->id);
}

int get_user_data_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	response_proto_uint32_uint32(p, p->waitcmd, *(uint32_t*)buf,*(uint32_t*)(buf+4), 0);
	return 0;
}

int set_user_data_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int i = 0;
	uint32_t buff[2] = {0};

	CHECK_BODY_LEN(bodylen, 8);

	UNPKG_UINT32(body, buff[0], i);//key
	UNPKG_UINT32(body, buff[1], i);//data

	return send_request_to_db(SVR_PROTO_SET_USER_DATA, p, 8, buff, p->id);
}

int set_user_data_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}


int get_client_object_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int i = 0;
	uint32_t key;

	CHECK_BODY_LEN(bodylen, 4);
	UNPKG_UINT32(body, key, i);

	DEBUG_LOG("get_client_object_cmd key=%d",key);

	return send_request_to_db(SVR_PROTO_GET_CLIENT_OBJ, p, 4, &key, p->id);
}

int get_client_object_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t key = *(uint32_t*)buf;

	DEBUG_LOG("get_client_object_cmd key=%d",key);

	int bytes = sizeof(protocol_t);
	PKG_UINT32(msg,key,bytes);
	PKG_STR(msg,buf + 4, bytes, len - 4);
	init_proto_head(msg,p->waitcmd,bytes);
	return send_to_self(p,msg,bytes,1);
}

int set_client_object_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen,4);
	CHECK_BODY_LEN_LE(bodylen,104);

	int i = 0;
	uint32_t key = 0;
	UNPKG_UINT32(body, key,i);

	DEBUG_LOG("set_client_object_cmd key=%d",key);

	i = 0;
	memset(p->session, 0, 108);
	PKG_H_UINT32(p->session,key,i);
	PKG_H_UINT32(p->session,bodylen - 4,i);
	memcpy(p->session + 8, body + 4, bodylen - 4);

	return send_request_to_db(SVR_PROTO_SET_CLIENT_OBJ, p, 108, p->session, p->id);
}

int set_client_object_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/* @brief 得到处于某种状态的拉姆的数量
 */
int get_lahm_count_accord_state_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t state = 0;
	unpack(body, sizeof(state), "L", &state);
	/*1表示神力超拉*/
	if (state != 1) {
		ERROR_RETURN(("INVALID STATE %u %u", p->id, state), 0);
	}
	return send_request_to_db(SVR_PROTO_GET_LAMU_COUNT_STATE, p, sizeof(state), &state, p->id);
}

/* @brief 得到拉姆处于某种状态的数量的回调函数
 */
int get_lahm_count_accord_state_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t count = 0;
	unpkg_host_uint32((uint8_t *)buf, &count);
	DEBUG_LOG("pet count %u", count);
	response_proto_uint32(p, p->waitcmd, count, 0);
	return 0;
}
//-------------------------------------------------------------------------------

/* @brief 删除死亡的拉姆
 */
int del_pet_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t petid;
	CHECK_BODY_LEN(bodylen,4);
	unpkg_uint32(body,bodylen,&petid);

	pet_t *pet = get_pet(p->id,petid);

	if(pet && PET_DEAD(pet) && (now.tv_sec - pet->sicktime) / 3600 >= 24 * 7) {
		del_pet(p->id,petid);
		response_proto_uint32(p,p->waitcmd,petid,0);
	} else {
		return send_to_self_error(p,p->waitcmd,-ERR_can_not_be_delete,1);
	}

	return 0;
}

