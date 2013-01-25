#ifdef __cplusplus
extern "C"
{
#endif

#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>
#include <libtaomee/timer.h>
#include <libtaomee/utils.h>
#include <libtaomee/conf_parser/config.h>
//#include  <statistic_agent/msglog.h>
#include "libtaomee/project/stat_agent/msglog.h"
#ifdef __cplusplus
}
#endif

#include <map>
#include "utils.h"
#include "bt_skill.h"
#include "bt_online.h"
#include "bt_bat_check.h"
#include "bt_timerid.h"
#include "global.h"
#include "bt_beastai.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libtaomee++/proto/Ccmd_map.h>
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>
#include <libtaomee++/utils/strings.hpp>
#include    "global_not_reload.h"

void report_battle_end (battle_info_t* abi);
void notify_user_out_time_out ( battle_info_t* abi, warrior_t* lp );
//-------------------------------------------------------
//函数定义
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    int proto_name( userid_t uid, uint32_t online_id, battle_info_t* abi , Cmessage* p_in ) ;
#include "./proto/mole2_btl_bind_for_client.h"

//-------------------------------------------------------------
//对应的结构体
#include "./proto/mole2_btl.h"
//-------------------------------------------------------------
//命令绑定
typedef   int(*P_DEALFUN_T)( userid_t uid, uint32_t online_id, battle_info_t* abi, Cmessage* p_in );

#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    {cmdid, new (c_in), md5_tag,bind_bitmap ,proto_name },

Ccmd< P_DEALFUN_T> g_cmd_list[]={
#include "./proto/mole2_btl_bind_for_client.h"
};


//命令map
Ccmd_map< Ccmd< P_DEALFUN_T> >  g_cmd_map;

#include "./proto/mole2_btl_enum.h"
#include "./proto/mole2_btl.h"


void read_from_struct_warrior(   warrior_t   *p ,stru_warrior_t  & w  );

extern timer_cb_func_t timer_funcs[1000];

int race_restrict[10][9] = { \
//	ren, kun,  zhi,  te, long, shou, fei, bu,  jin 
	{ 0,  1,  0,  0,  0,  2,  0, -2, -1}, \
	{-1,  0,  1,  2,  0,  0, -2,  0,  0}, \
	{ 0, -1,  0,  1,  0, -2,  0,  2,  0}, \
	{ 0, -2, -1,  0,  1,  0,  0,  0,  2}, \
	{ 0,  0,  0, -1,  0,  1,  2,  0, -2}, \
	{-2,  0,  2,  0, -1,  0,  1,  0,  0}, \
	{ 0,  2,  0,  0, -2, -1,  0,  1,  0}, \
	{ 2,  0, -2,  0,  0,  0, -1,  0,  1}, \
	{ 1,  0,  0, -2,  2,  0,  0, -1,  0}
};
struct user_exp_level_t{
	int32_t add_exp;//用户可增加多少经验
	uint32_t low_level_protect_exp;//用户可为高级用户增加多少守护经验(低级用户使用)
	uint32_t level;
	uint32_t is_need_add_protect_exp;
	uint32_t add_protect_exp;
};


enum{
	item_hp=0,
	item_revive=1,
};

static void remove_warrior_loading_timer(warrior_t* p)
{
	REMOVE_TIMERS(p);
}

static inline void free_battle_info(void* btr_info)
{
	battle_info_t* p = (battle_info_t*)btr_info;
	REMOVE_TIMERS(p);

	if (p->fight_timer){
		DEBUG_LOG("RM ATK TM\t[batid=%lu %p]", p->batid, p->fight_timer);
		REMOVE_TIMER(p->fight_timer);
		p->fight_timer = NULL;
	}

	if (p->load_timer){
		DEBUG_LOG("RM LOAD TM\t[batid=%lu %p]", p->batid, p->load_timer);
		REMOVE_TIMER(p->load_timer);
		p->load_timer= NULL;
	}

	list_head_t* next;
	list_head_t* cur;
	list_for_each_safe(cur, next, &p->attack_list) {
		resolved_atk_t* p_resolved_atk = list_entry(cur, resolved_atk_t, atk_list);
		list_del(&p_resolved_atk->atk_list);
		g_slice_free1(sizeof *p_resolved_atk, p_resolved_atk);
	}

	list_for_each_safe(cur, next, &p->warrior_list){
		warrior_t* lp_warrior = list_entry(cur, warrior_t, waor_list);
		if (lp_warrior->huwei_pos){
			remove_warrior_from_huweilist(lp_warrior);
		}
		remove_warrior_loading_timer(lp_warrior);
		delete lp_warrior->p_waor_state;
	}

	g_slice_free1(sizeof *p, p);
}

void free_batter_by_id(battle_id_t btid)
{
	DEBUG_LOG("free_batter_by_id=%lu",btid );

	battle_info_t* pbi    = (battle_info_t*)g_hash_table_lookup(battles,&btid );
	if (pbi){
		free_battle_info(pbi);
	}
	g_hash_table_remove(battles, &(btid));
}

uint8_t init_battles()
{
	battles = g_hash_table_new(g_int64_hash, g_int64_equal );
	return 0;
}


#define     entry_len (sizeof(warrior_base_t))
//得到存放宠物的位置
static int set_pet_warrior_pos(int personpos, battle_users_t* abc, const warrior_t* aw ,uint32_t btmode)
{
	int pos = (personpos >= MAX_PLAYERS_PER_TEAM)?(personpos - MAX_PLAYERS_PER_TEAM):personpos;
	int old_pos = pos;
	
	switch (aw->pet_state) {
		case on_fuzhu:
			pos = !abc->players[1].userid ? 1 : (!abc->players[2].userid ? 2 : -1);
			//双人组队时 可以带6个宠物
			if(btmode == 3) {
				//4 2 0 1 3
				//9 7 5 6 8 
				//队长及宠物位置5 0 1 6
				//队员及宠物位置6 1 3 8
				if(personpos==5 || personpos==0){
					//把队长辅宠放在2 7
					if(!abc->players[2].userid)
						pos=2;
					else if(!abc->players[7].userid)
						pos=7;
				}
				if(personpos==6 || personpos==1){
					//把队员的辅宠放在 3 8 
					if(!abc->players[8].userid)
						pos=8;
					else if(!abc->players[3].userid)
						pos=3;
				}
			}	
			break;
		case rdy_fight:
			pos = !abc->players[pos].userid ? pos : (!abc->players[pos + 5].userid ? pos + 5 : -1);
			if(btmode == 3 &&  !abc->players[1].userid) {
			//双人组队时队员6的主宠设在1
				if(personpos==6 && !abc->players[1].userid){
						pos=1;
				}else if(personpos==1 && !abc->players[6].userid){
						pos=6;
				}
			}
			break;
		case on_standby:
			pos += 10;
			pos = !abc->players[pos].userid ? pos : (!abc->players[pos + 5].userid ? pos + 5 : -1);
			if (pos == -1)
				pos = !abc->players[old_pos].userid ? old_pos : (!abc->players[old_pos + 5].userid ? old_pos + 5 : -1);;
			break;
		default:
			pos = -1;
			break;
	}
	if (pos != -1) {
		warrior_t* p = &abc->players[pos];
		memcpy(p, aw, sizeof(warrior_base_t));
		if (p->pet_state == on_fuzhu && aw->skill_cnt) {
			int avl_rate = 1000 / aw->skill_cnt;
			for (int i = 0; i < aw->skill_cnt-1; i++ ){
				p->skills[i].rand_rate = avl_rate;
			}
			p->skills[aw->skill_cnt-1].rand_rate = 1000 - (aw->skill_cnt-1) * avl_rate;
		}
	}
	
	return pos;
}

static int set_warrior_pos(battle_info_t* abi, battle_users_t* abc, battle_users_t* enemy, warrior_t* aw, int user_cnt)
{
	KDEBUG_LOG(aw->userid, "ADD PLAYER\t[%u %u]", aw->petid,aw->change_petid);
	// 非法属性
	if (aw->level <=0 || aw->hp <= 0 || aw->speed <= 0 || aw->spirit <= 0){
		KERROR_LOG(aw->userid, "invalid prop\t[pet=%u %u %d %u %u]", aw->petid, aw->level, aw->hp, aw->speed, aw->spirit);
		return -1;
	}
	
	int idx = -1;
	if (aw->petid) {//是宠物
		if (IS_BEAST_ID(aw->userid))
			KERROR_LOG(aw->userid, "pet uid is beast\t[batid=%lu petid=%u]", abi->batid, aw->petid);
		
		//得到主人pos
		int pos = get_warrior_pos(abc, aw->userid, 0);
		if (pos == -1 || (idx = set_pet_warrior_pos(pos, abc, aw,abi->bt_mode)) == -1) {
			KERROR_LOG(aw->userid, "Cannot find user\t[batid=%lu %d]", abi->batid, pos);
			return 0;
		}
	} else {//是主人
		if (IS_BEAST_ID(aw->userid)){
			idx = user_cnt;
			if (abc->npc_fri_cnt_alive) {
				for (uint32_t loop = MAX_PLAYERS_PER_TEAM * 2 - 1; loop >= MAX_PLAYERS_PER_TEAM; loop --) {
					if (!abc->players[loop].userid && !abc->players[loop - MAX_PLAYERS_PER_TEAM].userid) {
						idx = loop;
						break;
					}
				}
			}
		} else {
			idx = (aw->in_front) ? user_cnt : (user_cnt + MAX_PLAYERS_PER_TEAM);
		}

		if (abc->players[idx].userid){
			KERROR_LOG(aw->userid, "already has warrior\t[batid=%lu %u %u %d]", abi->batid, user_cnt, abc->players[idx].userid, idx);
			return -1;
		}
		memcpy(&abc->players[idx], aw, sizeof(warrior_base_t));
	}


	abc->players[idx].enemy_team = enemy;
	abc->players[idx].self_team = abc;
	if (idx >= 0 && idx < MAX_PLAYERS_PER_TEAM * 2){
		if (!aw->petid || aw->pet_state == rdy_fight || aw->pet_state == on_fuzhu){
			list_add_tail(&(abc->players[idx].waor_list), &abi->warrior_list);
			abc->players_pos[idx] = &abc->players[idx];
			KDEBUG_LOG(aw->userid,"add player:petid=%u state=%u changeid=%u[ok]",aw->petid,aw->pet_state,aw->change_petid);
		}
	}
	
	warrior_t* lp = &abc->players[idx];
	INIT_LIST_HEAD (&(lp->timer_list));
	INIT_LIST_HEAD (&(lp->huwei_list_header));
	lp->p_waor_state=new Cwarrior_state();
	if(!IS_BEAST(lp) && abi->is_pets_pk_mode()) {
		lp->set_not_in_battle_mode(true);
	} else {
		lp->flag = 0;
	}

	return 0;
}

int init_all_timer_type(int rstart)
{
	REGISTER_TIMER_TYPE(n_battle_load_expired, battle_load_expired, rstart);
	REGISTER_TIMER_TYPE(n_attack_op_expired, attack_op_expired, rstart);
	REGISTER_TIMER_TYPE(n_warrior_load_expired, warrior_load_expired, rstart);
	return 0;
}

static void remove_player_from_battle(battle_info_t* abi, battle_users_t* team, userid_t uid)
{
	// find player
	int person_pos = get_warrior_pos(team, uid, 0);
	if(person_pos == -1){
		return;
	}

	if (team->ren_alive_count == 1){
		KERROR_LOG(uid, "kickoffplayer only one player\t[batid=%lu uid=%u batobj=%p global_load_timer=%p global_fight_timer=%p]", 
			abi->batid, uid, abi, abi->load_timer, abi->fight_timer);

		if ((abi->challgee.team_type & team_beast) == team_beast){
			free_batter_by_id(abi->batid);
		}
		return;
	}

	uint8_t rmflag = 0;
	team->players_pos[person_pos] = NULL; // ren
	int pet_pos = (person_pos < MAX_PLAYERS_PER_TEAM)?(person_pos + MAX_PLAYERS_PER_TEAM) :(person_pos - MAX_PLAYERS_PER_TEAM); 
	team->players_pos[pet_pos] = NULL; // pet
	
	person_pos = (person_pos < MAX_PLAYERS_PER_TEAM)?person_pos:(person_pos - MAX_PLAYERS_PER_TEAM);
	int i = 0;
	for (i = 0; i < 4; i++){
		int pos = person_pos + 5 * i;
		warrior_t* lp = &(team->players[pos]);
		if (lp->userid){
			if (!lp->petid || lp->pet_state == rdy_fight){
				list_del(&lp->waor_list);
				rmflag++;
			}
			memset(lp, 0x00, sizeof(warrior_t));
		}
	}

	team->ren_alive_count--;
	calc_team_ave_lv(team);

	if (team->team_type & team_npc_friend){
		// 队伍中仅剩下npc
		if (team->ren_alive_count == team->npc_fri_cnt_alive){
			free_batter_by_id(abi->batid);
			return;
		}
	}

	team->ren_count--;
	team->count_ex -= rmflag;
	team->alive_count -= rmflag;
}

int bt_kick_off_teamer(userid_t uid, uint32_t online_id, battle_info_t* abi, Cmessage* c_in )
{
	if (abi->batl_state != battle_challenging){
		KERROR_LOG(uid, "kickoffplayer battle is in ready mode\t[batid=%lu uid=%u]", abi->batid, uid);
		return -1;
	}

	// da guai moshi xia jinzhi ti ren
	if ((abi->challgee.team_type | abi->challger.team_type) & team_npc_friend){
		KERROR_LOG(uid, "kickoffplayer battle in PVE(da gua) mode, cann't kick off player\t[batid=%lu uid=%u]", abi->batid, uid);
		return -1;
	}

	// &abi->warrior_list
	remove_player_from_battle(abi, &abi->challger, uid);
	return 0;
}


/* *  人－人对战，人－怪对战,代码极为: 一致
 * */
//void challenge_battle_op_ex(userid_t uid, uint32_t online_id, battle_info_t* abi, const unsigned char* body, int len, int cmdid)
void challenge_battle_op_ex(userid_t uid, uint32_t online_id, battle_info_t* abi,req_challenge_t & req_challenge_info  , int cmdid)
{
	//if (len <= 12){
		//ERROR_LOG("len invalid\t[btid=%lu len=%u entry_len=%lu ol=%u]", abi->batid, len, entry_len, online_id);
		//free_batter_by_id(abi->batid);
		//return;
	//}
	//int j = 0;
	uint32_t bt_mode = 0;
	battle_users_t* pbc = &(abi->challger);
	//UNPKG_H_UINT32(body, abi->challgee.teamid, j);
	//UNPKG_H_UINT32(body, pbc->wined_count, j);
	//UNPKG_H_UINT32(body, bt_mode, j);
	//UNPKG_H_UINT32(body, pbc->ren_count, j);
	abi->challgee.teamid=req_challenge_info.teamid;
	pbc->wined_count=req_challenge_info.win_cnt;
	bt_mode=req_challenge_info.mode;
	abi->bt_mode=bt_mode;
	pbc->ren_count=req_challenge_info.warriors.size();//req_challenge_info.ren_cnt;
	DEBUG_LOG("challenge_battle_op_ex %u %u %u",req_challenge_info.teamid,req_challenge_info.mode,(uint32_t)req_challenge_info.warriors.size());;
	pbc->teamid = uid;
	pbc->online_id = online_id;
	if(bt_mode==1)//pet pk
		abi->set_pets_pk_mode(true);
	//if (!pbc->ren_count || pbc->ren_count > int (MAX_PLAYERS_PER_TEAM * 4) || int (pbc->ren_count * entry_len) != (len - j)){
		//ERROR_LOG("len invalid\t[batid=%lu  revcnt=%d,need len=%u,  send len=%u olid=%u]", 
				//abi->batid, uint32_t( pbc->ren_count), (uint32_t)entry_len,(uint32_t)(len - j) , uint32_t (online_id));
		//free_batter_by_id(abi->batid);
		//return;
	//}
	if (!pbc->ren_count || pbc->ren_count > int (MAX_PLAYERS_PER_TEAM * 4)){
		ERROR_LOG("len invalid\t[batid=%lu  revcnt=%d,need len=%u,  olid=%u]", 
				abi->batid, uint32_t( pbc->ren_count), (uint32_t)entry_len, uint32_t (online_id));
		free_batter_by_id(abi->batid);
		return;
	}
	int user_cnt = 0, person_cnt = 0;

	for (int loop = 0; loop < pbc->ren_count; loop++) {
		//warrior_t* pw = (warrior_t*)(body + j);	
		warrior_t tmp={};
		read_from_struct_warrior(&tmp,req_challenge_info.warriors[loop]);
		warrior_t *pw=&tmp;

		if (pw->userid){
			if (IS_BEAST_ID(pw->userid))
				pbc->npc_fri_cnt_alive ++;

			if (!pw->petid || pw->pet_state == rdy_fight)
				pbc->count_ex ++;

			if (pbc->count_ex > (MAX_PLAYERS_PER_TEAM * 2)){
				ERROR_LOG("user cnt max\t[batid=%lu count=%u olid=%u]", abi->batid, pbc->count_ex, online_id);
				free_batter_by_id(abi->batid);
				send_to_online_error(BT_ERR_INVALID_TEAM_COUNT, uid, abi->batid, cmdid, online_id);
				return;
			}

			if (set_warrior_pos(abi, pbc, &abi->challgee, pw, user_cnt) == -1){
				free_batter_by_id(abi->batid);
				send_to_online_error(BT_ERR_INVALID_WARRIOR, uid, abi->batid, cmdid, online_id);
				return;
			}

			if (!pw->petid) {
				person_cnt++;
				user_cnt ++;
			}
		}else  {
			user_cnt ++;
		}
	//	j += entry_len;
	}

	if (pbc->count_ex <= 0){
		ERROR_LOG("user cnt invalid\t[batid=%lu count=%u olid=%u]", abi->batid, pbc->ren_count, online_id);
		free_batter_by_id(abi->batid);
		send_to_online_error(BT_ERR_INVALID_TEAM_COUNT, uid, abi->batid, cmdid, online_id);
		return;
	}	

	pbc->ren_alive_count = person_cnt;
	pbc->ren_count = person_cnt;
	pbc->team_type = pbc->ren_count > 1 ? team_person_group : team_person;

	//如果是: 加上team_npc flag
	if (cmdid == proto_bt_challenge_npc_battle)
		pbc->team_type |= team_npc ;

	// npc参与了人的一方
	if (pbc->npc_fri_cnt_alive)
		pbc->team_type |= team_npc_friend;

	// calc team ave level
	calc_team_ave_lv(pbc);
	send_to_online(0, NULL, uid, abi->batid, cmdid, online_id);
}

//static void challenge_battle_op(userid_t uid, uint32_t online_id, battle_info_t* abi, const unsigned char* body, int len)
int challenge_battle_op(userid_t uid, uint32_t online_id, battle_info_t* abi,Cmessage* c_in )
{
	challenge_battle_op_in *p_in=P_IN; 
	challenge_battle_op_ex(uid,online_id,abi,p_in->req_challenge_info ,proto_bt_challenge_battle);
	return 0;
}

// tiao zhan npc 
//static void challenge_npc_battle_op(userid_t uid, uint32_t online_id, battle_info_t* abi, const unsigned char* body, int len)
int challenge_npc_battle_op(userid_t uid, uint32_t online_id, battle_info_t* abi, Cmessage* c_in)
{
	challenge_npc_battle_op_in *p_in=P_IN; 
	challenge_battle_op_ex(uid,online_id,abi,p_in->req_challenge_info ,proto_bt_challenge_npc_battle  );
	return 0;
}

static void create_warrior_loading_timer(battle_info_t* abi, warrior_t* p)
{
	if (IS_BEAST(p))
		return;
//	DEBUG_LOG("ADD n_warrior_load_expired :%u %u %u,15",p->userid,p->petid,p->change_petid);
	ADD_ONLINE_TIMER(p, n_warrior_load_expired, abi, 15);
}

static void start_warrior_loading_timer(battle_info_t* abi)
{
	list_head_t* lh;
	list_for_each(lh, &abi->warrior_list) {
		warrior_t* p = list_entry(lh, warrior_t, waor_list);
		create_warrior_loading_timer(abi, p);
	}
	return;
}

int accept_battle_op(userid_t uid, uint32_t online_id, battle_info_t* abi, Cmessage* c_in)
{

	battle_users_t* pbc = &(abi->challgee);
	if ((abi->batl_state != battle_challenging) || (pbc->teamid != uid && !IS_BEAST_GRP_ID(pbc->teamid))){
		KERROR_LOG(uid, "bt state invalid\t[batid=%lu %u %u olid=%u]", abi->batid, uid, pbc->teamid,  online_id);
		return send_to_online_error(BT_ERR_WRONG_CHALLENGE, uid, abi->batid, proto_bt_accept_battle, online_id);
	}

	uint32_t real_cnt, beast_mark, difficulty_pencent;

	
	accept_battle_op_in *p_in=P_IN;
	abi->groupid=p_in->grpid;
	//pbc->wined_count=p_in->win;
	difficulty_pencent=p_in->percent;
	beast_mark=p_in->mark;
	real_cnt=p_in->warriors.size();
	abi->bt_mode=p_in->win;
	abi->difficulty=difficulty_pencent/100.0;	
	DEBUG_LOG("real_cnt:%u,beast_mark=%u bt_mode=%u",real_cnt,beast_mark, abi->bt_mode);

	pbc->online_id = online_id;

	if (real_cnt <= 0 || real_cnt >  int(MAX_PLAYERS_PER_TEAM * 4) ){
		ERROR_LOG("cnt invalid\t[batid=%lu  entry_len=%lu count=%u olid=%u]", abi->batid, entry_len, real_cnt, online_id);
		free_batter_by_id(abi->batid);
		return  send_to_online_error(BT_ERR_INVALID_TEAM_COUNT, uid, abi->batid, proto_bt_accept_battle, online_id);
	}
	uint8_t beast_cnt = 0;
	int user_cnt = 0;
	for (uint32_t loop = 0; loop < real_cnt; loop++) {
		//warrior_t* pw = (warrior_t*)(body + j);
		warrior_t tmp={};
		read_from_struct_warrior(&tmp,p_in->warriors[loop]);
		warrior_t *pw=&tmp;

		if (pw->userid){
			if (IS_BEAST_ID(pw->userid))
				beast_cnt++;
			
			if (pw->type_id == TYPEID_2008 && abi->groupid == 2506 ){//章界王
				abi->set_is_pk_beast_2008(true);
			}else if (pw->type_id == TYPEID_2004 && abi->groupid == 2507 ){
				abi->set_is_pk_beast_2004(true);
			}else if (pw->type_id == TYPEID_3017 && abi->groupid == 3102 ){
				abi->set_is_pk_beast_3017(true);
			}else if (pw->type_id == TYPEID_2005 && abi->groupid == 2508 ){
				abi->set_is_pk_beast_2005(true);
			} else if (pw->type_id == TYPEID_2004 && abi->groupid == 2906) {
				abi->set_is_pk_beast_2906(true);
			} else if (pw->type_id == TYPEID_2013 && abi->groupid == 2907) {
				//abi->set_is_pk_beast_2013(true);
			}

			if (!pw->petid || pw->pet_state == rdy_fight)
				pbc->count_ex ++;
			
			if (pbc->count_ex > (MAX_PLAYERS_PER_TEAM * 2)){
				ERROR_LOG("cnt max\t[batid=%lu count=%u olid=%u]", abi->batid, real_cnt, online_id);
				free_batter_by_id(abi->batid);
				return send_to_online_error(BT_ERR_INVALID_TEAM_COUNT, uid, abi->batid, proto_bt_accept_battle, online_id);
			}

			if (set_warrior_pos(abi, pbc, &abi->challger, pw, user_cnt) == -1){
				free_batter_by_id(abi->batid);
				return send_to_online_error(BT_ERR_INVALID_WARRIOR, uid, abi->batid, proto_bt_accept_battle, online_id);
			}

			if (!pw->petid){
				pbc->ren_count ++;
				user_cnt ++;
			}
		}else {
			user_cnt ++;
		}
	
	//	j += entry_len;
	}

	if (pbc->count_ex <= 0){
		ERROR_LOG("cnt invalid\t[batid=%lu  entry_len=%lu count=%u olid=%u]", abi->batid, entry_len, real_cnt, online_id);
		free_batter_by_id(abi->batid);
		return send_to_online_error(BT_ERR_INVALID_TEAM_COUNT, uid, abi->batid, proto_bt_accept_battle, online_id);
	}


	abi->batl_state = battle_ready;
	//设置对战开始时间
	abi->round_timestamp=time(NULL);
	pbc->ren_alive_count = pbc->ren_count;
	pbc->team_type = beast_mark ? team_beast : (pbc->ren_count > 1 ? team_person_group : team_person);

	//是单人pk怪物
	if (beast_mark && (abi->challger.ren_count - abi->challger.npc_fri_cnt_alive == 1) )
		abi->set_is_one_person_pk_beast(true);

	if (!beast_mark && beast_cnt)
		pbc->team_type |= team_npc_friend;

	// calc team ave level
	calc_team_ave_lv(pbc);

	// start global client init timer
	abi->load_timer = ADD_ONLINE_TIMER(abi, n_battle_load_expired, 0, 120);
	
	if (!IS_BEAST_GRP_ID(pbc->teamid))
		send_to_online(0, NULL, uid, abi->batid, proto_bt_accept_battle, online_id);

	abi->touxi = touxi_none; // normal
	if (!abi->is_pets_pk_mode() && (pbc->team_type & team_beast) && !(abi->challger.team_type & team_npc) && (rand() % 100) < 5)
		abi->touxi = rand() % 2 ? touxi_by_challger : touxi_by_challgee; // 主动方 tou xi

	response_battle_info(abi, proto_bt_noti_battle_created);
	start_warrior_loading_timer(abi);
	return 0;
}

int bt_reject_challenge(userid_t uid, uint32_t online_id, battle_info_t* abi, Cmessage* c_in )
{
	battle_users_t* pbc = &(abi->challgee);
	
	if (abi->batl_state != battle_challenging){
		ERROR_LOG("battle already start\t[batid=%lu btl_loop=%u btl_state=%u %u %u]", 
			abi->batid, abi->battle_loop, abi->batl_state, uid, pbc->teamid);
		return -1;
	}
	
	// 判断被挑战者是否是怪物
	// 如果是，则是打怪，就无法拒绝战斗
	if (IS_BEAST_ID(abi->challgee.teamid)){
		ERROR_LOG("WAI GUA: DAGUAI WUFA_JUJUE\t[batid=%lu btl_loop=%u btl_state=%u %u %u]", 
			abi->batid, abi->battle_loop, abi->batl_state, uid, pbc->teamid);
		return -1;
	}
	if (pbc->teamid != uid) {
		ERROR_LOG("not challenge you\t[batid=%lu %u %u]", abi->batid, uid, pbc->teamid);
		return -1;
	}
	
	send_to_online(0, NULL, abi->challger.teamid, abi->batid, proto_bt_reject_challenge, online_id);
	//free battle
	free_batter_by_id(abi->batid)	;
	return 0;
}

int bt_cancel_challenge(userid_t uid, uint32_t online_id, battle_info_t* abi, Cmessage* c_in )
{
	battle_users_t* pbc = &(abi->challger);
	// battle is starting
	if (abi->batl_state != battle_challenging || pbc->teamid != uid){
		// 1 error code
		send_to_online_error(1, uid, abi->batid, proto_bt_cancel_challenge, online_id);
		KERROR_LOG(uid, "not challenge you\t[batid=%lu %u %u]", abi->batid, uid, pbc->teamid);
		return -1;
	}

	// 判断被挑战者是否是怪物
	// 如果是，则是打怪，就无法取消战斗
	if (IS_BEAST_ID(abi->challgee.teamid)){
		// 1 error code
		send_to_online_error(1, uid, abi->batid, proto_bt_cancel_challenge, online_id);
		KERROR_LOG(uid, "DA_GUAI WUFA_QUXIAO\t[batid=%lu %u %u]", abi->batid, uid, pbc->teamid);
		return -1;
	}

	send_to_online(4, &(abi->challgee.teamid), abi->challger.teamid, abi->batid, proto_bt_cancel_challenge, online_id);
	//free battle
	free_batter_by_id(abi->batid)	;
	return 0;
}

static int chk_all_user_ready(battle_info_t* abi)
{
	list_head_t* lh;
	list_for_each(lh, &abi->warrior_list) {
		warrior_t* p = list_entry(lh, warrior_t, waor_list);
		if (!p->ready && !(IS_BEAST(p))) {
			return 0;
		}
	}

	return 1;
}

void clean_one_state(warrior_t* p_fighter, uint8_t state_bit, int8_t times)
{
	if (!p_fighter->check_state( state_bit)) {
		return;
	}
	KDEBUG_LOG(p_fighter->userid,"clean_one_state :bit=%u round=%u",state_bit,times);	
	if (state_bit >= run_away_bit && state_bit < bisha_bit) {
		RESET_WARRIOR_STATE(p_fighter->p_waor_state->state, state_bit);
		return;
	}
	
	int8_t effect_cnt= p_fighter->p_waor_state->change_effect_cnt(state_bit,-times );
	int8_t round_cnt= p_fighter->p_waor_state->change_round_cnt(state_bit,-times );
	if (effect_cnt ==0 || round_cnt==0 ) {
		p_fighter->p_waor_state->unset_state_info(state_bit );
		if(yiwang_bit == state_bit) {
			p_fighter->skills_forgot_cnt = 0;
			memset(p_fighter->skills_forgot,0,sizeof(p_fighter->skills_forgot));
		}

		if (parasitism_bit == state_bit) {
			p_fighter->p_waor_state->set_host_uid(0);
			p_fighter->p_waor_state->set_host_petid(0);
		}
	}
}

static void clean_one_time_state(warrior_t* p_fighter, uint32_t clean_state)
{
	switch (clean_state) {
		case shihua:
			clean_one_state(p_fighter, shihua_bit, 1);
			break;
		case yiwang:
			clean_one_state(p_fighter, yiwang_bit, 1);
			break;
		case hunluan:
			clean_one_state(p_fighter, hunluan_bit, 1);
			break;
		case zuzhou:
			clean_one_state(p_fighter, zuzhou_bit, 1);
			break;
		case zhongdu:
			clean_one_state(p_fighter, zhongdu_bit, 1);
			break;
		case hunshui:
			clean_one_state(p_fighter, hunshui_bit, 1);
			break;
		case gongji_xishou:
			clean_one_state(p_fighter, gongji_xishou_bit, 1);
			break;
		case gongji_fantan:
			clean_one_state(p_fighter, gongji_fantan_bit, 1);
			break;
		case gongji_wuxiao:
			clean_one_state(p_fighter, gongji_wuxiao_bit, 1);
			break;
		case mofa_xishou:
			clean_one_state(p_fighter, mofa_xishou_bit, 1);
			break;
		case mofa_fantan:
			clean_one_state(p_fighter, mofa_fantan_bit, 1);
			break;
		case mofa_wuxiao:
			clean_one_state(p_fighter, mofa_wuxiao_bit, 1);
			break;
		case resume_hp1:
			clean_one_state(p_fighter, resume_hp1_bit, 1);
			break;
		case resume_hp2:
			clean_one_state(p_fighter, resume_hp2_bit, 1);
			break;
		case resume_hp3:
			clean_one_state(p_fighter, resume_hp3_bit, 1);
			break;
		case resume_hp4:
			clean_one_state(p_fighter, resume_hp4_bit, 1);
			break;
		case dongshang:
			clean_one_state(p_fighter,  dongshang_bit, 1);
			break;
		case zhuoshang:
			clean_one_state(p_fighter,  zhuoshang_bit, 1);
			break;
		case shufu:
			clean_one_state(p_fighter,  shufu_bit, 1);
			break;
		case xuanyun:
			clean_one_state(p_fighter,  xuanyun_bit, 1);
			break;
		case tuoli:
			clean_one_state(p_fighter,  tuoli_bit, 1);
			break;
		case gaopin_zhenji:
			clean_one_state(p_fighter, gaopin_zhenji_bit, 1);
			break;
		default:
			//clean_one_state(p_fighter, fangyu_bit);
			break;
	}
	return;
}

static int chk_all_user_fighted(battle_info_t* abi)
{
	list_head_t* lh;
	list_for_each(lh, &abi->warrior_list) {
		warrior_t* p = list_entry(lh, warrior_t, waor_list);
		if (IS_BEAST_ID(p->userid) || CANNOT_ATTACK(p) || p->pet_state == on_fuzhu \
			|| (p->is_not_in_battle_mode() && p->is_attack_dumy()))
			continue;
		
		if (!p->atk_info[0].atk_type){//第一招是空的
			if(!p->petid) {//是用户,没有攻击
				KDEBUG_LOG(p->userid, "REN NEED ATK");
				return 0;
			}
			//宠物
			if(abi->is_pets_pk_mode()) {
				KDEBUG_LOG(p->userid, "PET(PK) NEED ATK");
				return 0;
			}
			if (p->pet_state == rdy_fight){
				return 0;
			}
			warrior_t* p_ren = get_warrior_from_warriorlist(abi, p->userid, 0);
			if (!NONEED_ATTACK(p_ren)) { //人能攻击,而宠物还没有攻击
				KDEBUG_LOG(p->userid,"==chk all fighted: petid=%u state=%x  [no]",p->petid,p->pet_state);
				return 0;
			}
		}

		//宠物没有第二招
		if (p->petid)
			continue;

		//是人
		if (p->atk_info[1].atk_type == 0){
			warrior_t* p_pet = get_pet_from_warriorlist(abi, p->userid);
			if (!p_pet || RUN_OR_OUT(p_pet)){
				KDEBUG_LOG(p->userid,"PET R2 NEED ATK");
				return 0;
			}
		}
	}

	return 1;
}

void set_warrior_offline(battle_info_t* abi, warrior_t* lp)
{
	//KDEBUG_LOG( lp->userid,"SET_WARRIOR_OFFLINE:");
	if (lp->huwei_pos){
		remove_warrior_from_huweilist(lp);
	}

	SET_WARRIOR_STATE(lp->p_waor_state->state, person_break_off);
	if (lp->petid){
		return;
	}

	warrior_t* lp_pet = get_pet_from_warriorlist(abi, lp->userid);
	if (lp_pet && !NONEED_ATTACK(lp_pet)) {
		if (lp_pet->huwei_pos){
			remove_warrior_from_huweilist(lp_pet);
		}
		//decr_pet_count(abi, lp_pet);
		SET_WARRIOR_STATE(lp_pet->p_waor_state->state, person_break_off);
	}
	//set fuzhu pet 
	list_head_t* lh;
	list_for_each(lh, &abi->warrior_list) {
		warrior_t* pfuzhu = list_entry(lh, warrior_t, waor_list);
		if (pfuzhu->userid == lp->userid && pfuzhu->petid &&  pfuzhu->pet_state == on_fuzhu ){
			//decr_pet_count(abi, pfuzhu);
			SET_WARRIOR_STATE(pfuzhu->p_waor_state->state, person_break_off);
		}
	}
}

void remove_atktype_from_speedlist(warrior_t*p, battle_info_t* abi)
{
	list_head_t* head = &abi->speed_list;
	list_head_t* cur;
	list_head_t* next;
	list_for_each_safe(cur, next, head) {
		atk_info_t* sped = list_entry(cur, atk_info_t, atk_info_list);
		if (sped->wrior->userid == p->userid){
			list_del(&sped->atk_info_list);
		}
	}
}

uint8_t send_dead_dumy_skill(warrior_t* p, battle_info_t* abi)
{
	int pos;
	int32_t res_pos = get_warrior_pos_ex(abi, p->userid, p->petid, &pos);
	if (res_pos == 0){
		uint8_t btl_res = battle_attack_op_ex(abi, p, 0, pos, skill_pa_dead_dumy, 1 );	
		return btl_res;
	}

	return btl_op_res_error;
}

uint8_t send_break_off_skill(warrior_t*p, battle_info_t* abi)
{
	uint8_t btl_res = btl_op_res_wait_skill;
	uint32_t skill_id = 0;
	if (p->atk_info[0].atk_type == 0) {
		int pos;
		KDEBUG_LOG(p->userid,"===break 3");
		int32_t res_pos = get_warrior_pos_ex(abi, p->userid, p->petid, &pos);
		if (res_pos == 0){
			skill_id = (p->petid)?skill_pet_break_off : skill_user_break_off;
			btl_res = battle_attack_op_ex(abi, p, 0, pos, skill_id, 1 );
			switch(btl_res){
			case btl_op_res_new_loop:
			case btl_op_res_over:
				return btl_res;
			case btl_op_res_error:
				ERROR_LOG("send_break_off_skill-1\t[batid=%lu btl_loop=%u]", abi->batid, abi->battle_loop);
				break;
			}
		}else{
			return btl_res;
		}
	}

	if (p->petid){
		return btl_res;
	}
	
	warrior_t* pw = get_pet_from_warriorlist(abi, p->userid);
	if (!pw || RUN_OR_OUT(pw)){
		if (p->atk_info[1].atk_type == 0){
			int pos;
			int32_t res_pos = get_warrior_pos_ex(abi, p->userid, p->petid, &pos);
			if (res_pos == 0){
				btl_res = battle_attack_op_ex(abi, p, 0, pos, skill_user_break_off, 1 );
				switch(btl_res){
				case btl_op_res_new_loop:
				case btl_op_res_over:
					return btl_res;
				case btl_op_res_error:
					ERROR_LOG("send_break_off_skill-2\t[batid=%lu btl_loop=%u]", abi->batid, abi->battle_loop);
					break;
				}
			}else{
				return btl_res;
			}
		}
	}else{
		int pos;
		int32_t res_pos = get_warrior_pos_ex(abi, pw->userid, pw->petid, &pos);
		if (res_pos == 0){
			skill_id = (pw->petid)?skill_pet_break_off:skill_user_break_off;
			btl_res = battle_attack_op_ex(abi, pw, 0, pos, skill_id, 1);
			switch(btl_res){
			case btl_op_res_new_loop:
			case btl_op_res_over:
				return btl_res;
			case btl_op_res_error:
				ERROR_LOG("send_break_off_skill-3\t[batid=%lu btl_loop=%u]", abi->batid, abi->battle_loop);
				break;
			}
		}else{
			return btl_res;
		}
	}

	return btl_res;
}

uint8_t send_break_off_skill_ex(warrior_t*p, battle_info_t* abi)
{
	KDEBUG_LOG(p->userid,"===break 2");

	if (p->atk_info[0].atk_type != 0 || p->atk_info[1].atk_type != 0){
		remove_atktype_from_speedlist(p, abi);
		
		memset(p->atk_info, 0, sizeof(p->atk_info));
		p->atk_info[0].wrior = p;
		p->atk_info[1].wrior = p;

		warrior_t* lp_pet = get_pet_from_warriorlist(abi, p->userid);
		if (lp_pet && !CANNOT_ATTACK(lp_pet)){
			memset(lp_pet->atk_info, 0, sizeof(lp_pet->atk_info));
			lp_pet->atk_info[0].wrior = lp_pet;
			lp_pet->atk_info[1].wrior = lp_pet;
		}
	}

	return send_break_off_skill(p, abi);
}

static int chk_lua_api_for_c(lua_State* lua_state)
{
	int ret = 0;
	// 检查api是否存在
	lua_getglobal(lua_state, BATRSERV_LUA_FOR_C_BEAST_AI); 
	if(!lua_isfunction(lua_state,-1)){
		ret = -1;
	}
	lua_pop(lua_state,1); 

	if (ret != 0){
		ERROR_LOG("chk_lua_api_for_c\t[%s] is not exist", BATRSERV_LUA_FOR_C_BEAST_AI);
		return ret;
	}

	// 检查api是否存在
	lua_getglobal(lua_state, BATRSERV_LUA_FOR_C_NEED_2_ATTACK); 
	if(!lua_isfunction(lua_state,-1)){
		ret = -1;
	}
	lua_pop(lua_state,1); 

	if (ret != 0){
		ERROR_LOG("chk_lua_api_for_c\t[%s] is not exist", BATRSERV_LUA_FOR_C_NEED_2_ATTACK);
	}

	return ret;
}

static int try_load_lua_script()
{
	clock_t tStart = clock();
	/* initialize Lua */
	lua_State* lua_state = lua_open();
	if (lua_state == NULL){
		return -1;
	}
	
	/* load Lua base libraries */
	luaopen_base(lua_state);
	/* load the script */
	if (luaL_dofile(lua_state, BATRSERV_LUA_SCRIPT) != 0){
		ERROR_LOG("try load [%s] failed", BATRSERV_LUA_SCRIPT);
		return -1;
	}

	int ret = chk_lua_api_for_c(lua_state);
	lua_close(lua_state);
	lua_state = NULL;
	
	clock_t tm_cost = clock() - tStart;
	tm_cost = (tm_cost * 1000) / CLOCKS_PER_SEC;
	if (tm_cost >= 1000){
		ERROR_LOG("try_load_lua_script: err %lu ms", tm_cost);
	}else{
		DEBUG_LOG("try_load_lua_script: ok %lu ms", tm_cost);
	}
	return ret;
}

int load_lua_script()
{
	struct stat buf;
	if (lstat(BATRSERV_LUA_SCRIPT, &buf) != 0){
		ERROR_LOG("%s is not a file", BATRSERV_LUA_SCRIPT);
		return -1;
	}
	
	if (!S_ISREG(buf.st_mode)){
		ERROR_LOG("%s is not a file -2", BATRSERV_LUA_SCRIPT);
		return -1;
	}

	if (buf.st_size <= 0){
		ERROR_LOG("%s is not a valid file -3\t[size is zero]", BATRSERV_LUA_SCRIPT);
		return -1;
	}

	if (try_load_lua_script() != 0){
		ERROR_LOG("try load lua script file[%s]failed", BATRSERV_LUA_SCRIPT);
		return -1;
	}
	

	if (g_lua_state){
		lua_close(g_lua_state);
		g_lua_state = NULL;
	}

	/* initialize Lua */
	g_lua_state = lua_open();
	if (g_lua_state == NULL){
		return -1;
	}
	
	/* load Lua base libraries */
	luaopen_base(g_lua_state);
	/* load the script */
	if (luaL_dofile(g_lua_state, BATRSERV_LUA_SCRIPT) != 0){
		ERROR_LOG("load_lua_script [%s] failed", BATRSERV_LUA_SCRIPT);
		return -1;
	}

	register_beast_ai_function();
	return 0;
}

int attack_op_expired(void* owner, void* data)
{
	battle_info_t* lbi = (battle_info_t*)owner;

	lbi->fight_timer = NULL;
	list_head_t* lh;
	list_for_each(lh, &lbi->warrior_list) {
		warrior_t* p = list_entry(lh, warrior_t, waor_list);
		if (!p || CANNOT_ATTACK(p) || IS_BEAST_ID(p->userid) \
			|| p->check_state( person_break_off)) // already offline
			continue;

		if (p->petid){//pet
			// offline
			if (p->atk_info[0].atk_type == 0){
				KDEBUG_LOG(p->userid, "PET NOT ATK\t[batid=%lu petid=%u]", lbi->batid, p->petid);
				// set ren and pet both offline
				warrior_t* lp_ren = get_warrior_from_warriorlist(lbi, p->userid, 0);
				if(lp_ren){
					set_warrior_offline(lbi, lp_ren);
					switch(send_break_off_skill_ex(lp_ren, lbi)){
						case btl_op_res_new_loop:
						case btl_op_res_over:
							return 0;
					}
				}
			}
		}else{// ren
			if (p->atk_info[0].atk_type == 0) {
				KDEBUG_LOG(p->userid, "REN NO 1st ATK\t[batid=%lu petid=%u]", lbi->batid, p->petid);
				set_warrior_offline(lbi, p);
				switch(send_break_off_skill_ex(p, lbi)){
				case btl_op_res_new_loop:
				case btl_op_res_over:
					return 0;
				}
			} else if (p->atk_info[1].atk_type == 0){// second
				KDEBUG_LOG(p->userid, "REN NO 2nd ATK\t[batid=%lu uid=%u petid=%u]", lbi->batid, p->userid, p->petid);
				warrior_t* p_pet = get_pet_from_warriorlist(lbi, p->userid);
				if (!p_pet || RUN_OR_OUT(p_pet) || (!CANNOT_ATTACK(p_pet) && p_pet->atk_info[0].atk_type == 0)){
					set_warrior_offline(lbi, p);
					switch(send_break_off_skill_ex(p, lbi)){
					case btl_op_res_new_loop:
					case btl_op_res_over:
						return 0;
					}
				}
			}
		}
	}
	return 0;
}

static void clean_battle_speeds_list(battle_info_t* abi)
{
	list_head_t* next;
	list_head_t* cur;
	list_for_each_safe(cur, next, &abi->speed_list) {
		atk_info_t* sped = list_entry(cur, atk_info_t, atk_info_list);
		list_del(&sped->atk_info_list);
	}
	INIT_LIST_HEAD (&abi->speed_list);
}

void report_user_fighted(battle_info_t* abi, uint32_t uid, uint32_t petid, uint16_t cmd)
{
	uint8_t buff[128] = {0};
	int j = sizeof(protocol_t);
	PKG_UINT32(buff, uid, j);
	PKG_UINT32(buff, petid, j);
	init_proto_head(buff, cmd, j);
	
	send_to_2_leader(abi, cmd, buff, j);
}

void report_user_loadrate(battle_info_t* abi, uint32_t uid, uint8_t loadrate, uint16_t cmd)
{
	uint8_t buff[128] = {0};
	int j = sizeof(protocol_t);
	PKG_UINT32(buff, uid, j);
	PKG_UINT8(buff, loadrate, j);
	init_proto_head(buff, cmd, j);
	send_to_2_leader(abi, cmd, buff, j);
}

static void brk_off_user_intel_attack(battle_info_t* abi, warrior_t* p)
{
	if (!p->petid && p->check_state( pause_bit)){
		clean_one_state(p, pause_bit, 1);
	}

	p->atk_info[0].atk_seq = 0;
	p->atk_info[0].atk_pos = 0;
	p->atk_info[0].atk_mark = 0;
	p->atk_info[0].atk_type = IS_BEAST(p)?skill_pet_break_off:skill_user_break_off;
	p->atk_info[0].atk_level = 1;
	p->atk_info[0].wrior = p;
	p->atk_info[0].atk_speed = p->speed;
	insert_atkinfo_to_list(abi, &p->atk_info[0]);

	if (p->petid){
		return;
	}

	warrior_t* lp = get_pet_from_warriorlist(abi, p->userid);
	if (!lp || RUN_OR_OUT(lp)){
		p->atk_info[1].atk_seq = 1;
		p->atk_info[1].atk_pos = 0;
		p->atk_info[1].atk_mark = 0;
		p->atk_info[1].atk_type = IS_BEAST(p)?skill_pet_break_off:skill_user_break_off;
		p->atk_info[1].atk_level = 1;
		p->atk_info[1].wrior = p;
		p->atk_info[1].atk_speed = p->speed;
		insert_atkinfo_to_list(abi, &p->atk_info[1]);
	}
}

skill_info_t* get_beast_skill(warrior_t* p)
{
	uint32_t rand_rate = rand() % 1000;
	uint32_t base_cmp = 0;
	skill_info_t* ppre_skill = NULL;
	for (int i = 0; i < p->skill_cnt; i++){
		skill_info_t* pskill = &p->skills[i];
		//DEBUG_LOG("B_SKILL\t[batid=%u %u %u %u]", pskill->skill_id, pskill->rand_rate, base_cmp, rand_rate);
		if (pskill->cool_round) {
			base_cmp += pskill->rand_rate;
			continue;
		}
		
		if (rand_rate >= base_cmp && rand_rate < (base_cmp + pskill->rand_rate))
			return pskill;
		
		ppre_skill = pskill;
		base_cmp += pskill->rand_rate;
	}

	return ppre_skill;
}
bool do_beast_atk_ex(battle_info_t* abi, warrior_t* p, atk_info_t* p_atk_info){

	/*
	 * taopao 每回合有30%概率逃跑。
	 */
	///DEBUG_LOG("XXXXXXXXX p->self_team->teamid=%u, p->type_id=%u ",   p->self_team->teamid,p->type_id);
	if (p->self_team->teamid==41 && p->type_id==88 && is_at_rand(30) ){
		p_atk_info->atk_mark = 0;
		p_atk_info->atk_type =skill_pet_run_away ;
		p_atk_info->atk_level = 1;
		p_atk_info->atk_pos = get_warrior_pos(p->self_team, p->userid, p->petid);
		//DEBUG_LOG("11111111 p->self_team->teamid=%u, p->type_id=%u ",   p->self_team->teamid,p->type_id);
		return true;
	}
	if (p->self_team->teamid==47 && p->type_id==91 && is_at_rand(10) ){
		p_atk_info->atk_mark = 0;
		p_atk_info->atk_type =skill_pet_run_away ;
		p_atk_info->atk_level = 1;
		p_atk_info->atk_pos = get_warrior_pos(p->self_team, p->userid, p->petid);
		///DEBUG_LOG("11111111 p->self_team->teamid=%u, p->type_id=%u ",   p->self_team->teamid,p->type_id);
		return true;
	}
	//DEBUG_LOG("111=p->self_team->teamid:%u,p->type_id=%u", p->self_team->teamid,p->type_id);

	return false;
	
}

void beast_intel_attack(battle_info_t* abi, warrior_t* p, atk_info_t* p_atk_info, int seq)
{
	int pos = get_warrior_pos(p->self_team, p->userid, p->petid);
	p_atk_info->atk_seq = seq;
	p_atk_info->wrior = p;
	p_atk_info->atk_speed = p->speed;

	if (do_beast_atk_ex(abi,p,p_atk_info )) {//已经处理过了
		return ;	
	}	
	
	if ((p->self_team == &abi->challger && abi->touxi == touxi_by_challgee) \
		|| (p->self_team == &abi->challgee && abi->touxi == touxi_by_challger)){
		p_atk_info->atk_type = skill_pa_pet_dai_ji;
		p_atk_info->atk_level = 1;
	} else {
		skill_info_t* pskill = get_beast_skill(p);
		if (pskill){
			p_atk_info->atk_type = pskill->skill_id;
			p_atk_info->atk_level = pskill->skill_level;
		} else{
			p_atk_info->atk_type = skill_pa_pet_dai_ji;
			p_atk_info->atk_level = 1;
		}
	}
	
	skill_attr_t* pbeast_skill = get_skill_attr(p_atk_info->atk_type, p->weapon_type);
	if (pbeast_skill){
		switch(pbeast_skill->target_type){
		case self_only:
			p_atk_info->atk_mark = 0;
			p_atk_info->atk_pos = get_warrior_pos(p->self_team, p->userid, p->petid);
			break;
		case except_self:
		case all_attacked:
			p_atk_info->atk_mark = 1;
			switch(pbeast_skill->distance){
			case near_attack:
				p_atk_info->atk_pos = get_near_rand_alive_warrior_pos(p, pos);
				break;
			default:
				p_atk_info->atk_pos = get_rand_alive_warrior_pos(p->enemy_team,p->pet_state);
				break;
			}
			break;
		case self_team:
			p_atk_info->atk_mark = 0;
			switch(pbeast_skill->applystatus){
			case attack_only_dead:
				p_atk_info->atk_pos = get_rand_disable_warrior_pos(p->self_team);
				break;
			case attack_only_alive:
				p_atk_info->atk_pos = get_rand_alive_warrior_pos(p->self_team,p->pet_state);
				break;
			default:
				if (rand() % 2){
					p_atk_info->atk_pos = get_rand_alive_warrior_pos(p->self_team,p->pet_state);
				}else{
					p_atk_info->atk_pos = get_rand_disable_warrior_pos(p->self_team);
				}
				break;
			}
			break;
		case self_team_except_self:
			p_atk_info->atk_mark = 0;
			p_atk_info->atk_pos = get_rand_alive_warrior_pos_ex(p->self_team, p);
			break;
		default:
			p_atk_info->atk_mark = 1;
			switch(pbeast_skill->distance){
			case near_attack:
				p_atk_info->atk_pos = get_near_rand_alive_warrior_pos(p, pos);
				break;
			default:
				p_atk_info->atk_pos = get_rand_alive_warrior_pos(p->enemy_team,p->pet_state);
				break;
			}
			break;
		}
	} else {
		p_atk_info->atk_mark = 1;
		p_atk_info->atk_pos = get_near_rand_alive_warrior_pos(p, pos);
	}

 

}

void clear_warrior_atkinfo(battle_info_t* abi, warrior_t* lp)
{
	memset(lp->atk_info, 0, sizeof(lp->atk_info));
	lp->atk_info[0].wrior = lp;
	lp->atk_info[1].wrior = lp;
}

int8_t chk_repeat_atkinfo_insert(battle_info_t* abi, atk_info_t* p_atk_info)
{
	list_head_t* head = &abi->speed_list;
	list_head_t* cur = NULL;
	list_head_t* next = NULL;
	list_for_each_safe(cur, next, head) {
		if (list_entry(cur, atk_info_t, atk_info_list) == p_atk_info){
			KDEBUG_LOG(p_atk_info->wrior->userid, "repeat insert\t[batid=%lu petid=%u type=%u seq=%u atk_pos=%d]", 
				abi->batid, p_atk_info->wrior->petid, p_atk_info->atk_type, p_atk_info->atk_seq, p_atk_info->atk_pos);
			return 1;
		}
	}

	return 0;
}

static void reset_huwei_atk_pos(battle_info_t* abi, warrior_t* p_fighter, uint8_t new_pos)
{
	int loop = 0;
	for (loop = 0; loop < 2; loop++){
		switch(p_fighter->atk_info[loop].atk_type){
		case skill_hw_huwei:
		case skill_hw_pet_huwei:
			break;
		default:
			continue;
		}

		// 考虑到后期宠物可能会发招2次(而且全是技能)，所以这里不能直接修改pos
		// 需要根据条件判断
		int pos_old = (new_pos >= MAX_PLAYERS_PER_TEAM)?(new_pos-MAX_PLAYERS_PER_TEAM):(new_pos + MAX_PLAYERS_PER_TEAM);
		if (p_fighter->atk_info[loop].atk_pos == pos_old){
			p_fighter->atk_info[loop].atk_pos = new_pos;
		}
	}
}

void chg_warrior_pos_from_huweilist(battle_info_t* abi, warrior_t* p_fighter, uint8_t new_pos)
{
	list_head_t* head = &p_fighter->huwei_list_header;
	list_head_t* cur = NULL;
	list_head_t* next = NULL;
	list_for_each_safe(cur, next, head) {
		warrior_t* huwei_lp = list_entry(cur, warrior_t, huwei_list_node);
		huwei_lp->huwei_pos = new_pos+1;
		reset_huwei_atk_pos(abi, huwei_lp, new_pos);
	}
}

void add_warrior_to_huweilist(warrior_t* p_fighter, uint8_t pos)
{
	warrior_t* p_fightee = p_fighter->self_team->players_pos[pos];
	list_head_t* head = &p_fightee->huwei_list_header;
	list_head_t* cur = NULL;
	list_head_t* next = NULL;
	list_for_each_safe(cur, next, head) {
		warrior_t* huwei_lp = list_entry(cur, warrior_t, huwei_list_node);
		if (huwei_lp->speed < p_fighter->speed){
			break;
		}
	}

	p_fighter->huwei_pos = pos + 1;
	list_add_tail(&p_fighter->huwei_list_node, cur);
}

void remove_warrior_from_huweilist(warrior_t* p_fighter)
{
	list_del(&(p_fighter->huwei_list_node));
	p_fighter->huwei_pos = 0;
}

void clean_warrior_huiwei_list(battle_info_t* abi, warrior_t* p_fighter)
{
	list_head_t* head = &p_fighter->huwei_list_header;
	list_head_t* cur = NULL;
	list_head_t* next = NULL;
	list_for_each_safe(cur, next, head) {
		warrior_t* huwei_lp = list_entry(cur, warrior_t, huwei_list_node);
		list_del(&(huwei_lp->huwei_list_node));
		huwei_lp->huwei_pos = 0;
	}
}

warrior_t* get_warrior_from_huweilist(warrior_t* p_fighter)
{
	list_head_t* head = &p_fighter->huwei_list_header;
	list_head_t* cur = NULL;
	list_head_t* next = NULL;
	list_for_each_safe(cur, next, head) {
		warrior_t* huwei_lp = list_entry(cur, warrior_t, huwei_list_node);
		if (NONEED_ATTACK(huwei_lp)){
			continue;
		}

		return huwei_lp;
	}

	return NULL;
}

void insert_atkinfo_to_list(battle_info_t* abi, atk_info_t* p_atk_info)
{
	DEBUG_LOG("insert_atkinfo_to_list skill=%u",p_atk_info->atk_type);
	int rand_rate = (rand() % 60) + 10;
	if (chk_repeat_atkinfo_insert(abi, p_atk_info))
		return;
	//DEBUG_LOG("11111ADD LIST:skillid=%u teamid=%u pos=%d",p_atk_info->atk_type ,abi->challger.teamid,p_atk_info->atk_pos);
	list_head_t* head = &abi->speed_list;
	list_head_t* cur = NULL;
	list_head_t* next = NULL;
	uint8_t firstmark = 0;
	warrior_t* lp = p_atk_info->wrior;
	list_for_each_safe(cur, next, head) {
		atk_info_t* old_ati = list_entry(cur, atk_info_t, atk_info_list);	
		warrior_t* lp_old = old_ati->wrior;
		//DEBUG_LOG("22222ADD LIST:skillid=%u teamid=%u pos=%d",p_atk_info->atk_type ,abi->challger.teamid,p_atk_info->atk_pos);
		if (lp->check_state( person_break_off)){
			if (p_atk_info->atk_seq == 0){
				if (!lp->petid){
					break; // ren
				}else{
					if (lp_old->userid == lp->userid && lp_old->petid == 0)
						firstmark = 1;

					if (firstmark && lp_old->userid != lp->userid){
						break;
					}
				}
			} else {
				if (lp == lp_old)
					firstmark = 1;

				if (firstmark && lp != lp_old)
					break;
			}

			continue;
		}
		
		//DEBUG_LOG("33333ADD LIST:skillid=%u teamid=%u pos=%d",p_atk_info->atk_type ,abi->challger.teamid,p_atk_info->atk_pos);
		if (lp_old->check_state( person_break_off))
			continue;
		//DEBUG_LOG("444444ADD LIST:skillid=%u teamid=%u pos=%d",p_atk_info->atk_type ,abi->challger.teamid,p_atk_info->atk_pos);
		if (!abi->rand_speed){//正常处理
			if (p_atk_info->atk_seq == 0){
				if (old_ati->atk_speed < p_atk_info->atk_speed)
					break;
			} else {
				if (lp == lp_old)
					firstmark = 1;

				if (firstmark && lp != lp_old && old_ati->atk_speed < p_atk_info->atk_speed)
					break;
			}
		}else{//随机
			if (p_atk_info->atk_seq == 0){
				if ((rand() % 100) > rand_rate)
					break;
			} else {
				if (lp == lp_old)
					firstmark = 1;

				if (firstmark && lp != lp_old && (rand() % 100) > rand_rate)
					break;
			}
		}
	}
	DEBUG_LOG("ADD LIST:skillid=%u teamid=%u pos=%d",p_atk_info->atk_type ,abi->challger.teamid,p_atk_info->atk_pos);

	// stl list
	list_add_tail(&p_atk_info->atk_info_list, cur);
}

static inline void new_round_battle(battle_info_t* abi)
{
	uint8_t rand_speed = 0;
	//每一回合都有5％机会使用随机速度,(pk 2008 不使用)
	if ((rand() % 100) < 5 && !abi->is_pk_beast_2008()  && !abi->is_pk_beast_2004() ){
		rand_speed = 1;
	}
	uint32_t old_timestamp=abi->round_timestamp;	
	abi->round_timestamp=time(NULL);
	if (abi->round_timestamp-old_timestamp<=1 ){//可能是外挂
		//打印出所有用户
		for (int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
			warrior_t* lp = &abi->challger.players[loop];
			if (lp->userid!=0 && lp->petid==0){
				KERROR_LOG(  lp->userid, "WG USER: time:%u:level:%u",
						abi->round_timestamp-old_timestamp , lp->level  );
			}
		}
	}
	abi->rand_speed = rand_speed;

	list_head_t* next;
	list_head_t* cur;
	list_for_each_safe(cur, next, &abi->attack_list) {
		resolved_atk_t* p_resolved_atk = list_entry(cur, resolved_atk_t, atk_list);
		list_del(&p_resolved_atk->atk_list);
		g_slice_free1(sizeof *p_resolved_atk, p_resolved_atk);
	}

	abi->seq_in_round = 0;
	INIT_LIST_HEAD (&abi->speed_list);

	// reset alive warrior's count
	abi->challger.alive_count = 0;
	abi->challgee.alive_count = 0;
	
	list_for_each_safe(cur, next, &abi->warrior_list){
		warrior_t* p = list_entry(cur, warrior_t, waor_list);

		p->set_attack_dumy(false);
		if (p->huwei_pos){
			remove_warrior_from_huweilist(p);
		}
		
		if (CANNOT_ATTACK(p)){
			clear_warrior_atkinfo(abi, p);
			if (!p->petid){
				set_warrior_stop_attack(abi, p);
			}
			continue;
		}

		p->self_team->alive_count++;

		if (IS_BEAST_ID(p->userid)) {
			clear_warrior_atkinfo(abi, p);
			continue;
		}

		if (p->check_state(person_break_off) && p->atk_info[0].atk_type == 0){
			KDEBUG_LOG(p->userid, "OFF LINE INTEL ATK\t[batid=%lu %u %u]", abi->batid, p->userid, p->petid);
			clear_warrior_atkinfo(abi, p);
			brk_off_user_intel_attack(abi, p);

			if (!p->petid){
				report_user_fighted(abi, p->userid, 0, proto_bt_noti_user_fighted);
			}
			continue;
		}

		clear_warrior_atkinfo(abi, p);
	}

	if (abi->fight_timer){
		DEBUG_LOG("RM ATK TM\t[batid=%lu %p]", abi->batid, abi->fight_timer);
		REMOVE_TIMER(abi->fight_timer);
		abi->fight_timer = NULL;
	}
	
	abi->fight_timer = ADD_ONLINE_TIMER(abi, n_attack_op_expired, 0, 120);	
}

static inline void init_user_state(battle_info_t* abi)
{
	list_head_t* lh;
	list_for_each(lh, &abi->warrior_list) {
		warrior_t* p = list_entry(lh, warrior_t, waor_list);
		p->p_waor_state->one_loop_state = 0;
		if (p->check_state( person_break_off)){
			p->p_waor_state->state = 0;
			SET_WARRIOR_STATE(p->p_waor_state->state, person_break_off);
		}else{
			p->p_waor_state->state = 0;
		}
	}
}

void report_battle_started(battle_info_t* abi)
{
	uint8_t buff[1000] = {0};

	int j_label = sizeof(protocol_t);
	PKG_UINT32(buff, abi->challgee.ave_lvl, j_label);

	int label_idx = j_label;
	uint32_t count=0;
	j_label += 4;

	list_head_t* lh;
	list_for_each(lh, &abi->warrior_list) {
		warrior_t* p = list_entry(lh, warrior_t, waor_list);
		if (!IS_BEAST_ID(p->userid) && p->petid==0 ){ //是人
				PKG_UINT32(buff, p->userid , j_label);
				PKG_UINT32(buff, 
						p->check_state( person_break_off)==0,
					   	j_label);

				count++;
		}
	}


	PKG_UINT32(buff, count, label_idx);
	init_proto_head(buff, proto_bt_noti_battle_started, j_label);

	send_to_online(j_label, buff, abi->challger.teamid, abi->batid, proto_bt_noti_battle_started, abi->challger.online_id);
	if (!IS_BEAST_ID(abi->challgee.teamid)){
		int tmp= sizeof(protocol_t);
		PKG_UINT32(buff, abi->challger.ave_lvl, tmp);
		send_to_online(j_label, buff, abi->challgee.teamid, abi->batid, proto_bt_noti_battle_started, abi->challgee.online_id);
	}

}

int bt_load_battle (userid_t uid, uint32_t online_id, battle_info_t* abi, Cmessage* c_in )
{
	bt_load_battle_in *p_in = P_IN;

	if (abi->batl_state != battle_ready) {
		ERROR_LOG("battle not in ready state\t[batid=%lu %u %u]", abi->batid, uid, abi->batl_state);
		return -1;
	}

	warrior_t* p = get_warrior_from_warriorlist(abi, uid, 0);
	if (p) {
		remove_warrior_loading_timer(p);
		create_warrior_loading_timer(abi, p);
		uint32_t cur_time = get_now_tv()->tv_sec;
		p->load_rate = p_in->rate;
		if (p->load_time != cur_time) {
		    p->load_time = cur_time;
		    report_user_loadrate(abi, uid, p_in->rate, proto_bt_noti_load_rate);
		}
	}
	return 0;
}

//检查,如果所有人都操作完毕，就开始计算
uint8_t check_battle_start(battle_info_t* abi)
{
	if (chk_all_user_ready(abi)) {
		if (abi->load_timer){
			REMOVE_TIMER(abi->load_timer);
			abi->load_timer = NULL;
		}
		
		abi->batl_state = battle_started;
		init_user_state(abi);
		report_battle_started(abi);
		new_round_battle(abi);
		return 1;
	}

	return 0;
}

int battle_load_expired(void* owner, void* data)
{
	battle_info_t* lbi = (battle_info_t*)owner;
	lbi->load_timer = NULL;
	std::vector<warrior_t*> time_out_list;
	
	list_head_t* lh;
	list_for_each(lh, &lbi->warrior_list) {
		warrior_t*p = list_entry(lh, warrior_t, waor_list);
		if (!p->ready && !(IS_BEAST(p))) {
			p->ready = 1;
			KDEBUG_LOG(p->userid, "XXXXX battle_load_expired set OFF LINE ");
			set_warrior_offline(lbi, p);
			time_out_list.push_back(p);
		}

		remove_warrior_loading_timer(p);
	}


	check_battle_start(lbi);
	for (uint32_t i=0;i<time_out_list.size();i++){
	   	notify_user_out_time_out(lbi,time_out_list[i]);
	}


	if (lbi->is_one_person_pk_beast()){
		chk_and_calc_attacks(lbi);
		report_battle_end(lbi);
	}
	
	return 0;
}

//超时,回调
int warrior_load_expired(void* owner, void* data)
{
	warrior_t* p = (warrior_t*)owner;
	battle_info_t* lbi = (battle_info_t*)data;
	ERROR_LOG("XXXXX warrior_load_expired ");
	DEBUG_LOG("XXXXX warrior_load_expired ");

	p->ready = 1;
	set_warrior_offline(lbi, p);
	check_battle_start(lbi);
	if (lbi->is_one_person_pk_beast()){
		chk_and_calc_attacks(lbi);
		report_battle_end(lbi);
	} else{
	   	notify_user_out_time_out(lbi,p);
	}
	
	return 0;
}

//水晶,相克
static int birthpet_restrict(pet_birth_t* fgter, pet_birth_t* fgtee)
{
	int a = fgter->water * fgtee->fire + fgter->fire * fgtee->wind + \
		fgter->wind * fgtee->earth + fgter->earth * fgtee->water;
	
	int b = fgtee->water * fgter->fire + fgtee->fire * fgter->wind + \
		fgtee->wind * fgter->earth + fgtee->earth * fgter->water;

	int a_suijin = fgter->water + fgter->fire + fgter->wind + fgter->earth;
	int b_suijin = fgtee->water + fgtee->fire + fgtee->wind + fgtee->earth;
	
	int ret = (a - b);
	if (a_suijin == 0 && b_suijin == 0 && ret == 0){
		return 0;
	}
	
	if (a_suijin == 0 && ret == 0){
		return -100;
	}

	if (b_suijin == 0 && ret == 0){
		return 100;
	}

	return ret;
}

int calc_hit_rate(warrior_t* p_fighter, warrior_t* p_fightee)
{
	float speed_div = 0;
	if (p_fighter->speed <= 0){
		KERROR_LOG(p_fighter->userid,"petid=%u SPEED INVALID",p_fighter->petid);
	}
	//速度300理论
	float ee_speed= p_fightee->speed>300?(300+3*((float)p_fightee->speed-300)/10):p_fightee->speed;
	float er_speed= p_fighter->speed>300?(300+3*((float)p_fighter->speed-300)/10):p_fighter->speed;
	speed_div = (ee_speed/er_speed);
	//speed_div *= speed_div;
	//speed_div *= 5.0;
	int hit_rate = 0;
	if (speed_div < 5.0){
		speed_div = 5.0;
	}else if (speed_div > 95.0){
		speed_div = 95.0;
	}
	//increase huibi
	float avoid_revise=1.0;
	int mifanquan_level=p_fightee->p_waor_state->get_mifanquan_level();
	if (mifanquan_level>0){
		DEBUG_LOG("mifanquan avoid effect");
		avoid_revise*=(1-0.07*mifanquan_level);
	}
	int jueqiquan_level=p_fightee->p_waor_state->get_jueqiquan_level ();
	if (jueqiquan_level>0){
		DEBUG_LOG("jueqiquan avoid effect");
		avoid_revise*=(1+0.07*jueqiquan_level );
	}

	float hit_revise=1.0;
	int jinghunquan_level=p_fighter->p_waor_state->get_jinghunquan_level();
	if(jinghunquan_level){
		DEBUG_LOG("jinghunquan hit effect");
		hit_revise*=(1.02+jinghunquan_level*0.03);
	}
	if (p_fighter->p_waor_state->state & xuanyun) { // xuanyun
		DEBUG_LOG("xuanyun effect");
		hit_revise*=0.75;
		//clean_one_time_state(p_fighter, xuanyun);
	}
	//DEBUG_LOG("fighter=%u fightee=%u",p_fighter->userid,p_fightee->userid);
	//DEBUG_LOG("shufu %lx  %lx",p_fighter->p_waor_state->state,p_fightee->p_waor_state->state);
	if (p_fightee->p_waor_state->state & shufu) { // shufu
		DEBUG_LOG("shufu effect");
		avoid_revise*=0.75;
		//clean_one_time_state(p_fightee, shufu);
	}
	hit_rate = 100 - speed_div + p_fighter->hit_rate*hit_revise - p_fightee->avoid_rate*avoid_revise;

	DEBUG_LOG("calc_hit_rate %d %f",hit_rate,speed_div);
	if (p_fightee->check_state( meiying_bit)){
		hit_rate = 100 - (100 - hit_rate) * (1.0 + 0.07 * p_fightee->p_waor_state->get_meiyin_level());
	}

	if (hit_rate < 5){
		hit_rate = 5;
	}else if (hit_rate > 95){
		hit_rate = 95;
	}
	
	KDEBUG_LOG(p_fighter->userid,"calc_hit_rate:petid=%u speed=%u hit_rate=%u",p_fighter->petid,p_fighter->speed,hit_rate);

	return hit_rate;
}

//XXX 计算命中率的
int calc_hit_result(battle_info_t* abi, warrior_t* p_fighter, warrior_t* p_fightee, nest_atk_t* p_nest_atk)
{
	DEBUG_LOG("calc_hit_result");
	if( p_nest_atk->atk_type == skill_ph_pet_gaopinzhenji 
			|| p_fightee->p_waor_state->check_state(jueduibizhang_bit)
			|| p_fightee->p_waor_state->check_state(shihua_bit)
			|| p_fightee->p_waor_state->check_state(hunshui_bit)){
		return 1;
	}
	int hit_rate = calc_hit_rate(p_fighter, p_fightee);
	if ((rand() % 100) > hit_rate && !IS_HEJI_SKILL(p_nest_atk->atk_type) \
		&& !p_fightee->check_state( fangyu_bit)
		&& !p_fightee->check_state( huandun_bit)
		&& !p_fightee->check_state( mokang_bit)
		&& !p_fightee->check_state( gongji_xishou_bit)
		&& !p_fightee->check_state( gongji_fantan_bit)
		&& !p_fightee->check_state( gongji_wuxiao_bit)
		&& !p_fightee->check_state( mofa_xishou_bit)
		&& !p_fightee->check_state( mofa_fantan_bit)
		&& !p_fightee->check_state( mofa_wuxiao_bit)) {

		if (abi->battle_loop != 0){
			return 0;
		}
		
		if (abi->touxi == touxi_by_challgee && p_fightee->self_team == &abi->challger){
		}else if (abi->touxi == touxi_by_challger && p_fightee->self_team == &abi->challgee){
		}else{
			return 0;
		}
	}

	return 1;
}

float calc_shuijing_xiangke(warrior_t* p_fighter, warrior_t* p_fightee)
{
	// (0.003 = 0.3 / 100)
	return 1 + 0.003 * birthpet_restrict((pet_birth_t*)&p_fighter->earth, (pet_birth_t*)&p_fightee->earth);
}

static int mp_fight(warrior_t* p_fighter, warrior_t* p_fightee, resolved_atk_t* p_resolved_atk, int hitresult, nest_atk_t* p_nest_atk, int huwei_happen)
{
	if (p_nest_atk->atk_level == 0){
		p_nest_atk->atk_level = 1;
	}

	float hit_revise = 1;
	if (!hitresult){
		hit_revise = 0;
		if (p_fightee->check_state( meiying_bit)){
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, meiying_bit);
		}else{
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, huibi_bit);
		}
	}

	int lhurt_mp = p_resolved_atk->hurtmp_rate * p_fightee->mp / 100;
	if (huwei_happen){
		lhurt_mp *= (1.66 - 0.08 * p_fightee->p_waor_state->get_huwei_level());
	}

	if (p_fightee->mp != 0 && p_resolved_atk->hurtmp_rate != 0 && lhurt_mp == 0){
		lhurt_mp = 1;
	}

	lhurt_mp *= hit_revise;
	return lhurt_mp;
}

static int physiq_fight(battle_info_t* abi, warrior_t* p_fighter, warrior_t* p_fightee, \
	nest_atk_t* p_nest_atk, int hitresult, int16_t* bisha_happen, int huwei_happen)
{
	if (p_nest_atk->atk_level == 0)
		p_nest_atk->atk_level = 1;

	// TAtk(攻击方参与伤害计算的攻击力)=300+3(Atk-300)/10
	float t_atk = p_fighter->attack_value;
	if (t_atk > 300.0){
		t_atk = 300.0 +  (t_atk - 300.0) * 3.0 / 10.0;
	}

	// TDef(目标方参与伤害计算的防御力)=300+3(Atk-300)/10
	float t_def = p_fightee->defense_value;
	if (t_def > 300.0){
		t_def = 300.0 +  (t_def - 300.0) * 3.0 / 10.0;
	}
	if (huwei_happen && !IS_BEAST(p_fightee))
		t_def *= (2 + 0.1 * p_fightee->p_waor_state->get_huwei_level());

	if(abi->is_pk_beast_2012() && p_fighter->type_id == TYPEID_2012) {
		if(rand() % 100 < 25) t_def *= 0.25;
	}
	//脱力:物攻、物防下降10%
	if (p_fighter->p_waor_state->state & tuoli) { // tuoli
		DEBUG_LOG("tuoli attack effect");
		t_atk *= 0.9;
		//clean_one_time_state(p_fighter, tuoli);
	}
	if (p_fightee->p_waor_state->state & tuoli) { // tuoli
		DEBUG_LOG("tuoli def effect");
		t_def *= 0.9;
		//clean_one_time_state(p_fightee, tuoli);
	}

	int mifanquan_level =p_fightee->p_waor_state->get_mifanquan_level();
	if (mifanquan_level>0){
		DEBUG_LOG("mifanquan fangyu effect");
		t_def*=(1+0.07*mifanquan_level  );
		//SET_WARRIOR_STATE(p_nest_atk->fightee_state, next_atk_fanji_bit);
	}
	int jueqiquan_level=p_fightee->p_waor_state->get_jueqiquan_level();
	if (jueqiquan_level>0){
		DEBUG_LOG("jueqiquan fangyu effect");
		t_def*=(1-0.07*jueqiquan_level );
	}

	//弱点透析对对手防御造成伤害
	if(p_fighter->check_state( ruodiantouxi_bit)){
		uint8_t rate=p_fighter->ruodiantouxi_lv*2+20;
		uint8_t hurt=p_fighter->ruodiantouxi_lv*2+13;
		if( rand()%100 < rate ){
			t_def *= 1-(float)hurt/100 	;
			//DEBUG_LOG("ruodiantouxi hurt t_def %f %u",t_def,p_fighter->ruodiantouxi_lv);
		}	
	}

	if (p_fightee->check_state( meiying_bit)){
		t_def *= (1 - 0.07 * p_fightee->p_waor_state->get_meiyin_level());
	}
	// bi_sha shanghai
	float bisha_revise = (t_def * p_fighter->level * 1.0)/(2.0 * p_fightee->level);
	
	//if (rand() % 100 > p_fighter->bisha)
		//bisha_revise = 0;
	//} else {
		//*bisha_happen = 1;
		//SET_WARRIOR_STATE(p_nest_atk->fightee_state, bisha_bit);
	//}
	jueqiquan_level=p_fighter->p_waor_state->get_jueqiquan_level();
	if (rand() % 100 < p_fighter->bisha+(jueqiquan_level*2+5)){
		DEBUG_LOG("bisha");
		*bisha_happen = 1;
		SET_WARRIOR_STATE(p_nest_atk->fightee_state, bisha_bit);
	} else {
		bisha_revise = 0;
	}

	float hit_revise = 1;
	if (!hitresult){
		hit_revise = 0;
		if (p_fightee->check_state( meiying_bit)){
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, meiying_bit);
		}else{
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, huibi_bit);
		}
	}

	// fangyu xiuzheng
	float defense_revise = 10;
	if (p_fightee->check_state( fangyu_bit) \
		&& (p_nest_atk->atk_type != skill_pa_bengya 
		&& p_nest_atk->atk_type != skill_jintouquan 
		&& p_nest_atk->atk_type != skill_pa_pet_bengya)) {
		defense_revise = rand() % 6;
		SET_WARRIOR_STATE(p_nest_atk->fightee_state, fangyu_bit);
	}
	
	if (!(p_fightee->check_state( fangyu_bit) || p_fightee->check_state( huandun_bit)) \
		&& (p_nest_atk->atk_type == skill_pa_bengya 
			|| p_nest_atk->atk_type == skill_pa_pet_bengya
			|| p_nest_atk->atk_type == skill_jintouquan)) {
		defense_revise = 0.0001;
	}
	
	// ji_neng xiu zheng, wuqi xiu zheng
	float skill_revise = 1, weapon_revise = 1.15;
	// shui_jing_xiang_ke (0.003 = 0.3 / 100)
	float birthpet_revise = calc_shuijing_xiangke(p_fighter, p_fightee);
	float attack_value = (3.0 * t_atk * t_atk) / (t_atk + 3 * t_def);
	// zhong_zu xiu zheng 
	uint32_t fighter_race=p_fighter->change_race?p_fighter->change_race:p_fighter->race;	
	uint32_t fightee_race=p_fightee->change_race?p_fightee->change_race:p_fightee->race;	
	//DEBUG_LOG("rrrrr %u %u %u",p_fighter->userid,p_fighter->petid,fighter_race);
	float race_revise = 1 + 0.15 * race_restrict[fighter_race][fightee_race];

	int32_t hurt_value = 0;
	if (hit_revise >= -0.001 && hit_revise <= 0.001){
		hurt_value = 0;
	}else{
		float real_hurt_value = weapon_revise * race_revise * skill_revise * (attack_value * birthpet_revise \
			* (90 + rand()%20) / 100 + bisha_revise) * defense_revise / 10.0;
		real_hurt_value = (real_hurt_value >= 1) ? real_hurt_value : 1;
		float result_value =  real_hurt_value * hit_revise;
		if (result_value >= 0 && result_value <= 1){
			hurt_value = 1;
		}else if (result_value <= 0 && result_value >= -1){
			hurt_value = -1;
		}else{
			hurt_value = result_value;
		}
	}

	if (p_fightee->check_state( huandun_bit) 
			&&  p_nest_atk->atk_type != skill_pa_bengya 
			&&  p_nest_atk->atk_type != skill_jintouquan
			&& p_nest_atk->atk_type != skill_pa_pet_bengya) {
		hurt_value = hurt_value * (50 - 5 * p_fightee->p_waor_state->get_hundun_level()) / 100.0;
		SET_WARRIOR_STATE(p_nest_atk->fightee_state, huandun_bit);
	}
	
	if(p_fightee->check_state( muyedun_bit)) {
		skill_attr_t *ska = get_skill_attr(p_nest_atk->atk_type,p_fighter->weapon_type);
		if(ska && ska->distance == near_attack) {
			if(rand() % 100 < 20 + 2 * p_fightee->p_waor_state->get_muyedun_level()) {
				SET_WARRIOR_STATE(p_nest_atk->fightee_state, muyedun_bit);
				hurt_value = hurt_value * (80.0 - 2.0 * p_fightee->p_waor_state->get_muyedun_level()) / 100.0;
			}
		}
	}

	mifanquan_level =p_fighter->p_waor_state->get_mifanquan_level();
	if (mifanquan_level>0){
		DEBUG_LOG("mifanquan hurt effect");
		hurt_value *= (0.7+0.1*mifanquan_level);
	}
	uint32_t qiliaodun_level=p_fightee->p_waor_state->get_qiliaodun_level(); 
	if (qiliaodun_level){
		DEBUG_LOG("qiliaodun effect hp");
		hurt_value *= (1-(0.37+0.03*qiliaodun_level));
	}

	uint32_t jintouquan_level=p_fighter->p_waor_state->get_jintouquan_level(); 
	//DEBUG_LOG("jintouquan effect %u %u",jintouquan_level,hurt_value);
	if (jintouquan_level){
		hurt_value *= (0.2+0.3*jintouquan_level);
	}

	//change_prof_revise 2012-3-19
	hurt_value *= 5;	

	if (hurt_value > 32767){
		hurt_value = 32767;
	}

	if (hit_revise >= -0.001 && hit_revise <= 0.001){
		hurt_value = 0;
	}else{
		if (hurt_value == 0){
			hurt_value = 1;
		}
	}
	KDEBUG_LOG(p_fighter->userid, "PH FIGHT\t[%f %f %f %d %f]", attack_value, t_atk, t_def, hurt_value, defense_revise);
	return hurt_value;
}



float scope_factor(skill_attr_t* p_skill_attr)
{
	switch (p_skill_attr->target_scale) {
		case t_enemy:
		//case c_enemy:
		//case r_enemy:
			return 0.6;
		case a_enemy:
			return 0.4;
		default:
			return 1.0;
	}
}

float spirit_revise_func(warrior_t* p_fighter, warrior_t* p_fightee)
{
	if (p_fightee->spirit == 0) 
		return 1;

	float spirit_scale = p_fighter->spirit / (float)p_fightee->spirit;
	if (spirit_scale >= 1.2) {
		return 1;
	} else if (spirit_scale >= 1.14) {
		return 0.9;
	} else if (spirit_scale >= 1.05) {
		return 0.82;
	} else if (spirit_scale >= 0.98) {
		return 0.63;
	} else if (spirit_scale >= 0.9) {
		return 0.55;
	} else if (spirit_scale >= 0.8) {
		return 0.36;
	} else if (spirit_scale >= 0.7) {
		return 0.27;
	} else {
		return 0.09;
	}
}

void magic_factor(uint32_t magic_skill, pet_birth_t* apb)
{
	switch (magic_skill) {
		case skill_ma_yunshishu: // yunshizhisu
		case skill_ma_pet_yunshishu:
		case skill_ma_jushizhou: // jushizheng
		case skill_ma_pet_jushizhou:
		case skill_ma_liuxingyu: //
		case skill_ma_pet_liuxingyu:
			apb->earth = 10; // tu
			return;
		case skill_ma_shuangdongshu:

		case skill_ma_pet_shuangdongshu:
		case skill_ma_hanbingzhou:
		case skill_ma_pet_hanbingzhou:
		case skill_ma_youyuanbing:
		case skill_ma_pet_youyuanbing:
			apb->water = 10; // shui
			return;
		case skill_ma_liuhuoshu:
		case skill_ma_pet_liuhuoshu:
		case skill_ma_liehuozhou:
		case skill_ma_pet_liehuozhou:
		case skill_ma_diyuhuo:
		case skill_ma_pet_diyuhuo:
			apb->fire = 10;  // huo
			return;
		case skill_ma_xuanfengshu:
		case skill_ma_pet_xuanfengshu:
		case skill_ma_kuangfengzhou:
		case skill_ma_pet_kuangfengzhou:
		case skill_ma_longjuanren:
		case skill_ma_pet_longjuanren:
			apb->wind = 10;  // feng
			return;
		default:
			return;
	}

}

static float calc_zhangmogong_revise(warrior_t* p_fighter, warrior_t* p_fightee, nest_atk_t* p_nest_atk)
{
	if (p_fighter->weapon_type != weapon_zhang){
		return 1.0;
	}

	float zhangmog_revise = 1.3;
	//float left_timit = (p_nest_atk->atk_level * 30.0 - 20);
	//float right_timit = (p_nest_atk->atk_level * 34.0 - 20);
    //if (p_fighter->mattack_value < left_timit){
    	//zhangmog_revise = 1.0;
    //}
    //else if(p_fighter->mattack_value >= left_timit && p_fighter->mattack_value < right_timit){
    	//zhangmog_revise = p_fighter->mattack_value/(p_nest_atk->atk_level * 30.0 - 20.0);
    //}
    //else{
    	//zhangmog_revise = 1.5;
    //}
	if( p_fighter->prof <= 5 ){
		float right_limit = (p_nest_atk->atk_level * 34.0 - 20)*1.3;
		if(p_fighter->mattack_value  < right_limit){
			zhangmog_revise = p_fighter->mattack_value/(p_nest_atk->atk_level * 30.0 - 20.0);
		}
		if(zhangmog_revise>=1.1){
			zhangmog_revise=1.1;
		}
	}else{
		float right_limit = (p_nest_atk->atk_level * 45.0 - 30)*1.3;
		if(p_fighter->mattack_value/5  < right_limit){
			zhangmog_revise = p_fighter->mattack_value/5/(p_nest_atk->atk_level * 45.0 - 30.0);
		}
	}
	return zhangmog_revise;
}
/*
int calc_magic_fight_hurt_value(battle_info_t* abi, warrior_t* p_fighter, warrior_t* p_fightee, atk_info_t* p_atk_info)
{
	skill_attr_t* p_skill_attr = get_skill_attr(p_atk_info->atk_type, p_fighter->weapon_type);
	if (!p_skill_attr || !p_skill_attr->id){
		return 0;
	}
	
	nest_atk_t ana_v;
	nest_atk_t* p_nest_atk = &ana_v;
	p_nest_atk->uid= p_atk_info->wrior->userid;
	p_nest_atk->petid= p_atk_info->wrior->petid;
	p_nest_atk->atk_type = p_atk_info->atk_type;
	p_nest_atk->atk_level = p_atk_info->atk_level;
	p_nest_atk->atk_uid = p_fightee->userid;
	p_nest_atk->atk_petid= p_fightee->petid;
		
	if (p_nest_atk->atk_level == 0){
		p_nest_atk->atk_level = 1;
	}
	
	float hurt_value;
	float ma_base_value, real_hurt_value, race_revise, weapon_revise = 1, birthpet_revise = 1;
	float zhangmog_revise = 1;
	float magic_revise, spirit_revise, ms_revise, magic_resist_revise = 1, magic_def_revise = 1;

	if (IS_BEAST_ID(p_fighter->userid) || p_fighter->petid){
		zhangmog_revise = 1.0;
		magic_def_revise = 1.0;
	}
	else{
		// zhang mo gong revise
		zhangmog_revise = calc_zhanmogong_revise(p_fighter, p_fightee, p_nest_atk);
		
		// mofang fangyu xiuzheng
		magic_def_revise = 1 - p_fighter->mdefense_value/1000.0;
	}

	// base attack
	ma_base_value =  (60.0 * p_nest_atk->atk_level + 23) * scope_factor(p_skill_attr);

	// shui jing xian ke
	birthpet_revise = 1 + 0.003 * birthpet_restrict((pet_birth_t*)&p_fighter->earth, (pet_birth_t*)&p_fightee->earth);
	
	pet_birth_t lpb = {0, 0, 0, 0};
	magic_factor(p_skill_attr->id, &lpb);
	// mo fa shuxing xianke
	magic_revise = 1 + 0.0035 * birthpet_restrict(&lpb, (pet_birth_t*)&p_fightee->earth);

	// zhong zhu xiuzheng
	race_revise = 0.1 * race_restrict[p_fighter->race][p_fightee->race];

	// shuang fa jing shen bi xiuzheng
	spirit_revise = spirit_revise_func(p_fighter, p_fightee);

	// fasong mofa jingshen bi xiuzheng
	ms_revise = (float)p_fighter->spirit / (p_nest_atk->atk_level * 20.0 + 103.0);
	
	ms_revise = ms_revise > 1.0 ? 1.0 : ms_revise;
	ms_revise *= (float)(95 + rand() % 11) / 100.0;
	
	real_hurt_value = ma_base_value * zhangmog_revise* birthpet_revise * weapon_revise * (magic_revise + \
		race_revise) * spirit_revise * ms_revise * magic_resist_revise * magic_def_revise;


	if (real_hurt_value >= -0.001 && real_hurt_value <= 0.001){
		hurt_value = 1;
	}else{
		hurt_value = real_hurt_value;
	}
	
	if (p_fightee->check_state( huandun_bit)) {
		// hurt_value = hurt_value * (60 - 5 * p_fightee->p_waor_state->hundun_level) / 100.0;
		real_hurt_value *= (60.0 - 5.0 * p_fightee->p_waor_state->get_hundun_level()) / 100.0;
		if (real_hurt_value >= -0.001 && real_hurt_value <= 0.001){
			hurt_value = 1;
		}else{
			hurt_value = real_hurt_value;
		}
	}

	if (p_fightee->check_state( mokang_bit)) {
		int factor = rand() % 6;
		factor = (factor) ? (factor * 10) : 1;
		// hurt_value = hurt_value * factor / 100.0;
		real_hurt_value *= factor / 100.0;
		if (real_hurt_value >= -0.001 && real_hurt_value <= 0.001){
			hurt_value = 1;
		}else{
			hurt_value = real_hurt_value;
		}
	}

	if (p_nest_atk->atk_type == skill_shengqiangtoushe	
			||p_nest_atk->atk_type== skill_shenglingzhaohuan){
		DEBUG_LOG("shengqiangtoushe   %u",p_fightee->race);
		if (p_fightee->race== race_undead ){
			DEBUG_LOG("shengyanshii:extra hurt for undead=%f",hurt_value);
			hurt_value*=(1.05+0.05*p_nest_atk->atk_level )	;
		}
	}

	if (hurt_value > 32767){
		hurt_value = 32767;
	}
	return (int)hurt_value;
}
*/
//魔法攻击
static int magic_fight(battle_info_t* abi, warrior_t* p_fighter, warrior_t* p_fightee, nest_atk_t* p_nest_atk, skill_attr_t* p_skill_attr)
{
		if (p_nest_atk->atk_level == 0){
			p_nest_atk->atk_level = 1;
		}
		
		float hurt_value;
		float ma_base_value, real_hurt_value, race_revise, weapon_revise = 1, birthpet_revise = 1;
		float zhangmog_revise = 1;
		float magic_revise, spirit_revise, ms_revise, magic_resist_revise = 1, magic_def_revise = 1;

		zhangmog_revise = 1.0;
		magic_def_revise = 1.0;

		switch (p_fighter->weapon_type){
			case weapon_zhang:
			case weapon_mo_zhang:
			case weapon_sheng_zhang:
				// zhang mo gong revise
				zhangmog_revise = calc_zhangmogong_revise(p_fighter, p_fightee, p_nest_atk);
				
				// mofang fangyu xiuzheng
				magic_def_revise = 1 - p_fightee->mdefense_value/1000.0;

				// wuqi xiuzheng
				weapon_revise = 1.15;
				break;
		}
		
		// base attack
		ma_base_value =  (60.0 * p_nest_atk->atk_level + 23) * scope_factor(p_skill_attr);

		// shui jing xian ke
		birthpet_revise = 1 + 0.003 * birthpet_restrict((pet_birth_t*)&p_fighter->earth, (pet_birth_t*)&p_fightee->earth);
		
		pet_birth_t lpb = {p_skill_attr->earth, p_skill_attr->water, p_skill_attr->fire, p_skill_attr->wind};
		if ((lpb.earth + lpb.fire + lpb.water + lpb.wind) == 0 && p_skill_attr->use_person){
			lpb.earth = p_fighter->earth;
			lpb.fire = p_fighter->fire;
			lpb.water = p_fighter->water;
			lpb.wind = p_fighter->wind;
		}
		
		// mo fa shuxing xiangke
		//if(p_nest_atk->atk_type != skill_fh_chouqu && (p_fighter->prof <= 5)) {
		//} else {
			//magic_revise = 1.0;
		//}
		if( p_nest_atk->atk_type == skill_fh_chouqu ||  (p_fighter->prof > 5) ){
			magic_revise = 1.0;
		}else{
			magic_revise = 1 + 0.0035 * birthpet_restrict(&lpb, (pet_birth_t*)&p_fightee->earth);
		}

		// zhong zhu xiuzheng
		uint32_t fighter_race=p_fighter->change_race?p_fighter->change_race:p_fighter->race;	
		uint32_t fightee_race=p_fightee->change_race?p_fightee->change_race:p_fightee->race;	
		race_revise = 0.1 * race_restrict[fighter_race][fightee_race];
		DEBUG_LOG("race_revise:%f magic_revise:%f %u",race_revise,magic_revise,race_restrict[p_fighter->race][p_fightee->race]);

		// shuang fa jing shen bi xiuzheng
		spirit_revise = spirit_revise_func(p_fighter, p_fightee);

		// fasong mofa jingshen bi xiuzheng
		ms_revise = (float)p_fighter->spirit / (p_nest_atk->atk_level * 20.0 + 103.0);
		ms_revise = ms_revise > 1.0 ? 1.0 : ms_revise;
		ms_revise *= (float)(95 + (rand() % 11)) / 100.0;
		// 203 * 
		real_hurt_value = ma_base_value * zhangmog_revise* birthpet_revise * weapon_revise * (magic_revise +  race_revise) * spirit_revise * ms_revise * magic_resist_revise * magic_def_revise;

		DEBUG_LOG("magic_attack:real_hurt_value: %f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f ",
				real_hurt_value,
				ma_base_value,
				zhangmog_revise,
				birthpet_revise,
				weapon_revise,
				(magic_revise+race_revise),
				spirit_revise,
				ms_revise,
				magic_resist_revise,
				magic_def_revise
				);

		if (real_hurt_value >= -0.001 && real_hurt_value <= 0.001){
			hurt_value = 1;
		}else{
			hurt_value = real_hurt_value;
		}
		
		if (p_fightee->check_state( huandun_bit)) {
			real_hurt_value *= (60.0 - 5.0 * p_fightee->p_waor_state->get_hundun_level()) / 100.0;
			if (real_hurt_value >= -0.001 && real_hurt_value <= 0.001){
				hurt_value = 1;
			}else{
				hurt_value = real_hurt_value;
			}
			
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, huandun_bit);
		}

		if (p_fightee->check_state( mokang_bit)) {
			int factor = rand() % 6;
			factor = (factor) ? (factor * 10) : 1;
			// hurt_value = hurt_value * factor / 100.0;
			real_hurt_value *= factor / 100.0;
			if (real_hurt_value >= -0.001 && real_hurt_value <= 0.001){
				hurt_value = 1;
			}else{
				hurt_value = real_hurt_value;
			}
			
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, mokang_bit);
		}

		if (p_nest_atk->atk_type == skill_shengqiangtoushe	
				||p_nest_atk->atk_type== skill_shenglingzhaohuan 	){
			DEBUG_LOG("shengyanshi:skill=%u",p_nest_atk->atk_type);
			if (p_fightee->race == race_undead ){
				hurt_value*=(1.05+0.05*p_nest_atk->atk_level )	;
			}
		}

		uint32_t qiliaodun_level=p_fightee->p_waor_state->get_qiliaodun_level(); 
		if (qiliaodun_level){
			DEBUG_LOG("qiliaodun effect mp");
			hurt_value *=(1-(0.37+0.03*qiliaodun_level));
		}

		//change_prof_revise 2012-3-19
		hurt_value *= 5;	

	//for skill 毒爆术 ，伤害增加体力*rate,清理一次中毒状态位 
		if (p_nest_atk->atk_type==skill_ma_pet_dubaoshu && p_fightee->check_state( zhongdu_bit)){
			p_fighter->set_dubaoshu_bit(1);
			clean_one_state(p_fightee,  zhongdu_bit, 10);
			RESET_WARRIOR_STATE(p_nest_atk->fightee_state ,zhongdu_bit );
			hurt_value +=(float)p_fightee->hp*(3+p_nest_atk->atk_level*2)/100;
			DEBUG_LOG("dubaoshu clean state and change hurt_value %f %f %u",hurt_value,((float)p_fightee->hp*(3+p_nest_atk->atk_level*2)/100),p_fightee->hp);
		}	
		float revise=1.0;//魔法攻击等级修正
		if(p_fighter->level > p_fightee->level){
			revise = pow(((float)(p_fighter->level))/p_fightee->level,0.5);
		}
		hurt_value *= revise;
		if (hurt_value > 32767){
			hurt_value = 32767;
		}
		DEBUG_LOG("MAGIC_ATTACK:ma_base_value:%d %f",int(ma_base_value),hurt_value);
		return (int)hurt_value;
}

static void calc_effector_state(battle_info_t* abi, warrior_t* effector, warrior_t* p_fighter, warrior_t* p_fightee, resolved_atk_t* p_resolved_atk, nest_atk_t* p_nest_atk, int bisha_happen)
{

	KDEBUG_LOG(effector->userid,"calc_effector_state:fightee's hp=%u",effector->hp);
	if (effector->hp != 0) 
		return;
	if (effector == p_fighter) { // 被反弹 
		if (abi->attack_out_able(effector) && bisha_happen) {
			set_warrior_attack_out(abi, effector);
			p_nest_atk->fantan_ret = 2;
		} else {
			if (!effector->check_state( attacked_out_bit)){
				set_warrior_dead_opt(abi, effector);
				p_nest_atk->fantan_ret = 1;
			}
		}
	} else if (!IS_HEJI_SKILL(p_nest_atk->atk_type)) {
		if (abi->attack_out_able(p_fightee) && (bisha_happen || p_nest_atk->hurt_hp < -p_fightee->hp_max * 3 ) ) {
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, attacked_out_bit);
			set_warrior_attack_out(abi, effector);
		} else {
			if (!effector->check_state( attacked_out_bit)){
				set_warrior_dead_opt(abi, effector);
				SET_WARRIOR_STATE(p_nest_atk->fightee_state, dead_bit);
			}
		}
	} else if (IS_HEJI_SKILL(p_nest_atk->atk_type) && p_resolved_atk->heji_seq == LAST_HEJI_WARR) {
		if (abi->attack_out_able(p_fightee) && (bisha_happen || p_fightee->heji_hurt < -p_fightee->hp_max * 3 )) {
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, attacked_out_bit);
			set_warrior_attack_out(abi, effector);
		} else {
			if (!effector->check_state( attacked_out_bit)){
				set_warrior_dead_opt(abi, effector);
				SET_WARRIOR_STATE(p_nest_atk->fightee_state, dead_bit);
			}
		}
	}
}

static void decre_def_equip_durable_value(battle_info_t* abi, warrior_t* effector, int32_t hurt_hp, int32_t hurt_mp)
{
	if (IS_BEAST(effector)){
		return;
	}
	
	if (hurt_hp >= 0 && hurt_mp >= 0){
		return;
	}

	if (effector->cloth_cnt <= 0){
		return;
	}

	int index = rand() % effector->cloth_cnt;
	if(effector->clothes[index].equip_type == part_weapon) {
		if(effector->cloth_cnt == 1) return;
		index = index + rand() % (effector->cloth_cnt - 1) + 1;
		index = index % effector->cloth_cnt;
	}
	effector->clothes[index].durable_val++;
}

static void check_incre_injury_lv_atk_out(int32_t hurt_hp, battle_info_t* abi, warrior_t* effector)
{
	//受攻击方等级<=15 不处理
	if (hurt_hp >= 0 || effector->level <= 15)// || effector->injury_lv >= injury_red)
		return;

	//对怪作战战不处理
	if (!((abi->challger.team_type | abi->challgee.team_type) & team_beast))
		return;

	//float over_rate = (float)(abs(hurt_hp) - effector->hp) / (float)(effector->hp_max);

	//0.2-0.5 不是打飞,可能要设置受伤标志
	//no use 2011-12-27
	//if (over_rate > 0.2 && over_rate < 0.5 && rand() % 100 < 80)
		//effector->injury_lv++;
}

static void rand_yiwang_skills(warrior_t* p)
{
	int i = 0;
	int n = 7;
	uint32_t skill[18] = {1,10,701,4,5,6,7,0};

	if(IS_BEAST(p)) n = 3;
	for(i = 0;i < p->skill_cnt;i++) {
		skill[n++] = p->skills[i].skill_id;
	}

	p->skills_forgot_cnt = rand() % 3 + 4;
	if(p->skills_forgot_cnt > n) p->skills_forgot_cnt = n;
	for(i = 0;i < p->skills_forgot_cnt;i++) {
		int k = rand() % n;
		p->skills_forgot[i] = skill[k];
		skill[k] = skill[--n];
		KDEBUG_LOG(p->userid,"set skills_forgot[%d]=%d",i,p->skills_forgot[i]);
	}
}

//XXX
static void do_skill_effect(warrior_t* p_fighter, warrior_t* p_fightee, nest_atk_t* p_next_atk)
{
	int effect_cnt = 0;
	int effect_prob = 0;
	int rand_prob = rand() % 10000;
	
	switch(p_next_atk->atk_type) {
		default:
			return;
		case skill_feiyejian:
		case skill_pet_feiyejian:
			effect_cnt = (p_next_atk->atk_level + 2) / 3;
			effect_prob = (p_next_atk->atk_level * 2 + 10) * 100;
			if(effect_prob > rand_prob) {
				p_fightee->p_waor_state->set_state_info(zhongdu_bit ,effect_cnt , 100 );
			}
			break;
		case skill_ph_pet_shihua:
			if(!(p_fightee->p_waor_state->state & abnormal_states)) {
				effect_prob = (p_next_atk->atk_level * 5 + 20) * (100 - p_fightee->r_shihua);
				if(effect_prob > rand_prob) {
					p_fightee->p_waor_state->set_state_info(shihua_bit ,2, 100 );
				}
			}
			break;
		case skill_ph_pet_yiwang:
			if(!(p_fightee->p_waor_state->state & abnormal_states)) {
				effect_prob = (p_next_atk->atk_level * 5 + 20) * (100 - p_fightee->r_yiwang);
				if(effect_prob > rand_prob) {
					p_fightee->p_waor_state->set_state_info(yiwang_bit ,2, 100 );
					rand_yiwang_skills(p_fightee);
				}
			}
			break;
		case skill_ph_pet_hunluan:
			if(!(p_fightee->p_waor_state->state & abnormal_states)) {
				effect_prob = (p_next_atk->atk_level * 5 + 20) * (100 - p_fightee->r_hunluan);
				if(effect_prob > rand_prob) {
					p_fightee->p_waor_state->set_state_info(hunluan_bit ,2, 100 );
				}
			}
			break;
		case skill_ph_pet_zuzhou:
			if(!(p_fightee->p_waor_state->state & abnormal_states)) {
				effect_prob = (p_next_atk->atk_level * 5 + 20) * (100 - p_fightee->r_zuzhou);
				if(effect_prob > rand_prob) {
					p_fightee->p_waor_state->set_state_info(zuzhou_bit ,2, 100 );
				}
			}
			break;
		case skill_ph_pet_zhongdu:
			if(!(p_fightee->p_waor_state->state & abnormal_states)) {
				effect_prob = (p_next_atk->atk_level * 5 + 20) * (100 - p_fightee->r_zhongdu);
				if(effect_prob > rand_prob) {
					p_fightee->p_waor_state->set_state_info(zhongdu_bit ,2, 100 );
				}
			}
			break;
		case skill_ph_pet_hunshui:
			if(!(p_fightee->p_waor_state->state & abnormal_states)) {
				effect_prob = (p_next_atk->atk_level * 5 + 20) * (100 - p_fightee->r_hunshui);
				if(effect_prob > rand_prob) {
					p_fightee->p_waor_state->set_state_info(hunshui_bit ,2, 100 );
				}
			}
			break;
		case skill_ma_pet_zhimianfenzhen:
			if(!(p_fightee->p_waor_state->state & abnormal_states) && rand_prob < (20 + p_next_atk->atk_level * 4) * 100) {

				p_fightee->p_waor_state->set_state_info(hunshui_bit ,2, 100 );
			}
			break;
		case skill_ph_pet_chanshentengman:
			if (!IS_BOSS_ID(p_fightee->userid) && !p_fightee->check_state( parasitism_bit) \
				&& rand_prob < (30 + p_next_atk->atk_level * 5) * 100) {
				p_fightee->p_waor_state->set_host_uid (p_fighter->userid);
				p_fightee->p_waor_state->set_host_petid (p_fighter->petid);

				p_fightee->p_waor_state->set_state_info(parasitism_bit ,2, 100 );
				p_fightee->p_waor_state->set_state_info(new_parasitism_bit ,0, 0 );
			}
			break;
		case skill_ma_pet_dubaoshu:
			if(!IS_BOSS_ID(p_fightee->userid) &&!(p_fightee->p_waor_state->state & abnormal_states)) {
				effect_prob = (10+p_next_atk->atk_level * 2) * (100 - p_fightee->r_zhongdu);
				if(!p_fighter->get_dubaoshu_bit()) {
					if(effect_prob > rand_prob) {
						if(p_fightee->p_waor_state->set_state_info(zhongdu_bit ,2, 100 )){
							SET_WARRIOR_STATE(p_next_atk->fightee_state, zhongdu_bit);
						}
					}
				}else{
					p_fighter->set_dubaoshu_bit(0);
				}
			}
			break;

	}
}

//XXX 每次计算攻击前
static void effect_fighter_fightee_hpmp(battle_info_t* abi, int32_t ahurt_hp, int32_t ahurt_mp, warrior_t* p_fighter, 
	warrior_t* p_fightee, skill_attr_t* p_skill_attr, resolved_atk_t* ap_resolved_atk, int fanji_seq, int bisha_happen, atk_info_t* p_atk_info )
{
	if (abs(ahurt_hp) > 32767){
		ahurt_hp = (ahurt_hp > 0)?32760:-32760;
	}

	if (abs(ahurt_mp) > 32767){
		ahurt_mp = (ahurt_mp > 0)?32760:-32760;
	}
	KDEBUG_LOG(p_fightee->userid,"petid=%u effect_fighter_fightee_hpmp:fightee's hp=%u",p_fightee->petid,p_fightee->hp);
	nest_atk_t* p_next_atk = &(ap_resolved_atk->fanji[fanji_seq]);
	p_next_atk->hurt_hp = ahurt_hp = -ahurt_hp;
	p_next_atk->hurt_mp = ahurt_mp = -ahurt_mp;
	warrior_t* effector = p_fightee;

	if(!IS_BEAST_ID(p_fightee->userid) && CHK_WARRIOR_STATE(p_next_atk->fightee_state,attacked_bit)) {
		if(abi->is_pk_beast_2012() && p_fighter->type_id == TYPEID_2012) {
			if(rand() % 100 < 50) {
				p_next_atk->add_hp = abs(ahurt_hp);
			}
			if(!(p_fightee->p_waor_state->state & abnormal_states) && rand() % 100 < 50) {
				p_fightee->p_waor_state->set_state_info(zhongdu_bit ,2, 100 );
			}
		}
	}

	if (p_skill_attr->skill_type == physi_attack || p_skill_attr->skill_type == mp_attack) {
		if(CHK_WARRIOR_STATE(p_next_atk->fightee_state,attacked_bit)) {
			do_skill_effect(p_fighter,p_fightee,p_next_atk);
		}
		if (p_fightee->p_waor_state->check_state(gongji_xishou_bit)) {
			ahurt_hp = -ahurt_hp;
			ahurt_mp = -ahurt_mp;
			p_next_atk->hurt_hp = ahurt_hp;	
			p_next_atk->hurt_mp = ahurt_mp;
			p_next_atk->fightee_state |= gongji_xishou;
			clean_one_time_state(p_fightee, gongji_xishou);
		}
		
		if (p_fightee->p_waor_state->state & gongji_fantan) { // must be closely attack
			effector = p_fighter;
			p_next_atk->hurt_hp = 0;			
			p_next_atk->hurt_mp = 0;
			p_next_atk->fantan_hp = ahurt_hp;
			p_next_atk->fantan_mp = ahurt_mp;
			p_next_atk->fightee_state |= gongji_fantan;

			clean_one_time_state(p_fightee, gongji_fantan);
		}
		
		if (p_fightee->p_waor_state->state & gongji_wuxiao) {
			ahurt_hp = 0;
			ahurt_mp = 0;
			p_next_atk->hurt_hp = 0;
			p_next_atk->hurt_mp = 0;
			p_next_atk->fightee_state |= gongji_wuxiao;

			clean_one_time_state(p_fightee, gongji_wuxiao);
		}
	}
	if (p_skill_attr->skill_type == magic_attack) {
		if(CHK_WARRIOR_STATE(p_next_atk->fightee_state,attacked_bit)) {
			do_skill_effect(p_fighter,p_fightee,p_next_atk);
		}
		if (p_fightee->p_waor_state->state & mofa_xishou) {//吸收
			ahurt_hp = -ahurt_hp;
			ahurt_mp = -ahurt_mp;
			p_next_atk->hurt_hp = ahurt_hp;	
			p_next_atk->hurt_mp = ahurt_mp;
			p_next_atk->fightee_state |= mofa_xishou;

			clean_one_time_state(p_fightee, mofa_xishou);
		}
		if (p_fightee->p_waor_state->state & mofa_fantan) { // 
			effector = p_fighter;
			p_next_atk->hurt_hp = 0;			
			p_next_atk->hurt_mp = 0;
			p_next_atk->fantan_hp = ahurt_hp;
			p_next_atk->fantan_mp = ahurt_mp;
			p_next_atk->fightee_state |= mofa_fantan;

			clean_one_time_state(p_fightee, mofa_fantan);
		}
		if (p_fightee->p_waor_state->state & mofa_wuxiao) {
			ahurt_hp = 0;
			ahurt_mp = 0;
			p_next_atk->hurt_hp = 0;
			p_next_atk->hurt_mp = 0;
			p_next_atk->fightee_state |= mofa_wuxiao;

			clean_one_time_state(p_fightee, mofa_wuxiao);
		}
	}

	switch(p_skill_attr->skill_type){
		case xixue_gongji:
		case fh_magic:
			p_next_atk->add_hp = abs(ahurt_hp) / 2;
			if (p_next_atk->add_hp < 1){
				p_next_atk->add_hp = 1;
			}
			
			if (p_skill_attr->skill_type == xixue_gongji){
				uint32_t maxhp = p_atk_info->atk_level * 30 + 10;
				maxhp *=5;
				p_next_atk->add_hp = (p_next_atk->add_hp > int (maxhp))?maxhp:p_next_atk->add_hp;
			}
			break;
	}
	if (ahurt_hp) {
		//check_incre_injury_lv_atk_out(ahurt_hp, abi, effector);
		effector->hp = bt_get_valid_value(effector->hp + ahurt_hp , 0, effector->hp_max); 
	}
	
	if (ahurt_mp) {
		effector->mp = bt_get_valid_value( effector->mp + ahurt_mp,0, effector->mp_max); 
	}

	switch(p_skill_attr->skill_type){
		case xixue_gongji:
		case fh_magic:
			if (effector == p_fighter && effector->hp == 0){
				p_next_atk->add_hp = 0;
				break;
			}
			p_fighter->hp = p_fighter->hp + p_next_atk->add_hp;
			p_fighter->hp =std::min(p_fighter->hp , p_fighter->hp_max); 
			break;
	}

	decre_def_equip_durable_value(abi, effector, ahurt_hp, ahurt_mp);

	if (IS_HEJI_SKILL(p_next_atk->atk_type)/* && effector == p_fightee*/) {
		if (effector->hp == 0){
			if (ap_resolved_atk->heji_seq != 0){
				p_atk_info->heji_seq = LAST_HEJI_WARR;
				ap_resolved_atk->heji_seq = LAST_HEJI_WARR;
				p_fightee->heji_hurt += ahurt_hp;
			}else{
				p_atk_info->atk_seq = 0;
				p_atk_info->atk_type = IS_BEAST(p_fighter)?skill_pa_pet_base_fight:skill_pa_base_fight;

				ap_resolved_atk->heji_seq = 0;
				ap_resolved_atk->atk_type = IS_BEAST(p_fighter)?skill_pa_pet_base_fight:skill_pa_base_fight;
				
				p_next_atk->atk_type = IS_BEAST(p_fighter)?skill_pa_pet_base_fight:skill_pa_base_fight;
			}
		}else{
			p_fightee->heji_hurt += ahurt_hp;
		}
	}

	calc_effector_state(abi, effector, p_fighter, p_fightee, ap_resolved_atk, p_next_atk, bisha_happen);

	//设置一些状态返回给客户端
	if (p_fightee->p_waor_state->state &need_send_warrior_state_flag && p_fightee->hp>0 ) {
		p_next_atk->fightee_state |= (need_send_warrior_state_flag & p_fightee->p_waor_state->state);
	}
}

static void clean_fangyu_state_before_second_attack(battle_info_t* abi, warrior_t* p_fighter, atk_info_t* p_atk_info)
{
	if (p_fighter->check_state( fangyu_bit)){
		RESET_WARRIOR_STATE(p_fighter->p_waor_state->state, fangyu_bit);
	}
	
	if (p_fighter->check_state( huandun_bit)) {
		RESET_WARRIOR_STATE(p_fighter->p_waor_state->state, huandun_bit);
		p_fighter->p_waor_state->set_hundun_level ( 0);
	}
	if (p_fighter->check_state( mokang_bit)) {
		RESET_WARRIOR_STATE(p_fighter->p_waor_state->state, mokang_bit);
	}

	if (p_fighter->check_state( huiji_bit)) {
		DEBUG_LOG("clean huiji bit");
		RESET_WARRIOR_STATE(p_fighter->p_waor_state->state, huiji_bit);
	}

	if (p_fighter->check_state( meiying_bit)) {
		RESET_WARRIOR_STATE(p_fighter->p_waor_state->state, meiying_bit);
	}

	if (p_fighter->huwei_pos){
		remove_warrior_from_huweilist(p_fighter);
	}
}

//回合结束，清除当前状态
static void clean_state_after_round_attack(battle_info_t* abi)
{
	DEBUG_LOG("clean_state_after_round_attack");
	list_head_t* lh;
	list_for_each(lh, &abi->warrior_list) {
		warrior_t* p = list_entry(lh, warrior_t, waor_list);
		if (p->check_state( fangyu_bit)){
			RESET_WARRIOR_STATE(p->p_waor_state->state, fangyu_bit);
		}
		
		if (p->check_state( huandun_bit)) {
			RESET_WARRIOR_STATE(p->p_waor_state->state, huandun_bit);
			p->p_waor_state->set_hundun_level ( 0);
		}
		
		if (p->check_state( mokang_bit)) {
			RESET_WARRIOR_STATE(p->p_waor_state->state, mokang_bit);
		}
		
		if (p->check_state( huiji_bit)) {
			RESET_WARRIOR_STATE(p->p_waor_state->state, huiji_bit);
		}

		if (p->check_state( meiying_bit)) {
			RESET_WARRIOR_STATE(p->p_waor_state->state, meiying_bit);
			p->p_waor_state->set_meiyin_level ( 0);
		}
		if (p->check_state( muyedun_bit)){
			RESET_WARRIOR_STATE(p->p_waor_state->state, muyedun_bit);
			p->p_waor_state->set_muyedun_level(0);
		}
		if (p->huwei_pos){
			remove_warrior_from_huweilist(p);
		}

		warrior_t* war_host = get_warrior_from_warriorlist(abi, p->p_waor_state->get_host_uid(), p->p_waor_state->get_host_petid());
		if ((!war_host || RUN_OR_OUT(war_host) || DEAD_FIGHTER(war_host))  && p->check_state( parasitism_bit)) {
			clean_one_state(p, parasitism_bit, 10);
		}

		if (p->check_state( new_parasitism_bit)) {
			RESET_WARRIOR_STATE(p->p_waor_state->state, new_parasitism_bit);
		}

		if (p->check_state( next_atk_fanji_bit)) {
			clean_one_state(p, next_atk_fanji_bit, 1);
		}

		if (p->check_state( yeshou_add_hert_hp_bit)) {
			clean_one_state(p, yeshou_add_hert_hp_bit, 1);
		}
		//清理弱点透析的状态位一次
		if (p->check_state( ruodiantouxi_bit)) {
			clean_one_state(p,ruodiantouxi_bit , 1);
		}
		if (p->check_state(fanghuzhao_bit)) {
			clean_one_state(p, fanghuzhao_bit, 1);
		}
		if (p->check_state(jueduibizhang_bit)) {
			clean_one_state(p, jueduibizhang_bit, 1);
		}

		for (uint32_t loop = 0; loop < p->skill_cnt; loop ++) {
			if (p->skills[loop].cool_round)
				p->skills[loop].cool_round --;
			p->skills[loop].cool_update = 0;
		}
		p->p_waor_state->set_qiliaodun_level(0);
		p->p_waor_state->set_mifanquan_level(0);
		p->p_waor_state->set_jueqiquan_level(0);
		p->p_waor_state->set_jinghunquan_level(0);
		p->p_waor_state->set_jintouquan_level(0);
	}
}

uint32_t get_beast_topic_id(battle_info_t* abi, warrior_t* p_fighter, atk_info_t* p_atk_info)
{
	// 怪物偷袭
	if (abi->touxi == touxi_by_challgee && abi->battle_loop == 0){
		return get_rand_beast_topic_id(topic_type_touxi);
	}

	// hp rate < 20%
	if ((p_fighter->hp / (p_fighter->hp_max * 1.0)) < 0.2){
		return get_rand_beast_topic_id(topic_type_hp);
	}

	skill_attr_t* p_skill_attr = get_skill_attr(p_atk_info->atk_type, p_fighter->weapon_type);
	if (p_skill_attr){
		return get_rand_beast_topic_id(p_skill_attr->skill_type + topic_type_usr_define);
	}

	return 0;
}

//.. 将会设置一个播放包。 状态位作用于hp mp   并会清除一次状态位
void state_effect_hpmp(battle_info_t* abi, warrior_t* p_fighter, atk_info_t* p_atk_info)
{
	KDEBUG_LOG(p_fighter->userid,"state_effect_hpmp");
	clean_fangyu_state_before_second_attack(abi, p_fighter, p_atk_info);
	int inc_seq_flag = (!IS_HEJI_SKILL(p_atk_info->atk_type) || (IS_HEJI_SKILL(p_atk_info->atk_type) && !p_atk_info->heji_seq));

	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p_fighter, p_atk_info, abi, inc_seq_flag);
	p_resolved_atk->atk_type = UPDATE_WARRIOR_STATE;
	p_resolved_atk->atk_uid = p_fighter->userid;
	p_resolved_atk->atk_petid= p_fighter->petid;

	//设置自身状态
	p_resolved_atk->fighter_state = p_fighter->p_waor_state->state;
	if (IS_BEAST_ID(p_fighter->userid)){
		p_resolved_atk->topic_id = get_beast_topic_id(abi, p_fighter, p_atk_info);
	}
	
	if (p_fighter->check_state( person_atk_out)){
		set_fangyu_state_after_first_attack(p_fighter, p_atk_info);
		return;
	}

	if (p_fighter->p_waor_state->state & resume_hp1) { // zai sheng zhizhen
		p_resolved_atk->change_hp += p_fighter->resume * p_fighter->p_waor_state->get_resume_hp1_level() * 100 / 100;
		clean_one_time_state(p_fighter, resume_hp1);
	}
	
	if (p_fighter->p_waor_state->state & resume_hp2) { // zai sheng lingyu
		p_resolved_atk->change_hp += p_fighter->resume * p_fighter->p_waor_state->get_resume_hp2_level() * 60 / 100;
		clean_one_time_state(p_fighter, resume_hp2);
	}
	
	if (p_fighter->p_waor_state->state & resume_hp3) { // zai sheng jiejie
		p_resolved_atk->change_hp += p_fighter->resume * p_fighter->p_waor_state->get_resume_hp3_level() * 40 / 100;
		clean_one_time_state(p_fighter, resume_hp3);
	}
	if (p_fighter->p_waor_state->state & resume_hp4) { // zaishengjiejie
		KDEBUG_LOG(p_fighter->userid,"zaishengjiejie effect");
		p_resolved_atk->change_hp += p_fighter->resume * p_fighter->p_waor_state->get_resume_hp4_level() * 40 / 100;
		clean_one_time_state(p_fighter, resume_hp4);
	}

	if (p_fighter->p_waor_state->state & zhuoshang ) {//zhuoshang
		DEBUG_LOG("zhuoshang effect hp=%u",p_fighter->hp);
		p_resolved_atk->change_hp -= (p_fighter->level*2)*5; 
		clean_one_time_state(p_fighter, zhuoshang );
	}
	if (p_fighter->p_waor_state->state & dongshang) { // dongshang
		p_resolved_atk->change_mp -= p_fighter->level*1.5;
		DEBUG_LOG("dongshang effect mp=%u",p_fighter->mp);
		clean_one_time_state(p_fighter, dongshang);
	}
	
	if (p_fighter->p_waor_state->state & zhongdu) {//zhongdu
		p_resolved_atk->change_hp -= p_fighter->hp * (10 + rand() % 3) / 100;
		clean_one_time_state(p_fighter, zhongdu);
		p_resolved_atk->fighter_state = p_fighter->p_waor_state->state;
	}
	
	if (p_fighter->p_waor_state->state & zuzhou) {//zuzhou
		p_resolved_atk->change_mp -= p_fighter->mp * (10 + rand() % 3) / 100;
		p_fighter->mp = (p_fighter->mp > p_fighter->mp_max)?p_fighter->mp_max:p_fighter->mp;
		clean_one_time_state(p_fighter, zuzhou);
	}

	if (p_fighter->check_state(parasitism_bit)) {
		warrior_t* war_host = get_warrior_from_warriorlist(abi, p_fighter->p_waor_state->get_host_uid(), p_fighter->p_waor_state->get_host_petid());
		if (!war_host || RUN_OR_OUT(war_host) || DEAD_FIGHTER(war_host)) {
			clean_one_state(p_fighter, parasitism_bit, 10);
		} else {
			p_resolved_atk->host_uid = war_host->userid;
			p_resolved_atk->host_petid = war_host->petid;
			if (p_fighter->check_state( new_parasitism_bit)) {
				p_resolved_atk->parasitism_hp = 0;
			} else {
				p_resolved_atk->parasitism_hp = -(p_fighter->hp * 0.1);
				p_resolved_atk->change_hp -=(p_fighter->hp * 0.1);
				war_host->hp -= p_resolved_atk->parasitism_hp;
				if (war_host->hp > war_host->hp_max)
					war_host->hp = war_host->hp_max;
				clean_one_state(p_fighter, parasitism_bit, 1);
			}
		}
	}
	p_fighter->hp += p_resolved_atk->change_hp;
	p_fighter->mp += p_resolved_atk->change_mp;
	DEBUG_LOG("state change mp=%d hp=%d",p_resolved_atk->change_mp,p_resolved_atk->change_hp);
	p_fighter->hp = (p_fighter->hp > p_fighter->hp_max)?p_fighter->hp_max:p_fighter->hp;
	p_fighter->hp = (p_fighter->hp < 0)?0:p_fighter->hp;
	p_fighter->mp = (p_fighter->mp > p_fighter->mp_max)?p_fighter->mp_max:p_fighter->mp;
	p_fighter->mp = (p_fighter->mp < 0)?0:p_fighter->mp;

	KDEBUG_LOG(p_fighter->userid,"state_effect_hpmp2:%u",p_fighter->hp);
	if(p_fighter->hp==0){//already dead  解决中了灼伤等标记位扣hp后不死问题，方法是在异常状态后检查hp是否是0，设置死状态
		set_warrior_dead_opt(abi,p_fighter);
		SET_WARRIOR_STATE(p_resolved_atk->fighter_state, dead_bit);
	}
	KDEBUG_LOG(p_fighter->userid,"state_effect_hpmp3:%u",p_fighter->p_waor_state->check_state(dead_bit));
	uint64_t ab_state = (p_fighter->p_waor_state->state & abnormal_states);
	switch(ab_state) {
		case hunluan:
			clean_one_state(p_fighter,hunluan_bit,1);
			if(rand() % 100 < p_fighter->r_hunluan) {
				clean_one_state(p_fighter,hunluan_bit,10);
			}
			break;
		case yiwang:
			clean_one_state(p_fighter,yiwang_bit,1);
			if(rand() % 100 < p_fighter->r_yiwang) {
				clean_one_state(p_fighter,yiwang_bit,10);
			}
			break;
		case zuzhou:
			if(rand() % 100 < p_fighter->r_zuzhou) {
				clean_one_state(p_fighter,zuzhou_bit,10);
			}
			break;
		case zhongdu:
			if(rand() % 100 < p_fighter->r_zhongdu) {
				DEBUG_LOG("clean_one_state(p_fighter,zhongdu_bit,10):%u",p_fighter->r_zhongdu );
				clean_one_state(p_fighter,zhongdu_bit,10);
			}
			break;
	}
	
	set_fangyu_state_after_first_attack(p_fighter, p_atk_info);
}

//XXX 使用技能  修改状态
static void change_enemy_state(battle_info_t* abi, warrior_t* p_fighter, warrior_t* p_fightee, resolved_atk_t* p_resolved_atk,nest_atk_t* p_nest_atk )
{
	int rand_prob = rand() % (1000 * 100);

	if(p_fightee->p_waor_state->state & abnormal_states) {
		return ;
	}

	switch (p_resolved_atk->atk_type) {
		case skill_ms_shihuazhizhen:
		case skill_ms_pet_shihuazhizhen:
			if(rand_prob < (40 + 60 * p_resolved_atk->atk_level) * (100 - p_fightee->r_shihua)) {

				p_fightee->p_waor_state->set_state_info(
						shihua_bit , 2 + p_resolved_atk->atk_level / 2, 100 );
			}
			break;
		
		case skill_ms_shihualingyu:
		case skill_ms_pet_shihualingyu:
			if(rand_prob < (24 + 36 * p_resolved_atk->atk_level) * (100 - p_fightee->r_shihua)) {

				p_fightee->p_waor_state->set_state_info(
						shihua_bit , 2 + p_resolved_atk->atk_level / 2, 100 );

			}
			break;
		case skill_ms_shihuajiejie:
		case skill_ms_pet_shihuajiejie:
			if(rand_prob < (16 + 24 * p_resolved_atk->atk_level) * (100 - p_fightee->r_shihua)) {

				p_fightee->p_waor_state->set_state_info(
						shihua_bit , 2 + p_resolved_atk->atk_level / 2, 100 );
			}
			break;
		case skill_ms_yiwangzhizhen:
		case skill_ms_pet_yiwangzhizhen:
			if(rand_prob < (40 + 60 * p_resolved_atk->atk_level) * (100 - p_fightee->r_yiwang)) {

				p_fightee->p_waor_state->set_state_info( yiwang_bit, 2 + p_resolved_atk->atk_level / 2, 100 );
				rand_yiwang_skills(p_fightee);
			}
			break;
		case skill_ms_yiwanglingyu:
		case skill_ms_pet_yiwanglingyu:
			if(rand_prob < (24 + 36 * p_resolved_atk->atk_level) * (100 - p_fightee->r_yiwang)) {

				p_fightee->p_waor_state->set_state_info( yiwang_bit, 2 + p_resolved_atk->atk_level / 2, 100 );
				rand_yiwang_skills(p_fightee);
			}
			break;
		case skill_ms_yiwangjiejie:
		case skill_ms_pet_yiwangjiejie:
			if(rand_prob < (16 + 24 * p_resolved_atk->atk_level) * (100 - p_fightee->r_yiwang)) {

				p_fightee->p_waor_state->set_state_info( yiwang_bit, 2 + p_resolved_atk->atk_level / 2, 100 );
				rand_yiwang_skills(p_fightee);
			}
			break;
		case skill_ms_hunluanzhizhen:
		case skill_ms_pet_hunluanzhizhen:
			if(rand_prob < (40 + 60 * p_resolved_atk->atk_level) * (100 - p_fightee->r_hunluan)) {

				p_fightee->p_waor_state->set_state_info(hunluan_bit  , 2 + p_resolved_atk->atk_level / 2, 100 );
			}
			break;
		case skill_ms_hunluanlingyu:
		case skill_ms_pet_hunluanlingyu:
			if(rand_prob < (24 + 36 * p_resolved_atk->atk_level) * (100 - p_fightee->r_hunluan)) {
				p_fightee->p_waor_state->set_state_info(hunluan_bit  , 2 + p_resolved_atk->atk_level / 2, 100 );
			}
			break;
		case skill_ms_hunluanjiejie:
		case skill_ms_pet_hunluanjiejie:
			if(rand_prob < (16 + 24 * p_resolved_atk->atk_level) * (100 - p_fightee->r_hunluan)) {

				p_fightee->p_waor_state->set_state_info(hunluan_bit  , 2 + p_resolved_atk->atk_level / 2, 100 );
			}
			break;
		case skill_ms_yuanlingzhifu:
		case skill_ms_pet_yuanlingzhifu:
			if(rand_prob < (40 + 60 * p_resolved_atk->atk_level) * (100 - p_fightee->r_zuzhou)) {

				p_fightee->p_waor_state->set_state_info(zuzhou_bit , 2 + p_resolved_atk->atk_level / 2, 100 );
			}
			break;
		case skill_ms_yuanlinglingyu:
		case skill_ms_pet_yuanlinglingyu:
			if(rand_prob < (24 + 36 * p_resolved_atk->atk_level) * (100 - p_fightee->r_zuzhou)) {

				p_fightee->p_waor_state->set_state_info(zuzhou_bit , 2 + p_resolved_atk->atk_level / 2, 100 );
			}
			break;
		case skill_ms_yuanlingjiejie:
		case skill_ms_pet_yuanlingjiejie:
			if(rand_prob < (16 + 24 * p_resolved_atk->atk_level) * (100 - p_fightee->r_zuzhou)) {

				p_fightee->p_waor_state->set_state_info(zuzhou_bit , 2 + p_resolved_atk->atk_level / 2, 100 );
			}
			break;
		case skill_ms_juduzhizhen:
		case skill_ms_pet_juduzhizhen:
			if(rand_prob < (40 + 60 * p_resolved_atk->atk_level) * (100 - p_fightee->r_zhongdu)) {

				p_fightee->p_waor_state->set_state_info(zhongdu_bit , 2 + p_resolved_atk->atk_level / 2, 100 );
			}
			break;
		case skill_ms_judulingyu:
		case skill_ms_pet_judulingyu:
			if(rand_prob < (24 + 36 * p_resolved_atk->atk_level) * (100 - p_fightee->r_zhongdu)) {
				p_fightee->p_waor_state->set_state_info(zhongdu_bit , 2 + p_resolved_atk->atk_level / 2, 100 );
			}
			break;
		case skill_ms_judujiejie:
		case skill_ms_pet_judujiejie:
			if(rand_prob < (16 + 24 * p_resolved_atk->atk_level) * (100 - p_fightee->r_zhongdu)) {

				p_fightee->p_waor_state->set_state_info(zhongdu_bit , 2 + p_resolved_atk->atk_level / 2, 100 );
			}
			break;
		case skill_ms_shuimianzhizhen:
		case skill_ms_pet_shuimianzhizhen:
			if(rand_prob < (40 + 60 * p_resolved_atk->atk_level) * (100 - p_fightee->r_hunshui)) {

				p_fightee->p_waor_state->set_state_info(hunshui_bit , 2 + p_resolved_atk->atk_level / 2, 100 );
			}
			break;
		case skill_ms_shuimianlingyu:
		case skill_ms_pet_shuimianlingyu:
			if(rand_prob < (24 + 36 * p_resolved_atk->atk_level) * (100 - p_fightee->r_hunshui)) {

				p_fightee->p_waor_state->set_state_info(hunshui_bit , 2 + p_resolved_atk->atk_level / 2, 100 );
			}
			break;
		case skill_ms_shuimianjiejie:
		case skill_ms_pet_shuimianjiejie:
			if(rand_prob < (16 + 24 * p_resolved_atk->atk_level) * (100 - p_fightee->r_hunshui)) {

				p_fightee->p_waor_state->set_state_info(hunshui_bit , 2 + p_resolved_atk->atk_level / 2, 100 );
			}
			break;


		case skill_zhouyuanzhimen:{
			uint8_t rand_bit_list[]={shihua_bit,hunshui_bit,hunluan_bit,yiwang_bit };
			uint8_t idx=rand()%4;
			int16_t anti_value=0;
			switch ( idx ){
				case 0:
					anti_value=p_fightee->r_zhongdu;
					break;
				case 1 :
					anti_value=p_fightee->r_zuzhou;
					break;
				case 2:
					anti_value=p_fightee->r_hunluan;
					break;
				case 3 :
					anti_value=p_fightee->r_yiwang;
					break;
				default :
					break;
			}
			uint8_t round=p_resolved_atk->atk_level/2+2;
			uint8_t rate=((p_resolved_atk->atk_level-1)/2*3+18)*(100-anti_value)/100;
			KDEBUG_LOG(p_fighter->userid,"skill_zhouyuanzhimen rate=%u idx=%u anti_valude=%u",rate,idx,anti_value);
			uint8_t need_set_bit= p_fightee->p_waor_state->set_some_state_info(rand_bit_list[idx],rate, round ,100);
			//uint8_t need_set_bit= p_fightee->p_waor_state->set_some_state_info(yiwang_bit,100, round ,100);
			DEBUG_LOG( "SET USER bit:uid=%u state=%u",p_fightee->userid,need_set_bit );
			if (need_set_bit ){
				SET_WARRIOR_STATE(p_nest_atk->fightee_state, need_set_bit );
			}

			break;
		}
		default:
			ERROR_LOG("invalid magic state change skill\t[batid=%lu btl_loop=%u %u %u]", abi->batid, abi->battle_loop, p_fighter->userid, p_resolved_atk->atk_type);
			return;
	}
}

static void magic_assist_skill(battle_info_t* abi, warrior_t* p_fighter, warrior_t* p_fightee, resolved_atk_t* p_resolved_atk, nest_atk_t* p_nest_atk)
{
	uint32_t atk_level=p_resolved_atk->atk_level;
	switch (p_resolved_atk->atk_type) {
		case skill_mas_zaishengzhizhen:
		case skill_mas_pet_zaishengzhizhen:
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, attacked_bit);
			p_fightee->p_waor_state->set_state_info(resume_hp1_bit,6,100);
			p_fightee->p_waor_state->set_resume_hp1_level (atk_level);
			break;
		case skill_mas_zaishenglingyu:			
		case skill_mas_pet_zaishenglingyu:
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, attacked_bit);
			p_fightee->p_waor_state->set_state_info(resume_hp2_bit,6,100);
			p_fightee->p_waor_state->set_resume_hp2_level ( atk_level);
			break;
		case skill_mas_zaishengjiejie:
		case skill_mas_pet_zaishengjiejie:
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, attacked_bit);
			p_fightee->p_waor_state->set_state_info(resume_hp3_bit,6,100);
			p_fightee->p_waor_state->set_resume_hp3_level ( atk_level);
			break;
		case skill_zaishengjiejie:
			DEBUG_LOG("zaishengjiejie set state");
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, attacked_bit);
			p_fightee->p_waor_state->set_state_info(resume_hp4_bit,6,100);
			p_fightee->p_waor_state->set_resume_hp4_level(atk_level);
			break;

		case skill_jueduibizhang:
			DEBUG_LOG("jueduibizhang set state");
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, attacked_bit);
			p_fightee->p_waor_state->set_state_info(jueduibizhang_bit,2,100);
			p_fightee->p_waor_state->set_fanghuzhao_value(
					p_fightee->resume *4* atk_level  );
			break;
		case skill_mas_wulixishou:
		case skill_mas_pet_wulixishou:
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, attacked_bit);
			p_fightee->p_waor_state->set_state_info(gongji_xishou_bit ,1,2 + atk_level / 2 );
			break;
		case skill_mas_wulifantan:
		case skill_mas_pet_wulifantan:
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, attacked_bit);
			p_fightee->p_waor_state->set_state_info(gongji_fantan_bit,1,2 + atk_level / 2 );
			break;
		case skill_mas_wulibizhang:
		case skill_mas_pet_wulibizhang:
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, attacked_bit);
			p_fightee->p_waor_state->set_state_info(gongji_wuxiao_bit,2,2 + atk_level / 2 );

			break;
		case skill_mas_mofaxishou:
		case skill_mas_pet_mofaxishou:
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, attacked_bit);
			p_fightee->p_waor_state->set_state_info(mofa_xishou_bit,1,2 + atk_level / 2 );
		
			break;
		case skill_mas_mofafantan:
		case skill_mas_pet_mofafantan:
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, attacked_bit);
			p_fightee->p_waor_state->set_state_info(mofa_fantan_bit,1, 2 + atk_level / 2 );
			break;
		case skill_mas_mofabizhang:
		case skill_mas_pet_mofabizhang:
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, attacked_bit);

			p_fightee->p_waor_state->set_state_info(mofa_wuxiao_bit ,2, 2 + atk_level / 2 );
			break;
		case skill_rh_pet_kuangyezhifun:  //狂野之魂
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, next_atk_fanji_bit);
			p_fightee->p_waor_state->set_state_info(next_atk_fanji_bit, 2+  atk_level / 2, 100 );
			break;
		case skill_rh_pet_ruodiantouxi:  //弱点透析
			DEBUG_LOG("ruodiantouxi set state");
			SET_WARRIOR_STATE(p_nest_atk->fightee_state, ruodiantouxi_bit);
			p_fightee->p_waor_state->set_state_info(ruodiantouxi_bit, 2 , 100 );
			break;

		default:
			ERROR_LOG("invalid magic assist skill\t[batid=%lu btl_loop=%u %u %u]", abi->batid, abi->battle_loop, p_fighter->userid, p_resolved_atk->atk_type);
			return;
	}
}

void clean_all_bad_state(warrior_t* aw)
{
	clean_one_state(aw, shihua_bit, 10);
	clean_one_state(aw, yiwang_bit, 10);
	clean_one_state(aw, hunluan_bit, 10);
	clean_one_state(aw, zuzhou_bit, 10);
	clean_one_state(aw, zhongdu_bit, 10);
	clean_one_state(aw, hunshui_bit, 10);

	clean_one_state(aw, zhuoshang_bit , 10);
	clean_one_state(aw, dongshang_bit, 10);
	clean_one_state(aw, shufu_bit, 10);
	clean_one_state(aw, xuanyun_bit, 10);
	clean_one_state(aw, tuoli_bit , 10);
	clean_one_state(aw, yeshou_add_hert_hp_bit,10);
	
}

static void clean_all_bad_state_by_rand(warrior_t* aw,int32_t rate)
{
	if( rand()%1000 < rate*10 ){
		clean_one_state(aw, shihua_bit, 10);
	}
	if( rand()%1000 < rate*10 ){
		clean_one_state(aw, yiwang_bit, 10);
	}
	if( rand()%1000 < rate*10 ){
		clean_one_state(aw, hunluan_bit, 10);
	}
	if( rand()%1000 < rate*10 ){
		clean_one_state(aw, zuzhou_bit, 10);
	}
	if( rand()%1000 < rate*10 ){
		clean_one_state(aw, zhongdu_bit, 10);
	}
	if( rand()%1000 < rate*10 ){
		clean_one_state(aw, hunshui_bit, 10);
	}
	if( rand()%1000 < rate*10 ){
		clean_one_state(aw, yeshou_add_hert_hp_bit,10);
	}
	if( rand()%1000 < rate*10 ){
		clean_one_state(aw, zhuoshang_bit , 10);
	}
	if( rand()%1000 < rate*10 ){
		clean_one_state(aw, dongshang_bit, 10);
	}
	if( rand()%1000 < rate*10 ){
		clean_one_state(aw, shufu_bit, 10);
	}
	if( rand()%1000 < rate*10 ){
		clean_one_state(aw, xuanyun_bit, 10);
	}
	if( rand()%1000 < rate*10 ){
		clean_one_state(aw, tuoli_bit , 10);
	}
}

static int magic_mend_skill(battle_info_t* abi, warrior_t* p_fighter, warrior_t* p_fightee, resolved_atk_t* p_resolved_atk, nest_atk_t* p_nest_atk, int * p_lhurt_mp )
{
	SET_WARRIOR_STATE(p_nest_atk->fightee_state, attacked_bit);
	int lhurt_hp = 0;
	switch (p_resolved_atk->atk_type) {
		case skill_rh_zhiyuzhizhen:
		case skill_rh_zhiyulingyu:
		case skill_rh_zhiyujiejie:
		case skill_rh_pet_zhiyuzhizhen:
		case skill_rh_pet_zhiyulingyu:
		case skill_rh_pet_zhiyujiejie:
		case skill_rh_pet_zhiyuhuahuan:
		case skill_shengtanglingyu :
		case skill_shenenjiejie:
			lhurt_hp = -p_resolved_atk->hurthp_rate * p_fightee->resume / 100;
			DEBUG_LOG("shenenjiejie %d %d",lhurt_hp,p_resolved_atk->hurthp_rate);
			break;
		case skill_rs_bingqingzhizhen:
		case skill_rs_pet_bingqingzhizhen:
		case skill_rs_bingqinglingyu:
		case skill_rs_pet_bingqinglingyu:
			if (rand() % 100 <= (35 + 5 * p_resolved_atk->atk_level)){
				clean_all_bad_state(p_fightee);
			}
			break;
		case skill_shengyanshipian:
			DEBUG_LOG("shengyanshipian");
			clean_all_bad_state_by_rand(p_fightee,(35 + 5 * p_resolved_atk->atk_level));
			break;
		case skill_rs_bingqingjiejie:
		case skill_rs_pet_bingqingjiejie:{
			if (rand() % 100 <= (25 + 5 * p_resolved_atk->atk_level)){
				clean_all_bad_state(p_fightee);
			}
			break;
		}
		case skill_rl_jiushu: //复活
		case skill_rl_pet_jiushu:
		case skill_shenyuhuhuan:
			// clear dead bit
			//DEBUG_LOG("shenyuhuhuan effect %u",p_fightee->check_state( dead_bit));
			if (p_fightee->check_state( dead_bit)){
				lhurt_hp = -p_resolved_atk->hurthp_rate / 100;
				*p_lhurt_mp = -p_resolved_atk->hurtmp_rate / 100;
				set_warrior_alive_opt(p_fightee);
				SET_WARRIOR_STATE(p_nest_atk->fightee_state, revive_bit);
				reset_warrior_stop_attack(abi, p_fightee);
			}
			DEBUG_LOG("jiushu effect %u %u",p_fightee->self_team->alive_count,p_fightee->hp);
			break;
		case skill_rh_mingsi:
		case skill_rh_pet_mingsi:
			lhurt_hp = - p_resolved_atk->hurthp_rate * p_fightee->hp_max / 100;
			break;
		case skill_qiliaodun:
			lhurt_hp = - p_resolved_atk->hurthp_rate * p_fightee->hp_max / 100;
			DEBUG_LOG("qiliaodun recover hp %d",lhurt_hp);
			p_fightee->p_waor_state->set_qiliaodun_level(p_resolved_atk->atk_level );
			break;

		default:
			RESET_WARRIOR_STATE(p_nest_atk->fightee_state, attacked_bit);
			ERROR_LOG("invalid magic assist skill\t[batid=%lu btl_loop=%u %u %u]", abi->batid, abi->battle_loop, p_fighter->userid, p_resolved_atk->atk_type);
	}
	return lhurt_hp;
}

static void calc_huiji_attack(battle_info_t* abi, resolved_atk_t* p_resolved_atk, warrior_t* p_fighter, warrior_t* p_fightee, atk_info_t* p_atk_info)
{
	//DEBUG_LOG("calc_huiji_attack %u",p_fightee->userid);
	uint32_t atk_type = IS_BEAST(p_fighter) ? skill_pd_pet_huiji : skill_pd_huiji;
	if( p_fighter->p_waor_state->get_mifanquan_level()){
		atk_type=skill_mifanquan;
	}
	skill_attr_t* p_skill_attr = get_skill_attr(atk_type, p_fighter->weapon_type);
	//DEBUG_LOG("calc_huiji_attack %u %u",atk_type,p_skill_attr->id);
	if (!p_skill_attr || !p_skill_attr->id){
		ERROR_LOG("invalid huiji skill\t[uid=%u petid=%u weapon_type=%u]", p_fighter->userid, p_fighter->petid, p_fighter->weapon_type);
		return;
	}

	//DEBUG_LOG("calc_huiji_attack2");
	nest_atk_t* p_next_atk = &(p_resolved_atk->fanji[p_resolved_atk->atk_cnt]);
	p_next_atk->uid= p_fighter->userid;
	p_next_atk->petid= p_fighter->petid;
	p_next_atk->atk_type = atk_type;
	p_next_atk->atk_level = 0;
	p_next_atk->atk_uid = p_fightee->userid;
	p_next_atk->atk_petid= p_fightee->petid;

	int32_t lhurt_hp = 0, lhurt_mp = 0;
	int16_t bisha_h = 0;
	int hitresult = calc_hit_result(abi, p_fighter, p_fightee, p_next_atk);
	lhurt_hp = physiq_fight(abi, p_fighter, p_fightee, p_next_atk, hitresult, &bisha_h, 0);
	lhurt_hp *= (0.8 + 0.1 * p_resolved_atk->atk_level);
	if (!CHK_WARRIOR_STATE(p_next_atk->fightee_state, huibi_bit)) {
		SET_WARRIOR_STATE(p_next_atk->fightee_state, attacked_bit);
	}
	
	effect_fighter_fightee_hpmp(abi, lhurt_hp, lhurt_mp, p_fighter, p_fightee, p_skill_attr, p_resolved_atk, p_resolved_atk->atk_cnt, bisha_h, p_atk_info);
	p_resolved_atk->atk_cnt++;
}

static void calc_fanji_attack(battle_info_t* abi, resolved_atk_t* p_resolved_atk, warrior_t* p_fighter, warrior_t* p_fightee, atk_info_t* p_atk_info  )
{
	DEBUG_LOG("calc_fanji_attack" );
	uint32_t atk_type = IS_BEAST(p_fighter) ? skill_pa_pet_base_fight : skill_pa_base_fight;
	skill_attr_t* p_skill_attr = get_skill_attr(atk_type, p_fighter->weapon_type);
	if (!p_skill_attr || !p_skill_attr->id){
		ERROR_LOG("fanji invalid\t[uid=%u %u weapon=%u]", p_fighter->userid, p_fighter->petid, p_fighter->weapon_type);
		return;
	}
		
	nest_atk_t* p_next_atk = &(p_resolved_atk->fanji[p_resolved_atk->atk_cnt]);
	p_next_atk->uid= p_fighter->userid;
	p_next_atk->petid= p_fighter->petid;
	p_next_atk->atk_type = atk_type;
	p_next_atk->atk_level = 0;
	p_next_atk->atk_uid = p_fightee->userid;
	p_next_atk->atk_petid= p_fightee->petid;



	int32_t lhurt_hp = 0, lhurt_mp = 0;
	int16_t bisha_h = 0;
	
	int hitresult = calc_hit_result(abi, p_fighter, p_fightee, p_next_atk);
	lhurt_hp = physiq_fight(abi, p_fighter, p_fightee, p_next_atk, hitresult, &bisha_h, 0);
	if (!CHK_WARRIOR_STATE(p_next_atk->fightee_state, huibi_bit)) {
		SET_WARRIOR_STATE(p_next_atk->fightee_state, attacked_bit);
	}


	effect_fighter_fightee_hpmp(abi, lhurt_hp, lhurt_mp, p_fighter, p_fightee, p_skill_attr, p_resolved_atk, p_resolved_atk->atk_cnt, bisha_h, p_atk_info);
	p_resolved_atk->atk_cnt++;
}

inline void set_fuzhu_pet_runaway(battle_info_t* abi, userid_t uid)
{
	list_head_t* lh;
	list_for_each(lh, &abi->warrior_list) {
		warrior_t* p = list_entry(lh, warrior_t, waor_list);
		if (p->userid == uid && p->petid &&  p->pet_state == on_fuzhu ){
			decr_pet_count(abi, p);
			SET_WARRIOR_STATE(p->p_waor_state->state, run_away_bit);
		}
	}
}

static void proc_run_away(battle_info_t* abi, warrior_t* aw, nest_atk_t* p_nest_atk)
{
	KDEBUG_LOG(aw->userid,"proc_run_away");
	if (aw->petid==0){
		if (!aw->check_state( dead_bit) && 
			!aw->check_state( attacked_out_bit)){
			decr_person_count(abi, aw);
		}
	}

	if (aw->huwei_pos){
		remove_warrior_from_huweilist(aw);
	}
	
	SET_WARRIOR_STATE(p_nest_atk->fightee_state, run_away_bit);
	SET_WARRIOR_STATE(aw->p_waor_state->state, run_away_bit);

	if (aw->petid==0){
		warrior_t* pw = get_pet_from_warriorlist(abi, aw->userid);
		// set chongwu tao pao mark
		if (pw) {
			// if chongwu huwei bieren
			// remove it from huwei list
			if (pw->huwei_pos){
				remove_warrior_from_huweilist(pw);
			}
			decr_pet_count(abi, pw);
			SET_WARRIOR_STATE(pw->p_waor_state->state, run_away_bit);
		}
		//set fuzhu pet taopao
		set_fuzhu_pet_runaway(abi,aw->userid);
	}else{
		decr_pet_count(abi, aw);
	}
}

static void change_use_position(battle_info_t* abi, warrior_t* aw)
{
	battle_users_t* selfteam = aw->self_team;

	//得到自己的位置
	int pos = get_warrior_pos(selfteam, aw->userid, aw->petid);
	int other_pos =  (pos >= MAX_PLAYERS_PER_TEAM) ? (pos - MAX_PLAYERS_PER_TEAM) : (pos + MAX_PLAYERS_PER_TEAM);

	//修改护卫列表
	chg_warrior_pos_from_huweilist(abi, aw, other_pos);
	aw->in_front = !aw->in_front;

	warrior_t* tmp_other= selfteam->players_pos[other_pos];
	if (tmp_other){
		chg_warrior_pos_from_huweilist(abi, tmp_other , pos);
		tmp_other->in_front = !tmp_other->in_front;
	}

	//调整位置
	selfteam->players_pos[pos] = selfteam->players_pos[other_pos];
	selfteam->players_pos[other_pos] = aw;

	notify_chg_pos(abi, aw);
}


static int proc_use_medical(battle_info_t* abi, warrior_t* p_fighter, warrior_t* p_fightee, nest_atk_t* p_nest_atk)
{
	// notify online to delete chemical
	notify_use_medical(abi, p_fighter);
	p_nest_atk->use_itemid = p_fighter->p_waor_state->get_use_item_itemid();
	SET_WARRIOR_STATE(p_nest_atk->fightee_state, use_item_bit);

	return -(p_fightee->resume * p_fighter->p_waor_state->get_use_item_add_hp()* (80 + rand() % 40)) / 100 / 100;
}

int calc_catch_pet_success(warrior_t* p_fighter, warrior_t* p_fightee)
{
	if (!p_fightee || NONEED_ATTACK(p_fightee) || !IS_BEAST_ID(p_fightee->userid) || !(p_fightee->catchable)){
		return 0;
	}

	uint32_t loop = 0;

	Cwarrior_state_ex_catch_pet *p_state_ex ;
	p_state_ex=(Cwarrior_state_ex_catch_pet *) p_fighter->p_waor_state->get_state_ex_info(CATCH_PET_STATE_BIT );
	if (!p_state_ex ){
		DEBUG_LOG("ERROR nofind Cwarrior_state_ex_catch_pet");
		return 0;
	}
	
	for (loop = 0; loop < p_state_ex->race_cnt; loop++){
		if (p_fightee->race == (uint8_t)p_state_ex->race[loop]){
			break;
		}
	}

	if (loop == p_state_ex->race_cnt){
		return 0;
	}

	double successrand = 1.0 / sqrt(p_fightee->level) * (1 - p_fightee->hp/(float)p_fightee->hp_max) * (1 / (float)(p_fightee->handbooklv + 1)) + 0.03 * p_fighter->pet_contract_lv + 0.03 * p_state_ex->feng_mo_lv;
	successrand = successrand * 100.0;
	successrand *= (p_fightee->catchable/1000.0);
	switch (p_fightee->level)	{
	case 1:
		successrand /= 2;
		break;
	}

	return (int)successrand;
}

static void proc_callback_fuzhu(battle_info_t* abi, warrior_t* p_fighter, warrior_t* p_fightee, resolved_atk_t* p_resolved_atk, nest_atk_t* p_nest_atk)
{
	if (!abi->is_one_person_pk_beast() ){
		DEBUG_LOG("ERR:proc_callback_fuzhu is_one_person_pk_beast==false" );
		return ;
	}
	struct battle_users *  p_team=p_fighter->self_team;
	//处理1，2位置
	for (int i=1;i<=2;i++ ){
		warrior_t *p_user= p_team->players_pos[i];
		if (p_user!=NULL){
			DEBUG_LOG("ADD callback_fuzhu_resolve_atk ");
			//以逃跑为标志确定是否召回或放出
			if( p_user->check_state( run_away_bit)){
				RESET_WARRIOR_STATE(p_user->p_waor_state->state, run_away_bit);
			}else{
				//将协助宠物设置为逃跑
				SET_WARRIOR_STATE(p_user->p_waor_state->state, run_away_bit);
			}
		}
	}

}


static void proc_recall_pet(battle_info_t* abi, warrior_t* p_fighter, warrior_t* p_fightee, resolved_atk_t* p_resolved_atk, nest_atk_t* p_nest_atk)
{
	p_nest_atk->atk_uid = p_fighter->userid;

	battle_users_t* selfteam = p_fighter->self_team;
	
	// zhao dao ren de wei zhi
	int pos = get_warrior_pos(selfteam, p_fighter->userid, p_fighter->petid);
	
	// zhao dao chong wu de wenzhi
	int pet_pos = (pos >= MAX_PLAYERS_PER_TEAM) ? (pos - MAX_PLAYERS_PER_TEAM) : (pos + MAX_PLAYERS_PER_TEAM);

	uint32_t cur_petid = 0;
	uint32_t new_petid = 0;

	// first call back
	warrior_t* cur_pet = selfteam->players_pos[pet_pos];
	if (cur_pet){
		if (cur_pet->huwei_pos){
			remove_warrior_from_huweilist(cur_pet);
		}
		cur_pet->pet_state = on_standby;
		cur_petid = cur_pet->petid;
		list_del(&cur_pet->waor_list);
		selfteam->players_pos[pet_pos] = NULL;
	}
	cur_pet = NULL;

	// then send out
	if (p_resolved_atk->atk_petid){
		int newphy = get_pet_phy_pos(selfteam, pos, p_resolved_atk->atk_petid);
		
		selfteam->players_pos[pet_pos] = &selfteam->players[newphy];
		cur_pet = selfteam->players_pos[pet_pos];		
		list_add_tail(&(selfteam->players[newphy].waor_list), &abi->warrior_list); //  set to warrior list
		
		cur_pet->pet_state = rdy_fight;
		new_petid = cur_pet->petid;
	}
	else{
		p_nest_atk->atk_uid = 0;
	}

	notify_recall_pet(abi, p_fighter, cur_petid, new_petid, cur_petid + new_petid);
}

static void proc_catch_pet(battle_info_t* abi, warrior_t* p_fighter, warrior_t* p_fightee, nest_atk_t* p_nest_atk)
{
	int catch_rand = calc_catch_pet_success(p_fighter, p_fightee);
	uint8_t succ = catch_rand && (rand() % 100 <= catch_rand);

	if (succ) {
		decr_person_count(abi, p_fightee);
		p_fighter->pet_cnt++;
		SET_WARRIOR_STATE(p_fightee->p_waor_state->state, catch_pet_succ);
		SET_WARRIOR_STATE(p_nest_atk->fightee_state, catch_pet_succ);
		notify_catch_pet(abi, p_fighter, p_fightee->userid, 1, p_fightee->level);
	} else {
		notify_catch_pet(abi, p_fighter, (p_fightee)?p_fightee->userid:0, 0, (p_fightee)?p_fightee->level:0);
	}
}

static uint8_t replace_fightee_by_huwei(skill_attr_t* p_skill_attr, warrior_t** pp_fightee, nest_atk_t* p_nest_atk)
{
	if (p_skill_attr->distance != near_attack || (p_nest_atk->atk_type % 5000) == skill_shunsha) {//先制无法护卫
		return 0;
	}

	warrior_t* huwei = get_warrior_from_huweilist(*pp_fightee);
	if (huwei == NULL){
		return 0;
	}

	*pp_fightee = huwei;
	p_nest_atk->huwei_pos = get_warrior_pos(huwei->self_team, huwei->userid, huwei->petid);
	p_nest_atk->huwei_pos++;

	SET_WARRIOR_STATE(huwei->p_waor_state->one_loop_state, one_loop_bit_huwei);
	return 1;
}

static uint8_t check_warrior_can_fanji(battle_info_t* abi, warrior_t* p_fighter, skill_attr_t* p_skill_attr, resolved_atk_t* p_resolved_atk)
{
	if (abi->touxi == touxi_by_challgee && abi->battle_loop == 0 && p_fighter->self_team == &abi->challger){
		DEBUG_LOG("fanji false 1");
		return 0;
	}else if (abi->touxi == touxi_by_challger && abi->battle_loop == 0 && p_fighter->self_team == &abi->challgee){
		DEBUG_LOG("fanji false 2");
		return 0;
	}

	if (IS_HEJI_SKILL(p_resolved_atk->atk_type) || !p_skill_attr->fanji){
		DEBUG_LOG("fanji false 3");
		return 0;
	}

	if (CHK_WARRIOR_STATE(p_fighter->p_waor_state->one_loop_state, one_loop_bit_huwei)){
		DEBUG_LOG("fanji false 4");
		return 0;
	}

	if (p_fighter->weapon_type == weapon_gong){
		DEBUG_LOG("fanji false 5");
		return 0;
	}

	if (p_fighter->check_state( fangyu_bit)  || 
		p_fighter->check_state( huandun_bit) ||
		p_fighter->check_state( mokang_bit)  ||
		p_fighter->check_state( shihua_bit)  ||
		p_fighter->check_state( hunshui_bit)  ){
		DEBUG_LOG("fanji false 6");
		return 0;
	}

	return 1;
}

static uint8_t check_warrior_can_huiji(warrior_t* p_fighter, skill_attr_t* p_skill_attr, resolved_atk_t* p_resolved_atk)
{
	if (IS_HEJI_SKILL(p_resolved_atk->atk_type) || !p_skill_attr->huiji){
		DEBUG_LOG("huiji fail1:%u %u",IS_HEJI_SKILL(p_resolved_atk->atk_type),!p_skill_attr->huiji);
		return 0;
	}

	if (!p_fighter->check_state( huiji_bit)){
		DEBUG_LOG("huiji fail2");
		return 0;
	}

	if (p_fighter->weapon_type == weapon_gong){
		DEBUG_LOG("huiji fail3");
		return 0;
	}

	if( !p_fighter->p_waor_state->get_mifanquan_level() && p_fighter->check_state( fangyu_bit)){
		DEBUG_LOG("huiji fail4");
	}	
	if (p_fighter->check_state( huandun_bit) ||
		p_fighter->check_state( mokang_bit)  ||
		p_fighter->check_state( shihua_bit)  ||
		p_fighter->check_state( hunshui_bit)  ){
		DEBUG_LOG("huiji fail5");
		return 0;
	}

	return 1;
}
static void calc_2004(  battle_info_t* abi, warrior_t* p_fightee , resolved_atk_t* p_resolved_atk   ,
	int32_t &lhurt_hp , int32_t &lhurt_mp 
		)
{
	if (abi->is_pk_beast_2004() &&  p_fightee->type_id==TYPEID_2004 ){
		int no_boss_count= get_no_boss_count( &(abi->challgee),TYPEID_2004 );
		if (! abi->is_no_boss_lt_3_in_pk_beast_2004() &&  //boss在招唤小弟不处理
			   	no_boss_count<5 && no_boss_count>=3 //小弟个数不在3－5不处理 
			) {
			uint32_t tmp_atk_type=p_resolved_atk->atk_type%5000;
			if (tmp_atk_type ==3701 || (tmp_atk_type>=101 && tmp_atk_type<=500 ) 
				||	tmp_atk_type ==1801 || (tmp_atk_type>=2101 && tmp_atk_type<=2500 ) ){
				lhurt_hp/=2;
				lhurt_mp/=2;
			}
		}else{
			lhurt_hp=0;
			lhurt_mp=0;
		}

	}
	
}

static void calc_2005 (  battle_info_t* abi,warrior_t* p_fighter, warrior_t* p_fightee , resolved_atk_t* p_resolved_atk   ,
	int32_t &lhurt_hp , int32_t &lhurt_mp 
		)
{
/*
	被技能2002（霜冻术）、2006（寒冰咒）、2010（幽怨冰）、7002（霜冻术）、7006（寒冰咒）、7010（幽怨冰）攻击，受到的伤害降低50%。
被技能2003（流火术）、2007（烈火咒）、2011（地狱火）、7003（流火术）、7007（烈火咒）、7011（地狱火）攻击，受到的伤害转为自身HP恢复。
被技能2004（旋风术）、2008（狂风咒）、2012（龙卷刃）、7004（旋风术）、7008（狂风咒）、7012（龙卷刃）攻击，受到的伤害降低75%。
 
  */	
	if (abi->is_pk_beast_2005() ){
		if(	p_fightee->type_id==TYPEID_2005  ){
			int tmp_atk_type=p_resolved_atk->atk_type;
			tmp_atk_type=tmp_atk_type%5000;//精灵>5000
			if ( 	tmp_atk_type== 2002 
				|| tmp_atk_type== 2006 
			  	|| tmp_atk_type== 2010
			  ){
				lhurt_hp*=0.5;
				lhurt_mp*=0.5;
			}else if (
 				tmp_atk_type== 2003 
				|| tmp_atk_type== 2007 
			  	|| tmp_atk_type== 2011
				){
				//lhurt_hp=100;
				lhurt_hp=-lhurt_hp;
			}else if( 
 				tmp_atk_type== 2004 
				|| tmp_atk_type== 2008 
				|| tmp_atk_type== 2012 
				) {
				lhurt_hp*=0.25;
				lhurt_mp*=0.25;
			}

			if (lhurt_hp>0 || lhurt_mp>0 ){
				p_fighter->p_waor_state->set_state_info(zhuoshang_bit,2,2 );
			}
			
		}
	}

}

static void calc_3017(  battle_info_t* abi, warrior_t* p_fightee , resolved_atk_t* p_resolved_atk   ,
	int32_t &lhurt_hp , int32_t &lhurt_mp 
		)
{
/*
风之龙：Grp=3042，Type=3017 特性“风之护佑”、“再动”。
注：仅当Boss属于Grp3042时，拥有该特性

	免疫技能2004（旋风术）、2008（狂风咒）、2012（龙卷刃）、7004（旋风术）、7008（狂风咒）、7012（龙卷刃）造成的伤害。风之护佑
	二次行动，都可使用技能，第二次行动速度为第一次的50%。再动
  */	
	if (abi->is_pk_beast_3017() &&  p_fightee->type_id==TYPEID_3017 ){
		int tmp_atk_type=p_resolved_atk->atk_type;
			tmp_atk_type=tmp_atk_type%5000;//精灵>5000
			if ( tmp_atk_type== 2004
			  || tmp_atk_type== 2008
			  || tmp_atk_type== 2012
			  ){
				lhurt_hp=0;
				lhurt_mp=0;
			}

	}
}

static void calc_2906(battle_info_t* abi, warrior_t* p_fightee, resolved_atk_t* p_resolved_atk, int32_t &lhurt_hp , int32_t &lhurt_mp)
{
	if (abi->is_pk_beast_2906() && p_fightee->type_id == TYPEID_2004) {
		if (lhurt_hp)
			lhurt_hp = 1;
		if (lhurt_mp)
			lhurt_mp = 1;
	}
}

static void calc_2013(battle_info_t* abi, warrior_t* p_fighter, warrior_t* p_fightee, resolved_atk_t* p_resolved_atk, int32_t &lhurt_hp , int32_t &lhurt_mp)
{
	if (abi->is_pk_beast_2013() && p_fighter->type_id == TYPEID_2013) {
		if ((lhurt_hp || lhurt_mp) && !(p_fightee->p_waor_state->state & abnormal_states) && rand() % 100 < 25) {
			p_fightee->p_waor_state->set_state_info(hunshui_bit,2, 100 );
		}
	}
}


static void calc_2007_before(warrior_t* p_fightee, resolved_atk_t* p_resolved_atk, int32_t &lhurt_hp , int32_t &lhurt_mp)
{
/*	
	 沃特尔王——超时间净化（Type2007特性“折射”、“缓冲”）：
		物攻技能101~500、魔攻技能2001~2500对其造成正常伤害，同时消耗其3%的MaxMP ；对其造成的伤害50%反作用到施放者身上。“折射”
	基础攻击10（普通攻击）与11（合击）对其造成的伤害降为正常伤害的25%；没有其它副作用。“缓冲”
	MP＜10%，失去“折射”、“缓冲”特性。
*
 */
	if ( p_fightee->type_id==2007/*  IS_BEAST_ID(p_fightee->userid)*/ && p_fightee->mp>=p_fightee->mp_max*0.1  &&  p_fightee->hp>0 ) 
	{
		int tmp_atk_type=p_resolved_atk->atk_type;
		tmp_atk_type=tmp_atk_type%5000;//精灵>5000

		if ( 	(tmp_atk_type>=101 && tmp_atk_type<=500   )
		 	||	(tmp_atk_type>=2001 && tmp_atk_type<=2500   )
			)
		{
			lhurt_mp+=p_fightee->mp_max*0.05 ;
		}

		if (tmp_atk_type==10 || tmp_atk_type==11 ){
			lhurt_hp/=4;
			if (lhurt_hp==0 ) lhurt_hp=1;
		}

	} 

	
}

static void calc_2007_end( warrior_t* p_fightee , warrior_t* p_fighter,resolved_atk_t* p_resolved_atk   ,
	int32_t &lhurt_hp , int32_t &lhurt_mp 
		)
{

	nest_atk_t* p_next_atk = &(p_resolved_atk->fanji[0]);
if (   p_fightee->type_id==2007 && p_fightee->mp>=p_fightee->mp_max*0.1 &&  p_fightee->hp>0 ) 
	{
		int tmp_atk_type=p_resolved_atk->atk_type;
		tmp_atk_type=tmp_atk_type%5000;//精灵>5000

		if ( 	(tmp_atk_type>=101 && tmp_atk_type<=500   )
		 	||	(tmp_atk_type>=2001 && tmp_atk_type<=2500   )
			)
		{
			if  ((tmp_atk_type>=2001 && tmp_atk_type<=2500 ) ){
				p_next_atk->fightee_state |= mofa_fantan;
			}else{
				p_next_atk->fightee_state |= gongji_fantan;
			}
			p_next_atk->fantan_hp=-lhurt_hp/2 ;
			//血量最少为1
			if (p_next_atk->fantan_hp+p_fighter->hp<1){
				p_next_atk->fantan_hp=-(p_fighter->hp-1);
			}
			p_fighter->hp+=p_next_atk->fantan_hp;
		}
	} 
}

static void set_last_atk(battle_info_t *abi,warrior_t* p,int idx,uint32_t atk_type,uint32_t atk_level) {
	warrior_t *lp = p;
	if(IS_BEAST_ID(p->userid)) return;
	if(p->petid) {
		lp = get_warrior_from_warriorlist(abi,p->userid,0);
		if(!lp || p->pet_state != rdy_fight) return;
		idx = 1;
	}

	lp->last_atk_type[idx]  = atk_type;
	lp->last_atk_level[idx] = atk_level;
}

//返回是否已经处理过了
static bool get_last_atk(battle_info_t *abi,warrior_t* p,int idx,uint32_t &atk_type,uint32_t &atk_level,uint32_t &atk_mark,int32_t &atk_pos ,uint32_t itemid,uint32_t add_hp) {
	warrior_t *lp = p;
	if(IS_BEAST_ID(p->userid)) return false;
	if(atk_type != skill_auto_attack) return false ;
	if(p->petid) {
		lp = get_warrior_from_warriorlist(abi,p->userid,0);
		if(!lp) return false;
		idx = 1;
	}

	if(lp->auto_atk_rounds) {
		atk_pos = -1;
		atk_mark = 1;
		atk_level = 1;
		atk_type = lp->last_atk_type[idx];

		if(!idx && !p->petid) lp->auto_atk_rounds--;
		if(p->is_pet())	{
			if( atk_type <5000 
					|| atk_type==5010
					||  atk_type==5008
					||  atk_type==5701
			  )
			atk_type=0;
		}

		/*
		skill_attr_t* ska = get_skill_attr(atk_type, p->weapon_type);
		if(ska) {
			switch(ska->skill_type) {
				case xixue_gongji:
				case physi_attack:
				case mp_attack:
				case magic_attack:
				case fh_magic:
				case hp_resume:					
					skill_info_t skill_info;
					atk_level = lp->last_atk_level[idx];
					skill_info.skill_id=atk_type;
					skill_info.skill_level= atk_level ;
					if(p->check_mp_enough(&skill_info)){
						//找到技能了
						return false;
					}
					//否则进入后台计算技能
				default:
					break;
			}
		}
		*/

		//没有设置时处理
		if (p->is_user()){
			  auto_skill_info_t skill_item;	
			  if (idx==0){
				skill_item=get_auto_pk_atk_for_user(p, itemid>0 );
				if ( skill_item.use_medical){//使用药品
					bt_use_chemical_in msg_in;
					msg_in.enemy_mark=0;
					msg_in.enemy_pos=skill_item.pos;
					msg_in.petid=0;
					msg_in.itemid=itemid;
					msg_in.add_hp=add_hp;
					bt_use_chemical(p->userid,0 , abi, &msg_in );
					//表示已经处理过了
					return true;
				}else{
					atk_type= skill_item.skill_id;
					atk_level= skill_item.skill_level;
					atk_pos = skill_item.pos;
					atk_mark = skill_item.mak;
				}
			  }else{
				atk_type=10;
				atk_level=1;
			  }
		}else{
			  skill_id_level_t skill_item=get_auto_pk_atk_for_pet(p);
			  atk_type= skill_item.skill_id;
			  atk_level= skill_item.skill_level;
			  KDEBUG_LOG(p->userid,"pet auto atk:%u",atk_type);
		}
	//	KDEBUG_LOG(p->userid,"33333333333 petid=%u,get last atk[%d]=(%d,%d)",p->petid,idx,atk_type,atk_level);
	}
	return false;
}

void do_magic_attack( battle_info_t* abi, resolved_atk_t* p_resolved_atk, warrior_t* p_fighter, warrior_t* p_fightee, atk_info_t* p_atk_info  ,nest_atk_t* p_next_atk, skill_attr_t* p_skill_attr ,int32_t *p_lhurt_hp )
{
	
	clean_one_state(p_fightee,hunshui_bit,10);
	*p_lhurt_hp = magic_fight(abi, p_fighter, p_fightee, p_next_atk, p_skill_attr);
	
	DEBUG_LOG("do magic attack : hurthp=%u",*p_lhurt_hp);
	SET_WARRIOR_STATE(p_next_atk->fightee_state, attacked_bit);
	
	set_last_atk(abi,p_fighter,p_atk_info->atk_seq,p_atk_info->atk_type,p_atk_info->atk_level);

	if (*p_lhurt_hp >0 ){//有打中
		switch ( p_next_atk->atk_type ){
			case skill_ma_pet_wanshoushishen :
				SET_WARRIOR_STATE(p_next_atk->fightee_state, yeshou_add_hert_hp_bit);
				p_fightee->p_waor_state->set_state_info(yeshou_add_hert_hp_bit,2,100);
				p_fightee->p_waor_state->set_yeshou_hurt_hp_percent(2+p_atk_info->atk_level);
				break;
			case skill_yandiezhiwu  ://zhuoshang_bit
			case skill_jinglengfengsha://dongshang_bit
			case skill_dadiciqiang://shufu_bit
			case skill_lunzhuanfengbao://xuanyun_bit
			case skill_anyunliejie://tuoli_bit 
			{
				uint8_t state_list[]={ zhuoshang_bit,dongshang_bit,shufu_bit,xuanyun_bit,tuoli_bit };
				uint8_t rate_base[]={ 12,12,11,11,10};
				uint8_t state_bit=state_list[ p_next_atk->atk_type -skill_yandiezhiwu ];
				uint8_t rate=rate_base[p_next_atk->atk_type -skill_yandiezhiwu]+p_next_atk->atk_level*3 ;
				//uint8_t rate=100;//rate_base[p_next_atk->atk_type -skill_yandiezhiwu]+p_next_atk->atk_level*3 ;
				DEBUG_LOG("heimodaoshi skillid=%u hurt=%u rate=%u",p_next_atk->atk_type,*p_lhurt_hp,rate);
				if ( p_fightee->p_waor_state->set_some_state_info(state_bit,rate,2,3 )){
					DEBUG_LOG("XXX set state : %u",state_bit);
					SET_WARRIOR_STATE(p_next_atk->fightee_state, state_bit );
				}
				break;
			}
			default :
				break;
		}
	}

}

//攻击的核心计算
static void calc_main_attack(battle_info_t* abi, resolved_atk_t* p_resolved_atk, warrior_t* p_fighter, warrior_t** pp_fightee, atk_info_t* p_atk_info) 
{ 
   	warrior_t* p_fightee = *pp_fightee; 
	nest_atk_t* p_next_atk = &(p_resolved_atk->fanji[0]);
	p_next_atk->uid= p_resolved_atk->fighter_id;
	p_next_atk->petid= p_resolved_atk->petid;
	p_next_atk->atk_type = p_resolved_atk->atk_type;
	p_next_atk->atk_level = p_resolved_atk->atk_level;
	p_next_atk->atk_uid = p_resolved_atk->atk_uid;
	p_next_atk->atk_petid= p_resolved_atk->atk_petid;


	p_resolved_atk->atk_cnt = 1;
	
	int32_t lhurt_hp = 0, lhurt_mp = 0;
	int16_t bisha_h = 0;

	switch(p_resolved_atk->atk_type){
		case skill_run_away:
		case skill_pet_run_away:
		case skill_pet_break_off:
		case skill_user_break_off:
			proc_run_away(abi, p_fighter, p_next_atk);
			return;
	}
	
	int hitresult = 0;
	uint8_t huwei_happen = 0;
	// first modi p_fighter's personal property
	change_attr_before_attack(p_fighter, p_resolved_atk, abi);

	skill_attr_t* p_skill_attr = get_skill_attr(p_resolved_atk->atk_type, p_fighter->weapon_type);
	if (!p_skill_attr){
		KERROR_LOG(p_fighter->userid,"skill invalid\t[%u %u %u]", p_resolved_atk->fighter_id, p_resolved_atk->atk_uid, p_resolved_atk->atk_type);
		return;
	}
	uint32_t atk_level=p_next_atk->atk_level;
	
	KDEBUG_LOG(p_fighter->userid,"calc_main_attack:atk_type=%u skill_type=%u atk_level=%u",
			p_resolved_atk->atk_type,p_skill_attr->skill_type,atk_level);
	switch (p_skill_attr->skill_type) {
		case xixue_gongji:
		case physi_attack:
			hitresult = calc_hit_result(abi, p_fighter, p_fightee, p_next_atk);
			if (hitresult && replace_fightee_by_huwei(p_skill_attr, pp_fightee, p_next_atk)){
				p_fightee = *pp_fightee;
				huwei_happen = 1;
			}
			lhurt_hp = physiq_fight(abi, p_fighter, p_fightee, p_next_atk, hitresult, &bisha_h, huwei_happen);
			if (lhurt_hp != 0){//有伤害
				float f_lhurt_hp = lhurt_hp * p_resolved_atk->hurthp_rate / 100.0;
				if (huwei_happen && IS_BEAST(p_fightee)){
					f_lhurt_hp *= (1.66 - 0.08 * p_fightee->p_waor_state->get_huwei_level());
				}
				if (f_lhurt_hp >= 0 && f_lhurt_hp <= 1.0){
					lhurt_hp = 1;
				}else if (f_lhurt_hp <= 0 && f_lhurt_hp >= -1.0){
					lhurt_hp = -1;
				}else{
					lhurt_hp = f_lhurt_hp;
				}
			}
			
			if (!CHK_WARRIOR_STATE(p_next_atk->fightee_state, huibi_bit)) {
				SET_WARRIOR_STATE(p_next_atk->fightee_state, attacked_bit);
			}
			if ((p_resolved_atk->atk_type == skill_pa_xianfazhiren 
						|| p_resolved_atk->atk_type == skill_pa_pet_xianfazhiren) 
				&& p_fighter->check_state( pause_bit)) {
				SET_WARRIOR_STATE(p_next_atk->fightee_state, pause_bit);

			} 

			if (lhurt_hp > 0 ){//设置状态位
				uint8_t need_set_state_bit=0;
				uint8_t idx=0;
				uint32_t rate=0;
				uint16_t anti_value=0;
				if ( p_resolved_atk->atk_type ==skill_zaiezhizhao ){
					uint8_t rand_bit_list[]={ zhongdu_bit,zuzhou_bit};
					idx=rand()%2;
					rate=(10+atk_level*5)*(100-anti_value)/100;
					anti_value=idx?p_fightee->r_zuzhou:p_fightee->r_zhongdu;
					KDEBUG_LOG(p_fighter->userid,"skill_zaiezhizhao hurt=%u idx=%u anti_valude=%u",lhurt_hp,idx,anti_value);
					need_set_state_bit=p_fightee->p_waor_state->set_some_state_info(
							rand_bit_list[idx],rate, 2 ,100);
				}else if ( p_resolved_atk->atk_type ==skill_hundunzhiren){
					uint8_t rand_bit_list[]={ hunluan_bit,yiwang_bit };
					idx=rand()%2;
					rate=(12+atk_level*3)*(100-anti_value)/100;
					anti_value=idx?p_fightee->r_hunluan:p_fightee->r_yiwang;
					KDEBUG_LOG(p_fighter->userid,"skill_hundunzhiren hurt=%u idx=%u anti_valude=%u",lhurt_hp,idx,anti_value);
					need_set_state_bit=p_fightee->p_waor_state->set_some_state_info(
							rand_bit_list[idx] , rate, 2 ,100);
				}else if ( p_resolved_atk->atk_type ==skill_shenpanzhilun){
					uint8_t rand_bit_list[]={zhongdu_bit,zuzhou_bit,hunluan_bit,yiwang_bit };
					idx=rand()%4;
					anti_value=0;
					switch ( idx ){
						case 0:
							anti_value=p_fightee->r_zhongdu;
							break;
						case 1 :
							anti_value=p_fightee->r_zuzhou;
							break;
						case 2:
							anti_value=p_fightee->r_hunluan;
							break;
						case 3 :
							anti_value=p_fightee->r_yiwang;
							break;
						default :
							break;
					}
					rate=(15+(atk_level-1)/2*5)*(100-anti_value)/100;
					need_set_state_bit=p_fightee->p_waor_state->set_some_state_info(rand_bit_list[idx] , 
							rate, 2+(atk_level-1)%2 ,100);
					KDEBUG_LOG(p_fighter->userid,"skill_shenpanzhimen hurt=%u idx=%u anti_valude=%u rate=%u",
							lhurt_hp,idx,anti_value,rate);

				}else if ( p_resolved_atk->atk_type ==skill_jintouquan ){
					if(!p_fightee->is_in_fangyu() && !p_fightee->check_state(huandun_bit)){//不在防御中 转成扣魔
						lhurt_hp=0;
						lhurt_mp=p_fightee->mp*(0.03*atk_level) ;
					}
					KDEBUG_LOG(p_fighter->userid,"jintouquan hurt_hp=%u hurt_mp=%u lv=%u mp=%u",lhurt_hp,lhurt_mp,atk_level,p_fightee->mp);
				}
				if (need_set_state_bit){
					DEBUG_LOG("XXX SET state :%u ",need_set_state_bit );
					SET_WARRIOR_STATE(p_next_atk->fightee_state, need_set_state_bit );
				}
			}
			
			set_last_atk(abi,p_fighter,p_atk_info->atk_seq,p_atk_info->atk_type,p_atk_info->atk_level);
			clean_one_state(p_fightee,hunshui_bit,10);
			// meiying_bit
			break;
		case mp_attack:// 破魂 
			clean_one_state(p_fightee,hunshui_bit,10);
			hitresult = calc_hit_result(abi, p_fighter, p_fightee, p_next_atk);
			if (hitresult && replace_fightee_by_huwei(p_skill_attr, pp_fightee, p_next_atk)){
				p_fightee = *pp_fightee;
				huwei_happen = !!IS_BEAST(p_fightee);
			}
			
		    lhurt_mp = mp_fight(p_fighter, p_fightee, p_resolved_atk, hitresult, p_next_atk, huwei_happen);
			if (!CHK_WARRIOR_STATE(p_next_atk->fightee_state, huibi_bit)) {
				SET_WARRIOR_STATE(p_next_atk->fightee_state, attacked_bit);
			}
			set_last_atk(abi,p_fighter,p_atk_info->atk_seq,p_atk_info->atk_type,p_atk_info->atk_level);
			break;
		case defense:
			p_resolved_atk->atk_cnt = 0;
			return;
		case hp_resume: // skill_rh_mingsi
		    lhurt_hp = magic_mend_skill(abi, p_fighter, p_fightee, p_resolved_atk, p_next_atk, &lhurt_mp );
			set_last_atk(abi,p_fighter,p_atk_info->atk_seq,p_atk_info->atk_type,p_atk_info->atk_level);
			break;
		case fh_magic: //  吸血魔法
		case magic_attack:
			do_magic_attack(abi, p_resolved_atk, p_fighter, p_fightee, p_atk_info, p_next_atk,  p_skill_attr ,&lhurt_hp);
			break;
		case abnormal_attack:
			//清理昏睡 受到攻击立即苏醒
			//clean_one_state(p_fightee,hunshui_bit,10);
			change_enemy_state(abi, p_fighter, p_fightee, p_resolved_atk,p_next_atk  );
			SET_WARRIOR_STATE(p_next_atk->fightee_state, attacked_bit);
			break;
		case set_fanyyu_state:
			magic_assist_skill(abi, p_fighter, p_fightee, p_resolved_atk, p_next_atk);
			break;
		case use_medical:
			if(IS_REVIVING_MEDICAL(p_fighter->p_waor_state->get_use_item_itemid())
					&& p_fightee->check_state(dead_bit)){
				/*2012-3-29 如果是使用复活药则重置复活位*/
				set_warrior_alive_opt(p_fightee);
				SET_WARRIOR_STATE(p_next_atk->fightee_state, revive_bit);
				reset_warrior_stop_attack(abi, p_fightee);
			}
			lhurt_hp = proc_use_medical(abi, p_fighter, p_fightee, p_next_atk);
			break;
		case change_position:
			change_use_position(abi, p_fighter);
			return;
		case catch_pet_opt:
			proc_catch_pet(abi, p_fighter, p_fightee, p_next_atk);
			return;
		case recall_pet:
			if  (p_resolved_atk->atk_type==skill_callback_fuzhu) {
				proc_callback_fuzhu(abi, p_fighter, p_fightee, p_resolved_atk, p_next_atk);
			}else{
				proc_recall_pet(abi, p_fighter, p_fightee, p_resolved_atk, p_next_atk);
			}
			return;
		case daiji:
			return;
	}

	//野兽加成  wanshoushishen
	if (p_fighter->race==race_yeshou 
			&&  p_fightee->p_waor_state->check_state( yeshou_add_hert_hp_bit )  ){
		lhurt_hp*=1+p_fightee->p_waor_state->get_yeshou_hurt_hp_percent()/100.0;		
	}

	//难度系数,修正,对怪
	if (IS_BEAST_ID(p_fighter->userid)) {
		if (lhurt_hp>0 ){
			lhurt_hp*=abi->difficulty;
			if (lhurt_hp==0) lhurt_hp=1;
		}

		if (lhurt_mp>0 ){
			lhurt_mp*=abi->difficulty;
			if (lhurt_mp==0) lhurt_mp=1;
		}
	}
	//2004修正
	calc_2004(abi, p_fightee, p_resolved_atk   , lhurt_hp, lhurt_mp);
	calc_2005(abi, p_fighter, p_fightee,p_resolved_atk   , lhurt_hp, lhurt_mp);
 	calc_3017(abi, p_fightee, p_resolved_atk   , lhurt_hp, lhurt_mp);
	calc_2906(abi, p_fightee, p_resolved_atk, lhurt_hp, lhurt_mp);

	if((p_resolved_atk->atk_type % 5000) == skill_shunsha) {
		if(!IS_BOSS_ID(p_fightee->type_id) && rand() % 100 < 4 * p_resolved_atk->atk_level)
			lhurt_hp = p_fightee->hp;
	}
	//石化 该状态下受到攻击造成的伤害减半
	if(p_fightee->p_waor_state->check_state(shihua_bit)) {
		lhurt_hp /= 2;
	}
		
	calc_2013(abi, p_fighter, p_fightee, p_resolved_atk, lhurt_hp, lhurt_mp);

	//沃特尔王
	calc_2007_before( p_fightee , p_resolved_atk   , lhurt_hp, lhurt_mp);
	// we need to rollback p_fighter's personal property after attack action
	change_attr_after_attack(p_fighter, p_resolved_atk, abi);

	//处理绝对壁障
	if (lhurt_hp>0  && p_fightee->check_state(jueduibizhang_bit ) ){
		//得到防护值
		int32_t fanghuzhao_level=p_fightee->p_waor_state->get_fanghuzhao_value();
		DEBUG_LOG("fanghuzhao hurt=%d fanghu=%u",lhurt_hp,fanghuzhao_level);
		if( fanghuzhao_level){//如果还可以防护 则设置相应位； 不能防护取消相应位
			p_fightee->p_waor_state->set_state_info(fanghuzhao_bit,1,10);
		}else{
			p_fightee->p_waor_state->unset_state_info(fanghuzhao_bit);
		}
		if (fanghuzhao_level >lhurt_hp){
			lhurt_hp=0;
			p_fightee->p_waor_state->set_fanghuzhao_value(fanghuzhao_level -lhurt_hp );
		}else{
			lhurt_hp=lhurt_hp-fanghuzhao_level ;
			p_fightee->p_waor_state->set_fanghuzhao_value(0);
		}
	}
	//扣血扣魔 并检查玩家是否已经死亡	
	effect_fighter_fightee_hpmp(abi, lhurt_hp, lhurt_mp, p_fighter, p_fightee, p_skill_attr, p_resolved_atk, 0, bisha_h, p_atk_info); // first fanji_seq
	//沃特尔王
	calc_2007_end( p_fightee , p_fighter, p_resolved_atk   , lhurt_hp, lhurt_mp);

	//  回击 
	if (check_warrior_can_huiji(p_fightee, p_skill_attr, p_resolved_atk)){
		DEBUG_LOG( "check_warrior_can_huiji in");
		if ((skill_pa_zhuiji == p_resolved_atk->atk_type || skill_pa_pet_zhuiji== p_resolved_atk->atk_type) 
				&& !NONEED_ATTACK(p_fighter) && !CANNOT_ATTACK(p_fightee) 
				&& p_resolved_atk->atk_cnt < MAX_FANJI_CNT) {
			SET_WARRIOR_STATE(p_fighter->p_waor_state->one_loop_state,one_loop_bit_huiji);
		}

		while (!NONEED_ATTACK(p_fighter) && !CANNOT_ATTACK(p_fightee) && p_resolved_atk->atk_cnt < MAX_FANJI_CNT) {
			warrior_t* pwtmp = p_fightee;
			p_fightee = p_fighter;
			p_fighter = pwtmp;
			calc_huiji_attack(abi, p_resolved_atk, p_fighter, p_fightee, p_atk_info);
			if (p_atk_info->atk_step != 0){
				break;
			}
			if (!NONEED_ATTACK(p_fighter) && !CANNOT_ATTACK(p_fightee) && p_resolved_atk->atk_cnt < MAX_FANJI_CNT){
				int32_t fanji_rate = p_fightee->fight_back;
				fanji_rate++;
		
				fanji_rate = (fanji_rate > 100)?100:fanji_rate;
				if ((rand() % 100) > fanji_rate){
					break;
				}

				pwtmp = p_fightee;
				p_fightee = p_fighter;
				p_fighter = pwtmp;
				calc_fanji_attack(abi, p_resolved_atk, p_fighter, p_fightee, p_atk_info);
			}
			else{
				break;
			}
		}
		return;
	}
	
	// fanji
	//DEBUG_LOG( "check_warrior_can_fanji cd ack_type=%u ",p_resolved_atk->atk_type );
	if (check_warrior_can_fanji(abi, p_fightee, p_skill_attr, p_resolved_atk)){
		DEBUG_LOG( "check_warrior_can_fanji in");
		while (!NONEED_ATTACK(p_fighter) && !CANNOT_ATTACK(p_fightee) && p_resolved_atk->atk_cnt < MAX_FANJI_CNT) {
			int32_t fanji_rate = p_fightee->fight_back;
			fanji_rate++;
			bool next_atk_fanji_bit_opt_flag=
				p_fightee ->p_waor_state->check_state(next_atk_fanji_bit);
 			if (next_atk_fanji_bit_opt_flag){
				DEBUG_LOG("XXXnext_atk_fanji_bit is 0k  ");
				fanji_rate=100;
			}
			fanji_rate = (fanji_rate > 100)?100:fanji_rate;
			KDEBUG_LOG(p_fightee->userid, "MAY FANJI\t[%d]", fanji_rate);
			if ((rand() % 100) > fanji_rate){
				break;
			}
			warrior_t* pwtmp = p_fightee;
			p_fightee = p_fighter;
			p_fighter = pwtmp;

			calc_fanji_attack(abi, p_resolved_atk, p_fighter, p_fightee, p_atk_info);
			if (next_atk_fanji_bit_opt_flag){
				DEBUG_LOG("XXXX RESET_WARRIOR_STATE(p_next_atk->fightee_state, next_atk_fanji_bit )  ");
				RESET_WARRIOR_STATE(p_next_atk->fightee_state, next_atk_fanji_bit );
				//清空状态
				clean_one_state(p_fighter, next_atk_fanji_bit,10);
			}

		}
		return;
	}
}

static int attack_indx(battle_info_t* abi, warrior_t* p)
{
	if(!abi->is_pets_pk_mode()) {
		if (p->petid ) {
			warrior_t* lp_ren = get_warrior_from_warriorlist(abi, p->userid, 0);
			//xxx
			if ((lp_ren->check_state( attacked_out_bit) || !lp_ren->check_state( dead_bit))
					&& !lp_ren->atk_info[0].atk_type){
				ERROR_LOG("WAI GUA: PET CANNT SEND SKILL FASTER THAN REN\t[batid=%lu uid=%u petid=%u]", abi->batid, p->userid, p->petid);
				return -1;
			}
		}
	} else {
		if (p->is_pet()) {
			warrior_t* lp = get_warrior_from_warriorlist(abi, p->userid, 0);
			lp->set_attack_dumy(true);
		}
	}

	//第一招不存在
	if (!p->atk_info[0].atk_type) {
		return 0;
	}
	
	//处理第二招
	if (IS_BEAST(p)) {//
		KERROR_LOG(p->userid,"is not beast: beast petid=%u",p->petid);
		return -1;
	}

	// avoid remote send three or more attack skill.
	// one can send skill 2 at most
	// repeat insert p->atk_info[1] into speed will cause dead loop.
	if (p->atk_info[1].atk_type){
		ERROR_LOG("WAI GUA: FA ZHAO CI SHU CHAO GUO XIANZHI\t[batid=%lu uid=%u petid=%u]", abi->batid, p->userid, p->petid);
		return -1;
	}

	//是宠物 
	if (p->petid){
		return 1;
	}
	
	//得到宠物
	warrior_t* lp = get_pet_from_warriorlist(abi, p->userid);
	//DEBUG_LOG("check pet %u %u ",lp!=NULL,RUN_OR_OUT(lp));
	if (!lp || RUN_OR_OUT(lp)) {
		//没有宠物时,为人第二招
		return 1;
	}

	return -1;
}

static void set_atk_info(atk_info_t* p_atk_info, int atk_idx, uint32_t enemy_mark, int32_t enemy_pos, uint32_t atk_type, uint32_t atk_level)
{
	p_atk_info->atk_seq = atk_idx;
	p_atk_info->atk_mark = enemy_mark;
	p_atk_info->atk_pos  = enemy_pos;
	p_atk_info->atk_type = atk_type;
	p_atk_info->atk_level = atk_level;
}

//检查用户被打飞后的处理
static void check_atack_out_action(battle_info_t* abi, warrior_t* lp)
{
	if (!lp || lp->petid || IS_BEAST_ID(lp->userid) || !lp->p_waor_state->check_state(attacked_out_bit)){
		return;
	}
	
	//warrior_t* lp_pet = get_pet_from_warriorlist(abi, lp->userid);
	list_head_t* lh;
	list_for_each(lh, &abi->warrior_list) {
		warrior_t* lp_pet = list_entry(lh, warrior_t, waor_list);
		if (lp_pet->userid == lp->userid && lp_pet->petid && (lp_pet->pet_state==on_fuzhu || lp_pet->pet_state == rdy_fight)){
			DEBUG_LOG("lllllp %u %u %u",lp_pet->userid,lp_pet->petid,lp_pet->p_waor_state->check_state( attacked_out_bit));
			if (!lp_pet || lp_pet->p_waor_state->check_state( attacked_out_bit))
				return;

			int idx = !lp_pet->atk_info[0].atk_type ? 0 : (!lp_pet->atk_info[1].atk_type ? 1 : -1);
			if (idx == -1){
				ERROR_LOG("Pet Already snd 2 Skill\t[batid=%lu uid=%u petid=%u]", abi->batid, lp_pet->userid, lp_pet->petid);
				return;
			}

			//设置自己的宠物逃跑
			set_atk_info(&(lp_pet->atk_info[idx]), idx, 0, 0, skill_pet_break_off, 1);
			lp_pet->atk_info[idx].wrior = lp_pet;

			if (chk_repeat_atkinfo_insert(abi, &(lp_pet->atk_info[idx])) == 0){
				list_add_tail(&(lp_pet->atk_info[idx].atk_info_list), &abi->speed_list);
			}
		}
	}
	
}

void calc_warrior_single_step_battle_result(battle_info_t* abi, resolved_atk_t* p_resolved_atk, atk_info_t* p_atk_info)
{
	uint8_t old_er = abi->challger.atk_out;
	uint8_t old_ee = abi->challgee.atk_out;
	warrior_t* p_fighter = get_warrior_from_warriorlist(abi, p_resolved_atk->fighter_id, p_resolved_atk->petid);
	warrior_t* p_fightee = get_warrior_from_warriorlist(abi, p_resolved_atk->atk_uid, p_resolved_atk->atk_petid);

	KDEBUG_LOG((p_fighter?p_fighter->userid:0),"技能作用一个人(%u %u)开始calc_warrior_single_step_battle_result {",
			(p_fightee?p_fightee->userid:0),(p_fightee?p_fightee->petid:0));
	////DEBUG_LOG("skill=%u fighter=%u fightee=%u",
		//	p_atk_info->atk_type,p_fighter->userid,p_fightee->userid);
	if (p_atk_info && p_atk_info->confrm_decre_mp == confrm_decre_mp_no_need)
		p_atk_info->confrm_decre_mp = confrm_decre_mp_after_atk;

	calc_main_attack(abi, p_resolved_atk, p_fighter, &p_fightee, p_atk_info);

	if (abi->challgee.alive_count > 0 && abi->challger.alive_count > 0 
		&& old_er != abi->challger.atk_out && p_resolved_atk->atk_type != skill_recall_pet){
		if (p_fightee && p_fightee->self_team == &abi->challger){
			check_atack_out_action(abi, p_fightee);
		}

		if (p_fighter != p_fightee && p_fighter->self_team == &abi->challger){
			check_atack_out_action(abi, p_fighter);
		}
	}

	if (abi->challgee.alive_count > 0 && abi->challger.alive_count > 0 
		&& old_ee != abi->challgee.atk_out && p_resolved_atk->atk_type != skill_recall_pet){
		if (p_fightee && p_fightee->self_team == &abi->challgee){
			check_atack_out_action(abi, p_fightee);
		}

		if (p_fighter != p_fightee && p_fighter->self_team == &abi->challgee){
			check_atack_out_action(abi, p_fighter);
		}
	}

	if (p_fightee && CHK_WARRIOR_STATE(p_fightee->p_waor_state->one_loop_state, one_loop_bit_huwei)){
		RESET_WARRIOR_STATE(p_fightee->p_waor_state->one_loop_state, one_loop_bit_huwei);
	}

	DEBUG_LOG("}技能作用一个人结束");
}
//end_mark :&0x01 ：1队输了 &0x02:2队输了
static void battle_end_for_npc(battle_info_t* abi, uint32_t* end_mark)
{
	*end_mark = 0;//对战还没有结束
	if (!((abi->challgee.team_type | abi->challger.team_type) & team_npc_friend)){
		//没有npc
		return;
	}
	
	list_head_t* next;
	list_head_t* cur;
	uint8_t aliver_cnt = 0;
	uint8_t deader_cnt = 0;
	uint8_t alivee_cnt = 0;
	uint8_t deadee_cnt = 0;
	
	list_for_each_safe(cur, next, &abi->warrior_list){
		warrior_t* p = list_entry(cur, warrior_t, waor_list);
		if (IS_BEAST(p)){
			continue;
		}

		if (RUN_OR_OUT(p) || WARRIOR_OFFLINE(p)){
			continue;
		}
		
		if (WARRIOR_DEAD(p)){
			if (p->self_team == &abi->challger){
				deader_cnt++;
			}else{
				deadee_cnt++;
			}
			continue;
		}

		if (p->self_team == &abi->challger){
			aliver_cnt++;
		}else{
			alivee_cnt++;
		}
	}
	
	if (abi->challger.team_type & team_npc_friend){
		// 存在活物，但是所有人都死掉，则活着的肯定是npc
		if (abi->challger.ren_alive_count > 0 && aliver_cnt == 0){
			if (deader_cnt==0){ //1队全死 
				(*end_mark) = 1;//1队有npc
			}
		}
	}

	if (abi->challgee.team_type & team_npc_friend){
		// 存在活物，但是所有人都死掉，则活着的肯定是npc
		if (abi->challgee.ren_alive_count > 0 && alivee_cnt == 0){
			if (!deadee_cnt){//2队全死 
				(*end_mark) = ((*end_mark) == 0)?2:3;
			}
		}
	}
}
//
static int battle_end(battle_info_t* abi)
{
	if(abi->is_pets_pk_mode()) {
		if(	!(abi->challgee.team_type & team_beast) && (abi->challgee.alive_count == abi->challgee.ren_alive_count 
			|| abi->challger.alive_count == abi->challger.ren_alive_count
			|| abi->challgee.ren_alive_count==0 || abi->challger.ren_alive_count==0) ){
			if(	abi->challgee.alive_count == abi->challgee.ren_alive_count || abi->challgee.ren_alive_count==0 ){
				abi->challgee.victory = fail;
				abi->challger.victory = victory;
			} else {
				abi->challger.victory = fail;
				abi->challgee.victory = victory;
			}
			return 1;
		} else if ((abi->challgee.team_type & team_beast) && (abi->challgee.alive_count == 0 \
			|| abi->challger.ren_alive_count == 0 || abi->challger.alive_count == abi->challger.ren_alive_count)) {
			if(	abi->challgee.alive_count == 0 ){
				abi->challgee.victory = fail;
				abi->challger.victory = victory;
			} else {
				abi->challger.victory = fail;
				abi->challgee.victory = victory;
			}
			return 1;
		}

		return 0;
	}
//xxx
	DEBUG_LOG("battle_end:%u %u",abi->challgee.alive_count,abi->challger.alive_count);
	/*2012-3-22:战斗结束条件改为人和宠物都死才结束 判断条件由ren_alive_count->alive_count*/
	if (abi->challgee.alive_count > 0 && abi->challger.alive_count > 0){
		//1,2队的人都死了,看看npc 情况
		if (abi->battle_loop < MAX_BATTLE_LOOP){
			uint32_t end_mark = 0;
			battle_end_for_npc(abi, &end_mark);
			switch(end_mark){
				case 0:
					return 0;//还没有结束,继续下一轮
				case 1:
					abi->challger.victory = fail;
					abi->challgee.victory = victory;
					return 1;
				case 2:
					abi->challger.victory = victory;
					abi->challgee.victory = fail;
					return 1;
				default://双方都有npc
					abi->challger.victory = fail;
					abi->challgee.victory = fail;
					return 1;
			}
		}else{
			abi->challger.victory = fail;
			abi->challgee.victory = fail;
		}
	}else if (abi->challger.alive_count > 0){
		abi->challger.victory = victory;
		abi->challgee.victory = fail;
	}else if (abi->challgee.alive_count > 0){
		abi->challger.victory = fail;
		abi->challgee.victory = victory;
	}else{
		abi->challger.victory = fail;
		abi->challgee.victory = fail;
	}
	return 1;
}

void calc_team_ave_lv(battle_users_t* abu)
{
	float ave_lv = 0;
	int cnt = 0;
	for (int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* lp = abu->players_pos[loop];
		if (lp) {
			ave_lv += lp->level;
			cnt++;
		}
	}
	
	abu->ave_lvl = ave_lv / (cnt ? cnt : 1);
}

//经验的等级加成
float g_level_add_exp[]={
5,5,5,5,5,5,5,4.5,4.5,4.5,4.5,4.5,4.5,4.5,4.5,4.5,4.5,4.5,4,4,4,4,4,4,4,4,4,4,3.5,3.5,3.5,3.5,3.5,3.5,3.5,3.5,3.5,3.5,3,3,3,3,3,3,3,3,3,3,2.5,2.5,2.5,2.5,2.5,2.5,2.5,2.5,2.5,2.5,2,2,2,2,2,2,2,2,2,2,1.8,1.8,1.8,1.8,1.8,1.8,1.8,1.8,1.8,1.8,1.6,1.6,1.6,1.6,1.6,1.6,1.6,1.6,1.6,1.6,1.6,1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5
};

//float g_group_change_exp[]={1, 1.4,1.6,1.8,2.0};



//计算经验值;  
//std::map<userid_t,user_exp_level_t>  &user_exp_level_map :保存用户的经验.等级信息,在计算宠物经验时使用
static int calc_experience(battle_info_t* abi, warrior_t* p,  std::map<userid_t,user_exp_level_t>  &user_exp_level_map ,bool is_leader )
{
	if (!IS_BEAST_GRP_ID(abi->challgee.teamid) || (abi->challger.victory != victory)){
		return 0;
	}
	
	if (p->p_waor_state->check_state( dead_bit)
		|| p->p_waor_state->check_state( attacked_out_bit)
		|| p->p_waor_state->check_state( run_away_bit)) {
		return 0;
	}

	float ave_lv = p->enemy_team->ave_lvl;
	int waor_lvl = p->level;
	
	if (waor_lvl<=0 || waor_lvl>100  ){
		waor_lvl=1;
	}
	//基础经验
	float beast_out_exp = p->enemy_team->ren_count * pow(ave_lv, 8.0/5);
	float harvest_exp = 1;//最终得到经验
	float sub_lv = waor_lvl - ave_lv;
	
	if (p->petid==0){//是人
		if (sub_lv > 5.0 && sub_lv < 25.0){
			harvest_exp = (beast_out_exp * (25.0 - waor_lvl + ave_lv) / 30.0);
		} else if (abs(sub_lv) <= 5.0) {
			harvest_exp = beast_out_exp;
		} else if (abs(sub_lv) >= 25.0) {
			harvest_exp = 1;
		} else{
			harvest_exp = (beast_out_exp *  (25.0 + waor_lvl - ave_lv) / 30.0);
		}
	}else{//宠物
		//（参与战斗并在战斗结束后结算经验的）宠物>=角色5级，战斗结算时该宠物可获得最大经验为：0。并在“战斗成果”面板所框位置增加（获得经验受限）图标，附带Tips“精灵高于主人5级时无法获得战斗经验”。
		//（参与战斗并在战斗结束后结算经验的）宠物小于角色5级，战斗结算时该宠物可获得最大经验为：Min[Exp战斗结算，(Lv人+5)4-Lv宠4]。
		
		int user_level = user_exp_level_map[p->userid].level;
		if (int(p->level) - user_level >=5 ){
			 return -1;
		}else{
			int level_exp_value= int(pow( user_level+5,4))-int(pow (p->level,4));
			harvest_exp= level_exp_value< beast_out_exp?level_exp_value:beast_out_exp;
		}
	}

	if ( harvest_exp < 1.0 && harvest_exp>0){
		harvest_exp = 1.0;
	}

	//组队加成
	//harvest_exp=harvest_exp*g_group_change_exp[ p->self_team->ren_count-1]/p->self_team->ren_count;

	//等级加成
	harvest_exp*=g_level_add_exp[waor_lvl-1];

	//队长
	if (is_leader ){
		harvest_exp*=1.2;
	}
	return harvest_exp;
}

static int pkg_body_clothes_ex(int count, const body_cloth_t* p, uint8_t *buf)
{
	int j = 0;
	for (int i = 0; i < MAX_ITEMS_WITH_BODY; i++) {
		PKG_H_UINT32 (buf, p[i].equip_type, j);
		PKG_H_UINT16 (buf, p[i].durable_val, j);
	}

	return j;
}
void  set_user_protect_exp_map( battle_info_t* abi, battle_users_t* abu , 
	std::map<userid_t,user_exp_level_t>  &user_exp_level_map )
{

	/* 
	 *POWER(低级用户打怪输出的经验/怪物个数,1/2)*POWER((高级用户等级-低级用户等级),32/25)*怪物个数 
	 * */
	int loop;

	//计算宠物个数
	int beast_count=0;
	for (loop = 0; loop < 4 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* lp = &abi->challgee.players[loop];
		if (lp->userid ){
			beast_count++;
		}
	}
	if (beast_count==0 ){
		ERROR_LOG("beast_count==0");
		//防止后面除0
		beast_count=1;
	}



	user_exp_level_t tmp_exp_level;
	//计算每一人经验
	for (loop = 0; loop < 4 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* lp = &abu->players[loop];
		if (lp->userid  && lp->petid==0){//只保存用户的经验
			tmp_exp_level.add_exp= calc_experience(abi, lp,user_exp_level_map, 
				 (! abi->is_one_person_pk_beast()) &&	lp->userid==abu->teamid );
			tmp_exp_level.level=lp->level;
			tmp_exp_level.low_level_protect_exp=0;
			tmp_exp_level.add_protect_exp=0;
			tmp_exp_level.is_need_add_protect_exp=lp->is_need_add_protect_exp;
			user_exp_level_map[lp->userid]=tmp_exp_level;
		}
	}


	std::map<userid_t,user_exp_level_t> lower_user_map ;
	//是否和怪对战
	bool is_beast_grp= IS_BEAST_GRP_ID(abi->challgee.teamid);
	if (is_beast_grp){//只有打怪才计算
		std::map<userid_t,user_exp_level_t>::iterator it;
		 bool is_low_user_add_protect=false;//是否有低级用户为高级用户加保护经验
		//得到每一个低等级的守护贡献
		for (it=user_exp_level_map.begin();it!=user_exp_level_map.end();++it ){
			if ( it->second.level < 15 ) {//是低等级的
				if( it->second.level-5<= abi->challgee.ave_lvl ){//比怪平均等级多5以内
					it->second.low_level_protect_exp=pow( it->second.add_exp/beast_count,0.5);
					is_low_user_add_protect=true;
				} 
				lower_user_map[it->first ]=it->second;
			}
		}
		//高级用户
		for (it=user_exp_level_map.begin();it!=user_exp_level_map.end();++it ){


			if ( it->second.level >=15 && it->second.is_need_add_protect_exp) {

				std::map<userid_t,user_exp_level_t>::iterator lower_it;
	 			//*POWER(低级用户打怪输出的经验/怪物个数,1/2)*POWER((高级用户等级-低级用户等级),32/25)*怪物个数 
				for (lower_it=lower_user_map.begin();
						lower_it!=lower_user_map.end();++lower_it){
					it->second.add_protect_exp+=
						lower_it->second.low_level_protect_exp* 
						pow(it->second.level-lower_it->second.level ,32.0/25 )*beast_count ;
				}


				if(	 is_low_user_add_protect){
					it->second.add_protect_exp=it->second.add_protect_exp/lower_user_map.size();
				}else if (lower_user_map.size()>0){ //队伍中还是有低级用户
					it->second.add_protect_exp=10;
				}else{
					it->second.add_protect_exp=0;
				}
			}
		}
	
	}
}

void sync_user_atk_attr (battle_info_t* abi, battle_users_t* abu,uint32_t other_userid, uint32_t cmd)
{
	uint8_t buff[40960] = {0};
	int j = 0, j_labal = 12, cnt = 0, loop;

	PKG_H_UINT32(buff, abu->victory, j);
	PKG_H_UINT32(buff, other_userid, j);
	PKG_H_UINT32(buff, IS_BEAST_GRP_ID(abi->challgee.teamid), j);
	j += 4;

	std::map<userid_t,user_exp_level_t>  user_exp_level_map;
	set_user_protect_exp_map( abi,  abu,user_exp_level_map  );
	

	for (loop = 0; loop < 4 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* lp = &abu->players[loop];
		if (lp->userid) {
			lp->hp = (lp->hp == 0) ? 1 : lp->hp;
			PKG_H_UINT32(buff, lp->userid, j);
			PKG_H_UINT32(buff, lp->petid, j);
			PKG_H_UINT32(buff, lp->hp, j);
			PKG_H_UINT32(buff, lp->mp, j);
			PKG_H_UINT32(buff, lp->injury_lv, j);
			PKG_H_UINT32(buff, lp->last_atk_type[0], j);
			PKG_H_UINT32(buff, lp->last_atk_type[1], j);
			PKG_H_UINT32(buff, lp->last_atk_level[0], j);
			PKG_H_UINT32(buff, lp->last_atk_level[1], j);
			PKG_H_UINT32(buff, lp->auto_atk_rounds, j);

			if (lp->petid==0 ){//是用户，已经计算过了
				PKG_H_INT32(buff,user_exp_level_map[lp->userid].add_exp ,j);
				PKG_H_UINT32(buff,user_exp_level_map[lp->userid].add_protect_exp,j);
			}else if (lp->pet_state == rdy_fight || (lp->pet_state ==  on_fuzhu && !(RUN_OR_OUT(lp) || WARRIOR_DEAD(lp)))){
				//宠物＋战斗
				int32_t add_exp=calc_experience(abi, lp,user_exp_level_map,false);
				if (lp->pet_state ==  on_fuzhu ){
						add_exp*=0.5;
				}
				PKG_H_INT32(buff,add_exp,j);
				PKG_H_UINT32(buff,0,j);
			}else{
				PKG_H_INT32(buff, 0, j);
				PKG_H_UINT32(buff,0,j);
			}
			
			int l;
			KDEBUG_LOG(lp->userid,"cloth_cnt=%u,skill_cnt=%u",lp->cloth_cnt,lp->skill_cnt);
			for (l = 0; l < MAX_SKILL_NUM_PER_PERSON; l++) {
				PKG_H_UINT32(buff, lp->skills[l].skill_id, j);
				PKG_H_UINT32(buff, lp->skills[l].skill_exp, j);
				PKG_H_UINT32(buff, lp->skills_use_count[l], j);
			}

			j += pkg_body_clothes_ex(lp->cloth_cnt, lp->clothes, buff + j);

			cnt++;
		}
	}
	

	battle_users_t* enemy_team = &abi->challgee;
	if (abu == &abi->challgee){
		enemy_team = &abi->challger;
	}

	for (loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* pbst = &(enemy_team->players[loop]);
		PKG_H_UINT32(buff, pbst->userid, j);
		PKG_H_UINT32(buff, pbst->type_id, j);
		PKG_H_UINT32(buff, pbst->level, j);
	}
	PKG_H_UINT32(buff, cnt, j_labal);
	DEBUG_LOG("SYNC_USER_ATK_ATTR cmdid=%u body_len=%u",cmd,j );

	send_to_online(j, buff, abu->teamid, abi->batid, cmd, abu->online_id);
}

void report_battle_end (battle_info_t* abi)
{
	sync_user_atk_attr(abi, &abi->challger, abi->challgee.get_one_userid() , proto_bt_battle_end);
	if (!IS_BEAST_GRP_ID(abi->challgee.teamid)){
		sync_user_atk_attr(abi, &abi->challgee, abi->challger.get_one_userid(), proto_bt_battle_end);
	}
}

void notify_user_out_time_out ( battle_info_t* abi, warrior_t* lp )
{
	uint32_t j = 0;
	uint8_t buff[4096] = {0};
	PKG_H_UINT32(buff, lp->userid, j);
	PKG_H_UINT32(buff, lp->hp, j);
	PKG_H_UINT32(buff, lp->mp, j);
	PKG_H_UINT32(buff, lp->injury_lv, j);
	PKG_H_UINT32(buff, lp->last_atk_type[0], j);
	PKG_H_UINT32(buff, lp->last_atk_type[1], j);
	PKG_H_UINT32(buff, lp->last_atk_level[0], j);
	PKG_H_UINT32(buff, lp->last_atk_level[1], j);
	PKG_H_UINT32(buff, lp->auto_atk_rounds, j);

	if (lp->p_waor_state->check_state( attacked_out_bit)){
		PKG_H_UINT32(buff, 1, j);
	} else{
		PKG_H_UINT32(buff, 0, j);
	}
	
	j += pkg_body_clothes_ex(lp->cloth_cnt, lp->clothes, buff + j);
	
	uint32_t jlabel = j;
	j += sizeof(uint32_t);
	uint32_t pet_cnt = 0;
	for (int idx = 0; idx < MAX_GROUP_PLAYERS*5; idx++){
		warrior_t* lp_pet = &(lp->self_team->players[idx]);
		if (lp_pet->userid != lp->userid || lp_pet->petid ==0){
			continue;
		}

		pet_cnt++;
		PKG_H_UINT32(buff, lp_pet->petid, j);
		PKG_H_UINT32(buff, lp_pet->hp, j);
		PKG_H_UINT32(buff, lp_pet->mp, j);
		PKG_H_UINT32(buff, lp_pet->injury_lv, j);
		PKG_H_UINT32(buff, lp_pet->last_atk_type[0], j);
		PKG_H_UINT32(buff, lp_pet->last_atk_type[1], j);
		PKG_H_UINT32(buff, lp_pet->last_atk_level[0], j);
		PKG_H_UINT32(buff, lp_pet->last_atk_level[1], j);
		PKG_H_UINT32(buff, lp_pet->auto_atk_rounds, j);

	}
	PKG_H_UINT32(buff, pet_cnt, jlabel);
	
	send_to_online(j, buff, abi->challger.teamid, abi->batid, proto_bt_noti_user_out, abi->challger.online_id);	
}


static void notify_user_out (battle_info_t* abi)
{
	int loop;
	int idx = 0;
	int pos = 0;
	uint32_t pet_cnt = 0;
	DEBUG_LOG("notify_user_out ");
	for (loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* lp = abi->challger.players_pos[loop];
		if (!lp || lp->petid || lp->p_waor_state->check_state(attack_out_sync))
			continue;
		DEBUG_LOG("challger:%u %u %u",lp->userid,lp->p_waor_state->check_state( run_away_bit)
				,lp->p_waor_state->check_state( attacked_out_bit));	
		if (lp->p_waor_state->check_state( run_away_bit) 
			|| lp->p_waor_state->check_state( attacked_out_bit)) {
			SET_WARRIOR_STATE(lp->p_waor_state->state, attack_out_sync);
			uint32_t j = 0;
			uint8_t buff[4096] = {0};
			PKG_H_UINT32(buff, lp->userid, j);
			PKG_H_UINT32(buff, lp->hp, j);
			PKG_H_UINT32(buff, lp->mp, j);
			PKG_H_UINT32(buff, lp->injury_lv, j);
			PKG_H_UINT32(buff, lp->last_atk_type[0], j);
			PKG_H_UINT32(buff, lp->last_atk_type[1], j);
			PKG_H_UINT32(buff, lp->last_atk_level[0], j);
			PKG_H_UINT32(buff, lp->last_atk_level[1], j);
			PKG_H_UINT32(buff, lp->auto_atk_rounds, j);

			if (lp->p_waor_state->check_state( attacked_out_bit)){
				PKG_H_UINT32(buff, 1, j);
			} else{
				PKG_H_UINT32(buff, 0, j);
			}
			
			j += pkg_body_clothes_ex(lp->cloth_cnt, lp->clothes, buff + j);
			
			uint32_t jlabel = j;
			j += sizeof(uint32_t);
			pet_cnt = 0;
			for (idx = 0; idx < MAX_GROUP_PLAYERS*5; idx++){
				warrior_t* lp_pet = &(lp->self_team->players[idx]);
				if (lp_pet->userid != lp->userid || lp_pet->petid ==0)
					continue;

				pet_cnt++;
				PKG_H_UINT32(buff, lp_pet->petid, j);
				PKG_H_UINT32(buff, lp_pet->hp, j);
				PKG_H_UINT32(buff, lp_pet->mp, j);
				PKG_H_UINT32(buff, lp_pet->injury_lv, j);
				PKG_H_UINT32(buff, lp_pet->last_atk_type[0], j);
				PKG_H_UINT32(buff, lp_pet->last_atk_type[1], j);
				PKG_H_UINT32(buff, lp_pet->last_atk_level[0], j);
				PKG_H_UINT32(buff, lp_pet->last_atk_level[1], j);
				PKG_H_UINT32(buff, lp_pet->auto_atk_rounds, j);


			}
			PKG_H_UINT32(buff, pet_cnt, jlabel);
			
			send_to_online(j, buff, abi->challger.teamid, abi->batid, proto_bt_noti_user_out, abi->challger.online_id);
		}
	}
	
	for (loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* lp = abi->challgee.players_pos[loop];
		if (!lp || lp->petid || lp->p_waor_state->check_state( attack_out_sync))
			continue;
		
		DEBUG_LOG("challgee:%u %u %u %u",lp->userid,lp->petid,lp->p_waor_state->check_state( run_away_bit)
				,lp->p_waor_state->check_state( attacked_out_bit));	
		if (lp->p_waor_state->check_state( run_away_bit) 
			|| lp->p_waor_state->check_state( attacked_out_bit)) {
			if (!IS_BEAST_ID(abi->challgee.teamid)) {
				SET_WARRIOR_STATE(lp->p_waor_state->state, attack_out_sync);
				uint32_t j = 0;
				uint8_t buff[4096] = {0};
				PKG_H_UINT32(buff, lp->userid, j);
				PKG_H_UINT32(buff, lp->hp, j);
				PKG_H_UINT32(buff, lp->mp, j);
				PKG_H_UINT32(buff, lp->injury_lv, j);
				PKG_H_UINT32(buff, lp->last_atk_type[0], j);
				PKG_H_UINT32(buff, lp->last_atk_type[1], j);
				PKG_H_UINT32(buff, lp->last_atk_level[0], j);
				PKG_H_UINT32(buff, lp->last_atk_level[1], j);
				PKG_H_UINT32(buff, lp->auto_atk_rounds, j);

				if (lp->check_state( attacked_out_bit)){
					PKG_H_UINT32(buff, 1, j);
				}
				else{
					PKG_H_UINT32(buff, 0, j);
				}
				
				j += pkg_body_clothes_ex(lp->cloth_cnt, lp->clothes, buff + j);

				pos = get_warrior_pos(lp->self_team, lp->userid, 0);
				pos = (pos >= MAX_PLAYERS_PER_TEAM)?(pos - MAX_PLAYERS_PER_TEAM):pos;

				uint32_t jlabel = j;
				j += sizeof(uint32_t);
				pet_cnt = 0;
			
				for (idx = 0; idx < MAX_GROUP_PLAYERS; idx++){
					warrior_t* lp_pet = &(lp->self_team->players[(pos + 5 * idx)]);
					if (!lp_pet->petid){
						continue;
					}

					pet_cnt++;
					PKG_H_UINT32(buff, lp_pet->petid, j);
					PKG_H_UINT32(buff, lp_pet->hp, j);
					PKG_H_UINT32(buff, lp_pet->mp, j);
					PKG_H_UINT32(buff, lp_pet->injury_lv, j);
					PKG_H_UINT32(buff, lp_pet->last_atk_type[0], j);
					PKG_H_UINT32(buff, lp_pet->last_atk_type[1], j);
					PKG_H_UINT32(buff, lp_pet->last_atk_level[0], j);
					PKG_H_UINT32(buff, lp_pet->last_atk_level[1], j);
					PKG_H_UINT32(buff, lp_pet->auto_atk_rounds, j);

				}
				PKG_H_UINT32(buff, pet_cnt, jlabel);
				
				send_to_online(j, buff, abi->challgee.teamid, abi->batid, proto_bt_noti_user_out, abi->challgee.online_id);
			}
		}
	}
}

int check_item_maybeheji(atk_info_t* p_cur_atk)
{
	switch (p_cur_atk->wrior->weapon_type){
	case weapon_gong:
		return 0;
	}
	return (IS_BASE_ATK_SKILL(p_cur_atk->atk_type) || IS_HEJI_SKILL(p_cur_atk->atk_type));
}

static uint8_t check_self_in_heji(battle_info_t * abi, atk_info_t* p_cur_atk)
{
	list_head_t* lh;
	uint8_t self = 0;
	list_for_each(lh, &abi->attack_list) {
		resolved_atk_t* p_resolved_atk = list_entry(lh, resolved_atk_t, atk_list);
		if (p_resolved_atk->atk_type == UPDATE_WARRIOR_STATE){
			continue;
		}

		switch(p_resolved_atk->atk_type){
		case skill_pa_heji:
		case skill_pa_pet_heji:
			if (p_cur_atk->wrior->userid == p_resolved_atk->fighter_id && p_cur_atk->wrior->petid == p_resolved_atk->petid){
				self = 1;
			}
			break;
		default:
			self = 0;
			break;
		}
	}

	return self;
}

int check_a_and_b_canbe_heji(battle_info_t* abi, warrior_t* lp, atk_info_t* p_pre_atk, atk_info_t* p_cur_atk)
{
	if (p_pre_atk == NULL || p_cur_atk == NULL){
		return 0;
	}

	if (p_pre_atk->atk_mark != p_cur_atk->atk_mark){
		return 0;
	}
	// 以上check可有可无


	// 判断技能id，是否为普通攻击和合击id
	if ((IS_BASE_ATK_SKILL(p_pre_atk->atk_type) || IS_HEJI_SKILL(p_pre_atk->atk_type))
			&& (IS_BASE_ATK_SKILL(p_cur_atk->atk_type) || IS_HEJI_SKILL(p_cur_atk->atk_type))){
	}else{
		return 0; // 不是返回
	}

	// 技能id修改为合击id
	uint32_t pre_atk_type = p_pre_atk->atk_type;
	p_pre_atk->atk_type = IS_BEAST(p_pre_atk->wrior)?skill_pa_pet_heji:skill_pa_heji;
	// 修正合法攻击对象
	if (get_valid_attack_warrior_ex(abi, p_pre_atk->wrior, p_pre_atk) == NULL){
		p_pre_atk->atk_type = pre_atk_type;
		return 0;
	}
	// 恢复技能id
	p_pre_atk->atk_type = pre_atk_type;

	// 修正技能id
	uint32_t cur_atk_type = p_cur_atk->atk_type;
	p_cur_atk->atk_type = IS_BEAST(p_cur_atk->wrior)?skill_pa_pet_heji:skill_pa_heji;
	// 修正合法攻击对象
	if (get_valid_attack_warrior_ex(abi, p_cur_atk->wrior, p_cur_atk) == NULL){
		p_cur_atk->atk_type = cur_atk_type;
		return 0;
	}
	// 恢复技能id
	p_cur_atk->atk_type = cur_atk_type;

	// 不存在合法攻击对象
	if (p_pre_atk->atk_pos == -1 || p_pre_atk->atk_pos == -1){
		return 0;
	}

	// 攻击对象不一致
	if (p_pre_atk->atk_pos != p_cur_atk->atk_pos){
		return 0;
	}

	// 攻击对象已经不能被攻击
	warrior_t* lp_ene = get_attacked_warrior(lp, p_pre_atk);
	if (!lp_ene || NONEED_ATTACK(lp_ene)){
		return 0;
	}
	
	lp_ene = get_attacked_warrior(lp, p_cur_atk);
	if (!lp_ene || NONEED_ATTACK(lp_ene)){
		return 0;
	}

	// 确保自己是合法的合击对象参与者
	if (check_self_in_heji(abi, p_cur_atk)){
		return 0;
	}

	// 怪物的合击概率为70%
	if (IS_BEAST_ID(p_cur_atk->wrior->userid)){
		if ((rand() % 100) >= 70){
			return 0;
		}
	}else{
		// 人的合击概率是96%
		if ((rand() % 100) >= 96){
			return 0;
		}
	}
	

	// 最终校验
	return (p_pre_atk->wrior->enemy_team == p_cur_atk->wrior->enemy_team 
			&& p_pre_atk->wrior != p_cur_atk->wrior
			&& p_pre_atk->atk_pos == p_cur_atk->atk_pos	
			&& p_pre_atk->atk_mark== p_cur_atk->atk_mark);
}
//检查并设置最终的操作目标
void check_and_set_attack_empty_pos(battle_info_t * abi, atk_info_t* p_cur_atk)
{
	switch(p_cur_atk->atk_type){
		case skill_recall_pet:
			return;
	}
	warrior_t* p = p_cur_atk->wrior;
	warrior_t* lp = get_attacked_warrior(p, p_cur_atk);
	if (!lp){
		if (p_cur_atk->atk_mark){//是对方
			//得到自己的位置
			int pos = get_warrior_pos(p->self_team, p->userid, p->petid);
			p_cur_atk->atk_pos = get_near_rand_alive_warrior_pos(p, pos);
		}else{
			p_cur_atk->atk_pos = get_rand_alive_warrior_pos(p->self_team,p->pet_state);
		}
	}
}
static uint8_t check_warrior_cannot_attack(atk_info_t* p_cur_atk)
{
	warrior_t* lp = p_cur_atk->wrior;
	if (!lp)
		return 1;
	if (CANNOT_ATTACK(lp) || WARRIOR_SHIHUA(lp) || WARRIOR_HUNSHUI(lp)){
		if (lp->petid && lp->pet_state == rdy_fight){
			switch(p_cur_atk->atk_type){
				case skill_user_break_off:
				case skill_pet_break_off:
					return 0;
			}
		}
		return 1;
	}

	if (!lp->petid){
		return 0;
	}
	
	switch(p_cur_atk->atk_type){
		case skill_user_break_off:
		case skill_pet_break_off:
			return 0;
	}

	if(	lp->check_state( stop_attack)){
		return 1;
	}

	return 0;
}


//计算对战结果
void resolve_and_calc_round_battle_result(battle_info_t* abi)
{
	DEBUG_LOG("resolve_and_calc_round_battle_result START:战斗开始 {");
	int heji_seq = 0;
	atk_info_t* p_pre_atk = NULL;

	list_head_t* next;
	list_head_t* cur;
	list_for_each_safe(cur, next, &abi->speed_list) {
		//xxx
		if (abi->challgee.alive_count <= 0 || abi->challger.alive_count <= 0)
			break;
		
		atk_info_t* p_cur_atk = list_entry(cur, atk_info_t, atk_info_list);
		warrior_t* p = p_cur_atk->wrior;
		if (check_warrior_cannot_attack(p_cur_atk)){
			continue;
		}
		if (WARRIOR_HUNSHUI(p)) {
			p_cur_atk->atk_level = 1;
			p_cur_atk->atk_type = IS_BEAST(p) ? skill_pa_pet_dai_ji : skill_pa_dai_ji;
		}

		check_and_set_attack_empty_pos(abi, p_cur_atk);
		
	//	KDEBUG_LOG(p->userid,"tttt111 %u",p->petid);
		if (check_item_maybeheji(p_cur_atk)){//该技能是可以合击
			if (p_pre_atk == NULL){//是第一个,过 
				p_pre_atk = p_cur_atk;
				continue;
			}

			if (check_a_and_b_canbe_heji(abi, p, p_pre_atk, p_cur_atk)){
				p_pre_atk->atk_type = IS_BEAST(p_pre_atk->wrior) ? skill_pa_pet_heji : skill_pa_heji;
				p_cur_atk->atk_type = IS_BEAST(p_cur_atk->wrior) ? skill_pa_pet_heji : skill_pa_heji;
				p_cur_atk->heji_seq = ++heji_seq;
				
				//DEBUG_LOG("SET HEJI\t[batid=%lu %u %u %u; %u %u %u HEPOS=%u %u]", 
					//abi->batid, p_pre_atk->wrior->userid, p_pre_atk->wrior->petid, p_cur_atk->wrior->userid, p_cur_atk->wrior->petid, p_pre_atk->heji_seq, p_cur_atk->heji_seq, p_pre_atk->atk_pos, p_cur_atk->atk_pos);
				
				resolve_and_calc_step_battle_result(abi, p_pre_atk);
				
				if (p_pre_atk->heji_seq == LAST_HEJI_WARR || (p_pre_atk->atk_type != skill_pa_pet_heji && p_pre_atk->atk_type != skill_pa_heji)){
					DEBUG_LOG("PRE IS HEJI END\t[batid=%lu]", abi->batid);
					p_cur_atk->atk_type = IS_BEAST(p_cur_atk->wrior) ? skill_pa_pet_base_fight : skill_pa_base_fight;
					p_cur_atk->heji_seq = 0;
					heji_seq = 0;
				}
				p_pre_atk = p_cur_atk;
				continue;
			}

			if (IS_HEJI_SKILL(p_pre_atk->atk_type)){
				p_pre_atk->heji_seq = LAST_HEJI_WARR;
				heji_seq = 0;
			}

			resolve_and_calc_step_battle_result(abi, p_pre_atk);

			// battle action above maybe kill curr atk
		    p_pre_atk = CANNOT_ATTACK(p) ? NULL : p_cur_atk;
			continue;
		} //if (check_item_maybeheji(p_cur_atk))

		if (p_pre_atk){
			if (IS_HEJI_SKILL(p_pre_atk->atk_type)){
				p_pre_atk->heji_seq = LAST_HEJI_WARR;
				heji_seq = 0;
			}
			resolve_and_calc_step_battle_result(abi, p_pre_atk);
			p_pre_atk = NULL;
		}

	//	KDEBUG_LOG(p->userid,"tttt222 %u",p->petid);
		if (!check_warrior_cannot_attack(p_cur_atk)){
			resolve_and_calc_step_battle_result(abi, p_cur_atk);
		}

	//	KDEBUG_LOG(p->userid,"tttt333 %u",p->petid);
	}//list_for_each_safe(cur, next, &abi->speed_list)

	// loop is over, check pre is null
	// if not, we haven't deal pre's battle in loop
	if (p_pre_atk){
		if (IS_HEJI_SKILL(p_pre_atk->atk_type)){
			p_pre_atk->heji_seq = LAST_HEJI_WARR;
			heji_seq = 0;
		}

		resolve_and_calc_step_battle_result(abi, p_pre_atk);
		p_pre_atk = NULL;
	}
	DEBUG_LOG("}resolve_and_calc_round_battle_result END ");
}

//检查,是否可以开始计算，可以则进行回合计算
uint8_t chk_and_calc_attacks(battle_info_t* abi)
{
	DEBUG_LOG("chk_and_calc_attacks");
	if (chk_all_user_fighted(abi)) {
		if (abi->fight_timer){
			REMOVE_TIMER(abi->fight_timer);
			abi->fight_timer = NULL;
		}
		//处理怪物的招术
		new_round_battle_beast(abi);
		DEBUG_LOG("========================ATTACK START===============================");
		//计算结果
		resolve_and_calc_round_battle_result(abi); 
		//在回合后清除状态
		clean_state_after_round_attack(abi);
		//通知用户,攻击演示列表
		response_attacks_info(abi, proto_bt_battle_attacks_info);
		//通知用户,最后的情况
		response_battle_info(abi, proto_bt_battle_round_result);
		//通用用户 ，被打飞的情况
		notify_user_out(abi);
		//清除速度列表
		clean_battle_speeds_list(abi);
		DEBUG_LOG("========================ATTACK END===============================");
		abi->battle_loop++;
		//DEBUG_LOG("CHECK BATTLE_END");
		if (battle_end(abi)) {
			DEBUG_LOG("---------------------------BATTLE_END-----------------------");
			report_battle_end(abi);
			free_batter_by_id(abi->batid);
			return btl_op_res_over;
		} else {
			abi->touxi = touxi_none;
			new_round_battle(abi);
			return btl_op_res_new_loop;
		}
	}
	return btl_op_res_wait_skill;
}

static int check_warrior_noneed_attack(battle_info_t* abi, warrior_t* lp, atk_info_t* p_atk_info )
{
	if (IS_BEAST(lp) || p_atk_info->atk_seq == 1)
		return 1;

	warrior_t* lp_pet = get_pet_from_warriorlist(abi, lp->userid);
	return lp_pet && !RUN_OR_OUT(lp_pet);
}

//偷袭处理
void check_touxi_attack(battle_info_t* abi, warrior_t* lp, atk_info_t* p_atk_info)
{
	//没有偷袭
	if (abi->touxi == touxi_none){
		return;
	}

	//不需要处理
	switch(p_atk_info->atk_type){
		case skill_pa_pet_dai_ji:
		case skill_pa_dai_ji:
		case skill_user_break_off:
		case skill_pet_break_off:
			return;
	}

	if (lp->self_team == &abi->challger && abi->touxi == touxi_by_challgee){
		ERROR_LOG("touxi must daiji\t[batid=%lu uid=%u petid=%u]", abi->batid, lp->userid, lp->petid);
		p_atk_info->atk_type = IS_BEAST(lp)?skill_pa_pet_dai_ji:skill_pa_dai_ji;
		p_atk_info->atk_level = 1;
		p_atk_info->atk_mark = 0;
	}

	if (lp->self_team == &abi->challgee && abi->touxi == touxi_by_challger){
		ERROR_LOG("touxi must daiji\t[batid=%lu uid=%u petid=%u]", abi->batid, lp->userid, lp->petid);
		p_atk_info->atk_type = IS_BEAST(lp)?skill_pa_pet_dai_ji:skill_pa_dai_ji;
		p_atk_info->atk_level = 1;
		p_atk_info->atk_mark = 0;
	}
}

void check_current_loop_pause(battle_info_t* abi, warrior_t* p, atk_info_t* p_atk_info)
{
	if (!p->petid && p->check_state( pause_bit)){
		switch(p_atk_info->atk_type){
		case skill_pa_dai_ji:
		case skill_pa_pet_dai_ji:
		case skill_user_break_off:
		case skill_pet_break_off:
			break;
		default:
			p_atk_info->atk_mark = 0;
			p_atk_info->atk_level = 1;
			p_atk_info->atk_pos = 0;
			p_atk_info->atk_type = skill_pa_dai_ji;
			ERROR_LOG("must daiji\t[batid=%lu btl_loop=%u uid=%u petid=%u enemymark=%u enemypos=%d atktype=%u]", 
				abi->batid, abi->battle_loop, p->userid, p->petid, p_atk_info->atk_mark, p_atk_info->atk_pos, p_atk_info->atk_type);
			break;
		}

		clean_one_state(p, pause_bit, 1);
	}
}

//attack start
uint8_t battle_attack_op_ex(battle_info_t* abi, warrior_t* p, uint32_t enemy_mark,int32_t enemy_pos, uint32_t atk_type,uint32_t atk_level ,uint32_t itemid, uint32_t add_hp )
{
	KDEBUG_LOG(p->userid,"WAR=%u ATK=%u ", p->petid, atk_type );

	if (abi->batl_state != battle_started) {
		ERROR_LOG("battle not in ready state\t[%u %u]", p->userid, abi->batl_state);
		return btl_op_res_error;
	}

	//KDEBUG_LOG(p->userid,"WAR1");
	if (	
		((atk_type % 5000) != skill_pa_dai_ji
		&& atk_type != skill_pa_dead_dumy 
		&& atk_type != skill_pet_break_off 
		&& atk_type != skill_user_break_off 
		&& (CANNOT_ATTACK(p) || p->is_not_in_battle_mode()) )
		||  atk_type== skill_rl_pet_jiushu_all_only_svr //是程序内部技能
			) {
		ERROR_LOG("ATTACK-PKG\t[batid=%lu uid=%u petid=%u enemymark=%u enemypos=%d atktype=%u atklevel=%u]", abi->batid, p->userid, p->petid, enemy_mark, enemy_pos, atk_type, atk_level);
		return btl_op_res_error;
	}

	uint8_t dumy_skill = 0;
	if (atk_type == skill_pa_dead_dumy){//表示自己已经死了
		// 暂时只考虑人使用这个技能
		// 如果是宠物就忽略掉
		if (p->petid){
			return btl_op_res_wait_skill;
		}

		if(!p->is_not_in_battle_mode()) {
			if (p->check_state( attacked_out_bit)){
				DEBUG_LOG("1.1");
				ERROR_LOG("ATK_OUT WARRIOR NONEED TO SEND DUMY SKILL\t[batid=%lu btl_loop=%u uid=%u petid=%u]", abi->batid, abi->battle_loop, p->userid, p->petid);
				return btl_op_res_error;
			}

			if (!p->check_state( dead_bit)){
				DEBUG_LOG("1.2");
				ERROR_LOG("ONLY DEAD WARRIOR CAN SEND DUMY SKILL\t[batid=%lu btl_loop=%u uid=%u petid=%u]", abi->batid, abi->battle_loop, p->userid, p->petid);
				return btl_op_res_error;
			}
			
			/*
			if (!((abi->challgee.team_type | abi->challger.team_type) & team_npc_friend)){
				DEBUG_LOG("1.3:%d:%d",abi->challgee.team_type,abi->challger.team_type  );
				// 非带npc参战的情况下，无需考虑这个技能
				return btl_op_res_wait_skill;
			}
			*/
		} else {
			p->set_attack_dumy(true);
		}

		dumy_skill = 1;//不会任何事
	}

	if (atk_level <= 0 || atk_level > 10){
		atk_level = 1;
	}

	if (!dumy_skill){//正常处理 
		//得到两次攻击中的哪一次
		int atk_idx = attack_indx(abi, p);
		if (atk_idx == -1) {
			KERROR_LOG(p->userid, "alrdy attack\t[batid=%lu %u %u %u %d %u]", abi->batid, p->userid, p->petid, enemy_mark, enemy_pos, atk_type);
			return btl_op_res_error;
		}
		if(get_last_atk(abi,p,atk_idx,atk_type,atk_level,enemy_mark,enemy_pos,itemid,add_hp)){
			//已经处理过了
			return 0;
		}

		//检查技能及其等级是否合法,super bug solved
		if((IS_BEAST(p) && atk_type > 5100 && atk_type != 5701) || (!IS_BEAST(p) && atk_type > 100 && atk_type != 701 &&  atk_type != skill_callback_fuzhu )) {
			skill_info_t *sk = get_beast_skill_by_atktype(p, atk_type);
			if(!sk || sk->skill_level < atk_level || sk->cool_round) {
				atk_level = 1;
				atk_type = IS_BEAST(p) ? skill_pa_pet_dai_ji : skill_pa_dai_ji;
				KERROR_LOG(p->userid, "WG:ATTACK-LEVEL INVALID\t[batid=%lu uid=%u petid=%u atk_type=%u atk_level=%u]", abi->batid, p->userid, p->petid, atk_type, atk_level);
			}
		}
		set_atk_info(&p->atk_info[atk_idx], atk_idx, enemy_mark, enemy_pos, atk_type, atk_level);
		//偷袭处理
		check_touxi_attack(abi, p, &p->atk_info[atk_idx]);
		check_current_loop_pause(abi, p, &p->atk_info[atk_idx]);

		check_user_step_attack_and_insert_atk_list(abi, p, &p->atk_info[atk_idx]);
		set_fangyu_state_before_round_attack(abi, p, &p->atk_info[atk_idx]);
		if (check_warrior_noneed_attack(abi, p, &p->atk_info[atk_idx]))
		{
			report_user_fighted(abi, p->userid, p->petid, proto_bt_noti_user_fighted);
			KDEBUG_LOG(p->userid, "petid=%d,report_user_fighted", p->petid);
		}
		DEBUG_LOG("battle_attack_op_ex[%u pos=%u skill=%u]",p->userid,p->atk_info[atk_idx].atk_pos,p->atk_info[atk_idx].atk_type);	
	}
	return chk_and_calc_attacks(abi);
} 


int get_pet_phy_pos(battle_users_t* team, int personpos, uint32_t petid)
{
	personpos = (personpos >= MAX_PLAYERS_PER_TEAM)?(personpos-MAX_PLAYERS_PER_TEAM):personpos;
	int i = 0;
	for (i = 0; i < MAX_GROUP_PLAYERS; i++){
		int pos = personpos + 5 * i;
		if (team->players[pos].petid == petid){
			return pos;
		}
	}

	return -1;
}

//掉线，强制逃跑
int bt_break_off(userid_t uid, uint32_t online_id, battle_info_t* abi, Cmessage* c_in)
{

	ERROR_LOG("XXXXX BT_BREAK_OFF ");
	if (abi->batl_state == battle_challenging){
		ERROR_LOG("BREAK OFF battle already start\t[batid=%lu uid=%u]", abi->batid, uid);
		return -1;
	}
	
	warrior_t* lp = NULL;
	if (abi->batl_state != battle_started){
		lp = get_warrior_from_warriorlist(abi, uid, 0);
		if (!lp){
			ERROR_LOG("WARRIOR IS NOT EXIST\t[batid=%lu uid=%u]", abi->batid, uid);
			return -1;
		}

		remove_warrior_loading_timer(lp);

		lp->ready = 1;
		if (check_battle_start(abi)){
			set_warrior_offline(abi, lp);
			send_break_off_skill(lp, abi);
		}else{
			set_warrior_offline(abi, lp);
		}
	} else{
		lp = get_warrior_from_warriorlist(abi, uid, 0);
		if (lp){
			KDEBUG_LOG(uid,"===break 1");
			// check if the warrior is already offline
			if (lp->check_state( person_break_off)){
				return -1;
			}

			set_warrior_offline(abi, lp);
			
			if (CANNOT_ATTACK(lp)){
				// 携带npc参战时，可能会存在用户死后 直接退出flash
				// 这个时候需要发送一个dumy技能id
				// 让战斗继续下去，否则对战会在后台一直卡死
				if (!lp->check_state( attacked_out_bit) && 
					lp->check_state( dead_bit)){
					send_dead_dumy_skill(lp, abi);
				}

				return -1;
			}

			// send offline skill
			send_break_off_skill_ex(lp, abi);
		}else{
			ERROR_LOG("USR IS NOT EXIST\t[batid=%lu uid=%u]", abi->batid, uid);
		}
	}
	return 0;
}

int bt_recall_pet(userid_t uid, uint32_t online_id, battle_info_t* abi, Cmessage* c_in)
{

	bt_recall_pet_in *p_in = P_IN;
	int  atk_idx;
	uint32_t sorp, enemy_mark, petid;
	int32_t  enemy_pos;

	enemy_mark = 0;
	sorp = 0;
	petid=p_in->petid ;

	if (abi->batl_state != battle_started) {
		ERROR_LOG("battle not in ready state\t[batid=%lu btl_loop=%u %u %u]", abi->batid, abi->battle_loop, uid, abi->batl_state);
		return -1;
	}

	if(abi->is_pets_pk_mode()) {
		KERROR_LOG(uid,"can't use skill_recall_pet");
		return -1;
	}

	warrior_t* p = get_warrior_from_warriorlist(abi, uid, sorp);
	if (!p) {
		ERROR_LOG("cannot find user\t[batid=%lu btl_loop=%u %u %u %u %d %u]", abi->batid, abi->battle_loop, uid, sorp, enemy_mark, enemy_pos, petid);
		return -1;
	}
	
	atk_idx = attack_indx(abi, p);
	if (atk_idx == -1) {
		ERROR_LOG("alrdy attack, cannot use che\t[batid=%lu btl_loop=%u %u %u %u %d %u]", abi->batid, abi->battle_loop, uid, sorp, enemy_mark, enemy_pos, petid);
		return -1;
	}

	enemy_pos = -1;
	if (petid){
		int person_pos = get_warrior_pos(p->self_team, uid, 0);
		enemy_pos = get_pet_phy_pos(p->self_team, person_pos, petid);
	}

	set_atk_info(&p->atk_info[atk_idx], atk_idx, enemy_mark, enemy_pos, skill_recall_pet, 1);
	check_touxi_attack(abi, p, &p->atk_info[atk_idx]);
	check_current_loop_pause(abi, p, &p->atk_info[atk_idx]);
	check_user_step_attack_and_insert_atk_list(abi, p, &p->atk_info[atk_idx]);
	
	if (check_warrior_noneed_attack(abi, p, &p->atk_info[atk_idx]))
	{
		report_user_fighted(abi, uid, sorp, proto_bt_noti_user_fighted);
	}
	chk_and_calc_attacks(abi);
	return 0;
}

int catch_pet_op(userid_t uid, uint32_t online_id, battle_info_t* abi, Cmessage* c_in)
{
	uint32_t enemy_mark, race_cnt, fengmo_level, itemid;
	
	uint32_t race[MAX_RACE_TYPE] = {0};
	int32_t  enemy_pos;

	catch_pet_op_in *p_in=P_IN;
	enemy_mark=p_in->mark;
	enemy_pos=p_in->pos;
	itemid=p_in->itemid;
	fengmo_level=p_in->level;
	race_cnt=p_in->data.size();
	if (race_cnt > MAX_RACE_TYPE){ // || (len - j) != int(race_cnt * sizeof(uint32_t))){
		ERROR_LOG("race cnt invalid\t[batid=%lu btl_loop=%u %u %u]", abi->batid, abi->battle_loop, uid, race_cnt);
		return 0;
	}

	
	for (uint32_t loop = 0; loop < race_cnt; loop++){
		//UNPKG_H_UINT32(body, race[loop], j);
		race[loop]=p_in->data[loop];
	}

	if (abi->batl_state != battle_started || abi->is_pets_pk_mode()) {
		ERROR_LOG("not ready or cannot\t[%lu %u %u %u]", abi->batid, abi->battle_loop, uid, abi->batl_state);
		return 0;
	}

	warrior_t* p = get_warrior_from_warriorlist(abi, uid, 0);
	if (!p) {
		ERROR_LOG("cannot find user\t[%lu %u %u]", abi->batid, abi->battle_loop, uid);
		return 0;
	}
	int atk_idx = attack_indx(abi, p);
	if (atk_idx == -1) {
		ERROR_LOG("alrdy attack\t[%lu %u %u %u %d %u]", abi->batid, abi->battle_loop, uid, enemy_mark, enemy_pos, race_cnt);
		return 0;
	}

	set_atk_info(&p->atk_info[atk_idx], atk_idx, enemy_mark, enemy_pos, skill_catch_pet, 1);
	check_touxi_attack(abi, p, &p->atk_info[atk_idx]);
	check_current_loop_pause(abi, p, &p->atk_info[atk_idx]);

	if (p->atk_info[atk_idx].atk_type == skill_catch_pet){
	 	Cwarrior_state_ex_catch_pet * p_state_ex=new Cwarrior_state_ex_catch_pet();
		p_state_ex->itemid = itemid;
		p_state_ex->feng_mo_lv = fengmo_level;
		p_state_ex->race_cnt = race_cnt;
		memcpy(p_state_ex->race, race, sizeof(race[0]) * race_cnt);
		p->p_waor_state->set_state_ex_info( CATCH_PET_STATE_BIT,p_state_ex   );
	}else{
		ERROR_LOG("daiji\t[batid=%lu btl_loop=%u %u %u %d]", abi->batid, abi->battle_loop, uid, enemy_mark, enemy_pos);
		return 0;
	}
	
	check_user_step_attack_and_insert_atk_list(abi, p, &p->atk_info[atk_idx]);
	
	if (check_warrior_noneed_attack(abi, p, &p->atk_info[atk_idx]))
		report_user_fighted(abi, uid, 0, proto_bt_noti_user_fighted);
	
	chk_and_calc_attacks(abi);
	return 0;
}
//使用药品: 生命之息 复活药
int bt_use_chemical(userid_t uid, uint32_t online_id, battle_info_t* abi, Cmessage* c_in )
{
	DEBUG_LOG("Xxxxxxx bt_use_chemical");
	int  atk_idx;
	uint32_t petid, enemy_mark, itemid, add_hp;
	int32_t  enemy_pos;
	bt_use_chemical_in *p_in = P_IN;
	
	petid=p_in->petid;
	enemy_mark=p_in->enemy_mark;
	enemy_pos=p_in->enemy_pos;
	itemid=p_in->itemid;
	add_hp=p_in->add_hp;

	if (abi->batl_state != battle_started) {
		ERROR_LOG("battle not in ready state\t[batid=%lu %u %u]", abi->batid, uid, abi->batl_state);
		return -1;
	}
	warrior_t* p = get_warrior_from_warriorlist(abi, uid, petid);
	if (!abi->is_pets_pk_mode()) {
		if(p->atk_info[0].atk_type>0 || p->p_waor_state->check_state(dead_bit)){//是宠物使用药品 2012-3-28 添加|| 修复人死宠物不能使用药品bug
			DEBUG_LOG("pet bt_use_chemical()");
			warrior_t* p_pet = get_pet_from_warriorlist(abi, p->userid);
			if( p_pet && !DEAD_FIGHTER(p_pet) ){//2012-3-29:如果没带宠物或者宠物死了则人使用药品 修复没带宠物人第二招使用药品出错
				p=p_pet;
			}
		}
	} else {
		p = get_pet_from_warriorlist(abi, p->userid);
	}
	
	//DEBUG_LOG("pppp %u %u %u",p->userid,p->petid,p->p_waor_state->check_state(dead_bit));
	if (!p) {
		KERROR_LOG(0,"cannot find user\t[batid=%lu btl_loop=%u %u %u %u %d %u]", abi->batid, abi->battle_loop, uid, petid, enemy_mark, enemy_pos, itemid);
		return -1;
	}

	atk_idx = attack_indx(abi, p);
	if (atk_idx == -1) {
		KERROR_LOG(p->userid,"alrdy attack, cannot use che\t[batid=%lu btl_loop=%u %u %u %u %d %u]", abi->batid, abi->battle_loop, uid, petid, enemy_mark, enemy_pos, itemid);
		return -1;
	}
	warrior_t* lp = get_warrior_by_pos(p->self_team, enemy_pos);
	if (!lp){
		enemy_pos = get_rand_alive_warrior_pos(p->self_team,p->pet_state);
	}
	//uint32_t skill_type=p->is_user()?skill_use_chemical:skill_pet_use_chemical ;
	uint32_t skill_type;
	skill_type=p->is_user()?skill_use_chemical:skill_pet_use_chemical ;
	set_atk_info(&p->atk_info[atk_idx], atk_idx, enemy_mark, enemy_pos, skill_type, 1);
	check_touxi_attack(abi, p, &p->atk_info[atk_idx]);
	check_current_loop_pause(abi, p, &p->atk_info[atk_idx]);

	if (p->atk_info[atk_idx].atk_type == skill_use_chemical 
			|| p->atk_info[atk_idx].atk_type == skill_pet_use_chemical){
		p->p_waor_state->set_use_item_itemid(itemid);
		p->p_waor_state->set_use_item_add_hp( add_hp);
	}else{
		ERROR_LOG("INVALID ATK_SKILL MUST BE<skill_use_chemical>\t[batid=%lu btl_loop=%u %u %u %u %d %u]", abi->batid, abi->battle_loop, uid, petid, enemy_mark, enemy_pos, itemid);
		return -1;
	}
	check_user_step_attack_and_insert_atk_list(abi, p, &p->atk_info[atk_idx]);
	if (check_warrior_noneed_attack(abi, p, &p->atk_info[atk_idx]))
	{
		report_user_fighted(abi, uid, petid, proto_bt_noti_user_fighted);
	}
	chk_and_calc_attacks(abi);

	DEBUG_LOG("Xxxxxxx end of bt_use_chemical");
	return 0;
}

static int pkg_body_clothes(int count, const body_cloth_t* p, uint8_t *buf)
{
	int j = 0;
	PKG_UINT8 (buf, count, j);
	for (int i = 0; i < count; i++) {
		PKG_UINT32 (buf, p[i].cloth_id, j);
		PKG_UINT32 (buf, p[i].type_id, j);
		PKG_UINT16 (buf, p[i].clevel, j);
	}

	return j;
}

static int pkg_user_skills(int count, const skill_info_t* psi, uint8_t *buf)
{
	int j = 0;
	PKG_UINT8 (buf, count, j);
	for (int i = 0; i < count; i++) {
		PKG_UINT32 (buf, psi[i].skill_id, j);
		PKG_UINT8 (buf, psi[i].skill_level, j);
		PKG_UINT8 (buf, psi[i].use_level, j);
		PKG_UINT8 (buf, psi[i].cool_round, j);
	}

	return j;
}
void response_battle_info_warrior_list_fmt(uint8_t * buff, int &j,  battle_info_t* abi, battle_users_t  *p_bu  )
{
	int cnt = 0, j_label = j;
	j += 4;
	for (int loop = 0; loop < 4 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* lp = &(p_bu->players[loop]);
		if (!lp || !lp->userid)
			continue;

		PKG_UINT32(buff, lp->userid, j);
		PKG_UINT32(buff, lp->petid, j);
		PKG_UINT32(buff, lp->type_id, j);
		
		int pos = -1;
		if (!lp->petid || lp->pet_state == rdy_fight){
			pos = get_warrior_pos(lp->self_team, lp->userid, lp->petid);
		}
		pos = (pos == -1)?loop:pos;
		PKG_UINT32(buff, pos, j);
		PKG_STR(buff, lp->nick, j, USER_NICK_LEN);
		PKG_UINT32(buff, lp->vip, j);
		PKG_UINT32(buff, lp->color, j);
		PKG_UINT32(buff, lp->level, j);
		PKG_UINT32(buff, lp->hp, j);
		PKG_UINT32(buff, lp->hp_max, j);
		PKG_UINT32(buff, lp->mp, j);
		PKG_UINT32(buff, lp->mp_max, j);
		PKG_UINT32(buff, lp->injury_lv, j);
		PKG_UINT32(buff, lp->change_petid, j);
		PKG_UINT32(buff, lp->auto_atk_rounds, j);
		PKG_UINT32(buff, lp->skills_forgot_cnt, j);
		for(int i=0; i < lp->skills_forgot_cnt;i++) {
			PKG_UINT32(buff, lp->skills_forgot[i], j);
		}

		PKG_UINT8(buff, lp->petid ? lp->pet_state : 0, j);
		PKG_UINT8(buff, lp->race, j);
		PKG_UINT16(buff, lp->catchable, j);

		if (IS_BEAST_ID(abi->challgee.teamid)){
			PKG_UINT8(buff, lp->earth, j);
			PKG_UINT8(buff, lp->water, j);
			PKG_UINT8(buff, lp->fire, j);
			PKG_UINT8(buff, lp->wind, j);
		}else{//不是打怪就不返回水晶信息
			PKG_UINT8(buff, 0, j);
			PKG_UINT8(buff, 0, j);
			PKG_UINT8(buff, 0, j);
			PKG_UINT8(buff, 0, j);
		}
		PKG_UINT8(buff, lp->prof, j);
		j += pkg_body_clothes(lp->cloth_cnt, lp->clothes, buff + j);
		j += pkg_user_skills(lp->skill_cnt, lp->skills, buff + j);
		cnt++;
	}	
	PKG_UINT32(buff, cnt, j_label);
}

void response_battle_info (battle_info_t* abi, uint16_t cmd)
{
	uint8_t* buff = g_proto_buff;
	int j = sizeof(protocol_t);
	PKG_UINT64(buff, abi->batid, j);
	PKG_UINT32(buff, abi->challger.teamid, j);

	// bei touxi biaozhi
	switch(cmd){
	case proto_bt_noti_battle_created:
		PKG_UINT8(buff, abi->touxi, j);
		break;
	default:
		PKG_UINT8(buff, 0, j);
		break;
	}

	if ((abi->challgee.team_type | abi->challger.team_type) & (team_beast | team_npc)){
		PKG_UINT8(buff, 0, j);
	} else{
		PKG_UINT8(buff, 1, j);
	}


	response_battle_info_warrior_list_fmt(buff, j, abi,  & (abi->challger));
	PKG_UINT32(buff, abi->challgee.teamid, j);
	response_battle_info_warrior_list_fmt(buff, j, abi,  & (abi->challgee));

	init_proto_head(buff, cmd, j);
	send_to_2_leader(abi, cmd, buff, j);
}


void response_attacks_info (battle_info_t* abi, uint16_t cmd)
{
	uint8_t* buff = g_proto_buff;
	int j = sizeof(protocol_t);
	int label_idx = j;
	j += 4;
	int cnt = 0;
	list_head_t* lh;
	// 10 * X
	DEBUG_LOG("response_attacks_info:通知用户,攻击演示列表");
	list_for_each(lh, &abi->attack_list) {
		resolved_atk_t* p_resolved_atk = list_entry(lh, resolved_atk_t, atk_list);

		KDEBUG_LOG(p_resolved_atk->fighter_id,"+atk[%u] [seq=%u,petid=%u =atk(skill=%u,level=%u,cnt=%u)=> (uid=%u,petid=%u) r_hp=%d,r_mp=%d,state=0x%lx]", cnt, 
				p_resolved_atk->seq,p_resolved_atk->petid,p_resolved_atk->atk_type,p_resolved_atk->atk_level,p_resolved_atk->atk_cnt,p_resolved_atk->atk_uid,p_resolved_atk->atk_petid,
				p_resolved_atk->change_hp, p_resolved_atk->change_mp,p_resolved_atk->fighter_state);
		PKG_UINT32(buff, p_resolved_atk->seq, j);
		PKG_UINT32(buff, p_resolved_atk->fighter_id, j);
		PKG_UINT32(buff, p_resolved_atk->petid, j);
		PKG_STATE(buff, p_resolved_atk->fighter_state, j);
		// 用户说话
		PKG_UINT32(buff, p_resolved_atk->topic_id, j);
		
		//PKG_UINT16(buff, p_resolved_atk->resume_hp1, j);
		//PKG_UINT16(buff, p_resolved_atk->resume_hp2, j);
		//PKG_UINT16(buff, p_resolved_atk->resume_hp3, j);
		//PKG_UINT16(buff, p_resolved_atk->poison_hp, j);
		PKG_UINT32(buff, p_resolved_atk->change_hp, j);
		PKG_UINT32(buff, p_resolved_atk->change_mp, j);
		PKG_UINT32(buff, p_resolved_atk->host_uid, j);
		PKG_UINT32(buff, p_resolved_atk->host_petid, j);
		PKG_UINT16(buff, p_resolved_atk->parasitism_hp, j);
		//PKG_UINT16(buff, p_resolved_atk->zhushang_hp, j);
		//PKG_UINT16(buff, p_resolved_atk->curse_mp, j);

		PKG_UINT16(buff, p_resolved_atk->atk_cnt, j);
		int loop;
		// 44 * 10
		for (loop = 0; loop < p_resolved_atk->atk_cnt; loop++) {
			nest_atk_t* p_next_atk = &p_resolved_atk->fanji[loop];
			KDEBUG_LOG(p_next_atk->uid,"%c---action[%u]:petid=%u =atk(%u,%u)=> (%u,%u):hurt_hp=%d,hurt_mp=%d,state=0x%lx",
				loop==p_resolved_atk->atk_cnt-1?'`':'|',loop,p_next_atk->petid,p_next_atk->atk_type,p_next_atk->atk_level,p_next_atk->atk_uid,p_next_atk->atk_petid,
				p_next_atk->hurt_hp, p_next_atk->hurt_mp,p_next_atk->fightee_state);


			PKG_UINT32(buff, p_next_atk->uid, j);
			PKG_UINT32(buff, p_next_atk->petid, j);
			PKG_UINT8(buff, p_next_atk->fantan_ret, j);
			PKG_UINT32(buff, p_next_atk->atk_type, j);
			PKG_UINT8(buff, p_next_atk->atk_level, j);
			PKG_UINT16(buff, p_next_atk->add_hp, j);
			PKG_UINT16(buff, p_next_atk->fantan_hp, j);
			PKG_UINT16(buff, p_next_atk->fantan_mp, j);
			PKG_UINT32(buff, p_next_atk->use_itemid, j);
			PKG_UINT32(buff, p_next_atk->atk_uid, j);
			PKG_UINT32(buff, p_next_atk->atk_petid, j);
			
			// 当前护卫者(替【p_fighter，petid】挨打的人)
			PKG_UINT8(buff, p_next_atk->huwei_pos, j);
			
			PKG_STATE(buff, p_next_atk->fightee_state, j);
			//DEBUG_LOG("ruodiantouxi state?%lu",(p_next_atk->fightee_state>>56)&1);	
			PKG_UINT16(buff, p_next_atk->hurt_hp, j);
			PKG_UINT16(buff, p_next_atk->hurt_mp, j);
		}
	
		cnt++;
	}
	PKG_UINT32(buff, cnt, label_idx);

	init_proto_head(buff, cmd, j);
	send_to_2_leader(abi, cmd, buff, j);
}
	
void clean_fdsess_onlinex_down(int fd)
{
	for (int loop = 0; loop < MAX_ONLINE_NUM; loop++) {
		if (online[loop] && online[loop]->fd == fd) {
			online[loop] = 0;
			break;
		}
	}
}
//cmd start
int battle_info_op(void* buf, int len, fdsession_t* fdsess)
{
	batrserv_proto_t* cheader = (batrserv_proto_t*)buf;
	if ( g_log_send_buf_hex_flag==1){
		char outbuf[13000];
		bin2hex(outbuf,(char*)buf,len  ,2000);
		DEBUG_LOG("CI[%u][%u][%s]",   cheader->id,cheader->opid ,outbuf  );
	}

	battle_info_t* pbi    = (battle_info_t*)g_hash_table_lookup(battles, &(cheader->challenger));
	if (cheader->opid == proto_bt_challenge_battle || cheader->opid == proto_bt_challenge_npc_battle) {
		if (pbi){
			ERROR_LOG("battle_info_op battle already exist\t[batid=%lu id=%u]", cheader->challenger, cheader->id);
			send_to_online_error(BT_ERR_ID_ALREADY_EXIST, cheader->id, cheader->challenger, cheader->opid, cheader->online_id);
			return 0;
		}
		
		pbi          	= (battle_info_t*)g_slice_alloc0(sizeof(battle_info_t));		

		pbi->flag=0;
		pbi->batid	 	= cheader->challenger;
		INIT_LIST_HEAD (&pbi->warrior_list);
		INIT_LIST_HEAD (&pbi->speed_list);
		INIT_LIST_HEAD (&pbi->timer_list);
		INIT_LIST_HEAD (&pbi->attack_list);
		g_hash_table_replace(battles, &(pbi->batid), pbi);
	}
	
	if (!pbi) {
		ERROR_LOG("not create battle\t[batid=%lu %u %u]", cheader->challenger, cheader->opid, cheader->id);
		send_to_online_error(BT_ERR_ID_NOT_EXIST, cheader->id, cheader->challenger, cheader->opid, cheader->online_id);
		return 0;
	}

#define DISPATCH_OP(op_, func_) \
		case (op_): (func_)(cheader->id, cheader->online_id, pbi, cheader->body, len - sizeof(batrserv_proto_t)); return 0; 
//old cmds
	int cmdid=cheader->opid;
	//switch (cmdid) {
		////DISPATCH_OP(proto_bt_challenge_battle, challenge_battle_op);
		////DISPATCH_OP(proto_bt_challenge_npc_battle, challenge_npc_battle_op);
		////DISPATCH_OP(proto_bt_accept_battle, accept_battle_op);
		////DISPATCH_OP(proto_bt_catch_pet, catch_pet_op);
		//default:
			//break;
	//}
#undef DISPATCH_OP

	//新加的命令
	//here "./proto/mole2_btl_bind_for_client.h"
    Ccmd< P_DEALFUN_T> * p_cmd_item =g_cmd_map.getitem(cmdid);
    if (p_cmd_item == NULL) {
        DEBUG_LOG("cmd_id not find: %u", cmdid);
        return -1;
    }
    //还原对象
    p_cmd_item->proto_pri_msg->init( );

    byte_array_t in_ba ((char*)cheader->body, len - sizeof(batrserv_proto_t) );
	//in_ba.set_is_bigendian(true);
    //失败
    if (!p_cmd_item->proto_pri_msg->read_from_buf(in_ba)) {
        DEBUG_LOG("还原对象失败");
        return -1;
    }
    //客户端多上传报文
    if (!in_ba.is_end()) {
        DEBUG_LOG("client 过多报文");
        return  -1;
    }
	p_cmd_item->func(cheader->id, cheader->online_id, pbi, p_cmd_item->proto_pri_msg);
	return 0;
}
/* @brief 初始化处理客户端数据的函数数组
 */
void init_cli_handle_funs()
{
    g_cmd_map.initlist(g_cmd_list,sizeof(g_cmd_list)/sizeof(g_cmd_list[0]));
}

int bt_load_battle_ready(userid_t uid, uint32_t online_id, battle_info_t* abi, Cmessage* c_in  )
{
	bt_load_battle_ready_in *p_in = P_IN;
	if (abi->batl_state != battle_ready) {
		KERROR_LOG(uid, "battle not ready\t[batid=%lu %u %u]", abi->batid, uid, abi->batl_state);
		return -1;
	}

    // find warrior from list
	warrior_t* p = get_warrior_from_warriorlist(abi, uid, 0);
	if (p) {
		if (p->ready){
			KERROR_LOG(uid, "warrior already ready\t[batid=%lu uid=%u %u %u]", abi->batid, uid, p->ready, p_in->ready);
			return -1 ;
		}
		
		p->ready = 1;
		if (!p_in->ready){
			KERROR_LOG(uid, "load battle failed\t[batid=%lu uid=%u]", abi->batid, uid);
			set_warrior_offline(abi, p);
		}

		remove_warrior_loading_timer(p);
	}else{
		KERROR_LOG(uid, "load_battle_ready, usr is not exist\t[batid=%lu uid=%u]", abi->batid, uid);
		return -1;
	}

 	check_battle_start(abi);
	return 0;
}

int bt_battle_attack(userid_t uid, uint32_t online_id, battle_info_t* abi, Cmessage* c_in  )
{
	bt_battle_attack_in *p_in = P_IN;
	warrior_t* p = get_warrior_from_warriorlist(abi, uid, p_in->petid);
	if (!p) {
		KERROR_LOG(uid, "cannot find warrior\t[%u %u %d %u]", p_in->petid,p_in->enemy_mark,p_in->enemy_pos,p_in->atk_type);
		return btl_op_res_error;
	}
	
	DEBUG_LOG("bt battle attack[%u pos=%u skill=%u]",uid,p_in->enemy_pos,p_in->atk_type);	
	return battle_attack_op_ex(abi, p, p_in->enemy_mark,p_in->enemy_pos,p_in->atk_type,p_in->atk_level ,
			p_in->itemid,p_in->add_hp) ;
}

void  warrior::get_t_user_list( uint32_t pos ,bool is_self_team ,std::list<struct warrior *> &warrior_list   )
{
	if (pos < 0 || pos >= 10){
		return ;
	}

	struct battle_users*	opt_team;
	if (is_self_team ){
		opt_team=this->self_team;
	}else{
		opt_team=this->enemy_team;
	}

	for (int loop = 0; loop < T_USER_MAX_NUM; loop++) {
		int lpos = t_users[pos][loop];
		if (lpos == -1){
			continue;
		}

		struct warrior * lp = opt_team->players_pos[lpos];
		if (!lp || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode()){
			continue;
		}
		warrior_list.push_back(lp);
	}
}

void  warrior::get_col_user_list( uint32_t pos ,bool is_self_team ,std::list<struct warrior *> &warrior_list   )
{
	if (pos < 0 || pos >= 10){
		return ;
	}
	struct battle_users*	opt_team;
	if (is_self_team ){
		opt_team=this->self_team;
	}else{
		opt_team=this->enemy_team;
	}

	for (int loop = 0; loop < COL_USER_MAX_NUM; loop++) {
		int lpos = col_users[pos][loop];
		if (lpos == -1){
			continue;
		}

		struct warrior * lp = opt_team->players_pos[lpos];
		if (!lp || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode()){
			continue;
		}
		warrior_list.push_back(lp);
	}
}

void  warrior::get_row_user_list( uint32_t pos ,bool is_self_team ,std::list<struct warrior *> &warrior_list   )
{
	if (pos < 0 || pos >= 10){
		return ;
	}
	struct battle_users*	opt_team;
	if (is_self_team ){
		opt_team=this->self_team;
	}else{
		opt_team=this->enemy_team;
	}

	for (int loop = 0; loop < ROW_USER_MAX_NUM; loop++) {
		int lpos = row_users[pos][loop];
		if (lpos == -1){
			continue;
		}

		struct warrior * lp = opt_team->players_pos[lpos];
		if (!lp || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode()){
			continue;
		}
		//DEBUG_LOG("kkkkk");
		warrior_list.push_back(lp);
	}
}

bool  warrior::check_mp_enough(skill_info_t * p_skill )
{
	if (p_skill->skill_id == 0){
		return 0;
	}
	// find skill from table by atk_type and prof
	skill_mp_exp_t* psu = get_skill_mp_exp(p_skill->skill_id , this->prof);
	if (psu && psu->id){
		int res_mp = this->mp;
		DEBUG_LOG("check mp enough[skillid=%u mp=%u need=%u]",p_skill->skill_id,res_mp,psu->mp_a * p_skill->skill_level+ psu->mp_b);
		res_mp -= (psu->mp_a * p_skill->skill_level+ psu->mp_b);
		// mp is not enough for current attack skill
		return res_mp >= 0;
	}
	// invalid skill
	return 0;
}

int warrior::check_self_team_hp_enough()
{
	for (int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t *lp=this->self_team->players_pos[loop];
		if(check_enermy_valid(lp) && (lp->hp*3 ) < lp->hp_max)	
		{
			return loop;
		}
	}
	return -1;
}

bool warrior::check_enermy_valid(warrior_t * lp)
{
		if (!lp || lp->is_not_in_battle_mode() || NONEED_ATTACK(lp))
			return false;

		if (this->pet_state==on_fuzhu && lp->level==1 && lp->catchable){//辅助宠物不打一级
			return false;
		}
		if (lp->flag_check_bit(bit_attacked)) {
			return false;
		}
		return true;
}

int warrior::get_self_weakest_warrior_pos()
{
	int pos=-1, hpmin=1000000; 
	for (int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t *lp=this->self_team->players_pos[loop];
		if(check_enermy_valid(lp) && hpmin > lp->hp)	
		{
			pos=loop;
			hpmin=lp->hp;
		}
	}
	return pos;
}

int warrior::get_weakest_warrior_pos()
{
	int pos=-1, hpmin=1000000; 
	for (int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t *lp=this->enemy_team->players_pos[loop];
		if(check_enermy_valid(lp) && hpmin > lp->hp)	
		{
			pos=loop;
			hpmin=lp->hp;
		}
	}
	return pos;
}

int warrior::get_most_warrior_pos()
{
	int cnt = 0,pos=-1,max=0;
	int psw[2 * MAX_PLAYERS_PER_TEAM] = {0};
	for (int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		if(check_enermy_valid(this->enemy_team->players_pos[loop])){
			psw[loop]=1;
		}	
	}
	for( uint32_t loop=0 ; loop< 2 * MAX_PLAYERS_PER_TEAM; loop++ ){
		if(psw[loop]){
			cnt=psw[t_users[loop][1]]+psw[t_users[loop][2]]+psw[t_users[loop][3]];
			if( max<cnt ){
				pos=loop;
			}
		}
	}
	return pos;
}

bool warrior::is_in_fangyu() {
	return this->check_state(fangyu_bit);
}

void read_from_struct_warrior(   warrior_t   *p ,stru_warrior_t  & w  )
{
	p->userid = w.userid;
	p->petid = w.petid;
	p->type_id = w.type_id;
	memcpy(p->nick , w.nick,sizeof(w.nick ) );
	p->warrior_flag = w.flag;
	p->vip = w.viplv;
	p->color = w.color;
	p->race = w.race;
	p->prof = w.prof;
	p->level = w.level;
	p->physique = w.physique;
	p->strength = w.strength;
	p->endurance = w.endurance;
	p->quick = w.quick;
	p->intelligence = w.intelligence;
	p->attr_addition = w.attr_addition;
	p->hp = w.hp;
	p->mp = w.mp;
	p->injury_lv = w.injury_lv;
	p->in_front = w.in_front;
	p->earth = w.earth;
	p->water = w.water;
	p->fire = w.fire;
	p->wind = w.wind;
	p->hp_max = w.hp_max;
	p->mp_max = w.mp_max;
	p->attack_value = w.attack;
	p->mattack_value = w.mattack;
	p->defense_value = w.defense;
	p->mdefense_value = w.mdefense;
	p->speed = w.speed;
	p->spirit = w.spirit;
	p->resume = w.resume;
	p->hit_rate = w.hit_rate;
	p->avoid_rate = w.avoid_rate;
	p->bisha = w.bisha;
	p->fight_back = w.fight_back;
	p->r_zhongdu = w.rpoison;
	p->r_shihua = w.rlithification;
	p->r_hunshui = w.rlethargy;
	p->r_zuzhou = w.rinebriation;
	p->r_hunluan = w.rconfusion;
	p->r_yiwang = w.roblivion;
	p->is_need_add_protect_exp = w.need_protect_exp;
	p->cloth_cnt = w.cloth_cnt;
	for( uint8_t loop=0 ; loop<8 ; loop++ ){
		p->clothes[loop].clevel=w.clothes[loop].clevel;
		p->clothes[loop].cloth_id=w.clothes[loop].cloth_id;
		p->clothes[loop].durable_val=w.clothes[loop].duration;
		p->clothes[loop].equip_type=w.clothes[loop].equip_pos;
		p->clothes[loop].type_id=w.clothes[loop].type_id;
	}
	
	//p->clothes = w.clothes;
	p->skill_cnt = w.skill_cnt;
	//p->skills = w.skills;
	for( uint8_t loop=0 ; loop<10 ; loop++ ){
		p->skills[loop].cool_round=w.skills[loop].cool_round;	
		p->skills[loop].cool_update=w.skills[loop].cool_update;	
		p->skills[loop].rand_rate=w.skills[loop].rand_rate;	
		p->skills[loop].skill_exp=w.skills[loop].skill_exp;	
		p->skills[loop].skill_id=w.skills[loop].skill_id;	
		p->skills[loop].skill_level=w.skills[loop].skill_level;	
		p->skills[loop].use_level=w.skills[loop].use_level;	
	}
	p->weapon_type = w.weapon_type;
	p->shield = w.shield;
	p->pet_cnt = w.pet_cnt_actual;
	p->catchable = w.catchable;
	p->handbooklv = w.handbooklv;
	p->pet_contract_lv = w.pet_contact_lv;
	p->pet_state = w.pet_state;

	p->last_atk_type[0] = w.default_atk[0];
	p->last_atk_type[1] = w.default_atk[1];
	p->last_atk_level[0] = w.default_atklv[0];
	p->last_atk_level[1] = w.default_atklv[1];

	p->auto_atk_rounds = w.autofight_count;
	p->change_race=w.changerace;
	p->change_petid=w.changepetid;
}


void clean_warrior_loop_state(warrior_t* p_fighter, uint8_t state_bit)
{
	if (!p_fighter->check_state( state_bit)) {
		return;
	}
	
	//处理一下状态
	if (state_bit > resume_hp3_bit && state_bit < bisha_bit) {
		RESET_WARRIOR_STATE(p_fighter->p_waor_state->state, state_bit);
		return;
	}
	
	p_fighter->p_waor_state->unset_state_info(state_bit );
}



