/*
 *
 * =====================================================================================
 *
 *       Filename:  battle_room.cpp
 *
 *
 *        Version:  1.0
 *        Created:  03/03/11 22:18:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#include <libtaomee++/inet/pdumanip.hpp>
extern "C" {
#include <libtaomee/log.h>
}
using namespace taomee;
#include "battle_room.hpp"
#include "singleton.hpp"
#include "online.hpp"

//int player_city_team(uint32_t uid, uint32_t role_time)
//{
//	static int team[11] = { 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1};
//
//	time_t now_time = get_now_tv()->tv_sec;
//
//	struct tm * now_tm = localtime(&now_time);
//
//	uint32_t t_value = (now_tm->tm_mday +  role_time) % 11; 
//
//	return team[t_value];
//}

enum {
	max_pkg_size = 8192,
};

BattleRoom::BattleRoom()
{
	init();
}


BattleRoom::~BattleRoom()
{

}

void BattleRoom::SetPVPInfo(uint32_t battle,
						    uint32_t room,
							uint32_t lv,
							uint32_t mode)
{
	BattleId = battle;
	RoomId = room;
	CreatorLv = lv;
	PVPMode = mode;
}


void BattleRoom::SetPVEInfo(uint32_t battle,
					        uint32_t room,
							uint32_t stage,
							uint32_t diff)
{
	BattleId = battle;
	RoomId = room;
	StageId = stage;
	Diffcult = diff;
}


void BattleRoom::SetFirstUser(uint32_t uid,
						      uint32_t user_tm,
							  uint32_t role_type,
							  char * name)
{
	FirstUser.userid = uid;
	FirstUser.role_tm = user_tm;
	FirstUser.role_type = role_type;
	memcpy(FirstUser.nick, name, 16);
//	DEBUG_LOG("SET FIRST USER INFO %u %u %s", uid, user_tm, name);
}

void BattleRoom::SetSecondUser(uint32_t uid,
				               uint32_t user_tm,
							   uint32_t role_type,
							   char * name)
{
	SecondUser.userid = uid;
	SecondUser.role_tm = user_tm;
	SecondUser.role_type = role_type;
	memcpy(SecondUser.nick, name, 16);
//	DEBUG_LOG("SET FIRST USER INFO %u %u %s", uid, user_tm, name);
}

void BattleRoom::PackPVPInfo(void * buf, int & idx)
{
	pack_h(buf, BattleId, idx);
	pack_h(buf, RoomId, idx);
	pack_h(buf, Status, idx);
	pack_h(buf, FirstUser.userid, idx);
	pack_h(buf, FirstUser.role_tm, idx);
	pack_h(buf, FirstUser.role_type, idx);
	pack(buf, FirstUser.nick, 16, idx);

//	DEBUG_LOG("PACK FIRST USER INFO %u %u %s", FirstUser.userid, FirstUser.role_tm, FirstUser.nick) ;
	pack_h(buf, SecondUser.userid, idx);
	pack_h(buf, SecondUser.role_tm, idx);
	pack_h(buf, SecondUser.role_type, idx);
	pack(buf, SecondUser.nick, 16, idx);

//	DEBUG_LOG("PACK FIRST USER INFO %u %u %s", SecondUser.userid, SecondUser.role_tm, SecondUser.nick );

}

RoomFactory::RoomFactory()
{
	curMax_ = 20000;
	for (uint32_t i = 0; i < curMax_; i++) {
		BattleRoom * room = new BattleRoom();
		mList_.push_back(room);
	}
   
}

RoomFactory::~RoomFactory()
{
	for (RoomList::iterator it = mList_.begin(); it != mList_.end(); ++it) {
		BattleRoom * room = *it;
		delete room;
	}
	mList_.clear();
}


BattleRoom * 
RoomFactory::CreatePVERoom(uint32_t battle_id,
						   uint32_t room_id,
						   uint32_t stage,
						   uint32_t diff)
{
	if (mList_.empty()) {
		for (uint32_t i = 0; i < 2*curMax_; i++) {
			BattleRoom * room = new BattleRoom();
			mList_.push_back(room);
		}

		curMax_ += 2*curMax_;
	}

	BattleRoom * room = mList_.front();

	mList_.pop_front();

	room->SetPVEInfo(battle_id, room_id, stage, diff);
	return room;
}

BattleRoom *
RoomFactory::CreatePVPRoom(uint32_t battle_id,
		                   uint32_t room_id,
						   uint32_t lv,
						   uint32_t mode)
{
	if (mList_.empty()) {
		for (uint32_t i = 0; i < 2*curMax_; i++) {
			BattleRoom * room = new BattleRoom();
			mList_.push_back(room);
		}
		curMax_ = 2*curMax_;
	}

	BattleRoom * room = mList_.front();

	mList_.pop_front();

	room->SetPVPInfo(battle_id, room_id, lv, mode);
	return room;
}

void RoomFactory::DestroyRoom(BattleRoom * room)
{
	room->init();
	mList_.push_back(room);
}



BattleRoom * CreatePVERoom(uint32_t battle_id,
						   uint32_t room_id,
						   uint32_t stage,
						   uint32_t diff)
{
	RoomFactory & sFactory = singleton_default<RoomFactory>::instance();

	return sFactory.CreatePVERoom(battle_id,
							      room_id,
							      stage,
							      diff);

}

BattleRoom * CreatePVPRoom(uint32_t battle_id,
		                   uint32_t room_id,
						   uint32_t lv,
						   uint32_t mode)
{
	RoomFactory & sFactory = singleton_default<RoomFactory>::instance();

	return sFactory.CreatePVPRoom(battle_id,
							      room_id,
							      lv,
							      mode);
}

void DestroyRoom(BattleRoom * room)
{
	RoomFactory & sFactory = singleton_default<RoomFactory>::instance();

	return sFactory.DestroyRoom(room);

}

RoomManager::RoomManager()
{

}

RoomManager::~RoomManager()
{
	if (!PVPWaitingList_.empty()) {
		for (RoomList::iterator it = PVPWaitingList_.begin();
			   	it != PVPWaitingList_.end(); ++it) {
			BattleRoom * room = *it;

			DestroyRoom(room);
		}

		PVPWaitingList_.clear();
	}

	if (!PVEWaitingList_.empty()) {
		for (RoomList::iterator it = PVEWaitingList_.begin();
				it != PVEWaitingList_.end(); ++it) {
			BattleRoom * room = *it;

			DestroyRoom(room);
		}
		PVEWaitingList_.clear();
	}

	if (!PVPGoingList_.empty()) {
		for (RoomList::iterator it = PVPGoingList_.begin();
				it != PVPGoingList_.end(); ++it) {
			BattleRoom * room = *it;
			DestroyRoom(room);
		}
		PVPGoingList_.clear();
	}

	if (!PVEGoingList_.empty()) {
		for (RoomList::iterator it = PVEGoingList_.begin();
				it != PVEGoingList_.end(); ++it) {
			BattleRoom * room = *it;
			DestroyRoom(room);
		}
		PVEGoingList_.clear();
	}

	if (!PVEReadyList_.empty()) {
		for (RoomList::iterator it = PVEReadyList_.begin();
				it != PVEReadyList_.end(); ++it) {
			BattleRoom * room = *it;
			DestroyRoom(room);
		}
		PVEReadyList_.clear();
	}

	if (!PVPReadyList_.empty()) {
		for (RoomList::iterator it = PVPReadyList_.begin();
				it != PVPReadyList_.end(); ++it) {
			BattleRoom * room = *it;
			DestroyRoom(room);
		}
		PVPReadyList_.clear();
	}
}


void RoomManager::AddPVPRoom(BattleRoom * room)
{
	PVPWaitingList_.push_back(room);

}

void RoomManager::AddPVERoom(BattleRoom * room)
{
	PVEWaitingList_.push_back(room);
	
}

BattleRoom *
RoomManager::FindPVPRoom(uint32_t lv, uint32_t mode)
{
	ClearOutDateRoom();
	RoomList::iterator it = PVPWaitingList_.begin();
	while (it != PVPWaitingList_.end()) {
		if ((*it)->CreatorLv <= (lv + 5) && (*it)->CreatorLv >= (lv -5)
				&& (*it)->PVPMode == mode) {
			BattleRoom * room = *it;
			it = PVPWaitingList_.erase(it);
			room->SetMatched();
			PVPReadyList_.push_back(room);
			return room;
		} else if (mode == pvp_dragon_ship && (*it)->PVPMode == mode) {
			BattleRoom * room = *it;
			it = PVPWaitingList_.erase(it);
			room->SetMatched();
			PVPReadyList_.push_back(room);
			return room;
		}
		++it;
	}	

	return 0;

}

BattleRoom *
RoomManager::FindRedBlueRoom(uint32_t join_lv,  uint32_t pvp_mode)
{
	ClearOutDateRoom();
	RoomList::iterator it = PVPWaitingList_.begin();
	while (it != PVPWaitingList_.end()) {
		if ((*it)->PVPMode == pvp_mode) {
			BattleRoom * room = *it;
			it = PVPWaitingList_.erase(it);
			room->SetMatched();
			PVPReadyList_.push_back(room);
			return room;
		}
		++it;
	}	
	return 0;
}





BattleRoom * 
RoomManager::FindPVERoom(uint32_t stage, uint32_t diff)
{
	ClearOutDateRoom();
	RoomList::iterator it = PVEWaitingList_.begin();
	while (it != PVEWaitingList_.end()) {
		if ((*it)->StageId == stage && (*it)->Diffcult == diff) {
			BattleRoom * room = *it;
			it = PVEWaitingList_.erase(it);
			room->SetMatched();
			PVEReadyList_.push_back(room);
			return room;
		}
		++it;
	}
	return 0;
}

void RoomManager::StartPVPRoom(uint32_t battle, uint32_t room)
{
	RoomList::iterator it = PVPGoingList_.begin();
	while (it != PVPGoingList_.end()) {
		if ((*it)->BattleId == battle && (*it)->RoomId == room) {
			(*it)->Status = going_status;
			return;
		}
		++ it;
	}
}


void RoomManager::StartPVERoom(uint32_t battle, uint32_t room)
{
	RoomList::iterator it = PVEGoingList_.begin();
	while (it != PVEGoingList_.end()) {
		if ((*it)->BattleId == battle && (*it)->RoomId == room) {
			(*it)->Status = going_status;
			return;
		}
		++ it;
	}
}

void RoomManager::DestroyPVERoom(uint32_t battle, uint32_t room)
{
	RoomList::iterator it = PVEGoingList_.begin();
	while (it != PVEGoingList_.end()) {
		if ((*it)->BattleId == battle && (*it)->RoomId == room) {
			BattleRoom * room = *it;
			it = PVEGoingList_.erase(it);
			DestroyRoom(room);
			return;
		}
		++it;
	}

	it = PVEWaitingList_.begin();
	while (it != PVEWaitingList_.end()) {
		if ((*it)->BattleId == battle && (*it)->RoomId == room) {
			BattleRoom * room = *it;
			it = PVEWaitingList_.erase(it);
			DestroyRoom(room);
			return;
		}
		++it;
	}

	it = PVEReadyList_.begin();
	while (it != PVEReadyList_.end()) {
		if ((*it)->BattleId == battle && (*it)->RoomId == room) {
			BattleRoom * room = *it;
			it = PVEReadyList_.erase(it);
			DestroyRoom(room);
			return;
		}
		++it;
	}

}

void RoomManager::DestroyPVPRoom(uint32_t battle, uint32_t room)
{
	RoomList::iterator it = PVPGoingList_.begin();
	while (it != PVPGoingList_.end()) {
		if ((*it)->BattleId == battle && (*it)->RoomId == room) {
			BattleRoom * room = *it;
			it = PVPGoingList_.erase(it);
			DestroyRoom(room);
			return;
		}
		++it;
	}

	it = PVPWaitingList_.begin();
	while (it != PVPWaitingList_.end()) {
		if ((*it)->BattleId == battle && (*it)->RoomId == room) {
			BattleRoom * room = *it;
			it = PVPWaitingList_.erase(it);
			DestroyRoom(room);
			return;
		}
		++it;
	}

	it = PVPReadyList_.begin();
	while (it != PVPReadyList_.end()) {
		if ((*it)->BattleId == battle && (*it)->RoomId == room) {
			BattleRoom * room = *it;
			it = PVPReadyList_.erase(it);
			DestroyRoom(room);
			return;
		}
		++it;
	}
}

void RoomManager::ClearBattleRoom(uint32_t SvrId)
{
	RoomList::iterator it = PVEGoingList_.begin();
	while (it != PVEGoingList_.end()) {
		if ((*it)->BattleId == SvrId) {
			BattleRoom * room = *it;
			it = PVEGoingList_.erase(it);
			DestroyRoom(room);
		}
		++it;
	}

	it = PVEWaitingList_.begin();
	while (it != PVEWaitingList_.end()) {
		if ((*it)->BattleId == SvrId) {
			BattleRoom * room = *it;
			it = PVEWaitingList_.erase(it);
			DestroyRoom(room);
		}
		++it;
	}


	it = PVPGoingList_.begin();
	while (it != PVPGoingList_.end()) {
		if ((*it)->BattleId == SvrId) {
			BattleRoom * room = *it;
			it = PVPGoingList_.erase(it);
			DestroyRoom(room);
		}
		++it;
	}

	it = PVPWaitingList_.begin();
	while (it != PVPWaitingList_.end()) {
		if ((*it)->BattleId == SvrId) {
			BattleRoom * room = *it;
			it = PVPWaitingList_.erase(it);
			DestroyRoom(room);
		}
		++it;
	}

	it = PVPReadyList_.begin();
	while (it != PVPReadyList_.end()) {
		if ((*it)->BattleId == SvrId) {
			BattleRoom * room = *it;
			it = PVPReadyList_.erase(it);
			DestroyRoom(room);
		}
		++it;
	}

	it = PVEReadyList_.begin();
	while (it != PVEReadyList_.end()) {
		if ((*it)->BattleId == SvrId) {
			BattleRoom * room = *it;
			it = PVEReadyList_.erase(it);
			DestroyRoom(room);
		}
		++it;
	}
}

void RoomManager::ClearOutDateRoom()
{
	for (RoomList::iterator it = PVPReadyList_.begin(); it != PVPReadyList_.end(); ++it) {
		if ((*it)->IsOutDate()) {
			BattleRoom * room = *it;
			it = PVPReadyList_.erase(it);
			memset(room->SecondUser.nick, 0, 16);
			room->SecondUser.userid = 0;
			room->SecondUser.role_tm = 0;
			room->SecondUser.role_type = 0;
			room->Status = waiting_status;
			PVPWaitingList_.push_back(room);
		}
	}

	for (RoomList::iterator it = PVEReadyList_.begin(); it != PVEReadyList_.end(); ++it) {
		if ((*it)->IsOutDate()) {
			BattleRoom * room = *it;
			it = PVEReadyList_.erase(it);
			memset(room->SecondUser.nick, 0, 16);
			room->SecondUser.userid = 0;
			room->SecondUser.role_tm = 0;
			room->SecondUser.role_type = 0;
			room->Status = waiting_status;
			PVEWaitingList_.push_back(room);
		}
	}


}

void RoomManager::JoinPVPRoom(uint32_t battle, uint32_t room)
{
	RoomList::iterator it = PVPReadyList_.begin();
	while (it != PVPReadyList_.end()) {
		if ((*it)->BattleId == battle && (*it)->RoomId == room) {
			BattleRoom * room = *it;
			it = PVPReadyList_.erase(it);
			PVPGoingList_.push_back(room);
			return;
		}
		++it;

	}

}

void RoomManager::JoinPVERoom(uint32_t battle, uint32_t room)
{
	RoomList::iterator it = PVEReadyList_.begin();
	while (it != PVEReadyList_.end()) {
		if ((*it)->BattleId == battle && (*it)->RoomId == room) {
			BattleRoom * room = *it;
			it = PVEReadyList_.erase(it);
			PVEGoingList_.push_back(room);
			return;
		}
		++it;
	}

}

void RoomManager::PackGoingPVPRoomInfo(uint32_t mode, void * buf, int & idx) 
{
	uint32_t cnt = 0;
	int t_idx = idx + 4;
	for (RoomList::iterator it = PVPGoingList_.begin(); it != PVPGoingList_.end(); ++it) {
		if ((*it)->PVPMode == mode) {
			if (t_idx + 40 < max_pkg_size) {
				(*it)->PackPVPInfo(buf, t_idx);
				cnt ++;
			} else {
				break;
			}
		}
	}
	pack_h(buf, cnt, idx);
	idx = t_idx; 
}

















