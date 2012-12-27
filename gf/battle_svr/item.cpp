#include <libtaomee++/inet/pdumanip.hpp>

#include "cli_proto.hpp"
#include "dbproxy.hpp"
#include "utils.hpp"
#include "player.hpp"
#include "stage.hpp"
#include "item.hpp"
#include "battle_impl.hpp"
#include "player_attribute.hpp"

std::map<uint32_t, unique_item_pos_t> unique_item_map;
suit_t suit_arr[max_suit_id + 1];
/*! treasure box info */
treasure_box_t 			treasure_boxs[max_treasure_box_cnt];
stage_treasure_box_t 	stage_boxs[stage_max_num];
vip_treasure_box_t   	vip_boxs[vip_max_lv];
holiday_drop_t holiday_drop_arr[max_holiday_drop_type];

using namespace taomee;

/***********************************************************************************************
 ** treasure box
 **********************************************************************************************/
static int pack_treasure_box_item(Player* p, uint8_t* buf, const treasure_box_item_t* p_item, 
		uint32_t drop_lv, uint32_t flag, uint32_t* drop_info);


const treasure_box_t* get_treasure_box(uint32_t id)
{
	return &(treasure_boxs[id - 1]);
}

const box_id_info_t* get_box_ids(Player* p, uint32_t box_type)
{
	//normal box
	if (box_type == normal_box) {
		const stage_t* stg = p->btl->stage();
		const stage_treasure_box_t* p_stage_box = &(stage_boxs[stg->id - 1]);
		uint32_t score_grade = p->score.stage_grade;
		if (score_grade > score_max_cnt || score_grade == 0 || 
				stg->id > stage_max_num || stg->id > 900) {
			WARN_LOG("no box:score[%u %u %u]",p->id, score_grade, stg->id);
			return 0;
		}
		return &(p_stage_box->boxs[score_grade - 1]);
	}
	
	//vip box
	if (box_type == vip_box) {
		//TODO:
		//uint32_t vip_lv = 1;
		uint32_t vip_lv = p->vip_level;
		if ( vip_lv > vip_max_lv || !(p->is_vip_player()) ) {
			WARN_LOG("no box:viplv[%u %u]",p->id, vip_lv);
			return 0;
		}
		return &(vip_boxs[vip_lv - 1].boxs);
	}

	return 0;
}

uint32_t get_clothes_drop_lv(Player*p, uint32_t box_type)
{
	//if (box_type == normal_box) {
		const stage_t* stg = p->btl->stage();
		const stage_treasure_box_t* p_stage_box = &(stage_boxs[stg->id - 1]);
		return p_stage_box->drop_lv;
	//}

	//return 0;
}

int date2timestamp(const char* fromstr, const char* fmt, time_t &totime)
{
	struct tm tmp_time;
	strptime(fromstr, fmt, &tmp_time);
	totime = mktime(&tmp_time);
	return totime;
}


//------------------------------------------------------------------
/**
 * @brief player open treasure box
 */
int player_open_treasure_box_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t box_type = 0;
	uint32_t box_id = 0;
	uint32_t item_pack_max = 0;
	unpack_h(body, item_pack_max, idx);
	unpack_h(body, box_type, idx);
	unpack_h(body, box_id, idx);
	TRACE_LOG("[%u %u %u %u]:[%u %u]", p->id, item_pack_max, box_type, box_id, p->vip, p->vip_level);
	
	//judge if can open boxs
	if (p->btl && p->btl->is_battle_tower()) {
		if (!(p->can_open_box) || (box_type != normal_box && box_type != vip_box) ||
			(box_type == normal_box && p->normal_box_opened >= p->cur_tower_layer) ||
			(box_type == vip_box && (!(p->is_vip_player()) || p->vip_box_opened >= p->cur_tower_layer)) ) {
			WARN_LOG("cannot open box[%u %u %u][%u %u]",p->id, p->vip, box_type, p->normal_box_opened, p->vip_box_opened);
			return send_header_to_player(p, p->waitcmd, cli_err_battle_cannot_open_box, 1);
		}
	} else {
		if (!(p->can_open_box) || (box_type != normal_box && box_type != vip_box) ||
			(box_type == normal_box && p->normal_box_opened) ||
			(box_type == vip_box && (!(p->is_vip_player()) || p->vip_box_opened)) ) {
			WARN_LOG("cannot open box[%u %u %u][%u %u]",p->id, p->vip, box_type, p->normal_box_opened, p->vip_box_opened);
			return send_header_to_player(p, p->waitcmd, cli_err_battle_cannot_open_box, 1);
		}
	}

	//get boxs which to open, normal boxs or vip boxs
	const box_id_info_t* p_box_ids = get_box_ids(p, box_type);
	if (!p_box_ids || p_box_ids->box_cnt == 0) {
		WARN_LOG("no box id[%u %u %u]",p->id, box_type, p_box_ids ? p_box_ids->box_cnt : 0);
		return send_header_to_player(p, p->waitcmd, cli_err_battle_cannot_open_box, 1);
	}

	//set open flag
	if (p->btl && p->btl->is_battle_tower()) {
		if (box_type == normal_box) {
			p->normal_box_opened = p->cur_tower_layer;
		} else if (box_type == vip_box) {
			p->vip_box_opened = p->cur_tower_layer;
		}
	} else {
		if (box_type == normal_box) {
			p->normal_box_opened = true;
		} else if (box_type == vip_box) {
			p->vip_box_opened = true;
		}
	}

	//open all boxs ,and choose one give to player
	uint32_t drop_item_info[2];
	memset(drop_item_info, 0x00, sizeof(drop_item_info));

	uint32_t count = 0;
	int cnt_idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, item_pack_max, cnt_idx);
	pack_h(pkgbuf, box_type, cnt_idx);
	pack_h(pkgbuf, box_id, cnt_idx);
	TRACE_LOG("[%u,%u]",box_type, box_id);
	idx = cnt_idx + 4;

	uint32_t drp_lv = get_clothes_drop_lv(p, box_type);

	uint32_t tmp_box_cnt = box_type == vip_box ? 2 : p_box_ids->box_cnt;
	if (p_box_ids->box_cnt < 2) {
		tmp_box_cnt = p_box_ids->box_cnt;
	}
	uint32_t rand_box = rand() % tmp_box_cnt;
	TRACE_LOG("rand:[%u %u] drplv[%u]", rand_box, p_box_ids->box_cnt, drp_lv);
	
	for (uint32_t i = 0; i < tmp_box_cnt; i++) {
		uint32_t box_id = p_box_ids->box_id[i];
		treasure_box_t box = *(get_treasure_box(box_id));
		//rand choose one item from some box
		uint32_t rand_odds = rand() % 100;
		TRACE_LOG("item cnt:[%u]", box.items_cnt);
		for (uint32_t j = 0; j < box.items_cnt; j++) {
			TRACE_LOG("rand odds:[%u %u %u]", rand_odds, box.items[j].odds_lower, box.items[j].odds_upper);
			if ( rand_odds >= box.items[j].odds_lower && 
				 rand_odds <= box.items[j].odds_upper ) {
				if (box.items[j].give_id == give_type_exp && p->lv >= 55) {
					box.items[j].give_id = give_type_xiaomee;
				}
				uint32_t get_flag = (i==rand_box) ? 1 : 0;
				pack_h(pkgbuf, box.quality, idx);
				pack_h(pkgbuf, get_flag, idx);

				idx += pack_treasure_box_item(p, pkgbuf + idx, &(box.items[j]), drp_lv, get_flag, drop_item_info);
				count++;
				TRACE_LOG("chose one item[%u [%u %u]]",p->id, i, j);
				break;
			}
		}
	}
	pack_h(pkgbuf, count, cnt_idx);

	//init_btl_proto_head(pkgbuf, p->waitcmd, idx);
	//send_to_player(p, pkgbuf, idx, 1);
	
	if (drop_item_info[0] != 0) {
		//treasure_box_drop_items(p, drop_item_info[0], drop_item_info[1]);
		send_request_to_db(p, p->id, p->role_tm, dbproto_player_treasure_box, 
				pkgbuf + sizeof(btl_proto_t), idx - sizeof(btl_proto_t));
	} else {
		init_btl_proto_head(pkgbuf + 4, p->waitcmd, idx - 4);
		send_to_player(p, pkgbuf + 4, idx - 4, 1);
	}

	return 0;
}

int db_player_treasure_box_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	const db_treasure_box_rsp_t* rsp = reinterpret_cast<db_treasure_box_rsp_t*>(body);

	CHECK_VAL_EQ(bodylen, (uint32_t)(sizeof(db_treasure_box_rsp_t) + rsp->box_cnt * sizeof(db_treasure_box_elem_t)) ) ;
	
	uint32_t itm_id = 0;
	uint32_t itm_cnt = 0;
	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, rsp->box_type, idx);
	pack_h(pkgbuf, rsp->box_id, idx);
	pack_h(pkgbuf, rsp->box_cnt, idx);
	TRACE_LOG("[%u %u %u]", rsp->box_type, rsp->box_id, rsp->box_cnt);
	for ( uint32_t i = 0; i < rsp->box_cnt; i++) {
		const db_treasure_box_elem_t* p_box = &(rsp->boxs[i]);
		//save fail,ready to drop
		if (p_box->get_flag > 1) {
			itm_id = p_box->item_id;
			itm_cnt = p_box->item_cnt;
		}
		pack_h(pkgbuf, p_box->box_quality, idx);
		pack_h(pkgbuf, p_box->get_flag, idx);
		pack_h(pkgbuf, p_box->item_type, idx);
		pack_h(pkgbuf, p_box->item_id, idx);
		pack_h(pkgbuf, p_box->item_cnt, idx);
		pack_h(pkgbuf, p_box->duration, idx);
		pack_h(pkgbuf, p_box->unique_index, idx);
		TRACE_LOG("[%u %u %u %u %u %u %u]", p_box->box_quality, p_box->get_flag, p_box->item_type, p_box->item_id, 
				p_box->item_cnt, p_box->duration, p_box->unique_index);
	}

	init_btl_proto_head(pkgbuf, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);
	
	//drop
	if (itm_id != 0) {
		treasure_box_drop_items(p, itm_id, itm_cnt);
	}
	return 0;
}

int pack_treasure_box_item(Player* p, uint8_t* buf, const treasure_box_item_t* p_item, 
						uint32_t drop_lv, uint32_t flag, uint32_t* drop_info)
{
	uint32_t item_id = 0;
	uint32_t item_cnt = 0;
	uint32_t item_type = p_item->give_type;
	uint32_t item_duration = 0;
	uint32_t unique_index = 0;
	bool is_in_tower = p->btl->is_battle_tower();
	TRACE_LOG("[%u %u %u]", p_item->give_type, p_item->give_id, p_item->give_cnt);
	if (p_item->give_type == give_type_player_attr) {
		//update player attr
		item_id = p_item->give_id;
		item_cnt = p_item->give_cnt;
		if (flag) {
			switch(p_item->give_id) {		
				case give_type_xiaomee :
					p->coins += p_item->give_cnt;
					
					do_stat_item_log_2(stat_log_produce_coins, 0, 0, p_item->give_cnt);
					if (is_in_tower) {
						uint32_t tmp_buf[6] = {0};
						tmp_buf[1] = 1;
						do_stat_item_log_4(stat_log_fumo_reward, 0, 0, tmp_buf, sizeof(tmp_buf));
					}
					break;
				case give_type_exp :
					p->exp += p_item->give_cnt;
					if (is_in_tower) {
						uint32_t tmp_buf[6] = {0};
						tmp_buf[0] = 1;
						do_stat_item_log_4(stat_log_fumo_reward, 0, 0, tmp_buf, sizeof(tmp_buf));
					}
					break;
				case give_type_skillpoint :
					p->skill_point += p_item->give_cnt;
					break;
				case give_type_fumo_point :
					p->fumo_points_end += p_item->give_cnt;
					break;
			}
			drop_info[0] = 0;
			drop_info[1] = 0;
		}
	} else if (p_item->give_type == give_type_normal_item) {
		//drop item
		item_id = p_item->give_id;
		item_cnt = p_item->give_cnt;
		if (flag) {
			//treasure_box_drop_items(p, item_id, item_cnt);
			drop_info[0] = item_id;
			drop_info[1] = item_cnt;
		}
	} else if (p_item->give_type == give_type_clothes) {
		//drop clothes
		if (p_item->give_id > max_drop_lv) {
			item_id = p_item->give_id;
		} else {
			item_id = items->get_drop_clothesid(drop_lv, p_item->give_id, p_item->equip_part, p->role_type);
		}

		item_cnt = p_item->give_cnt;

		//tmp:TODO
		if (item_id == 0) {
			item_id = 1600001;
			item_cnt = 2;
			item_type = give_type_normal_item;
		} else {
			const GfItem* itm = items->get_item(item_id);
			item_duration = itm->duration * clothes_duration_ratio;
		}

		if (item_id != 0 && flag) {
			//treasure_box_drop_items(p, item_id, item_cnt);
			drop_info[0] = item_id;
			drop_info[1] = item_cnt;
			if (is_in_tower) {
				const GfItem* itm = items->get_item(item_id);
				if (itm->quality_lv < 5 && itm->quality_lv > 0) {
					uint32_t tmp_buf[6] = {0};
					tmp_buf[1 + itm->quality_lv] = 1;
					do_stat_item_log_4(stat_log_fumo_reward, 0, 0, tmp_buf, sizeof(tmp_buf));
				}
			}
		}
	}

	int idx = 0;
	pack_h(buf, item_type, idx);
	pack_h(buf, item_id, idx);
	pack_h(buf, item_cnt, idx);
	pack_h(buf, item_duration, idx);
	pack_h(buf, unique_index, idx);
	TRACE_LOG("[%u %u %u %u]",p->id, item_id, item_cnt, flag);

	return idx;
}

/***********************************************************************************************
 ** clothes duration function
 **********************************************************************************************/
/**
 * @brief player get clothes duration
 */
int player_clothes_duration_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, static_cast<uint32_t>(p->clothes_info.clothes_cnt), idx);

	for (uint32_t i = 0; i < p->clothes_info.clothes_cnt; i++) {
		player_clothes_info_t* cloth = &(p->clothes_info.clothes[i]);
		pack_h(pkgbuf, cloth->unique_id, idx);
		pack_h(pkgbuf, cloth->duration, idx);
	}
	init_btl_proto_head(pkgbuf, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
 * @brief get player's clothes duration
 */
/*uint8_t get_duration_state(const player_clothes_info_t* clothes)
{
	float duration_rate = clothes->duration / clothes->init_duration;
	uint8_t duration_state = 0;
	if (duration_rate > 0.50) {
		duration_state = no_loss;
	} else if (duration_rate > 0.30) {
		duration_state = loss_50_per;
	} else if (duration_rate > 0.20) {
		duration_state = loss_70_per;
	} else if (clothes->duration > 0) {
		duration_state = loss_80_per;
	} else {
		duration_state = loss_100_per;
	}

	return duration_state;
}*/


/**
 * @brief get player's clothes duration
 */
int duration_state_chg_noti(Player* p, player_clothes_info_t* cloth)
{
	//get clothes duration state
	TRACE_LOG("duration[%u %u]", cloth->duration, cloth->init_duration);
	uint8_t duration_state = get_cloth_duration_state(cloth->duration, cloth->init_duration);
	if (duration_state == cloth->duration_state) {
		return 0;
	}
	
	TRACE_LOG("clothes duration state[%u %u],[%u %u]", p->id, cloth->clothes_id, cloth->duration_state, duration_state);
	cloth->duration_state = duration_state;
	
	//calc_player_def_atk(p);
	p->atk       = calc_player_atk_attr(p);
	p->def_value = calc_player_def_value_attr(p); 
	p->def_rate  = calc_player_def_rate_attr(p);
	
	p->atk       += p->atk_buf_trim;
	p->def_rate  += p->def_buf_trim;
	

	// notify the player the clothes duration state change
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf, cloth->unique_id, idx);
	pack(pkgbuf, static_cast<uint32_t>(cloth->duration), idx);
	pack(pkgbuf, static_cast<uint32_t>(cloth->duration_state), idx);

	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), p->id, cli_proto_duration_state_chg, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	send_to_player(p, pkgbuf, idx, 0);

	return 0;
}

/**
 * @brief player's clothes duration change in a battle
 */
void clothes_copy(player_clothes_info_t* p_clothes, const player_clothes_base_info_t* p_base_info, uint32_t init_duration)
{
	p_clothes->clothes_id = p_base_info->clothes_id;
	p_clothes->unique_id = p_base_info->unique_id;
	p_clothes->duration = p_base_info->duration;
	p_clothes->lv = p_base_info->lv;
	p_clothes->gettime = p_base_info->gettime;
	p_clothes->timelag = p_base_info->timelag;
	p_clothes->init_duration = init_duration;
	p_clothes->duration_state = get_cloth_duration_state(p_clothes->duration, p_clothes->init_duration);
	TRACE_LOG("clothes info[%u %u %u %u]", p_clothes->clothes_id, p_clothes->duration,p_clothes->init_duration,
		   	p_clothes->duration_state);
}

/**
 * @brief player's clothes duration change in a battle
 */
void player_clothes_init(Player* p, const player_attr_t* attr)
{
	memset(&(p->clothes_info), 0, sizeof(p->clothes_info));
	p->clothes_info.clothes_cnt = attr->clothes_num;
	TRACE_LOG("clothes cnt[%u %u]", p->id, p->clothes_info.clothes_cnt);

/*	//debug:
	for (uint32_t i = 0; i < attr->clothes_num; i++) {
		DEBUG_LOG("debug bf init: player=%u, clothes[%u %u %u %u %u %u]", p->id, attr->clothes[i].clothes_id, 
				attr->clothes[i].unique_id, attr->clothes[i].duration, attr->clothes[i].lv, attr->clothes[i].gettime, attr->clothes[i].timelag);
	}*/

	uint8_t flag[max_clothes_on] = {0};
	//weapon
	for (uint32_t i = 0; i < attr->clothes_num; i++) {
		const GfItem* itm = items->get_item(attr->clothes[i].clothes_id);
		if (!itm) {
			WARN_LOG("item error: p[%u], clothes_id[%u]", p->id, attr->clothes[i].clothes_id);
			continue;
		}
		if (items->is_atk_clothes(itm->equip_part())) {
			int idx = p->clothes_info.weapon_cnt;
			clothes_copy(&(p->clothes_info.clothes[idx]), &(attr->clothes[i]), itm->duration * clothes_duration_ratio);
			p->clothes_info.weapon_cnt = 1;
			flag[i] = 1;
			break;
		}
	}
	
	//armor
	for (uint32_t i = 0; i < attr->clothes_num; i++) {
		if (flag[i]) {
			continue;
		}
		const GfItem* itm = items->get_item(attr->clothes[i].clothes_id);
		if (!itm) {
			WARN_LOG("item error: p[%u], clothes_id[%u]", p->id, attr->clothes[i].clothes_id);
			continue;
		}
		if (items->is_def_clothes(itm->equip_part())) {
			int idx = p->clothes_info.weapon_cnt + p->clothes_info.armor_cnt;
			clothes_copy(&(p->clothes_info.clothes[idx]), &(attr->clothes[i]), itm->duration * clothes_duration_ratio);
			p->clothes_info.armor_cnt++;
			flag[i] = 1;
		}
	}
	
	//jewwlry
	for (uint32_t i = 0; i < attr->clothes_num; i++) {
		if (flag[i]) {
			continue;
		}
		const GfItem* itm = items->get_item(attr->clothes[i].clothes_id);
		if (!itm) {
			WARN_LOG("item error: p[%u], clothes_id[%u]", p->id, attr->clothes[i].clothes_id);
			continue;
		}
		if (items->is_jew_clothes(itm->equip_part())) {
			int idx = p->clothes_info.weapon_cnt + p->clothes_info.armor_cnt + p->clothes_info.jewelry_cnt;
			clothes_copy(&(p->clothes_info.clothes[idx]), &(attr->clothes[i]), itm->duration * clothes_duration_ratio);
			p->clothes_info.jewelry_cnt++;
			flag[i] = 1;
		}
	}
	
	//other
	uint32_t other_cnt = 0;
	for (uint32_t i = 0; i < attr->clothes_num; i++) {
		if (flag[i]) {
			continue;
		}
		int idx = p->clothes_info.weapon_cnt + p->clothes_info.armor_cnt + p->clothes_info.jewelry_cnt + other_cnt;
		clothes_copy(&(p->clothes_info.clothes[idx]), &(attr->clothes[i]), 0);
		other_cnt++;
	}
/*	//debug:
	for (uint32_t i = 0; i < p->clothes_info.clothes_cnt; i++) {
		DEBUG_LOG("debug af cnt: player=%u, clothes[%u %u %u %u %u %u]", p->id, 
            p->clothes_info.clothes[i].clothes_id, p->clothes_info.clothes[i].unique_id, 
            p->clothes_info.clothes[i].duration, p->clothes_info.clothes[i].lv,
            p->clothes_info.clothes[i].gettime,p->clothes_info.clothes[i].timelag);
	}*/


	TRACE_LOG("clothes cnt[%u %u %u %u %u]", p->id, p->clothes_info.weapon_cnt, p->clothes_info.armor_cnt, 
			p->clothes_info.jewelry_cnt, other_cnt);
}

/**
 * @brief player's clothes duration change in a battle
 */
void player_clothes_duration_chg(Player* p, uint32_t duration_type)
{
	TRACE_LOG("clothes cnt[%u %u %u %u]", p->id, p->clothes_info.weapon_cnt, p->clothes_info.armor_cnt, 
			p->clothes_info.jewelry_cnt);
	int idx = 0;
	if (duration_type == weapon) {
		if (p->clothes_info.weapon_cnt != 0) {
			player_clothes_info_t* cloth = &(p->clothes_info.clothes[idx]);
			//judge odds
			uint32_t rand_num = rand() % 100;
			uint32_t rand_odds = 100;
			const GfItem* itm = items->get_item(cloth->clothes_id);
			if (itm) {
				switch(itm->quality_lv) {
					case 2:
						rand_odds = 85;
						break;
					case 3:
						rand_odds = 60;
						break;
					case 4:
						rand_odds = 50;
						break;
				}
			}
			if (rand_num < rand_odds){
				cloth->duration = cloth->duration > weapon_loss ? cloth->duration - weapon_loss : 0;
				TRACE_LOG("clothes loss duration[%u %u],[%u %u]", p->id, cloth->clothes_id, weapon_loss, cloth->duration);
				cloth->duration_chg_flag = 1;
	
				duration_state_chg_noti(p, cloth);
			}
		}
	} else if (duration_type == jewelry) {
		for (uint32_t i = 0; i < p->clothes_info.jewelry_cnt; i++) {
			idx = p->clothes_info.weapon_cnt + p->clothes_info.armor_cnt + i;
			player_clothes_info_t* cloth = &(p->clothes_info.clothes[idx]);
			cloth->duration = cloth->duration > jewelry_loss ? cloth->duration - jewelry_loss : 0;
			TRACE_LOG("clothes loss duration[%u %u],[%u %u]", p->id, cloth->clothes_id, jewelry_loss, cloth->duration);
			cloth->duration_chg_flag = 1;
			
			duration_state_chg_noti(p, cloth);
		}	
	} else if (duration_type == all_clothes) {
		for (uint32_t i = 0; i < p->clothes_info.clothes_cnt; i++) {
			player_clothes_info_t* cloth = &(p->clothes_info.clothes[i]);
			cloth->duration -= cloth->duration / 5;
			cloth->duration_chg_flag = 1;

			duration_state_chg_noti(p, cloth);
		}

	} else {
		if (p->clothes_info.armor_cnt != 0) {
			/*int i = rand() % p->clothes_info.armor_cnt;
			idx = p->clothes_info.weapon_cnt + i;
			uint32_t duration_loss = 0;
			if (duration_type == armor_normal) {
				duration_loss = armor_normal_loss;
			} else if (duration_type == armor_crit) {
				duration_loss = armor_crit_loss;
			} else {
				duration_loss = armor_boss_loss;
			}
			player_clothes_info_t* cloth = &(p->clothes_info.clothes[idx]);
			cloth->duration = cloth->duration > duration_loss ? cloth->duration - duration_loss : 0;
			TRACE_LOG("clothes loss duration[%u %u],[%u %u]", p->id, cloth->clothes_id, duration_loss, cloth->duration);
			cloth->duration_chg_flag = 1;
			
			duration_state_chg_noti(p, cloth);*/
			for (uint32_t i = 0; i < p->clothes_info.armor_cnt; i++) {
				int idx = p->clothes_info.weapon_cnt + i;
				uint32_t duration_loss = armor_normal_loss;
				player_clothes_info_t* cloth = &(p->clothes_info.clothes[idx]);

				uint32_t rand_num = rand() % 100;
				uint32_t rand_odds = 35;
				const GfItem* itm = items->get_item(cloth->clothes_id);
				if (itm) {
					switch(itm->quality_lv) {
						case 2:
							rand_odds = 32;
							break;
						case 3:
							rand_odds = 28;
							break;
						case 4:
							rand_odds = 23;
							break;
					}
				}
				if (rand_num > rand_odds){
					continue;
				}

				cloth->duration = cloth->duration > duration_loss ? cloth->duration - duration_loss : 0;
				TRACE_LOG("clothes loss duration[%u %u],[%u %u]", p->id, cloth->clothes_id, duration_loss, cloth->duration);
				cloth->duration_chg_flag = 1;
			
				duration_state_chg_noti(p, cloth);
			}
		}
	}
}


/**
 * @brief calc player's def & atk by clothes
 */
void calc_player_def_atk(Player* p)
{
	p->def_rate      = calc_def_rate(p->lv);
	p->weapon_atk[0] = 0;
	p->weapon_atk[1] = 0;

	p->def_value = calc_def_value(p->lv);

	uint32_t itms_def = 0;
    uint32_t low_atk = 0, high_atk = 0;
    int atk = 0;
	for (uint32_t i = 0; i < p->clothes_info.clothes_cnt; i++) {
		const player_clothes_info_t* clothes = &(p->clothes_info.clothes[i]);
		const GfItem* itm = items->get_item(clothes->clothes_id);
		
		itms_def += itm->def - itm->def * clothes->duration_state / 100;
		
		// 4 - weapon
		if (items->is_atk_clothes(itm->equip_part())) {
            if (clothes->lv >= 1) {
                atk += calc_weapon_add_atk(itm->use_lv(), clothes->lv, itm->quality_lv);
            }
            low_atk = itm->atk[0] + atk;
            high_atk = itm->atk[1] + atk;
			p->weapon_atk[0] = low_atk - low_atk * clothes->duration_state / 100;
			p->weapon_atk[1] = high_atk - high_atk * clothes->duration_state / 100; 
			//p->weapon_atk[0] = itm->atk[0] - itm->atk[0] * clothes->duration_state / 100;
			//p->weapon_atk[1] = itm->atk[1] - itm->atk[1] * clothes->duration_state / 100;
		}
	}
	itms_def += p->suit_add_attr.def;
	p->def_rate += itms_def  / static_cast<double>(p->lv + 50) * 0.09;
	p->def_rate = 1 -  p->def_rate;
	p->def_value += itms_def;
}
/**
 * @brief calc player's Attribute by attire strengthen level
 */
void calc_player_strengthen_attire(Player* p)
{
	//static float crit_para = (26.3)/ pow(p->lv, 1.32);
	static float hit_para = (25)/ pow(p->lv, 1.32) / 100;
	//static float dodge_para = (26.3) / pow(p->lv, 1.32) / 100;

    int  agility = 0, strength = 0, hit = 0, def = 0, body = 0, hp = 0, dodge = 0;
    for (uint32_t i = 0; i < p->clothes_info.clothes_cnt; i++) {
        const player_clothes_info_t* clothes = &(p->clothes_info.clothes[i]);
        const GfItem* itm = items->get_item(clothes->clothes_id);
        //TRACE_LOG("=======================================================");
        //TRACE_LOG("strengthen[%u %u %u]",itm->use_lv(), clothes->lv, itm->quality_lv);
        if ( items->is_atk_clothes(itm->equip_part()) && clothes->lv >= 4 ) {
            //atk      += calc_weapon_add_atk(itm->use_lv(), clothes->lv, itm->quality_lv);
            agility  += calc_weapon_add_agility(itm->use_lv(), clothes->lv, itm->quality_lv);
            strength += calc_weapon_add_strength(itm->use_lv(), clothes->lv, itm->quality_lv);
            hit      += calc_weapon_add_hit(itm->use_lv(), clothes->lv, itm->quality_lv);
            TRACE_LOG("strengthen:[%u %u %u]", agility, strength, hit);
        }
        if ( items->is_def_clothes(itm->equip_part()) && clothes->lv >= 1 ) {
            def   += calc_clothes_add_defense(clothes->lv);
            body  += calc_clothes_add_body(itm->use_lv(), clothes->lv, itm->quality_lv);
        	//hp    += calc_clothes_add_hp(itm->use_lv(), clothes->lv, itm->quality_lv);
        	p->clothes_addhp += calc_clothes_add_hp(itm->use_lv(), clothes->lv, itm->quality_lv);
            dodge += calc_clothes_add_dodge(itm->use_lv(), clothes->lv, itm->quality_lv);
            TRACE_LOG("strengthen:[%u %u %u %u]", def, body, hp, dodge);
        }
    }
    //hp    += calc_hp[p->role_type](body);
    //atk   += calc_atk(strength);
    //dodge += calc_dodge_rate(agility);

    p->maxhp        += hp;
    //p->atk          += atk;
    p->agility_      += agility;
    p->strength     += strength;
    //p->hit_rate     += hit / 2000.0;
    p->hit_rate     += hit * hit_para;
    p->def_rate     += def;
    p->body_quality_ += body;
    //p->dodge_rate   += dodge * dodge_para;
}
/**
  * @brief return bitpos of unique items 
  * @return 0 none
  */
uint32_t get_unique_item_bitpos(uint32_t item_id)
{
	std::map<uint32_t, unique_item_pos_t>::iterator iter = unique_item_map.find(item_id);
	if (iter == unique_item_map.end()) {
		return 0;
	}
	return iter->second.bit_pos;
	
}

/**
  * @brief a timer function to kick user offline if max online time expires
  * @return 0
  */
int set_holiday_drop_start(void* owner, void* data)
{
	DEBUG_LOG("start holiday drop type=%lu", (unsigned long)(data));
	unsigned long type = (unsigned long)(data);
	if (type < max_holiday_drop_type) {
		holiday_drop_arr[type].start_flg = true;
	}
	return 0;
}

int set_holiday_drop_end(void* owner, void* data)
{
	DEBUG_LOG("end holiday drop type=%p", data);
	unsigned long type = (unsigned long)data;
	if (type < max_holiday_drop_type) {
		holiday_drop_arr[type].start_flg = false;
	}
	return 0;
}

void add_holiday_timer()
{
	for (uint32_t i = 0; i < max_holiday_drop_type; i++) {
		holiday_drop_t* p_drop = &(holiday_drop_arr[i]);
		if (p_drop->cnt) {
			if (get_now_tv()->tv_sec < p_drop->start_time) {
				ADD_TIMER_EVENT(&g_events, set_holiday_drop_start, reinterpret_cast<void*>(i), p_drop->start_time);
				ADD_TIMER_EVENT(&g_events, set_holiday_drop_end, reinterpret_cast<void*>(i), p_drop->end_time);
			} else if (get_now_tv()->tv_sec > p_drop->start_time && get_now_tv()->tv_sec < p_drop->end_time) {
				p_drop->start_flg = true;
				ADD_TIMER_EVENT(&g_events, set_holiday_drop_end, reinterpret_cast<void*>(i), p_drop->end_time);
			} else if (get_now_tv()->tv_sec >= p_drop->end_time) {
			}
		}
	}
}


//------------------------------------------
//load xml files

/**
  * @brief load unique items from an xml file
  * @return 0 on success, -1 on error
  */
int load_unique_items(xmlNodePtr cur)
{
	// load unique items from xml file
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("UniqueDrop"))) {
			uint32_t id;
			uint32_t bit_pos;
			get_xml_prop(id, cur, "ID");
			get_xml_prop(bit_pos, cur, "Bit_Pos");

			if (unique_item_map.size() > 100) {
				ERROR_LOG("unique items to many %u ", (uint32_t)unique_item_map.size());
				return -1;
			}
			if (unique_item_map.find(id) != unique_item_map.end()) {
				ERROR_LOG("id:%u  defined twice", id);
				return -1;
			}
			unique_item_pos_t item(id, bit_pos);
			unique_item_map.insert(std::map<uint32_t, unique_item_pos_t>::value_type(id, item));
			TRACE_LOG("unique_item_map: %u %u %u", id, item.item_id, item.bit_pos);
		}
		cur = cur->next;
	}
	return 0;
}


void clear_suit_arr()
{
	memset(suit_arr, 0x0, sizeof(suit_arr));
}

/**
  * @brief load suit addition configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_cp_addition(xmlNodePtr cur, struct suit_addition_t* p_addition)
{
    if ( !xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("CpAddition")) ) {
        get_xml_prop(p_addition->skill_id, cur, "SkillID");

        get_xml_prop_def(p_addition->cut_cd, cur, "CutCD", 0);
        get_xml_prop_def(p_addition->cut_mp, cur, "CutMp", 0);
        get_xml_prop_def(p_addition->attr_dmg, cur, "AttrDmg", 0);
        get_xml_prop_def(p_addition->extra_dmg, cur, "ExtraDmg", 0);

        TRACE_LOG("suit addition [%u] [%u %u %u %u]", p_addition->skill_id, p_addition->cut_cd,
            p_addition->cut_mp, p_addition->attr_dmg, p_addition->extra_dmg);

    }
    return 0;
}

/**
  * @brief load suit step configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_an_suit(xmlNodePtr cur, struct suit_t* p_suit)
{
	uint32_t i = 0;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Step"))) {
			if (i >= max_suit_step) {
				ERROR_LOG("suit step id[%u] steps too many", p_suit->id);
				return -1;
			}
			uint32_t step_cnt = 0;
			get_xml_prop(step_cnt, cur, "Cnt");
			if (step_cnt > max_suit_step) {
				ERROR_LOG("itemid[%u]", p_suit->id);
				return -1;	
			}
			suit_step_t* p_step = &(p_suit->suit_step[step_cnt]);
			p_step->attire_num = step_cnt;
			//base attr
			get_xml_prop_def(p_step->strength, cur, "Strength", 0);
			get_xml_prop_def(p_step->agility, cur, "Agility", 0);
			get_xml_prop_def(p_step->body_quality, cur, "BodyQuality", 0);
			get_xml_prop_def(p_step->stamina, cur, "Stamina", 0);
			//attr level 2
			get_xml_prop_def(p_step->atk, cur, "Atk", 0);
			get_xml_prop_def(p_step->def, cur, "Def", 0);
			get_xml_prop_def(p_step->hit, cur, "Hit", 0);
			get_xml_prop_def(p_step->dodge, cur, "Dodge", 0);
			get_xml_prop_def(p_step->crit, cur, "Crit", 0);
			get_xml_prop_def(p_step->hp, cur, "Hp", 0);
			get_xml_prop_def(p_step->mp, cur, "Mp", 0);
			get_xml_prop_def(p_step->add_hp, cur, "AddHp", 0);
			get_xml_prop_def(p_step->add_mp, cur, "AddMp", 0);
			get_xml_prop_def(p_step->skill_atk, cur, "SkillAtk", 0);
	
            // load configs for each step
			if (load_cp_addition(cur->xmlChildrenNode, &(p_step->addition) ) == -1) {
				throw XmlParseError(std::string("failed to load an suit addition information"));
				return -1;
			}

			TRACE_LOG("suit id %u [%u %u %u %u %u %u %u %u %u %u %u %u %u]", 
				p_suit->id, 
				p_step->attire_num, 
				p_step->strength, 
				p_step->agility, 
				p_step->body_quality, 
				p_step->stamina, 
				p_step->atk, 
				p_step->def, 
				p_step->hit, 
				p_step->dodge, 
				p_step->crit, 
				p_step->add_hp, 
				p_step->add_mp, 
				p_step->skill_atk);
			
			i++;
		}
		cur = cur->next;
	}
	if (i == 0) {
		ERROR_LOG("suit[%u] step is 0", p_suit->id);
		return -1;
	}
	p_suit->cnt = i;
	return 0;
}

/**
  * @brief load suit configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_suit(xmlNodePtr cur)
{
	clear_suit_arr();
		
	// load items from an xml file
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Item"))) {
			uint32_t id = 0;
			get_xml_prop(id, cur, "ID");
			if (id > max_suit_id) {
				ERROR_LOG("suit id:[%u] max[%u]", id, max_suit_id);
				throw XmlParseError(std::string("suit id invaild"));
				return -1;
			}
			suit_t* p_suit = suit_arr + id;
			p_suit->id = id;
			// load configs for each step
			if (load_an_suit(cur->xmlChildrenNode, p_suit) == -1) {
				throw XmlParseError(std::string("failed to load an suit"));
				return -1;
			}
		}
		cur = cur->next;
	}
	return 0;
}

int check_item_suit(const GfItem& itm, void* in)
{
	uint32_t set_id = itm.suit_id();
	
	if (set_id > 0) {
		if (set_id > max_suit_id || !(suit_arr[set_id].id)) {
			ERROR_LOG("suit id not found: %u %u ", itm.id(), set_id);
			return -1;
		}
	}
	return 0;
}


/**
  * @brief load an box item configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_an_box_item(xmlNodePtr cur, treasure_box_t* p_box)
{
	int i = 0;
	uint32_t odd_begin = 0;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Item"))) {
			if (i >= max_box_items) {
				ERROR_LOG("box item cnts[%u] too many", i);
				return -1;
			}
			treasure_box_item_t* p_item = &(p_box->items[i]);

			uint32_t odds = 0;
			get_xml_prop(p_item->give_type, cur, "give_type");
			if (p_item->give_type == give_type_clothes) {
				get_xml_prop(p_item->equip_part, cur, "equip_part");	
			}
			get_xml_prop(p_item->give_id, cur, "give_id");
			get_xml_prop(p_item->give_cnt, cur, "cnt");
			get_xml_prop(odds, cur, "odds");
            if (p_item->give_type == 1 || p_item->give_type == 2) {
                const GfItem* itm = items->get_item(p_item->give_id);
                if ( (p_item->give_type == 1 && (p_item->give_id > 10 || p_item->give_id == 0))  
                    || ( p_item->give_type == 2 && !itm ) ) {
                    ERROR_LOG("treasure_box.xml give_type:%u -- give_id:%u",p_item->give_type,p_item->give_id);
                    return -1;
                }
            }
			p_item->odds_lower = odd_begin;
			p_item->odds_upper = odds + odd_begin;
			odd_begin += odds;
			if (odd_begin > 100) {
				ERROR_LOG("box item odds too large[%u %u]", odd_begin, p_box->id);
				return -1;
			}
			i++;
		}
		cur = cur->next;
	}
	p_box->items_cnt = i;
	return 0;
}

/**
  * @brief load treasure box configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_treasure_box(xmlNodePtr cur)
{
	// load treasure box from xml file
	memset(treasure_boxs, 0x00, sizeof(treasure_boxs));
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Box"))) {
			uint32_t id;
			uint32_t quality;
			//uint32_t lv;
			get_xml_prop(id, cur, "ID");
			get_xml_prop(quality, cur, "Quality");
			//get_xml_prop(lv, cur, "Lv");

			/*if ( (quality > max_box_quality || quality <= 0)
				|| (lv > max_box_lv || lv <= 0) ) {
				ERROR_LOG("qualit or lv than max: %u %u", quality, lv);
				return -1;
			}*/
			if (id > max_treasure_box_cnt || treasure_boxs[id - 1].id != 0) {
				ERROR_LOG("treasure box id error:%u", id);
				return -1;
			}
			treasure_boxs[id - 1].id = id;
			treasure_boxs[id - 1].quality = quality;
			//treasure_boxs[id - 1].lv	  = lv;

			treasure_box_t* p_box = &(treasure_boxs[id - 1]);
			// load configs for each item
			if (load_an_box_item(cur->xmlChildrenNode, p_box) == -1) {
				throw XmlParseError(std::string("failed to load an box item"));
				return -1;
			}
		}
		cur = cur->next;
	}

	//log
	for (int i = 0; i < max_treasure_box_cnt; i++) {
		for (int k = 0; k < max_box_items; k++) {
			treasure_box_item_t* p_item = &(treasure_boxs[i].items[k]);
			TRACE_LOG("treasure box[%u] items[%u %u %u [%u %u]]", i, p_item->give_type, 
					p_item->give_id, p_item->give_cnt, p_item->odds_lower, p_item->odds_upper);
		}
	}
	return 0;
	
}

/**
  * @brief load an stage box configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_an_stage_boxs(xmlNodePtr cur, stage_treasure_box_t* p_box)
{
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Score"))) {
			uint32_t id = 0;
			get_xml_prop(id, cur, "ID");
			if (id > score_max_cnt) {
				ERROR_LOG("score:id too large[%u]", id);
				return -1;
			}
			box_id_info_t* p_info = &(p_box->boxs[id - 1]);

			p_info->box_cnt = get_xml_prop_arr(p_info->box_id, cur, "Boxs");
			if (p_info->box_cnt > max_stage_box_cnt) {
				ERROR_LOG("stage:box cnt too large[%u %u]", id, p_info->box_cnt);
				return -1;
			}
			//test box id
			for (uint32_t i = 0; i < p_info->box_cnt; i++) {
				uint32_t box_id = p_info->box_id[i];
				if (box_id > max_treasure_box_cnt || treasure_boxs[box_id - 1].id == 0) {
					ERROR_LOG("stage:box id error[%u %u]", id, box_id);
					return -1;
				}
			}
		}
		cur = cur->next;
	}
	return 0;
}

/**
  * @brief load stage box configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_stage_boxs(xmlNodePtr cur)
{
	// load treasure box from xml file
	memset(stage_boxs, 0x00, sizeof(stage_boxs));
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("StagesBox"))) {
			uint32_t id = 0;
			get_xml_prop(id, cur, "ID");
			if (id > stage_max_num || stage_boxs[id - 1].id != 0) {
				ERROR_LOG("stage box id too large[%u]", id);
				return -1;
			}
			stage_boxs[id - 1].id = id;
			get_xml_prop(stage_boxs[id - 1].drop_lv, cur, "DropLv");
			
			if (load_an_stage_boxs(cur->xmlChildrenNode, &(stage_boxs[id - 1])) == -1) {
				throw XmlParseError(std::string("failed to load an stage box"));
				return -1;
			}
		}
		cur = cur->next;
	}


	//log
	for (uint32_t i = 0; i < stage_max_num; i++) {
		if (stage_boxs[i].id != 0) {
			TRACE_LOG("stage box:[%u %u]",stage_boxs[i].id, stage_boxs[i].drop_lv);
			for (uint32_t k = 0; k < score_max_cnt; k++) {
				box_id_info_t* info = &(stage_boxs[i].boxs[k]);
				for (uint32_t j = 0; j < info->box_cnt; j++) {
					TRACE_LOG("[%u]:[%u]", k, info->box_id[j]);
				}
			}
		}
	}
	return 0;
}

/**
  * @brief load vip box configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_vip_boxs(xmlNodePtr cur)
{
	// load treasure box from xml file
	memset(vip_boxs, 0x00, sizeof(vip_boxs));
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("VipBox"))) {
			uint32_t lv = 0;
			get_xml_prop(lv, cur, "Lv");
			if (lv > vip_max_lv || vip_boxs[lv - 1].vip_lv != 0) {
				ERROR_LOG("vip box lv too large[%u]", lv);
				return -1;
			}
			vip_boxs[lv - 1].vip_lv = lv;
			
			box_id_info_t* p_info = &(vip_boxs[lv - 1].boxs);

			p_info->box_cnt = get_xml_prop_arr(p_info->box_id, cur, "Boxs");
			if (p_info->box_cnt > max_stage_box_cnt) {
				ERROR_LOG("vip:box cnt too large[%u %u]", lv, p_info->box_cnt);
				return -1;
			}
			//test box id
			for (uint32_t i = 0; i < p_info->box_cnt; i++) {
				uint32_t box_id = p_info->box_id[i];
				if (box_id > max_treasure_box_cnt || treasure_boxs[box_id - 1].id == 0) {
					ERROR_LOG("vip:box id error[%u %u]", lv, box_id);
					return -1;
				}
			}
		}
		cur = cur->next;
	}
	
	//log
	for (uint32_t i = 0; i < vip_max_lv; i++) {
		TRACE_LOG("vip box:[%u]",vip_boxs[i].vip_lv);
		box_id_info_t* info = &(vip_boxs[i].boxs);
		for (uint32_t j = 0; j < info->box_cnt; j++) {
			TRACE_LOG("[%u]",info->box_id[j]);
		}
	}
	return 0;
}



int load_an_holiday_drop(xmlNodePtr cur, holiday_drop_t* p_one_drop)
{
	int i = 0;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Drop"))) {
			holiday_drop_item_t* p_drop_item = &(p_one_drop->drop_item[i]);

			if (i > 10) {
				ERROR_LOG("too many drop item: starttime=%u endtime=%u", p_one_drop->start_time, p_one_drop->start_time);
				return -1;
			}
			uint32_t id = 0;
			get_xml_prop(id, cur, "ID");
			const GfItem* itm = items->get_item(id);
			if (!itm) {
				ERROR_LOG("item not exsit id[%u]", id);
				return -1;
			}
			p_drop_item->itemid = id;
			size_t n = get_xml_prop_arr(p_drop_item->mon_lv, cur, "MonLv");
			if (n != 2) {
				ERROR_LOG("invalid level config: itemid=%u lv_num=%u", id, (uint32_t)n);
				return -1;
			}
			get_xml_prop(p_drop_item->drop_odds, cur, "DropOdds");
			i++;
		}
		cur = cur->next;
	}
	p_one_drop->cnt = i;
	return 0;
}

int load_holiday_drop(xmlNodePtr cur)
{
	memset(holiday_drop_arr, 0x00, sizeof(holiday_drop_arr));
	cur = cur->xmlChildrenNode;
	uint32_t i = 0;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Time"))) {
			uint32_t id = 0;
			get_xml_prop(id, cur, "ID");
			if (id > max_holiday_drop_type || id != i + 1) {
				ERROR_LOG("stage box id too large or not continues[%u %u]", id, i + 1);
				return -1;
			}
			
			char fromstr[128] = "";
			time_t tmptime;
			get_xml_prop_raw_str(fromstr, cur, "From");
			if (!date2timestamp(fromstr, "%Y-%m-%d %H:%M:%S", tmptime)) {
				ERROR_LOG("time format invalid[%s]", fromstr);
				return -1;
			}
			holiday_drop_arr[id - 1].start_time = tmptime;
			char tostr[128] = "";
			get_xml_prop_raw_str(tostr, cur, "To");
			if (!date2timestamp(tostr, "%Y-%m-%d %H:%M:%S", tmptime)) {
				ERROR_LOG("time format invalid[%s]", tostr);
				return -1;
			}
			holiday_drop_arr[id - 1].end_time= tmptime;

			if (holiday_drop_arr[id - 1].end_time <= holiday_drop_arr[id - 1].start_time) {
				throw XmlParseError(std::string("start end time err"));
				return -1;
			}
			if (load_an_holiday_drop(cur->xmlChildrenNode, &(holiday_drop_arr[id - 1])) == -1) {
				throw XmlParseError(std::string("failed to load an stage box"));
				return -1;
			}
			i++;
		}
		cur = cur->next;
	}
	//log
	for (uint32_t i = 0; i < max_holiday_drop_type; i++) {
		TRACE_LOG("holiday_drop:[%u %u %u %u]", holiday_drop_arr[i].cnt, holiday_drop_arr[i].start_time, 
			holiday_drop_arr[i].end_time, holiday_drop_arr[i].start_flg);
		
		for (uint32_t j = 0; j < holiday_drop_arr[i].cnt; j++) {
			TRACE_LOG("[%u %u %u %u]", holiday_drop_arr[i].drop_item[j].itemid, 
				holiday_drop_arr[i].drop_item[j].drop_odds,
				holiday_drop_arr[i].drop_item[j].mon_lv[0],
				holiday_drop_arr[i].drop_item[j].mon_lv[1]);
		}
	}
	return 0;

}

/*! the pointer to ItemManager for handling all items*/
GfItemManager* items;

