//-----------------------------------//
//-----------------------------------//

#ifndef _SINGLE_POP_GAME_H_MINGLIN_XUU_
#define _SINGLE_POP_GAME_H_MINGLIN_XUU_

//-----------------------------------//
//-----------------------------------//

#include <set>
#include <ctime>
#include <cstdlib>
#include <utility>
#include "pop.h"

//-----------------------------------//
//-----------------------------------//

#ifndef _MULTI_PLAYER_VERSION
#define _MAX_POP_MAP_ROW_NUM   15
#else
#define _MAX_POP_MAP_ROW_NUM   20
#endif//_MULTI_PLAYER_VERSION

#define _MAX_POP_MAP_COL_NUM   10

#define _TOTAL_ROW_NUM_PER_SCREEN    10
#define _INIT_START_ROW_INDEX         7

//-----------------------------------//
//-----------------------------------//

typedef enum {
    props_type_shoot = 1,
    props_type_bomb = 2,
    props_type_spider = 3,
} props_type_t;

//-----------------------------------//
//-----------------------------------//

typedef struct {
    unsigned short neighbors_array[6];
    unsigned char neighbors_num;
} pop_neighbors_t;

//-----------------------------------//
//-----------------------------------//

typedef enum {
    pop_row_status_full = 1,
    pop_row_status_not_full = 2,
    pop_row_status_empty = 3,
} pop_row_status_t;

//-----------------------------------//
//-----------------------------------//

typedef enum {
    game_progress_uninit = 0,
    game_progress_inited = 1,
    game_progress_going = 2,
    game_progress_level_over = 3, //where go to next level,game progress change to game_progress_going
    game_progress_over_success = 4,
    game_progress_over_fail = 5,
} single_pop_game_progress_t;

//-----------------------------------//
//-----------------------------------//

typedef struct {
    unsigned char col;
    unsigned char row;
    pop_color_t pop_color;
} added_pop_info_t;

//-----------------------------------//
//-----------------------------------//

typedef struct {
    int mole_coin;
    int exp;
    int strength;
} pop_game_bonus_t;

//-----------------------------------//
//-----------------------------------//

class c_single_pop_game_t
{
public:
    c_single_pop_game_t();
    virtual ~c_single_pop_game_t();

    int init_single_pop_game();
    int start_game();
    int goto_next_level();
    int end_game(bool is_win);

    int get_map_info(unsigned char* p_recv_buffer,int buffer_len,int* p_data_len);

    int add_shooted_pop(int row,int col,c_pop_t* p_shooted_pop);

    int erase_same_color_pop_group(c_pop_t& pop,unsigned short* p_recv_buffer,int buffer_len,int* p_data_len);
    int clear_isolated_pops(unsigned short* p_recv_buffer,int buffer_len,int* p_data_len);
    int erase_pops_around_bomb(c_pop_t& bomb_pop,unsigned short* p_recv_buffer,int buffer_len,int* p_data_len);
    int erase_pops_around_bomb_enlarge(c_pop_t& bomb_pop,unsigned short* p_recv_buffer,int buffer_len,int* p_data_len);

    int get_current_shoot_pop(c_pop_t* p_current_shoot_pop);
    int get_current_bakup_pop(c_pop_t* p_current_bakup_pop);
    int request_new_bakup_pop(c_pop_t* p_new_bakup_pop);
    int get_shoot_pop_array(c_pop_t* p_recv_buffer,int buffer_len);
    int request_new_shoot_pop(c_pop_t* p_new_shoot_pop);

    int use_props_shoot();
    int use_props_bomb();
    int use_props_spider();

    int get_game_progress(single_pop_game_progress_t* p_game_progress);
    int get_current_level();
    int get_props_shoot_num();
    int get_props_bomb_num();
    int get_props_spider_num();
    int get_total_score();

    int move_downward();
    int generate_colorful_pop(int ttl,c_pop_t* p_new_colorful_pop);
    int clear_colorful_pop(c_pop_t* p_cleared_colorful_pop);

    int get_bottom_row_index();
    int get_move_interval_time();

    int is_game_all_over();

    int set_user_data(long user_data);
    long get_user_data();

    int get_raw_pop_map(unsigned char* p_recv_buffer,int buffer_len,int* p_data_len);
    int set_raw_pop_map(unsigned char* p_map_data,int data_len);

    int add_pop(int add_pop_num,added_pop_info_t* p_recv_buffer,int buffer_len,int* p_data_len);

    time_t get_game_start_timestamp();

    int get_pop_game_bonus(pop_game_bonus_t* p_bonus,bool is_win);

protected:
    int reset_game_data();

    int find_neighbors(c_pop_t& center_pop,pop_neighbors_t* p_neighbors);
    int find_neighbors_on_screen(c_pop_t& center_pop,pop_neighbors_t* p_neighbors);
    int find_present_neighbors(c_pop_t& center_pop,pop_neighbors_t* p_neighbors);
    int find_present_neighbors_on_screen(c_pop_t& center_pop,pop_neighbors_t* p_neighbors);
    int find_present_neighbors_same_color(c_pop_t& center_pop,pop_neighbors_t* p_neighbors);
    int find_present_neighbors_same_color_on_screen(c_pop_t& center_pop,pop_neighbors_t* p_neighbors);
    int find_same_color_pop_recursive(c_pop_t& center_pop);
    int find_connected_pop_path_recursive(c_pop_t& center_pop);

    int verify_shoot_pop_pos(int row,int col);
    int get_pop_row_status(int row,pop_row_status_t* p_pop_row_status);
    int get_random_props();

    bool is_pop_in_the_air(c_pop_t& pop);
    bool is_pop_connected(c_pop_t& pop);

protected:
    c_pop_t m_pop_map[_MAX_POP_MAP_ROW_NUM][_MAX_POP_MAP_COL_NUM];   //the whole pop map

    single_pop_game_progress_t   m_game_progress;

    int m_max_row_index;                             //max row index of the pop map which has pops (not full) (may >= _MAX_POP_MAP_ROW_NUM) (out of map)
    int m_min_row_index;                             //min row index of pop map which has pops (not full) (may < 0) (out of map)

    int m_current_level;                             //current level of the game(5 levels total) (1 - 5)
    int m_total_score;                               //total score

    int m_bottom_row_index;                          //bottom row index showed in client screen

    c_pop_t m_current_shoot_pop;                     //current pop ready to shoot
    c_pop_t m_current_bakup_pop;                     //bakup pop behind current pop
    c_pop_t m_third_pop;
    c_pop_t m_forth_pop;
    c_pop_t m_fifth_pop;

    c_pop_t m_current_colorful_pop;

    int m_props_shoot_num;                           //the num of shoot props
    int m_props_bomb_num;                            //the num of bomb props
    int m_props_spider_num;                          //the num of spider props

    long m_user_data;                                 //user defined data
    time_t m_game_start_timestamp;

    typedef std::set<unsigned short> pop_set_t;
    typedef std::set<unsigned short>::iterator pop_set_iterator_t;
    typedef std::set<unsigned short>::const_iterator pop_set_const_iterator_t;

    pop_set_t  m_temp_pop_set;
    pop_set_t  m_temp_isolated_pop_set;
    pop_set_t  m_temp_connected_pop_set;
};

//-----------------------------------//
//-----------------------------------//

#endif//_SINGLE_POP_GAME_H_MINGLIN_XUU_

//-----------------------------------//
//-----------------------------------//
