//-----------------------------------//
//-----------------------------------//

#ifndef _POP_GAME_IMPL_H_MINGLIN_XUU_
#define _POP_GAME_IMPL_H_MINGLIN_XUU_

//-----------------------------------//
//-----------------------------------//

#include <stdint.h>

#include "../mpog.hpp"

extern "C"
{
#include "../../proto.h"
#include "../../sprite.h"
#include "../../timer.h"
#include "../../dbproxy.h"

}

#include "single_pop_game.h"

//-----------------------------------//
//-----------------------------------//

class c_pop_game_impl_t : public mpog
{
public:
    c_pop_game_impl_t();
    virtual ~c_pop_game_impl_t();

	virtual void init(sprite_t* p_sprite);
	virtual int  handle_data(sprite_t* p_sprite,int cmd,const uint8_t body[],int data_len);
	virtual int  handle_timeout(void* data) { return 0; };

    int init_pop_game();

private:
    int handle_single_pop_game_data(sprite_t* p_sprite,int cmd,const uint8_t data[], int data_len);

    //-----------------------------------------------------

    int setup_single_game_movedown_timer(sprite* p_sprite);
    int setup_single_game_colorful_pop_timer(sprite* p_sprite);
    static int deal_single_game_movedown_timer(void* p_data1,void* p_data2);//return 0,timer will be removed
    static int deal_single_game_generate_colorful_pop_timer(void* p_data1,void* p_data2);
    static int deal_single_game_clear_colorful_pop_timer(void* p_data1,void* p_data2);
    int del_single_game_all_timer(sprite* p_sprite);

    int response_single_game_over_fail(sprite_t* p_sprite);
    int response_single_game_over_success(sprite_t* p_sprite);
    int response_single_game_level_info(sprite_t* p_sprite);
    int response_single_game_current_pops_update(sprite_t* p_sprite);
    int response_single_game_props_num_shoot(sprite* p_sprite);
    int response_single_game_props_num_bomb(sprite* p_sprite);
    int response_single_game_score(sprite* p_sprite);
    int response_single_game_bonus(sprite* p_sprite,bool is_get_props);

    int response_single_game_shoot_pop_array(sprite_t* p_sprite);
    int response_single_game_tail_shoot_pop_update(sprite_t* p_sprite,c_pop_t& new_shoot_pop);

    int response_single_game_pops_erased(sprite* p_sprite,int seq_num,unsigned short* pop_array,int pop_num);
    int response_single_game_pops_cleared(sprite* p_sprite,int seq_num,unsigned short* pop_array,int pop_num);

    int response_single_game_bottom_row_index(sprite* p_sprite);
    int response_single_game_request_use_props_ack(sprite* p_sprite,int props_type,int result,int seq_num);

    int response_single_game_level_over(sprite* p_sprite);

    int response_single_game_generate_colorful_pop(sprite* p_sprite,int col,int row);
    int response_single_game_clear_colorful_pop(sprite* p_sprite,int col,int row);

    int response_single_game_error(sprite* p_sprite,int cmd,int errno);

    //------------------------------------------------------

private:
    c_single_pop_game_t* m_p_single_pop_game;
};

//-----------------------------------//
//-----------------------------------//

class c_multi_pop_game_impl_t : public mpog
{
public:
    c_multi_pop_game_impl_t();
    virtual ~c_multi_pop_game_impl_t();

    virtual void init(sprite_t* p_sprite);
	virtual int  handle_data(sprite_t* p_sprite,int cmd,const uint8_t body[],int data_len);
	virtual int  handle_timeout(void* data) { return 0; };

	int init_multi_pop_game();

private:
	int handle_multi_pop_game_data(c_single_pop_game_t* p_pop_game,sprite* p_sprite,int cmd,const uint8_t data[],int data_len);

	//-----------------------------------------------------

	int setup_multi_game_movedown_timer(game_group* p_game_group);
    int setup_multi_game_colorful_pop_timer(game_group* p_game_group);
    static int deal_multi_game_movedown_timer(void* p_data1,void* p_data2);//return 0,timer will be removed
    static int deal_multi_game_generate_colorful_pop_timer(void* p_data1,void* p_data2);
    static int deal_multi_game_clear_colorful_pop_timer(void* p_data1,void* p_data2);
    int del_multi_game_all_timer(game_group* p_game_group);

	int response_multi_game_over_fail(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite);
    int response_multi_game_over_success(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite);
    int response_multi_game_level_info(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite);
    int response_multi_game_current_pops_update(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite);
    int response_multi_game_props_num_shoot(c_single_pop_game_t* p_pop_game,sprite* p_sprite);
    int response_multi_game_props_num_bomb(c_single_pop_game_t* p_pop_game,sprite* p_sprite);
    int response_multi_game_props_num_spider(c_single_pop_game_t* p_pop_game,sprite* p_sprite);
    int response_multi_game_score(c_single_pop_game_t* p_pop_game,sprite* p_sprite);
    int response_multi_game_bonus(c_single_pop_game_t* p_pop_game,sprite* p_sprite,bool is_win);

    int response_multi_game_shoot_pop_array(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite);
    int response_multi_game_tail_shoot_pop_update(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite,c_pop_t& new_shoot_pop);

    int response_multi_game_pops_erased(c_single_pop_game_t* p_pop_game,sprite* p_sprite,int seq_num,unsigned short* pop_array,int pop_num);
    int response_multi_game_pops_cleared(c_single_pop_game_t* p_pop_game,sprite* p_sprite,int seq_num,unsigned short* pop_array,int pop_num);

    int response_multi_game_bottom_row_index(c_single_pop_game_t* p_pop_game,sprite* p_sprite);
    int response_multi_game_request_use_props_ack(c_single_pop_game_t* p_pop_game,sprite* p_sprite,int props_type,int result,int seq_num);

    int response_multi_game_generate_colorful_pop(c_single_pop_game_t* p_pop_game,sprite* p_sprite,int col,int row);
    int response_multi_game_clear_colorful_pop(c_single_pop_game_t* p_pop_game,sprite* p_sprite,int col,int row);

    int response_multi_game_other_over_fail(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite);
    int response_multi_game_other_over_success(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite);
    int response_multi_game_other_screen_status(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite,unsigned char* p_data,int data_len);
    int response_multi_game_other_use_spider(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite);
    int response_multi_game_other_add_pop_notify(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite,int add_pop_num);

    int response_multi_game_pop_added(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite,added_pop_info_t* p_data,int data_len);

    int response_multi_game_error(c_single_pop_game_t* p_pop_game,sprite* p_sprite,int cmd,int errno);

	//-----------------------------------------------------

private:
    c_single_pop_game_t* m_p_single_pop_game_first;
    c_single_pop_game_t* m_p_single_pop_game_second;
};

//-----------------------------------//
//-----------------------------------//

extern "C" void* create_game(struct game_group* p_game_group);
extern "C" int   game_init();
extern "C" void  game_destroy();

//-----------------------------------//
//-----------------------------------//

#endif//_POP_GAME_IMPL_H_MINGLIN_XUU_

//-----------------------------------//
//-----------------------------------//
