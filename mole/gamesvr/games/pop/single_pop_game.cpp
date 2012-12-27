///----------------------------------//
//-----------------------------------//

#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "single_pop_game.h"
#include "my_errno.h"

///----------------------------------//
//-----------------------------------//

#define null 0

#define _MAX_SINGLE_GAME_LEVEL_NUM          5

#define _MAX_SINGLE_GAME_MOLE_COIN_NUM      160
#define _MAX_SINGLE_GAME_EXP_NUM            32
#define _MAX_SINGLE_GAME_STRENGTH_NUM       16

#define _MULTI_GAME_WIN_MOLE_COIN_NUM       50
#define _MULTI_GAME_LOSE_MOLE_COIN_NUM      20
#define _MULTI_GAME_WIN_EXP_NUM             10
#define _MULTI_GAME_LOSE_EXP_NUM             4
#define _MULTI_GAME_WIN_STRENGTH_NUM         5
#define _MULTI_GAME_LOSE_STRENGTH_NUM        2

#define _MIN_MULTI_GAME_TIME                45

///----------------------------------//
//-----------------------------------//

template<typename type_name_t> type_name_t min(type_name_t left,type_name_t right)
{
    if(left < right)
    {
        return left;
    }
    else
    {
        return right;
    }
}

template<typename type_name_t> type_name_t max(type_name_t left,type_name_t right)
{
    if(left > right)
    {
        return left;
    }
    else
    {
        return right;
    }
}

///----------------------------------//
//-----------------------------------//

c_single_pop_game_t::c_single_pop_game_t()
{
    m_game_progress = game_progress_uninit;
}

///----------------------------------//
//-----------------------------------//

c_single_pop_game_t::~c_single_pop_game_t()
{
    m_game_progress = game_progress_uninit;
}

///----------------------------------//
//-----------------------------------//
//note: if game has been inited,return E_LOGIC,else return S_OK

int c_single_pop_game_t::init_single_pop_game()
{
    //---------------------
    //check current game progress
    if(m_game_progress != game_progress_uninit)
    {
        //---------------------
        //game already inited
        return E_LOGIC;
    }

    //---------------------
    //init game data
    m_current_level = 1;
    m_total_score = 0;

    m_props_shoot_num = 1;
    m_props_bomb_num = 1;
    m_props_spider_num = 0;

    m_temp_pop_set.clear();
    m_temp_isolated_pop_set.clear();
    m_temp_connected_pop_set.clear();

    reset_game_data();

    m_game_start_timestamp = time(null);
    m_game_progress = game_progress_inited;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::start_game()
{
    //---------------------
    //check current game progress
    if(m_game_progress != game_progress_inited)
    {
        return E_LOGIC;
    }

    m_game_start_timestamp = time(null);
    m_game_progress = game_progress_going;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//
//note:  if game is all over,return S_FALSE,if go to next level,return S_OK,else return error code

int c_single_pop_game_t::goto_next_level()
{
    //---------------------
    //check current game progress
    if(m_game_progress == game_progress_over_success)
    {
        return S_FALSE;
    }

    if(m_game_progress != game_progress_level_over)
    {
        return E_LOGIC;
    }

    //---------------------
    //determin current level
    if(m_current_level >= _MAX_SINGLE_GAME_LEVEL_NUM)
    {
        //---------------------
        //game is all over,and success
        m_game_progress = game_progress_over_success;

        return S_FALSE;
    }

    //---------------------
    //reset all game data
    m_temp_pop_set.clear();
    m_temp_isolated_pop_set.clear();
    m_temp_connected_pop_set.clear();

    reset_game_data();

    //---------------------
    //increase current level
    m_current_level++;
    m_total_score += 500;

    //---------------------
    //set current game progress
    m_game_progress = game_progress_going;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::end_game(bool is_win)
{
    if(is_win)
    {
        m_game_progress = game_progress_over_success;
    }
    else
    {
        m_game_progress = game_progress_over_fail;
    }

    return S_OK;
}

///----------------------------------//
//-----------------------------------//
//map storage struct:
// 1st byte: x,col
// 2st byte: y,row
// 3st byte: -1:null 0:empty 1:red 2:blue 3:yellow 4:orange 5:green
// map data start from _INIT_START_ROW_INDEX  row
// should init the game before call this function

int c_single_pop_game_t::get_map_info(unsigned char* p_recv_buffer,int buffer_len,int* p_data_len)
{
    //---------------------
    //check game progress
    if(m_game_progress == game_progress_uninit)
    {
        return E_LOGIC;
    }

    //---------------------
    //param check
    if(null == p_recv_buffer || buffer_len < 1 || null == p_data_len)
    {
        return E_INVALIDARG;
    }

    if(buffer_len <= (_MAX_POP_MAP_ROW_NUM - _INIT_START_ROW_INDEX + 1) * _MAX_POP_MAP_COL_NUM * 3)
    {
        return E_BUFFERTOOSHORT;
    }

    //---------------------
    //fill map data
    int data_len = 0;

    for(int row = _INIT_START_ROW_INDEX; row < _MAX_POP_MAP_ROW_NUM; row++)
    {
        for(int col = 0; col < _MAX_POP_MAP_COL_NUM; col++)
        {
            if(data_len >= buffer_len)
            {
                break;
            }

            *(p_recv_buffer + data_len) = col;
            data_len++;

            *(p_recv_buffer + data_len) = row;
            data_len++;

            pop_status_t pop_status;
            m_pop_map[row][col].get_pop_status(&pop_status);

            if(pop_status == pop_status_present)
            {
                pop_color_t pop_color;
                m_pop_map[row][col].get_pop_color(&pop_color);

                *(p_recv_buffer + data_len) = (unsigned char)pop_color;
                data_len++;
            }
            else if(pop_status == pop_status_erased)
            {
                *(p_recv_buffer + data_len) = 0;
                data_len++;
            }
            else if(pop_status == pop_status_null)
            {
                *(p_recv_buffer + data_len) = -1;
                data_len++;
            }
            else
            {
                assert(false);
            }
        }//for col++
    }//for row++

    *p_data_len = data_len;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//
//param:    [out,opt]p_shoot_pop: the pop just added to the map
//note:     if shoot pos is invalid,return E_FAIL,if success,return S_OK,else return error code

int c_single_pop_game_t::add_shooted_pop(int row,int col,c_pop_t* p_shooted_pop)
{
    //---------------------
    //check current game progress
    if(m_game_progress != game_progress_going)
    {
        return E_LOGIC;
    }

    //---------------------
    //verify shoot pos
    int result = verify_shoot_pop_pos(row,col);

    if(result != S_OK)
    {
        //---------------------
        //invalid shoot pos,game is over
        m_game_progress = game_progress_over_fail;

        return E_FAIL;
    }

    //---------------------
    //place shooted pop to it's position
    pop_type_t current_shoot_pop_type;
    m_current_shoot_pop.get_pop_type(&current_shoot_pop_type);

    pop_color_t current_shoot_pop_color;
    m_current_shoot_pop.get_pop_color(&current_shoot_pop_color);

    m_pop_map[row][col].set_pop_status(pop_status_present);
    m_pop_map[row][col].set_pop_type(current_shoot_pop_type);
    m_pop_map[row][col].set_pop_color(current_shoot_pop_color);

    if(p_shooted_pop != null)
    {
        *p_shooted_pop = m_pop_map[row][col];
    }

    return S_OK;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::get_current_shoot_pop(c_pop_t* p_current_shoot_pop)
{
    //---------------------
    //param check
    if(null == p_current_shoot_pop)
    {
        return E_INVALIDARG;
    }

    *p_current_shoot_pop = m_current_shoot_pop;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::get_current_bakup_pop(c_pop_t* p_current_bakup_pop)
{
    //---------------------
    //param check
    if(null == p_current_bakup_pop)
    {
        return E_INVALIDARG;
    }

    *p_current_bakup_pop = m_current_bakup_pop;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//
//note: generate new bakup pop,current bakup pop replace current shoot pop

int c_single_pop_game_t::request_new_bakup_pop(c_pop_t* p_new_bakup_pop)
{
    //---------------------
    //check game progress
    if(m_game_progress != game_progress_going)
    {
        return E_LOGIC;
    }

    //---------------------
    //param check
    if(null == p_new_bakup_pop)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //generate new bakup pop
    c_pop_t new_bakup_pop;
    new_bakup_pop.reset_pop_data();

    new_bakup_pop.set_pop_status(pop_status_for_shoot);
    new_bakup_pop.set_pop_type(pop_type_normal);

    pop_color_t new_bakup_pop_color;
    c_pop_t::generate_random_color(&new_bakup_pop_color);

    new_bakup_pop.set_pop_color(new_bakup_pop_color);

    //---------------------
    //current bakup pop should replace current shoot pop
    m_current_shoot_pop = m_current_bakup_pop;
    m_current_bakup_pop = new_bakup_pop;

    *p_new_bakup_pop = new_bakup_pop;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::use_props_shoot()
{
    //---------------------
    //check game progress
    if(m_game_progress != game_progress_going)
    {
        return E_LOGIC;
    }

    //---------------------
    //determine whether able to use shoot
    if(m_props_shoot_num < 1)
    {
        return E_LOGIC;
    }

    //---------------------
    //use shoot
    m_props_shoot_num--;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//
//note: the bomb should replace current baukp pop

int c_single_pop_game_t::use_props_bomb()
{
    //---------------------
    //check game progress
    if(m_game_progress != game_progress_going)
    {
        return E_LOGIC;
    }

    //---------------------
    //determine whether able to use bomb
    if(m_props_bomb_num < 1)
    {
        return E_LOGIC;
    }

    //---------------------
    //the bomb replace current bakup pop
    m_current_bakup_pop.set_pop_type(pop_type_bomb);
    m_props_bomb_num--;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::use_props_spider()
{
    //---------------------
    //check game progress
    if(m_game_progress != game_progress_going)
    {
        return E_LOGIC;
    }

    //---------------------
    //determine whether able to use spider
    if(m_props_spider_num < 1)
    {
        return E_LOGIC;
    }

    //---------------------
    //use spider
    m_props_spider_num--;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::get_game_progress(single_pop_game_progress_t* p_game_progress)
{
    //---------------------
    //param check
    if(null == p_game_progress)
    {
        return E_INVALIDARG;
    }

    *p_game_progress = m_game_progress;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::get_current_level()
{
    return m_current_level;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::get_props_shoot_num()
{
    return m_props_shoot_num;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::get_props_bomb_num()
{
    return m_props_bomb_num;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::get_props_spider_num()
{
    return m_props_spider_num;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::get_total_score()
{
    return m_total_score;
}

///----------------------------------//
//-----------------------------------//
//the colorful pop should in screen scale
//note: return S_OK if successfully generate a colorful pop,return E_FAIL failed to generate a colorful pop

int c_single_pop_game_t::generate_colorful_pop(int ttl,c_pop_t* p_new_colorful_pop)
{
    //---------------------
    //check game progress
    if(m_game_progress != game_progress_going)
    {
        return E_LOGIC;
    }

    //---------------------
    //param check
    if(null == p_new_colorful_pop || ttl < 0)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //determine whether already had colorful pop
    if(m_current_colorful_pop.get_pop_id() != INVALID_POP_ID)
    {
        return E_LOGIC;
    }

    //---------------------
    //get a random num
    srand(time(null));
    int random_num = rand();

    int index = 0;

    //---------------------
    //random generate colorful pop in min row and max row
    for(int row = m_min_row_index; row <= m_max_row_index; row++)
    {
        if(row >= _MAX_POP_MAP_ROW_NUM || row < 0)
        {
            break;
        }

        for(int col = 0; col < _MAX_POP_MAP_COL_NUM; col++)
        {
            pop_status_t pop_status;
            m_pop_map[row][col].get_pop_status(&pop_status);

            pop_type_t pop_type;
            m_pop_map[row][col].get_pop_type(&pop_type);

            if(pop_status == pop_status_present && pop_type == pop_type_normal)
            {
                if(index >= random_num % 25)
                {
                    //---------------------
                    //it's the new colorful pop
                    m_pop_map[row][col].set_pop_type(pop_type_colorful);

                    //---------------------
                    //set colorful pop ttl(time to live)
                    m_pop_map[row][col].set_user_data(ttl);

                    m_current_colorful_pop = m_pop_map[row][col];
                    *p_new_colorful_pop = m_pop_map[row][col];

                    return S_OK;
                }
            }
            else
            {
                continue;
            }

            //---------------------
            //recycle to loop
            index++;

            if(row == m_max_row_index && col == _MAX_POP_MAP_COL_NUM - 1)
            {
                row = m_min_row_index - 1;
            }

        }//for col++

        //---------------------
        //avoid infinite loop
        if(index >= _MAX_POP_MAP_ROW_NUM * _MAX_POP_MAP_COL_NUM)
        {
            break;
        }

    }//for row++

    return E_FAIL;
}

///----------------------------------//
//-----------------------------------//
//note: if the colorful pop is been erased,should reset current colorful pop
//note: if colorful pop is cleared,return S_OK,if ttl > 0,return S_FALSE or error code

int c_single_pop_game_t::clear_colorful_pop(c_pop_t* p_cleared_colorful_pop)
{
    //---------------------
    //check game progress
    if(m_game_progress != game_progress_going)
    {
        return E_LOGIC;
    }

    //---------------------
    //param check
    if(null == p_cleared_colorful_pop)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //determine whether has colorful pop to clear
    if(m_current_colorful_pop.get_pop_id() == INVALID_POP_ID)
    {
        return E_LOGIC;
    }

    //---------------------
    //get pop pos
    int col = high_word(m_current_colorful_pop.get_pop_id());
    int row = low_word(m_current_colorful_pop.get_pop_id());

    if(row < 0 || row >= _MAX_POP_MAP_ROW_NUM)
    {
        return E_LOGIC;
    }

    if(col <0 || col >= _MAX_POP_MAP_COL_NUM)
    {
        return E_LOGIC;
    }

    pop_status_t pop_status;
    m_pop_map[row][col].get_pop_status(&pop_status);

    pop_type_t pop_type;
    m_pop_map[row][col].get_pop_type(&pop_type);

    if(pop_status == pop_status_present && pop_type == pop_type_colorful)
    {
        //---------------------
        //get ttl of colorful pop
        int ttl = m_pop_map[row][col].get_user_data();

        if(ttl > 0)
        {
            //---------------------
            //unable to clear
            ttl--;
            m_pop_map[row][col].set_user_data(ttl);

            return S_FALSE;
        }
        else
        {
            //---------------------
            //the colorful pop should be cleared
            *p_cleared_colorful_pop = m_pop_map[row][col];
            m_pop_map[row][col].set_pop_type(pop_type_normal);

            m_current_colorful_pop.reset_pop_data();

            return S_OK;
        }
    }
    else
    {
        //---------------------
        //pop may had been erased(or cleared)(no need to response to client)
        m_current_colorful_pop.reset_pop_data();

        return E_LOGIC;
    }
}

///----------------------------------//
//-----------------------------------//

#ifndef _MULTI_PLAYER_VERSION

int c_single_pop_game_t::get_random_props()
{
    //---------------------
    //get a random num
    srand(time(null));
    int random_num = rand();

    if(random_num % 8 <= 5)
    {
        m_props_shoot_num++;
    }
    else
    {
        m_props_bomb_num++;
    }

    return 0;
}

#else

int c_single_pop_game_t::get_random_props()
{
    //---------------------
    //get a random num
    srand(time(null));
    int random_num = rand();

    if(random_num % 8 <= 3)
    {
        m_props_shoot_num++;
    }
    else if( (random_num % 8 > 3) && (random_num % 8 <= 5) )
    {
        m_props_bomb_num++;
    }
    else
    {
        m_props_spider_num++;
    }

    return 0;
}

#endif//_MULTI_PLAYER_VERSION

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::get_bottom_row_index()
{
    return m_bottom_row_index;
}

///----------------------------------//
//-----------------------------------//
//param:    [in]center_pop:  which neighbors to find
//param:    [out]p_neighbors: all neighbors's pop id(may not present)
//return:   S_OK if success or error code if failed

int c_single_pop_game_t::find_neighbors(c_pop_t& center_pop,pop_neighbors_t* p_neighbors)
{
    //---------------------
    //param check
    if(null == p_neighbors)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //determin the center pop is on odd line or even line
    pop_pos_t pop_pos;
    memset(&pop_pos,0,sizeof(pop_pos));

    center_pop.get_pop_pos(&pop_pos);

    //---------------------
    //verify the center_pop
    if(pop_pos.x >= _MAX_POP_MAP_COL_NUM || pop_pos.y >= _MAX_POP_MAP_ROW_NUM)
    {
        return E_INVALIDARG;
    }

    if(pop_pos.y % 2)
    {
        //---------------------
        //pop is on odd line,x should < _MAX_POP_MAP_COL_NUM - 1
        if(pop_pos.x >= _MAX_POP_MAP_COL_NUM - 1)
        {
            return E_INVALIDARG;
        }
    }

    //---------------------
    //get neighbors of the center_pop
    p_neighbors->neighbors_num = 0;

    if(pop_pos.y % 2)
    {
        //---------------------
        //pop is on odd line,get 1 position
        if(pop_pos.y < _MAX_POP_MAP_ROW_NUM - 1)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x,pop_pos.y + 1);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 2 position
        if(pop_pos.y < _MAX_POP_MAP_ROW_NUM - 1)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x + 1,pop_pos.y + 1);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 3 position
        if(pop_pos.x < _MAX_POP_MAP_COL_NUM - 1 - 1)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x + 1,pop_pos.y);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 4 position
        if(pop_pos.y > 0)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x + 1,pop_pos.y - 1);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 5 position
        if(pop_pos.y > 0)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x,pop_pos.y - 1);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 6 position
        if(pop_pos.x > 0)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x - 1,pop_pos.y);
            p_neighbors->neighbors_num++;
        }

        assert(p_neighbors->neighbors_num > 0);
        assert(p_neighbors->neighbors_num <= 6);

        return S_OK;
    }//if on odd line
    else
    {
        //---------------------
        //pop is on even line,get 1 position
        if(pop_pos.y < _MAX_POP_MAP_ROW_NUM - 1 && pop_pos.x > 0)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x - 1,pop_pos.y + 1);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 2 position
        if(pop_pos.y < _MAX_POP_MAP_ROW_NUM - 1 && pop_pos.x < _MAX_POP_MAP_COL_NUM - 1)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x,pop_pos.y + 1);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 3 position
        if(pop_pos.x < _MAX_POP_MAP_COL_NUM - 1)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x + 1,pop_pos.y);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 4 position
        if(pop_pos.y > 0 && pop_pos.x < _MAX_POP_MAP_COL_NUM - 1)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x,pop_pos.y - 1);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 5 position
        if(pop_pos.y > 0 && pop_pos.x > 0)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x - 1,pop_pos.y - 1);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 6 position
        if(pop_pos.x > 0)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x - 1,pop_pos.y);
            p_neighbors->neighbors_num++;
        }

        assert(p_neighbors->neighbors_num > 0);
        assert(p_neighbors->neighbors_num <= 6);

        return S_OK;

    }//else on even line
}

///----------------------------------//
//-----------------------------------//
//note: similar to find_neighbors except the pops out of screen
//note: the center_pop must be on screen

int c_single_pop_game_t::find_neighbors_on_screen(c_pop_t& center_pop,pop_neighbors_t* p_neighbors)
{
    //---------------------
    //param check
    if(null == p_neighbors)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //determine the center pop is on odd line or even line
    pop_pos_t pop_pos;
    memset(&pop_pos,0,sizeof(pop_pos));

    center_pop.get_pop_pos(&pop_pos);

    //---------------------
    //verify the center_pop
    if(pop_pos.x >= _MAX_POP_MAP_COL_NUM || pop_pos.y >= _MAX_POP_MAP_ROW_NUM)
    {
        return E_INVALIDARG;
    }

    if(pop_pos.y % 2)
    {
        //---------------------
        //pop is on odd line,x should < _MAX_POP_MAP_COL_NUM - 1
        if(pop_pos.x >= _MAX_POP_MAP_COL_NUM - 1)
        {
            return E_INVALIDARG;
        }
    }

    //---------------------
    //determine whether the center_pop is on screen
    if(pop_pos.y > m_bottom_row_index + _TOTAL_ROW_NUM_PER_SCREEN - 1)
    {
        p_neighbors->neighbors_num = 0;
        return S_OK;
    }

    //---------------------
    //get neighbors of the center_pop
    p_neighbors->neighbors_num = 0;

    if(pop_pos.y % 2)
    {
        //---------------------
        //pop is on odd line,get 1 position
        if(pop_pos.y < min(m_bottom_row_index + _TOTAL_ROW_NUM_PER_SCREEN - 1,_MAX_POP_MAP_ROW_NUM - 1) )
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x,pop_pos.y + 1);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 2 position
        if(pop_pos.y < min(m_bottom_row_index + _TOTAL_ROW_NUM_PER_SCREEN - 1,_MAX_POP_MAP_ROW_NUM - 1) )
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x + 1,pop_pos.y + 1);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 3 position
        if(pop_pos.x < _MAX_POP_MAP_COL_NUM - 1 - 1)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x + 1,pop_pos.y);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 4 position
        if(pop_pos.y > 0)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x + 1,pop_pos.y - 1);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 5 position
        if(pop_pos.y > 0)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x,pop_pos.y - 1);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 6 position
        if(pop_pos.x > 0)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x - 1,pop_pos.y);
            p_neighbors->neighbors_num++;
        }

        assert(p_neighbors->neighbors_num > 0);
        assert(p_neighbors->neighbors_num <= 6);

        return S_OK;
    }//if on odd line
    else
    {
        //---------------------
        //pop is on even line,get 1 position
        if(pop_pos.y < min(m_bottom_row_index + _TOTAL_ROW_NUM_PER_SCREEN - 1,_MAX_POP_MAP_ROW_NUM - 1) && pop_pos.x > 0)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x - 1,pop_pos.y + 1);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 2 position
        if(pop_pos.y < min(m_bottom_row_index + _TOTAL_ROW_NUM_PER_SCREEN - 1,_MAX_POP_MAP_ROW_NUM - 1) && pop_pos.x < _MAX_POP_MAP_COL_NUM - 1)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x,pop_pos.y + 1);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 3 position
        if(pop_pos.x < _MAX_POP_MAP_COL_NUM - 1)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x + 1,pop_pos.y);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 4 position
        if(pop_pos.y > 0 && pop_pos.x < _MAX_POP_MAP_COL_NUM - 1)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x,pop_pos.y - 1);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 5 position
        if(pop_pos.y > 0 && pop_pos.x > 0)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x - 1,pop_pos.y - 1);
            p_neighbors->neighbors_num++;
        }

        //---------------------
        //get 6 position
        if(pop_pos.x > 0)
        {
            p_neighbors->neighbors_array[p_neighbors->neighbors_num] = make_word(pop_pos.x - 1,pop_pos.y);
            p_neighbors->neighbors_num++;
        }

        assert(p_neighbors->neighbors_num > 0);
        assert(p_neighbors->neighbors_num <= 6);

        return S_OK;

    }//else on even line
}

///----------------------------------//
//-----------------------------------//
//note: similar to find_neighbors,the difference is that it find present neighbors only

int c_single_pop_game_t::find_present_neighbors(c_pop_t& center_pop,pop_neighbors_t* p_neighbors)
{
    //---------------------
    //param check
    if(null == p_neighbors)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //find all neighbors
    pop_neighbors_t pop_neighbors;
    memset(&pop_neighbors,0,sizeof(pop_neighbors));

    int result = find_neighbors(center_pop,&pop_neighbors);

    if(result != S_OK)
    {
        return result;
    }
    else
    {
        assert(pop_neighbors.neighbors_num > 0);

        //---------------------
        //extract present pop
        p_neighbors->neighbors_num = 0;

        for(int index = 0; index < pop_neighbors.neighbors_num; index++)
        {
            int col = high_word(pop_neighbors.neighbors_array[index]);
            int row = low_word(pop_neighbors.neighbors_array[index]);

            pop_status_t pop_status;
            m_pop_map[row][col].get_pop_status(&pop_status);

            assert(pop_status != pop_status_null);

            if(pop_status == pop_status_present)
            {
                p_neighbors->neighbors_array[p_neighbors->neighbors_num] = pop_neighbors.neighbors_array[index];
                p_neighbors->neighbors_num++;
            }
        }

        return S_OK;
    }//else
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::find_present_neighbors_on_screen(c_pop_t& center_pop,pop_neighbors_t* p_neighbors)
{
    //---------------------
    //param check
    if(null == p_neighbors)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //find all neighbors on screen
    pop_neighbors_t pop_neighbors;
    memset(&pop_neighbors,0,sizeof(pop_neighbors));

    int result = find_neighbors_on_screen(center_pop,&pop_neighbors);

    if(result != S_OK)
    {
        return result;
    }
    else
    {
        //assert(pop_neighbors.neighbors_num > 0);

        //---------------------
        //extract present pop on screen
        p_neighbors->neighbors_num = 0;

        for(int index = 0; index < pop_neighbors.neighbors_num; index++)
        {
            int col = high_word(pop_neighbors.neighbors_array[index]);
            int row = low_word(pop_neighbors.neighbors_array[index]);

            pop_status_t pop_status;
            m_pop_map[row][col].get_pop_status(&pop_status);

            assert(pop_status != pop_status_null);

            if(pop_status == pop_status_present)
            {
                p_neighbors->neighbors_array[p_neighbors->neighbors_num] = pop_neighbors.neighbors_array[index];
                p_neighbors->neighbors_num++;
            }
        }

        return S_OK;
    }//else
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::find_present_neighbors_same_color(c_pop_t& center_pop,pop_neighbors_t* p_neighbors)
{
    //---------------------
    //param check
    if(null == p_neighbors)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //find all neighbors
    pop_neighbors_t pop_neighbors;
    memset(&pop_neighbors,0,sizeof(pop_neighbors));

    int result = find_neighbors(center_pop,&pop_neighbors);

    if(result != S_OK)
    {
        return result;
    }
    else
    {
        assert(pop_neighbors.neighbors_num > 0);

        //---------------------
        //extract present pop with same color
        p_neighbors->neighbors_num = 0;

        for(int index = 0; index < pop_neighbors.neighbors_num; index++)
        {
            int col = high_word(pop_neighbors.neighbors_array[index]);
            int row = low_word(pop_neighbors.neighbors_array[index]);

            pop_status_t pop_status;
            m_pop_map[row][col].get_pop_status(&pop_status);
            assert(pop_status != pop_status_null);

            pop_type_t pop_type;
            m_pop_map[row][col].get_pop_type(&pop_type);

            pop_color_t pop_color;
            m_pop_map[row][col].get_pop_color(&pop_color);

            pop_color_t center_pop_color;
            center_pop.get_pop_color(&center_pop_color);

            if(pop_status == pop_status_present && pop_color == center_pop_color && (pop_type == pop_type_normal || pop_type == pop_type_colorful) )
            {
                p_neighbors->neighbors_array[p_neighbors->neighbors_num] = pop_neighbors.neighbors_array[index];
                p_neighbors->neighbors_num++;
            }
        }

        return S_OK;
    }//else
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::find_present_neighbors_same_color_on_screen(c_pop_t& center_pop,pop_neighbors_t* p_neighbors)
{
    //---------------------
    //param check
    if(null == p_neighbors)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //find all neighbors on screen
    pop_neighbors_t pop_neighbors;
    memset(&pop_neighbors,0,sizeof(pop_neighbors));

    int result = find_neighbors_on_screen(center_pop,&pop_neighbors);

    if(result != S_OK)
    {
        return result;
    }
    else
    {
        //assert(pop_neighbors.neighbors_num > 0);

        //---------------------
        //extract present pop with same color on screen
        p_neighbors->neighbors_num = 0;

        for(int index = 0; index < pop_neighbors.neighbors_num; index++)
        {
            int col = high_word(pop_neighbors.neighbors_array[index]);
            int row = low_word(pop_neighbors.neighbors_array[index]);

            pop_status_t pop_status;
            m_pop_map[row][col].get_pop_status(&pop_status);
            assert(pop_status != pop_status_null);

            pop_type_t pop_type;
            m_pop_map[row][col].get_pop_type(&pop_type);

            pop_color_t pop_color;
            m_pop_map[row][col].get_pop_color(&pop_color);

            pop_color_t center_pop_color;
            center_pop.get_pop_color(&center_pop_color);

            if(pop_status == pop_status_present && pop_color == center_pop_color && (pop_type == pop_type_normal || pop_type == pop_type_colorful) )
            {
                p_neighbors->neighbors_array[p_neighbors->neighbors_num] = pop_neighbors.neighbors_array[index];
                p_neighbors->neighbors_num++;
            }
        }

        return S_OK;
    }//else
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::reset_game_data()
{
    //---------------------
    //reset and init all pop data on pop map
    for(int row = 0; row < _MAX_POP_MAP_ROW_NUM; row++)
    {
        for(int col = 0; col < _MAX_POP_MAP_COL_NUM; col++)
        {
            m_pop_map[row][col].reset_pop_data();

            if(row < _INIT_START_ROW_INDEX)
            {
                if(row % 2)
                {
                    //---------------------
                    //pop on odd line
                    if(col < _MAX_POP_MAP_COL_NUM - 1)
                    {
                        m_pop_map[row][col].set_pop_status(pop_status_erased);

                        pop_pos_t pop_pos;
                        pop_pos.x = col;
                        pop_pos.y = row;
                        m_pop_map[row][col].set_pop_pos(pop_pos);
                    }
                }
                else
                {
                    //---------------------
                    //pop on even line
                    m_pop_map[row][col].set_pop_status(pop_status_erased);

                    pop_pos_t pop_pos;
                    pop_pos.x = col;
                    pop_pos.y = row;
                    m_pop_map[row][col].set_pop_pos(pop_pos);
                }
            }// if pop is on white space
            else
            {
                if(row % 2)
                {
                    //---------------------
                    //pop on odd line
                    if(col < _MAX_POP_MAP_COL_NUM - 1)
                    {
                        //---------------------
                        //generate random normal pop
                        m_pop_map[row][col].set_pop_status(pop_status_present);
                        m_pop_map[row][col].set_pop_type(pop_type_normal);

                        pop_pos_t pop_pos;
                        pop_pos.x = col;
                        pop_pos.y = row;
                        m_pop_map[row][col].set_pop_pos(pop_pos);

                        pop_color_t random_pop_color;
                        c_pop_t::generate_random_color(&random_pop_color);

                        m_pop_map[row][col].set_pop_color(random_pop_color);
                    }
                }
                else
                {
                    //---------------------
                    //pop on even line

                    //---------------------
                    //generate random normal pop
                    m_pop_map[row][col].set_pop_status(pop_status_present);
                    m_pop_map[row][col].set_pop_type(pop_type_normal);

                    pop_pos_t pop_pos;
                    pop_pos.x = col;
                    pop_pos.y = row;
                    m_pop_map[row][col].set_pop_pos(pop_pos);

                    pop_color_t random_pop_color;
                    c_pop_t::generate_random_color(&random_pop_color);

                    m_pop_map[row][col].set_pop_color(random_pop_color);
                }
            }//else
        }//for col++
    }//for row++

    //---------------------
    //regenerate current_shoot_pop and current_bakup_pop(third pop to fifth pop 2008-06-24)
    m_current_shoot_pop.reset_pop_data();
    m_current_shoot_pop.set_pop_status(pop_status_for_shoot);
    m_current_shoot_pop.set_pop_type(pop_type_normal);

    pop_color_t random_pop_color;
    c_pop_t::generate_random_color(&random_pop_color);
    m_current_shoot_pop.set_pop_color(random_pop_color);

    //---------------------
    m_current_bakup_pop.reset_pop_data();
    m_current_bakup_pop.set_pop_status(pop_status_for_shoot);
    m_current_bakup_pop.set_pop_type(pop_type_normal);

    c_pop_t::generate_random_color(&random_pop_color);
    m_current_bakup_pop.set_pop_color(random_pop_color);

    //---------------------
    //third pop
    m_third_pop.reset_pop_data();
    m_third_pop.set_pop_status(pop_status_for_shoot);
    m_third_pop.set_pop_type(pop_type_normal);

    c_pop_t::generate_random_color(&random_pop_color);
    m_third_pop.set_pop_color(random_pop_color);

    //---------------------
    //forth pop
    m_forth_pop.reset_pop_data();
    m_forth_pop.set_pop_status(pop_status_for_shoot);
    m_forth_pop.set_pop_type(pop_type_normal);

    c_pop_t::generate_random_color(&random_pop_color);
    m_forth_pop.set_pop_color(random_pop_color);

    //---------------------
    //fifth pop
    m_fifth_pop.reset_pop_data();
    m_fifth_pop.set_pop_status(pop_status_for_shoot);
    m_fifth_pop.set_pop_type(pop_type_normal);

    c_pop_t::generate_random_color(&random_pop_color);
    m_fifth_pop.set_pop_color(random_pop_color);

    //---------------------
    //reset other data
    m_current_colorful_pop.reset_pop_data();

    m_max_row_index = _INIT_START_ROW_INDEX;
    m_min_row_index = _INIT_START_ROW_INDEX;

    m_bottom_row_index = 0;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//
//note: pop may be not present,check the pos only

bool c_single_pop_game_t::is_pop_in_the_air(c_pop_t& pop)
{
    //---------------------
    //assert the pop is not null
    pop_status_t pop_status;
    pop.get_pop_status(&pop_status);

    assert(pop_status != pop_status_null);
    assert(pop_status != pop_status_for_shoot);

    //---------------------
    //if pop is on the max row,it's not in the air
    int row = low_word(pop.get_pop_id());

    if(row == _MAX_POP_MAP_ROW_NUM - 1)
    {
        return false;
    }

    //---------------------
    //get neighbors of the pop
    pop_neighbors_t pop_neighbors;
    memset(&pop_neighbors,0,sizeof(pop_neighbors));

    find_present_neighbors(pop,&pop_neighbors);

    if(pop_neighbors.neighbors_num > 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

///----------------------------------//
//-----------------------------------//
//if the pop pos is valid, return S_OK,else return error code

int c_single_pop_game_t::verify_shoot_pop_pos(int row,int col)
{
    //---------------------
    //param check
    if(row < 0 || row >= _MAX_POP_MAP_ROW_NUM)
    {
        return E_INVALIDARG;
    }

    if(col < 0 || col >= _MAX_POP_MAP_COL_NUM)
    {
        return E_INVALIDARG;
    }

    if(row % 2)
    {
        if(col >= _MAX_POP_MAP_COL_NUM - 1)
        {
            return E_INVALIDARG;
        }
    }

    //---------------------
    //check row
    if(row < 0)
    {
        return E_LOGIC;
    }

    //---------------------
    //check whether the pos is below the screen or above screen
    //note: row may be lower 1 than m_bottom_row_index
    if(row < m_bottom_row_index - 1)
    {
        return E_LOGIC;
    }

    if(row > min(m_bottom_row_index + _TOTAL_ROW_NUM_PER_SCREEN - 1,_MAX_POP_MAP_ROW_NUM - 1) )
    {
        return E_LOGIC;
    }

    //---------------------
    //check whether the pos is already present
    pop_status_t pop_status;
    m_pop_map[row][col].get_pop_status(&pop_status);

    assert(pop_status != pop_status_null);
    assert(pop_status != pop_status_for_shoot);

    if(pop_status == pop_status_present)
    {
        return E_LOGIC;
    }

    //---------------------
    //check whether the pos is in the air
    bool result = is_pop_in_the_air(m_pop_map[row][col]);

    if(result)
    {
        return E_LOGIC;
    }

    return S_OK;
}

///----------------------------------//
//-----------------------------------//
//param:    [in]pop: the shoot pop
//param:    [out]p_recv_buffer: recv buffer of the erased pop (array)
//param:    [in]buffer_len: recv buffer len
//param:    [out]p_data_len: data len in recv buffer
//return:   S_OK if success or S_FALSE if level is over(success or fail),error code
//note:     if pops was erased,must call clear_isolated_pop

int c_single_pop_game_t::erase_same_color_pop_group(c_pop_t& pop,unsigned short* p_recv_buffer,int buffer_len,int* p_data_len)
{
    //---------------------
    //check game progress
    if(m_game_progress != game_progress_going)
    {
        return E_LOGIC;
    }

    //---------------------
    //param check
    if(null == p_recv_buffer || null == p_data_len || buffer_len < 1)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //verify pop type
    pop_type_t pop_type;
    pop.get_pop_type(&pop_type);

    assert(pop_type == pop_type_normal);

    //---------------------
    //clear temp pop set first
    m_temp_pop_set.clear();

    //---------------------
    //add self to temp pop set
    m_temp_pop_set.insert(pop.get_pop_id());

    //---------------------
    //recursively get all neighbor same color pop
    find_same_color_pop_recursive(pop);

    //---------------------
    //deal all pops which may be erased
    if(m_temp_pop_set.size() < 3)
    {
        //---------------------
        //unable to erase
        pop_set_iterator_t iterator = m_temp_pop_set.begin();

        for(; iterator != m_temp_pop_set.end(); ++iterator)
        {
            unsigned short pop_id = *iterator;
            int row = low_word(pop_id);

            //---------------------
            //update min row index,min index can only be smaller
            m_min_row_index = min(m_min_row_index,row);
        }

        //---------------------
        //if the min row index < m_bottom_row_index,the game is over
        if(m_min_row_index < m_bottom_row_index)
        {
            m_game_progress = game_progress_over_fail;

            return S_FALSE;
        }

        //---------------------
        //update max row index
        int index = _MAX_POP_MAP_ROW_NUM - 1;

        for(; index >= m_min_row_index; index--)
        {
            pop_row_status_t pop_row_status;
            get_pop_row_status(index,&pop_row_status);

            if(pop_row_status == pop_row_status_full)
            {
                if(index > m_min_row_index)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }

        m_max_row_index = index;
        assert(m_max_row_index >= m_min_row_index);

        //---------------------
        //return
        *p_data_len = 0;

        return S_OK;
    }//if
    else
    {
        //---------------------
        //should erase
        int data_len = 0;
        pop_set_iterator_t iterator = m_temp_pop_set.begin();

        for(; iterator != m_temp_pop_set.end(); ++iterator)
        {
            unsigned short pop_id = *iterator;

            //---------------------
            //erase pops
            int col = high_word(pop_id);
            int row = low_word(pop_id);

            //---------------------
            //determine whether is colorful pop
            pop_type_t pop_type;
            m_pop_map[row][col].get_pop_type(&pop_type);

            if(pop_type == pop_type_colorful)
            {
                //---------------------
                //colorful pop,get random props
                get_random_props();
            }

            m_pop_map[row][col].set_pop_status(pop_status_erased);

            //---------------------
            //update max index,max row index can only be larger
            m_max_row_index = max(m_max_row_index,row);

            //---------------------
            //add pop to recv buffer
            if(data_len < buffer_len)
            {
                *(p_recv_buffer + data_len) = pop_id;
               data_len++;
            }
        }

        assert(data_len >= 3);

        *p_data_len = data_len;

        //---------------------
        //update score
        if(data_len >= 6)
        {
            m_total_score += (data_len * 20);
        }
        else
        {
            m_total_score += (data_len * 10);
        }

        //---------------------
        //determine whether max row is empty
        pop_row_status_t pop_row_status;
        get_pop_row_status(m_max_row_index,&pop_row_status);

        assert(pop_row_status != pop_row_status_full);

        if(pop_row_status == pop_row_status_empty)
        {
            m_max_row_index++;

            //---------------------
            //determin whether all pop is cleared
            if(m_max_row_index >= _MAX_POP_MAP_ROW_NUM && (m_max_row_index - 1 == m_min_row_index))
            {
                //---------------------
                //all pop has been cleared,level is over,and success
                m_game_progress = game_progress_level_over;

                return S_FALSE;
            }
        }

        return S_OK;
    }//else
}

///----------------------------------//
//-----------------------------------//
//note: recursively find all pops with the same color in neighborhood

int c_single_pop_game_t::find_same_color_pop_recursive(c_pop_t& center_pop)
{
    //---------------------
    //get all present neighbors with same color on screen
    pop_neighbors_t same_color_neighbors;
    memset(&same_color_neighbors,0,sizeof(same_color_neighbors));

    int result = find_present_neighbors_same_color_on_screen(center_pop,&same_color_neighbors);

    if(result != S_OK)
    {
        return result;
    }

    if(same_color_neighbors.neighbors_num <= 0)
    {
        //---------------------
        //must be first run
        return S_OK;
    }

    //---------------------
    //extract new pops (not already in m_temp_pop_set)
    pop_neighbors_t new_same_color_neighbors;
    memset(&new_same_color_neighbors,0,sizeof(new_same_color_neighbors));

    for(int index = 0; index < same_color_neighbors.neighbors_num; index++)
    {
        std::pair<pop_set_iterator_t,bool> pop_pair = m_temp_pop_set.insert(same_color_neighbors.neighbors_array[index]);

        if(pop_pair.second)
        {
            //---------------------
            //new pop with same color
            new_same_color_neighbors.neighbors_array[new_same_color_neighbors.neighbors_num] = same_color_neighbors.neighbors_array[index];
            new_same_color_neighbors.neighbors_num++;
        }
    }

    for(int index = 0; index < new_same_color_neighbors.neighbors_num; index++)
    {
        //---------------------
        //new pop with same color,recursively loop
        int col = high_word(new_same_color_neighbors.neighbors_array[index]);
        int row = low_word(new_same_color_neighbors.neighbors_array[index]);

        find_same_color_pop_recursive(m_pop_map[row][col]);
    }

    return S_OK;
}

///----------------------------------//
//-----------------------------------//
//note:  return S_OK if success or error code

int c_single_pop_game_t::get_pop_row_status(int row,pop_row_status_t* p_pop_row_status)
{
    //---------------------
    //param check
    if(row < 0 || row >= _MAX_POP_MAP_ROW_NUM || null == p_pop_row_status)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //check row status
    bool is_full = true;
    bool is_empty = true;

    for(int index = 0; index < _MAX_POP_MAP_COL_NUM; index++)
    {
        pop_status_t pop_status;
        m_pop_map[row][index].get_pop_status(&pop_status);

        if(pop_status == pop_status_null)
        {
            assert(index == _MAX_POP_MAP_COL_NUM - 1);
            continue;
        }
        else if(pop_status == pop_status_present)
        {
            is_empty = false;
        }
        else if(pop_status == pop_status_erased)
        {
            is_full = false;
        }
        else
        {
            assert(false);
        }
    }//for

    assert( !(is_full && is_empty) );

    if(is_full)
    {
        *p_pop_row_status = pop_row_status_full;

        return S_OK;
    }
    else if(is_empty)
    {
        *p_pop_row_status = pop_row_status_empty;

        return S_OK;
    }
    else
    {
        *p_pop_row_status = pop_row_status_not_full;

        return S_OK;
    }
}

///----------------------------------//
//-----------------------------------//
//note: return S_OK if find path,E_FAIL if no path,S_FALSE if uncertain

int c_single_pop_game_t::find_connected_pop_path_recursive(c_pop_t& center_pop)
{
    //---------------------
    //get all present neighbors
    pop_neighbors_t all_present_neighbors;
    memset(&all_present_neighbors,0,sizeof(all_present_neighbors));

    find_present_neighbors(center_pop,&all_present_neighbors);

    if(all_present_neighbors.neighbors_num <= 0)
    {
        return E_FAIL;
    }

    //---------------------
    //extract new pops (not already in m_temp_pop_set)
    pop_neighbors_t new_neighbors;
    memset(&new_neighbors,0,sizeof(new_neighbors));

    for(int index = 0; index < all_present_neighbors.neighbors_num; index++)
    {
        std::pair<pop_set_iterator_t,bool> pop_pair = m_temp_pop_set.insert(all_present_neighbors.neighbors_array[index]);

        if(pop_pair.second)
        {
            //---------------------
            //new pops
            new_neighbors.neighbors_array[new_neighbors.neighbors_num] = all_present_neighbors.neighbors_array[index];
            new_neighbors.neighbors_num++;
        }
    }//for

    if(new_neighbors.neighbors_num <= 0)
    {
        //---------------------
        //uncertain
        return S_FALSE;
    }

    //---------------------
    //determine whether has neighbors which is connected pop or is in connected pop set,or is in isolated pop set
    for(int index = 0; index < new_neighbors.neighbors_num; index++)
    {
        //---------------------
        //determine whether is connected pop
        //int col = high_word(new_neighbors.neighbors_array[index]);
        int row = low_word(new_neighbors.neighbors_array[index]);

        if(row >= _MAX_POP_MAP_ROW_NUM - 1 || row > m_max_row_index)
        {
            return S_OK;
        }

        //---------------------
        //determine whether in connected pop set
        if(m_temp_connected_pop_set.count(new_neighbors.neighbors_array[index]))
        {
            return S_OK;
        }

        //---------------------
        //determine whether in isolated pop set
        if(m_temp_isolated_pop_set.count(new_neighbors.neighbors_array[index]))
        {
            return E_FAIL;
        }
    }//for

    //---------------------
    //continue to recursively find path
    for(int index = 0; index < new_neighbors.neighbors_num; index++)
    {
        int col = high_word(new_neighbors.neighbors_array[index]);
        int row = low_word(new_neighbors.neighbors_array[index]);

        int result = find_connected_pop_path_recursive(m_pop_map[row][col]);

        if(result == S_OK)
        {
            return S_OK;
        }
        else if(result == E_FAIL)
        {
            return E_FAIL;
        }
        else
        {
            continue;
        }
    }//for

    return S_FALSE;
}

///----------------------------------//
//-----------------------------------//

bool c_single_pop_game_t::is_pop_connected(c_pop_t& pop)
{
    //---------------------
    //assert pop status;
    pop_status_t pop_status;
    pop.get_pop_status(&pop_status);

    assert(pop_status == pop_status_present);

    //---------------------
    //self test
    pop_pos_t self_pop_pos;
    pop.get_pop_pos(&self_pop_pos);

    if(self_pop_pos.y > m_max_row_index || self_pop_pos.y >= _MAX_POP_MAP_ROW_NUM - 1)
    {
        return true;
    }

    //---------------------
    //find path to connected pop
    m_temp_pop_set.clear();

    //---------------------
    //add self to temp_pop_set (important!!!!)
    m_temp_pop_set.insert(pop.get_pop_id());

    //---------------------
    //recrusively find path to connected pop
    int result = find_connected_pop_path_recursive(pop);

    if(result == S_OK)
    {
        return true;
    }
    else
    {
        return false;
    }
}

///----------------------------------//
//-----------------------------------//
//note: if no pop was erased before,it's no need to clear isolated pops...
//note: if level is over with success,return S_FALSE,else return S_OK or error code

int c_single_pop_game_t::clear_isolated_pops(unsigned short* p_recv_buffer,int buffer_len,int* p_data_len)
{
    //---------------------
    //check game progress
    if(m_game_progress != game_progress_going)
    {
        return E_LOGIC;
    }

    //---------------------
    //param check
    if(null == p_recv_buffer || null == p_data_len || buffer_len < 1)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //clear connected pop set and isolated pop set
    m_temp_connected_pop_set.clear();
    m_temp_isolated_pop_set.clear();

    //---------------------
    //scan all present pops between max index and min index
    for(int row = m_max_row_index; row >= m_min_row_index; row--)
    {
        if(row >= _MAX_POP_MAP_ROW_NUM)
        {
            continue;
        }

        if(row < 0)
        {
            break;
        }

        for(int col = 0; col < _MAX_POP_MAP_COL_NUM; col++)
        {
            pop_status_t pop_status;
            m_pop_map[row][col].get_pop_status(&pop_status);

            if(pop_status != pop_status_present)
            {
                continue;
            }

            pop_type_t pop_type;
            m_pop_map[row][col].get_pop_type(&pop_type);

            assert(pop_type == pop_type_normal || pop_type == pop_type_colorful);

            bool result = is_pop_connected(m_pop_map[row][col]);

            if(result)
            {
                //---------------------
                //add to connected pop set
                m_temp_connected_pop_set.insert(m_pop_map[row][col].get_pop_id());
            }
            else
            {
                //---------------------
                //add to isolated pop set
                m_temp_isolated_pop_set.insert(m_pop_map[row][col].get_pop_id());
            }
        }//for col++
    }//for row++

    //---------------------
    //clear isolated pops and fill recv buffer
    int data_len = 0;
    pop_set_iterator_t iterator = m_temp_isolated_pop_set.begin();

    for(; iterator != m_temp_isolated_pop_set.end(); ++iterator)
    {
        unsigned short pop_id = *iterator;
        int col = high_word(pop_id);
        int row = low_word(pop_id);

        //---------------------
        //determine whether is colorful pop
        pop_type_t pop_type;
        m_pop_map[row][col].get_pop_type(&pop_type);

        if(pop_type == pop_type_colorful)
        {
            //---------------------
            //colorful pop,get random props
            get_random_props();
        }

        m_pop_map[row][col].set_pop_status(pop_status_erased);

        //---------------------
        //update max row index
        m_max_row_index = max(m_max_row_index,row);

        if(data_len < buffer_len)
        {
            *(p_recv_buffer + data_len) = pop_id;
            data_len++;
        }
    }

    *p_data_len = data_len;

    //---------------------
    //update score
    if(data_len >= 6)
    {
        m_total_score += data_len * 15;
    }
    else
    {
        m_total_score += data_len * 10;
    }

    //---------------------
    //important note: there is no need to update m_max_row_index
    //update m_min_row_index
    int index = m_max_row_index - 1;

    for(; index >= 0; index--)
    {
        pop_row_status_t pop_row_status;
        get_pop_row_status(index,&pop_row_status);

        if(pop_row_status == pop_row_status_empty)
        {
            break;
        }
        else
        {
            continue;
        }
    }

    assert(index + 1 >= m_min_row_index);
    m_min_row_index = index + 1;

    assert(m_min_row_index >= 0);
    assert(m_min_row_index <= m_max_row_index);

    //---------------------
    //check whether the level is over
    if(m_max_row_index >= _MAX_POP_MAP_ROW_NUM && m_min_row_index == m_max_row_index)
    {
        m_game_progress = game_progress_level_over;
        return S_FALSE;
    }

    //---------------------
    //update bottom row index (may move upward)
    int m_min_bottom_row_index = m_min_row_index - 7;
    m_bottom_row_index = max(m_bottom_row_index,m_min_bottom_row_index);

    return S_OK;
}

///----------------------------------//
//-----------------------------------//
//note: if level is over,return S_FALSE,else return S_OK or error code
//note: must call clear_isolated_pops after call this function

int c_single_pop_game_t::erase_pops_around_bomb(c_pop_t& bomb_pop,unsigned short* p_recv_buffer,int buffer_len,int* p_data_len)
{
    //---------------------
    //check game progress
    if(m_game_progress != game_progress_going)
    {
        return E_LOGIC;
    }

    //---------------------
    //param check
    if(null == p_recv_buffer || null == p_data_len || buffer_len < 1)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //erase the bomb itself
    pop_pos_t bomb_pos;
    bomb_pop.get_pop_pos(&bomb_pos);

    m_pop_map[bomb_pos.y][bomb_pos.x].set_pop_status(pop_status_erased);

    //---------------------
    //add bomb to recv buffer
    int data_len = 0;

    *(p_recv_buffer + data_len) = bomb_pop.get_pop_id();
    data_len++;

    //---------------------
    //get all present neighbors on screen
    pop_neighbors_t all_present_neighbors;
    memset(&all_present_neighbors,0,sizeof(all_present_neighbors));

    find_present_neighbors_on_screen(bomb_pop,&all_present_neighbors);

    //---------------------
    //erase all pops around bomb
    for(int index = 0; index < all_present_neighbors.neighbors_num; index++)
    {
        int col = high_word(all_present_neighbors.neighbors_array[index]);
        int row = low_word(all_present_neighbors.neighbors_array[index]);

        //---------------------
        //determine whether is colorful pop
        pop_type_t pop_type;
        m_pop_map[row][col].get_pop_type(&pop_type);

        if(pop_type == pop_type_colorful)
        {
            //---------------------
            //colorful pop,get random props
            get_random_props();
        }

        m_pop_map[row][col].set_pop_status(pop_status_erased);

        //---------------------
        //update max row index,max row can only be larger
        m_max_row_index = max(m_max_row_index,row);

        //---------------------
        //add to recv buffer
        if(data_len < buffer_len)
        {
            *(p_recv_buffer + data_len) = all_present_neighbors.neighbors_array[index];
            data_len++;
        }
    }

    *p_data_len = data_len;

    //---------------------
    //determin whether max row is empty
    pop_row_status_t pop_row_status;
    get_pop_row_status(m_max_row_index,&pop_row_status);

    assert(pop_row_status != pop_row_status_full);

    if(pop_row_status == pop_row_status_empty)
    {
        m_max_row_index++;

        if(m_max_row_index >= _MAX_POP_MAP_ROW_NUM && (m_max_row_index - 1 == m_min_row_index))
        {
            //---------------------
            //level is over,and success
            m_game_progress = game_progress_level_over;

            return S_FALSE;
        }
    }

    /*

    //---------------------
    //update bottom row index
    m_bottom_row_index -= 2;

    if(m_bottom_row_index < 0)
    {
        m_bottom_row_index = 0;
    }

    */

    return S_OK;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::erase_pops_around_bomb_enlarge(c_pop_t& bomb_pop,unsigned short* p_recv_buffer,int buffer_len,int* p_data_len)
{
    //---------------------
    //check game progress
    if(m_game_progress != game_progress_going)
    {
        return E_LOGIC;
    }

    //---------------------
    //param check
    if(null == p_recv_buffer || null == p_data_len || buffer_len < 1)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //erase the bomb itself
    pop_pos_t bomb_pos;
    bomb_pop.get_pop_pos(&bomb_pos);

    m_pop_map[bomb_pos.y][bomb_pos.x].set_pop_status(pop_status_erased);

    //---------------------
    //add bomb to recv buffer
    int data_len = 0;

    *(p_recv_buffer + data_len) = bomb_pop.get_pop_id();
    data_len++;

    //---------------------
    //get all neighbors around (pops around bomb may not be present) (neighbor's neighbors)(enlarge version)
    m_temp_pop_set.clear();

    pop_neighbors_t bomb_neighbors;
    memset(&bomb_neighbors,0,sizeof(bomb_neighbors));

    int result = find_neighbors_on_screen(bomb_pop,&bomb_neighbors);
    assert(result == S_OK);

    for(int index = 0; index < bomb_neighbors.neighbors_num; index++)
    {
        int col = high_word(bomb_neighbors.neighbors_array[index]);
        int row = low_word(bomb_neighbors.neighbors_array[index]);

        //---------------------
        //determine whether pop is present
        pop_status_t pop_status;
        m_pop_map[row][col].get_pop_status(&pop_status);

        if(pop_status == pop_status_present)
        {
            //---------------------
            //add pop to the temp pop set
            m_temp_pop_set.insert(m_pop_map[row][col].get_pop_id());
        }

        //---------------------
        //get neighbors of neighbor pop
        pop_neighbors_t temp_neighbors;
        memset(&temp_neighbors,0,sizeof(temp_neighbors));

        result = find_present_neighbors_on_screen(m_pop_map[row][col],&temp_neighbors);
        assert(result == S_OK);

        for(int inner_index = 0; inner_index < temp_neighbors.neighbors_num; inner_index++)
        {
            //---------------------
            //add pop to temp pop set
            m_temp_pop_set.insert(temp_neighbors.neighbors_array[inner_index]);
        }
    }

    //---------------------
    //erase all pops around bomb (enlarge version)
    pop_set_iterator_t iterator = m_temp_pop_set.begin();

    for(; iterator != m_temp_pop_set.end(); ++iterator)
    {
        unsigned short pop_id = *iterator;
        int col = high_word(pop_id);
        int row = low_word(pop_id);

        //---------------------
        //determine whether is colorful pop
        pop_type_t pop_type;
        m_pop_map[row][col].get_pop_type(&pop_type);

        if(pop_type == pop_type_colorful)
        {
            //---------------------
            //colorful pop,get random props
            get_random_props();
        }

        m_pop_map[row][col].set_pop_status(pop_status_erased);

        //---------------------
        //update max row index,max row can only be larger
        m_max_row_index = max(m_max_row_index,row);

        //---------------------
        //add to recv buffer
        if(data_len < buffer_len)
        {
            *(p_recv_buffer + data_len) = pop_id;
            data_len++;
        }
    }

    *p_data_len = data_len;

    //---------------------
    //determin whether max row is empty
    pop_row_status_t pop_row_status;
    get_pop_row_status(m_max_row_index,&pop_row_status);

    assert(pop_row_status != pop_row_status_full);

    if(pop_row_status == pop_row_status_empty)
    {
        m_max_row_index++;

        if(m_max_row_index >= _MAX_POP_MAP_ROW_NUM && (m_max_row_index - 1 == m_min_row_index))
        {
            //---------------------
            //level is over,and success
            m_game_progress = game_progress_level_over;

            return S_FALSE;
        }
    }

    /*

    //---------------------
    //update bottom row index
    m_bottom_row_index -= 2;

    if(m_bottom_row_index < 0)
    {
        m_bottom_row_index = 0;
    }

    */

    return S_OK;
}

///----------------------------------//
//-----------------------------------//
//note: if game is over(fail),return S_FALSE,else return S_OK or error code

int c_single_pop_game_t::move_downward()
{
    //---------------------
    //check game progress
    if(m_game_progress != game_progress_going)
    {
        return E_LOGIC;
    }

    //---------------------
    //move downward
    m_bottom_row_index++;

    if(m_min_row_index < m_bottom_row_index)
    {
        //---------------------
        //game is over and fail
        m_game_progress = game_progress_over_fail;

        return S_FALSE;
    }

    return S_OK;
}

///----------------------------------//
//-----------------------------------//
//return time interval to move down in seconds

int c_single_pop_game_t::get_move_interval_time()
{
    if(m_current_level == 1)
    {
        return 25;
    }
    else if(m_current_level == 2)
    {
        return 20;
    }
    else if(m_current_level == 3)
    {
        return 10;
    }
    else if(m_current_level == 4)
    {
        return 8;
    }
    else if(m_current_level == 5)
    {
        return 5;
    }
    else
    {
        return 3;
    }
}

///----------------------------------//
//-----------------------------------//
//return S_OK if game is all over,else E_FAIL

int c_single_pop_game_t::is_game_all_over()
{
    if(m_current_level >= _MAX_SINGLE_GAME_LEVEL_NUM && m_game_progress == game_progress_level_over)
    {
        m_game_progress = game_progress_over_success;
        return S_OK;
    }
    else if(m_game_progress == game_progress_over_success)
    {
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::set_user_data(long user_data)
{
    m_user_data = user_data;
    return 0;
}

///----------------------------------//
//-----------------------------------//

long c_single_pop_game_t::get_user_data()
{
    return m_user_data;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::get_raw_pop_map(unsigned char* p_recv_buffer,int buffer_len,int* p_data_len)
{
    //---------------------
    //param check
    if(null == p_recv_buffer || null == p_data_len || buffer_len < 1)
    {
        return E_INVALIDARG;
    }

    if(buffer_len < (int)sizeof(m_pop_map) + 1)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //get raw map data
    memcpy(p_recv_buffer,m_pop_map,sizeof(m_pop_map));
    *p_data_len = sizeof(m_pop_map);

    return S_OK;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::set_raw_pop_map(unsigned char* p_map_data,int data_len)
{
    //---------------------
    //param check
    if(null == p_map_data || data_len != sizeof(m_pop_map))
    {
        return E_INVALIDARG;
    }

    //---------------------
    //set map data
    memcpy(m_pop_map,p_map_data,data_len);

    return S_OK;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::add_pop(int add_pop_num,added_pop_info_t* p_recv_buffer,int buffer_len,int* p_data_len)
{
    //---------------------
    //check game progress
    if(m_game_progress != game_progress_going)
    {
        return E_LOGIC;
    }

    //---------------------
    //param check
    if(null == p_recv_buffer || null == p_data_len || buffer_len < 1)
    {
        return E_INVALIDARG;
    }

    if(add_pop_num <= 0)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //add pop from m_max_row_index
    int pop_num_added = 0;

    for(int row = m_max_row_index; row >= m_bottom_row_index; row--)
    {
        for(int col = 0; col < _MAX_POP_MAP_COL_NUM - 1; col++)
        {
            if(pop_num_added >= add_pop_num || pop_num_added >= buffer_len)
            {
                break;
            }

            pop_status_t pop_status;
            m_pop_map[row][col].get_pop_status(&pop_status);

            if(pop_status != pop_status_erased)
            {
                continue;
            }

            //---------------------
            //get random color
            pop_color_t pop_color;
            c_pop_t::generate_random_color(&pop_color);

            m_pop_map[row][col].set_pop_status(pop_status_present);
            m_pop_map[row][col].set_pop_type(pop_type_normal);
            m_pop_map[row][col].set_pop_color(pop_color);

            //---------------------
            //update m_min_row_index
            m_min_row_index = min(m_min_row_index,row);

            //---------------------
            //add to recv buffer
            (p_recv_buffer + pop_num_added)->col = col;
            (p_recv_buffer + pop_num_added)->row = row;
            (p_recv_buffer + pop_num_added)->pop_color = pop_color;

            pop_num_added++;

        }//for col++
    }//for row--

    //---------------------
    //update m_max_row_index
    int index = _MAX_POP_MAP_ROW_NUM - 1;

    for(; index >= m_min_row_index; index--)
    {
        pop_row_status_t pop_row_status;
        get_pop_row_status(index,&pop_row_status);

        if(pop_row_status == pop_row_status_full)
        {
            if(index > m_min_row_index)
            {
                continue;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    m_max_row_index = index;
    assert(m_max_row_index >= m_min_row_index);

    //---------------------
    //return
    *p_data_len = pop_num_added;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//

time_t c_single_pop_game_t::get_game_start_timestamp()
{
    return m_game_start_timestamp;
}

///----------------------------------//
//-----------------------------------//

#ifndef _MULTI_PLAYER_VERSION

int c_single_pop_game_t::get_pop_game_bonus(pop_game_bonus_t* p_bonus,bool is_win)
{
    //---------------------
    //param check
    if(null == p_bonus)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //get current game progress
    if(m_game_progress == game_progress_level_over || m_game_progress == game_progress_over_success)
    {
        p_bonus->mole_coin = (int)round(_MAX_SINGLE_GAME_MOLE_COIN_NUM / (float)_MAX_SINGLE_GAME_LEVEL_NUM) * m_current_level;
        p_bonus->exp = (int)round(_MAX_SINGLE_GAME_EXP_NUM / (float)_MAX_SINGLE_GAME_LEVEL_NUM) * m_current_level;
        p_bonus->strength = (int)round(_MAX_SINGLE_GAME_STRENGTH_NUM / (float)_MAX_SINGLE_GAME_LEVEL_NUM) * m_current_level;
    }
    else
    {
        p_bonus->mole_coin = (int)round(_MAX_SINGLE_GAME_MOLE_COIN_NUM / (float)_MAX_SINGLE_GAME_LEVEL_NUM) * (m_current_level - 1);
        p_bonus->exp = (int)round(_MAX_SINGLE_GAME_EXP_NUM / (float)_MAX_SINGLE_GAME_LEVEL_NUM) * (m_current_level - 1);
        p_bonus->strength = (int)round(_MAX_SINGLE_GAME_STRENGTH_NUM / (float)_MAX_SINGLE_GAME_LEVEL_NUM) * (m_current_level - 1);
    }

    return S_OK;
}

#else

int c_single_pop_game_t::get_pop_game_bonus(pop_game_bonus_t* p_bonus,bool is_win)
{
    //---------------------
    //param check
    if(null == p_bonus)
    {
        return E_INVALIDARG;
    }

    //---------------------
    //check time
    if(time(null) - m_game_start_timestamp < _MIN_MULTI_GAME_TIME)
    {
        //---------------------
        //unable to get bonus
        p_bonus->mole_coin = 0;
        p_bonus->exp = 0;
        p_bonus->strength = 0;

        return S_OK;
    }

    //---------------------
    //get bonus
    if(!is_win)
    {
        p_bonus->mole_coin = _MULTI_GAME_LOSE_MOLE_COIN_NUM;
        p_bonus->exp = _MULTI_GAME_LOSE_EXP_NUM;
        p_bonus->strength = _MULTI_GAME_LOSE_STRENGTH_NUM;
    }
    else
    {
        p_bonus->mole_coin = _MULTI_GAME_WIN_MOLE_COIN_NUM;
        p_bonus->exp = _MULTI_GAME_WIN_EXP_NUM;
        p_bonus->strength = _MULTI_GAME_WIN_STRENGTH_NUM;
    }

    return S_OK;
}

#endif//_MULTI_PLAYER_VERSION

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::get_shoot_pop_array(c_pop_t* p_recv_buffer,int buffer_len)
{
    //---------------------
    //param check
    if(null == p_recv_buffer || buffer_len < 5)
    {
        return E_INVALIDARG;
    }

    *(p_recv_buffer + 0) = m_current_shoot_pop;
    *(p_recv_buffer + 1) = m_current_bakup_pop;
    *(p_recv_buffer + 2) = m_third_pop;
    *(p_recv_buffer + 3) = m_forth_pop;
    *(p_recv_buffer + 4) = m_fifth_pop;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//

int c_single_pop_game_t::request_new_shoot_pop(c_pop_t* p_new_shoot_pop)
{
    //---------------------
    //check game progress
    if(m_game_progress != game_progress_going)
    {
        return E_LOGIC;
    }

    //---------------------
    //generate new shoot pop
    c_pop_t new_shoot_pop;
    new_shoot_pop.reset_pop_data();

    new_shoot_pop.set_pop_status(pop_status_for_shoot);
    new_shoot_pop.set_pop_type(pop_type_normal);

    pop_color_t new_shoot_pop_color;
    c_pop_t::generate_random_color(&new_shoot_pop_color);

    new_shoot_pop.set_pop_color(new_shoot_pop_color);

    //---------------------
    //new shoot pop color add to tail of shoot pop array
    m_current_shoot_pop = m_current_bakup_pop;
    m_current_bakup_pop = m_third_pop;
    m_third_pop = m_forth_pop;
    m_forth_pop = m_fifth_pop;
    m_fifth_pop = new_shoot_pop;

    if(p_new_shoot_pop != null)
    {
        *p_new_shoot_pop = new_shoot_pop;
    }

    return S_OK;
}

///----------------------------------//
//-----------------------------------//
