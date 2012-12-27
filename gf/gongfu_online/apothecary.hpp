/**
 *  *============================================================
 *  @file      apohecary.h
 *  @brief     apohecary related functions are declared here
 *    
 *  compiler   gcc4.1.2
 *  platform   Linux
 *  
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *   
 *   ============================================================
**/


#ifndef _APOHECARY_H_
#define _APOHECARY_H_

#include <stdint.h>
#include <vector>
#include <map>
#include "player.hpp"

//the money you should pay for learning one secondary profession
#define  LEARN_SECONDARY_PRO_COSTS 		(998)
//learn secondary professtion level limit 
#define  LEARN_SECONDARY_PRO_LEVEL_LIMIT (10)

using namespace std;

/*----------------------------fomulation config data structure and class -----------------------------------------------------*/
typedef struct material_data
{
	material_data()
	{
		item_id = 0;
		item_count = 0;
	}
	//  item id 
	uint32_t item_id;
	//  item count
	uint32_t item_count;
}material_data;

typedef struct random_item_data
{
	random_item_data()
	{
		item_id = 0;
		item_count = 0;
		odds = 0;
	}
	//  item id 
	uint32_t item_id;
	//  item count
	uint32_t item_count;
	uint32_t odds;
}random_item_data;


typedef struct formulation_data
{
	formulation_data()
	{
		id = 0;
		level_limit = 0;
		add_exp  = 0;
		create_item_id = 0;
		create_item_count = 0;
		pro_type = 0;
		need_vitality_point = 0;
	}
	//formulation_id
	uint32_t  id;
	//level limit
	uint32_t  level_limit;
	//add exp
	uint32_t  add_exp;
	//create item id
	uint32_t  create_item_id;
	//create item count
	uint32_t  create_item_count;
	//profession type
	uint32_t  pro_type;
	//need vitality_point;
	uint32_t  need_vitality_point;
	//material arrays
	std::vector<material_data>  materials;
	//random_items arrays
	std::vector<random_item_data>  random_items;
}formulation_data;


class formulation_data_mgr
{
public:
	formulation_data_mgr(){ }
	~formulation_data_mgr(){ }
public:
	bool  init(const char* xml);
	bool  final();
public:
	//add one formulation data to manager
	bool  	  add_formulation_data( formulation_data& data );
	//get formulation data counts from manager
	uint32_t  get_formulation_data_count();
	//check formulation data exist through id
	bool      is_formulation_exist(uint32_t id);
	//get formulation data point by id 
	formulation_data*  get_formulation_data_by_id(uint32_t id);
private:
	std::map<uint32_t, formulation_data>  m_data_maps;
};


/*-----------------------------------------------------------------------------------------------*/
struct learn_secondary_pro_rsp_t
{
	uint32_t type;
	uint32_t left_coin;
};


struct db_material_item_t
{
	uint32_t item_id;
	uint32_t item_count;
};

struct db_random_add_item_t
{
	uint32_t item_id;
	uint32_t item_count;
};

struct secondary_pro_fuse_rsp_t
{
	uint32_t pro_type;
	uint32_t add_exp;
	uint32_t id;
	//uint32_t create_item_id;
	//uint32_t create_item_count;
	uint32_t material_count;
	uint32_t random_count;
	db_material_item_t  items[];
};

struct secondary_pro_info_t
{
	uint32_t  pro;
	uint32_t  pro_exp;
};

struct get_secondary_pro_list_rsp_t
{
	uint32_t pro_count;
	secondary_pro_info_t pros[];
};

/*---------------------------profession interface--------------------------------------------*/

/**
 *    @brief initialization player's secondary professtion
 *    @param player_t* 
 *    @return  true sucess,  false  otherwirse fail
**/
bool init_player_secondary_profession(player_t* p);

/**
 *    @brief unitialization player's secondary professtion
 *    @param player_t*  
 *    @return  true sucess,  false  otherwirse fail
**/
bool final_player_secondary_profession(player_t* p);

/**
 *    @brief get player's secondary professtion by type
 *    @param player_t*  utin32_t 
 *    @return  point sucess,  NULL otherwirse fail
**/
secondary_profession_data*  get_player_secondary_profession_by_type(player_t* p , uint32_t type);

/**
 *    @brief check player's secondary professtion exist
 *    @param player_t*  uint32_t 
 *    @return  true exist,  not exist false
**/
bool is_player_secondary_profession_exist(player_t*p, uint32_t type);


/**
 *  @brief add player's secondary professtion 
 *  @param player_t*  uint32_t 
 *  @return  true sucess, false otherwise fail
**/
bool add_player_secondary_profession(player_t* p, uint32_t type);

/**
 *  @brief del player's secondary professtion 
 *  @param player_t*  uint32_t 
 *   @return  true sucess, false otherwise fail
**/
bool del_player_secondary_professoin(player_t* p, uint32_t type);


/**
 *  @brief set player's secondary professtion exp and calc the level from exp
 *  @param player_t* , uint32_t, uint32_t
 *  @return  true sucess, false otherwise fail
**/
bool set_player_secondary_professoin_exp(player_t*p, uint32_t type, uint32_t exp);


/**
 *  @brief add player's secondary professtion exp and calc the level from exp
 *  @param player_t* , uint32_t, uint32_t
 *  @return  true sucess, false otherwise fail
**/
bool add_player_secondary_professoin_exp(player_t*p, uint32_t type, uint32_t exp, uint32_t* fact_add_exp);


/**
 *  @brief learn player's secondary professtion 
 *  @param player_t* , uint8_t, uint32_t
 *  @return  true sucess, false otherwise fail
**/
int learn_secondary_pro_cmd(player_t* p, uint8_t* body, uint32_t bodylen);


/**
 *  @brief send learn secondary professtion db request to dbserver
 *  @param player_t* , uint32_t, uint32_t
 *  @return  0 sucess, unzero otherwise 
**/
int learn_secondary_pro_db_request(player_t* p,  uint32_t type, uint32_t money);


/**
	*  @brief  learn secondary professtion db callback function
    *  @param player_t* 
    *  @return 0 sucess, unzero otherwise 
**/
int db_learn_secondary_pro_callback( player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);


/**
 *  @brief  fuse one formulation
 *  @param player_t* 
 *  @return 0 sucess, unzero otherwise 
**/
int secondary_pro_fuse_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 *  @brief  send fuse one formulation db request to db server
 *  @param player_t* 
 *  @return 0 sucess, unzero otherwise 
**/
int secondary_pro_fuse_db_request(player_t* p, formulation_data* pData, uint32_t count, uint32_t max_exp_limit);


/**
 *  @brief  check fuse one formulation condition
 *  @param player_t* 
 *  @return true sucess, false otherwise fail
**/
bool check_secondary_pro_fuse_condition(player_t*p, formulation_data* pData, uint32_t count, uint32_t* err_code);

/**
 *   @brief  fuse one formulation condtion callback
 *   @param player_t* 
 *   @return 0 sucess, unzero otherwise  
**/
int db_secondary_pro_fuse_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 *  @brief send get profession list to dbserver
 *  @param player_t* 
 *  @return 0 sucess, unzero otherwise  
**/
int db_get_secondary_pro_list(player_t* p);

/**
 *  @brief db get profession list callback
 *  @param player_t* 
 *  @return 0 sucess, unzero otherwise  
**/
int db_get_secondary_pro_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);



/**
 *  @brief init player profession list callback
 *  @param player_t* 
 *  @return 0 sucess, unzero otherwise  
**/
int init_player_secondary_pro_list(player_t*p, get_secondary_pro_list_rsp_t* rsp);

int get_player_max_vitality_point(player_t* p);

int player_auto_recover_vitality_point(void* owner, void* data);

int notify_player_vitality_point_change(player_t* p);

int db_set_player_vitality_point(player_t* p, uint32_t point);

int add_player_vitality_point(player_t* p, uint32_t point);

int dec_player_vitality_point(player_t* p, uint32_t point);

int notify_player_second_pro_exp_lv_change(player_t* p, uint32_t pro_type);

int db_set_player_second_pro_exp(player_t* p, uint32_t pro_type, uint32_t pro_exp);

#endif
