#ifndef  MOLE2_DB_H
#define  MOLE2_DB_H
#include "mole2.h"

class stru_cloth_info: public Cmessage {
	public:
		/*获取时间*/
		uint32_t		gettime;
		/**/
		uint32_t		clothid;
		/*位置标识*/
		uint32_t		gridid;
		/*体力最大值*/
		uint32_t		hpmax;
		/*魔法最大值*/
		uint32_t		mpmax;
		/*等级*/
		uint16_t		level;
		/*耐久*/
		uint16_t		duration;
		/**/
		uint16_t		mduration;
		/*攻击*/
		uint16_t		atk;
		/*魔法攻击*/
		uint16_t		matk;
		/**/
		uint16_t		defense;
		/*魔法防御*/
		uint16_t		mdef;
		/*速度*/
		uint16_t		speed;
		/*精神*/
		uint16_t		spirit;
		/*恢复*/
		uint16_t		resume;
		/*命中值*/
		uint16_t		hit;
		/*闪避值*/
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
		/**/
		uint32_t		quality;
		/**/
		uint32_t		validday;
		/**/
		uint32_t		crystal_attr;
		/**/
		uint32_t		bless_type;

		stru_cloth_info();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class buy_cloth_in_mall_in: public Cmessage {
	public:
		//
		std::vector<stru_cloth_info>		cloths;

		buy_cloth_in_mall_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_item_count: public Cmessage {
	public:
		/**/
		uint32_t		itemid;
		/*个数*/
		uint32_t		count;

		stru_item_count();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class buy_item_in_mall_in: public Cmessage {
	public:
		//
		std::vector<stru_item_count>		items;

		buy_item_in_mall_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class comm_get_dbser_version_out: public Cmessage {
	public:
		/*消息*/
		char		msg[255]; 

		comm_get_dbser_version_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class day_sub_ssid_count_in: public Cmessage {
	public:
		/**/
		uint32_t		ssid;
		/*个数*/
		uint32_t		count;

		day_sub_ssid_count_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class get_all_pet_type_out: public Cmessage {
	public:
		//
		std::vector<uint32_t>		petlist;

		get_all_pet_type_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class get_rand_info_range_in: public Cmessage {
	public:
		/**/
		uint32_t		minid;
		/**/
		uint32_t		maxid;

		get_rand_info_range_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_rand_info: public Cmessage {
	public:
		/*时间*/
		uint32_t		time;
		/**/
		uint32_t		randid;
		/*个数*/
		uint32_t		count;

		stru_rand_info();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class get_rand_info_range_out: public Cmessage {
	public:
		//
		std::vector<stru_rand_info>		item_list;

		get_rand_info_range_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class item_handbook_t: public Cmessage {
	public:
		/**/
		uint32_t		itemid;
		/*状态。0：未接任务，1：任务进行，2：任务完成*/
		uint32_t		state;
		/*个数*/
		uint32_t		count;

		item_handbook_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class log_vip_item_in: public Cmessage {
	public:
		/**/
		uint32_t		logtype;
		/**/
		uint32_t		itemid;
		/*个数*/
		int32_t		count;

		log_vip_item_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_cloth_limit: public Cmessage {
	public:
		/**/
		uint32_t		clothid;
		/*个数*/
		uint32_t		count;
		/**/
		uint32_t		limit;

		stru_cloth_limit();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mall_chk_cloths_limits_in: public Cmessage {
	public:
		//
		std::vector<stru_cloth_limit>		cloths;

		mall_chk_cloths_limits_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_add_day_count_in: public Cmessage {
	public:
		/**/
		uint32_t		itemid;
		/*个数*/
		uint32_t		count;

		mole2_add_day_count_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_add_graduation_uid_in: public Cmessage {
	public:
		/**/
		uint32_t		uid;

		mole2_add_graduation_uid_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_add_medals_in: public Cmessage {
	public:
		/**/
		uint32_t		medals;

		mole2_add_medals_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_add_medals_out: public Cmessage {
	public:
		/**/
		uint32_t		medals;

		mole2_add_medals_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_add_relation_exp_in: public Cmessage {
	public:
		/*经验*/
		int32_t		exp;

		mole2_add_relation_exp_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_add_relation_uid_in: public Cmessage {
	public:
		/*flag =1 加师傅,自己是徒弟,2,加徒弟,自己是师傅*/
		uint32_t		flag;
		/**/
		uint32_t		uid;

		mole2_add_relation_uid_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_add_relation_uid_out: public Cmessage {
	public:
		/*标志*/
		uint32_t		flag;
		/**/
		uint32_t		uid;

		mole2_add_relation_uid_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_add_relation_val_in: public Cmessage {
	public:
		/**/
		uint32_t		uid;
		/**/
		int32_t		addval;

		mole2_add_relation_val_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_add_relation_val_out: public Cmessage {
	public:
		/**/
		uint32_t		uid;
		/*数值*/
		uint32_t		value;

		mole2_add_relation_val_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_add_skill_exp_in: public Cmessage {
	public:
		/**/
		int32_t		add_exp;

		mole2_add_skill_exp_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_add_skill_exp_out: public Cmessage {
	public:
		/**/
		uint32_t		cur_exp;

		mole2_add_skill_exp_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_add_val_in: public Cmessage {
	public:
		/*米米号*/
		uint32_t		userid;
		/**/
		uint32_t		limit;

		mole2_add_val_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_add_val_out: public Cmessage {
	public:
		/*米米号*/
		uint32_t		userid;
		/**/
		uint32_t		val;

		mole2_add_val_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_day_set_ssid_count_in: public Cmessage {
	public:
		/**/
		uint32_t		ssid;
		/*个数*/
		uint32_t		count;
		/**/
		uint32_t		totalcnt;

		mole2_day_set_ssid_count_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_del_relation_uid_in: public Cmessage {
	public:
		/*标志*/
		uint32_t		flag;
		/**/
		uint32_t		uid;
		/**/
		uint32_t		op_uid;

		mole2_del_relation_uid_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_del_relation_uid_out: public Cmessage {
	public:
		/*标志*/
		uint32_t		flag;
		/*经验*/
		uint32_t		exp;
		/**/
		uint32_t		graduation;
		/**/
		uint32_t		total_val;
		//
		std::vector<stru_relation_item>		relations;

		mole2_del_relation_uid_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_del_vip_buff_in: public Cmessage {
	public:
		//宠物ID
		std::vector<uint32_t>		petid;

		mole2_del_vip_buff_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class user2_info_t: public Cmessage {
	public:
		/**/
		uint32_t		vip_auto;
		/**/
		uint32_t		vip_level;
		/**/
		uint32_t		vip_ex_val;
		/**/
		uint32_t		vip_base_val;
		/**/
		uint32_t		vip_end_time;
		/**/
		uint32_t		vip_begin_time;
		/**/
		uint32_t		vip_activity;
		/**/
		uint32_t		vip_activity_2;
		/**/
		uint32_t		vip_activity_3;
		/**/
		uint32_t		vip_activity_4;

		user2_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class user_day_t: public Cmessage {
	public:
		/**/
		uint32_t		ssid;
		/**/
		uint32_t		total;
		/*个数*/
		uint32_t		count;
		/**/
		uint32_t		today;

		user_day_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_beast_book: public Cmessage {
	public:
		/**/
		uint32_t		beastid;
		/*状态。0：未接任务，1：任务进行，2：任务完成*/
		uint32_t		state;
		/*个数*/
		uint32_t		count;

		stru_beast_book();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class title_t: public Cmessage {
	public:
		/**/
		uint32_t		titleid;
		/**/
		uint32_t		gen_time;

		title_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_vip_buff: public Cmessage {
	public:
		/*宠物ID*/
		uint32_t		petid;
		/**/
		char		buff[128]; 

		stru_vip_buff();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_all_info_out: public Cmessage {
	public:
		/**/
		user2_info_t		user2_info;
		//每日限制
		std::vector<user_day_t>		day_limit_list;
		//
		std::vector<item_handbook_t>		item_handbook_list;
		//
		std::vector<stru_cloth_info>		attire_list;
		//
		std::vector<stru_cli_buff>		cli_buff_list;
		//
		std::vector<stru_beast_book>		beast_book_list;
		//
		std::vector<title_t>		user_title_list;
		//
		std::vector<stru_vip_buff>		vip_buf_list;
		//
		std::vector<stru_cloth_info>		cloths;

		mole2_get_all_info_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_day_count_in: public Cmessage {
	public:
		/**/
		uint32_t		itemid;

		mole2_get_day_count_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_day_count_out: public Cmessage {
	public:
		/**/
		uint32_t		total;
		/*个数*/
		uint32_t		count;

		mole2_get_day_count_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_hero_team_out: public Cmessage {
	public:
		/**/
		uint32_t		teamid;
		/**/
		uint32_t		medals;

		mole2_get_hero_team_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_relation_out: public Cmessage {
	public:
		/*标志*/
		uint32_t		flag;
		/*经验*/
		uint32_t		exp;
		/**/
		uint32_t		graduation;
		/**/
		uint32_t		total_val;
		//
		std::vector<stru_relation_item>		relations;

		mole2_get_relation_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_survey_in: public Cmessage {
	public:
		/**/
		uint32_t		surveyid;

		mole2_get_survey_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_survey_reply_new: public Cmessage {
	public:
		/**/
		uint32_t		option_id;
		/**/
		uint32_t		answer_id;
		/**/
		uint32_t		total;

		stru_survey_reply_new();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_survey_out: public Cmessage {
	public:
		//
		std::vector<stru_survey_reply_new>		replys;

		mole2_get_survey_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_task_rank_out: public Cmessage {
	public:
		//
		std::vector<stru_rank_val>		uids;

		mole2_get_task_rank_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_team_rank_out: public Cmessage {
	public:
		//
		std::vector<stru_team_rank_t>		ranks;

		mole2_get_team_rank_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_type_pets_in: public Cmessage {
	public:
		/*宠物类型ID*/
		uint32_t		pettype;
		/**/
		uint32_t		start;
		/**/
		uint32_t		limit;

		mole2_get_type_pets_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_skill_info: public Cmessage {
	public:
		/*技能ID*/
		uint32_t		skillid;
		/*等级*/
		uint32_t		level;
		/*经验*/
		uint32_t		exp;

		stru_skill_info();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_pet_info: public Cmessage {
	public:
		/*获取时间*/
		uint32_t		gettime;
		/*宠物类型ID*/
		uint32_t		pettype;
		/*种族*/
		uint32_t		race;
		/*标志*/
		uint32_t		flag;
		/*昵称*/
		char		nick[16]; 
		/*等级*/
		uint32_t		level;
		/*经验*/
		uint32_t		exp;
		/*体力初值*/
		uint16_t		physiqueinit;
		/*力量初值*/
		uint16_t		strengthinit;
		/*耐力初值*/
		uint16_t		enduranceinit;
		/*速度初值*/
		uint16_t		quickinit;
		/*智慧初值*/
		uint16_t		iqinit;
		/*体力成长值*/
		uint16_t		physiqueparam;
		/*力量成长值*/
		uint16_t		strengthparam;
		/*耐力成长值*/
		uint16_t		enduranceparam;
		/*速度成长值*/
		uint16_t		quickparam;
		/*智慧成长值*/
		uint16_t		iqparam;
		/*体力配点值*/
		uint16_t		physique_add;
		/*力量配点值*/
		uint16_t		strength_add;
		/*耐力配点值*/
		uint16_t		endurance_add;
		/*速度配点值*/
		uint16_t		quick_add;
		/*智慧配点值*/
		uint16_t		iq_add;
		/*剩余属性点*/
		uint16_t		attr_addition;
		/*体力*/
		uint32_t		hp;
		/*魔法值*/
		uint32_t		mp;
		/*地*/
		uint8_t		earth;
		/*水*/
		uint8_t		water;
		/*火*/
		uint8_t		fire;
		/*风*/
		uint8_t		wind;
		/*受伤等级 0:未受伤, 1:白 2:黄 3:红*/
		uint32_t		injury_state;
		/*位置 ,不能为0*/
		uint32_t		location;
		//
		std::vector<stru_skill_info>		skills;

		stru_pet_info();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_type_pets_out: public Cmessage {
	public:
		/**/
		uint32_t		total;
		/**/
		uint32_t		start;
		//
		std::vector<stru_pet_info>		pets;

		mole2_get_type_pets_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_user_cli_buff_in: public Cmessage {
	public:
		/**/
		uint32_t		min_id;
		/**/
		uint32_t		max_id;

		mole2_get_user_cli_buff_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_user_cli_buff_out: public Cmessage {
	public:
		//
		std::vector<stru_cli_buff>		item_list;

		mole2_get_user_cli_buff_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_user_rank_out: public Cmessage {
	public:
		//
		std::vector<stru_user_rank_t>		ranks;

		mole2_get_user_rank_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_user_sql_in: public Cmessage {
	public:
		/**/
		uint32_t		obj_userid;

		mole2_get_user_sql_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_user_sql_out: public Cmessage {
	public:
		//
		uint32_t _sql_str_len;
		char		sql_str[8000000];

		mole2_get_user_sql_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_val_by_id_in: public Cmessage {
	public:
		//
		std::vector<stru_ex_item>		list;

		mole2_get_val_by_id_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_val_by_id_out: public Cmessage {
	public:
		//
		std::vector<stru_id_cnt>		vals;

		mole2_get_val_by_id_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_usr_val: public Cmessage {
	public:
		/*米米号*/
		uint32_t		userid;
		/**/
		uint32_t		val;

		stru_usr_val();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_val_out: public Cmessage {
	public:
		//
		std::vector<stru_usr_val>		vals;

		mole2_get_val_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_vip_buff_in: public Cmessage {
	public:
		//宠物ID
		std::vector<uint32_t>		petid;

		mole2_get_vip_buff_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_vip_buff_out: public Cmessage {
	public:
		//
		std::vector<stru_vip_buff>		petlist;

		mole2_get_vip_buff_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_get_vip_flag_out: public Cmessage {
	public:
		/*0x01:vip,0x100,曾经是vip*/
		uint32_t		vip_flag;

		mole2_get_vip_flag_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_init_feed_count_in: public Cmessage {
	public:
		/*类型*/
		uint32_t		type;
		/*物品ID*/
		uint32_t		itemid;

		mole2_init_feed_count_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_login_out: public Cmessage {
	public:
		/*昵称*/
		char		nick[16]; 
		/*个性签名*/
		char		signature[32]; 
		/*标志*/
		uint32_t		flag;
		/*颜色*/
		uint32_t		color;
		/*注册时间*/
		uint32_t		register_time;
		/*种族*/
		uint8_t		race;
		/*职业 0：无 1:剑士 2:弓箭手 3:魔法师 4:传教士*/
		uint8_t		professtion;
		/*joblevel*/
		uint32_t		joblevel;
		/*荣誉值*/
		uint32_t		honor;
		/*小米*/
		uint32_t		xiaomee;
		/*等级*/
		uint32_t		level;
		/*经验*/
		uint32_t		experience;
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
		/*剩余属性*/
		uint16_t		attr_addition;
		/*体力*/
		uint32_t		hp;
		/*魔法值*/
		uint32_t		mp;
		/*受伤等级 0:未受伤, 1:白 2:黄 3:红*/
		uint32_t		injury_state;
		/*人物战斗默认站位 1:前排 0:后排*/
		uint8_t		in_front;
		/*人物携带最大装备数*/
		uint32_t		max_attire;
		/*人物携带最大药剂数*/
		uint32_t		max_medicine;
		/*人物携带最大材料数*/
		uint32_t		max_stuff;

		mole2_login_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_vip_args: public Cmessage {
	public:
		/*类型*/
		uint32_t		type;
		/**/
		uint32_t		ex_val;
		/**/
		uint32_t		base_val;
		/**/
		uint32_t		end_time;
		/**/
		uint32_t		begin_time;

		stru_vip_args();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_battle_info: public Cmessage {
	public:
		/**/
		uint32_t		all_score;
		/**/
		uint32_t		day_score;
		/**/
		uint32_t		update_time;

		stru_battle_info();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_item_info: public Cmessage {
	public:
		/**/
		uint32_t		itemid;
		/*个数*/
		uint32_t		count;
		/**/
		uint32_t		storage_cnt;

		stru_item_info();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_task_done: public Cmessage {
	public:
		/*任务ID*/
		uint32_t		taskid;
		/**/
		uint32_t		node;
		/*状态。0：未接任务，1：任务进行，2：任务完成*/
		uint32_t		state;
		/*上次任务完成时间*/
		uint32_t		optdate;
		/**/
		uint32_t		fin_time;
		/**/
		uint32_t		fin_num;

		stru_task_done();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_task_doing: public Cmessage {
	public:
		/*任务ID*/
		uint32_t		taskid;
		/**/
		uint32_t		node;
		/*状态。0：未接任务，1：任务进行，2：任务完成*/
		uint32_t		state;
		/*上次任务完成时间*/
		uint32_t		optdate;
		/**/
		uint32_t		fin_time;
		/**/
		uint32_t		fin_num;
		/**/
		char		cli_buf[128]; 
		/**/
		char		ser_buf[20]; 

		stru_task_doing();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_map_state: public Cmessage {
	public:
		/*地图编号*/
		uint32_t		mapid;
		/*状态。0：未接任务，1：任务进行，2：任务完成*/
		uint32_t		state;

		stru_map_state();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_day_limit: public Cmessage {
	public:
		/**/
		uint32_t		ssid;
		/**/
		uint32_t		total;
		/*个数*/
		uint32_t		count;

		stru_day_limit();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_online_login_out: public Cmessage {
	public:
		/*昵称*/
		char		nick[16]; 
		/*标志*/
		uint32_t		flag;
		/*颜色*/
		uint32_t		color;
		/*注册时间*/
		uint32_t		register_time;
		/*种族*/
		uint32_t		race;
		/*职业 0：无 1:剑士 2:弓箭手 3:魔法师 4:传教士*/
		uint32_t		professtion;
		/**/
		uint32_t		joblevel;
		/*荣誉值*/
		uint32_t		honor;
		/*小米*/
		uint32_t		xiaomee;
		/*等级*/
		uint32_t		level;
		/*经验*/
		uint32_t		experience;
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
		/*受伤等级 0:未受伤, 1:白 2:黄 3:红*/
		uint32_t		injury_state;
		/*人物战斗默认站位 1:前排 0:后排*/
		uint32_t		in_front;
		/*人物携带最大装备数*/
		uint32_t		max_attire;
		/*人物携带最大药剂数*/
		uint32_t		max_medicine;
		/*人物携带最大材料数*/
		uint32_t		max_stuff;
		/*时间*/
		uint32_t		time;
		/*坐标x轴*/
		uint32_t		axis_x;
		/*坐标y轴*/
		uint32_t		axis_y;
		/*地图编号*/
		uint32_t		mapid;
		/**/
		uint32_t		daytime;
		/**/
		uint32_t		fly_mapid;
		/**/
		uint32_t		expbox;
		/*电量值*/
		uint32_t		energy;
		/**/
		uint32_t		skill_expbox;
		/**/
		uint32_t		flag_ex;
		/**/
		uint32_t		winbossid;
		/**/
		uint32_t		parent;
		/**/
		stru_vip_args		vipargs;
		/**/
		stru_battle_info		battle;
		/**/
		uint32_t		all_pet_cnt;
		//
		std::vector<stru_skill_info>		skills;
		//
		std::vector<stru_pet_info>		pets;
		//
		std::vector<stru_cloth_info>		cloths;
		//
		std::vector<stru_cloth_info>		cloths_in_bag;
		//
		std::vector<stru_item_info>		items;
		//
		std::vector<uint32_t>		titles;
		//
		std::vector<stru_task_done>		task_done;
		//
		std::vector<stru_task_doing>		task_doing;
		//
		std::vector<stru_map_state>		mapstates;
		//
		std::vector<stru_beast_book>		beastbook;
		//
		std::vector<stru_day_limit>		daylimits;
		//
		std::vector<stru_vip_buff>		vipbuffs;
		//消息列表
		uint32_t _msglist_len;
		char		msglist[2000];

		mole2_online_login_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_petfight_get_rank_out: public Cmessage {
	public:
		//用户和分数
		std::vector<stru_petfight_rank_t>		ranks;

		mole2_petfight_get_rank_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_petfight_get_user_rank_out: public Cmessage {
	public:
		/*当前排名*/
		uint32_t		rank;
		/*上次排名*/
		uint32_t		last_rank;

		mole2_petfight_get_user_rank_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_petfight_set_scores_in: public Cmessage {
	public:
		/*增加的分数*/
		uint32_t		scores;

		mole2_petfight_set_scores_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_survey_reply: public Cmessage {
	public:
		/**/
		uint32_t		option_id;
		/**/
		uint32_t		value_id;

		stru_survey_reply();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_reply_question_in: public Cmessage {
	public:
		/**/
		uint32_t		surveyid;
		//
		std::vector<stru_survey_reply>		replys;

		mole2_reply_question_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_set_flag_bits_in: public Cmessage {
	public:
		/*0-63*/
		uint32_t		pos_id;
		/**/
		uint32_t		is_set_true;

		mole2_set_flag_bits_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_set_flag_bits_out: public Cmessage {
	public:
		/*标志*/
		uint32_t		flag;
		/**/
		uint32_t		pos_id;

		mole2_set_flag_bits_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_set_hero_team_in: public Cmessage {
	public:
		/**/
		uint32_t		teamid;

		mole2_set_hero_team_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_set_levelup_time_in: public Cmessage {
	public:
		/*时间*/
		uint32_t		time;

		mole2_set_levelup_time_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_set_pet_attributes_in: public Cmessage {
	public:
		/*宠物ID*/
		uint32_t		petid;
		/*等级*/
		uint32_t		level;
		/*经验*/
		uint32_t		exp;
		/*体力初值*/
		uint16_t		physiqueinit;
		/*力量初值*/
		uint16_t		strengthinit;
		/*耐力初值*/
		uint16_t		enduranceinit;
		/*速度初值*/
		uint16_t		quickinit;
		/*智慧初值*/
		uint16_t		iqinit;
		/*体力成长值*/
		uint16_t		physiqueparam;
		/*力量成长值*/
		uint16_t		strengthparam;
		/*耐力成长值*/
		uint16_t		enduranceparam;
		/*速度成长值*/
		uint16_t		quickparam;
		/*智慧成长值*/
		uint16_t		iqparam;
		/*体力配点值*/
		uint16_t		physiqueadd;
		/*力量配点值*/
		uint16_t		strengthadd;
		/*耐力配点值*/
		uint16_t		enduranceadd;
		/*速度配点值*/
		uint16_t		quickadd;
		/*智慧配点值*/
		uint16_t		iqadd;
		/**/
		uint16_t		attradd;
		/*体力*/
		uint32_t		hp;
		/*魔法值*/
		uint32_t		mp;

		mole2_set_pet_attributes_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_set_pet_attributes_out: public Cmessage {
	public:
		/*宠物ID*/
		uint32_t		petid;
		/*等级*/
		uint32_t		level;
		/*经验*/
		uint32_t		exp;
		/*体力初值*/
		uint16_t		physiqueinit;
		/*力量初值*/
		uint16_t		strengthinit;
		/*耐力初值*/
		uint16_t		enduranceinit;
		/*速度初值*/
		uint16_t		quickinit;
		/*智慧初值*/
		uint16_t		iqinit;
		/*体力成长值*/
		uint16_t		physiqueparam;
		/*力量成长值*/
		uint16_t		strengthparam;
		/*耐力成长值*/
		uint16_t		enduranceparam;
		/*速度成长值*/
		uint16_t		quickparam;
		/*智慧成长值*/
		uint16_t		iqparam;
		/*体力配点值*/
		uint16_t		physiqueadd;
		/*力量配点值*/
		uint16_t		strengthadd;
		/*耐力配点值*/
		uint16_t		enduranceadd;
		/*速度配点值*/
		uint16_t		quickadd;
		/*智慧配点值*/
		uint16_t		iqadd;
		/**/
		uint16_t		attradd;
		/*体力*/
		uint32_t		hp;
		/*魔法值*/
		uint32_t		mp;

		mole2_set_pet_attributes_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_set_pet_flag_in: public Cmessage {
	public:
		/*宠物ID*/
		uint32_t		petid;
		/*第xx位*/
		uint32_t		bit;
		/*0 or 1*/
		uint32_t		state;

		mole2_set_pet_flag_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_set_skill_def_level_in: public Cmessage {
	public:
		/*设置人物时petid=0*/
		uint32_t		petid;
		/*技能ID*/
		uint32_t		skillid;
		/**/
		int32_t		def_level;

		mole2_set_skill_def_level_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_set_system_time_in: public Cmessage {
	public:
		/**/
		char		datetime[20]; 

		mole2_set_system_time_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_set_user_cli_buff_in: public Cmessage {
	public:
		/*类型*/
		uint32_t		type;
		/**/
		char		buff[512]; 

		mole2_set_user_cli_buff_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_set_vip_activity3_in: public Cmessage {
	public:
		/**/
		int32_t		val;

		mole2_set_vip_activity3_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_set_vip_activity_in: public Cmessage {
	public:
		/*增加值*/
		int32_t		val;

		mole2_set_vip_activity_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_set_vip_buff_in: public Cmessage {
	public:
		//
		std::vector<stru_vip_buff>		petlist;

		mole2_set_vip_buff_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_user_add_vip_ex_val_in: public Cmessage {
	public:
		/**/
		uint32_t		vip_ex_val;

		mole2_user_add_vip_ex_val_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_user_del_title_in: public Cmessage {
	public:
		/**/
		uint32_t		titleid;

		mole2_user_del_title_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_user_get_shop_name_out: public Cmessage {
	public:
		/*昵称*/
		char		nick[16]; 

		mole2_user_get_shop_name_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_user_pet_skill_add_in: public Cmessage {
	public:
		/*宠物ID*/
		uint32_t		petid;
		/*技能ID*/
		uint32_t		skillid;
		/*等级*/
		uint8_t		level;

		mole2_user_pet_skill_add_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_user_pet_skill_add_out: public Cmessage {
	public:
		/*宠物ID*/
		uint32_t		petid;
		/*技能ID*/
		uint32_t		skillid;
		/*等级*/
		uint8_t		level;

		mole2_user_pet_skill_add_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_user_pet_skill_del_in: public Cmessage {
	public:
		/*宠物ID*/
		uint32_t		petid;
		/*技能ID*/
		uint32_t		skillid;
		/*等级*/
		uint8_t		level;

		mole2_user_pet_skill_del_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_user_pet_skill_del_out: public Cmessage {
	public:
		/*宠物ID*/
		uint32_t		petid;
		/*技能ID*/
		uint32_t		skillid;
		/*等级*/
		uint8_t		level;

		mole2_user_pet_skill_del_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_user_set_shop_name_in: public Cmessage {
	public:
		/*昵称*/
		char		nick[16]; 

		mole2_user_set_shop_name_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_user_set_vip_base_val_in: public Cmessage {
	public:
		/**/
		uint32_t		growth_base;
		/**/
		uint16_t		chnl_id;

		mole2_user_set_vip_base_val_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_user_set_vip_info_in: public Cmessage {
	public:
		/*标志*/
		uint32_t		flag;
		/**/
		uint32_t		months;
		/*vip过期时间*/
		uint32_t		vip_end_time;
		/**/
		uint32_t		vip_auto;
		/**/
		uint32_t		vip_begin_time;
		/**/
		uint32_t		last_charge_chnl_id;
		/**/
		uint8_t		vip_type;

		mole2_user_set_vip_info_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_user_set_vip_level_in: public Cmessage {
	public:
		/**/
		uint32_t		vip_level;

		mole2_user_set_vip_level_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_user_update_rank_in: public Cmessage {
	public:
		/**/
		uint32_t		val;
		/*昵称*/
		char		nick[16]; 

		mole2_user_update_rank_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_user_user_create_role_in: public Cmessage {
	public:
		/**/
		uint32_t		parent;
		/*昵称*/
		char		nick[16]; 
		/*个性签名*/
		char		signature[32]; 
		/*颜色*/
		uint32_t		color;
		/*职业 0：无 1:剑士 2:弓箭手 3:魔法师 4:传教士*/
		uint8_t		professtion;

		mole2_user_user_create_role_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_user_user_update_nick_in: public Cmessage {
	public:
		/*昵称*/
		char		nick[16]; 

		mole2_user_user_update_nick_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class mole2_user_user_update_nick_out: public Cmessage {
	public:
		/*昵称*/
		char		nick[16]; 

		mole2_user_user_update_nick_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class struct_test_info: public Cmessage {
	public:
		/**/
		uint32_t		n1;
		/**/
		uint16_t		n2;
		//
		uint32_t _n3_len;
		char		n3[10];
		/**/
		char		n4[9]; 

		struct_test_info();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_attire_info: public Cmessage {
	public:
		/*物品ID*/
		uint32_t		attire_id;
		/*位置标识*/
		uint32_t		gridid;
		/*等级*/
		uint16_t		level;
		/**/
		uint16_t		mduration;
		/*耐久*/
		uint16_t		duration;
		/*体力最大值*/
		uint32_t		hpmax;
		/*魔法最大值*/
		uint32_t		mpmax;
		/*攻击*/
		uint16_t		atk;
		/*魔法攻击*/
		uint16_t		matk;
		/*防御*/
		uint16_t		def;
		/*魔法防御*/
		uint16_t		mdef;
		/*速度*/
		uint16_t		speed;
		/*精神*/
		uint16_t		spirit;
		/*恢复*/
		uint16_t		resume;
		/*命中值*/
		uint16_t		hit;
		/*闪避值*/
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
		/**/
		uint32_t		quality;
		/**/
		uint32_t		validday;
		/**/
		uint32_t		crystal_attr;
		/**/
		uint32_t		bless_type;

		stru_attire_info();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class stru_limit_item: public Cmessage {
	public:
		/**/
		uint32_t		eid;
		/**/
		uint32_t		itemid;
		/**/
		uint32_t		val;

		stru_limit_item();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_get_last_pet_list_out: public Cmessage {
	public:
		//
		std::vector<stru_pet_info>		pet_list;

		su_get_last_pet_list_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_add_black_in: public Cmessage {
	public:
		/**/
		uint32_t		black_userid;

		su_mole2_add_black_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_add_cloth_in: public Cmessage {
	public:
		/*物品ID*/
		uint32_t		attire_id;

		su_mole2_add_cloth_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_add_friend_in: public Cmessage {
	public:
		/**/
		uint32_t		friendid;

		su_mole2_add_friend_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_add_item_in: public Cmessage {
	public:
		/**/
		uint32_t		itemid;
		/**/
		int32_t		add_count;
		/**/
		int32_t		add_storage;

		su_mole2_add_item_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_add_mail_in: public Cmessage {
	public:
		/*类型*/
		uint32_t		type;
		/**/
		uint32_t		themeid;
		/**/
		uint32_t		sender_id;
		/**/
		char		sender_nick[64]; 
		/**/
		char		title[64]; 
		/**/
		char		message[256]; 
		/**/
		char		items[64]; 

		su_mole2_add_mail_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_add_pet_in: public Cmessage {
	public:
		/**/
		stru_pet_info		pet;

		su_mole2_add_pet_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_add_skill_in: public Cmessage {
	public:
		/*技巧*/
		stru_skill_info		skill;

		su_mole2_add_skill_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_add_task_in: public Cmessage {
	public:
		/*任务ID*/
		uint32_t		taskid;

		su_mole2_add_task_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_add_title_in: public Cmessage {
	public:
		/**/
		uint32_t		titleid;
		/*时间*/
		uint32_t		time;

		su_mole2_add_title_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_add_xiaomee_in: public Cmessage {
	public:
		/*小米*/
		int32_t		xiaomee;

		su_mole2_add_xiaomee_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_delete_cloths_in: public Cmessage {
	public:
		/*获取时间*/
		uint32_t		gettime;
		/**/
		uint32_t		clothid;

		su_mole2_delete_cloths_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_delete_title_in: public Cmessage {
	public:
		/**/
		uint32_t		titleid;

		su_mole2_delete_title_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_del_black_in: public Cmessage {
	public:
		/**/
		uint32_t		black_userid;

		su_mole2_del_black_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_del_friend_in: public Cmessage {
	public:
		/**/
		uint32_t		friendid;

		su_mole2_del_friend_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_del_pet_in: public Cmessage {
	public:
		/*宠物ID*/
		uint32_t		petid;

		su_mole2_del_pet_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_del_skill_in: public Cmessage {
	public:
		/*技能ID*/
		uint32_t		skillid;

		su_mole2_del_skill_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_del_task_in: public Cmessage {
	public:
		/*任务ID*/
		uint32_t		taskid;

		su_mole2_del_task_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_get_ban_flag_out: public Cmessage {
	public:
		/*标志*/
		uint32_t		flag;

		su_mole2_get_ban_flag_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_get_blacks_out: public Cmessage {
	public:
		//
		std::vector<uint32_t>		blacks;

		su_mole2_get_blacks_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_get_day_items_out: public Cmessage {
	public:
		//
		std::vector<stru_day_limit>		day_items;

		su_mole2_get_day_items_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_get_equips_out: public Cmessage {
	public:
		//
		std::vector<stru_cloth_info>		cloths;

		su_mole2_get_equips_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_get_expbox_out: public Cmessage {
	public:
		/**/
		uint32_t		expbox;

		su_mole2_get_expbox_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_get_friends_out: public Cmessage {
	public:
		//
		std::vector<uint32_t>		friends;

		su_mole2_get_friends_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_get_items_out: public Cmessage {
	public:
		//
		std::vector<stru_item_info>		items;

		su_mole2_get_items_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_get_pets_out: public Cmessage {
	public:
		//
		std::vector<stru_pet_info>		pets_in_bag;
		//
		std::vector<stru_pet_info>		pets_in_home;

		su_mole2_get_pets_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_get_skills_out: public Cmessage {
	public:
		//
		std::vector<stru_skill_info>		skills;

		su_mole2_get_skills_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_get_tasks_out: public Cmessage {
	public:
		//
		std::vector<stru_task_doing>		tasks_doing;
		//
		std::vector<stru_task_done>		tasks_done;

		su_mole2_get_tasks_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_get_user_out: public Cmessage {
	public:
		/*用户昵称*/
		char		nick[16]; 
		/*标志*/
		uint32_t		flag;
		/*颜色*/
		uint32_t		color;
		/**/
		uint32_t		regist_time;
		/*种族*/
		uint32_t		race;
		/**/
		uint32_t		profession;
		/**/
		uint32_t		joblevel;
		/*荣誉值*/
		uint32_t		honor;
		/*小米*/
		uint32_t		xiaomee;
		/*等级*/
		uint32_t		level;
		/*经验*/
		uint32_t		experience;
		/*体力*/
		uint32_t		physique;
		/*力量*/
		uint32_t		strength;
		/*耐力*/
		uint32_t		endurance;
		/*速度*/
		uint32_t		quick;
		/*智力*/
		uint32_t		intelligence;
		/*剩余属性点*/
		uint32_t		attr_addition;
		/*体力*/
		uint32_t		hp;
		/*魔法值*/
		uint32_t		mp;
		/*受伤等级 0:未受伤, 1:白 2:黄 3:红*/
		uint32_t		injury_state;
		/*人物战斗默认站位 1:前排 0:后排*/
		uint32_t		in_front;
		/*人物携带最大装备数*/
		uint32_t		max_attire;
		/*人物携带最大药剂数*/
		uint32_t		max_medicine;
		/*人物携带最大材料数*/
		uint32_t		max_stuff;
		/*时间*/
		uint32_t		time;
		/*坐标x轴*/
		uint32_t		axis_x;
		/*坐标y轴*/
		uint32_t		axis_y;
		/*地图编号*/
		uint32_t		mapid;
		/**/
		uint32_t		daytime;
		/**/
		uint32_t		fly_mapid;
		/**/
		uint32_t		expbox;
		/*电量值*/
		uint32_t		energy;
		/**/
		uint32_t		skill_expbox;
		/**/
		uint32_t		flag_ex;
		/**/
		uint32_t		winbossid;
		/**/
		uint32_t		parent;
		/**/
		stru_battle_info		battle;
		/*签到天数*/
		uint32_t		sign_count;

		su_mole2_get_user_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_get_vip_flag_out: public Cmessage {
	public:
		/*标志*/
		uint32_t		flag;

		su_mole2_get_vip_flag_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_get_vip_info_out: public Cmessage {
	public:
		/**/
		uint32_t		vip_auto_charge;
		/**/
		uint32_t		vip_level;
		/**/
		uint32_t		vip_value;
		/**/
		uint32_t		vip_base_value;
		/**/
		uint32_t		vip_end_time;
		/**/
		uint32_t		vip_begin_time;

		su_mole2_get_vip_info_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_set_ban_flag_in: public Cmessage {
	public:
		/*标志*/
		uint32_t		flag;

		su_mole2_set_ban_flag_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_set_base_attr_in: public Cmessage {
	public:
		/*体力*/
		int32_t		physique;
		/*力量*/
		int32_t		strength;
		/*耐力*/
		int32_t		endurance;
		/*速度*/
		int32_t		quick;
		/*智力*/
		int32_t		intelligence;
		/**/
		uint32_t		addition;

		su_mole2_set_base_attr_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_set_expbox_in: public Cmessage {
	public:
		/**/
		uint32_t		expbox;

		su_mole2_set_expbox_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_set_field_value_in: public Cmessage {
	public:
		/*1:user,2:user2,3:pet,4:attire 5:day item 6:task*/
		uint32_t		opt_type;
		/*petid, aittreid, itemid ,taskid*/
		uint32_t		opt_id;
		/*<font color="red">用户数据:nick 昵称 flag 标志位  color 颜色  regist_time 注册时间 race 种族  profession 职业 joblevel 职业等级 honor 荣誉值 xiaomee 星豆  level 等级 experience 经验 physique 体力 strength 力量 endurance 耐力 quick 速度 intelligence 智力 attr_addition 剩余属性点 hp 体力  mp 魔法值 injury_state 受伤等级 0:未受伤 in_front 人物战斗默认站位 1:前排 0:后排 max_attire 人物携带最大装备数 max_medicine 人物携带最大药剂数 max_stuff 人物携带最大材料数 time	时间 axis_x 坐标x轴 axis_y 坐标y轴 mapid 地图编号  expbox 经验树 energy 电量值 skill_expbox 技能经验树</font><br><font color="blue">宠物数据： gettime 获取时间 pettype 宠物类型ID race 种族 flag 标志  nick 昵称 level 等级 exp 经验 physiqueinit 体力初值 strengthinit 力量初值 enduranceinit 耐力初值 quickinit 速度初值 iqinit 智慧初值 physiqueparam 体力成长值 strengthparam 力量成长值 enduranceparam 耐力成长值 quickparam 速度成长值 iqparam 智慧成长值 physique_add 体力配点值 strength_add 力量配点值 endurance_add 耐力配点值 quick_add 速度配点值 iq_add 智慧配点值 attr_addition 剩余属性点 hp 体力 mp 魔法值 earth 地 water 水 fire 火 wind 风 injury_state 受伤等级 0:未受伤, 1:白 2:黄 3:红 location 位置. 0：未定义，1：休息， 2：待命， 3：出战，[10000,20000):仓库</font>*/
		char		field[32]; 
		/*数值*/
		char		value[255]; 

		su_mole2_set_field_value_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_set_nick_in: public Cmessage {
	public:
		/*昵称*/
		char		nick[16]; 

		su_mole2_set_nick_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_set_pet_in: public Cmessage {
	public:
		/**/
		stru_pet_info		pet;

		su_mole2_set_pet_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_set_position_in: public Cmessage {
	public:
		/**/
		uint32_t		position;

		su_mole2_set_position_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_set_profession_in: public Cmessage {
	public:
		/**/
		uint32_t		profession;

		su_mole2_set_profession_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_set_task_in: public Cmessage {
	public:
		/*任务ID*/
		uint32_t		taskid;
		/*1:未完成,2:已完成*/
		uint32_t		state;

		su_mole2_set_task_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_mole2_simple_opt_in: public Cmessage {
	public:
		/*1:user,2:user2,3:item,6:user_title*/
		uint32_t		table_type;
		/*1:add,2:delete,3:set,4:change*/
		uint32_t		opt_type;
		/**/
		uint32_t		v1;
		/**/
		uint32_t		v2;
		/**/
		uint32_t		v3;
		/**/
		int32_t		v4;
		/**/
		int32_t		v5;

		su_mole2_simple_opt_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class su_switch_send_msg_in: public Cmessage {
	public:
		/*消息*/
		char		msg[1024]; 

		su_switch_send_msg_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class user_home_t: public Cmessage {
	public:
		/*标志*/
		uint32_t		flag;
		/*类型*/
		uint32_t		type;
		/*等级*/
		uint32_t		level;
		/*经验*/
		uint32_t		exp;
		/**/
		char		name[16]; 
		/**/
		uint32_t		photo;
		/**/
		uint32_t		blackboard;
		/**/
		uint32_t		bookshelf;
		/**/
		uint32_t		honorbox;
		/**/
		uint32_t		petfun;
		/**/
		uint32_t		expbox;
		/**/
		uint32_t		effigy;
		/**/
		uint32_t		postbox;
		/**/
		uint32_t		itembox;
		/**/
		uint32_t		petbox;
		/**/
		uint32_t		compose1;
		/**/
		uint32_t		compose2;
		/**/
		uint32_t		compose3;
		/**/
		uint32_t		compose4;
		/**/
		uint32_t		wall;
		/**/
		uint32_t		floor;
		/**/
		uint32_t		bed;
		/**/
		uint32_t		desk;
		/**/
		uint32_t		ladder;
		/**/
		uint32_t		petbox_cnt;

		user_home_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
#endif // MOLE2_DB_H
