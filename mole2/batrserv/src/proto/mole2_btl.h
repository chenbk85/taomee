#ifndef  MOLE2_BTL_H
#define  MOLE2_BTL_H
#include "mole2.h"

class accept_battle_op_in: public Cmessage {
	public:
		/**/
		uint32_t		grpid;
		/*目前用作传参数type 0普通 1多精灵对战时可以带辅助宠物2双人组队可以带辅助宠物*/
		uint32_t		win;
		/**/
		uint32_t		percent;
		/**/
		uint32_t		mark;
		/**/
		std::vector<stru_warrior_t>		warriors;

		accept_battle_op_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class nest_atk_info_t: public Cmessage {
	public:
		/**/
		uint32_t		uid;
		/**/
		uint32_t		petid;
		/**/
		uint8_t		fantan_ret;
		/**/
		uint32_t		atk_type;
		/**/
		uint8_t		atk_level;
		/*增加多少hp*/
		uint16_t		add_hp;
		/**/
		uint16_t		fantan_hp;
		/**/
		uint16_t		fantan_mp;
		/**/
		uint32_t		use_itemid;
		/**/
		uint32_t		atk_uid;
		/**/
		uint32_t		atk_petid;
		/**/
		uint8_t		huwei_pos;
		/**/
		uint64_t		fightee_state;
		/**/
		uint16_t		hurt_hp;
		/**/
		uint16_t		hurt_mp;

		nest_atk_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class attack_info_t: public Cmessage {
	public:
		/**/
		uint32_t		seq;
		/**/
		uint32_t		fighter;
		/**/
		uint32_t		petid;
		/**/
		uint64_t		fighter_state;
		/**/
		uint32_t		topic_id;
		/**/
		uint16_t		resume_hp1;
		/**/
		uint16_t		resume_hp2;
		/**/
		uint16_t		resume_hp3;
		/**/
		uint16_t		poison_hp;
		/**/
		uint32_t		host_uid;
		/**/
		uint32_t		host_petid;
		/**/
		uint16_t		parasitism_hp;
		/**/
		uint16_t		zhushang_hp;
		/**/
		uint16_t		curse_mp;
		/**/
		std::vector<nest_atk_info_t>		nest_akt_list;

		attack_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class bt_battle_attack_in: public Cmessage {
	public:
		/**/
		uint32_t		petid;
		/**/
		uint32_t		enemy_mark;
		/**/
		int32_t		enemy_pos;
		/**/
		uint32_t		atk_type;
		/**/
		uint32_t		atk_level;
		/*药物id*/
		uint32_t		itemid;
		/*增加多少hp*/
		uint32_t		add_hp;

		bt_battle_attack_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class bt_load_battle_in: public Cmessage {
	public:
		/**/
		uint32_t		rate;

		bt_load_battle_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class bt_load_battle_ready_in: public Cmessage {
	public:
		/**/
		uint32_t		ready;

		bt_load_battle_ready_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class bt_noti_battle_attacks_info_out: public Cmessage {
	public:
		/**/
		std::vector<attack_info_t>		attack_info_list;

		bt_noti_battle_attacks_info_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class bt_recall_pet_in: public Cmessage {
	public:
		/**/
		uint32_t		petid;

		bt_recall_pet_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class bt_use_chemical_in: public Cmessage {
	public:
		/**/
		uint32_t		petid;
		/**/
		uint32_t		enemy_mark;
		/**/
		int32_t		enemy_pos;
		/**/
		uint32_t		itemid;
		/**/
		uint32_t		add_hp;

		bt_use_chemical_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class catch_pet_op_in: public Cmessage {
	public:
		/**/
		uint32_t		mark;
		/**/
		uint32_t		pos;
		/**/
		uint32_t		itemid;
		/**/
		uint32_t		level;
		/**/
		std::vector<uint32_t>		data;

		catch_pet_op_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class req_challenge_t: public Cmessage {
	public:
		/**/
		uint32_t		teamid;
		/**/
		uint32_t		win_cnt;
		/**/
		uint32_t		mode;
		/**/
		std::vector<stru_warrior_t>		warriors;

		req_challenge_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class challenge_battle_op_in: public Cmessage {
	public:
		/**/
		req_challenge_t		req_challenge_info;

		challenge_battle_op_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class challenge_npc_battle_op_in: public Cmessage {
	public:
		/**/
		req_challenge_t		req_challenge_info;

		challenge_npc_battle_op_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_bt_atk_back: public Cmessage {
	public:
		/*米米号*/
		uint32_t		userid;
		/**/
		uint32_t		petid;
		/**/
		uint32_t		skill_id;
		/**/
		uint32_t		skill_lvl;
		/**/
		uint32_t		atk_userid;
		/**/
		uint32_t		atk_petid;

		stru_bt_atk_back();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_bt_atk_list: public Cmessage {
	public:
		/**/
		uint32_t		seq;
		/*米米号*/
		uint32_t		userid;
		/**/
		uint32_t		petid;
		/**/
		uint32_t		topic_id;

		stru_bt_atk_list();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_bt_kvalue: public Cmessage {
	public:
		/**/
		uint16_t		key;
		/**/
		int16_t		value;

		stru_bt_kvalue();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_bt_atk_pkt: public Cmessage {
	public:
		/**/
		uint32_t		seq;
		/*米米号*/
		uint32_t		userid;
		/**/
		uint32_t		petid;
		/**/
		uint32_t		topic;
		/**/
		uint64_t		state;
		/**/
		std::vector<stru_bt_kvalue>		changes;

		stru_bt_atk_pkt();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
#endif // MOLE2_BTL_H
