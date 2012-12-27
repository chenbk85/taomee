/**
 *============================================================
 *  @file      SpecialMonAI.hpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_SPECIAL_MON_AI_HPP_
#define KFBTL_SPECIAL_MON_AI_HPP_

extern "C" {
#include <libtaomee/log.h>
}

class MonsterAI;
class HighLvAI;
class MiddleLvAI;
class Player;

/****************************************************************
 * melon mole monster
 ****************************************************************/
class MoleAI : public MonsterAI {
public:
	~MoleAI() {}
	
	void wait(Player* player, int millisec);
	void linger(Player* player, int millisec);
	void move(Player* player, int millisec);
	void evade(Player* player, int millisec);
	void attack(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	void defense(Player* player, int millisec);
	
	//void UpdateAI(uint32 millisec );

	/**
	  * @brief this is a singleton
	  */
	static MoleAI* instance();
	
private:
	MoleAI(){ TRACE_LOG("MoleAI create"); }
};

inline MoleAI*
MoleAI::instance()
{
	static MoleAI instance;

	return &instance;
}

/****************************************************************
 * red eye monkey monster
 ****************************************************************/
class RedEyeMonkeyAI : public MonsterAI {
public:
	~RedEyeMonkeyAI() {}
	
	void wait(Player* player, int millisec);
	void linger(Player* player, int millisec);
	void move(Player* player, int millisec);
	void evade(Player* player, int millisec);
	void attack(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	void defense(Player* player, int millisec);
	
	//void UpdateAI(uint32 millisec );

	/**
	  * @brief this is a singleton
	  */
	static RedEyeMonkeyAI* instance();
	
private:
	RedEyeMonkeyAI(){ TRACE_LOG("RedEyeMonkeyAI create"); }
};

inline RedEyeMonkeyAI*
RedEyeMonkeyAI::instance()
{
	static RedEyeMonkeyAI instance;

	return &instance;
}

/****************************************************************
 * Leopard monster
 ****************************************************************/
class LeopardAI : public MonsterAI {
public:
	~LeopardAI() {}
	
	void wait(Player* player, int millisec);
	void linger(Player* player, int millisec);
	void move(Player* player, int millisec);
	void evade(Player* player, int millisec);
	void attack(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	void defense(Player* player, int millisec);
	
	//void UpdateAI(uint32 millisec );

	/**
	  * @brief this is a singleton
	  */
	static LeopardAI* instance();
	
private:
	LeopardAI(){ TRACE_LOG("LeopardAI create"); }
	
	player_skill_t*  select_crit_skill(Player* player, const Player* target);
};

inline LeopardAI*
LeopardAI::instance()
{
	static LeopardAI instance;

	return &instance;
}

/****************************************************************
 * JungleWolf monster
 ****************************************************************/
class JungleWolfAI : public MonsterAI {
public:
	~JungleWolfAI() {}
	
	void wait(Player* player, int millisec);
	void linger(Player* player, int millisec);
	void move(Player* player, int millisec);
	void evade(Player* player, int millisec);
	void attack(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	void defense(Player* player, int millisec);
	
	//void UpdateAI(uint32 millisec );

	/**
	  * @brief this is a singleton
	  */
	static JungleWolfAI* instance();
	
private:
	JungleWolfAI(){ TRACE_LOG("JungleWolfAI create"); }
};

inline JungleWolfAI*
JungleWolfAI::instance()
{
	static JungleWolfAI instance;

	return &instance;
}

/****************************************************************
 * Gorilla monster
 ****************************************************************/
class GorillaAI : public MonsterAI {
public:
	~GorillaAI() {}
	
	void wait(Player* player, int millisec);
	void linger(Player* player, int millisec);
	void move(Player* player, int millisec);
	void evade(Player* player, int millisec);
	void attack(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	void defense(Player* player, int millisec);
	
	//void UpdateAI(uint32 millisec );
	
	/**
	  * @brief this is a singleton
	  */
	static GorillaAI* instance();
	
private:
	GorillaAI(){ TRACE_LOG("GorillaAI create"); }
};

inline GorillaAI*
GorillaAI::instance()
{
	static GorillaAI instance;

	return &instance;
}

/****************************************************************
 * NoShadowCat monster
 ****************************************************************/
class NoShadowCatAI : public MonsterAI {
public:
	~NoShadowCatAI() {}
	
	void wait(Player* player, int millisec);
	void linger(Player* player, int millisec);
	void move(Player* player, int millisec);
	void evade(Player* player, int millisec);
	void attack(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	void defense(Player* player, int millisec);
	
	//void UpdateAI(uint32 millisec );
	
	/**
	  * @brief this is a singleton
	  */
	static NoShadowCatAI* instance();
	
private:
	NoShadowCatAI(){ TRACE_LOG("NoShadowCatAI create"); }
};

inline NoShadowCatAI*
NoShadowCatAI::instance()
{
	static NoShadowCatAI instance;

	return &instance;
}

/****************************************************************
 * Villager monster
 ****************************************************************/
class VillagerAI : public MonsterAI {
public:
	~VillagerAI() {}
	
	void wait(Player* player, int millisec);
	void linger(Player* player, int millisec);
	void move(Player* player, int millisec);
	void evade(Player* player, int millisec);
	void attack(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	void defense(Player* player, int millisec);
	
	//void UpdateAI(uint32 millisec );
	
	/**
	  * @brief this is a singleton
	  */
	static VillagerAI* instance();
	
private:
	VillagerAI(){ TRACE_LOG("VillagerAI create"); }
};

inline VillagerAI*
VillagerAI::instance()
{
	static VillagerAI instance;

	return &instance;
}

/****************************************************************
 * Raven monster
 ****************************************************************/
class RavenAI : public MonsterAI {
public:
	~RavenAI() {}
	
	void wait(Player* player, int millisec);
	void linger(Player* player, int millisec);
	void move(Player* player, int millisec);
	void evade(Player* player, int millisec);
	void attack(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	void defense(Player* player, int millisec);
	
	//void UpdateAI(uint32 millisec );
	
	/**
	  * @brief this is a singleton
	  */
	static RavenAI* instance();
	
private:
	RavenAI(){ TRACE_LOG("RavenAI create"); }
};

inline RavenAI*
RavenAI::instance()
{
	static RavenAI instance;

	return &instance;
}

/****************************************************************
 * Pig monster
 ****************************************************************/
class PigAI : public MiddleLvAI {
public:
	~PigAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static PigAI* instance();
	
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
	PigAI(){ TRACE_LOG("PigAI create"); }
	
};

inline PigAI*
PigAI::instance()
{
	static PigAI instance;

	return &instance;
}

/****************************************************************
 * gear monster
****************************************************************/
class GearAI: public MonsterAI
{
public:
	~GearAI(){}
	static GearAI* instance();
	void wait(Player* player, int millisec);
	void linger(Player* player, int millisec);
	void move(Player* player, int millisec);
	void evade(Player* player, int millisec);
	void attack(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	void defense(Player* player, int millisec);
private:
	GearAI(){ TRACE_LOG("GearAI create"); }
};

inline GearAI* GearAI::instance()
{
	static GearAI instance;
	return &instance;
}

/****************************************************************
  * bomb ai
*****************************************************************/

class BombAI: public HighLvAI
{
public:
	~BombAI(){ }
	static BombAI* instance();
	bool extra_wait_action(Player* player);
	bool extra_attack_action(Player* player);
private:
	BombAI(){ TRACE_LOG("BombAI create");}
};

inline BombAI* BombAI::instance()
{
	static BombAI instance;
	return &instance;
}

/***************************************************************
  * worm egg ai
***************************************************************/
class WormEggAI: public HighLvAI
{
public:
	~WormEggAI(){ }
	static WormEggAI* instance();
	bool extra_wait_action(Player * player);
	bool extra_stuck_action(Player* player); 
private:
	WormEggAI(){ TRACE_LOG("WormEggAI create");}
};

inline WormEggAI* WormEggAI::instance()
{
	static WormEggAI instance;
	return &instance;
}

/***************************************************************
 *  Walnut ai  
***************************************************************/

class WalnutAI: public HighLvAI
{
public:
	~WalnutAI(){ }
	static WalnutAI* instance();
	bool extra_wait_action(Player* player);
	bool extra_stuck_action(Player* player);
private:
	WalnutAI(){ TRACE_LOG("WalnutAI create");}
};


inline WalnutAI* WalnutAI::instance()
{
	static WalnutAI instance;
	return &instance;
}

class JiGuan_AI : public HighLvAI
{
	public:
		~JiGuan_AI(){}
		static JiGuan_AI * instance();
		bool extra_wait_action(Player * player);
	private:
		JiGuan_AI()
		{
			TRACE_LOG("JiGuan_AI create!");
		}
};

inline JiGuan_AI *
JiGuan_AI::instance()
{
	static JiGuan_AI instance;
	return &instance;
}

class ColorDoll_AI : public HighLvAI
{
	public:
		~ColorDoll_AI(){}
		static ColorDoll_AI * instance();
		bool extra_wait_action(Player * player);
		void move(Player * player, int timediff);
		
	private:
		ColorDoll_AI()
		{
			TRACE_LOG("ColorDoll_AI create!");
		}
};

inline ColorDoll_AI *
ColorDoll_AI::instance()
{
	static ColorDoll_AI instance;
	return &instance;
}

class GoldDoll_AI : public HighLvAI
		
{
	public:
		~GoldDoll_AI(){}
		static GoldDoll_AI * instance();
		bool extra_wait_action(Player * player);
		bool extra_stuck_action(Player * player);
		bool extra_defense_action(Player * player);
		void move(Player *player, int timediff);
	private:
		GoldDoll_AI()
		{
			TRACE_LOG("GoldDoll_AI create!");
		}
};

inline GoldDoll_AI *
GoldDoll_AI::instance()
{
	static GoldDoll_AI instance;
	return &instance;
}



class GoodGuyRabbit: public HighLvAI
{
public:
	~GoodGuyRabbit(){ }
	static GoodGuyRabbit* instance();
	bool extra_wait_action(Player * player);
	bool extra_stuck_action(Player * player);
	bool extra_defense_action(Player * player);
private:
	GoodGuyRabbit()
	{
		TRACE_LOG("GoodGuyRabbit!");
	}
};

inline GoodGuyRabbit*
GoodGuyRabbit::instance()
{
	static GoodGuyRabbit instance;
	return &instance;
}

/////////////////////////////////////////////////////////

class BadGuyRabbit: public HighLvAI
{
	public:
		~BadGuyRabbit(){ }
		static BadGuyRabbit* instance();
		bool extra_wait_action(Player * player);
		bool extra_stuck_action(Player * player);
		bool extra_defense_action(Player * player);
	private:
		BadGuyRabbit()
		{
			TRACE_LOG("BadGuyRabbit!");
		}
};


inline BadGuyRabbit*
BadGuyRabbit::instance()
{
	static BadGuyRabbit instance;
	return &instance;
}

/**
 *
 */
class ThreeWood1_AI: public HighLvAI
{
	public:
		~ThreeWood1_AI(){ }
		static ThreeWood1_AI* instance();
		bool extra_wait_action(Player * player);
	private:
		ThreeWood1_AI()
		{
			TRACE_LOG("ThreeWood1_AI!");
		}
};


inline ThreeWood1_AI*
ThreeWood1_AI::instance()
{
	static ThreeWood1_AI instance;
	return &instance;
}

class ThreeWood2_AI: public HighLvAI
{
	public:
		~ThreeWood2_AI(){ }
		static ThreeWood2_AI* instance();
		bool extra_wait_action(Player * player);
	private:
		ThreeWood2_AI()
		{
			TRACE_LOG("ThreeWood2_AI!");
		}
};


inline ThreeWood2_AI*
ThreeWood2_AI::instance()
{
	static ThreeWood2_AI instance;
	return &instance;
}

class Ghost_AI : public HighLvAI
{
	public:
		~Ghost_AI(){}
		static Ghost_AI * instance();
		bool extra_wait_action(Player * player);
		void move(Player *player, int timediff);
	private:
		Ghost_AI()
		{
			TRACE_LOG("Ghost_AI create!");
		}
};
/** 
 * @brief 道具触发AI
 */
class ThreeWoodItem_AI : public HighLvAI
{
	public:
		~ThreeWoodItem_AI(){}
		static ThreeWoodItem_AI * instance();
		bool extra_wait_action(Player * player);
	private:
		ThreeWoodItem_AI()
		{
			TRACE_LOG("ThreeWoodItem_AI");
		}
};

inline ThreeWoodItem_AI * 
ThreeWoodItem_AI::instance()
{
	static ThreeWoodItem_AI instance;
	return &instance;
}


inline Ghost_AI *
Ghost_AI::instance()
{
	static Ghost_AI instance;
	return &instance;
}

class Ghost_T_AI : public HighLvAI
{
	public:
		~Ghost_T_AI(){}
		static Ghost_T_AI * instance();
		bool extra_wait_action(Player * player);
		void move(Player * player, int timediff);
	private:
		Ghost_T_AI()
		{
			TRACE_LOG("Ghost_T_AI create!");
		}
};

inline Ghost_T_AI *
Ghost_T_AI::instance()
{
	static Ghost_T_AI instance;
	return &instance;
}

/** 
 * @brief 道具触发AI
 */
class Item_AI : public HighLvAI
{
	public:
		~Item_AI(){}
		static Item_AI * instance();
		bool extra_wait_action(Player * player);
	private:
		Item_AI()
		{
			TRACE_LOG("Item_AI");
		}
};

inline Item_AI * 
Item_AI::instance()
{
	static Item_AI instance;
	return &instance;
}

/*--------------------------------*/

class Tool_AI: public HighLvAI
{
public:
	~Tool_AI(){}
	static Tool_AI* instance();
	bool extra_wait_action(Player* player);
private:
	Tool_AI()
	{
		TRACE_LOG("Tool_AI");
	}
};


inline Tool_AI* Tool_AI::instance()
{
	static Tool_AI instance;
	return &instance;
}

/*------------------------------------*/

class Box_943_AI: public HighLvAI
{
public:
	~Box_943_AI(){}
	static Box_943_AI* instance();
	bool extra_wait_action(Player* player);
private:
	Box_943_AI()
	{
		TRACE_LOG("Box_943_AI create");
	}
};

inline Box_943_AI* Box_943_AI::instance()
{
	static Box_943_AI instance;
	return &instance;
}


class DarkCurse_AI: public HighLvAI
{
public:
	~DarkCurse_AI(){}
	bool extra_wait_action(Player* player);
	static DarkCurse_AI * instance();
private:
	DarkCurse_AI()
	{
		TRACE_LOG("Dark Curse AI create");
	}
};

inline DarkCurse_AI *
DarkCurse_AI::instance()
{
	static DarkCurse_AI instance;
	return &instance;
}

class DarkWizard_AI: public HighLvAI
{
public:
	~DarkWizard_AI(){}
	bool extra_wait_action(Player* player);
	static DarkWizard_AI * instance();
private:
	DarkWizard_AI()
	{
		TRACE_LOG("Dark Wizard AI create");
	}
};

inline DarkWizard_AI *
DarkWizard_AI::instance()
{
	static DarkWizard_AI instance;
	return &instance;
}

class Worm_AI: public HighLvAI
{
public:
	~Worm_AI(){}
	bool extra_wait_action(Player * player);
	static Worm_AI * instance();
	void move(Player * player, int diff_tm);
private:
	Worm_AI()
	{
		TRACE_LOG("Worm_AI Create!");
	}
};	

inline Worm_AI *
Worm_AI::instance()
{
	static Worm_AI instance;
	return &instance;
}

class WuSeng_AI : public HighLvAI
{
 public:
	 ~WuSeng_AI(){}
	 bool extra_wait_action(Player * player);
	 virtual void move(Player * player, int timediff);
	 static WuSeng_AI * instance();
 private:
	 WuSeng_AI()
	 {
		 TRACE_LOG("True WuSeng AI Create!");
	 }

};

inline WuSeng_AI *
WuSeng_AI::instance()
{
	static WuSeng_AI instance;
	return &instance;
}

class GuiseWuSeng_AI : public HighLvAI
{
 public:
	 ~GuiseWuSeng_AI(){}
	 bool extra_wait_action(Player * player);
	 virtual void move(Player * player, int timediff);
	 static GuiseWuSeng_AI * instance();
 private:
	 GuiseWuSeng_AI()
	 {
		 TRACE_LOG("True GuiseWuSeng AI Create!");
	 }

};

inline GuiseWuSeng_AI *
GuiseWuSeng_AI::instance()
{
	static GuiseWuSeng_AI instance;
	return &instance;
}

class PreHistory_Wuseng : public HighLvAI
{
 public:
	 ~PreHistory_Wuseng() {}
	 static PreHistory_Wuseng * instance();
	 bool extra_wait_action(Player * player);
 private:
	 PreHistory_Wuseng()
	 {
		 TRACE_LOG("PREHISTORY WuSeng create!");
	 }
};

inline PreHistory_Wuseng * PreHistory_Wuseng::instance()
{
	static PreHistory_Wuseng instance;
	return &instance;
}

class FireMon_AI : public HighLvAI
{
 public:
	 ~FireMon_AI(){}
	 static FireMon_AI * instance();
	 bool extra_wait_action(Player * player);
 private:
	FireMon_AI()
	{
		TRACE_LOG("FIRE_MONSTER AI created!");
	}
};

inline FireMon_AI * 
FireMon_AI::instance()
{
  static FireMon_AI instance;
  return &instance;
}

class ShouTouTuo_AI : public HighLvAI
{
 public:
  ~ShouTouTuo_AI() {}
  static ShouTouTuo_AI * instance();
  bool extra_wait_action(Player * player);
 private:
  ShouTouTuo_AI()
  {
	  TRACE_LOG("ShouTouTuo_AI created!");
  }
};
inline ShouTouTuo_AI *
ShouTouTuo_AI::instance()
{
	static ShouTouTuo_AI instance;
	return &instance;
}
/****************************************************************
  * Trackbomb ai
*****************************************************************/

class TrackBombAI: public HighLvAI
{
public:
	~TrackBombAI(){ }
	static TrackBombAI* instance();
	bool extra_wait_action(Player* player);
	bool extra_attack_action(Player* player);
private:
	TrackBombAI(){ TRACE_LOG("TrackBombAI create");}
};

inline TrackBombAI* TrackBombAI::instance()
{
	static TrackBombAI instance;
	return &instance;
}

/****************************************************************
  * LampDefenceAI ai
*****************************************************************/

class LampDefenceAI: public HighLvAI
{
public:
	~LampDefenceAI(){ }
	static LampDefenceAI* instance();
	bool extra_wait_action(Player* player);
	bool extra_attack_action(Player* player);
	bool extra_move_action(Player* player);
private:
	LampDefenceAI(){ TRACE_LOG("LampDefenceAI create");}
};

inline LampDefenceAI* LampDefenceAI::instance()
{
	static LampDefenceAI instance;
	return &instance;
}




#endif // KFBTL_SPECIAL_MON_AI_HPP_
