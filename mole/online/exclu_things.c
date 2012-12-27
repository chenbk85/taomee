#include <libtaomee/bitmanip/bitmanip.h>
#include <statistic_agent/msglog.h>

#include "homeinfo.h"
#include "party.h"
#include "small_require.h"
#include "lahm_sport.h"
#include "mole_homeland.h"
#include "market.h"
#include "login.h"
#include "message.h"
#include "tasks.h"
#include "exclu_things.h"
#include "mole_doctor.h"
#include "mole_pasture.h"
#include "mole_happy_card.h"
#include "game_pk.h"
#include "lahm_diary.h"
#include "mole_work.h"
#include "gift.h"
#include "race_car.h"
#include "mole_candy.h"
#include "hallowmas.h"
#include "thanksgiving.h"
#include "rand_itm.h"
#include "lamu_skill.h"
#include "delicous_food.h"
#include "lahm_sport.h"
#include "add_sth_rand.h"
#include "swap.h"
#include "game_bonus.h"
#include "street.h"
#include "mail.h"
#include "snowball_war.h"
#include "mole_pasture.h"
#include "mole_angel.h"
#include "chris_cake.h"
#include "tv_question.h"
#include "angel_fight.h"
#include "moon_cake.h"
#include "mole_cutepig.h"
#include "xhx_server.h"
#include "super_guider.h"
#include "christmas_sock.h"
#include "charitable_party.h"
#include "week_activity.h"
#include "year_feast.h"
#include "flower_party.h"
#include "advanced_class.h"
#include "mole_cake.h"

enum {
	EXCLU_max_things	= 1024
};

enum OutPutKind {
	EXCLU_OUT_item,
	EXCLU_OUT_pet_attr
};

static exclu_things_t all_exclu_things[EXCLU_max_things];

//------------- Callbacks for dbproxy.c Begin ---------------------
//
int set_sth_done_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t day_cnt = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, day_cnt, j);

	switch (p->waitcmd) {
	case PROTO_SET_STH_DONE:
	{
		const exclu_things_t* exclu = *((const exclu_things_t**)(p->session));

		DEBUG_LOG("STH DONE BONUS\t[uid=%u exid=%d outkind=%u]",
					p->id, exclu->exchg_id, exclu->outputkind);
		/*向导礼包统计*/
		if(exclu->exchg_id == 1502) {
			uint32_t data[] = {p->id, 0};
	        msglog(statistic_logfile, 0x202000a, now.tv_sec, data, sizeof(data));
		}
		switch (exclu->outputkind) {
		case EXCLU_OUT_item:
			if (exclu->exchg_id) {
				return do_exchange_item(p, exclu->exchg_id, 0);
			}
			response_proto_uint32_uint32(p, p->waitcmd, 0, 0, 0);
			return 0;
			break;
		case EXCLU_OUT_pet_attr:
			if (p->followed) {
				uint32_t buff[4];
				buff[0] = p->followed->id;
				buff[1] = 190592;
				buff[2] = 0;
				buff[3] = 3600;
				p->followed->logic_birthday -= 3600;
				send_request_to_db(SVR_PROTO_ADD_LAMU_GROWTH, 0, 16, buff, p->id);

				const exclu_things_t* exclu = *((const exclu_things_t**)(p->session));

				int i = sizeof(protocol_t);
				PKG_UINT32(msg, exclu->exclu_id, i);
				PKG_UINT32(msg, 1, i);
				PKG_UINT32(msg, 1001, i);
				PKG_UINT32(msg, 1, i);
				init_proto_head(msg, p->waitcmd, i);
				return send_to_self(p, msg, i, 1);
			}
			ERROR_RETURN(("Not Walking Pet Now: uid=%u", p->id), -1);
		default:
			break;
		}

		break;
	}
	case PROTO_LEARN_PET_SKILL:
	{
		uint8_t  buf[8];
		uint32_t type  = *(uint32_t*)(p->session) + 1;
		uint32_t skill = p->followed->skill;

		skill = set_bit_on32(skill, type);

		int len = 0;
		PKG_H_UINT32(buf, p->followed->id, len);
		PKG_H_UINT32(buf, skill, len);
		p->sess_len = 0;

		return db_set_pet_skill(p, buf);
	}
	case PROTO_POLL:
	{
		int idx = *((int*)(p->session));
		db_set_cnt(0, idx, 1);
		response_proto_head(p, p->waitcmd, 0);
		p->sess_len = 0;
		break;
	}
	case PROTO_LOOK_STH_CNT:
	{
		uint32_t type = *(uint32_t*)p->session;
		return db_chk_cnt(p,type);
	}
	case PROTO_VOTE_HOME:
	{
		return vote_home_flower_mud_callback(p, id, buf, len);
	}
	case PROTO_FERTILIZE_WATER_TREE:
	{
		if (!p->tiles ) // || !IS_HOUSE_MAP(p->tiles->id))
			ERROR_RETURN(("you must enter his home first"), -1);

		return db_operate_home_type_obj(p, p->session + 4, p->tiles->id);
	}
	case PROTO_VOTE_SMC:
	{
		uint32_t uid = *(uint32_t*)p->session;
		return db_operate_home_type_obj(p, p->session + 4, uid);
	}
	//to be delete
	case PROTO_SEND_FU:
	{
		return db_operate_home_type_obj(p, p->session + 4, p->tiles->id);
	}
	case PROTO_DISCOVER_RAND_ITEM:
	{
		return add_random_item(p);
	}
	case PROTO_LAHM_FOOD_MACHINE:
	{
		return do_lahm_food_mechine(p);
	}
	//end to be delete
	case PROTO_SET_MOLE_PARTY:
	{
		return db_set_mole_party(p, p->session);
	}
	case PROTO_INC_RUN_TIMES:
	{
		return db_inc_run_times(p);
	}
	case PROTO_GET_FRUIT_FREE:
	{
		return db_get_fruit_from_nbr(p, p->session, GET_UID_IN_JY(p->tiles->id));
		//return do_get_fruit_free(p);
	}
	case PROTO_SUBMIT_MAGIC_CODE:
	{
		return db_chk_if_magic_code_used(p, p->session + 1024);
	}
    case PROTO_GET_HAPPY_CARD_CLOTH:
    {
        return send_request_to_db(SVR_PROTO_GET_HAPPY_CLOTH, p, 0, NULL, p->id);
    }
    case PROTO_DOCTOR_CURE:
    case PROTO_DOCTOR_DUTY:
    {
        return handle_doctor_work(p);
    }
	case PROTO_GET_500_XIAOME:
    {
		uint32_t itmid = *(uint32_t*)p->session;
		if (!itmid) {  // give XiaoMee
        	return db_change_xiaome(p, 50, ATTR_CHG_roll_back, 0, p->id);
		} else {
			db_single_item_op(0, p->id, itmid, 1, 1);
			int j = sizeof(protocol_t);
			PKG_UINT32(msg, itmid, j);
			PKG_UINT32(msg, 1, j);
			PKG_UINT32(msg, 1, j); //count
			init_proto_head(msg, p->waitcmd, j);
			return send_to_self(p, msg, j, 1);
		}
    }
	case PROTO_PAY_MONEY:
	{
		int money = *(int*)p->session;
		return db_change_xiaome(p, -money, 0, 0, p->id);
	}
	case PROTO_SET_EGG_HATCHS:
	{
		int buff[2] = {0,1};
		return send_request_to_db(SVR_PROTO_SET_EGG_HATCH_TIMES,p,8,buff, p->id);
	}
    case PROTO_GET_LOTTERY:
    {
        return db_chk_vip_info(p);
    }
    case PROTO_SET_GAME_PK_INFO:
    {
		userid_t pkee = *(uint32_t*)p->session;
		uint32_t gid = *(uint32_t*)(p->session + 4);
		uint32_t score = *(uint32_t*)(p->session + 8);
        return db_update_pk_flag(p, gid, score, p->id, pkee);
    }
    case PROTO_EDIT_LAHM_DIARY:
    {
        return db_set_lahm_diary(p , p->session, p->id);
    }
	case PROTO_SET_WORK_NUM:
	{
		uint32_t yxb = 500;
		return db_change_xiaome(p, yxb, ATTR_CHG_roll_back, 0, p->id);
	}
	case PROTO_GET_WORK_SALARY:
	{
		return db_get_work_num(p);
	}
	case PROTO_SEND_DIARY_FLOWER:
	{
		uint32_t diary_id = *(uint32_t*)p->session;
		uint32_t uid = *(uint32_t*)(p->session + 4);
		return db_send_diary_flower(p, &diary_id, uid);
	}
	case PROTO_GGJ_EXCHANGE_ITM:
	{
		return ggj_exchg_item(p);
	}
    case PROTO_GET_SHEEP:
    {
		return db_single_item_op(p, p->id, 190351, 10, 0);
    }
	case PROTO_SEND_PET_HOME:
    {
        return do_send_pet_home(p);
    }
	case PROTO_SEND_ONE_GIFT:
    {
		send_one_t* lso = (send_one_t*)p->session;
		return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, lso->dec_len, lso->dec_buf, p->id);
    }
	case PROTO_CANDY_FROM_RECY:
	{
		return do_fetch_from_recy(p);
	}
	case PROTO_SET_QUES_NAIRE:
	{
		db_set_question_naire(0, p->session, p->sess_len, p->id);

		uint32_t type = *(uint32_t*)p->session;
		if (type != 25)
		{
		    int j = sizeof(protocol_t);
			if (type == 26)
			{
				uint32_t itemids[][2] = {{1613127, 1}};
		    	db_exchange_single_item_op(p, 202, itemids[0][0], itemids[0][1], 0);
				send_request_to_db(SVR_PROTO_CP_DAREN_VOTE, NULL, 4, p->session+12, p->id);

		   		PKG_UINT32(msg, 1, j);
		    	PKG_UINT32(msg, itemids[0][0], j);
		    	PKG_UINT32(msg, itemids[0][1], j);

				uint32_t msg_buff[2] = {p->id, 1};
				msglog(statistic_logfile, 0x0409BD8E, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
			}
			else
			{

		    	uint32_t itemids[][2] = {{190141,1},{190028,1}, {190142,1}};
		    	db_exchange_single_item_op(p, 202, itemids[0][0], itemids[0][1], 0);
		    	db_exchange_single_item_op(p, 202, itemids[1][0], itemids[1][1], 0);
		    	db_exchange_single_item_op(p, 202, itemids[2][0], itemids[2][1], 0);
		   		PKG_UINT32(msg, 3, j);
		    	PKG_UINT32(msg, itemids[0][0], j);
		    	PKG_UINT32(msg, itemids[0][1], j);
		    	PKG_UINT32(msg, itemids[1][0], j);
		    	PKG_UINT32(msg, itemids[1][1], j);
		    	PKG_UINT32(msg, itemids[2][0], j);
		    	PKG_UINT32(msg, itemids[2][1], j);
			}
		    init_proto_head(msg, p->waitcmd, j);
		    return send_to_self(p, msg, j, 1);
		}
		else
		{
		    int j = sizeof(protocol_t);
		    PKG_UINT32(msg, 0, j);
		    init_proto_head(msg, p->waitcmd, j);
		    return send_to_self(p, msg, j, 1);
		}
	}
	case PROTO_CANDY_PACKET_SEND:
		return do_send_candy_packet(p);
	case PROTO_GET_CANDY_FROM_OTHER:
	{
		return db_single_item_op(p, *(uint32_t*)(p->session + 4), 190441, *(uint32_t*)p->session, 0);
	}
	case PROTO_VIP_PATCH_WORK:
		return db_change_xiaome(p, 200, 0, 0, p->id);
	case PROTO_UPDATE_NPC_LOVELY:
	{
		int delta = *(int*)p->session;
		uint32_t indx = *(uint32_t*)(p->session + 4);
		int buff[] = {delta, indx};
		send_request_to_db(SVR_UPDATE_NPC_LOVELY, 0, 8, buff, p->id);
		p->npc_lovely[indx-1] += delta;
		int k = sizeof(protocol_t);
		PKG_UINT32(msg, indx, k);
		PKG_UINT16(msg, p->npc_lovely[indx-1], k);
		PKG_UINT32(msg, get_now_tv()->tv_sec, k);
		init_proto_head(msg, p->waitcmd, k);
		return send_to_self(p, msg, k, 1);
	}
	case PROTO_GET_VIP_INVATATION:
	{
		db_single_item_op(0, p->id, -2/*宫廷花卉*/, 1, 1);
		response_proto_uint32(p, p->waitcmd, -2, 0);
		return 0;
	}
	case PROTO_GET_FIRE_EGG:
	{
		return do_get_egg_only_one(p);
	}
	case PROTO_BUY_LIMITED_ITEM:
	{
		return do_buy_limited_item(p);
	}
	case PROTO_BEAUTY_COMPETE:
	{
		return do_beauty_dress_compete(p);
	}
	case PROTO_FISHING:
	{
	    uint32_t angel_id = 0;
		uint32_t fish_id = *(uint32_t*)p->session;
		if (fish_id != 0) {
			db_single_item_op(0, p->id, fish_id, 1, 1);
 /*           if(p->tiles && p->tiles->id == 112)
			{
			    uint32_t rand_value = rand()%100;
			    if (rand_value < 40)
			    {
			        angel_id = 1353205;
			        db_single_item_op(0, p->id, angel_id, 1, 1);
			    }
			} */
		}

		response_proto_uint32_uint32(p, p->waitcmd, fish_id, angel_id, 0);
		return 0;
	}
	case PROTO_GET_10XIAOMEE_50TIMES:
	{
		db_change_xiaome(0, 10, 0, 0, p->id);
		p->yxb += 10;
		response_proto_uint32(p, p->waitcmd, 10, 0);
		return 0;
	}
	case PROTO_OCCUPY_BOOTH:
	{
		return do_occupy_booth(p);
	}
	case PROTO_GET_CHRISTMAS_GIFTS:
	{
		return db_change_gift_num(p, 5, p->id);
	}
	case PROTO_COLLECT_RAIN_GIFT:
	{
		uint32_t itmid = *(uint32_t*)(p->session + 4);
		return db_buy_itm_freely(p, p->id, itmid, 1);
	}
	case PROTO_FETCH_JINIAN_JIAOYIN:
	{
		uint32_t itmid = *(uint32_t*)p->session;
		db_single_item_op(0, p->id, itmid, 1, 1);
		response_proto_uint32(p, p->waitcmd, itmid, 0);
		return 0;
	}
	case PROTO_SEND_RAND_ITEM:
	{
		return db_add_rand_item(p);
	}
	case PROTO_RAND_ITEM_SWAP:
	{
		uint32_t buff[2] = {0};
		uint32_t count = *(uint32_t*)(p->session + 12);

		buff[0] = 0;
		buff[1] = count;
		msglog(statistic_logfile, 0x02014300, now.tv_sec, buff, 8);
		return db_single_item_op(p,p->id,190671, count,0);
	}
	case PROTO_GIVE_NPC_SOME_ITEMS:
	{
		return do_give_npc_items(p);
	}
	case PROTO_GET_SKILL_ITME_LIBAO:
	{
		return  lamu_give_skill_item_libao(p);
	}
	case PROTO_PET_HELP_CIWEI:
	{
		uint32_t itmid = *(uint32_t*)p->session;
		db_single_item_op(0, p->id, itmid, 1, 1);
		response_proto_uint32(p, p->waitcmd, itmid, 0);
		return 0;
	}
	case PROTO_GET_PIPI_OR_DOUYA:
		add_pipi_or_douya(p);
		return 0;
	case PROTO_TANTALIZE_CAT:
		tantalize_cat_set_sth_done_callback(p);
		return 0;
	case PROTO_SUPER_LAMU_PARTY_GET:
		return super_lamu_party_get_callback(p);
	case PROTO_SET_SHOP_EVENT:
		return set_restuarant_event_callback(p,id,buf,len);
	case PROTO_ADD_LAMU_GROWTH:
		return add_lamu_growth_use_tick_callback(p, id, buf, len);
	case PROTO_ADD_STH_RAND:
		return day_limit_callback(p, day_cnt);
	case PROTO_EXCHANGE_STH:
		return swap_day_limit_callback(p);
	case PROTO_FIRE_CUP_ADD_GAME_BONUS:
		return game_bonus_day_limit_callback(p);
	case PROTO_SNOWBALL_GET_GAME_BONUS:
		return snowball_game_bonus_day_limit_callback(p);
	case PROTO_SNOWBALL_GET_GAME_NEW_BONUS:
		return snowball_game_new_bonus_day_limit_callback(p);
	case PROTO_GAIN_MAP_POS_EGG:
		return gain_map_pos_egg(p);
	case PROTO_GAIN_USE_SKILL_PRIZE_ITEM:
		return gain_use_skill_prize_item(p);
	case PROTO_GAIN_GAME_ANGLES:
		return gain_game_angles(p);
	case PROTO_GAIN_WEEKEND_BONUS:
		return get_weekend_login_bonus(p);
	case PROTO_SAVE_MONSTER_GAME:
		return save_monster_game(p);
	case PROTO_ANSWER_TV_QUESTION:
		return answer_tv_question_callback(p, id, buf, len);
	case PROTO_CP_EXCHANGE_PIG:
		return cutepig_exchange_pig_sth_done_back(p);
	case PROTO_XHX_ADD_ITEMS:
		return xhx_check_add_items_callback(p, id, buf, len);
	case PROTO_CP_GET_BEAUTY_GAME_PRIZE:
		return cutepig_check_beauty_game_callback(p, id, buf, len);
	case PROTO_CP_NPC_WEIGHT_PK:
		return cutepig_npc_weight_pk_check_callback(p, id, buf, len);
	case PROTO_GUESS_DATE_ITEM_PRICE:
		return guess_item_price_day_limit_callback(p);
	case PROTO_FP_GET_AWARD:
		return fp_get_award_day_limit_callback(p);
	case PROTO_AC_CHANGE_FLAG:
		return ac_change_flag_award_day_limit_callback(p, id);
	default:
		ERROR_RETURN(("Unexpected WaitCmd %d uid=%u", p->waitcmd, p->id), -1);
	}

	return 0;
}
//------------- Callbacks for dbproxy.c End ---------------------

int set_month_task_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t ex_type = *(uint32_t*)buf;
	if (ex_type > 47)
		ERROR_RETURN(("bad month taskid \t[%u %u]", p->id, ex_type), -1);
	p->month_task[ex_type] = 1;
	switch (p->waitcmd) {
	case PROTO_GET_SMC_SALARY:
		return db_get_profession(p, p->id);
	case PROTO_ENGLISH_CAN_GET_XIAOME:
		return db_change_xiaome(p, 1000, ATTR_CHG_roll_back, 0,  p->id);
	default:
		ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}
}

//-------------------------------------------------------------
// For proto.c
//
int set_sth_done_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t type;
	if (unpkg_uint32(body, bodylen, &type) == -1) {
		return -1;
	}

	const exclu_things_t* exclu = get_exclu_thing(type);
	if (exclu && (!VIP_ONLY_EXCLU_THING(exclu) || ISVIP(p->flag)) && (check_vip_level(p, exclu->vip_level))) {
		//
		((const exclu_things_t**)(p->session))[0] = exclu;
		p->sess_len = sizeof exclu;
		//
		DEBUG_LOG("SET STH DONE\t[uid=%u type=%u]", p->id, type);

		if (exclu->id >= 23 && exclu->id <= 26)
		{
	        msglog(statistic_logfile, 0x04020500+exclu->vip_level, now.tv_sec, &(p->id), 4);
		}
		else if (exclu->id >= 27 && exclu->id <= 30)
		{
	        msglog(statistic_logfile, 0x0409B482+exclu->vip_level-5, now.tv_sec, &(p->id), 4);
		}

		return db_set_sth_done(p, exclu->exclu_id, 1, p->id);
	}
	ERROR_RETURN(("invalid type: uid=%u flag=%X type=%u, viplelve=%u", p->id, p->flag, type, p->vip_level), -1);
}

int get_month_tasks_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
typedef struct {
	uint32_t type;
	uint32_t cnt;
}month_task_t;

	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + 8 * count);

	int loop;
	month_task_t* mtt = (month_task_t*)(buf + 4);
	for (loop = 0; loop < count && loop < MAX_MONTH_TASKS; loop++, mtt++) {
		if (mtt->type >= MAX_MONTH_TASKS)
			continue;
		p->month_task[mtt->type] = mtt->cnt;
	}

	if (!p->month_task[0] && have_profession(p)) {
		char txt[256];
#ifdef TW_VER
		snprintf(txt, sizeof txt, "親愛的%s，感謝你一個月的辛勤工作。上月的SMC職業薪資已經幫你存入銀行的SMC專用提款機了，趕快去銀行領取吧！", p->nick);
		send_postcard("洛克行政官", 0, p->id, 1000059, txt, 0);
#else
		snprintf(txt, sizeof txt, "亲爱的%s，感谢你一个月的辛勤劳动，洛克行政官给你发放了上个月的工资，赶快去银行SMC专用提款机提取吧！", p->nick);
		send_postcard("洛克行政官", 0, p->id, 1000059, txt, 0);
#endif
		db_set_month_task_done(0, 0, 1, p->id);
		p->month_task[0] = 1;
	}

	if(ISVIP_BEFORE(p->flag)) {
		//send_mail_to_self(p, 1, 1000173);
	}
	if(ISVIP(p->flag)) {
		//send_mail_to_self(p, 1, 1000264);
	}

	send_mail_to_self(p, 1, 1000438);
	send_mail_to_self(p, 1, 1000440);
	send_mail_to_self(p, 1, 1000442);

    if(set_only_one_bit(p, 217)){
		
		send_mail_to_self(p, 1, 1000441);
	}

	if ((p->id >= 240001000 && p->id <= 240976689)
        || (p->id >= 241000100 && p->id <= 241201966)
        || (p->id >= 241400010 && p->id <= 241601554)
		|| (p->id == 202608883 || p->id == 99655314))
    {
        if(set_only_one_bit(p, 111))
        {
            send_postcard("淘米校巴", 0, p->id, 1000227, "", 0);
            db_exchange_single_item_op(p, 202, 1230048, 1, 0);
            uint32_t data[] = {p->id, 1};
	        msglog(statistic_logfile, 0x0A010001, now.tv_sec, data, sizeof(data));
	    }
	}

	db_login_get_cnt_list(p, START_DAY_LIMIT, END_DAY_LIMIT);

	
	struct tm tm_tmp;
	time_t t = p->birthday;
	localtime_r(&t, &tm_tmp);

	uint32_t t_month_day = (get_now_tm()->tm_mon + 1)*30 + get_now_tm()->tm_mday;
	uint32_t b_month_day = (tm_tmp.tm_mon+ 1)*30 + tm_tmp.tm_mday;
	if ( (b_month_day < t_month_day) && (t_month_day - b_month_day <= 7))
	{
		if(set_only_one_bit(p, 211)){
			send_postcard("奇奇", 0, p->id, 1000316, "", 0);
		}
	}

	uint32_t t_day = (get_now_tm()->tm_mon + 1)*100 + get_now_tm()->tm_mday;
	uint32_t b_day = (tm_tmp.tm_mon+ 1)*100 + tm_tmp.tm_mday;
	if (b_day == t_day)
	{
		if(set_only_one_bit(p, 213)){
			send_postcard("奇奇", 0, p->id, 1000320, "", 0);
		}
	}

#ifndef TW_VER
	if(set_only_one_bit(p, 215) && (get_today() >= 20120529 && get_today() <= 20120607)){
		char txt[256];
		int len = snprintf(txt, sizeof txt, "经过庄园人口普查，有部分摩尔成为消失人口，我们将于2012年6月7日，清除这些在2年内从未登录过、且不是超级拉姆的小摩尔账户数据。");
	    new_notify_system_message(p, txt, len, 0);
	}
	
	    if(set_only_one_bit(p, 216) && (get_today() >= 20120619 && get_today() <= 20120622)){
			char txt[256];
			int len = snprintf(txt, sizeof txt, "由于机器合作方技术故障，导致部分小摩尔在6月18日收益损失。为表示最诚挚的歉意，在6.21-8.21期间登录，可通过邮件领取补偿礼包！");
			new_notify_system_message(p, txt, len, 0);									
		}

#endif

	return 0;
}

//--------------------------------------------------------------------------

// Exclusive Things Conf File Parser
static int load_exclusive_things_config(xmlNodePtr cur_node);

//------------------ Parse Exclusive Things Conf File Begin ------------------
//
int  load_exclusive_things(const char* file)
{
	int err = -1;

	xmlDocPtr doc = xmlParseFile(file);
	if (!doc) ERROR_RETURN(("Failed to Load Exclusive Things Config"), -1);

	xmlNodePtr cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG("xmlDocGetRootElement error");
		goto exit;
	}

	err = load_exclusive_things_config(cur);
exit:
	xmlFreeDoc(doc);
	BOOT_LOG(err, "Load Exclusive Things File %s", file);
}
//
void destroy_exclusive_things()
{
}
//
static int load_exclusive_things_config(xmlNodePtr cur_node)
{
	int excluid;
	exclu_things_t* exclu;

	cur_node = cur_node->xmlChildrenNode;
	while (cur_node) {
		if (!xmlStrcmp(cur_node->name, (const xmlChar *)"Thing")) {
			DECODE_XML_PROP_INT(excluid, cur_node, "ID");
			if (excluid >= EXCLU_max_things) {
				ERROR_RETURN( ("Failed to Parse Event Conf File, EventID=%u", excluid), -1 );
			}

			exclu     = &(all_exclu_things[excluid]);
			exclu->id = excluid;
			DECODE_XML_PROP_INT_DEFAULT(exclu->exclu_id, cur_node, "ExcluID", excluid);
			DECODE_XML_PROP_INT_DEFAULT(exclu->exchg_id, cur_node, "RelatedExchgID", 0);
			decode_xml_prop_uint32_default(&(exclu->outputkind), cur_node, "OutPutKind", EXCLU_OUT_item);
			decode_xml_prop_uint32_default(&(exclu->flag), cur_node, "Flag", 0);
			decode_xml_prop_uint32_default(&(exclu->vip_level), cur_node, "VipLevel", 0);
		}
		cur_node = cur_node->next;
	}
	return 0;
}

const exclu_things_t* get_exclu_thing(int idx)
{
	if ((idx < EXCLU_max_things) && (all_exclu_things[idx].id == idx)) {
		return &(all_exclu_things[idx]);
	}
	return 0;
}

int chk_if_sth_done_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	if(p->waitcmd == PROTO_GET_SHOP_EVENT) {
		return get_restuarant_event_callback(p,id,buf,len);
	} else if (p->waitcmd == PROTO_GET_TV_QUESTION) {
		return get_tv_question_callback(p, id, buf, len);
	} else if (p->waitcmd == PROTO_AF_DEAL_FRIEND_EVENT) {
		return af_check_friend_event_callback(p, id, buf, len);
	} else if (p->waitcmd == PROTO_USER_EAT_MOONCAKE) {
		return user_get_eat_mooncake_callback(p, id, buf, len);
	} else if (p->waitcmd == PROTO_XHX_CHECK_USER) {
		return user_check_entry_xhx_callback(p, id, buf, len);
	} else if (p->waitcmd == PROTO_SG_GET_PATROL_TESK) {
		return sg_get_patrol_tast_callback(p, id, buf, len);
	} else if (p->waitcmd == PROTO_CP_GET_BEAUTY_LIMIT_GIFT) {
		return cutepig_get_beauty_game_gift_callback(p, id, buf, len);
	} else if (p->waitcmd == PROTO_CP_BEAUTY_SHOW_WITH_NPC) {
		return cp_beauty_show_check_use_npc_pig_callback(p, id, buf, len);
	} else if (p->waitcmd == PROTO_GET_CHRISTMAS_SOCK_GIFTBOX) {
		return get_christmas_sock_giftbox_callback(p, id, buf, len);
	} else if (p->waitcmd == PROTO_DONATE_FOOD_ITEM) {
		return user_check_donate_food_item_cnt_callback(p, id, buf, len);
	} else if (p->waitcmd == PROTO_YEAR_FEAST_GET_PRIZE) {
		return year_feast_check_day_limit_callback(p, id, buf, len);
	}
	else if(p->waitcmd == PROTO_CP_USER_PIGLET_USE_ACCELERATE_TOOL){
		uint32_t cnt = *(uint32_t*)buf;
		typedef struct acc_machine_tool{
			uint32_t uid;
			uint32_t toolid;
			uint32_t type;
			uint32_t index;
		}acc_mh_t;
		acc_mh_t* cur = (acc_mh_t*)p->session;
		DEBUG_LOG("cnt: %u, toolid: %u", cnt, cur->toolid);
		if(cur->toolid == 1614011 ||cur->toolid == 1614009 || cur->toolid == 1614010){
			if(cnt >= 20){
				return send_to_self_error(p, p->waitcmd, -ERR_use_glue_to_machine_day_limit, 1);
			}
		}

		*(uint32_t*)(p->session + sizeof(acc_mh_t)) =  cnt;
		uint32_t db_buff[]={cur->toolid, cur->type, cur->index};
		return  send_request_to_db(SVR_PROTO_USER_USE_ACCELERATE_TOOL, p, sizeof(db_buff), db_buff, cur->uid);
		//return cute_piglet_get_use_special_acc_tool_cnt_callback(p, id, buf, len);
	}
	else if(p->waitcmd == PROTO_VOTE_KFC_FRIEND_DECORATION){
		return vote_KFC_friend_decorate_callback(p, id, buf, len);

	}
	else if(p->waitcmd == PROTO_GET_MAKE_BIRTHDAY_CAKE_AWARD){
		return get_make_birthday_cake_award_callback(p, id, buf, len);

	}
	else if(p->waitcmd == PROTO_EXCHANGE_STH){
		return exchange_check_if_set_something_done(p, id, buf, len);
	}
	else if(p->waitcmd == PROTO_GET_RANDOM_DANCING_AWARD){
		return give_award_crystal_dancing_ball(p, id, buf, len);
	}
	else if(p->waitcmd == PROTO_CHECK_DANCING_THREE_TIMES){
		return get_part_in_three_times_dancing(p, id, buf, len);
	}
	else if(p->waitcmd == PROTO_SET_MOLE_MISS_NOTE){
		return get_mole_miss_note_day_times(p, id, buf, len);
	}
	else if(p->waitcmd == PROTO_USER_WATER_JACK_AND_MODOU ){
		CHECK_BODY_LEN(len ,4);
		*(uint32_t*)p->session = *(uint32_t*)buf;
		return  send_request_to_db(SVR_PROTO_SYSARG_USER_WATER, p, 0, NULL, p->id);
	}
	else if(p->waitcmd == PROTO_MOLE_GET_MAKE_SHIP){
		CHECK_BODY_LEN(len ,4);
		uint32_t today_cnt = *(uint32_t*)buf;
		if(today_cnt >= 10){
			return send_to_self_error(p, p->waitcmd, -ERR_have_got_day_limit, 1);
		}
		return send_request_to_db(SVR_PROTO_GET_MAKE_SHIP, p, 0, NULL, p->id);
	}
	else if(p->waitcmd == PROTO_MOLE_GET_INVADE_MONSTER){
		return mole_get_invade_monster_callback(p, id, buf, len);
	}
	else if(p->waitcmd == PROTO_MOLE_SET_OCEAN_ADVENTURE){
		CHECK_BODY_LEN(len, 4);
		uint32_t today_cnt = *(uint32_t*)buf;
		if(today_cnt < 20 ){
			*(uint32_t*)(p->session+8) = 0;
		}
		else{
			*(uint32_t*)(p->session+8) = 1;
		}
		uint32_t db_buff[2] = { *(uint32_t*)p->session, *(uint32_t*)(p->session+4) };
		return send_request_to_db(SVR_PROTO_SET_OCEAN_ADVENTURE, p, 8, db_buff, p->id);
	}

	CHECK_BODY_LEN(p->sess_len, 4);
	DEBUG_LOG("chk if sth done:type[%u] count[%u]", *(uint32_t*)p->session, *(uint32_t*)buf);
	int i = sizeof(protocol_t);
	PKG_STR(msg, p->session, i, p->sess_len);
	PKG_UINT32(msg, *((uint32_t*)buf), i);
	init_proto_head(msg, p->waitcmd, i);
	p->sess_len = 0;
	return send_to_self(p, msg, i, 1);

}

/* @breif change all of one id to another id, delete this id
 */
int exchange_thing_to_another(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	struct exchange_id {
		uint32_t itemid_in;
		uint32_t itemid_out;
	};
	struct count_exchange_id {
		uint32_t count;
		struct exchange_id exchange[5];
	};
	struct count_exchange_id exchange_info[] = {
									{2,{{190295, 1270041},{190296, 1270040}}},
								 };
	int k = 0;
	uint32_t array_index = 0;
	UNPKG_UINT32(body, array_index, k);

	if (array_index > (sizeof(exchange_info) / sizeof(exchange_info[0]))) {
		ERROR_RETURN(("wrong array index %u %u", p->id, array_index), -1);
	}
	/*which table, count*/
	int i = 0;
	uint32_t db_buf[48] = {0, exchange_info[array_index -1].count};
	for (i = 0; i < exchange_info[array_index -1].count; i++) {
		db_buf[2 + 2 * i ] = exchange_info[array_index -1].exchange[i].itemid_in;
		db_buf[2 + 2 * i + 1] = exchange_info[array_index -1].exchange[i].itemid_out;
	}

	uint32_t db_len = sizeof(struct exchange_id) * exchange_info[array_index -1].count + 2 * 4;
	return send_request_to_db(SVR_PROTO_EXCHANGE_ALL_TO_ANOTHER, p, db_len, db_buf, p->id);
}

/* breif callback for exchange thing
 */
int exchange_thing_to_another_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/* @brief get item by skill li bao every week
 */
int lamu_get_skill_item_libao(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	if (p->followed == NULL) {
		ERROR_RETURN(("not followed pet %u", p->id), -1);
	}
	if (check_three_basic_skill(p->followed)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_study_basic_skills, 1);
	}
	if (!PET_IS_SUPER_LAHM(p->followed)) {
		return send_to_self_error(p, p->waitcmd, -ERR_no_super_lahm, 1);
	}
	return db_set_sth_done(p, 1000000014, 1, p->id);
}

int lamu_help_ciwei_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	/*check pet follow*/
	if (p->followed == NULL) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_lahm_not_followed, 1);
	}

	if (PET_IS_SUPER_LAHM(p->followed)) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_pet_is_super_lamu, 1);
	}

	/*check action is water*/
	if (p->lamu_action == 0 || ((p->lamu_action - 1) % 3) != 1) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_action_not_fit, 1);
	}

	/*check lahm is water*/
	if (!(p->followed->skill_bits & 2)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_water_lahm, 1);
	}

	*(uint32_t*)p->session = 1270045;
	return db_set_sth_done(p, 1000000023, 1, p->id);
}

/* @brief give item get by skill
 */
int lamu_give_skill_item_libao(sprite_t* p)
{
	return do_exchange_item(p, 1614, 0);
}

int get_yymmdd(time_t t)
{
	struct tm tm_tmp;
	localtime_r(&t, &tm_tmp) ;
	return (tm_tmp.tm_year+1900)*10000+(tm_tmp.tm_mon+1)*100+tm_tmp.tm_mday;
}
