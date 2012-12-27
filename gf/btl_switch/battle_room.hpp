/*
 * =====================================================================================
 *
 *       Filename:  battle_room.hpp
 *
 *    Description: common info team room  
 *
 *        Version:  1.0
 *        Created:  03/03/11 22:17:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */

#ifndef BT_BATTLE_ROOM_H
#define BT_BATTLE_ROOM_H

extern "C" {
#include <stdint.h>
#include <libtaomee/timer.h>
}
#include <cstring>
#include <list>

typedef enum status_t {
	waiting_status = 0,
	ready_status = 1,
	going_status  = 2,
} room_status_t;


struct user_info_t {
	uint32_t userid;
	uint32_t role_tm;
	uint32_t role_type;
	uint8_t  nick[16];
	user_info_t()
	{
		userid = 0;
		role_type = 0;
		memset(nick, 0, 16);
	}
};


class BattleRoom 
{
	public:
		BattleRoom();
		~BattleRoom();

		void SetPVEInfo(uint32_t battle, 
						uint32_t room,
					   	uint32_t stage,
					   	uint32_t diff);

		void SetPVPInfo(uint32_t battle,
						uint32_t room,
						uint32_t lv,
						uint32_t mode);

		void SetFirstUser(uint32_t uid,
				  		  uint32_t user_tm,
				  		  uint32_t role_type,
					      char * name);


		void SetSecondUser(uint32_t uid,
					       uint32_t user_tm,
						   uint32_t role_type,
					       char * name);

		void PackPVPInfo(void * buf, int & idx);

		void SetMatched()
		{
			Status = ready_status;
			MatchTime = get_now_tv()->tv_sec;
		}

		bool IsStart()
		{
			return (Status == going_status);
		}

		bool IsOutDate()
		{
			uint32_t cur_time = get_now_tv()->tv_sec;
			if (MatchTime  && cur_time - 5 > MatchTime) {
				return true;
			}
			return false;
		}

		void init()
		{
			BattleId = 0;
			RoomId = 0;
			Status = waiting_status;
			StageId = 0;
			PVPMode = 0;
			CreatorLv = 0;
			Diffcult = 0;
			MatchTime = 0;

			FirstUser.userid = 0;
			FirstUser.role_tm = 0;
			FirstUser.role_type =0;
			memset(FirstUser.nick, 0, 16);

			SecondUser.userid = 0;
			SecondUser.role_tm = 0;
			SecondUser.role_type =0;
			memset(SecondUser.nick, 0, 16);

		}

		uint32_t BattleId;;
		uint32_t RoomId;
		status_t Status;
		uint32_t StageId;
		uint32_t PVPMode;
		uint32_t CreatorLv;
		uint32_t Diffcult;

		user_info_t  FirstUser;

		user_info_t  SecondUser;

		uint32_t MatchTime;
		
	private:
		BattleRoom(const BattleRoom & room);


};

typedef std::list<BattleRoom*> RoomList;

class RoomFactory 
{
	public:

		RoomFactory();

		~RoomFactory();

		BattleRoom *CreatePVERoom(uint32_t batttle,
								  uint32_t room,
								  uint32_t stage,
								  uint32_t diff);

		BattleRoom * CreatePVPRoom(uint32_t battle,
				                   uint32_t room,
								   uint32_t stage,
								   uint32_t diff);

		void DestroyRoom(BattleRoom * room);

	private:
		uint32_t curMax_;
		RoomList mList_;
};

BattleRoom * CreatePVERoom(uint32_t battle_id,
						   uint32_t room_id,
						   uint32_t stage,
						   uint32_t diff);

BattleRoom * CreatePVPRoom(uint32_t battle_id,
		                   uint32_t room_id,
						   uint32_t lv,
						   uint32_t mode);

void DestroyRoom(BattleRoom * room);

class RoomManager
{
	public:

		RoomManager();
		~RoomManager();

		void AddPVERoom(BattleRoom * Room);

		void AddPVPRoom(BattleRoom * Room);

		void DestroyPVERoom(uint32_t battle, uint32_t room);

		void DestroyPVPRoom(uint32_t battle, uint32_t room);

		void StartPVPRoom(uint32_t battle, uint32_t room);

		void StartPVERoom(uint32_t battle, uint32_t room);

		BattleRoom * FindPVERoom(uint32_t stage, uint32_t diff);

		BattleRoom * FindPVPRoom(uint32_t user_lv, uint32_t pvp_mode);

		BattleRoom * FindRedBlueRoom(uint32_t user_lv, uint32_t pvp_mode);

		void JoinPVPRoom(uint32_t battle, uint32_t room);

		void JoinPVERoom(uint32_t battle, uint32_t room);

		void PackGoingPVPRoomInfo(uint32_t mode, void * buf, int & idx); 

		void ClearOutDateRoom();


		void ClearBattleRoom(uint32_t SvrId);


	private:
		RoomList  PVPWaitingList_;


		RoomList  PVPGoingList_;


		RoomList  PVEWaitingList_;


		RoomList  PVEGoingList_;

		RoomList  PVPReadyList_;

		RoomList  PVEReadyList_;
};
#endif
