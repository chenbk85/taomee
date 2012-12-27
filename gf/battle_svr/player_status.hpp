/**
 *  *============================================================
 *  @file      skill_imme_effect.h
 *  @brief     skill immediately related functions are declared here
 *        
 *  compiler   gcc4.1.2
 *  platform   Linux
 *      
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *  
 *============================================================
**/

#ifndef GF_PLAYER_STATUS_H
#define GF_PLAYER_STATUS_H

/**
 *   @brief init player status
 *   @param Player*
 *   @return  true sucess,  false otherwirse fail
**/
bool init_player_status(Player* p);

/**
 *   @brief final player status
 *   @param Player*
 *   @return  true sucess,  false otherwirse fail
**/
bool final_player_status(Player* p);

////////////////////buff interface//////////////////////////////
/**
 *    @brief add buff to player with para duration time 
 *    @param Player*  if duration time = 0 the buff duration time will set by xml file, else will set by parameters 
 *    @return  true sucess,  false otherwirse fail
**/
bool add_player_buff(Player* p, uint32_t buff_id, uint32_t creater_id, uint32_t duration_time = 0, bool notify = true, bool include_invincible = false);

bool check_player_immunity_this_effect(Player* p, uint32_t buff_id);

/**
 *    @brief add buff to player
 *    @param Player*  
 *    @return  true sucess,  false otherwirse fail
**/
bool add_player_buff(Player* p, buff* pBuff, bool notify = true);

/**
 *   @brief delete buff from player
 *   @param Player*, uint32_t  
 *   @return  true sucess,  false otherwirse fail
**/
bool del_player_buff(Player* p, uint32_t buff_id, bool notify = true);

/**
 *   @brief delete all buff from player
 *   @param Player*
 *   @return  true sucess,  false otherwirse fail
**/
bool del_player_all_buff(Player* p);

bool del_player_all_buff(Player* p, uint32_t buff_type);


bool del_random_player_buff(Player* p, uint32_t buff_type, uint32_t max_count = 1);


/**
 *  @brief delete buff when player dead
 *  @param Player*
 *  @return true sucess,  false otherwirse fail
**/

bool del_buff_when_condition(Player* p, uint32_t reason);
/**
 *   @brief reset buff 
 *   @param Player*, uint32_t
 *   @return  true sucess,  false otherwirse fail
**/
bool reset_player_buff(Player* p, uint32_t buff_id);

/**
 *    @brief check the buff is exist to player
 *    @param Player*, uint32_t
 *    @return  true sucess,  false otherwirse fail
**/
bool is_player_buff_exist(Player* p, uint32_t buff_id);

/**
 *    @brief get buff from player by id
 *    @param Player*, uint32_t
 *    @return  buff* sucess,  NULL otherwirse fail
**/
buff* get_player_buff(Player* p, uint32_t buff_id);


/**
 *   @brief player buff list routing
 *   @param Player*, struct timeval
 *   @return  void
**/
void player_bufflist_routing(Player* p, struct timeval cur_time);



void player_passive_buff_trigger(Player* p, struct timeval cur_time,  uint32_t trigger_type);
////////////////////aura interface//////////////////////////////


void player_auralist_routing(Player* p, struct timeval cur_time);


void player_aura_infect(Player* p, aura* pAura, std::set<Player*>& m);
/**
 *    @brief add aura to player
 *    @param Player*, uint32_t
 *    @return  true sucess,  false otherwirse fail
**/
bool add_player_aura(Player* p,  uint32_t aura_id, uint32_t create_id, bool notify = true);

/**
 *    @brief add aura to player
 *    @param Player*, aura*
 *    @return  true sucess,  false otherwirse fail
**/
bool add_player_aura(Player* p,  aura* pAura, bool notify = true);

/**
 *    @brief delete aura from player
 *    @param Player*, uint32
 *    @return  true sucess,  false otherwirse fail
**/
bool del_player_aura(Player* p,  uint32_t aura_id, bool notify = true);

/**
 *    @brief delete all aura from player
 *    @param Player*
 *    @return  true sucess,  false otherwirse fail
**/
bool del_player_all_aura(Player* p, bool notify = true);

/**
 *   @brief delete aura by create id
 *   @param Player*  uint32_t bool
 *   @return  true sucess,  false otherwirse fail
 **/

bool de_player_aura_by_creater_id(Player* p, uint32_t create_id, bool notify = true);
/**
 *    @brief check the aura is exist 
 *    @param Player*, uint32
 *    @return  true sucess,  false otherwirse fail
**/
bool is_player_aura_exist(Player* p,  uint32_t aura_id);


/**
 *    @brief player aura list routing 
 *    @param Player*, struct timeval
 *    @return  true sucess,  false otherwirse fail
**/
void player_auralist_routing(Player* p, struct timeval cur_time);


/**
 *    @brief get player aura 
 *    @param Player*, uint32
 *    @return  aura* sucess,  NULL otherwirse fail
**/
aura* get_player_aura(Player* p, uint32_t aura_id);


////////////////////player additional  attribute interface///////////////////

/**
 *   @brief calc player attribute from buff and aura
 *   @param Player*
 *   @return  void
**/
void calc_player_additional_attribute(Player* p);

/**
 *   @brief calc player basic attribute from buff and aura
 *   @param Player*
 *   @return  void
**/
bool calc_player_additional_basic_attribute(Player* p);

/**
 *     @brief get player add atk value from buff and aura
 *     @param Player*
 *     @return  int
**/
int get_player_additional_atk(Player* p);


/**
 *    @brief get player add def rate from buff and aura
 *    @param Player*
 *    @return float
**/
float get_player_additional_def_rate(Player* p);

/**
 *    @brief get player critical rate from buff and aura
 *    @param Player*
 *    @return  int
**/
int get_player_additional_critical_rate(Player* p);


/**
 *   @brief get player critical rate from buff and aura
 *   @param Player*
 *   @return  int
**/
float get_player_additional_exp_factor(Player* p);


/**
 *   @brief get player max_hp from buff and aura
 *   @param Player* 
 *   @return int
 **/
int get_player_additional_max_hp(Player* p);


/**
 *   @brief get player max_mp from buff and aura
 *   @param Player* 
 *   @return int
 **/
int get_player_additional_max_mp(Player* p);



/**
 *  @brief notify player add a buff
 *  @param Player* ,  buff*
 *  @return void
 **/
void notify_add_buff_to_player(Player* p, buff* pBuff);

/**
 *  @brief notify player delete a buff
 *  @param Player* ,  buff*
 *  @return void 
 * */
void notify_del_buff_to_player(Player* p, buff* pBuff);

/**
 *  @brief notify player delete a buff
 *  @param Player*, uint32_t 
 *  @return void
 * */
void notify_del_buff_to_player(Player* p, uint32_t buff_id);

/**
 *  @brief check is the effect type exist in player
 *  @param Player*, uint32_t effect_type
 *  @return void
 **/
bool is_effect_exist_in_player(Player* p, uint32_t effect_type);


/**
 *   @brief del aura when player dead
 *   @param Player*
 *   @return bool
 **/
bool del_infect_aura_when_dead(Player* p);
/**
 *   @brief notify player add an aura
 *   @param Player* ,  aura*
 *   @return void
**/
void notify_add_aura_to_player(Player* p,  aura* pAura);


/**
 *   @brief notify player delete an aura
 *   @param Player* ,  aura*
 *   @return void
 **/

void notify_del_aura_to_player(Player* p,  aura* pAura);

/**
 *   @brief notify player delete an aura
 *   @param Player* ,  aura*
 *   @return void
 **/

void notify_del_aura_to_player(Player* p,  uint32_t aura_id);

/**
 *  @brief get effect_data from player 
 *  @param Player*, uint32_t effect_type
 *  @return effect_data*
 **/
effect_data* get_effect_data_in_player(Player* p, uint32_t effect_type);
#endif



