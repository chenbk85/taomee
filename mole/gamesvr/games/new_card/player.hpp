/**
 * =====================================================================================
 *       @file  player.hpp
 *      @brief  玩家信息类头文件
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  05/31/2010 11:08:16 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
 *
 *     @author  tommychen
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef PLAYER_HPP
#define PLAYER_HPP
extern "C" {
#include  <libtaomee/log.h>
#include  <gameserv/game.h>
}

#include <vector>


/** 游戏等级对应的经验值 */
enum rank_exp
{
	new_rank1_exp = 500,
	new_rank2_exp = 1500,
	new_rank3_exp = 3000,
	new_rank4_exp = 5000,
};

/**
 * 用于保存数据库发送的用户卡牌信息
 */
typedef struct u_card_info
{
	uint32_t sl_flag;
	uint32_t exp;
	uint32_t flag;
	uint32_t day_exp;
	uint32_t day_monster;
	uint32_t mole_exp;
	uint32_t total_cnt;
	uint32_t card_id[100];
} __attribute__((packed)) player_cards_info_t;

class Cplayer
{
public:
    Cplayer();
    uint32_t id();
    int get_roll_point();
    sprite_t* get_sprite() { return p;}
    void set_sprite(sprite_t* p_sprite) {  p = p_sprite;}
	player_cards_info_t* get_cards_info() { return p_cards_info;}
	void set_cards_info(player_cards_info_t* p_cards) { p_cards_info = p_cards;}
    int get_grid_id() ;
    void set_grid_id(int id);
    int get_num() { return num;}
    void set_num(int value) { num = value;}
    int roll();
	int calculate_rank(player_cards_info_t *p_info);
    int get_rank() { return rank;}
    void set_rank(int value) { rank = value;}
    int get_card_id(int i) { return cards_id[i] ;}
    int get_card_size( ) { return cards_id.size() ;}
	int create_cards_seq();
    int get_status() { return status;}
    void set_status(int in_status ) { status = in_status;}
    void set_life_value(int value) { life_value = value;}
	int act_bout_card(int pos);
    int get_bout_cardid() { return bout_cardid;}
    int get_life_value() { return life_value;}
    int update_life_dec(int value) ;
    int update_life_inc(int value) ;
	void set_bout_result(int result);
    int get_bout_result() { return bout_result;}
    void set_grade(int value) ;
    int get_grade( ) ;
	void bout_clear();
    void set_itemid(int value) { itemid = value;}
    int get_itemid( ) { return  itemid;}
    void set_bout_exp(int value) { bout_exp = value;}
    int get_bout_exp( ) { return  bout_exp;}
    int get_new_rank( ) ;
    bool is_up_rank( ) ;
    void set_cardid(int value) { cardid = value;}
    int get_cardid( ) { return  cardid;}
    void set_bout_xiaomee(int value) { bout_xiaomee = value;}
    int get_bout_xiaomee( ) { return  bout_xiaomee;}
    int get_mole_level( ) ;
	int cal_mole_level();
    int get_day_monster( );
    int get_sl_flag( );
    void set_high_card(int value) { high_card = value;}
    int get_high_card( ) { return  high_card;}
    int get_game_status() { return game_status;}
    void set_game_status(int in_status ) { game_status = in_status;}
    int get_v_itemid(int i) { return itemids[i] ;}
    int get_v_itemid_size( ) { return itemids.size() ;}
    int add_v_itemid(int itemid);
    void clear_v_itemid( ) {  itemids.clear() ;}
	int get_card_exp( );
	bool is_in_cards(int cardid);
	int get_flag() ;

public:
	player_cards_info_t *p_cards_info;
private:
    sprite_t *p;
    int roll_point;
    int grid_id;
    int num;
	std::vector<int> cards_id;
	int rank;
	int status;
	int bout_cardid;
	int life_value;
	int grade;
	int bout_result;
	int itemid; //个人装扮，只能有一件
	int bout_exp;
	int cardid;
	int bout_xiaomee;
	int mole_level;
	int high_card;
	int game_status;
	std::vector<int> itemids;

};
#endif

