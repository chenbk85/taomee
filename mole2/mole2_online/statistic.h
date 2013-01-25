#ifndef STATISTIC_H_
#define STATISTIC_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "libtaomee/project/stat_agent/msglog.h"
#ifdef __cplusplus
}
#endif

#include "global.h"
#include "benchapi.h"

enum {
	MSG_ID_MOLE2_BUS		= 0x00000001,
	MSG_ID_LOG_ON_OFF		= 0x0B000001,
	MSG_ID_LOG_ON_OFF_DX	= 0x0B000010,
	MSG_ID_LOG_ON_OFF_WT	= 0x0B000011,
	MSG_ID_INCR_COUNT		= 0x0B000003,
	MSG_ID_USER_LEVEL		= 0x0B000004,
	MSG_ID_USER_LEVEL_NEW	= 0x0B000020,
	MSG_ID_XIAOMEE			= 0x0B000005,
	MSG_ID_FRIEND_CNT		= 0x0B000006,
	MSG_ID_PET_COUNT		= 0x0B000007,
	MSG_ID_SWORDMAN_BASE	= 0x0B000301,
	MSG_ID_ANCHER_BASE		= 0x0B000307,
	MSG_ID_MAGICIAN_BASE	= 0x0B00030D,
	MSG_ID_CHURCHMAN_BASE	= 0x0B000313,
	MSG_ID_NINJA_BASE		= 0x0B000319,
	MSG_ID_PET_LEVEL_BASE	= 0x0B000201,
	MSG_ID_PET_RACE_BASE	= 0x0B00020B,
	MSG_ID_PET_TASK_BASE	= 0x0B080000,
	MSG_ID_PET_CATCH_BASE	= 0x0B100000,
	MSG_ID_PET_ACTIVATE_BASE= 0x0B083000,
	MSG_ID_TASK_MAJOR_BASE	= 0x0B300000,
	MSG_ID_TASK_MINOR_BASE	= 0x0B350000,
	MSG_ID_TASK_DAY_BASE	= 0x0B400000,
	MSG_ID_SINGLE_PK		= 0x0B000008,
	MSG_ID_TEAM_PK			= 0x0B000009,
	MSG_ID_BEAST_TYPE_BASE	= 0x0B201000,
	MSG_ID_BEAST_LV_BASE	= 0x0B202000,
	MSG_ID_BEAST_GRP_BASE	= 0x0B203000,
	MSG_ID_ONLINE_TIME		= 0x0B00000B,
	MSG_LOG_INCR_CNT_MM		= 0x0B00000A,
	MSG_ID_PET_EGG_GET		= 0x0B500000,
	MSG_ID_PK_POINT			= 0x0B00000C,
	MSG_ID_PKPT_EXCHANGE	= 0x0B600000,
	MSG_ID_EXCHANGE_ACT		= 0x0B610000,
	MSG_ID_COMPOSITE_CLOTH	= 0x0B620000,
	MSG_ID_COMPOSITE_FASION	= 0x0B590000,
	MSG_ID_READ_NEWS_DX		= 0x0B00000E,
	MSG_ID_READ_NEWS_WT		= 0x0B00000F,
	MSG_ID_READ_CALENDAR_DX	= 0x0B000051,
	MSG_ID_READ_CALENDAR_WT = 0x0B000052,
	MSG_ID_HELP_CLEAN		= 0x0B530001,
	MSG_ID_HELP_BOSS		= 0x0B530002,
	MSG_ID_GRP_CHALLENG_9	= 0x0B531001,
	MSG_ID_GRP_CHALLENG_10	= 0x0B531002,
	MSG_ID_GRP_CHALLENG_11	= 0x0B531003,
	MSG_ID_GRP_CHALLENG_12	= 0x0B531004,
	MSG_ID_GRP_CHALLENG_13	= 0x0B531005,
	MSG_ID_GRP_CHALLENG_15	= 0x0B531006,
	MSG_ID_CLEAN_BOSS_BASE	= 0x0B531401,
	MSG_VIP_ITEM_GET_BASE	= 0x0B532001,
	MSG_VIP_ITEM_USE_BASE	= 0x0B535001,
	MSG_ID_READ_VIP			= 0x0B538001,
	MSG_ID_GET_MIZONG		= 0x0B534801,
	MSG_ID_USE_MIZONG		= 0x0B537801,
	MSG_ID_VACANCY			= 0x0B000021,

	MSG_ID_READ_MAIL_DX		= 0x0B000022,
	MSG_ID_READ_MAIL_WT		= 0x0B000023,

	MSG_HERO_CUP_WIND_REG	= 0x0B531641,
	MSG_HERO_CUP_EARTH_REG	= 0x0B531671,
	MSG_HERO_CUP_WATER_REG	= 0x0B5316A1,
	MSG_HERO_CUP_FIRE_REG	= 0x0B5316D1,

	MSG_HERO_CUP_WIND_IN	= 0x0B531642,
	MSG_HERO_CUP_EARTH_IN	= 0x0B531672,
	MSG_HERO_CUP_WATER_IN	= 0x0B5316A2,
	MSG_HERO_CUP_FIRE_IN	= 0x0B5316D2,

	MSG_HERO_CUP_WIND_M		= 0x0B531643,
	MSG_HERO_CUP_EARTH_M	= 0x0B531673,
	MSG_HERO_CUP_WATER_M	= 0x0B5316A3,
	MSG_HERO_CUP_FIRE_M		= 0x0B5316D3,

	MSG_HERO_CUP_WIND_MU	= 0x0B531644,
	MSG_HERO_CUP_EARTH_MU	= 0x0B531674,
	MSG_HERO_CUP_WATER_MU	= 0x0B5316A4,
	MSG_HERO_CUP_FIRE_MU	= 0x0B5316D4,
	MSG_TUZHI				= (0x0B630000),

	MSG_PET_GET_EXP			= 0x0B640001,
	MSG_PET_GET_SKILL_EXP	= 0x0B640002,
	MSG_PET_GET_PET_EXP		= 0x0B640003,
	MSG_PET_GET_XIAOMEE		= 0x0B640004,
	MSG_PET_GET_VACANCY		= 0x0B640005,

	MSG_ID_GRADUATE			= 0x0B000053,
	
	MSG_ID_WAR_ENTER		= 0x0B640006,
	MSG_ID_WAR_FIN			= 0x0B640007,

	MSG_ID_FIREWORK_GET		= 0x0B5316d5,

	MSG_ID_EXP_HANG			= 0x0B538002,
	MSG_ID_PET_HANG			= 0x0B538003,
	MSG_ID_SKILL_HANG		= 0x0B538004,

	MSG_ID_TASK_39001_BASE	= 0x0B5316da,
	MSG_ID_LOOP_GRP_BASE	= 0x0B531700,

	MSG_ID_SIGNAL			= 0x0B53174a,

	MSG_ID_ACTIVE_ITEM_BASE	= 0x0B531742,
	MSG_ID_GRASS_NORMAL		= 0x0B531740,
	MSG_ID_GRASS_VIP		= 0x0B531741,
	MSG_ID_CHALLENGE_REWARD	= 0x0B531738,
	MSG_ID_CHALLENGE_TOTAL	= 0x0B531732,
	MSG_ID_CHALLENGE_WIN	= 0x0B531733,
	MSG_ID_CHALLENGE_SINGLE	= 0x0B531734,
	MSG_ID_CHALLENGE_SWIN	= 0x0B531735,
	MSG_ID_CHALLENGE_UCNT	= 0x0B531736,
	MSG_ID_CHALLENGE_UWIN	= 0x0B531737,
	MSG_ID_FEED_PET			= 0x0B531731,
	MSG_ID_PLANT_ACTIVE		= 0x0B531730,
	MSG_ID_GAME_BASE		= 0x0B531723,
	MSG_ID_BEAST_ITEM_BASE	= 0x0B531753,
};

enum {
	pet_egg_lucky_draw		= 1,
	pet_egg_magic_code		= 2,
	pet_egg_exchange		= 3,
	pet_egg_task_reward		= 4,
	pet_egg_active_get		= 5,
	pet_egg_damee_buy		= 6,
	pet_egg_challenge		= 7,
};

static inline void
msg_log_uint32(uint32_t msgid, uint32_t value)
{
	msglog(statistic_logfile, msgid, get_now_tv()->tv_sec, &value, 4);
}

static inline void
msg_log_uint32_uint32(uint32_t msgid, uint32_t val1, uint32_t val2)
{
	uint32_t out[2];
	out[0] = val1;
	out[1] = val2;
	msglog(statistic_logfile, msgid, get_now_tv()->tv_sec, out, 8);
}

static inline void
msg_log_uint32_uint32_uint32(uint32_t msgid, uint32_t val1, uint32_t val2, uint32_t val3)
{
	uint32_t out[3];
	out[0] = val1;
	out[1] = val2;
	out[2] = val3;
	msglog(statistic_logfile, msgid, get_now_tv()->tv_sec, out, 12);
}


static inline void
msg_log_user_on(uint32_t uid)
{
	uint32_t out[3];
	out[0] = 1;
	out[1] = 0;
	out[2] = uid;
	msglog(statistic_logfile, MSG_ID_LOG_ON_OFF, get_now_tv()->tv_sec, out, 12);

	if (idc_type == idc_type_dx) {
		msglog(statistic_logfile, MSG_ID_LOG_ON_OFF_DX, get_now_tv()->tv_sec, out, 12);
	} else if (idc_type == idc_type_wt){
		msglog(statistic_logfile, MSG_ID_LOG_ON_OFF_WT, get_now_tv()->tv_sec, out, 12);
	}
}

static inline void
msg_log_user_off(uint32_t uid)
{
	uint32_t out[3];
	out[0] = 0;
	out[1] = 1;
	out[2] = uid;
	msglog(statistic_logfile, MSG_ID_LOG_ON_OFF, get_now_tv()->tv_sec, out, 12);
	if (idc_type == idc_type_dx) {
		msglog(statistic_logfile, MSG_ID_LOG_ON_OFF_DX, get_now_tv()->tv_sec, out, 12);
	} else if (idc_type == idc_type_wt) {
		msglog(statistic_logfile, MSG_ID_LOG_ON_OFF_WT, get_now_tv()->tv_sec, out, 12);
	}
}

static inline void
msg_log_online_time(uint32_t oltime)
{
	msg_log_uint32(MSG_ID_ONLINE_TIME, oltime);
}

static inline void
msg_log_mole_reg(uint32_t uid)
{
	msg_log_uint32_uint32_uint32(MSG_LOG_INCR_CNT_MM, uid, uid, get_now_tv()->tv_sec);
	msg_log_uint32(MSG_ID_INCR_COUNT, 1);
}

static inline void
msg_log_user_level(uint32_t uid, uint32_t regtime, uint32_t level)
{
	msg_log_uint32_uint32(MSG_ID_USER_LEVEL, uid, level);
	if (regtime >= 1306447200)
		msg_log_uint32_uint32(MSG_ID_USER_LEVEL_NEW, uid, level);
}

static inline void
msg_log_user_xiaomee(uint32_t uid, uint32_t xiaomee)
{
	msg_log_uint32_uint32(MSG_ID_XIAOMEE, uid, xiaomee);
}

static inline void
msg_log_user_friend(uint32_t uid, uint32_t frcnt)
{
	msg_log_uint32_uint32(MSG_ID_FRIEND_CNT, uid, frcnt);
}

static inline void
msg_log_pet_cnt(uint32_t uid, uint32_t petcnt)
{
	msg_log_uint32_uint32(MSG_ID_PET_COUNT, uid, petcnt);
}

static inline void
msg_log_help_clean(uint32_t uid, uint32_t cleancnt)
{
	msg_log_uint32_uint32(MSG_ID_HELP_CLEAN, uid, cleancnt);
}

static inline void
msg_log_hero_cup_reg(uint32_t team)
{
	switch (team) {
	case 1:
		msg_log_uint32(MSG_HERO_CUP_WIND_REG, 1);
		break;
	case 2:
		msg_log_uint32(MSG_HERO_CUP_EARTH_REG, 1);
		break;
	case 3:
		msg_log_uint32(MSG_HERO_CUP_WATER_REG, 1);
		break;
	case 4:
		msg_log_uint32(MSG_HERO_CUP_FIRE_REG, 1);
		break;
	}
}

static inline void
msg_log_hero_cup_in(uint32_t team)
{
	switch (team) {
	case 1:
		msg_log_uint32(MSG_HERO_CUP_WIND_IN, 1);
		break;
	case 2:
		msg_log_uint32(MSG_HERO_CUP_EARTH_IN, 1);
		break;
	case 3:
		msg_log_uint32(MSG_HERO_CUP_WATER_IN, 1);
		break;
	case 4:
		msg_log_uint32(MSG_HERO_CUP_FIRE_IN, 1);
		break;
	}
}

static inline void
msg_log_hero_cup_medal(uint32_t uid, uint32_t team, uint32_t count)
{
	switch (team) {
	case 1:
		msg_log_uint32(MSG_HERO_CUP_WIND_M, count);
		msg_log_uint32_uint32(MSG_HERO_CUP_WIND_MU, uid, count);
		break;
	case 2:
		msg_log_uint32(MSG_HERO_CUP_EARTH_M, 1);
		msg_log_uint32_uint32(MSG_HERO_CUP_EARTH_MU, uid, count);
		break;
	case 3:
		msg_log_uint32(MSG_HERO_CUP_WATER_M, 1);
		msg_log_uint32_uint32(MSG_HERO_CUP_WATER_MU, uid, count);
		break;
	case 4:
		msg_log_uint32(MSG_HERO_CUP_FIRE_M, 1);
		msg_log_uint32_uint32(MSG_HERO_CUP_FIRE_MU, uid, count);
		break;
	}
}



static inline void
msg_log_help_boss(uint32_t uid, uint32_t bosscnt)
{
	msg_log_uint32_uint32(MSG_ID_HELP_BOSS, uid, bosscnt);
}

static inline void
msg_log_vip_item_get(uint32_t uid, uint32_t itemid)
{
	if (itemid > 350000 && itemid < 360000)
		msg_log_uint32(MSG_VIP_ITEM_GET_BASE + itemid - 350001, uid);
}

static inline void
msg_log_vip_item_use(uint32_t uid, uint32_t itemid, uint32_t count)
{
	if (itemid == 220039)
		msg_log_uint32_uint32(MSG_ID_USE_MIZONG, uid, count);
	else if (itemid > 350000)
		msg_log_uint32_uint32(MSG_VIP_ITEM_GET_BASE + itemid - 350001, uid, count);
}

static inline void
msg_log_vacancy(uint32_t uid, uint32_t count)
{
	msg_log_uint32_uint32(MSG_ID_VACANCY, uid, count);
}

static inline int
get_profession_msgid_base(uint32_t pro)
{
	switch (pro) {
	case 1: 
		return MSG_ID_SWORDMAN_BASE;
	case 2:
		return MSG_ID_ANCHER_BASE;
	case 3:
		return MSG_ID_MAGICIAN_BASE;
	case 4:
		return MSG_ID_CHURCHMAN_BASE;
	case 5:
		return MSG_ID_NINJA_BASE;
	default:
		return 0;
	}
}

static inline void
msg_log_profession(uint32_t pro, uint32_t prolv, uint32_t prepro, uint32_t preprolv)
{
	int msgid = get_profession_msgid_base(prepro);
	if (msgid && preprolv < 6) {
		msg_log_uint32_uint32(msgid + preprolv, 0 ,1);
	}
	msgid = get_profession_msgid_base(pro);
	if (msgid && prolv < 6) {
		msg_log_uint32_uint32(msgid + prolv, 1 ,0);
	}
}

static inline void
msg_log_pet_level(uint32_t level, uint32_t prelv)
{
	if (prelv) {
		msg_log_uint32_uint32(MSG_ID_PET_LEVEL_BASE + prelv / 10, 0 ,1);
	}

	msg_log_uint32_uint32(MSG_ID_PET_LEVEL_BASE + level / 10, 1, 0);
}

static inline void
msg_log_pet_race(uint32_t race, int add)
{
	if (add) {
		msg_log_uint32_uint32(MSG_ID_PET_RACE_BASE + race, 1, 0);
	} else {
		msg_log_uint32_uint32(MSG_ID_PET_RACE_BASE + race, 0, 1);
	}
}

static inline void
msg_log_pet_egg(uint32_t beastid, uint32_t channel, uint32_t count)
{
	uint32_t msgid = MSG_ID_PET_EGG_GET + channel * 2000 + beastid;
	msg_log_uint32(msgid, count);
}

static inline void
msg_log_pet_activate_get(uint32_t beastid)
{
	msg_log_uint32(MSG_ID_PET_ACTIVATE_BASE + beastid, 1);
}

/*装备图纸*/
static inline void
msg_log_tuzhi(uint32_t  tuzhi_id,int count)
{
	if (tuzhi_id>=330000 &&  tuzhi_id<340000 ){
		if (count>0)  {
			DEBUG_LOG("msg_log_tuzhi add tuzhi_id=%u,%d ",tuzhi_id ,count );
			msg_log_uint32(MSG_TUZHI+ (tuzhi_id-330000), count);
		}
	}
}



static inline void
msg_log_pet_task(uint32_t beastid)
{
	msg_log_uint32(MSG_ID_PET_TASK_BASE + beastid, 1);
}

static inline void
msg_log_pet_catch(uint32_t beastid, uint32_t lv)
{
	msg_log_uint32_uint32(MSG_ID_PET_CATCH_BASE + beastid, (lv == 1), (lv != 1));
}

static inline void
msg_log_task(uint32_t taskid, uint32_t nodeid)
{
	uint32_t msgid = 0;
	switch(taskid / 10000) {
		case 0:
			msgid = MSG_ID_TASK_MAJOR_BASE + taskid * 0x20 + nodeid;
			break;
		case 1:
			msgid = MSG_ID_TASK_MINOR_BASE + (taskid - 10000) * 0x20 + nodeid;
			break;
		case 2:
		case 3:
		case 4:
			msgid = MSG_ID_TASK_DAY_BASE + (taskid - 20000) * 0x20 + nodeid;
			//DEBUG_LOG("log task id=%u nodeid=%u",taskid,nodeid);
			break;
		default:
			return;
	}
	msg_log_uint32(msgid, 1);
}

static inline void
msg_log_single_pk()
{
	msg_log_uint32(MSG_ID_SINGLE_PK, 1);
}

static inline void
msg_log_clean_boss_road(uint32_t uid, uint32_t grpid)
{
	msg_log_uint32(MSG_ID_CLEAN_BOSS_BASE + grpid - 2501, uid);
}

static inline void
msg_log_team_pk()
{
	msg_log_uint32(MSG_ID_TEAM_PK, 1);
}

static inline void
msg_log_beast_type_kill(uint32_t beastid)
{
	uint32_t msgid;
	if (beastid <= 2000) {
		msgid = MSG_ID_BEAST_TYPE_BASE + beastid;
	} else return;
	msg_log_uint32(msgid, 1);
}

static inline void
msg_log_beast_lv_kill(uint32_t level)
{
	msg_log_uint32(MSG_ID_BEAST_LV_BASE + level, 1);
}

static inline void
msg_log_beast_grp_kill(uint32_t grpid)
{
	msg_log_uint32(MSG_ID_BEAST_GRP_BASE + grpid, 1);
}

static inline void msg_log_grp_spec_challenge(sprite_t* p, uint32_t grpid)
{
	uint32_t msgid = 0;
	switch (grpid) {
		case 9:
			msgid = MSG_ID_GRP_CHALLENG_9;
			break;
		case 10:
			msgid = MSG_ID_GRP_CHALLENG_10;
			break;
		case 11:
			msgid = MSG_ID_GRP_CHALLENG_11;
			break;
		case 12:
			msgid = MSG_ID_GRP_CHALLENG_12;
			break;
		case 13:
			msgid = MSG_ID_GRP_CHALLENG_13;
			break;
		case 15:
			msgid = MSG_ID_GRP_CHALLENG_15;
			break;
		default:
			return;
	}

	if (!p->btr_team) {
		msg_log_uint32_uint32(msgid, p->id, 0);
		return;
	}

	int loop;
	for (loop = 0; loop < p->btr_team->count; loop ++) {
		msg_log_uint32_uint32(msgid, p->btr_team->players[loop]->id, 0);
	}
	
}

static inline void msg_log_pet_spec_catch(uint32_t uid, uint32_t beastid)
{
	uint32_t msgid = 0;
	switch (beastid) {
		case 42:
			msgid = MSG_ID_GRP_CHALLENG_9;
			break;
		case 43:
			msgid = MSG_ID_GRP_CHALLENG_10;
			break;
		case 35:
			msgid = MSG_ID_GRP_CHALLENG_11;
			break;
		case 47:
			msgid = MSG_ID_GRP_CHALLENG_12;
			break;
		case 48:
			msgid = MSG_ID_GRP_CHALLENG_13;
			break;
		case 19:
			msgid = MSG_ID_GRP_CHALLENG_15;
			break;
		default:
			return;
	}

	msg_log_uint32_uint32(msgid, 0, uid);
}

typedef struct bus_proto {
	uint16_t		pkglen;
	uint16_t		cmdid;
	uint32_t		userid;
	uint8_t			version;
	uint8_t			body[];
}__attribute__((packed))bus_proto_t;


static inline void
msg_log_bus_mole2_sync_vip(uint32_t userid, char* nick)
{
	uint8_t out[64];
	bus_proto_t* pbp = (bus_proto_t *)out;
	int i = sizeof(bus_proto_t);
	PKG_H_UINT32(out, get_now_tv()->tv_sec, i);
	PKG_STR(out, nick, i, USER_NICK_LEN);
	pbp->pkglen = i;
	pbp->userid = userid;
	pbp->cmdid = 5004;
	pbp->version = 1;
	msglog(statistic_logfile, 1, get_now_tv()->tv_sec, out, i);
}

static inline void
msg_log_bus_mole2_reg(uint32_t userid, char* nick)
{
	uint8_t out[64];
	bus_proto_t* pbp = (bus_proto_t *)out;
	int i = sizeof(bus_proto_t);
	PKG_H_UINT32(out, get_now_tv()->tv_sec, i);
	PKG_STR(out, nick, i, USER_NICK_LEN);
	pbp->pkglen = i;
	pbp->userid = userid;
	pbp->cmdid = 5001;
	pbp->version = 1;
	msglog(statistic_logfile, 1, get_now_tv()->tv_sec, out, i);
}

static inline void
msg_log_bus_mole2_jobup(uint32_t userid, char* nick, uint32_t oldlv, uint32_t newlv,uint32_t prof)
{
	uint8_t out[64];
	bus_proto_t* pbp = (bus_proto_t *)out;
	int i = sizeof(bus_proto_t);
	PKG_H_UINT32(out, get_now_tv()->tv_sec, i);
	PKG_STR(out, nick, i, USER_NICK_LEN);
	PKG_H_UINT32(out, oldlv, i);
	PKG_H_UINT32(out, newlv, i);
	PKG_H_UINT32(out, prof, i);
	pbp->pkglen = i;
	pbp->userid = userid;
	pbp->cmdid = 5002;
	pbp->version = 1;
	msglog(statistic_logfile, 1, get_now_tv()->tv_sec, out, i);
}

static inline void
msg_log_pk_point(uint32_t userid, uint32_t pkpoint)
{
	msg_log_uint32_uint32(MSG_ID_PK_POINT, userid, pkpoint);
}

static inline void
msg_log_pkpt_exchange(uint32_t uid, uint32_t exchangeid)
{
	if (exchangeid < 20000)
		msg_log_uint32_uint32(MSG_ID_PKPT_EXCHANGE + exchangeid, uid, 1);
	msg_log_uint32_uint32(MSG_ID_EXCHANGE_ACT + exchangeid, uid, 1);
}

static inline void
msg_log_composite_cloth(uint32_t clothid)
{
	if (clothid > 140000)
		msg_log_uint32(MSG_ID_COMPOSITE_CLOTH + clothid - 140000, 1);
	else if (clothid > 88000 && clothid < 89000)
		msg_log_uint32(MSG_ID_COMPOSITE_FASION + clothid - 88000, 1);
}

static inline void
msg_log_read_news(uint32_t uid)
{
	switch (idc_type) {
		case idc_type_dx:
			msg_log_uint32_uint32(MSG_ID_READ_NEWS_DX, uid, 1);
			break;
		case idc_type_wt:
			msg_log_uint32_uint32(MSG_ID_READ_NEWS_WT, uid, 1);
			break;
	}
}

static inline void
msg_log_read_calendar(uint32_t uid)
{
	switch (idc_type) {
		case idc_type_dx:
			msg_log_uint32_uint32(MSG_ID_READ_CALENDAR_DX, uid, 1);
			break;
		case idc_type_wt:
			msg_log_uint32_uint32(MSG_ID_READ_CALENDAR_WT, uid, 1);
			break;
	}
}


static inline void
msg_log_read_mail(uint32_t uid)
{
	switch (idc_type) {
		case idc_type_dx:
			msg_log_uint32_uint32(MSG_ID_READ_MAIL_DX, uid, 1);
			break;
		case idc_type_wt:
			msg_log_uint32_uint32(MSG_ID_READ_MAIL_WT, uid, 1);
			break;
	}
}


static inline void
msg_log_read_vip(uint32_t uid)
{
	msg_log_uint32(MSG_ID_READ_VIP, uid);
}

static inline void
msg_log_pet_for_exp(uint32_t uid)
{
	msg_log_uint32_uint32(MSG_PET_GET_EXP, uid, 1);
}

static inline void
msg_log_pet_for_skill_exp(uint32_t uid)
{
	msg_log_uint32_uint32(MSG_PET_GET_SKILL_EXP, uid, 1);
}

static inline void
msg_log_pet_for_pet_exp(uint32_t uid)
{
	msg_log_uint32_uint32(MSG_PET_GET_PET_EXP, uid, 1);
}

static inline void
msg_log_pet_for_vacancy(uint32_t uid)
{
	msg_log_uint32_uint32(MSG_PET_GET_VACANCY, uid, 1);
}

static inline void
msg_log_pet_for_xiaomee(uint32_t uid)
{
	msg_log_uint32_uint32(MSG_PET_GET_XIAOMEE, uid, 1);
}

static inline void
msg_log_graduation(uint32_t uid)
{
	msg_log_uint32_uint32(MSG_ID_GRADUATE, uid, 1);
}

static inline void
msg_log_war_enter(uint32_t uid)
{
	msg_log_uint32_uint32(MSG_ID_WAR_ENTER, uid, 1);
}

static inline void
msg_log_war_fin(uint32_t uid)
{
	msg_log_uint32_uint32(MSG_ID_WAR_FIN, uid, 1);
}

static inline void
msg_log_firework_get(uint32_t uid)
{
	msg_log_uint32_uint32(MSG_ID_FIREWORK_GET, uid, 1);
}

static inline void
msg_log_exp_hang(uint32_t uid)
{
	msg_log_uint32(MSG_ID_EXP_HANG, uid);
}

static inline void
msg_log_pet_hang(uint32_t uid)
{
	msg_log_uint32(MSG_ID_PET_HANG, uid);
}

static inline void
msg_log_skill_hang(uint32_t uid)
{
	msg_log_uint32(MSG_ID_SKILL_HANG, uid);
}

static inline void
msg_log_task_39001(uint32_t uid, uint32_t recv, uint32_t fin, uint32_t level)
{
	if (level < 10 || level > 50)
		return;
	
	uint32_t msgid = MSG_ID_TASK_39001_BASE;
	if (level > 15)
		msgid += (level - 11) / 5;
	uint32_t out[4];
	out[0] = recv ? uid : 0;
	out[1] = recv;
	out[2] = fin ? uid : 0;
	out[3] = fin;
	msglog(statistic_logfile, msgid, get_now_tv()->tv_sec, out, 16);
}

static inline void
msg_log_loop_grp(uint32_t uid, uint32_t fight, uint32_t refresh, uint32_t grpid)
{
	if (grpid > 2)
		return;
	
	uint32_t msgid = MSG_ID_LOOP_GRP_BASE + grpid;
	uint32_t out[4];
	out[0] = fight ? uid : 0;
	out[1] = fight;
	out[2] = refresh ? uid : 0;
	out[3] = refresh;
	msglog(statistic_logfile, msgid, get_now_tv()->tv_sec, out, 16);
}

static inline void
msg_log_signal(uint32_t uid, uint32_t buy, uint32_t use)
{
	uint32_t out[3];
	out[0] = buy ? uid : 0;
	out[1] = use ? uid : 0;
	out[2] = use;
	msglog(statistic_logfile, MSG_ID_SIGNAL, get_now_tv()->tv_sec, out, sizeof(out));
}


static inline void
msg_log_draw_once(uint32_t uid, uint32_t type)
{
	switch (type) {
	case 1:
		msg_log_uint32_uint32(0x0B531710, uid, 1);
		break;
	case 2:
		msg_log_uint32_uint32(0x0B531711, uid, 1);
		break;
	}
}

static inline void
msg_log_draw_get_item(uint32_t itemid)
{
	uint32_t msgid = 0;
	switch (itemid) {
	case 350015:
		msgid = 0x0B531712;
		break;
	case 89003:
		msgid = 0x0B531713;
		break;
	case 350014:
		msgid = 0x0B531714;
		break;
	case 350009:
		msgid = 0x0B531715;
		break;
	case 350007:
		msgid = 0x0B531716;
		break;
	case 350003:
		msgid = 0x0B531717;
		break;
	case 210008:
		msgid = 0x0B531718;
		break;
	case 300002:
		msgid = 0x0B53171a;
		break;
	case 310021:
		msgid = 0x0B53171b;
		break;
	case 310022:
		msgid = 0x0B53171c;
		break;
	case 310023:
		msgid = 0x0B53171d;
		break;
	case 220036:
		msgid = 0x0B53171e;
		break;
	case 220035:
		msgid = 0x0B53171f;
		break;
	case 300001:
		msgid = 0x0B531720;
		break;
	case 210006:
		msgid = 0x0B531721;
		break;
	case 310041:
		msgid = 0x0B531781;
		break;
	case 350020:
		msgid = 0x0B531782;
		break;
	case 350025:
		msgid = 0x0B531783;
		break;
	case 350026:
		msgid = 0x0B531784;
		break;
	default:
		return;
	}
}

static inline void
msg_log_active_item(int idx)
{
	msg_log_uint32(MSG_ID_ACTIVE_ITEM_BASE + idx, 1);
}

static inline void
msg_log_beast_item_get(int idx, uint32_t count)
{
	msg_log_uint32(MSG_ID_BEAST_ITEM_BASE + idx, count);
}


static inline void
msg_log_grass_normal(uint32_t uid)
{
	msg_log_uint32_uint32(MSG_ID_GRASS_NORMAL, uid, 1);
}

static inline void
msg_log_grass_vip(uint32_t uid)
{
	msg_log_uint32_uint32(MSG_ID_GRASS_VIP, uid, 1);
}

static inline void
msg_log_challenge_reward(int idx)
{
	msg_log_uint32(MSG_ID_CHALLENGE_REWARD + idx, 1);
}

static inline void
msg_log_challenge_total()
{
	msg_log_uint32(MSG_ID_CHALLENGE_TOTAL, 1);
}

static inline void
msg_log_challenge_win()
{
	msg_log_uint32(MSG_ID_CHALLENGE_WIN, 1);
}

static inline void
msg_log_challenge_singel()
{
	msg_log_uint32(MSG_ID_CHALLENGE_SINGLE, 1);
}

static inline void
msg_log_challenge_swin()
{
	msg_log_uint32(MSG_ID_CHALLENGE_SWIN, 1);
}

static inline void
msg_log_challenge_user(uint32_t uid)
{
	msg_log_uint32(MSG_ID_CHALLENGE_UCNT, uid);
}

static inline void
msg_log_challenge_uwin(uint32_t uid)
{
	msg_log_uint32(MSG_ID_CHALLENGE_UWIN, uid);
}

static inline void
msg_log_feed_pet(uint32_t uid)
{
	msg_log_uint32(MSG_ID_FEED_PET, uid);
}

static inline void
msg_log_plant_active(uint32_t engage)
{
	msg_log_uint32_uint32(MSG_ID_PLANT_ACTIVE, engage, !engage);
}

static inline void
msg_log_game(uint32_t uid, int idx)
{
	msg_log_uint32(MSG_ID_GAME_BASE + idx, uid);
}

static inline void
msg_log_kuro_chanllenge(uint32_t uid, uint32_t grpid, uint32_t challenge)
{
	uint32_t msgid = 0;
	switch (grpid) {
	case 2908:
		msgid = 0x0B531750;
		break;
	case 2909:
		msgid = 0x0B531751;
		break;
	case 2910:
		msgid = 0x0B531752;
		break;
	default:
		return;
	}
	
	uint32_t out[5];
	out[0] = challenge;
	out[1] = 0;
	out[2] = uid;
	out[3] = 0;
	out[4] = 1;
	msglog(statistic_logfile, msgid, get_now_tv()->tv_sec, out, sizeof(out));
}

static inline void
msg_log_kuro_win(uint32_t uid, uint32_t grpid, uint32_t win)
{
	uint32_t msgid = 0;
	switch (grpid) {
	case 2908:
		msgid = 0x0B531750;
		break;
	case 2909:
		msgid = 0x0B531751;
		break;
	case 2910:
		msgid = 0x0B531752;
		break;
	default:
		return;
	}
	
	uint32_t out[5];
	out[0] = 0;
	out[1] = win;
	out[2] = 0;
	out[3] = uid;
	out[4] = 0;
	msglog(statistic_logfile, msgid, get_now_tv()->tv_sec, out, sizeof(out));
}

static inline void msg_log_copy_engage(uint32_t uid, uint32_t lv)
{
	uint32_t msgid = 0;
	if (lv >= 15 && lv <= 20) {
		msgid = 0x0B531771;
	} else if (lv >= 21 && lv <= 25) {
		msgid = 0x0B531773;
	} else if (lv >= 26 && lv <= 30) {
		msgid = 0x0B531775;
	} else if (lv >= 31 && lv <= 35) {
		msgid = 0x0B531777;
	} else if (lv >= 36 && lv <= 40) {
		msgid = 0x0B53177a;
	} else if (lv >= 41 && lv <= 45) {
		msgid = 0x0B53177c;
	} else if (lv >= 46 && lv <= 50) {
		msgid = 0x0B53177e;
	}

	if (msgid)
		msg_log_uint32_uint32(msgid, uid, 1);
}

static inline void msg_log_copy_fin(uint32_t uid, uint32_t lv)
{
	uint32_t msgid = 0;
	if (lv >= 15 && lv <= 20) {
		msgid = 0x0B531772;
	} else if (lv >= 21 && lv <= 25) {
		msgid = 0x0B531774;
	} else if (lv >= 26 && lv <= 30) {
		msgid = 0x0B531776;
	} else if (lv >= 31 && lv <= 35) {
		msgid = 0x0B531778;
	} else if (lv >= 36 && lv <= 40) {
		msgid = 0x0B53177b;
	} else if (lv >= 41 && lv <= 45) {
		msgid = 0x0B53177d;
	} else if (lv >= 46 && lv <= 50) {
		msgid = 0x0B53177f;
	}

	if (msgid)
		msg_log_uint32_uint32(msgid, uid, 1);
}

static inline void
msg_log_mooncake(uint32_t count)
{
	msg_log_uint32(0x0B531780, count);
}

static inline void
msg_log_online_gift(uint32_t idx, uint32_t uid)
{
	msg_log_uint32(0x0B531766+idx, uid);
}

static inline void
msg_log_precious_box(uint32_t idx, uint32_t uid)
{
	msg_log_uint32(0x0B531790+idx, uid);
}


static inline void
msg_log_play_roulette(uint32_t uid)
{
	msg_log_uint32_uint32(0x0B650001, uid,1);
}

static inline void
msg_log_roulette_item(uint32_t idx, uint32_t uid)
{
	msg_log_uint32(0x0B650011+idx, uid);
}

static inline void
msg_log_pumpkin_activity(uint32_t uid)
{
	msg_log_uint32(0x0B53170b, uid);
}

static inline void
msg_log_pumpkin_reward(uint32_t count)
{
	msg_log_uint32(0x0B53170c, count);
}

static inline void
msg_log_challenge_user_by_level(uint32_t grpid,uint32_t count, uint32_t uid)
{
	switch ( grpid ){
		case 2921 :
			msg_log_uint32(0x0B531797+count,uid);
			break;
		case 48 :
		case 49 :
			msg_log_uint32(0x0B531800+count,uid);
			break;
		case 50:
			msg_log_uint32(0x0B53180a+count,uid);
			break;
		case 51:
			msg_log_uint32(0x0B531826+count,uid);
			break;
		default :
			break;
	}
}

static inline void
msg_log_exchange(uint32_t eid,uint32_t count)
{
	if( eid>=20051 && eid <= 20068 ){
		msg_log_uint32((0x0B531813+(eid-20051)),count);
	}else if(eid>=10084 && eid <= 10094){
		msg_log_uint32((0x0B53182f+(eid-10084)),count);
	}
}

static inline void
msg_log_pvp_user(uint32_t uid,uint32_t type)
{
	switch ( type ){
		case 0 :
			msg_log_uint32(0x0B5316d7,uid);	
			break;
		case 1 :
			msg_log_uint32(0x0B5316e3,uid);	
			break;
		default :
			break;
	}
}

static inline void
msg_log_pvp_start(uint32_t uid,uint32_t type)
{
	switch ( type ){
		case 0 :
			msg_log_uint32(0x0B5316d6,uid);	
			break;
		case 1 :
			msg_log_uint32(0x0B5316e2,uid);	
			break;
		default :
			break;
	}
}

static inline void
msg_log_login_reward(uint32_t day , uint32_t uid)
{
	msg_log_uint32(0x0B699701+(day-3), uid);	
}

static inline void
msg_log_lucky_star_exchange(uint32_t type, uint32_t uid)
{
	msg_log_uint32(0x0B799701+type, uid);	
}

static inline void
msg_log_pet_exchange_egg(uint32_t itemid, uint32_t uid)
{
	if(itemid==310026){
		msg_log_uint32(0xbb100001, uid);	
	}else if(itemid>=310028 && itemid <= 310032 ){
		msg_log_uint32(0xbb100002+itemid-310028, uid);	
	}else if(itemid>=310041 && itemid <= 310043 ){
		msg_log_uint32(0xbb100007+itemid-310041, uid);	
	}else if(itemid>=310044 && itemid <= 310053 ){
		msg_log_uint32(0xbb100010+itemid-310044, uid);	
	}else if(itemid==350054){
		msg_log_uint32(0xbb100020, uid);	
	}else if(itemid==350012){
		msg_log_uint32(0xbb100021, uid);	
	}else if(itemid==350015){
		msg_log_uint32(0xbb100022, uid);	
	}else if(itemid==350026){
		msg_log_uint32(0xbb100023, uid);	
	}else if(itemid==350030){
		msg_log_uint32(0xbb100024, uid);	
	}else if(itemid>=350032 && itemid <= 350033 ){
		msg_log_uint32(0xbb100025+itemid-350032, uid);	
	}else if(itemid==350036){
		msg_log_uint32(0xbb100027, uid);	
	}else if(itemid==350035){
		msg_log_uint32(0xbb100028, uid);	
	}else if(itemid==350039){
		msg_log_uint32(0xbb100029, uid);	
	}else if(itemid>=350040 && itemid <= 350041 ){
		msg_log_uint32(0xbb100030+itemid-350040, uid);	
	}

}
#endif
