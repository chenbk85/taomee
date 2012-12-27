/**
 *============================================================
 *  @file      BossAI.hpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_BOSS_AI_HPP_
#define KFBTL_BOSS_AI_HPP_

extern "C" {
#include <libtaomee/log.h>
}

class HighLvAI;
class Player;

/****************************************************************
 * normal Boss
 ****************************************************************/
class BossAI : public HighLvAI {
public:
	~BossAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static BossAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	BossAI(){ TRACE_LOG("BossAI create"); }
};

inline BossAI*
BossAI::instance()
{
	static BossAI instance;

	return &instance;
}

/****************************************************************
 * Pumpkin Boss
 ****************************************************************/
class PumpkinBossAI : public HighLvAI {
public:
	~PumpkinBossAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static PumpkinBossAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
	/**
	  * @brief boss teleport
	  */
	//void teleport(Player* player, uint32_t x, uint32_t y);
	
private:
	PumpkinBossAI(){ TRACE_LOG("PumpkinBossAI create"); }
};

inline PumpkinBossAI*
PumpkinBossAI::instance()
{
	static PumpkinBossAI instance;

	return &instance;
}

/****************************************************************
 * BellCat Boss
 ****************************************************************/
class BellCatAI : public HighLvAI {
public:
	~BellCatAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static BellCatAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	BellCatAI(){ TRACE_LOG("BellCatAI create"); }
};

inline BellCatAI*
BellCatAI::instance()
{
	static BellCatAI instance;

	return &instance;
}
 
/****************************************************************
 * Bitores Boss
 ****************************************************************/
class BitoresAI : public HighLvAI {
public:
	~BitoresAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static BitoresAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	BitoresAI(){ TRACE_LOG("BitoresAI create"); }
};

inline BitoresAI*
BitoresAI::instance()
{
	static BitoresAI instance;

	return &instance;
}

/****************************************************************
 * Balu Boss
 ****************************************************************/
class BaluAI : public HighLvAI {
public:
	~BaluAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static BaluAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	BaluAI(){ TRACE_LOG("BaluAI create"); }
};

inline BaluAI*
BaluAI::instance()
{
	static BaluAI instance;

	return &instance;
}

/****************************************************************
 * Chifeng Boss
 ****************************************************************/
class ChifengAI : public HighLvAI {
public:
	~ChifengAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static ChifengAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	ChifengAI(){ TRACE_LOG("ChifengAI create"); }
};

inline ChifengAI*
ChifengAI::instance()
{
	static ChifengAI instance;

	return &instance;
}

/****************************************************************
 * RabBro1 Boss
 ****************************************************************/
class RabBro1AI : public HighLvAI {
public:
	~RabBro1AI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static RabBro1AI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	RabBro1AI(){ TRACE_LOG("RabBro1AI create"); }
	
	/**
	  * @brief check other rabbit if dead
	  */
	bool check_other_bro(Player* player);
};

inline RabBro1AI*
RabBro1AI::instance()
{
	static RabBro1AI instance;

	return &instance;
}

/****************************************************************
 * RabBro2 Boss
 ****************************************************************/
class RabBro2AI : public HighLvAI {
public:
	~RabBro2AI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static RabBro2AI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	RabBro2AI(){ TRACE_LOG("RabBro2AI create"); }
	
	/**
	  * @brief check other rabbit if dead
	  */
	bool check_other_bro(Player* player);
};

inline RabBro2AI*
RabBro2AI::instance()
{
	static RabBro2AI instance;

	return &instance;
}

/****************************************************************
 * RabBro3 Boss
 ****************************************************************/
class RabBro3AI : public HighLvAI {
public:
	~RabBro3AI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static RabBro3AI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	RabBro3AI(){ TRACE_LOG("RabBro3AI create"); }
	
	/**
	  * @brief check other rabbit if dead
	  */
	bool check_other_bro(Player* player);
};

inline RabBro3AI*
RabBro3AI::instance()
{
	static RabBro3AI instance;

	return &instance;
}

/****************************************************************
 * Rabbits Boss
 ****************************************************************/
class RabbitsAI : public HighLvAI {
public:
	~RabbitsAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static RabbitsAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
protected:
	RabbitsAI(){ TRACE_LOG("RabbitsAI create"); }

};

inline RabbitsAI*
RabbitsAI::instance()
{
	static RabbitsAI instance;

	return &instance;
}

/****************************************************************
 * ChuanYin Boss
 ****************************************************************/
class ChuanYinAI : public HighLvAI {
public:
	~ChuanYinAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static ChuanYinAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	ChuanYinAI(){ TRACE_LOG("ChuanYinAI create"); }
	
};

inline ChuanYinAI*
ChuanYinAI::instance()
{
	static ChuanYinAI instance;

	return &instance;
}

/****************************************************************
 * HaBo Boss
 ****************************************************************/
class HaBoAI : public HighLvAI {
public:
	~HaBoAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static HaBoAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	HaBoAI(){ TRACE_LOG("HaBoAI create"); }
	
};

inline HaBoAI*
HaBoAI::instance()
{
	static HaBoAI instance;

	return &instance;
}

/****************************************************************
 * JiXuan Boss
 ****************************************************************/
class JiXuanAI : public HighLvAI {
public:
	~JiXuanAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static JiXuanAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	JiXuanAI(){ TRACE_LOG("JiXuanAI create"); }
	
};

inline JiXuanAI*
JiXuanAI::instance()
{
	static JiXuanAI instance;

	return &instance;
}

/****************************************************************
 * JiaCi Boss
 ****************************************************************/
class JiaCiAI : public HighLvAI {
public:
	~JiaCiAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static JiaCiAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	JiaCiAI(){ TRACE_LOG("JiaCiAI create"); }
	
};

inline JiaCiAI*
JiaCiAI::instance()
{
	static JiaCiAI instance;

	return &instance;
}

/****************************************************************
 * YouQi Boss
 ****************************************************************/
class YouQiAI : public HighLvAI {
public:
	~YouQiAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static YouQiAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	YouQiAI(){ TRACE_LOG("YouQiAI create"); }
	
};

inline YouQiAI*
YouQiAI::instance()
{
	static YouQiAI instance;

	return &instance;
}

/****************************************************************
 * ShaKe Boss
 ****************************************************************/
class ShaKeAI : public HighLvAI {
public:
	~ShaKeAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static ShaKeAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	ShaKeAI(){ TRACE_LOG("ShaKeAI create"); }
	
};

inline ShaKeAI*
ShaKeAI::instance()
{
	static ShaKeAI instance;

	return &instance;
}

/****************************************************************
 * MangYa Boss
 ****************************************************************/
class MangYaAI : public HighLvAI {
public:
	~MangYaAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static MangYaAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	MangYaAI(){ TRACE_LOG("MangYaAI create"); }
	
};

inline MangYaAI*
MangYaAI::instance()
{
	static MangYaAI instance;

	return &instance;
}

/****************************************************************
 * DaoLang Boss
 ****************************************************************/
class DaoLangAI : public HighLvAI {
public:
	~DaoLangAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static DaoLangAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
	/**
	  * @brief extra defense action
	  */
	bool extra_defense_action(Player* player);
	
private:
	DaoLangAI(){ TRACE_LOG("DaoLangAI create"); }
	
};

inline DaoLangAI*
DaoLangAI::instance()
{
	static DaoLangAI instance;

	return &instance;
}

/****************************************************************
 *  * Moyi Boss
****************************************************************/
class MoyiAI : public HighLvAI
{
public:
	~MoyiAI(){}
	 static MoyiAI* instance();
	 bool extra_wait_action(Player* player);
	 bool extra_attack_action(Player* player);
	 bool extra_stuck_action(Player* player);
	 bool extra_defense_action(Player* player);
private:
	 MoyiAI(){ TRACE_LOG("MoyiAI create"); }
};

inline MoyiAI* 
MoyiAI::instance()
{
	static MoyiAI instance;
	return &instance;
}

/****************************************************************
 *  * Hunfu Boss
****************************************************************/

class HunfuAI: public HighLvAI
{
public:
	~HunfuAI(){}
	 static HunfuAI* instance();
	 bool extra_wait_action(Player* player);
	 bool extra_attack_action(Player* player);
	 bool extra_stuck_action(Player* player);
	 bool extra_defense_action(Player* player);
	 void move(Player* player, int millisec);
private:
	HunfuAI()
	{ 
		TRACE_LOG("HunfuAI create"); 
	}
};

inline HunfuAI* 
HunfuAI::instance()
{
	static HunfuAI instance;
	return &instance;
}

/****************************************************************
 * * lichi touling Boss
****************************************************************/
class lichi_toulingAI: public HighLvAI
{
public:
    ~lichi_toulingAI(){}
	static lichi_toulingAI* instance();
	bool extra_wait_action(Player* player);
	bool extra_attack_action(Player* player);
	bool extra_stuck_action(Player* player);
	bool extra_defense_action(Player* player);
public:
	bool check_call_monster_condition(Player* player);
private:
	lichi_toulingAI()
	{ 
		TRACE_LOG("lichi_toulingAI create"); 
	}
};


inline lichi_toulingAI*
lichi_toulingAI::instance()
{
	static lichi_toulingAI instance;
	return &instance;
}

/****************************************************************
  * * xili Boss
****************************************************************/

class xili_AI : public HighLvAI
{
public:
	~xili_AI(){}
	static xili_AI* instance();
public:
	bool extra_wait_action(Player* player);
	bool extra_attack_action(Player* player);
	bool extra_stuck_action(Player* player);
	bool extra_defense_action(Player* player);
public:
	bool check_call_monster_condition(Player* player);
private:
	xili_AI()
	{
		TRACE_LOG("xili_AI create");
	}
};

inline xili_AI* 
xili_AI::instance()
{
	static xili_AI instance;
	return &instance;
}

/****************************************************************
	* * huanying mowang Boss
****************************************************************/

class huanying_AI: public HighLvAI
{
public:
	~huanying_AI(){}
	static huanying_AI* instance();
public:
	bool extra_wait_action(Player* player);
	bool extra_attack_action(Player* player);
	bool extra_stuck_action(Player* player);
	bool extra_defense_action(Player* player);
public:
	int32_t get_idol_count(Player* player);
	Player* get_current_player(Player* player);
	uint32_t get_idol_id(Player* player);
private:
	huanying_AI()
	{
		TRACE_LOG("huanying_AI create");
	}
};

inline huanying_AI*
huanying_AI::instance()
{
	static huanying_AI instance;
	return &instance;
}


/****************************************************************
  fumo Rabbits Boss
****************************************************************/

class FumoRabbits_AI: public RabbitsAI
{
public:
	~FumoRabbits_AI(){}
	static FumoRabbits_AI* instance();
public:
	bool extra_wait_action(Player* player);
	bool extra_attack_action(Player* player);
	bool extra_stuck_action(Player* player);
	bool extra_defense_action(Player* player);
public:
	int32_t check_other_rabbits_count(Player* player);
private:
	FumoRabbits_AI()
	{
		TRACE_LOG("FumoRabbits_AI create");
	}
};

inline FumoRabbits_AI*
FumoRabbits_AI::instance()
{
	static FumoRabbits_AI instance;
	return &instance;
}


/****************************************************************
 callcat Boss
****************************************************************/

class CallCat_AI: public HighLvAI
{
public:
	~CallCat_AI(){}
	static CallCat_AI* instance();
public:
	bool extra_wait_action(Player* player);
	bool extra_attack_action(Player* player);
	bool extra_stuck_action(Player* player);
	bool extra_defense_action(Player* player);
public:
	bool check_call_condition(Player* player);
private:
	CallCat_AI()
	{
		TRACE_LOG("CallCat_AI create");
	}
};

inline CallCat_AI* 
CallCat_AI::instance()
{
	static CallCat_AI instance;
	return &instance;
}


/****************************************************************
 *  callcat Boss
****************************************************************/
class YaoYaoBoss_AI: public HighLvAI
{
public:
	~YaoYaoBoss_AI(){}
	static YaoYaoBoss_AI* instance();
public:
	bool extra_wait_action(Player* player);
	bool extra_attack_action(Player* player);
	bool extra_stuck_action(Player* player);
	bool extra_defense_action(Player* player);
public:
	uint32_t get_stone_count(Player* player);
private:
	YaoYaoBoss_AI()
	{
		TRACE_LOG("YaoYaoBoss_AI create");
	}
};

inline YaoYaoBoss_AI* 
YaoYaoBoss_AI::instance()
{
	static YaoYaoBoss_AI instance;
	return &instance;
}


/****************************************************************
 *  *  Joe_Step1 Boss
****************************************************************/

class Joe_Step1AI: public HighLvAI
{
public:
	~Joe_Step1AI(){}
	static Joe_Step1AI* instance();
public:
	bool extra_wait_action(Player* player);
	bool extra_attack_action(Player* player);
	bool extra_stuck_action(Player* player);
	bool extra_defense_action(Player* player);
public:
	uint32_t get_batman_count(Player* player);
	uint32_t get_idol_count(Player* player);
private:
	Joe_Step1AI()
	{
		TRACE_LOG("Joe_Step1AI create");
	}
};
inline Joe_Step1AI*
Joe_Step1AI::instance()
{
	static Joe_Step1AI instance;
	return &instance;
}

/****************************************************************
 *  *  *  Joe_Step2 Boss
****************************************************************/
class Joe_Step2AI: public HighLvAI
{
public:
	~Joe_Step2AI(){}
	static Joe_Step2AI* instance();
public:
	bool extra_wait_action(Player* player);
	bool extra_attack_action(Player* player);
	bool extra_stuck_action(Player* player);
	bool extra_defense_action(Player* player);
private:
	Joe_Step2AI()
	{
		TRACE_LOG("Joe_Step2AI create");
	}
};

inline Joe_Step2AI*
Joe_Step2AI::instance()
{
	static Joe_Step2AI instance;
	return &instance;
}

/****************************************************************
 *  *  *  *  farui Boss
****************************************************************/
class Farui_AI: public HighLvAI
{
public:
	~Farui_AI(){}
	static Farui_AI* instance();
public:
	bool extra_wait_action(Player* player);
	bool extra_attack_action(Player* player);
	bool extra_stuck_action(Player* player);
	bool extra_defense_action(Player* player);
private:
	Farui_AI()
	{
		TRACE_LOG("Farui_AI create");
	}		
};

inline Farui_AI* 
Farui_AI::instance()
{
	static Farui_AI instance;
	return &instance;
}


/****************************************************************
 *  *  *  *  *  farui Boss
****************************************************************/
class Lengyue_AI: public HighLvAI
{
public:
	~Lengyue_AI(){}
	static Lengyue_AI* instance();
public:
	bool extra_wait_action(Player* player);
	bool extra_attack_action(Player* player);
	bool extra_stuck_action(Player* player);
	bool extra_defense_action(Player* player);
private:
	Lengyue_AI()
	{
		TRACE_LOG("Lengyue_AI create");
	}
};

inline Lengyue_AI*
Lengyue_AI::instance()
{
	static Lengyue_AI instance;
	return &instance;
}


/****************************************************************
 *  *  *  *  *  *  fish Boss
****************************************************************/

class Fish_AI: public HighLvAI
{
public:
	~Fish_AI(){}
   static Fish_AI* instance();
public:
	bool extra_wait_action(Player* player);
 	bool extra_attack_action(Player* player);
	bool extra_stuck_action(Player* player);
	bool extra_defense_action(Player* player);
	void move(Player* player, int millisec);
public:
	uint32_t get_bonbon_count(Player* player);
	bool get_random_bonbon_pos(Player* player, uint32_t& x,  uint32_t& y);
private:
	Fish_AI()
	{
		TRACE_LOG("Fish_AI create");
	}	
};

inline Fish_AI*
Fish_AI::instance()
{
	static Fish_AI instance;
	return &instance;
}

/****************************************************************
 *  *  *  *  *  *  *  Balrog_AI Boss
****************************************************************/

class Balrog_AI: public HighLvAI
{
public:
	~Balrog_AI(){}
	static Balrog_AI* instance();
public:
	bool extra_wait_action(Player* player);
	bool extra_attack_action(Player* player);
	bool extra_stuck_action(Player* player);
	bool extra_defense_action(Player* player);
public:
	bool call_volcano(Player* player);
private:
	Balrog_AI()
	{
		TRACE_LOG("Balrog_AI create");
	}
};

inline Balrog_AI*
Balrog_AI::instance()
{
	static Balrog_AI instance;
	return &instance;
}

/** 
 * @brief TuMars_AI for boss 
 */
class TuMars_AI: public HighLvAI 
{
public:
	~TuMars_AI(){}
	static TuMars_AI * instance();
	bool extra_wait_action(Player* player);
 	bool extra_attack_action(Player* player);
	bool extra_stuck_action(Player* player);
	bool extra_defense_action(Player* player);
private:
	TuMars_AI()
	{
		TRACE_LOG("TuMars_AI create");
	}
};
inline TuMars_AI *
TuMars_AI::instance()
{
	static TuMars_AI instance;
	return &instance;
}

class Shark_AI : public HighLvAI
{
	public:
		~Shark_AI(){}
		static Shark_AI * instance();
		bool extra_wait_action(Player * player);
		bool extra_attack_action(Player * player);
		bool extra_stuck_action(Player * player);
		bool extra_defense_action(Player * player);

		void fish_attack(Player * player);
		void get_next_pos(Player * player);
	private:
		Shark_AI()
		{
			TRACE_LOG("Shark_AI create");
		}

};

inline Shark_AI *
Shark_AI::instance() 
{
	static Shark_AI instance;
	return &instance;
}

class Dmhunter_AI : public HighLvAI
{
public:
	~Dmhunter_AI(){}
	static Dmhunter_AI * instance();
	bool extra_wait_action(Player * player);
	bool extra_attack_action(Player * player);
	bool extra_stuck_action(Player * player);
	bool extra_defense_action(Player * player);
	void move(Player * player, int millisec);
	bool check_ghost_fire(Player * player);
private:
	Dmhunter_AI()
	{
		TRACE_LOG("Dmhunter_AI creater");
	}

};	

inline Dmhunter_AI *
Dmhunter_AI::instance()
{
	static Dmhunter_AI instance;
	return &instance;

}



/** 
 * @brief 元素炼狱BOSS
 */
class YuanSu_AI : public HighLvAI
{
	public:
		~YuanSu_AI(){}
		static YuanSu_AI * instance();
		bool extra_wait_action(Player * player);
		bool extra_stuck_action(Player * player);
		bool extra_defense_action(Player * player);
		void move(Player * player, int millisec);
	private:
		YuanSu_AI()
		{
			TRACE_LOG("YuanSu_AI create!");
		}
};

inline YuanSu_AI *
YuanSu_AI::instance()
{
	static YuanSu_AI instance;
	return &instance;
}
/** 
 * @brief 三段梅花流BOSS
 */
class ThreeWoodBossL_AI : public HighLvAI
{
	public:
		~ThreeWoodBossL_AI(){}
		static ThreeWoodBossL_AI * instance();
		bool extra_wait_action(Player * player);
		void move(Player * player, int millisec);
	private:
		ThreeWoodBossL_AI()
		{
			TRACE_LOG("ThreeWood_AI create!");
		}
};

inline ThreeWoodBossL_AI *
ThreeWoodBossL_AI::instance()
{
	static ThreeWoodBossL_AI instance;
	return &instance;
}
class ThreeWoodBossR_AI : public HighLvAI
{
	public:
		~ThreeWoodBossR_AI(){}
		static ThreeWoodBossR_AI * instance();
		bool extra_wait_action(Player * player);
		void move(Player * player, int millisec);
	private:
		ThreeWoodBossR_AI()
		{
			TRACE_LOG("ThreeWood_AI create!");
		}
};

inline ThreeWoodBossR_AI *
ThreeWoodBossR_AI::instance()
{
	static ThreeWoodBossR_AI instance;
	return &instance;
}


/** 
 * @brief 天下第一比武大会派派侠士
 */
class ConPP_AI : public HighLvAI
{
	public:
		~ConPP_AI(){}
	   	static	ConPP_AI * instance();
		bool extra_wait_action(Player * player);
	//	void move(Player * player, int timediff);
	private:
		ConPP_AI()
		{
			TRACE_LOG("ConPP_AI create!");
		}
};

inline ConPP_AI *
ConPP_AI::instance()
{
	static ConPP_AI instance;
	return &instance;
}

/** 
 * @brief 伊尔侠士
 */
class ConYiEr_AI : public HighLvAI
{
	public:
		~ConYiEr_AI(){}
		static ConYiEr_AI * instance();
		bool extra_wait_action(Player * player);
		void move(Player * player, int timediff);
	private:
		ConYiEr_AI()
		{
			TRACE_LOG("ConYiEr_AI create!");

		}
};

inline ConYiEr_AI *
ConYiEr_AI::instance()
{
	static ConYiEr_AI instance;
	return &instance;
}

/** 
 * @brief  大竹侠士
 */
class ConDaZhu_AI : public HighLvAI
{
	public:
		~ConDaZhu_AI(){}
		static ConDaZhu_AI * instance();
		bool extra_wait_action(Player * player);
	//	void move(Player * player, int timediff);
	private:
		ConDaZhu_AI()
		{
			TRACE_LOG("ConDaZhu_AI create!");
		}
};

inline ConDaZhu_AI *
ConDaZhu_AI::instance()
{
	static ConDaZhu_AI instance;
	return &instance;
}

/** 
 * @brief 傲天侠士
 */
class ConAoTian_AI : public HighLvAI
{
	public:
		~ConAoTian_AI(){}
		static ConAoTian_AI * instance();
		bool extra_wait_action(Player * player);
	//	void move(Player * player, int timediff);
	private:
		ConAoTian_AI()
		{
			TRACE_LOG("ConAoTian_AI create!");
		}
};

inline ConAoTian_AI *
ConAoTian_AI::instance()
{
	static ConAoTian_AI instance;
	return &instance;
}


/** 
 * @brief 淘汰赛三段梅花流BOSS
 */
class TaoTaiBoss_AI : public HighLvAI
{
	public:
		~TaoTaiBoss_AI(){}
		static TaoTaiBoss_AI * instance();
		bool extra_wait_action(Player * player);
		void move(Player * player, int millisec);
	private:
		TaoTaiBoss_AI()
		{
			TRACE_LOG("TaoTai_AI create!");
		}
};

inline TaoTaiBoss_AI *
TaoTaiBoss_AI::instance()
{
	static TaoTaiBoss_AI instance;
	return &instance;
}



class PetMonster_AI: public HighLvAI
{
	public:
		~PetMonster_AI(){}
		static PetMonster_AI* instance();
		bool extra_wait_action(Player* player);
	private:
		PetMonster_AI()
		{
			TRACE_LOG("PetMonster_AI create");
		}
};

inline PetMonster_AI* PetMonster_AI::instance()
{
	static PetMonster_AI instance ;
	return &instance;
}

/////////////////////////////////////////
/*
class ShadowRat_AI: public SummonAI
{
	public:
		~ShadowRat_AI(){}
		void attack(Player* player, int millisec);
		static ShadowRat_AI* instance();
	private:
		ShadowRat_AI(){
			TRACE_LOG("ShadowRat_AI create");
		}
};


inline ShadowRat_AI*
ShadowRat_AI::instance()
{
	static ShadowRat_AI instance;
	return &instance;
}
*/


/** 
 * @brief 伏魔派派侠士
 */
class FMPP_AI : public HighLvAI
{
	public:
		~FMPP_AI(){}
	   	static	FMPP_AI * instance();
		bool extra_wait_action(Player * player);
	//	void move(Player * player, int timediff);
	private:
		FMPP_AI()
		{
			TRACE_LOG("FMPP_AI create!");
		}
};

inline FMPP_AI *
FMPP_AI::instance()
{
	static FMPP_AI instance;
	return &instance;
}

/** 
 * @brief 伏魔伊尔侠士
 */
class FMYiEr_AI : public HighLvAI
{
	public:
		~FMYiEr_AI(){}
		static FMYiEr_AI * instance();
		bool extra_wait_action(Player * player);
		void move(Player * player, int timediff);
	private:
		FMYiEr_AI()
		{
			TRACE_LOG("FMYiEr_AI create!");

		}
};

inline FMYiEr_AI *
FMYiEr_AI::instance()
{
	static FMYiEr_AI instance;
	return &instance;
}

/** 
 * @brief  伏魔大竹侠士
 */
class FMDaZhu_AI : public HighLvAI
{
	public:
		~FMDaZhu_AI(){}
		static FMDaZhu_AI * instance();
		bool extra_wait_action(Player * player);
	//	void move(Player * player, int timediff);
	private:
		FMDaZhu_AI()
		{
			TRACE_LOG("FMDaZhu_AI create!");
		}
};

inline FMDaZhu_AI *
FMDaZhu_AI::instance()
{
	static FMDaZhu_AI instance;
	return &instance;
}

/** 
 * @brief 伏魔傲天侠士
 */
class FMAoTian_AI : public HighLvAI
{
	public:
		~FMAoTian_AI(){}
		static FMAoTian_AI * instance();
		bool extra_wait_action(Player * player);
//		void move(Player * player, int timediff);
	private:
		FMAoTian_AI()
		{
			TRACE_LOG("FMAoTian_AI create!");
		}
};

inline FMAoTian_AI *
FMAoTian_AI::instance()
{
	static FMAoTian_AI instance;
	return &instance;
}

class JinJi_AI : public HighLvAI
{
	public:
		~JinJi_AI(){}
		static JinJi_AI * instance();
		bool extra_wait_action(Player * player);
		bool extra_stuck_action(Player * player);
		bool extra_defense_action(Player * player);
	
	private:
		JinJi_AI()
		{
			TRACE_LOG("JinJi_AI create!");
		}
};

inline JinJi_AI *
JinJi_AI::instance()
{
	static JinJi_AI instance;
	return &instance;
}

class WoodsCold_AI : public HighLvAI
{
	public:
		~WoodsCold_AI(){}
		static WoodsCold_AI * instance();
		bool extra_wait_action(Player * player);
	private:
		WoodsCold_AI()
		{
			TRACE_LOG("WoodsCold_AI creatr!");
		}
};

inline  WoodsCold_AI * 
WoodsCold_AI::instance()
{
	static WoodsCold_AI instance;
	return &instance;
}

//魔牙之怒
class GhostTeeth_AI : public HighLvAI
{
	public:
		~GhostTeeth_AI(){}

		static GhostTeeth_AI * instance();

		bool extra_wait_action(Player * player);

		bool extra_stuck_action(Player * player);

		bool extra_defense_action(Player * player);

		void move(Player * player, int timediff);

	private:
		GhostTeeth_AI()
		{
			TRACE_LOG("GhostTeeth_AI create!");
		}
};	

inline GhostTeeth_AI *
GhostTeeth_AI::instance()
{
	static GhostTeeth_AI instance;
	return &instance;
}

class YaoShi_AI : public HighLvAI
{
 public:
	~YaoShi_AI(){}

	static YaoShi_AI * instance();

	bool extra_wait_action(Player * player);
 private:
	YaoShi_AI()
	{
		TRACE_LOG("YaoShi_AI create!");
	}
};

inline YaoShi_AI * YaoShi_AI::instance()
{
	static YaoShi_AI instance;
	return &instance;
}

class EggBoss_AI : public HighLvAI
{
 public:
  ~EggBoss_AI(){}
  static EggBoss_AI * instance();
  bool extra_wait_action(Player * player);
 private:
  EggBoss_AI()
  {
    TRACE_LOG("EggBoss_AI created!");
  }
};

inline EggBoss_AI * EggBoss_AI::instance()
{
	static EggBoss_AI instance;
	return &instance;
}

class IceDragon_AI : public HighLvAI
{
 public:
  ~IceDragon_AI(){}
  static IceDragon_AI * instance();
  bool extra_wait_action(Player * player);
 private:
  IceDragon_AI()
  {
	  TRACE_LOG("IceDragon_AI created!");
  }
};

inline IceDragon_AI * 
IceDragon_AI::instance()
{
	static IceDragon_AI instance;
	return &instance;
}

class ShadowDragon_AI : public HighLvAI
{
 public:
  ~ShadowDragon_AI(){}
  static ShadowDragon_AI * instance();
  bool extra_wait_action(Player * player);
 private:
  ShadowDragon_AI()
  {
	  TRACE_LOG("ShadowDragon_AI created!");
  }
};

inline ShadowDragon_AI * 
ShadowDragon_AI::instance()
{
	static ShadowDragon_AI instance;
	return &instance;
}



class DarkDragon_AI : public HighLvAI
{ 
 public:
  ~DarkDragon_AI(){}
  
  bool extra_wait_action(Player * player);

  static DarkDragon_AI * instance();
 private:
  DarkDragon_AI()
  {
    TRACE_LOG("DarkDragonAI created!");
  }
};

inline DarkDragon_AI *
DarkDragon_AI::instance()
{
  static DarkDragon_AI instance;
  return &instance;
}

class FightDragon_AI : public HighLvAI
{
 public:
  ~FightDragon_AI(){}
  static FightDragon_AI * instance();
  bool extra_wait_action(Player * player);
 private:
  FightDragon_AI()
  {
    TRACE_LOG("FightDragon_AI create!");
  }
};
inline FightDragon_AI *
FightDragon_AI::instance()
{
  static FightDragon_AI instance;
  return &instance;
}

class FuryDragon_AI : public HighLvAI
{
 public:
  ~FuryDragon_AI(){}
  static FuryDragon_AI * instance();
  bool extra_wait_action(Player * player);
 private:
  FuryDragon_AI()
  {
	  TRACE_LOG("FuryDragon_AI created!");
  }
};

inline FuryDragon_AI *
FuryDragon_AI::instance()
{
 static FuryDragon_AI instance;
 return &instance;
}

class XueYao_AI : public HighLvAI
{
 public:
	 ~XueYao_AI() {}
	 static XueYao_AI * instance();
	 bool extra_wait_action(Player * player);
 private:
	 XueYao_AI()
	 {
		 TRACE_LOG("PREHISTORY WuSeng create!");
	 }
};

inline XueYao_AI *
XueYao_AI::instance()
{
 static XueYao_AI instance;
 return &instance;
}

class Fenshenkuilei_AI : public HighLvAI
{
 public:
	 ~Fenshenkuilei_AI() {}
	 static Fenshenkuilei_AI * instance();
	 bool extra_wait_action(Player * player);

	 bool extra_stuck_action(Player * player);

	 bool extra_attack_action(Player* player);
 private:
	 Fenshenkuilei_AI()
	 {
		 TRACE_LOG("PREHISTORY WuSeng create!");
	 }
};

inline Fenshenkuilei_AI *
Fenshenkuilei_AI::instance()
{
 static Fenshenkuilei_AI instance;
 return &instance;
}


/****************************************************************
 * nine head dragon Boss
 ****************************************************************/
class NineHeadDragonAI : public HighLvAI {
public:
	~NineHeadDragonAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static NineHeadDragonAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	NineHeadDragonAI(){ TRACE_LOG("NineHeadDragonAI create"); }
};

inline NineHeadDragonAI*
NineHeadDragonAI::instance()
{
	static NineHeadDragonAI instance;

	return &instance;
}

/****************************************************************
 * dagui Boss
 ****************************************************************/
class DaguiAI : public HighLvAI {
public:
	~DaguiAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static DaguiAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);
	
	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	DaguiAI(){ TRACE_LOG("DaguiAI create"); }
};

inline DaguiAI*
DaguiAI::instance()
{
	static DaguiAI instance;

	return &instance;
}

#endif // KFBTL_BOSS_AI_HPP_




