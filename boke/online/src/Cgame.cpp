/*
 * =========================================================================
 *
 *        Filename: Cgame.cpp
 *
 *        Version:  1.0
 *        Created:  2011-08-31 08:01:49
 *        Description:  
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */
#include  "Cgame.h"
#include  "global.h"
#include  "Csprite.h"
#include    "./proto/pop_online_enum.h"
#include "./proto/pop_db.h"
#include "./proto/pop_db_enum.h"
#include "svr_proto.h"
#include "Cactivity.h"
#include "Ctimer.h"
#include "timer_func.h"
#include <math.h>

#define NV		150.0
#define V0		-220.0
#define G		483.0
#define NG		14400.0

void Cgame_item::init(uint32_t game_map_id,uint32_t gameid, uint32_t src_userid,uint32_t obj_userid  )
{

	this->game_stat=GAME_START;	
	this->gameid = gameid;

	//需要配置
	this->start_userid=src_userid;
	this->cur_opt_userid=src_userid;
	this->step_timeout=1000;

	Cgame_user_info game_user_info;
	game_user_info.opt_count=0;
	game_user_info.can_start=false;
	game_user_info.state = 0;

	game_user_info.userid=src_userid;
	this->user_map[src_userid]=game_user_info;

	game_user_info.userid=obj_userid;
	this->user_map[obj_userid]=game_user_info;
	this->game_map_id = game_map_id;
	this->is_start_timer = false;

	std::map<uint32_t , Cgame_user_info>::iterator it;
	//设置游戏引用
	for (it=this->user_map.begin();it!=this->user_map.end();++it ){
		uint32_t obj_userid=(it->second.userid) ;
		Csprite* obj_p=g_sprite_map->get_sprite(obj_userid );		
		KDEBUG_LOG(obj_p->id ,"set obj_p->game_map_id=%u %u", obj_p->game_map_id, game_map_id);
		obj_p->game_map_id= game_map_id;
	}
}

bool Cgame_item::set_user_can_start(uint32_t userid)
{
	if(this->game_stat!=GAME_START ){
		KERROR_LOG(userid, "game_stat!=GAME_START:%d ", this->game_stat );
		return false;
	}
	
	std::map<uint32_t , Cgame_user_info>::iterator it;
	it=this->user_map.find(userid );
	if(it!=this->user_map.end() ){
		KDEBUG_LOG(userid, "set_user_can_start");
		it->second.can_start=true;
		it->second.state = 0;
	}else{
		KERROR_LOG(userid, "nofind user");
		return false;
	}

	for ( it=this->user_map.begin();it!=this->user_map.end();++it){
		if (!it->second.can_start){
			return false;
		}
	}

	this->is_start_timer = false;
	this->start_time = time(NULL);
	this->start_jump = false;

	//都已经好了,开始
	this->game_stat=GAME_OPT;
	cli_noti_game_start_out noti_out;
	noti_out.gameid=this->gameid;
	DEBUG_LOG("GAME START\t[%u %u %u]", this->gameid, this->start_userid, this->step_timeout);
	noti_out.start_userid=this->start_userid;
	noti_out.step_timeout=this->step_timeout;

	for (it=this->user_map.begin();it!=this->user_map.end();++it ){
		noti_out.userid_list.push_back((it->second.userid)) ;
	}

	this->noti_all(cli_noti_game_start_cmd, &noti_out);
	return true;
}

TimeVal Cgame_item::get_land_time()
{
	double ms_land = 0.0;
	std::map<uint32_t , Cgame_user_info>::iterator it;
	TimeVal land_time(now);
	for (it = this->user_map.begin(); it != this->user_map.end(); it ++) {
		Cgame_user_info& game_user = it->second;
		double ms = 0.0;
		if (game_user.state == 2) {
			ms = 3492;		// 初始速度为V0，坐标Y=2176，加速度g=483的时间
		} else if (game_user.state == 3) {
			ms = double(2176000 - game_user.open_y) / double(NV);
			ms -= (land_time - game_user.open_time);
		}

		if (ms > ms_land)
			ms_land = ms;
	}
	land_time.ms_timeadd(ms_land);
	return land_time;
}	

bool Cgame_item::game_user_opt_deal(uint32_t userid, cli_noti_game_opt_out* p_out, cli_game_opt_in* p_in)
{
	if (gameid == 4) {
		if (p_in->_gamemsg_len < 4)
			return false;

		std::map<uint32_t , Cgame_user_info>::iterator it;

		uint32_t state = ntohl(*(uint32_t *)p_in->gamemsg);
		KDEBUG_LOG(userid, "CLI STATE\t[%u]", state);
		Cgame_user_info& game_user = this->user_map[userid];
		if (state > 3 || state <= game_user.state || game_user.state == 3 || game_user.state == 1) {
			KERROR_LOG(userid, "invalid\t[%u %u]", state, game_user.state);
			return false;
		}

		TimeVal cur_time(now);

		bool all_over_jump = false;
		bool may_need_set_timer = true;
		if (state == 1) {
			all_over_jump = true;
			may_need_set_timer = false;
			// 抢跳
			for (it = this->user_map.begin(); it != this->user_map.end(); it ++) {
				if (it->second.state != 1 && it->second.userid != userid) {
					all_over_jump = false;
					break;
				}
			}
		} else if (state == 2) {
			// 起跳 可能设置定时器 
			game_user.jump_time = cur_time;
		} else if (state == 3) {
			// 打开伞 可能更改定时器
			int t = cur_time - game_user.jump_time;
			double cur_y = V0 * t + G * t * t / 2 / 1000;
			KDEBUG_LOG(userid, "OPEN \t[%d %f]", t, cur_y);
			if (cur_y > 1676000) {
				return false;
			}

			game_user.open_time = cur_time;
			game_user.open_y = cur_y;
			game_user.open_v = V0 * 1000 + G * t;
			if (game_user.open_v < NV * 1000)
				game_user.open_v = NV * 1000;
		}
		game_user.state = state;

		memcpy(p_out->gamemsg, p_in->gamemsg, p_in->_gamemsg_len);
		p_out->_gamemsg_len = p_in->_gamemsg_len;
		*(uint32_t *)(p_out->gamemsg + p_out->_gamemsg_len) = htonl(this->user_map.size());
		p_out->_gamemsg_len += 4;
		for (it = this->user_map.begin(); it != this->user_map.end(); it ++) {
			Cgame_user_info& game_user = it->second;
		
			*(uint32_t *)(p_out->gamemsg + p_out->_gamemsg_len) = htonl(game_user.userid);
			p_out->_gamemsg_len += 4;
			uint32_t cur_y = 0, cur_v = 0;
			if (game_user.state == 2) {
				int t = cur_time - game_user.jump_time;
				cur_v = V0 + G * t;
				cur_y = V0 * t + G * t * t / 2 / 1000;
			} else if (game_user.state == 3) {
				int t = cur_time - game_user.open_time;
				cur_y = NV * t + game_user.open_y;
				cur_v = NV * 1000;
			}

            *(uint32_t *)(p_out->gamemsg + p_out->_gamemsg_len) = htonl(cur_y);
            p_out->_gamemsg_len += 4;
            *(uint32_t *)(p_out->gamemsg + p_out->_gamemsg_len) = htonl(cur_v);
            p_out->_gamemsg_len += 4;
            *(uint32_t *)(p_out->gamemsg + p_out->_gamemsg_len) = htonl(game_user.state);
            p_out->_gamemsg_len += 4;
		}

		if (may_need_set_timer) {
			TimeVal uptime = this->get_land_time();
			if (!this->is_start_timer) {
				this->timer_seq = g_timer_map->add_timer(uptime,n_deal_game_timer, this->start_userid, &this->game_map_id, 4);
				this->is_start_timer = true;
			} else {
				g_timer_map->mod_expired_time(this->start_userid, this->timer_seq, uptime);
			}
		}
		this->noti_all(cli_noti_game_opt_cmd, p_out);
		if (all_over_jump) {
			this->game_end(userid, 2, false, false);
		}
		return false;
	} else {
		memcpy(p_out->gamemsg,p_in->gamemsg,p_in->_gamemsg_len);
		p_out->_gamemsg_len=p_in->_gamemsg_len;
		return true;
	}
}

bool Cgame_item::user_opt( uint32_t userid, cli_game_opt_in *p_in)
{
	if (this->game_stat!= GAME_OPT ){
		return false;
	}

	std::map<uint32_t , Cgame_user_info>::iterator it;
	it=this->user_map.find(userid );
	if(it!=this->user_map.end() ){//找到用户
		Cgame_user_info *p_item=&(it->second);
		p_item->opt_count++;
	}else{
		return false;
	}
	//通知用户
	cli_noti_game_opt_out noti_out; 
	noti_out.src_userid=userid;

	//复制消息
	if (p_in->_gamemsg_len>sizeof( noti_out.gamemsg)){
		return false;
	}

	if (game_user_opt_deal(userid, &noti_out, p_in))
		this->noti_others( userid,cli_noti_game_opt_cmd,&noti_out);
	
	return true;
}

void Cgame_item::noti_all( uint32_t cmdid, Cmessage *c_in, uint32_t except_uid)
{
	std::map<uint32_t , Cgame_user_info>::iterator it;
	for (it=this->user_map.begin();it!=this->user_map.end();++it ){
		uint32_t obj_userid=it->second.userid;
		Csprite *obj_p=g_sprite_map->get_sprite(obj_userid);
		if(obj_p && obj_p->id != except_uid){
			obj_p->send_succ_server_noti(cmdid,c_in );
		}
	}
}

void Cgame_item::do_timer_func(uint32_t* data)
{
	if (this->gameid == 4) {
		bool all_not_open = true;
		uint32_t userid;
		TimeVal cur_time(now);
		cur_time.s_timeadd(1000);
		cur_time.s_timeadd(10);
		std::map<uint32_t , Cgame_user_info>::iterator it;
		for (it = this->user_map.begin(); it != this->user_map.end(); it ++) {
			if (it->second.state == 3) {
				all_not_open = false;
				/*
				uint32_t t1 = (it->second.open_v - NV * 1000) / NG;
				uint32_t s1 = (it->second.open_v * t1 - NG * t1 * t1 / 2) / 1000;
				uint32_t ms_need = (2176000 - it->second.open_y - s1) / NV + t1;
				KDEBUG_LOG(it->second.userid, "TIMER UP \t[%u %u %u]", ms_need, t1, it->second.open_v);
				*/
				uint32_t ms_need = (2176000 - it->second.open_y) / NV;

				TimeVal arrive_time = it->second.open_time;
				arrive_time.ms_timeadd(ms_need);
				if (arrive_time < cur_time) {
					cur_time = arrive_time;
					userid = it->second.userid;
				}
			}
		}
		if (all_not_open) {
			this->game_end(this->start_userid, 2, false, false);
		} else {
			this->game_end(userid, 1, false, false);
		}
	}
}

bool Cgame_item::get_game_user(game_user_t& game_user, uint32_t userid)
{
	if (userid == 0) userid = this->start_userid;
	if (this->start_userid != userid)
		return false;
	
	game_user.src_userid = userid;
	game_user.gameid = this->gameid;
	std::map<uint32_t, Cgame_user_info>::iterator it;
	for (it = this->user_map.begin(); it != this->user_map.end(); it ++) {
		if (it->second.userid != userid)
			game_user.userlist.push_back(it->second.userid);
	}
	return true;
}


void Cgame_item::noti_others( uint32_t userid, uint32_t cmdid, Cmessage *c_in)
{
	std::map<uint32_t , Cgame_user_info>::iterator it;
	for (it=this->user_map.begin();it!=this->user_map.end();++it ){
		if (it->second.userid==userid ){
			continue;
		}else{
			uint32_t obj_userid=it->second.userid;
			Csprite *obj_p=g_sprite_map->get_sprite(obj_userid);
			if(obj_p ){
				obj_p->send_succ_server_noti(cmdid,c_in );
			}
		}
	}

}

/*
 * 功能：小游戏结束的结算
 * 		del_game_flag == true: 用户离开，不给积分，删除缓存信息
 * 		del_game_flag == false: 小游戏结束，先到实际判定胜负，作为结算标准
 */
bool Cgame_item::game_end(uint32_t userid, uint32_t win_flag, bool del_game_flag, bool except_self)
{
	std::map<uint32_t, Cgame_user_info>::iterator it=this->user_map.find(userid);
	if(it!=this->user_map.end()){//找到用户
		it->second.opt_count++;
	}else{
		return false;
	}

	// 检查是否在规定的时间完成，如果太快则不给经验
	bool valid_end = true;
	uint32_t min_game_time = g_game_conf_map.get_min_game_time(this->gameid);
	if (this->start_time + min_game_time > time(NULL))
		valid_end = false;
	//通知所有人
	cli_noti_game_end_out noti_out;
	
	if (!del_game_flag){
		uint32_t other_user_win_flag=(win_flag + 1) % 2;
		if (win_flag >= 2) {
			win_flag = 2;
			other_user_win_flag = 2;
		}

		for (it=this->user_map.begin();it!=this->user_map.end();++it ){
			user_game_win_t item;
			item.obj_userid=(it->second.userid) ;
			if(item.obj_userid ==userid){//是自己
				item.win_flag=win_flag;
			}else{
				item.win_flag=other_user_win_flag;
			}

			//if (this->game_stat == GAME_OPT && valid_end) {
			if (this->game_stat == GAME_OPT) {
				Csprite* obj_p=g_sprite_map->get_sprite(item.obj_userid);
				if (item.win_flag < 2) {
					uint32_t day_oltime = obj_p->get_day_oltime();
					// 计算小游戏积分
					int change_val = g_game_conf_map.get_game_point(this->gameid, day_oltime, item.win_flag); 
					KDEBUG_LOG(item.obj_userid, "GAME PT\t[%u %u %d]", this->gameid, item.win_flag, change_val);
					// change cache and sync_db
					obj_p->spec_item_list.change_count(spec_item_game_point, change_val);
					item.get_point = change_val;
				}

				obj_p->game_stat_map.add_game_stat(this->gameid, item.win_flag);
			}

			noti_out.user_win_list.push_back(item);
		}

		this->noti_all(cli_noti_game_end_cmd, &noti_out, except_self ? userid : 0);
	}

	//清空游戏
	for (it=this->user_map.begin();it!=this->user_map.end();++it ){
		uint32_t obj_userid=(it->second.userid) ;
		if (del_game_flag){//删除游戏时 要清空game_map_id
			Csprite* obj_p=g_sprite_map->get_sprite(obj_userid );		
			KDEBUG_LOG(obj_p->id,"del: obj_p->game_map_id:%u ",obj_p->game_map_id);
			obj_p->game_map_id=0;
		}

		it->second.can_start=false;
		it->second.opt_count=0;
	}

	this->game_stat=GAME_START;	

	return true;
}

//游戏结束
bool Cgame_map::game_end( uint32_t game_map_id, uint32_t userid,uint32_t win_flag, bool del_game_flag )
{
	bool ret = false;
	std::map <uint32_t, Cgame_item >::iterator it=this->game_item_map.find(game_map_id);
	if(it!=this->game_item_map.end()){
		ret=it->second.game_end(userid, win_flag, del_game_flag);
		if (ret==true && del_game_flag){//处理好了
			this->game_item_map.erase(game_map_id);	
		}
	}

	return ret;
}

bool Cgame_map::get_game_user(uint32_t game_map_id, game_user_t& game_user, uint32_t userid)
{
	std::map<uint32_t, Cgame_item>::iterator it = this->game_item_map.find(game_map_id);
	if (it != this->game_item_map.end()) {
		return it->second.get_game_user(game_user, userid);
	}

	return false;
}

bool Cgame_map::game_user_left( uint32_t game_map_id, uint32_t userid)
{
	std::map <uint32_t, Cgame_item >::iterator it;
	it=this->game_item_map.find(game_map_id);
	if(it!=this->game_item_map.end()){
		cli_noti_game_user_left_game_out noti_out;
		noti_out.obj_userid=userid;
		it->second.noti_others(userid, cli_noti_game_user_left_game_cmd,&noti_out );
		this->game_end(game_map_id,userid,0,true );
		return true;
	}
	return false;
}

void Cgame_map::noti_all(uint32_t game_map_id, uint32_t cmdid, Cmessage* c_in, uint32_t except_uid)
{
	std::map <uint32_t, Cgame_item >::iterator it;
	it=this->game_item_map.find(game_map_id);
	if(it!=this->game_item_map.end()){
		it->second.noti_all(cmdid, c_in, except_uid);
	}
}

// 通知场景中其他人有人进入小游戏
void Cgame_map::noti_map_game_stat(uint32_t game_map_id, uint32_t state)
{
	std::map <uint32_t, Cgame_item >::iterator it;
	it=this->game_item_map.find(game_map_id);
	if(it!=this->game_item_map.end()){
		cli_noti_game_user_out noti_out;
		it->second.get_game_user(noti_out.game_user, 0);
		noti_out.state = state;
		Csprite* obj_p = g_sprite_map->get_sprite(noti_out.game_user.src_userid);
		if (obj_p)
			g_sprite_map->noti_to_map_others(obj_p, cli_noti_game_user_cmd, &noti_out);
	}
}

// 用户准备好了，可以开始游戏
bool Cgame_map::set_user_can_start(uint32_t game_map_id, uint32_t userid)
{
	std::map<uint32_t, Cgame_item >::iterator it=this->game_item_map.find(game_map_id);
	if(it!=this->game_item_map.end()){
		return it->second.set_user_can_start( userid );
	}
	return false;
}

void Cgame_map::do_timer_func(uint32_t game_map_id, uint32_t* data)
{
	std::map<uint32_t, Cgame_item >::iterator it=this->game_item_map.find(game_map_id);
	if(it!=this->game_item_map.end()){
		it->second.do_timer_func(data);
	}
}

// 根据某些特定的值生成返回给用户的验证session
void Cgame_session::make_session(char* session, uint32_t v1, uint32_t v2, uint32_t v3)
{
	stru_session_md5 md5;
	md5.v1 = v1;
	md5.v2 = v2;
	md5.v3 = v3;
	md5.time = time(NULL);
	char buf1[16], buf2[64];
	des_encrypt_n(MDKEY, &md5, buf1, 2);
    hex2str (buf1, 16, buf2);
	memcpy(session, buf2, 32);
}

// 校验session
bool Cgame_session::check_session(char* session, uint32_t v1, uint32_t v2, uint32_t v3)
{
	char desbuf[16];
	str2hex(session, 32, desbuf);

	struct stru_session_md5 md5;
    des_decrypt_n(MDKEY, desbuf, (char *)&md5, 2);
	return v1 == md5.v1 && v2 == md5.v2 && v3 == md5.v3;
}

// 小游戏对战等级与积分的对应关系 积分=对战等级的3次方 * 10
bool Cgame_conf_map::init()
{
	reward.clear();
	for (uint32_t loop = 1; loop <= max_game_level; loop ++) {
		game_level_point[loop - 1] = loop * loop * loop * 10;
	}
	return true;
}

// 根据积分计算对战等级;
uint32_t Cgame_conf_map::calc_game_level(uint32_t game_point)
{
	if (game_point == 0) return 1;

	for (uint32_t loop = 0; loop < max_game_level; loop ++) {
		if (game_level_point[loop] > game_point)
			return loop;
	}
	return max_game_level;
}

// 小游戏配置信息
bool Cgame_conf_map::add_game(uint32_t gameid, int32_t winpt, int32_t losept, uint32_t min_time)
{
	std::map<uint32_t, game_reward>::iterator it = this->reward.find(gameid);
	if (it != this->reward.end()) {
		it->second.winpt = winpt;
		it->second.losept = -losept;
		it->second.min_time = min_time;
	} else {
		game_reward tmp_game_reward;
		tmp_game_reward.winpt = winpt;
		tmp_game_reward.losept = -losept;
		tmp_game_reward.min_time = min_time;
		this->reward.insert(std::pair<uint32_t, game_reward>(gameid, tmp_game_reward));
	}
	return true;
}

// 计算小游戏的输赢积分
int32_t Cgame_conf_map::get_game_point(uint32_t gameid, uint32_t day_oltime, bool win)
{
	if (day_oltime > GAME_MAX_SEC) // 超过5小时，无收益
		return 0;

	std::map<uint32_t, game_reward>::iterator it = this->reward.find(gameid);
	if (it == this->reward.end())
		return 0;

	int pt = win ? it->second.winpt : it->second.losept;
	if (day_oltime > GAME_NORMAL_SEC) // 超过2小时，收益减半
		pt = (pt + 1) / 2;

	return pt;
}

// 获取小游戏的最短游戏时间，如果是非法小游戏，设定为2分钟
uint32_t Cgame_conf_map::get_min_game_time(uint32_t gameid)
{
	std::map<uint32_t, game_reward>::iterator it = this->reward.find(gameid);
	if (it == this->reward.end())
		return default_min_game_time;
	return it->second.min_time;
}
