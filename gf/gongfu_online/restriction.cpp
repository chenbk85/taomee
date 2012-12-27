#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/random/random.hpp>

using namespace taomee;

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/project/stat_agent/msglog.h>
}

#include "utils.hpp"
#include "item.hpp"
#include "login.hpp"
#include "task.hpp"
#include "cli_proto.hpp"
#include "restriction.hpp"
#include "ambassador.hpp"
#include "apothecary.hpp"
#include "player.hpp"
#include "global_data.hpp"
#include "common_op.hpp"

//restriction_t g_all_restr[max_restr_count];

static void do_stat_log_caiji(uint32_t userid, uint32_t action_id);

void do_stat_log_caiji(uint32_t userid, uint32_t action_id)
{
	uint32_t msg_content = 1;
	
	switch(action_id)
	{
		case  2001://井水抽取
		{
			msglog(statistic_logfile,  stat_log_jinshui_caiji,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}
		break;

		case 2002://壁虎尾巴
		{
			msglog(statistic_logfile,   stat_log_pihuweiba,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}
		break;

		case 2003://一气学院掉落
		case 2013:
		case 2023:
		{
			msglog(statistic_logfile,   stat_log_yiqixueyuan_caiji,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}
		break;

		case 2005://矿山镇掉落
		case 2015:
		case 2025:
		case 2035:
		case 2045:
		case 2055:
		{
			msglog(statistic_logfile,  stat_log_kuangshazheng_caiji,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}
		break;

		case 2006://十字路口
		case 2016:
	    case 2026:
		case 2036:
		case 2046:
		{
			msglog(statistic_logfile,  stat_log_shizilukou_caiji,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}		
		break;

		case 2017://功夫城
		case 2027:
		case 2037:
		case 2047:
		{
			msglog(statistic_logfile,  stat_log_gongfucheng_caiji,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}
		break;

		case 2008://功夫城西
		case 2018:
		case 2028:
		case 2038:
		case 2048:
		{
			msglog(statistic_logfile,  stat_log_gongfuchengxi_caiji,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}
		break;

		case 2009://奇面族营地
		case 2019:
		case 2029:
		{
			msglog(statistic_logfile,  stat_log_qimianzu_caiji,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}
		break;

		case 2061://铸剑谷
		case 2062:
		{
			msglog(statistic_logfile,   stat_log_zhujiangu_caiji,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}
		break;


		case 2063://虎丘
		case 2064:
		case 2065:
		case 2066:
		case 2067:
		{
			msglog(statistic_logfile,  stat_log_huqiu_caiji,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}
		break;
	}
}

void do_stat_log_exchange(uint32_t uid, uint32_t role_type, uint32_t act_id, uint32_t itemid)
{
    uint32_t buf[1] = {0};
    buf[0] = 1;
    uint32_t id = 0;
    switch (act_id) {
        case 454:
            id = (itemid - 100000 * role_type) + 0x4000 + (role_type << 12);
            break;
        case 455:
        case 456:
        case 457:
        case 458:
        case 468:
        case 467:
            id = 0x3200 + (itemid - 1300000);
            break;
        case 459:
        case 460:
        case 461:
        case 462:
        case 469:
            id = 0x4000 + (itemid - 1700000);
            break;
        default:
            return;
    }
    msglog(statistic_logfile, stat_log_exchange_base_cmd+id, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("do stat exchange: cmd=[%x] uid=[%u] act_id=[%u] itemid=[%u]",
        stat_log_exchange_base_cmd, uid, act_id, itemid);
}

//------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------
void build_restriction_list(player_t* p, uint32_t count, const struct db_res_elem_t* db_res)
{
	memset(p->daily_restriction_count_list, 0x00, sizeof(p->daily_restriction_count_list));	
	for (uint32_t i = 0; i < count; i++) {
		p->daily_restriction_count_list[db_res->id - 1] = db_res->cnt;
		TRACE_LOG("restr list:[%u %u %u]",p->id, db_res->id, p->daily_restriction_count_list[db_res->id - 1]);
		db_res++;
	}
}

inline bool pack_res_swap_item(player_t* p, uint32_t resid, uint32_t role_type, restriction_t* p_out_res, restriction_t* p_out_mail)
{
	restriction_t res_total = { 0 };
	restriction_t* p_total = &res_total;
	restriction_t* p_res = &(g_all_restr[resid - 1]);
	
	memcpy(p_out_res, &(g_all_restr[resid - 1]), sizeof(*p_out_res));
	memcpy(p_out_mail, &(g_all_restr[resid - 1]), sizeof(*p_out_mail));
	memcpy(p_total, &(g_all_restr[resid - 1]), sizeof(*p_total));
	TRACE_LOG("%u %u", p_out_res->id, p_out_res->userflg);
	
	p_out_res->rew_count = 0;
	p_total->rew_count = 0;
	p_out_mail->rew_count = 0;
	
	uint32_t rand_num = rand() % 1000;
	uint32_t odds = 0;
	uint32_t j = 0;
	uint32_t reward_item_cnt = 0;
	uint32_t reward_attire_cnt = 0;

    for (uint32_t i = 0; i < p_res->rew_count; i++) {
        TRACE_LOG("role_type odds : [%u %u]",p_res->rewards[i].role_type, p_res->rewards[i].odds);
        // role_type limit but wrong role_type
        if (((p_res->rewards[i].role_type != 0) && (p_res->rewards[i].role_type != role_type)) ||
				(p->lv >= max_exp_lv && p_res->rewards[i].give_type == give_type_player_attr && p_res->rewards[i].give_id == give_type_exp)) {
			 if (p->lv >= max_exp_lv && p_res->rewards[i].give_type == give_type_player_attr && p_res->rewards[i].give_id == give_type_exp) {
			 	send_header_to_player(p, p->waitcmd, cli_err_max_exp_lv, 1);
			 	return false;
			 }
			 continue;
        } else {
            //odds += p_res->rewards[i].odds;
            if (p_res->rewards[i].odds != 0) {
                odds += p_res->rewards[i].odds;
                TRACE_LOG("rand item1 :[%u %u %u]", rand_num, odds, p_res->rewards[i].odds);
                if (rand_num < odds) {
					uint32_t idx = i;
					if (resid == 233) {
						if (!(g_fanfan_mrg.judge_item_can_out_put(p_res->rewards[i].give_id))) {
							idx = 3;
							if (idx > (p_res->rew_count - 1) ) {
								idx = 0;
							}
						}
                	}
					
					if (p_res->rewards[idx].give_type == give_type_normal_item) {
						reward_item_cnt ++;
					} else if (p_res->rewards[idx].give_type == give_type_clothes) {
						reward_attire_cnt ++;
					}
                    p_total->rew_count++;

					p_total->rewards[j].give_id = p_res->rewards[idx].give_id;

					/*
					const GfItem* itm = items->get_item(p_res->rewards[idx].give_id);
					if (itm && is_valid_montype(itm->summon_id()) ) {
						if (check_summon_type_exist(p, itm->summon_id()) || 
							p->my_packs->is_item_exist(p_res->rewards[idx].give_id)) {
							p_total->rewards[j].give_id = 1302001;
						}
					}
					*/
                    p_total->rewards[j].role_type = p_res->rewards[idx].role_type;
                    p_total->rewards[j].give_type = p_res->rewards[idx].give_type;
                    p_total->rewards[j].count = p_res->rewards[idx].count;

                    TRACE_LOG("rand item2 :[%u %u %u]",p_total->rewards[j].give_id, 
                        p_total->rewards[j].give_type, p_total->rewards[j].count);
                    j++;
                    break;
                } else {
                    continue;
                }
            } else {
				if (p_res->rewards[i].give_type == give_type_normal_item) {
					reward_item_cnt ++;
				} else if (p_res->rewards[i].give_type == give_type_clothes) {
					reward_attire_cnt ++;
				}

                p_total->rew_count++;
                p_total->rewards[j].role_type = p_res->rewards[i].role_type;
				p_total->rewards[j].give_id = p_res->rewards[i].give_id;

				/*
				if (p_res->rewards[i].give_type == give_type_normal_item) {
					const GfItem* itm = items->get_item(p_res->rewards[i].give_id);
					if (itm && is_valid_montype(itm->summon_id()) ) {
						if (check_summon_type_exist(p, itm->summon_id()) ||
							p->my_packs->is_item_exist(p_res->rewards[i].give_id)) {
							p_total->rewards[j].give_id = 1302001;
						}
					}
					
				}*/
                p_total->rewards[j].give_type = p_res->rewards[i].give_type;
                p_total->rewards[j].count = p_res->rewards[i].count;
                j++;
            }
        }
    }

	uint32_t all_item_cnt = get_player_total_item_bag_grid_count(p);
	uint32_t cur_item_cnt = p->my_packs->all_items_cnt();
	uint32_t free_item_bag = all_item_cnt > cur_item_cnt ? all_item_cnt - cur_item_cnt : 0;
	uint32_t all_attire_cnt = get_player_total_item_bag_grid_count(p);
	uint32_t cur_attire_cnt = p->my_packs->all_clothes_cnt();
	uint32_t free_attire_bag = all_attire_cnt > cur_attire_cnt ? all_attire_cnt - cur_attire_cnt : 0;

	TRACE_LOG("%u %u %u %u %u", free_item_bag, free_attire_bag, all_attire_cnt, reward_item_cnt, reward_attire_cnt);
	if ( free_item_bag < reward_item_cnt || free_attire_bag < reward_attire_cnt ) {
		uint32_t mail_flag = 1;
		/*
		uint32_t in_item_cnt = reward_item_cnt;
		for (uint32_t i = 0; i < p_total->rew_count; i++) {
			if (p_total->rewards[i].give_type == give_type_clothes) {
				mail_flag = 1;
				break;
			} else if (p_total->rewards[i].give_type == give_type_normal_item) {
				const GfItem* itm = items->get_item(p_total->rewards[i].give_id);
				uint32_t item_max = itm->max();
				if (p->my_packs->get_item_cnt(p_total->rewards[i].give_id) + p_total->rewards[i].count > item_max) {
					mail_flag = 1;
					break;
				}
			}
		}
*/
		if (mail_flag) {
			for (uint32_t i = 0; i < p_total->rew_count; i++) {
				if (((p_total->rewards[i].give_type != give_type_normal_item && 
					p_total->rewards[i].give_type != give_type_clothes)) ||
					(p_total->rewards[i].give_id >= 1700009 && p_total->rewards[i].give_id <= 1700018)
                    || is_strengthen_material(p_total->rewards[i].give_id) ) {
					
					p_out_res->rewards[p_out_res->rew_count].role_type = p_total->rewards[i].role_type;
					p_out_res->rewards[p_out_res->rew_count].give_id = p_total->rewards[i].give_id;
					p_out_res->rewards[p_out_res->rew_count].give_type = p_total->rewards[i].give_type;
					p_out_res->rewards[p_out_res->rew_count].count = p_total->rewards[i].count;
					
					TRACE_LOG("RESOUT : %u %u", p_out_res->rewards[p_out_res->rew_count].give_id,
						p_out_res->rewards[p_out_res->rew_count].count);
					p_out_res->rew_count++;
				} else {		
					p_out_mail->rewards[p_out_mail->rew_count].role_type = p_total->rewards[i].role_type;
					p_out_mail->rewards[p_out_mail->rew_count].give_id = p_total->rewards[i].give_id;
					p_out_mail->rewards[p_out_mail->rew_count].give_type = p_total->rewards[i].give_type;
					p_out_mail->rewards[p_out_mail->rew_count].count = p_total->rewards[i].count;
					
					TRACE_LOG("MAIL : %u %u", p_out_mail->rewards[p_out_mail->rew_count].give_id,
						p_out_mail->rewards[p_out_mail->rew_count].count);
					p_out_mail->rew_count++;
				}
			}
		} else {
			memcpy(p_out_res, p_total, sizeof(*p_out_res));
		}

	} else {
		memcpy(p_out_res, p_total, sizeof(*p_out_res));
	}
	return true;
}

int send_recv_restrict_action_reward(player_t* p, const restriction_t* p_restr, const restriction_t* p_res_mail)
{
	int idx = sizeof(cli_proto_t);

	uint32_t item_cnt = 0;
	int item_cnt_idx = idx;
	idx += 4;
	for (uint32_t i = 0; i < p_restr->rew_count; i++) {

		pack(pkgbuf, p_restr->rewards[i].give_id, idx);
		pack(pkgbuf, p_restr->rewards[i].count, idx);

		TRACE_LOG("restr reward item:[%u %u %u]",p->id, p_restr->rewards[i].give_id,
								p_restr->rewards[i].count);
		item_cnt++;
	}
	for (uint32_t i = 0; i < p_res_mail->rew_count; i++) {

		pack(pkgbuf, p_res_mail->rewards[i].give_id, idx);
		pack(pkgbuf, p_res_mail->rewards[i].count, idx);

		TRACE_LOG("restr reward item:[%u %u %u]",p->id, p_res_mail->rewards[i].give_id,
								p_res_mail->rewards[i].count);
		item_cnt++;
	}

	pack(pkgbuf, item_cnt, item_cnt_idx);
	
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
	
}


int send_restrict_action_rsp_to_client(player_t* p, uint32_t restr_id)
{
	int idx = sizeof(cli_proto_t);
	
	TRACE_LOG("restr id:[%u %u %u]",p->id, restr_id, p->daily_restriction_count_list[restr_id - 1]);
	
	restriction_t* p_restr = (restriction_t*)(p->session);//&(g_all_restr[restr_id - 1]);
	const swap_item_rsp_t* swap_item_rsp = reinterpret_cast<const swap_item_rsp_t*>(p->session + sizeof(*p_restr) * 2);
    
    uint32_t gold_coin_count = 0;

	pack(pkgbuf, p_restr->type, idx);
	pack(pkgbuf, restr_id, idx);
	//update my packs
	for (uint32_t i = 0; i < p_restr->cost_count; i++) {
		if (p_restr->costs[i].give_type == give_type_normal_item) {
			p->my_packs->del_item(p, p_restr->costs[i].give_id, p_restr->costs[i].count, channel_string_active);

			if (p_restr->costs[i].give_id == 1500340) { 
                do_stat_log_universal_interface_1(stat_log_element_cost, 0, p_restr->rewards[i].count);
            }
			if (p_restr->costs[i].give_id == 1500344) { 
                do_stat_log_universal_interface_2(stat_log_lingli_neihe, 0, 0, p_restr->rewards[i].count);
            }
		} else if (p_restr->costs[i].give_type == give_type_clothes) {
			p->my_packs->del_clothes_by_id(p_restr->costs[i].give_id, channel_string_active);
		}
		if (p_restr->costs[i].give_type == give_type_player_attr) {
			if (p_restr->costs[i].give_id == give_type_expliot) {
				p->exploit = p->exploit > p_restr->costs[i].count ? 
					p->exploit - p_restr->costs[i].count : 0;
			}
		}
		KDEBUG_LOG(p->id, "ACTION COST\t[type=%u id=%u cnt=%u channel=%s res_id=%u]", 
				p_restr->costs[i].give_type, p_restr->costs[i].give_id, p_restr->costs[i].count, channel_string_active, p_restr->id);

        gold_coin_count += (get_feedback_coin_cnt(p_restr->costs[i].give_id) * p_restr->costs[i].count);
	}
	
	uint32_t item_cnt = 0;
	int item_cnt_idx = idx;
	idx += 4;
	for (uint32_t i = 0; i < p_restr->rew_count; i++) {
		//add exploit to player
		if (p_restr->rewards[i].give_type == give_type_player_attr) {
			if (p_restr->rewards[i].give_id == give_type_expliot) {
				p->exploit += p_restr->rewards[i].count;
			} else if (p_restr->rewards[i].give_id == give_type_apothecary_exp) {
				 //add exp for secondary profession
				 uint32_t fact_add_exp = 0;
				 add_player_secondary_professoin_exp(p, apothecary_profession_type, p_restr->rewards[i].count, &fact_add_exp);
				 p_restr->rewards[i].count = fact_add_exp;
			}  
		}
		if (p_restr->rewards[i].give_type == give_type_normal_item) {
            if (restr_id == 398 || (restr_id >= 28 && restr_id <= 33) || (restr_id >= 655 && restr_id <= 661) 
                || restr_id == 36 || restr_id == 37 || (restr_id >= 679 && restr_id <= 681)
                || (restr_id >= 383 && restr_id <= 390) )
                p->my_packs->add_item(p, p_restr->rewards[i].give_id, p_restr->rewards[i].count, channel_string_active, true, vip_receive);
            else 
                p->my_packs->add_item(p, p_restr->rewards[i].give_id, p_restr->rewards[i].count, channel_string_active, true, swap_action);
			//if the item is relive item then log it
			stat_log_relive_item(p_restr->rewards[i].give_id, p_restr->rewards[i].count);
            // if the item is coupons item then log it
            //if (p_restr->rewards[i].give_id == 1700061) { 
            //    do_stat_log_universal_interface_1(stat_log_get_coupons_item, 0, p_restr->rewards[i].count);
            //}
			if (p_restr->rewards[i].give_id == 1500339) { 
                do_stat_log_universal_interface_2(stat_log_peach_got, 0, p_restr->rewards[i].count, 0);
            }
				
			if (restr_id == 233) {
				g_fanfan_mrg.add_item_out_put_cnt(p_restr->rewards[i].give_id, p_restr->rewards[i].count);
			}
		}

		
		if (p_restr->rewards[i].give_type == give_type_normal_item 
			|| p_restr->rewards[i].give_type == give_type_player_attr) {
			pack(pkgbuf, p_restr->rewards[i].give_id, idx);
			pack(pkgbuf, p_restr->rewards[i].count, idx);

			
			
			TRACE_LOG("restr reward item:[%u %u %u]",p->id, p_restr->rewards[i].give_id,
									p_restr->rewards[i].count);
            do_stat_log_fumo_consume(p_restr->rewards[i].give_id, p->lv, p->id, p_restr->costs[i].count);
			item_cnt++;
		}
		KDEBUG_LOG(p->id, "ACTION REWARD\t[type=%u id=%u cnt=%u channel=%s res_id=%u]", 
			p_restr->rewards[i].give_type, p_restr->rewards[i].give_id, p_restr->rewards[i].count, channel_string_active, p_restr->id);
	}
	pack(pkgbuf, item_cnt, item_cnt_idx);

	pack(pkgbuf, swap_item_rsp->clothes_cnt, idx);
	for (uint32_t i = 0; i < swap_item_rsp->clothes_cnt; i++) {
		pack(pkgbuf, swap_item_rsp->clothes[i].clothes_id, idx);
		pack(pkgbuf, swap_item_rsp->clothes[i].unique_id, idx);
		pack(pkgbuf, swap_item_rsp->clothes[i].gettime, idx);
		pack(pkgbuf, swap_item_rsp->clothes[i].endtime, idx);
		TRACE_LOG("restr reward cloth:[%u %u %u %u %u] len=[%u]",p->id, swap_item_rsp->clothes[i].clothes_id,
            swap_item_rsp->clothes[i].unique_id,swap_item_rsp->clothes[i].gettime,swap_item_rsp->clothes[i].endtime,idx);
        do_stat_log_fumo_consume(swap_item_rsp->clothes[i].clothes_id, p->lv, p->id, p_restr->costs[i].count);
		p->my_packs->add_clothes(p, swap_item_rsp->clothes[i].clothes_id, swap_item_rsp->clothes[i].unique_id, 0, channel_string_active, 
            swap_item_rsp->clothes[i].gettime,swap_item_rsp->clothes[i].endtime);
	}
	
	//init_cli_proto_head(pkgbuf, p, cli_proto_restrict_action, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);

    //-------- gold coin feedback---------
    player_gain_item(p, 2, gold_coin_item_id, gold_coin_count, channel_string_active, false);

	if (restr_id > 208 && restr_id < 214) {
		do_stat_log_universal_interface_1(stat_log_element_daily, restr_id, 1);
		do_stat_log_universal_interface_1(stat_log_element_join, restr_id, p->id);
	}
	if (restr_id > 227 && restr_id < 235) {
		do_stat_log_universal_interface_2(stat_log_peach_daily, p_restr->id, 1, p->id);
	}
	if ((restr_id > 234 && restr_id < 241) || 
		(restr_id > 241 && restr_id < 262)) {
		do_stat_log_universal_interface_1(stat_log_exchange_times_1, p_restr->id, 1);
	}
	uint32_t duration = 0, lifetime = 0;
	char title[MAX_MAIL_TITLE_LEN + 1] = {0};
	char content[MAX_MAIL_CONTENT_LEN + 1] = {0};
	std::vector<mail_item_enclosure> item_vec;
	std::vector<mail_equip_enclosure> equips;
	const restriction_t* p_res_mail = (restriction_t*)(p->session + sizeof(restriction_t));
	if (p_res_mail->rew_count) {
		send_header_to_player(p,p->waitcmd, cli_err_bag_no_space_2_mail, 0);
	}
	strncpy(title, p_res_mail->name, sizeof(title) - 1);
	strcpy(content,"");
	TRACE_LOG("mail: %s ===== %s", title, content);
	for (uint32_t i = 0; i < p_res_mail->rew_count; i++) {
		if (p_res_mail->rewards[i].give_type == give_type_normal_item) {
			item_vec.push_back(mail_item_enclosure(p_res_mail->rewards[i].give_id, p_res_mail->rewards[i].count));
			//if the item is relive item then log it
			stat_log_relive_item(p_res_mail->rewards[i].give_id, p_res_mail->rewards[i].count);
		} else if (p_res_mail->rewards[i].give_type == give_type_clothes) {
			const GfItem* itm = items->get_item(p_res_mail->rewards[i].give_id);
			duration = itm->duration * clothes_duration_ratio;
			lifetime = itm->lifetime();
            //TRACE_LOG("send mail:[%u %u %u]",p_res_mail->rewards[i].give_id,duration,lifetime);
			equips.push_back(mail_equip_enclosure(p_res_mail->rewards[i].give_id, 
				get_now_tv()->tv_sec, 0, duration, lifetime));
		} 
		if (item_vec.size() == 3) {
			db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_normal, 0, &item_vec, 0);
			item_vec.clear();
		}
		if (equips.size() == 3) {
			db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_normal, 0, &item_vec, &equips);
			equips.clear();
		}
	}
	if (item_vec.size() || equips.size()) {
		db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_normal, 0, 
			item_vec.size() ? &item_vec : 0, equips.size() ? &equips : 0);
		item_vec.clear();
		equips.clear();
	}

	return 0;
}

void stat_log_relive_item(uint32_t item_id, uint32_t cnt, uint32_t get_or_use) 
{
	if (get_or_use) {
	//get
		if (item_id == 1302000) {
			do_stat_log_universal_interface_2(stat_log_relive_item1, 0, cnt, 0);
		} else if (item_id == 1302001) {
			do_stat_log_universal_interface_2(stat_log_relive_item2, 0, cnt, 0);
		} 
	} else {
		if (item_id == 1302000) {
			do_stat_log_universal_interface_2(stat_log_relive_item1, 0, 0, cnt);
		} else if (item_id == 1302001) {
			do_stat_log_universal_interface_2(stat_log_relive_item2, 0, 0, cnt);
		} 
	}

}
int db_set_unique_item_bit(player_t* p)
{
	int idx = 0;
	pack(dbpkgbuf, p->unique_item_bit, sizeof(p->unique_item_bit), idx);
	
	return send_request_to_db(0, p->id, p->role_tm, dbproto_set_unique_item_bit, dbpkgbuf, idx);
}

//---------------------------Parse Xml File-------------------------------------------------
/**
  * @brief parse the "costs" and "rewards" sub node
  * @param cur sub node 
  * @param idx index to indicate certain restriction action
  * @return true on success, false on error
  */
bool parse_restriction_cost_reward(xmlNodePtr cur, uint32_t idx)
{
	uint32_t cnt1 = 0, cnt2 = 0;

	while (cur) {
		if ((!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>("Costs")))) {
			xmlNodePtr chl = cur->xmlChildrenNode;
			while (chl) {
				if ((!xmlStrcmp(chl->name, reinterpret_cast<const xmlChar *>("Cost")))) {
					get_xml_prop(g_all_restr[idx].costs[cnt1].give_type, chl, "give_type");
					get_xml_prop(g_all_restr[idx].costs[cnt1].give_id, chl, "give_id");
					get_xml_prop(g_all_restr[idx].costs[cnt1].count, chl, "count");
					TRACE_LOG("Cost:[%u %u %u]",g_all_restr[idx].costs[cnt1].give_type, 
											g_all_restr[idx].costs[cnt1].give_id, g_all_restr[idx].costs[cnt1].count);
					cnt1++;
				} 
				chl = chl->next;
			}
		}

		if ((!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>("Rewards")))) {
			get_xml_prop_def(g_all_restr[idx].odds_flg, cur, "OddsFlg", 0);
			xmlNodePtr chl = cur->xmlChildrenNode;
			while (chl) {
				if ((!xmlStrcmp(chl->name, reinterpret_cast<const xmlChar *>("Reward")))) {
					get_xml_prop(g_all_restr[idx].rewards[cnt2].give_type, chl, "give_type");
					get_xml_prop_def(g_all_restr[idx].rewards[cnt2].role_type, chl, "role_type", 0);
					get_xml_prop(g_all_restr[idx].rewards[cnt2].give_id, chl, "give_id");
                    //KDEBUG_LOG(p->id, "dailyAction give: [%u ---- %u]",g_all_restr[idx].rewards[cnt2].give_type,g_all_restr[idx].rewards[cnt2].give_id);
                    if ( g_all_restr[idx].rewards[cnt2].give_type == 1 ) {
                        const GfItem* itm = items->get_item(g_all_restr[idx].rewards[cnt2].give_id);
                        if ( !itm || !items->is_clothes(itm->category()) ) {
                            ERROR_LOG("dailyAction.xml parameter err.[give_type=1 - give_id=%u]",
                                g_all_restr[idx].rewards[cnt2].give_id);
                            return false;
                        }
                    } else if ( g_all_restr[idx].rewards[cnt2].give_type == 2 ) {
                        const GfItem* itm = items->get_item(g_all_restr[idx].rewards[cnt2].give_id);
                        if ( !itm || items->is_clothes(itm->category()) ) {
                            ERROR_LOG("dailyAction.xml parameter err.[give_type=2 - give_id=%u]",
                                g_all_restr[idx].rewards[cnt2].give_id);
                            return false;
                        }
                    } else if ( g_all_restr[idx].rewards[cnt2].give_type == 3 ) {
                        if ( g_all_restr[idx].rewards[cnt2].give_id > 100000 ) {
                            ERROR_LOG("dailyAction.xml parameter err.[give_type=3 - give_id=%u]",
                                g_all_restr[idx].rewards[cnt2].give_id);
                            return false;
                        }
                    }
					get_xml_prop(g_all_restr[idx].rewards[cnt2].count, chl, "count");
					get_xml_prop_def(g_all_restr[idx].rewards[cnt2].odds, chl, "Odds", 0);
					TRACE_LOG("Reward:[%u %u %u %u %u]",g_all_restr[idx].rewards[cnt2].give_type, 
                        g_all_restr[idx].rewards[cnt2].role_type,g_all_restr[idx].rewards[cnt2].give_id, 
                        g_all_restr[idx].rewards[cnt2].count, g_all_restr[idx].rewards[cnt2].odds);
					cnt2++;
				}
				chl = chl->next;
			}
		}
	
		cur = cur->next;
	} //end while(cur)

	(g_all_restr[idx]).cost_count = cnt1;
	(g_all_restr[idx]).rew_count = cnt2;
	TRACE_LOG("restr cnt[%u %u]",(g_all_restr[idx]).cost_count, (g_all_restr[idx]).rew_count);
	return true;
}

/**
  * @brief load restriction configs from an xml file
  * @param cur xml node 
  * @return 0 on success, -1 on error
  */
int load_restrictions(xmlNodePtr cur)
{
	uint32_t id_test = 0, dbpos_test = flag_pos_daily_active_start - 1;
	uint32_t id = 0, type = 0, restr_flag = 0, toplimit = 0, vip = 0, year_vip = 0, userflg = 0, dbpos = 0;
	cur = cur->xmlChildrenNode; 
	memset(g_all_restr, 0, sizeof(g_all_restr));
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Restriction"))) {
			id_test++;
			get_xml_prop(id, cur, "ID");
			//if (id != id_test) {
			//	ERROR_LOG("Id duplicated restriction: id=%u", id);
			//	return -1;
			//}
		
			get_xml_prop(type, cur, "Type");
			uint32_t idx = id - 1;
			if ((id < 1) || (idx >= max_restr_count) || (g_all_restr[idx]).type) {
				ERROR_LOG("invalid/duplicated restriction: type=%u max=%u %u", id, max_restr_count, g_all_restr[idx].type);
				return -1;
			}
			get_xml_prop_raw_str_def(g_all_restr[idx].name, cur, "Name", "兑换物品");
			get_xml_prop(restr_flag, cur, "Restr_Flag");
			get_xml_prop_def(toplimit, cur, "Toplimit", 999999);
			get_xml_prop_def(vip, cur, "Vip", 0);
			get_xml_prop_def(year_vip, cur, "YearVip", 0);
			//get_xml_prop_def(uselv, cur, "UseLv", 0);
            if ( get_xml_prop_arr_def(g_all_restr[idx].uselv, cur, "UseLv", 0) != 2 ) {
                WARN_LOG("invalid appearlevel in parsing restriction UserLv: type=%u id=%u %u %u", type, id,
					g_all_restr[idx].uselv[0], g_all_restr[idx].uselv[1]);
                g_all_restr[idx].uselv[0] = 0;
				g_all_restr[idx].uselv[1] = 100;
            }
			get_xml_prop_def(userflg, cur, "UserFlag", 0);
			if (userflg && restr_flag == daily_active_for_ever) {
				get_xml_prop(dbpos, cur, "DbPos");
				if (dbpos != dbpos_test + 1) {
					ERROR_LOG("restriction dbpos[%u %u] error!", dbpos, dbpos_test);
					//return -1;
				}
				dbpos_test = dbpos;
			}
			if (!userflg && restr_flag == daily_active_for_ever) {
				if (!get_unique_item_bitpos(id)) {
					ERROR_LOG("unique_item_map not have id %u ! ", id);
					return -1;
				}
			}

			if (get_xml_prop_arr_def(g_all_restr[idx].tm_range, cur, "TimeRange", 0) != 2) {
				ERROR_LOG("invalid appeartime in parsing restriction TimeRange: type=%u", type);
				return -1;
			}
			(g_all_restr[idx]).id = id;
			(g_all_restr[idx]).type = type;
			(g_all_restr[idx]).restr_flag = restr_flag;
			(g_all_restr[idx]).toplimit = toplimit;
			(g_all_restr[idx]).vip = vip;
			(g_all_restr[idx]).year_vip = year_vip;
			//(g_all_restr[idx]).uselv = uselv;
			(g_all_restr[idx]).userflg = userflg;
			(g_all_restr[idx]).dbpos = dbpos;
			
            TRACE_LOG("restr:[%u %u %u %u [%u %u] %u %u]",(g_all_restr[idx]).id, (g_all_restr[idx]).type, 
                (g_all_restr[idx]).restr_flag, (g_all_restr[idx]).toplimit, 
                (g_all_restr[idx]).uselv[0],(g_all_restr[idx]).uselv[1],
                (g_all_restr[idx]).vip,(g_all_restr[idx]).userflg);
            if (!parse_restriction_cost_reward(cur->xmlChildrenNode, idx)) {
                ERROR_LOG("Parse restriction costs and rewards error!");
				return -1;
			}
		}
		cur = cur->next;
	}

	return 0;
}
//-----------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------------
// Cmds
//---------------------------------------------------------------------------------------------

int show_restrict_reward_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	return restrict_action_cmd(p, body, bodylen);
}

int restrict_action_ex_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	return restrict_action_cmd(p, body, bodylen);
}

bool is_come_to_vip_active(player_t* p, uint32_t id)
{
    switch (id) {
        case 364:
            if (p->act_record[act_record_pos] == 0 || p->act_record[act_record_pos] > 2) {
                return false;
            }
            break;
        case 365:
            if (p->act_record[act_record_pos] <= 2 || p->act_record[act_record_pos] > 6) 
                return false;
            break;
        case 366:
            if (p->act_record[act_record_pos] <= 6 || p->act_record[act_record_pos] > 11) 
                return false;
            break;
        case 367:
            if (p->act_record[act_record_pos] < 12) 
                return false;
            break;
        default:
            break;
    }
    return true;
}
bool is_can_get_taotai_gift(player_t* p, uint32_t id)
{
    if (p->taotai_info->win_times > (id - 374)) {
        return true;
    }
    return false;
}

bool is_can_get_advance_gift(player_t *p, uint32_t id)
{
	if (p->advance_info->win_times >= (id - 391 + 2)) {
		return true;
	}
	return false;
}
/**
  * @brief player performs periodical action
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int restrict_action_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t id;
	unpack(body, id, idx);

	if (id != 334 && p->battle_grp) {// && ((id <= 213) || (id >= 217))) {  214-216 促销活动 武圣祝福
		return send_header_to_player(p, p->waitcmd, cli_err_unable_in_battle, 1);
	}
	
	int i = id - 1;
	if ((i < 0) || (static_cast<uint32_t>(i) >= max_restr_count) || (g_all_restr[i].id != id)) {
		ERROR_LOG("Invalid action type\t[id=%u action_type=%u]", p->id, id);
		p->waitcmd = 0;
		return 0;
	}
	if (id == 37) {
	// for moon pet, some people have got this pet in holiday activity befor. so this they forbided 
		if (check_summon_type_exist(p, 1012) || p->my_packs->is_item_exist(1400067)) {
			return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
		}
	}

    if ( !is_come_to_vip_active(p, id) ) {
        return send_header_to_player(p, p->waitcmd, cli_err_limit_vip_act, 1);
    }
    if (id >=374 && id <= 380) {
        if (!is_can_get_taotai_gift(p, id)) {
            return send_header_to_player(p, p->waitcmd, cli_err_cannot_take_gift_taotai_game, 1);
        }
    }

	if (id >= 391 && id <= 397) {
		if (!is_can_get_advance_gift(p, id)) {
            return send_header_to_player(p, p->waitcmd, cli_err_cannot_take_gift_advance_game, 1);
		}
	}
	//fumo top rewards
	if (g_all_restr[i].type == 20 && p->fumo_reward_flg != fumo_top_can_reward) {
	//fumo top rewards
		return send_header_to_player(p, p->waitcmd, cli_err_fumo_have_got_reward, 1);
	}
	
    if (g_all_restr[i].uselv[0] > p->lv) {
        return send_header_to_player(p, p->waitcmd, cli_err_less_lv, 1);
    }
    if (g_all_restr[i].uselv[1] <= p->lv) {
        return send_header_to_player(p, p->waitcmd, cli_err_large_lv, 1);
    }
	if (g_all_restr[i].restr_flag == daily_active_for_ever) {
		if (g_all_restr[i].userflg) {
			// test user's bit buf : once_bit
			if (test_bit_on(p->once_bit, g_all_restr[i].dbpos)) {
				return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
			}
		} else {
			uint32_t pos = get_unique_item_bitpos(g_all_restr[i].id);
			TRACE_LOG("get_unique_item_bitpos%u:%u ", pos, g_all_restr[i].id);
			// test role's bit buf : unique_item_bit
			if (!pos || test_bit_on(p->unique_item_bit, pos)) {
				return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
			}
		}
	} else {
		if (g_all_restr[i].toplimit != 0 && p->daily_restriction_count_list[i] >= g_all_restr[i].toplimit) {
            TRACE_LOG("restriction [uid=%u toplimit=%u]", p->id, g_all_restr[i].toplimit);
			return send_header_to_player(p, p->waitcmd, cli_err_day_limit_time_act + g_all_restr[i].restr_flag, 1);
		}
	}
	const struct tm* cur_tm = get_now_tm();
	uint32_t hour = static_cast<uint32_t>(cur_tm->tm_hour);
	if ((hour < g_all_restr[i].tm_range[0]) || (hour >= g_all_restr[i].tm_range[1])) {
		return send_header_to_player(p, p->waitcmd, cli_err_limit_time_act, 1);
	}

	TRACE_LOG("%u %u %u %u %u", p->id, id, g_all_restr[i].vip, g_all_restr[i].year_vip, p->vip_level);
    if ( g_all_restr[i].year_vip && (!is_vip_year_player(p)) ) {
		return send_header_to_player(p, p->waitcmd, cli_err_vipyear_exclusive_right, 1);
    }
	if (g_all_restr[i].vip && (g_all_restr[i].vip > p->vip_level || !(is_vip_player(p)))) {	
		return send_header_to_player(p, p->waitcmd, cli_err_nonvip_operation_forbidden, 1);
	} 

	KDEBUG_LOG(p->id, "ADD ACTION CNT\t[id=%u id=%u limit=%u]", p->id, id, g_all_restr[i].toplimit);
	//return db_add_action_count(p, &(g_all_restr[i]));
    restriction_t res = {0};
	restriction_t res_mail = {0};
    TRACE_LOG("%u %u", id, g_all_restr[id - 1].rew_count);
    if (pack_res_swap_item(p, id, p->role_type, &res, &res_mail)) {
		if (p->waitcmd == cli_proto_show_restrict_reward) {
			int idx = 0;
			//pack type in player's session
			pack(p->tmp_session, &res, sizeof(res), idx);
			pack(p->tmp_session, &res_mail, sizeof(res_mail), idx);
			send_recv_restrict_action_reward(p, &res, &res_mail);
			g_fanfan_mrg.add_daily_cnt();
			return db_use_item_ex(0, p->id, p->role_tm, 1700068, 1, true);
		}
	    return db_swap_item(p, &res, &res_mail);
    }
	return 0;
}

int recv_restrict_reward_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	restriction_t res = {0};
	restriction_t res_mail = {0};
    
	int idx = 0;
	//pack type in player's session
	unpack(p->tmp_session, &res, sizeof(res), idx);
	unpack(p->tmp_session, &res_mail, sizeof(res_mail), idx);

	TRACE_LOG("%u %u %u", res.id, res.rew_count, res_mail.rew_count);
	return db_swap_item(p, &res, &res_mail);
}

/**
  * @brief player performs periodical action
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_restrict_action_times_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0, idx2 = 0;
    uint32_t id = 0;
    unpack(body, id, idx);

	if ( !id || id > max_restr_count ) {
		return -1;
	}
    uint32_t cnt = p->daily_restriction_count_list[id - 1];

	if (g_all_restr[id - 1].restr_flag == daily_active_for_ever) {
		if (g_all_restr[id - 1].userflg) {
			cnt = test_bit_on(p->once_bit, g_all_restr[id - 1].dbpos);
		} else {
			uint32_t pos = get_unique_item_bitpos(g_all_restr[id - 1].id);
			TRACE_LOG("get_unique_item_bitpos%u:%u ", pos, g_all_restr[id - 1].id);
			// test role's bit buf : unique_item_bit
			cnt = test_bit_on(p->unique_item_bit, pos);
		}
	}

    TRACE_LOG("action times: [%u %u]", id, cnt);
    idx2 = sizeof(cli_proto_t);
    pack(pkgbuf, id, idx2);
    pack(pkgbuf, cnt, idx2);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx2);
    return send_to_player(p, pkgbuf, idx2, 1);
}

//---------------------------------------------------------------------------------------------
// db_XXX function
//---------------------------------------------------------------------------------------------
int db_swap_item(player_t* p, const restriction_t* p_restr, const restriction_t* p_res_mail)
{
	p->sesslen = 0;
	//pack type in player's session
	pack(p->session, p_restr, sizeof(*p_restr) , p->sesslen);
	pack(p->session, p_res_mail, sizeof(*p_res_mail) , p->sesslen);
	restriction_t* p_restr_sess = (restriction_t*)(p->session);

	TRACE_LOG("%u %u", p->id, p_restr_sess->userflg);
	
	uint32_t cost_cnt = p_restr_sess->cost_count;
	uint32_t rew_cnt = p_restr_sess->rew_count;
	
	int idx = 0;
	pack_h(dbpkgbuf, static_cast<uint32_t>(0), idx);
	pack_h(dbpkgbuf, cost_cnt, idx);
	pack_h(dbpkgbuf, rew_cnt, idx);
	pack_h(dbpkgbuf, get_player_total_item_bag_grid_count(p), idx);
	TRACE_LOG("restr :[%u %u %u]",p->id, cost_cnt, rew_cnt);
	for (uint32_t i = 0; i < cost_cnt; i++) {
		pack_h(dbpkgbuf, p_restr_sess->costs[i].give_type, idx);
		pack_h(dbpkgbuf, p_restr_sess->costs[i].give_id, idx);
		pack_h(dbpkgbuf, p_restr_sess->costs[i].count, idx);
		TRACE_LOG("cost item :[%u %u %u %u]",p->id, p_restr_sess->costs[i].give_type, p_restr_sess->costs[i].give_id,
								p_restr_sess->costs[i].count);
	}
	for (uint32_t i = 0; i < rew_cnt; i++) {
		uint32_t duration = 0;
		uint32_t lifetime = 0;
		uint32_t item_max = 999;

		if (p_restr_sess->rewards[i].give_type == give_type_player_attr) {
			if (p_restr_sess->rewards[i].give_id == give_type_xiaomee && p->coins > 4000000000) {
				return send_header_to_player(p, p->waitcmd, cli_err_coins_too_much, 1);
			}
			if (p_restr_sess->rewards[i].give_id == give_type_summon_exp && p->allocator_exp > 4000000000) {
				return send_header_to_player(p, p->waitcmd, cli_err_max_exp_lv, 1);
			}
		}


		if (p_restr_sess->rewards[i].give_type == give_type_normal_item
				 || p_restr_sess->rewards[i].give_type == give_type_clothes) {
			const GfItem* itm = items->get_item(p_restr_sess->rewards[i].give_id);
			duration = itm->duration * clothes_duration_ratio;
            lifetime = itm->lifetime();
			item_max = itm->max();
		} else if (p_restr_sess->rewards[i].give_type == give_type_player_attr && 
					p_restr_sess->rewards[i].give_id == give_type_apothecary_exp) {
			secondary_profession_data* p_sec_data;
			p_sec_data = get_player_secondary_profession_by_type(p, apothecary_profession_type);
			if (p_sec_data) {
				uint32_t cur_exp = p_sec_data->get_profession_exp();
				uint32_t max_exp = p_sec_data->m_pCalc->get_max_exp();
				uint32_t add_exp = p_restr_sess->rewards[i].count;
				if (cur_exp < max_exp) {
					p_restr_sess->rewards[i].count = (add_exp + cur_exp) > max_exp ? (max_exp - cur_exp) : add_exp;
				} else {
					p_restr_sess->rewards[i].count = 0;
				}
				item_max = max_exp;
			} else {
				return send_header_to_player(p, p->waitcmd, cli_err_secondary_pro_not_exist, 1);
			}
			
		}
		
		pack_h(dbpkgbuf, p_restr_sess->rewards[i].give_type, idx);
		pack_h(dbpkgbuf, p_restr_sess->rewards[i].give_id, idx);
		pack_h(dbpkgbuf, p_restr_sess->rewards[i].count, idx);
		pack_h(dbpkgbuf, duration, idx);
		pack_h(dbpkgbuf, lifetime, idx);
		pack_h(dbpkgbuf, item_max, idx);
        if (i+1 >= 454 && i+1 <= 469) {
            do_stat_log_exchange(p->id, p->role_type, i+1, p_restr_sess->rewards[i].give_id);
        } else if (i+1 == 663 || i+1 == 664) {
            do_stat_log_get_material(p_restr_sess->rewards[i].give_id, p_restr_sess->rewards[i].count);
        }
		TRACE_LOG("reward item :[%u %u %u %u %u %u %u]",p->id, p_restr_sess->rewards[i].give_type, p_restr_sess->rewards[i].give_id,
								p_restr_sess->rewards[i].count, duration, lifetime, item_max);
	}
	return send_request_to_db(p, p->id, p->role_tm, dbproto_swap_item, dbpkgbuf, idx);
}
/**
  * @brief add action count restriction to db
  * @param p the player who launches the request
  * @param p_restr
  */
int db_add_action_count(player_t* p, userid_t uid, uint32_t role_tm, uint32_t id, uint32_t flag, uint32_t toplimit, uint8_t userflg)
{
	int idx = 0;
	pack_h(dbpkgbuf, id, idx);
	pack_h(dbpkgbuf, flag, idx);
	pack_h(dbpkgbuf, toplimit, idx);
	TRACE_LOG("restr count:[%u %u %u %u %u]", uid, id, flag, toplimit, userflg);

	return send_request_to_db(p, uid, userflg ? 0 : role_tm, dbproto_add_action_count, dbpkgbuf, idx);
}


//---------------------------------------------------------------------------------------------
// Callbacks
//---------------------------------------------------------------------------------------------
/**
  * @brief callback for handling adding periodical action returned from dbproxy
  * @param p the requester
  * @param uid id of the requester
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */ 
int db_add_action_count_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	db_add_action_rsp_t* rsp = reinterpret_cast<db_add_action_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(db_add_action_rsp_t));
	uint32_t restr_id = rsp->id;
	p->daily_restriction_count_list[restr_id - 1]++;

	if (p->waitcmd == cli_proto_restrict_action || 
	     p->waitcmd == cli_proto_recv_restrict_reward||
	     p->waitcmd == cli_proto_restrict_action_ex) {
		return send_restrict_action_rsp_to_client(p, rsp->id);
	} else if (p->waitcmd == cli_proto_double_exp_action || 
		p->waitcmd == cli_proto_double_exp_action_use_item) {
		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, p->day_flag, idx);
	    pack(pkgbuf, p->dexp_time, idx);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	}
	return 0;
}

int db_swap_res_item_callback(player_t * p, const swap_item_rsp_t* rsp, int rsp_len)
{

	//pack DB package
	uint32_t id = 0;
	uint32_t toplimit = 0;
	uint32_t flag = 0;
	uint8_t userflg = 0;
	

	//pack type in player's session
	pack(p->session, rsp, rsp_len, p->sesslen);
	
	restriction_t* p_res = (restriction_t*)(p->session);
	id = p_res->id;
	toplimit = p_res->toplimit;
	flag = p_res->restr_flag;
	userflg = p_res->userflg;

	if (p_res->type == 20) {
	//fumo top rewards
		p->fumo_reward_flg = fumo_top_have_reward;
		send_request_to_db(0, p->id, p->role_tm, dbproto_del_player_old_hero_top, 0, 0);
	}
	
	do_stat_log_caiji(p->id, id);
    if (id > 200 && id < 209) {
        do_stat_log_universal_interface_1(stat_log_use_time_sack,id - 200, 1);
    } else if (id >= 262 && id <= 321) {
        do_stat_log_universal_interface_1(0x09523000, id, 1);
    } else if ( id == 400 ) {
        do_stat_log_universal_interface_1(stat_log_yaoshi_game, id - 400, p->id);
    } /*else if ( id == 46 ) {
        do_stat_log_universal_interface_1(stat_log_accept_up_level, 0, 1);
    } else if ( id == 47 ) {
        do_stat_log_universal_interface_1(stat_log_finish_up_level, 0, 1);
    } */
	if (p_res->type == 13) {
		do_stat_log_universal_interface_1(stat_log_daily_get_reward_times, get_award_cnt(p), 1);
	}
	
	//统计汤圆活动的兑换次数
	if( p_res->type == 25){
		p->player_show_state += 1;
		db_set_player_show_state(p, p->player_show_state);
		db_add_donate(p, 1);
	}

	if (p_res->restr_flag == daily_active_for_ever) {
		if (userflg) {
			set_once_bit_on(p, p_res->dbpos);
			db_set_user_flag(p, 0);
		} else {
			uint32_t pos = get_unique_item_bitpos(p_res->id);
			TRACE_LOG("get_unique_item_bitpos%u:%u ", pos, p_res->id);
			if (pos) {
				taomee::set_bit_on(p->unique_item_bit, pos);
				db_set_unique_item_bit(p);
			}
		}
		return send_restrict_action_rsp_to_client(p, p_res->id);
	} else {
		return db_add_action_count(p, p->id, p->role_tm, id, flag, toplimit, userflg);
	}
}

/**
  * @brief callback for handling query restriction list when login
  * @param p the requester
  * @param uid id of the requester
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */ 
int db_query_restriction_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	if (p->id != id) {
		return 0;
	}

	const query_res_list_rsp_t* rsp = reinterpret_cast<query_res_list_rsp_t *>(body);
	CHECK_VAL_EQ(bodylen, sizeof(query_res_list_rsp_t) + rsp->cnt * sizeof(db_res_elem_t));

	p->fumo_reward_flg = rsp->fumo_reward_flg;
	uint32_t restr_cnt = rsp->cnt;
	TRACE_LOG("%u %u %u", p->id, rsp->fumo_reward_flg, rsp->cnt);
	if (rsp->cnt > max_daily_restriction_num) {
		restr_cnt = max_daily_restriction_num;
		WARN_LOG("Too many max_daily_restriction_num, uid=%u, res_cnt=%u", p->id, rsp->cnt);
	}
	build_restriction_list(p, restr_cnt, rsp->res);
#ifdef NEW_LOGIN_PACKAGE
	if ( p->waitcmd == cli_proto_login ) {
			return db_get_summon_list(p, p->id, p->role_tm);
		}
	return 0;
#else
	return db_get_fumo_info(p);
#endif
}


//---------------------------------------------------------------------------------------------
