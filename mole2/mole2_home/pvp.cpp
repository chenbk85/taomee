/*
 * =========================================================================
 *
 *        Filename: pvp.cpp
 *
 *        Version:  1.0
 *        Created:  2011-10-18 18:59:06
 *        Description:  
 *
 *        Author:  francisco (francisco@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */
#include  "pvp.h"
#include  <time.h>
#include  <libtaomee++/proto/proto_util.h>
#include  "proto.h"
#include  "util.h"
#include  "./proto/mole2_home_enum.h"
#include  "./proto/mole2_home.h"

void Cpvp::add_user_to_user_info_map(userid_t uid, char nick[16], uint32_t onlineid)
{
	stru_pre_user_info info;

	info.opt_level = 0;
	info.onlineid = onlineid;

	user_info_map[uid] = info;  
}

void Cpvp::send_msg_to_all_users(Cmessage* out, mapid_t homeid, uint32_t opid)
{
	DEBUG_LOG("Cpvp::send_msg_to_all_users(homeid = %lu, opid = %u)", homeid, opid);

	std::map<mapid_t,Cgame_item>::iterator map_it = homeid_game_map.find(homeid);
	Cgame_item& game_item = map_it->second;

	home_proto_t head_pkg; 
	init_pkg_proto(&(head_pkg) ,0,61002,0, homeid , opid);
	std::set<uint32_t>::iterator set_it;	

	for(set_it=game_item.userid_set.begin();set_it!=game_item.userid_set.end();++set_it){
			
		uint32_t userid=*set_it;
	
		DEBUG_LOG("START  SEND PKG: userid=%u ",userid);
		std::map<uint32_t, stru_pre_user_info>::iterator it;
		it = user_info_map.find(userid);
		if (it != user_info_map.end()) {
			stru_pre_user_info* p_pre_user_info = &(it->second);
			head_pkg.id = userid;
			head_pkg.onlineid = p_pre_user_info->onlineid;
			if ( all_fds[p_pre_user_info->onlineid]){
				send_msg_to_client(all_fds[p_pre_user_info->onlineid], (char*)&head_pkg, out);
			}
			DEBUG_LOG("START  SEND PKG   " );
		}
	}
}

void Cpvp::send_msg_to_a_user(Cmessage* msg,mapid_t homeid, userid_t uid, uint32_t opid)
{
	DEBUG_LOG("Cpvp::send_msg_to_a_user(homeid = %lu, userid = %u, opid = %u)", homeid, uid, opid);

	home_proto_t head_pkg;
	init_pkg_proto(&(head_pkg) ,0,61002,0, homeid , opid);

	DEBUG_LOG("START  SEND PKG: userid=%u ",uid);
	std::map<uint32_t, stru_pre_user_info>::iterator it = user_info_map.find(uid);
	if (it != user_info_map.end()) {
		stru_pre_user_info* p_pre_user_info = &(it->second);
		head_pkg.id= uid;
		head_pkg.onlineid= p_pre_user_info->onlineid;
		if ( all_fds[p_pre_user_info->onlineid]){
			send_msg_to_client(all_fds[p_pre_user_info->onlineid], (char*)&head_pkg, msg);
		}
		DEBUG_LOG("START  SEND PKG   " );
	}
}

void Cpvp::send_noti_pvp_winner(mapid_t homeid, userid_t winner, userid_t loser)
{
	//loser是玩家本身，winner是玩家对手
	std::map<mapid_t,Cgame_item>::iterator it = homeid_game_map.find(homeid);
	if (it == homeid_game_map.end()) {
		return;
	}

	std::map<userid_t, stru_user_score_info>::iterator user_it;
	std::map<userid_t, stru_user_score_info>& user_score_map =
		it->second.user_score_map;

	home_game_noti_pvp_winner_out winner_out;
	winner_out.winner = winner;
	winner_out.loser = loser;

	pvp_score_t score;
	//print_userid_sets(homeid);
	DEBUG_LOG("user_score_map.size %lu", user_score_map.size());
	for (user_it = it->second.user_score_map.begin();
			user_it != it->second.user_score_map.end(); user_it++) {
		if (it->second.userid_set.find(user_it->first) == it->second.userid_set.end()
				&& it->second.start_time == 0) {
			continue;
		}
		score.userid = user_it->first;
		memcpy(score.nick, user_it->second.nick, 16);
		score.scores = user_it->second.score;
		score.is_alive = user_it->second.is_alive;

		winner_out.score_list.push_back(score);
	}

	send_msg_to_all_users(&winner_out, homeid, home_game_noti_pvp_winner_cmd);
}

void Cpvp::print_userid_sets(mapid_t homeid)
{
	DEBUG_LOG("==========================================");

	std::map<mapid_t, Cgame_item>::iterator it;

	it = homeid_game_map.find(homeid);
	if (it == homeid_game_map.end()) {
		DEBUG_LOG("mapid %lu not found", homeid);
		DEBUG_LOG("==========================================");

		return;
	}

	Cgame_item& item = homeid_game_map[homeid];
	std::map<userid_t, stru_user_score_info>::iterator score_it;
	DEBUG_LOG("user_score_map");
	for (score_it = item.user_score_map.begin(); score_it != item.user_score_map.end(); score_it++) {
		DEBUG_LOG("%u ", score_it->first);
	}
	DEBUG_LOG("userid_set");
	std::set<userid_t>::iterator set_it;
	for (set_it = item.userid_set.begin(); set_it != item.userid_set.end(); set_it++) {
		DEBUG_LOG("%u ", *set_it);
	}
	DEBUG_LOG("==========================================");
}

void Cpvp::start_new_game(Cgame_item& game_item)
{
	DEBUG_LOG("Cpvp::start_new_game() called");

	if (game_item.start_time != 0) {
		return;
	}

	games[game_item.opt_level] = 0;

	game_item.start_time=time(NULL);
	game_item.end_time=game_item.start_time + total_battle_timeout;

	end_time_homeid_map.insert(std::make_pair(game_item.start_time + battle_timeout_sync,game_item.homeid) );

	print_userid_sets(game_item.homeid);

	std::map<userid_t, stru_user_score_info>::iterator it;
	for (it = game_item.user_score_map.begin(); it != game_item.user_score_map.end();) {
		if (game_item.userid_set.find(it->first) == game_item.userid_set.end()) {
			game_item.user_score_map.erase(it++);
		} else {
			it++;
		}
	}
	
	//通知开始
	home_game_noti_start_out out;
	out.left_time=total_battle_timeout;
	send_msg_to_all_users(&out, game_item.homeid,home_game_noti_start_cmd); 
}

void Cpvp::del_user(mapid_t homeid, userid_t userid, userid_t rival_winner)
{
	DEBUG_LOG("Cpvp::del_user(userid = %u, rival = %u)", userid, rival_winner);

	std::map<mapid_t,Cgame_item>::iterator it = homeid_game_map.find(homeid);
	if (it != homeid_game_map.end()) {
		std::set<userid_t>::iterator user_set_it = it->second.userid_set.find(userid);
		if (user_set_it == it->second.userid_set.end()) {
			//此人已经离开
			return;
		}

		std::map<userid_t, stru_user_score_info>::iterator user_it;
		std::map<userid_t, stru_user_score_info>& user_score_map =
			it->second.user_score_map;
		user_it = user_score_map.find(userid);
		if (user_it != user_score_map.end()) {
			user_it->second.is_alive = 0;
		}

		if (it->second.start_time > 0) {
			//游戏已经启动，此时del_user()是因为有人打输了或者退出了
			if (rival_winner != 0) {
				//打输退出
				DEBUG_LOG("Cpvp::del_user(), user %u loses", userid);
				print_userid_sets(homeid);

				user_it = user_score_map.find(rival_winner);
				if (user_it != user_score_map.end()) {
					//胜者加一分
					user_it->second.score++;
				}

				send_noti_pvp_winner(homeid, rival_winner, userid);
			} else {
				//用户自行离开
				DEBUG_LOG("Cpvp::del_user(), user %u exits", userid);
				print_userid_sets(homeid);
				send_noti_pvp_winner(homeid, rival_winner, userid);
			}

			it->second.userid_set.erase(userid);
			user_info_map.erase(userid);

			//如果剩下最后一个人直接通知打赢了
			if (it->second.userid_set.size() == 1) {
				userid_t last_user = *it->second.userid_set.begin();
				DEBUG_LOG("Cpvp::del_user(), last stand: %u", last_user);
				print_userid_sets(homeid);

				user_it = user_score_map.find(last_user);
				user_it->second.score += 5;
				send_noti_pvp_winner(homeid, 0, last_user);

				home_game_noti_info_out out;
				out.is_time_out = 0;
				out.is_win = 1;
				send_msg_to_a_user(&out, homeid, last_user, home_game_noti_info_cmd);

				homeid_game_map.erase(homeid);
			}
		} else {
			//游戏未启动，此时del_user()是因为有人未等到游戏开始就退出了
			DEBUG_LOG("Cpvp::del_user(), %u left the game before start", userid);
			print_userid_sets(homeid);
			it->second.userid_set.erase(userid);
			it->second.user_score_map.erase(userid);
			send_noti_pvp_winner(homeid, 0, 0);
			user_info_map.erase(userid);

			if (it->second.userid_set.size() == 1) {
				home_game_noti_left_time_out out;
				out.time_out_type = 1;
				out.left_time = 0xffffffff;
				send_msg_to_a_user(&out, homeid, *it->second.userid_set.begin(), home_game_noti_left_time_cmd);
			}
		}
	}
}

void Cpvp::cd_map(mapid_t homeid, uint32_t userid)
{
	DEBUG_LOG("Cpvp::cd_map(homeid = %lu,userid = %u) called", homeid, userid);
	std::map<mapid_t, Cgame_item>::iterator it;
	it = homeid_game_map.find(homeid);
	if (it != homeid_game_map.end()) {
		Cgame_item& item = it->second;
		if (item.user_score_map.find(userid) != item.user_score_map.end()) {
			item.userid_set.insert(userid);

			std::map<userid_t, stru_user_score_info>::iterator user_it = item.user_score_map.find(userid);
			if (user_it != item.user_score_map.end()) {
				DEBUG_LOG("%u is_alive", user_it->first);
				user_it->second.is_alive = 1;
			}
			send_noti_pvp_winner(homeid, 0, 0);
			
			home_game_noti_left_time_out out;
			out.time_out_type = 1;
			out.left_time = no_entry_timeout;
			if (item.userid_set.size() == 1) {
				out.left_time = 0xffffffff; //第一个人一直等待，无超时
				item.add_user_time_out_and_start_game_time =  0xffffffff;
			} else {
				uint32_t timeout = time(NULL) + no_entry_timeout;
				item.add_user_time_out_and_start_game_time = timeout;
				no_entry_time_homeid_map.insert(std::make_pair(timeout, homeid));
			}

			send_msg_to_all_users(&out, homeid, home_game_noti_left_time_cmd);

			print_userid_sets(homeid);

			size_t size = item.userid_set.size();
			if (size == game_user_count) {
				start_new_game(item);		
			} else if (size < game_user_count) {
				if (size < item.user_score_map.size()) {
					//有人获取了mapid但并没有进入地图 
				}
			} else {
				DEBUG_LOG("Cpvp::cd_map() too many users in userid_set.");
			}
		}
	}
}

void Cpvp::check_time_outs()
{
	uint32_t now = time(NULL);
	std::multimap<uint32_t, mapid_t>::iterator it;
	while (true) {
		//得到最久的游戏
		it= this->end_time_homeid_map.begin();
		if(it==this->end_time_homeid_map.end()) {
			break;
		}

		uint32_t end_time=it->first;
		if (end_time<=now){//超时了
			mapid_t	homeid=it->second ;
			std::map<mapid_t,Cgame_item>::iterator home_it;
 			home_it=this->homeid_game_map.find( homeid);

			if (home_it !=this->homeid_game_map.end() 
					){//是存在超时游戏
				this->do_battle_time_out_game(homeid);
			}
			end_time_homeid_map.erase(it);
		}else{//还没有超时
			break;
		}
	}

	//------------------------------------- 
	while (true) {
		//得到最久的游戏
		it= this->no_entry_time_homeid_map.begin();
		if(it==this->no_entry_time_homeid_map.end()) {
			break;
		}

		uint32_t end_time=it->first;
		if (end_time<=now){//超时了
			mapid_t	homeid=it->second ;
			std::map<mapid_t,Cgame_item>::iterator home_it;
 			home_it=this->homeid_game_map.find( homeid);
			if ( home_it!=this->homeid_game_map.end() 
					&& end_time==home_it->second.add_user_time_out_and_start_game_time //时间对的
					){//是存在超时游戏
				this->do_no_entry_time_out_game(homeid);
			}else{
				DEBUG_LOG("add_user_time_out_and_start_game_time homeid=%lu, pass:time= %u ", 
						homeid,end_time );
			}
			no_entry_time_homeid_map.erase(it);
		}else{//还没有超时
			break;
		}
	}

	while (true) {
		//得到最久的游戏
		it= this->wait_time_out_homeid_map.begin();
		if(it==this->wait_time_out_homeid_map.end()) {
			break;
		}

		uint32_t end_time=it->first;
		if (end_time<=now){//超时了
			mapid_t	homeid=it->second ;
			if ( this->homeid_game_map.find( homeid) !=this->homeid_game_map.end()){//是存在超时游戏
				this->do_wait_time_out_game(homeid);
			}
			wait_time_out_homeid_map.erase(it);
		}else{//还没有超时
			break;
		}
	}
}

void Cpvp::do_battle_time_out_game( mapid_t homeid)
{
	DEBUG_LOG("Cpvp::do_battle_time_out_game(%lu) called", homeid);
	std::map<mapid_t, Cgame_item>::iterator it = homeid_game_map.find(homeid);
	if (it == homeid_game_map.end()) {
		return;
	}
	Cgame_item& item = it->second;
	
	uint32_t now = time(NULL);
	if (item.end_time <= now) {
		// total_battle_timeout 超时
		home_game_noti_info_out out;
		out.is_time_out = 1;
		out.is_win = 0;
		send_msg_to_all_users(&out, homeid, home_game_noti_info_cmd);
		homeid_game_map.erase(homeid);
	} else {
		// battle_timeout_sync同步定时器超时
		home_game_noti_left_time_out out;
		out.time_out_type = 2;
		out.left_time = item.end_time - now;
		send_msg_to_all_users(&out, homeid,home_game_noti_left_time_cmd); 
		end_time_homeid_map.insert(std::make_pair(now + battle_timeout_sync,homeid));
	}
}

bool Cpvp::is_in(uint32_t uid)
{
	return (user_info_map.find(uid) != user_info_map.end()) ;
}

void Cpvp::do_no_entry_time_out_game(mapid_t homeid)
{
	DEBUG_LOG("Cpvp::do_no_entry_time_out_game(%lu) called", homeid);

	std::map<mapid_t, Cgame_item>::iterator it = homeid_game_map.find(homeid);
	if (it == homeid_game_map.end()) {
		return;
	}

	Cgame_item& item = it->second;
	if (item.userid_set.size() > 1) {
			start_new_game(item);
	} else {
		DEBUG_LOG("Cpvp::do_no_entry_time_out_game(): userid_set.size() == 1");
	}
}

void Cpvp::do_wait_time_out_game(mapid_t homeid)
{
	DEBUG_LOG("Cpvp::do_wait_time_out_game(%lu) called", homeid);

	std::map<mapid_t, Cgame_item>::iterator it = homeid_game_map.find(homeid);
	if (it == homeid_game_map.end()) {
		return;
	}

	Cgame_item& item = it->second;
	if (item.userid_set.size() > 1) {
			start_new_game(item);
	} else {
		DEBUG_LOG("Cpvp::do_wait_time_out_game(): userid_set.size() == 1");
	}
}

void Cpvp::get_map_id(userid_t uid, char nick[16], uint32_t level, uint32_t onlineid,uint32_t pvp_type)
{
	DEBUG_LOG("Cpvp::get_map_id(%u)", uid);

	uint32_t opt_level = level / level_divider;
	if (opt_level>=opt_level_count) return;

	//加入
	add_user_to_user_info_map(uid, nick, onlineid);

	if (games[opt_level] == 0) {
		this->next_dupid++;
		mapid_t temp = ((next_dupid & 0xFFFFFF) | 0xFE000000);
		temp |= pvp_type<<23;
		temp = (temp << 32) | 41221 ;//41221 pvp map

		games[opt_level] = temp;

		Cgame_item item;
		item.start_time = 0; 
		item.end_time = 0;
		item.homeid = temp;
		item.opt_level = opt_level;
		homeid_game_map[games[opt_level]] = item;
	}

	std::map<mapid_t, Cgame_item>::iterator it;

	it = homeid_game_map.find(games[opt_level]);
	if (it == homeid_game_map.end()) {
		DEBUG_LOG("homeid(%lu) is not in homeid_game_map", games[opt_level]);
		return;
	}

	Cgame_item* p_item = &it->second;

	stru_user_score_info user_info;
	memcpy(user_info.nick, nick, 16);
	user_info.score = 0;
	user_info.is_alive = 0;
	p_item->user_score_map[uid] = user_info;
	
	//print_userid_sets(games[opt_level]);

	size_t size = p_item->user_score_map.size();
	if (size == game_user_count) {
		p_item->time_out_and_start_game_time=time(NULL) + wait_timeout;
		wait_time_out_homeid_map.insert(std::make_pair(p_item->time_out_and_start_game_time , p_item->homeid));
		games[opt_level]=0;
	} else if (size < game_user_count) {

	} else {
		DEBUG_LOG("Cpvp::get_map_id(): too many users in user_score_map.");
	}

	home_game_get_map_id_out cli_out;
	cli_out.mapid = p_item->homeid;
	send_msg_to_a_user(&cli_out, p_item->homeid, uid, home_game_get_map_id_cmd);

	return;
}

