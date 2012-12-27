/*
 * =====================================================================================
 *
 *       Filename:  hom_impl.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/04/2011 04:53:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#ifndef HOME_IMPL_HPP
#define HOME_IMPL_HPP
#include <map>
#include "player.hpp"


class Player;
class Plant;
class Home;
class HomeDecorate;

struct plant_elem_t;
struct plant_t;
struct Ferilizer_t;
typedef struct home_attr_data home_attr_data;

class HomeImpl {
 public:
 // HomeImpl(){}
  virtual ~HomeImpl(){}

  virtual void PlayerEnter(Player * p) = 0;

  virtual void PlayerLeave(Player * p) = 0;

  virtual PlayerMap & HomePlayers() = 0;

//  virtual PlayerMap & HomeMonsters() = 0;

  virtual void Update() = 0;
};

typedef std::map<uint32_t, Player*> PlayerMap;


class SummonHome :public HomeImpl  {

 public:

   SummonHome(Home* p_home); 

   ~SummonHome(); 

   PlayerMap & HomePlayers() {
	   return Players_;
   }


   void PlayerEnter(Player * p);
   void PlayerLeave(Player * p);

   PlayerMap Players_;
   //小屋属性信息
   home_attr_data home_data;
   uint32_t get_home_exp()
   {
	   return home_data.home_exp;
   }
   void set_home_exp(uint32_t exp)
   {
	   home_data.home_exp = exp;
   }

   void add_home_exp(uint32_t exp)
   {
	   home_data.home_exp += exp;		   
   }

   uint32_t get_home_level()
   {
	   return home_data.home_level;
   }
   void home_level_up()
   {
	   home_data.home_level ++;
   }
   void set_home_level(uint32_t level)
   {
	   home_data.home_level = level;
   }

   void add_home_level(uint32_t level = 1)
   {
	   home_data.home_level += level;
   }
   //主人的宠物信息
   uint32_t summon_cnt;
   summon_t homesummon[max_summon_num];

   Home* p_base_home;

   void syn_pet_info(Player *p);
//plants manager
public:

//	Plant* add_plant(uint32_t plant_id_in, uint32_t plant_tm_in, uint32_t status_in, uint32_t status_tm_in, bool notify = true);
    
	Plant * add_plant(plant_elem_t * plant_in, bool notify = true);

	bool is_plants_db_updated() {
		return updated_flg;
	}
	void clear_plant();
	void pack_all_plants_info(uint8_t * pkg, int & idx);
	void set_plants_db_updated() {
		updated_flg = true;
	}
	Plant* get_plant(uint32_t field_id);
	void set_plant_status(Player* p, uint32_t field_id, uint32_t status);
	bool pick_fruit(Player* p, uint32_t field, uint32_t fruit_id);

	bool sow_seed(uint32_t field, plant_t * p_in);

//	void init_plants();
//	void add_init_plants();

	void ferilizer_home(Ferilizer_t* p_ferilizer);
	void db_add_effect_to_all_plants(Ferilizer_t* p_ferilizer);
	void Update();
	void RandSowSeed(uint32_t odd_times);
public:
    HomeDecorate* p_decorate;
private:
	std::vector<Plant*> Plants_;
	bool updated_flg;
	uint32_t tm_flag;
};

#endif

