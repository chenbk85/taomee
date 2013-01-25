/*
 * =========================================================================
 *
 *        Filename: market.h
 *
 *        Version:  1.0
 *        Created:  2012-02-16 08:45:10
 *        Description:  
 *
 *        Author:  francisco (francisco@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */
#ifndef  MARKET_H
#define  MARKET_H
#include <libtaomee++/proto/proto_base.h>
#include  <set>
#include  <map>

typedef struct player{
	uint32_t xiaomee;
	uint32_t roomid;
}player_t;

enum{
	shop_init=0,
	shop_register=1,
	shop_open=2,
};

typedef struct Shop {
	uint8_t  state;
	uint32_t uid;
	char shopname[16];
}Shop_t;

#define     MAX_ROOM_USER_CNT 25 
#define     MAX_SHOP_CNT  6 
class Croom {
	public:
		std::set<uint32_t> enter_users;//已经进入的队列
		//std::map<uint32_t,uint32_t> waiting_users;//等待队列
		Shop shops[MAX_SHOP_CNT];
		Croom(){memset(shops,0,sizeof(shops));}
		~Croom(){};
		inline int add_enter_user(uint32_t uid);
		//inline int add_waiting_user(uint32_t uid);
		inline int del_user_from_room(uint32_t uid);
		inline int register_shop(uint32_t uid, uint32_t sid);
		inline int close_shop(uint32_t sid);
		inline int hault_shop(uint32_t sid);
		inline int open_shop(uint32_t sid);
		inline bool check_shop_valid(uint32_t sid);
		inline bool is_full();
};


#define    	MAX_ROOM_CNT	1000 
class Cmarket {
	public:
		Croom rooms[MAX_ROOM_CNT];
		std::map<uint32_t,player_t> user_info;
		uint32_t rooms_cnt;
		Cmarket():rooms_cnt(MAX_ROOM_CNT){}
		int add_user_to_market(uint32_t uid,uint32_t roomid);
		int del_user_from_market(uint32_t uid);
		int enter_room(uint32_t uid);
		int cd_room(uint32_t uid);
		//void check_out_time();
		~Cmarket(){};

};

inline bool Croom::is_full()
{
	//return (waiting_users.size()+enter_users.size())>=MAX_ROOM_USER_CNT;
	return (enter_users.size())>=MAX_ROOM_USER_CNT;
}

inline bool Croom::check_shop_valid(uint32_t sid)
{
	return shops[sid].state==shop_init;
}

inline int Croom::register_shop(uint32_t uid, uint32_t sid)
{
	shops[sid].state=shop_register;
	shops[sid].uid=uid;
	return 0;
}

inline int Croom::hault_shop(uint32_t sid)
{
	shops[sid].state=shop_register;
	return 0;
}

inline int Croom::open_shop(uint32_t sid)
{
	shops[sid].state=shop_open;
	return 0;
}

inline int Croom::close_shop(uint32_t sid)
{
	memset(&shops[sid],0,sizeof(Shop_t));
	return 0;
}

inline int Croom::add_enter_user(uint32_t uid)
{
	DEBUG_LOG("add enter user %u",uid);
	enter_users.insert(uid);		
	return 0;
}

/*inline int Croom::add_waiting_user(uint32_t uid)*/
/*{*/
/*DEBUG_LOG("add waiting user %u",uid);*/
/*waiting_users.insert(std::make_pair(uid,(uint32_t)(time(NULL))));		*/
/*return 0;*/
/*}*/

#endif  


