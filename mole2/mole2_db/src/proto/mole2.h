#ifndef  MOLE2_H
#define  MOLE2_H
#include <libtaomee++/proto/proto_base.h>
#include <stdio.h>

class stru_relation_item: public Cmessage {
	public:
		/**/
		uint32_t		uid;
		/**/
		uint32_t		day;
		/**/
		uint32_t		val;

		stru_relation_item();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_cli_buff: public Cmessage {
	public:
		/*类型*/
		uint32_t		type;
		/**/
		char		buff[512]; 

		stru_cli_buff();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_rank_val: public Cmessage {
	public:
		/*米米号或者编号*/
		uint32_t		id;
		/**/
		uint32_t		val;
		/*昵称*/
		char		nick[16]; 

		stru_rank_val();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_team_rank_t: public Cmessage {
	public:
		/**/
		uint32_t		teamid;
		/*个数*/
		uint32_t		count;
		/**/
		uint32_t		medals;

		stru_team_rank_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_user_rank_t: public Cmessage {
	public:
		/*米米号*/
		uint32_t		userid;
		/**/
		uint32_t		teamid;
		/**/
		uint32_t		medals;

		stru_user_rank_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_ex_item: public Cmessage {
	public:
		/**/
		uint32_t		eid;
		/**/
		uint32_t		itemid;

		stru_ex_item();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_id_cnt: public Cmessage {
	public:
		/*米米号或者编号*/
		uint32_t		id;
		/*个数*/
		uint32_t		count;

		stru_id_cnt();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_petfight_rank_t: public Cmessage {
	public:
		/*米米号*/
		uint32_t		userid;
		/**/
		uint32_t		scores;

		stru_petfight_rank_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class map_grp_item_t: public Cmessage {
	public:
		/*刷新组（挑战时发送）*/
		uint32_t		refresh_id;
		/*怪物组ID*/
		uint32_t		grpid;
		/*随机生成的怪物组素材ID*/
		uint32_t		type_id;
		/*x坐标*/
		uint16_t		posx;
		/*y坐标*/
		uint16_t		posy;
		/*个数*/
		uint32_t		count;

		map_grp_item_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class pvp_score_t: public Cmessage {
	public:
		/*米米号*/
		uint32_t		userid;
		/*昵称*/
		char		nick[16]; 
		/**/
		uint32_t		scores;
		/*是否还在战场上*/
		uint32_t		is_alive;

		pvp_score_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_body_cloth: public Cmessage {
	public:
		/*装扮ID*/
		uint32_t		clothid;
		/*装扮类型*/
		uint32_t		clothtype;
		/*位置*/
		uint32_t		grid;
		/*装扮等级*/
		uint16_t		clothlv;
		/*最大耐久度*/
		uint16_t		duration_max;
		/*当前耐久度*/
		uint16_t		duration;
		/*hp最大值*/
		uint32_t		hpmax;
		/*mp最大值*/
		uint32_t		mpmax;
		/*物攻*/
		uint16_t		attack;
		/*魔攻*/
		uint16_t		mattack;
		/*物防*/
		uint16_t		defense;
		/*魔法防御*/
		uint16_t		mdef;
		/*敏捷*/
		uint16_t		speed;
		/*精神*/
		uint16_t		spirit;
		/*回复*/
		uint16_t		resume;
		/*命中*/
		uint16_t		hit;
		/*闪避*/
		uint16_t		dodge;
		/*必杀*/
		uint16_t		crit;
		/*反击*/
		uint16_t		fightback;
		/*抗毒*/
		uint16_t		rpoison;
		/*抗石化*/
		uint16_t		rlithification;
		/*抗昏睡*/
		uint16_t		rlethargy;
		/*抗酒醉*/
		uint16_t		rinebriation;
		/*抗混乱*/
		uint16_t		rconfusion;
		/*抗遗忘*/
		uint16_t		roblivion;
		/*品质（颜色）*/
		uint32_t		quality;
		/*有效日数*/
		uint32_t		validday;
		/*水晶属性*/
		uint32_t		crystal_attr;
		/*水晶祝福类型*/
		uint32_t		blesstype;

		stru_body_cloth();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_game_change_add: public Cmessage {
	public:
		/*1:摩尔, 2:PP, 3:大玩国,4,DUDU, 5:小花仙, 6,功夫, 21:帕拉巫*/
		uint32_t		gameid;
		/*米米号*/
		uint32_t		userid;
		/*记录时间*/
		uint32_t		logtime;
		/*操作类型：0-4: 对v1-v5中的值加1*/
		uint32_t		opt_type;
		/**/
		int32_t		ex_v1;
		/**/
		int32_t		ex_v2;
		/**/
		int32_t		ex_v3;
		/**/
		int32_t		ex_v4;

		stru_game_change_add();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_trade_cloth: public Cmessage {
	public:
		/**/
		uint32_t		tradeid;
		/*衣服id*/
		uint32_t		clothid;
		/*类型*/
		uint32_t		clothtype;
		/*价 格*/
		uint32_t		price;
		/*税收*/
		uint32_t		tax;
		/**/
		stru_body_cloth		attr;

		stru_trade_cloth();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_trade_cloth_new: public Cmessage {
	public:
		/**/
		uint32_t		tradeid;
		/*衣服id*/
		uint32_t		clothid;
		/*类型*/
		uint32_t		clothtype;
		/*价 格*/
		uint32_t		price;
		/*税收*/
		uint32_t		tax;

		stru_trade_cloth_new();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_trade_item: public Cmessage {
	public:
		/**/
		uint32_t		tradeid;
		/**/
		uint32_t		itemid;
		/*物品数量*/
		uint32_t		itemcnt;
		/**/
		uint32_t		price;
		/**/
		uint32_t		tax;

		stru_trade_item();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_trade_record: public Cmessage {
	public:
		/*物品id或者装备类型clothtype*/
		uint32_t		itemid;
		/*数量*/
		uint32_t		itemcnt;
		/*交易价格*/
		uint32_t		price;
		/*税收*/
		uint32_t		tax;
		/*交易时间*/
		uint32_t		opt_time;

		stru_trade_record();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_trade_shop: public Cmessage {
	public:
		/*状态。0：未接任务，1：任务进行，2：任务完成*/
		uint32_t		state;
		/**/
		uint32_t		uid;
		/**/
		uint32_t		prof;
		/*昵称*/
		char		nick[16]; 

		stru_trade_shop();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_warrior_cloth: public Cmessage {
	public:
		/**/
		uint32_t		cloth_id;
		/**/
		uint32_t		type_id;
		/**/
		uint16_t		clevel;
		/*耐久*/
		uint16_t		duration;
		/**/
		uint8_t		equip_pos;

		stru_warrior_cloth();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_warrior_skill: public Cmessage {
	public:
		/**/
		uint32_t		skill_id;
		/**/
		uint8_t		skill_level;
		/**/
		uint8_t		use_level;
		/**/
		uint32_t		skill_exp;
		/**/
		uint16_t		rand_rate;
		/**/
		uint8_t		cool_round;
		/**/
		uint8_t		cool_update;

		stru_warrior_skill();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_warrior_t: public Cmessage {
	public:
		/*米米号*/
		uint32_t		userid;
		/*宠物ID*/
		uint32_t		petid;
		/**/
		uint32_t		type_id;
		/*昵称*/
		char		nick[16]; 
		/*标志*/
		uint32_t		flag;
		/**/
		uint32_t		viplv;
		/*颜色*/
		uint32_t		color;
		/*种族*/
		uint8_t		race;
		/**/
		uint8_t		prof;
		/*等级*/
		uint32_t		level;
		/*体力*/
		uint16_t		physique;
		/*力量*/
		uint16_t		strength;
		/*耐力*/
		uint16_t		endurance;
		/*速度*/
		uint16_t		quick;
		/*智力*/
		uint16_t		intelligence;
		/*剩余属性点*/
		uint16_t		attr_addition;
		/*体力*/
		uint32_t		hp;
		/*魔法值*/
		uint32_t		mp;
		/**/
		uint32_t		injury_lv;
		/*人物战斗默认站位 1:前排 0:后排*/
		uint8_t		in_front;
		/*地*/
		uint8_t		earth;
		/*水*/
		uint8_t		water;
		/*火*/
		uint8_t		fire;
		/*风*/
		uint8_t		wind;
		/**/
		uint32_t		hp_max;
		/**/
		uint32_t		mp_max;
		/**/
		int16_t		attack;
		/**/
		int16_t		mattack;
		/**/
		int16_t		defense;
		/**/
		int16_t		mdefense;
		/*速度*/
		int16_t		speed;
		/*精神*/
		int16_t		spirit;
		/*恢复*/
		int16_t		resume;
		/**/
		int16_t		hit_rate;
		/**/
		int16_t		avoid_rate;
		/**/
		int16_t		bisha;
		/**/
		int16_t		fight_back;
		/*抗毒*/
		int16_t		rpoison;
		/*抗石化*/
		int16_t		rlithification;
		/*抗昏睡*/
		int16_t		rlethargy;
		/*抗酒醉*/
		int16_t		rinebriation;
		/*抗混乱*/
		int16_t		rconfusion;
		/*抗遗忘*/
		int16_t		roblivion;
		/**/
		uint8_t		need_protect_exp;
		/**/
		uint8_t		cloth_cnt;
		/**/
		stru_warrior_cloth		clothes[8]; 
		/**/
		uint8_t		skill_cnt;
		/**/
		stru_warrior_skill		skills[10]; 
		/**/
		uint32_t		weapon_type;
		/**/
		uint32_t		shield;
		/**/
		uint8_t		pet_cnt_actual;
		/**/
		uint16_t		catchable;
		/**/
		uint32_t		handbooklv;
		/**/
		uint32_t		pet_contact_lv;
		/**/
		uint8_t		pet_state;
		/**/
		uint32_t		default_atk[2]; 
		/**/
		uint8_t		default_atklv[2]; 
		/**/
		uint32_t		autofight_count;
		/*变身怪物race*/
		uint32_t		changerace;
		/*变身怪物id*/
		uint32_t		changepetid;

		stru_warrior_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
#endif // MOLE2_H
