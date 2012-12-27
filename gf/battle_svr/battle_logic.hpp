#ifndef _BATTLE_LOGIC_H_
#define _BATTLE_LOGIC_H_

#include "fwd_decl.hpp"

void init_call_monsters_order_data();


void process_battle_925_logic(Battle * btl, struct timeval next_tm);
void process_battle_925_init_logic(Battle* btl, void* para);

void process_battle_45_logic(Battle* btl, struct timeval  next_tm);
void process_battle_926_logic(Battle * btl, struct timeval next_tm);


void process_battle_927_logic(Battle * btl, struct timeval next_tm);
void process_battle_928_logic(Battle * btl, struct timeval next_tm);

void process_battle_929_logic(Battle * btl, struct timeval next_tm);
void* battle_929_data_alloc();
void battle_929_data_release(void* para);

void process_battle_930_logic(Battle * ble, struct timeval next_tm);
//彩人堂
void process_battle_932_logic(Battle * btl, struct timeval next_tm);
//古墓1
void process_battle_933_logic(Battle * btl, struct timeval next_tm);

void process_battle_934_logic(Battle * btl, struct timeval next_tm);

void process_battle_942_logic(Battle * btl, struct timeval next_tm);
//taotai
void process_battle_939_logic(Battle * btl, struct timeval next_tm);

void process_battle_952_logic(Battle * btl, struct timeval next_tm);
/**
 * @brief pvp final game
 */
void process_battle_941_final_game_logic(Battle *btl, struct timeval next_tm);

int extra_mechanism_logic(Battle * btl, Player * mechanism);

//天外飞石
void process_battle_953_logic(Battle *btl, struct timeval next_tm);
//真假武圣
void process_battle_977_logic(Battle * btl, struct timeval next_tm);

void process_battle_978_logic(Battle *btl, struct timeval next_tm);

void process_battle_979_logic(Battle *btl, struct timeval next_tm);

void process_battle_984_logic(Battle * ble, struct timeval next_tm);

void process_battle_985_logic(Battle * ble, struct timeval next_tm);

void process_battle_986_logic(Battle * btl, struct timeval next_tm);

void process_battle_988_logic(Battle * btl, struct timeval next_tm);

void process_battle_989_logic(Battle * btl, struct timeval next_tm);

void process_battle_990_logic(Battle * btl, struct timeval next_tm);

void process_battle_991_logic(Battle * btl, struct timeval next_tm);

void process_battle_993_logic(Battle * btl, struct timeval next_tm);

void process_battle_702_logic(Battle * btl, struct timeval next_tm);

void process_battle_703_logic(Battle * btl, struct timeval next_tm);

void process_battle_995_logic(Battle * btl, struct timeval next_tm);

void process_battle_996_logic(Battle * btl, struct timeval next_tm);

void process_battle_997_logic(Battle * btl, struct timeval next_tm);

void process_battle_701_logic(Battle *btl, struct timeval next_tm);

void process_battle_964_logic(Battle * btl, struct timeval next_tm);

void process_battle_705_logic(Battle * btl, struct timeval next_tm);

void process_battle_719_logic(Battle * btl, struct timeval next_tm);
void process_battle_718_logic(Battle * btl, struct timeval next_tm);
void process_battle_721_logic(Battle * btl, struct timeval next_tm);

void process_battle_734_logic(Battle * btl, struct timeval next_tm); 

class SpecialExpDoubleTimeMrg : public SpecialTimeMrg, public taomee::EventableObject{
	private:
		bool double_tm_flag;
		bool double_tm_flag_whitch_can_still_use_double_tm_item;
		bool double_tm_flag_2;
	private:
		int reset_exp_double_time_flag();
		int reset_exp_double_time_2_flag();
		int reset_exp_double_time_flag_which_can_use_double_tm_item();
	public:
		SpecialExpDoubleTimeMrg()
		{
			double_tm_flag = false;
			double_tm_flag_whitch_can_still_use_double_tm_item = false;
			double_tm_flag_2 = false;
		}
		void init();
		
	public:
		bool is_in_double_time()
		{
			return double_tm_flag;
		}
		bool is_in_double_time_witch_can_still_use_double_tm_item()
		{
			return double_tm_flag_whitch_can_still_use_double_tm_item;
		}
		bool is_in_double_time_2()
		{
			return double_tm_flag_2;
		}
};

extern SpecialExpDoubleTimeMrg g_special_double_time_mrg;



#endif

