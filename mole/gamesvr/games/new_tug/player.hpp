/**
 * =====================================================================================
 *       @file  player.hpp
 *      @brief  玩家信息类头文件
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/27/2010 11:08:16 AM
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


/**
 * 用于保存数据库发送的用户卡牌信息
 */
typedef struct u_card_info
{
	uint32_t total_cnt;
	uint32_t card_id[100];
} __attribute__((packed)) player_cards_info_t;

class Cplayer
{
public:
    Cplayer();
    uint32_t id();
    sprite_t* get_sprite() { return p;}
    void set_sprite(sprite_t* p_sprite) {  p = p_sprite;}
	player_cards_info_t* get_cards_info() { return p_cards_info;}
	void set_cards_info(player_cards_info_t* p_cards) { p_cards_info = p_cards;}
    int get_card_id(int i) { return cards_id[i] ;}
    int get_card_size( ) { return cards_id.size() ;}
	int create_cards_seq();
    void set_life_value(int value) { life_value = value;}
	int act_bout_card(int pos);
    int get_bout_cardid() { return bout_cardid;}
    int get_life_value() { return life_value;}
    int update_life_dec(int value) ;
	void set_result(int value);
    int get_result() { return result;}
	void bout_clear();
    void set_xiaomee(int value) { xiaomee = value;}
    int get_xiaomee( ) { return  xiaomee;}
    void set_medal(int value) { medal = value;}
    int get_medal( ) { return  medal;}
    void set_exp(int value) { exp = value;}
    int get_exp( ) { return  exp;}
    void set_game_status(int value) { game_status = value;}
    int get_game_status( ) { return  game_status;}

private:

    sprite_t *p;
	std::vector<int> cards_id;
	player_cards_info_t *p_cards_info;
	int bout_cardid;
	int life_value;
	int result;
	int exp;
	int medal;
	int xiaomee;
	int game_status;

};
#endif

