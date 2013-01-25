/*
 * =========================================================================
 *
 *        Filename: market.cpp
 *
 *        Version:  1.0
 *        Created:  2012-02-16 11:34:01
 *        Description: 交易相关 
 *
 *        Author:  francisco (francisco@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */
#include  "market.h"
#include  <time.h>
#include  "proto.h"
#include  "util.h"
#include  "./proto/mole2_home_enum.h"
#include  "./proto/mole2_home.h"
/*从房间里删除用户*/
int Croom::del_user_from_room(uint32_t uid)
{
	std::set<uint32_t>::iterator ie=enter_users.find(uid);
	if( ie != enter_users.end() ){
		enter_users.erase(ie);		
	}
	//std::map<uint32_t,uint32_t>::iterator iw=waiting_users.find(uid);
	//iw=waiting_users.find(uid);
	//if( iw != waiting_users.end() ){
		//waiting_users.erase(iw);		
	//}
	for( uint8_t loop=0 ; loop<MAX_SHOP_CNT ; loop++ ){
		if( shops[loop].state>shop_init && shops[loop].uid == uid ){
			close_shop(loop);
			break;
		}
	}
	DEBUG_LOG("del a user %u",uid);
	return 0;
}

/*在market里登记用户并将用户添加到房间的等待队列中*/
int Cmarket::add_user_to_market(uint32_t uid,uint32_t roomid)
{
	del_user_from_market(uid);
	player_t tmp;	
	tmp.roomid=roomid;
	user_info.insert(std::make_pair(uid,tmp));
	//return rooms[roomid].add_waiting_user(uid);
	return 0;
}

/*删除用户所有信息*/
int Cmarket::del_user_from_market(uint32_t uid)
{
	std::map<uint32_t,player_t>::iterator it=user_info.find(uid);
	if(it != user_info.end()){
		rooms[it->second.roomid].del_user_from_room(uid);
		user_info.erase(it);
	}
	return 0;
}
/*进入房间即从等待队列中添加到已经进入队列*/
int Cmarket::cd_room(uint32_t uid)
{
	std::map<uint32_t,player_t>::iterator it=user_info.find(uid);
	if(it != user_info.end()){
		DEBUG_LOG("cd room %u %u",uid,it->second.roomid);
		//rooms[it->second.roomid].waiting_users.erase(uid);
		rooms[it->second.roomid].add_enter_user(uid);
	}
	return 0;
}

////检查等待队列中超时2秒没有进入的用户
//void Cmarket::check_out_time()
//{
	//for( uint8_t room_loop=0 ; room_loop<MAX_ROOM_CNT ; room_loop++ ){
		//std::map<uint32_t,uint32_t>::iterator it= this->rooms[room_loop].waiting_users.begin() ;
		//for( ; it != this->rooms[room_loop].waiting_users.end();){
			//if(it->second+2 >= time(NULL) ){
				//DEBUG_LOG("user time out %u",it->first);
				//std::map<uint32_t,uint32_t>::iterator tmp=it;
				//it++;
				//this->rooms[room_loop].waiting_users.erase(tmp);
			//}else{
				//it++;
			//}
		//}		
	//}
//}
