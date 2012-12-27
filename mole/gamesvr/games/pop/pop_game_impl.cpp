//-----------------------------------//
//-----------------------------------//

#include "pop_game_impl.h"
#include "../../ant/inet/pdumanip.hpp"
#include "my_errno.h"

//-----------------------------------//
//-----------------------------------//

#define null 0

#define GENERATE_COLORFUL_POP_TIMER_INTERVAL  30
#define CLEAR_COLORFUL_POP_TIMER_INTERVAL      2
#define COLORFUL_POP_TTL                      10

#define MULTI_GAME_MOVEDOWN_TIMER_INTERVAL    15

#define CMD_REQUEST_NEW_LEVEL           31101
#define CMD_NEW_LEVEL_INFO              31102
#define CMD_SHOOT_POP_UPDATE            31103
#define CMD_PROPS_NUM_UPDATE            31104
#define CMD_SCORE_UPDATE                31105
#define CMD_SHOOT_POS                   31106
#define CMD_ERASE_POPS                  31107
#define CMD_CLEAR_POPS                  31108
#define CMD_GENERATE_COLORFUL_POP       31109
#define CMD_CLEAR_COLORFUL_POP          31110
#define CMD_BOTTOM_ROW_UPDATE           31111
#define CMD_REQUEST_USE_PROP            31112
#define CMD_USE_PROP_ACK                31113
#define CMD_LEVEL_OVER                  31114
#define CMD_GAME_OVER_FAIL              31115
#define CMD_GAME_OVER_SUCCESS           31116
#define CMD_USER_EXIT                   31117
#define CMD_GAME_BONUS                  31118

#define CMD_OTHER_GAME_OVER_FAIL        31130
#define CMD_OTHER_GAME_OVER_SUCCESS     31131
#define CMD_USER_SCREEN_STATUS          31132
#define CMD_OTHER_USER_SCREEN_STATUS    31133
#define CMD_OTHER_USE_SPIDER            31134
#define CMD_ADD_POP_NOTIFY              31135
#define CMD_REQUEST_ADD_POP             31136
#define CMD_POP_ADDED                   31137

#define CMD_SHOOT_POP_ARRAY             31138
#define CMD_TAIL_SHOOT_POP_UPDATE       31139

//-----------------------------------//
//-----------------------------------//

extern "C" void* create_game(struct game_group* p_game_group)
{
    DEBUG_LOG("create_game: gamegroup: groupid:%d,count:%d,game:%lu,gamehandler:%lu",(int)p_game_group->id,(int)p_game_group->count,(long)p_game_group->game,(long)p_game_group->game_handler);

    if(p_game_group->count == 1)
    {
        c_pop_game_impl_t* p_pop_game =  new (std::nothrow) c_pop_game_impl_t();

        if(p_pop_game == null)
        {
            return null;
        }
        else
        {
            int result = p_pop_game->init_pop_game();

            if(result == S_OK)
            {
                return p_pop_game;
            }
            else
            {
                delete p_pop_game;
                return null;
            }
        }
    }
    else if(p_game_group->count == 2)
    {
        c_multi_pop_game_impl_t* p_multi_pop_game = new (std::nothrow) c_multi_pop_game_impl_t();

        if(p_multi_pop_game == null)
        {
            return null;
        }
        else
        {
            int result = p_multi_pop_game->init_multi_pop_game();

            if(result == S_OK)
            {
                return p_multi_pop_game;
            }
            else
            {
                delete p_multi_pop_game;
                return null;
            }
        }
    }
    else
    {
        return null;
    }
}

//-----------------------------------//
//-----------------------------------//

extern "C" int game_init()
{
    return 0;
}

//-----------------------------------//
//-----------------------------------//

extern "C" void game_destroy()
{
}

//-----------------------------------//
//-----------------------------------//

//**********************************************************************************//
//**********************************************************************************//
//-----  c_pop_game_impl_t  ------//
//**********************************************************************************//
//**********************************************************************************//

//-----------------------------------//
//-----------------------------------//

c_pop_game_impl_t::c_pop_game_impl_t()
{
    m_p_single_pop_game = null;
}

//-----------------------------------//
//-----------------------------------//

c_pop_game_impl_t::~c_pop_game_impl_t()
{
    if(m_p_single_pop_game)
    {
        delete m_p_single_pop_game;
        m_p_single_pop_game = null;
    }
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::init_pop_game()
{
    m_p_single_pop_game = new (std::nothrow) c_single_pop_game_t();

    if(m_p_single_pop_game != null)
    {
        return S_OK;
    }
    else
    {
        return E_OUTOFMEMORY;
    }
}

//-----------------------------------//
//-----------------------------------//

void c_pop_game_impl_t::init(sprite_t* p_sprite)
{
    assert(m_p_single_pop_game != null);
    m_p_single_pop_game->init_single_pop_game();
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::handle_data(sprite_t* p_sprite, int cmd, const uint8_t body[], int data_len)
{
    assert(m_p_single_pop_game != null);
    return handle_single_pop_game_data(p_sprite,cmd,body,data_len);
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::handle_single_pop_game_data(sprite_t* p_sprite,int cmd,const uint8_t data[], int data_len)
{
    DEBUG_LOG("RESPONSE: handle_single_pop_game_data");

    assert(m_p_single_pop_game != null);

    if(cmd == CMD_REQUEST_NEW_LEVEL)
    {
        DEBUG_LOG("RECV PACKAGE: CMD_REQUEST_NEW_LEVEL");

        if(data_len != 1)
        {
            return 0;
        }

        //---------------------
        //request new level,get current level first
        int current_level = *data;

        if(current_level == 0)
        {
            //---------------------
            //new game (first run)
            int result = m_p_single_pop_game->start_game();

            if(result != S_OK)
            {
                response_single_game_error(p_sprite,CMD_REQUEST_NEW_LEVEL,result);
                return 1;
            }

            //---------------------
            //return level info
            response_single_game_level_info(p_sprite);

            //---------------------
            //return shoot pop array
            response_single_game_shoot_pop_array(p_sprite);

            //---------------------
            //return current props num
            response_single_game_props_num_shoot(p_sprite);
            response_single_game_props_num_bomb(p_sprite);

            //---------------------
            //return current score
            response_single_game_score(p_sprite);

            //---------------------
            //setup movedown timer
            setup_single_game_movedown_timer(p_sprite);

            //---------------------
            //setup colorful pop timer
            setup_single_game_colorful_pop_timer(p_sprite);

            return 0;
        }
        else
        {
            //---------------------
            //goto next level
            int result = m_p_single_pop_game->goto_next_level();

            if(result == S_FALSE)
            {
                //---------------------
                //game is over and success!!!
                response_single_game_over_success(p_sprite);
                return 1;
            }
            else if(result != S_OK)
            {
                response_single_game_error(p_sprite,CMD_REQUEST_NEW_LEVEL,result);
                return 1;
            }

            //---------------------
            //return level info
            response_single_game_level_info(p_sprite);

            //---------------------
            //return shoot pop array
            response_single_game_shoot_pop_array(p_sprite);

            //---------------------
            //return current props num
            response_single_game_props_num_shoot(p_sprite);
            response_single_game_props_num_bomb(p_sprite);

            //---------------------
            //return current score
            response_single_game_score(p_sprite);

            //---------------------
            //setup movedown timer
            setup_single_game_movedown_timer(p_sprite);

            //---------------------
            //setup colorful pop timer
            setup_single_game_colorful_pop_timer(p_sprite);

            return 0;
        }//else
    }
    else if(cmd == CMD_SHOOT_POS)
    {
        DEBUG_LOG("RECV PACKAGE: CMD_SHOOT_POS");

        if(data_len != 3)
        {
            return 0;
        }

        //---------------------
        //get x and y of the shoot pos
        int col = *data;
        int row = *(data + 1);
        int seq_num = *(data + 2);

        //---------------------
        //add shooted pop
        c_pop_t shooted_pop;
        int result = m_p_single_pop_game->add_shooted_pop(row,col,&shooted_pop);

        if(result == S_FALSE)
        {
            //---------------------
            //game over
            response_single_game_over_fail(p_sprite);
            return 1;
        }
        else if(result == E_FAIL)
        {
            //---------------------
            //invalid pos
            response_single_game_over_fail(p_sprite);
            return 1;
        }
        else if(result != S_OK)
        {
            //---------------------
            //other error
            //response_single_game_over_fail(p_sprite);
            return 0;
        }

        //---------------------
        //request new shoot pop
        c_pop_t new_shoot_pop;
        m_p_single_pop_game->request_new_shoot_pop(&new_shoot_pop);

        response_single_game_tail_shoot_pop_update(p_sprite,new_shoot_pop);

        //---------------------
        //erase pops
        unsigned short pops_array[256];
        int pops_num = 0;

        //---------------------
        //determine whether current shoot pop is bomb or normal pop
        pop_type_t shooted_pop_type;
        shooted_pop.get_pop_type(&shooted_pop_type);

        if(shooted_pop_type == pop_type_normal)
        {
            result = m_p_single_pop_game->erase_same_color_pop_group(shooted_pop,pops_array,sizeof(pops_array)/sizeof(unsigned short),&pops_num);

            //---------------------
            //response erased pops
            response_single_game_pops_erased(p_sprite,seq_num,pops_array,pops_num);

            if(result == S_FALSE)
            {
                //---------------------
                //game may over or level is over
                single_pop_game_progress_t game_progress;
                m_p_single_pop_game->get_game_progress(&game_progress);

                assert(game_progress == game_progress_over_fail || game_progress == game_progress_level_over);

                if(game_progress == game_progress_over_fail)
                {
                    response_single_game_over_fail(p_sprite);
                    return 1;
                }
                else
                {
                    response_single_game_level_over(p_sprite);
                    return 0;
                }
            }

            assert(result == S_OK);
        }//if shooted_pop_type == pop_type_normal
        else
        {
            result = m_p_single_pop_game->erase_pops_around_bomb_enlarge(shooted_pop,pops_array,sizeof(pops_array)/sizeof(unsigned short),&pops_num);

            //---------------------
            //response erased pops
            response_single_game_pops_erased(p_sprite,seq_num,pops_array,pops_num);

            if(result == S_FALSE)
            {
                //---------------------
                //level is over
                response_single_game_level_over(p_sprite);
                return 0;
            }

            assert(result == S_OK);
        }

        //---------------------
        //clear isolated pops
        if(pops_num > 0)
        {
            pops_num = 0;

            result = m_p_single_pop_game->clear_isolated_pops(pops_array,sizeof(pops_array)/sizeof(unsigned short),&pops_num);

            //---------------------
            //response cleared pops
            response_single_game_pops_cleared(p_sprite,seq_num,pops_array,pops_num);

            if(result == S_FALSE)
            {
                //---------------------
                //level is over
                response_single_game_level_over(p_sprite);
                return 0;
            }
        }//if pops_num > 0

        //---------------------
        //response scroe
        response_single_game_score(p_sprite);

        //---------------------
        //update bottom row index
        response_single_game_bottom_row_index(p_sprite);

        //---------------------
        //update props num
        response_single_game_props_num_shoot(p_sprite);
        response_single_game_props_num_bomb(p_sprite);

        return 0;

    }//else if cmd ==
    else if(cmd == CMD_REQUEST_USE_PROP)
    {
        DEBUG_LOG("RECV PACKAGE: CMD_REQUEST_USE_PROP");

        if(data_len != 2)
        {
            return 0;
        }

        int pop_type = *data;
        int seq_num = *(data + 1);

        int allow_to_use = 0;

        if(pop_type != 1 && pop_type != 2 && pop_type != 3)
        {
            return 0;
        }

        //---------------------
        //attemp to use pop
        if(pop_type == 1)
        {
            //---------------------
            //attemp to use props shoot
            int result = m_p_single_pop_game->use_props_shoot();

            if(result == S_OK)
            {
                allow_to_use = 1;
            }

            //---------------------
            //response to user
            response_single_game_request_use_props_ack(p_sprite,pop_type,allow_to_use,seq_num);
        }
        else if(pop_type == 2)
        {
            //---------------------
            //attemp to use props bomb
            int result = m_p_single_pop_game->use_props_bomb();

            if(result == S_OK)
            {
                allow_to_use = 1;
            }

            //---------------------
            //response to user
            response_single_game_request_use_props_ack(p_sprite,pop_type,allow_to_use,seq_num);
        }
        else
        {
        }

        //---------------------
        //update props num
        response_single_game_props_num_shoot(p_sprite);
        response_single_game_props_num_bomb(p_sprite);

    }//else if cmd =
    else if(cmd == proto_player_leave)
    {
        //---------------------
        //determine game progress
        single_pop_game_progress_t game_progress;
        m_p_single_pop_game->get_game_progress(&game_progress);

        if(game_progress == game_progress_going || game_progress == game_progress_level_over)
        {
            //---------------------
            //user abort game,submit game bonus
            response_single_game_bonus(p_sprite,false);
        }

        return 1;
    }
    else
    {
    }

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::setup_single_game_movedown_timer(sprite* p_sprite)
{
    DEBUG_LOG("TIMER: setup_single_game_movedown_timer,sprite = %p",p_sprite);

    assert(m_p_single_pop_game != null);

    //---------------------
    //get expire time
    int expire_time = time(null) + m_p_single_pop_game->get_move_interval_time();

    //---------------------
    //add move down timer
    ADD_TIMER_EVENT(p_sprite,deal_single_game_movedown_timer,this,expire_time);

    return 0;
}

//-----------------------------------//
//-----------------------------------//
//note: return 0,the timer will be removed

int c_pop_game_impl_t::deal_single_game_movedown_timer(void* p_data1/*p_sprite*/,void* p_data2/*p_pop_game*/)
{
    DEBUG_LOG("TIMER: deal_single_game_movedown_timer");

    sprite* p_sprite = (sprite*)p_data1;
    c_pop_game_impl_t* p_pop_game = (c_pop_game_impl_t*)p_data2;

    assert(p_sprite != null);
    assert(p_pop_game != null);
    assert(p_pop_game->m_p_single_pop_game != null);

    //---------------------
    //determine current game progress
    single_pop_game_progress_t game_progress;
    p_pop_game->m_p_single_pop_game->get_game_progress(&game_progress);

    if(game_progress != game_progress_going)
    {
        return 0;
    }

    //---------------------
    //move down bottom row index
    int result = p_pop_game->m_p_single_pop_game->move_downward();

    if(result == S_FALSE)
    {
        //---------------------
        //game is over
        p_pop_game->response_single_game_over_fail(p_sprite);

        return 1;
    }
    else
    {
        //---------------------
        //go on,get current expire time,and add timer
        int expire_time = time(null) + p_pop_game->m_p_single_pop_game->get_move_interval_time();

        ADD_TIMER_EVENT(p_sprite,deal_single_game_movedown_timer,p_pop_game,expire_time);

        //---------------------
        //response move down to user
        p_pop_game->response_single_game_bottom_row_index(p_sprite);

        return 0;
    }
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::setup_single_game_colorful_pop_timer(sprite* p_sprite)
{
    DEBUG_LOG("TIMER: setup_single_game_colorful_pop_timer");

    assert(m_p_single_pop_game != null);

    //---------------------
    //generate colorful pop timer
    int expire_time = time(null) + GENERATE_COLORFUL_POP_TIMER_INTERVAL;
    ADD_TIMER_EVENT(p_sprite,deal_single_game_generate_colorful_pop_timer,this,expire_time);

    //---------------------
    //clear colorful pop timer
    expire_time = time(null) + CLEAR_COLORFUL_POP_TIMER_INTERVAL;
    ADD_TIMER_EVENT(p_sprite,deal_single_game_clear_colorful_pop_timer,this,expire_time);

    return 0;
}

int c_pop_game_impl_t::deal_single_game_generate_colorful_pop_timer(void* p_data1,void* p_data2)
{
    DEBUG_LOG("TIMER: deal_single_game_generate_colorful_pop_timer,p_data1:%p",p_data1);

    sprite* p_sprite = (sprite*)p_data1;
    c_pop_game_impl_t* p_pop_game = (c_pop_game_impl_t*)p_data2;

    assert(p_sprite != null);
    assert(p_pop_game != null);
    assert(p_pop_game->m_p_single_pop_game != null);

    //---------------------
    //try to generate colorful pop
    c_pop_t colorful_pop;
    int result = p_pop_game->m_p_single_pop_game->generate_colorful_pop(COLORFUL_POP_TTL,&colorful_pop);

    if(result == S_OK)
    {
        //---------------------
        //successful generate a colorful pop
        int col = high_word(colorful_pop.get_pop_id());
        int row = low_word(colorful_pop.get_pop_id());

        p_pop_game->response_single_game_generate_colorful_pop(p_sprite,col,row);
    }

    //---------------------
    //continue to work
    int expire_time = time(null) + GENERATE_COLORFUL_POP_TIMER_INTERVAL;
    ADD_TIMER_EVENT(p_sprite,deal_single_game_generate_colorful_pop_timer,p_pop_game,expire_time);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::deal_single_game_clear_colorful_pop_timer(void* p_data1,void* p_data2)
{
    DEBUG_LOG("TIMER: deal_single_game_clear_colorful_pop_timer,p_data1:%p",p_data1);

    sprite* p_sprite = (sprite*)p_data1;
    c_pop_game_impl_t* p_pop_game = (c_pop_game_impl_t*)p_data2;

    assert(p_sprite != null);
    assert(p_pop_game != null);
    assert(p_pop_game->m_p_single_pop_game != null);

    //---------------------
    //try to clear colorful pop
    c_pop_t colorful_pop;
    int result = p_pop_game->m_p_single_pop_game->clear_colorful_pop(&colorful_pop);

    if(result == S_OK)
    {
        //---------------------
        //success clear a colorful pop
        int col = high_word(colorful_pop.get_pop_id());
        int row = low_word(colorful_pop.get_pop_id());

        p_pop_game->response_single_game_clear_colorful_pop(p_sprite,col,row);
    }

    //---------------------
    //continue to work
    int expire_time = time(null) + CLEAR_COLORFUL_POP_TIMER_INTERVAL;
    ADD_TIMER_EVENT(p_sprite,deal_single_game_clear_colorful_pop_timer,p_pop_game,expire_time);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::del_single_game_all_timer(sprite* p_sprite)
{
    DEBUG_LOG("TIMER: del_single_game_all_timer");

    //---------------------
    //remove all timer
    REMOVE_TIMERS(p_sprite);

    return 1;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_over_fail(sprite_t* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_single_game_over_fail");

    assert(m_p_single_pop_game != null);

    //---------------------
    //response game bonus first
    response_single_game_bonus(p_sprite,false);

    single_pop_game_progress_t game_progress;
    m_p_single_pop_game->get_game_progress(&game_progress);
    assert(game_progress == game_progress_over_fail);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)0,pkg_index);

    init_proto_head(pkg,CMD_GAME_OVER_FAIL,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,0);

    //---------------------
    //remove all timers
    //del_single_game_all_timer(p_sprite);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_over_success(sprite_t* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_single_game_over_success");

    assert(m_p_single_pop_game != null);

    //---------------------
    //response game bonus first
    response_single_game_bonus(p_sprite,true);

    single_pop_game_progress_t game_progress;
    m_p_single_pop_game->get_game_progress(&game_progress);
    assert(game_progress == game_progress_over_success || game_progress == game_progress_level_over);

    int pkg_index = sizeof(protocol_t);

    init_proto_head(pkg,CMD_GAME_OVER_SUCCESS,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,0);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_level_info(sprite_t* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_single_game_level_info");

    assert(m_p_single_pop_game != null);

    int current_level = m_p_single_pop_game->get_current_level();
    int pops_num = (_MAX_POP_MAP_ROW_NUM - _INIT_START_ROW_INDEX) * _MAX_POP_MAP_COL_NUM;

    unsigned char map_info[1024 * 2];
    int map_info_data_len = 0;
    int result = m_p_single_pop_game->get_map_info(map_info,sizeof(map_info),&map_info_data_len);

    if(result != S_OK)
    {
        return result;
    }

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)current_level,pkg_index);
    ant::pack(pkg,(uint16_t)pops_num,pkg_index);

    memcpy(pkg + pkg_index,map_info,map_info_data_len);
    pkg_index += map_info_data_len;

    init_proto_head(pkg,CMD_NEW_LEVEL_INFO,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_current_pops_update(sprite_t* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_single_game_current_pops_update");

    assert(m_p_single_pop_game != null);

    //---------------------
    //send current shoot pop
    c_pop_t current_shoot_pop;
    m_p_single_pop_game->get_current_shoot_pop(&current_shoot_pop);

    pop_type_t current_shoot_pop_type;
    current_shoot_pop.get_pop_type(&current_shoot_pop_type);

    int pop_type = -1;
    if(current_shoot_pop_type == pop_type_normal)
    {
        pop_type = 1;
    }
    else
    {
        pop_type = 0;
    }

    pop_color_t current_shoot_pop_color;
    current_shoot_pop.get_pop_color(&current_shoot_pop_color);

    int pop_color = (int)current_shoot_pop_color;

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)1,pkg_index);
    ant::pack(pkg,(uint8_t)pop_type,pkg_index);
    ant::pack(pkg,(uint8_t)pop_color,pkg_index);

    init_proto_head(pkg,CMD_SHOOT_POP_UPDATE,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    //---------------------
    //send current bakup pop
    c_pop_t current_bakup_pop;
    m_p_single_pop_game->get_current_bakup_pop(&current_bakup_pop);

    pop_type_t current_bakup_pop_type;
    current_bakup_pop.get_pop_type(&current_bakup_pop_type);

    if(current_bakup_pop_type == pop_type_normal)
    {
        pop_type = 1;
    }
    else
    {
        pop_type = 0;
    }

    pop_color_t current_bakup_pop_color;
    current_bakup_pop.get_pop_color(&current_bakup_pop_color);

    pop_color = (int)current_bakup_pop_color;

    pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)0,pkg_index);
    ant::pack(pkg,(uint8_t)pop_type,pkg_index);
    ant::pack(pkg,(uint8_t)pop_color,pkg_index);

    init_proto_head(pkg,CMD_SHOOT_POP_UPDATE,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_props_num_shoot(sprite* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_single_game_props_num_shoot");

    assert(m_p_single_pop_game != null);

    int props_shoot_num = m_p_single_pop_game->get_props_shoot_num();

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)1,pkg_index);
    ant::pack(pkg,(uint8_t)props_shoot_num,pkg_index);

    init_proto_head(pkg,CMD_PROPS_NUM_UPDATE,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_props_num_bomb(sprite* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_single_game_props_num_bomb");

    assert(m_p_single_pop_game != null);

    int props_bomb_num = m_p_single_pop_game->get_props_bomb_num();

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)2,pkg_index);
    ant::pack(pkg,(uint8_t)props_bomb_num,pkg_index);

    init_proto_head(pkg,CMD_PROPS_NUM_UPDATE,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_score(sprite* p_sprite)
{
    assert(m_p_single_pop_game != null);

    int game_score = m_p_single_pop_game->get_total_score();

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint32_t)game_score,pkg_index);

    init_proto_head(pkg,CMD_SCORE_UPDATE,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_pops_erased(sprite* p_sprite,int seq_num,unsigned short* pop_array,int pop_num)
{
    DEBUG_LOG("RESPONSE: response_single_game_pops_erased");

    assert(m_p_single_pop_game != null);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)seq_num,pkg_index);
    ant::pack(pkg,(uint8_t)pop_num,pkg_index);

    for(int index = 0; index < pop_num; index++)
    {
       int col = high_word( *(pop_array + index) );
       int row = low_word( *(pop_array + index) );

       ant::pack(pkg,(uint8_t)col,pkg_index);
       ant::pack(pkg,(uint8_t)row,pkg_index);
    }

    init_proto_head(pkg,CMD_ERASE_POPS,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_pops_cleared(sprite* p_sprite,int seq_num,unsigned short* pop_array,int pop_num)
{
    DEBUG_LOG("RESPONSE: response_single_game_pops_cleared");

    assert(m_p_single_pop_game != null);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)seq_num,pkg_index);
    ant::pack(pkg,(uint8_t)pop_num,pkg_index);

    for(int index = 0; index < pop_num; index++)
    {
       int col = high_word( *(pop_array + index) );
       int row = low_word( *(pop_array + index) );

       ant::pack(pkg,(uint8_t)col,pkg_index);
       ant::pack(pkg,(uint8_t)row,pkg_index);
    }

    init_proto_head(pkg,CMD_CLEAR_POPS,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_bottom_row_index(sprite* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_single_game_bottom_row_index");

    assert(m_p_single_pop_game != null);

    int bottom_row_index = m_p_single_pop_game->get_bottom_row_index();

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)bottom_row_index,pkg_index);

    init_proto_head(pkg,CMD_BOTTOM_ROW_UPDATE,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_request_use_props_ack(sprite* p_sprite,int props_type,int result,int seq_num)
{
    DEBUG_LOG("RESPONSE: response_single_game_request_use_props_ack");

    assert(m_p_single_pop_game != null);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)props_type,pkg_index);
    ant::pack(pkg,(uint8_t)result,pkg_index);
    ant::pack(pkg,(uint8_t)seq_num,pkg_index);

    init_proto_head(pkg,CMD_USE_PROP_ACK,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_level_over(sprite* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_single_game_level_over");

    assert(m_p_single_pop_game != null);

    single_pop_game_progress_t game_progress;
    m_p_single_pop_game->get_game_progress(&game_progress);
    assert(game_progress == game_progress_level_over);

    int current_level = m_p_single_pop_game->get_current_level();

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)current_level,pkg_index);

    init_proto_head(pkg,CMD_LEVEL_OVER,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    //---------------------
    //remove all timers
    del_single_game_all_timer(p_sprite);

    //---------------------
    //determine whether game is all over
    if(m_p_single_pop_game->is_game_all_over() == S_OK)
    {
        response_single_game_over_success(p_sprite);
        return 1;
    }

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_error(sprite* p_sprite,int cmd,int errno)
{
    DEBUG_LOG("RESPONSE: response_single_game_error");

    return send_to_self_error(p_sprite,cmd,errno,1);
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_generate_colorful_pop(sprite* p_sprite,int col,int row)
{
    DEBUG_LOG("RESPONSE: response_single_game_generate_colorful_pop");

    assert(m_p_single_pop_game != null);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)col,pkg_index);
    ant::pack(pkg,(uint8_t)row,pkg_index);

    init_proto_head(pkg,CMD_GENERATE_COLORFUL_POP,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_clear_colorful_pop(sprite* p_sprite,int col,int row)
{
    DEBUG_LOG("RESPONSE: response_single_game_clear_colorful_pop");

    assert(m_p_single_pop_game != null);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)col,pkg_index);
    ant::pack(pkg,(uint8_t)row,pkg_index);

    init_proto_head(pkg,CMD_CLEAR_COLORFUL_POP,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_bonus(sprite* p_sprite,bool is_get_props)
{
    DEBUG_LOG("RESPONSE: response_single_game_bonus");

    assert(m_p_single_pop_game != null);

    //--------------------
    //get pop game bonus,and submit to db
    pop_game_bonus_t pop_game_bonus;
    memset(&pop_game_bonus,0,sizeof(pop_game_bonus));
    m_p_single_pop_game->get_pop_game_bonus(&pop_game_bonus,1);

    game_score_t game_score;
    memset(&game_score,0,sizeof(game_score));

    pack_score_session(p_sprite,&game_score,p_sprite->group->game->id,m_p_single_pop_game->get_total_score());

    game_score.coins = pop_game_bonus.mole_coin;
    game_score.exp = pop_game_bonus.exp;
    game_score.strong = pop_game_bonus.strength;
    game_score.score = m_p_single_pop_game->get_total_score();

    //--------------------
    //determine whether should get props
    if(is_get_props)
    {
        srand(time(null));
        int random_num = rand();

        if(random_num % 10 == 0)
        {
            game_score.itmid = 12111;
            game_score.itmkind = 0;
            game_score.itm_max = 1;
        }
    }
	/*
	if(IS_ENABLE_YUANBAO(p_sprite)) {
		DEBUG_LOG("%d has enable yuanbao box", p_sprite->id);
		if (game_score.score < 201) {

		} else {
			game_score.itmid = get_yuanbao(p_sprite);
		}
	}
	if (game_score.itmid) {
		DEBUG_LOG("%d get item %d", p_sprite->id, game_score.itmid);
	}
	*/
    submit_game_score(p_sprite,&game_score);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_shoot_pop_array(sprite_t* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_single_game_shoot_pop_array");

    assert(m_p_single_pop_game != null);

    //--------------------
    //get shoot pop array
    const int shoot_pop_num = 5;
    c_pop_t shoot_pop_array[shoot_pop_num];
    m_p_single_pop_game->get_shoot_pop_array(shoot_pop_array,shoot_pop_num);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)shoot_pop_num,pkg_index);

    for(int index = 0; index < shoot_pop_num; index++)
    {
        pop_color_t pop_color;
        shoot_pop_array[index].get_pop_color(&pop_color);

        ant::pack(pkg,(uint8_t)1,pkg_index);
        ant::pack(pkg,(uint8_t)pop_color,pkg_index);
    }

    init_proto_head(pkg,CMD_SHOOT_POP_ARRAY,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_game_impl_t::response_single_game_tail_shoot_pop_update(sprite_t* p_sprite,c_pop_t& new_shoot_pop)
{
    DEBUG_LOG("RESPONSE: response_single_game_tail_shoot_pop_update");

    assert(m_p_single_pop_game != null);

    pop_color_t pop_color;
    new_shoot_pop.get_pop_color(&pop_color);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)1,pkg_index);
    ant::pack(pkg,(uint8_t)pop_color,pkg_index);

    init_proto_head(pkg,CMD_TAIL_SHOOT_POP_UPDATE,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

//**********************************************************************************//
//**********************************************************************************//

//-----------------------------------//
//-----------------------------------//

c_multi_pop_game_impl_t::c_multi_pop_game_impl_t()
{
    m_p_single_pop_game_first = null;
    m_p_single_pop_game_second = null;
}

//-----------------------------------//
//-----------------------------------//

c_multi_pop_game_impl_t::~c_multi_pop_game_impl_t()
{
    if(m_p_single_pop_game_first)
    {
        delete m_p_single_pop_game_first;
        m_p_single_pop_game_first = null;
    }

    if(m_p_single_pop_game_second)
    {
        delete m_p_single_pop_game_second;
        m_p_single_pop_game_second = null;
    }
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::init_multi_pop_game()
{
    m_p_single_pop_game_first = new (std::nothrow) c_single_pop_game_t();
    m_p_single_pop_game_second = new (std::nothrow) c_single_pop_game_t();

    if(m_p_single_pop_game_first == null || m_p_single_pop_game_second == null)
    {
        if(m_p_single_pop_game_first)
        {
            delete m_p_single_pop_game_first;
            m_p_single_pop_game_first = null;
        }

        if(m_p_single_pop_game_second)
        {
            delete m_p_single_pop_game_second;
            m_p_single_pop_game_second = null;
        }

        return E_OUTOFMEMORY;
    }
    else
    {
        return S_OK;
    }
}

//-----------------------------------//
//-----------------------------------//

void c_multi_pop_game_impl_t::init(sprite_t* p_sprite)
{
    assert(m_p_single_pop_game_first != null);
    assert(m_p_single_pop_game_second != null);

    m_p_single_pop_game_first->init_single_pop_game();
    m_p_single_pop_game_first->start_game();

    m_p_single_pop_game_second->init_single_pop_game();
    m_p_single_pop_game_second->start_game();

    assert(p_sprite == p_sprite->group->players[0] || p_sprite == p_sprite->group->players[1]);

    //---------------------
    //set sprite to user data
    m_p_single_pop_game_first->set_user_data((long)p_sprite->group->players[0]);
    m_p_single_pop_game_second->set_user_data((long)p_sprite->group->players[1]);

    //---------------------
    //set second's pop map to first's
    unsigned char raw_pop_map[1024 * 8];
    int data_len = 0;

    m_p_single_pop_game_first->get_raw_pop_map(raw_pop_map,sizeof(raw_pop_map),&data_len);
    assert(data_len > 0);

    m_p_single_pop_game_second->set_raw_pop_map(raw_pop_map,data_len);

    //---------------------
    //setup movedown timer
    setup_multi_game_movedown_timer(p_sprite->group);

    //---------------------
    //setup colorful pop timer
    setup_multi_game_colorful_pop_timer(p_sprite->group);
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::handle_data(sprite_t* p_sprite, int cmd, const uint8_t body[], int data_len)
{
    if(m_p_single_pop_game_first->get_user_data() == (long)p_sprite)
    {
        return handle_multi_pop_game_data(m_p_single_pop_game_first,p_sprite,cmd,body,data_len);
    }
    else
    {
    	DEBUG_LOG("m_p_single_pop_game_second->get_user_data():%lu", m_p_single_pop_game_second->get_user_data());
        assert(m_p_single_pop_game_second->get_user_data() == (long)p_sprite);
        return handle_multi_pop_game_data(m_p_single_pop_game_second,p_sprite,cmd,body,data_len);
    }
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::handle_multi_pop_game_data(c_single_pop_game_t* p_pop_game,sprite* p_sprite,int cmd,const uint8_t data[],int data_len)
{
    DEBUG_LOG("RESPONSE: handle_multi_pop_game_data");

    assert(p_pop_game != null);

    if(cmd == CMD_REQUEST_NEW_LEVEL)
    {
        DEBUG_LOG("MULTI GAME RECV PACKAGE: CMD_REQUEST_NEW_LEVEL");

        //---------------------
        //return level info
        response_multi_game_level_info(p_pop_game,p_sprite);

        //---------------------
        //return shoot pop array
        response_multi_game_shoot_pop_array(p_pop_game,p_sprite);

        //---------------------
        //return current props num
        response_multi_game_props_num_shoot(p_pop_game,p_sprite);
        response_multi_game_props_num_bomb(p_pop_game,p_sprite);
        response_multi_game_props_num_spider(p_pop_game,p_sprite);

        //---------------------
        //return current score
        response_multi_game_score(p_pop_game,p_sprite);

        return 0;

    }//if cmd == CMD_REQUEST_NEW_LEVEL
    else if(cmd == CMD_SHOOT_POS)
    {
        DEBUG_LOG("MULTI GAME RECV PACKAGE: CMD_SHOOT_POS");

        if(data_len != 3)
        {
            return 0;
        }

        //---------------------
        //get x and y of the shoot pos
        int col = *data;
        int row = *(data + 1);
        int seq_num = *(data + 2);

        //---------------------
        //add shooted pop
        c_pop_t shooted_pop;
        int result = p_pop_game->add_shooted_pop(row,col,&shooted_pop);

        if(result == S_FALSE)
        {
            //---------------------
            //game over
            response_multi_game_over_fail(p_pop_game,p_sprite);
            response_multi_game_other_over_fail(p_pop_game,p_sprite);

            return 1;
        }
        else if(result == E_FAIL)
        {
            //---------------------
            //invalid pos
            WARN_LOG("[PPL][Invalid Pos][UserId = %d]",p_sprite->id);

            response_multi_game_over_fail(p_pop_game,p_sprite);
            response_multi_game_other_over_fail(p_pop_game,p_sprite);

            return 1;
        }
        else if(result != S_OK)
        {
            //response_multi_game_over_fail(p_pop_game,p_sprite);
            //response_multi_game_other_over_fail(p_pop_game,p_sprite);

            return 0;
        }

        //---------------------
        //request new shoot pop and response to client
        c_pop_t new_shoot_pop;
        p_pop_game->request_new_shoot_pop(&new_shoot_pop);

        response_multi_game_tail_shoot_pop_update(p_pop_game,p_sprite,new_shoot_pop);

        //---------------------
        //erase pops
        unsigned short pops_array[256];
        int pops_num = 0;

        //---------------------
        //determine whether current shoot pop is bomb or normal pop
        pop_type_t shooted_pop_type;
        shooted_pop.get_pop_type(&shooted_pop_type);

        if(shooted_pop_type == pop_type_normal)
        {
            result = p_pop_game->erase_same_color_pop_group(shooted_pop,pops_array,sizeof(pops_array)/sizeof(unsigned short),&pops_num);

            //---------------------
            //response erased pops
            response_multi_game_pops_erased(p_pop_game,p_sprite,seq_num,pops_array,pops_num);

            if(result == S_FALSE)
            {
                //---------------------
                //game may over or success
                single_pop_game_progress_t game_progress;
                p_pop_game->get_game_progress(&game_progress);

                assert(game_progress == game_progress_over_fail || game_progress == game_progress_level_over);

                if(game_progress == game_progress_over_fail)
                {
                    response_multi_game_over_fail(p_pop_game,p_sprite);
                    response_multi_game_other_over_fail(p_pop_game,p_sprite);

                    return 1;
                }
                else
                {
                    response_multi_game_over_success(p_pop_game,p_sprite);
                    response_multi_game_other_over_success(p_pop_game,p_sprite);

                    return 1;
                }
            }

            //---------------------
            //determine whether erased pop should add to others
            if(pops_num >= 6)
            {
                response_multi_game_other_add_pop_notify(p_pop_game,p_sprite,(int)pops_num / 2);
            }

            assert(result == S_OK);
        }//if shooted_pop_type == pop_type_normal
        else
        {
            result = p_pop_game->erase_pops_around_bomb_enlarge(shooted_pop,pops_array,sizeof(pops_array)/sizeof(unsigned short),&pops_num);

            //---------------------
            //response erased pops
            response_multi_game_pops_erased(p_pop_game,p_sprite,seq_num,pops_array,pops_num);

            if(result == S_FALSE)
            {
                //---------------------
                //game success
                response_multi_game_over_success(p_pop_game,p_sprite);
                response_multi_game_other_over_success(p_pop_game,p_sprite);

                return 1;
            }

            assert(result == S_OK);
        }

        //---------------------
        //clear isolated pops
        if(pops_num > 0)
        {
            pops_num = 0;

            result = p_pop_game->clear_isolated_pops(pops_array,sizeof(pops_array)/sizeof(unsigned short),&pops_num);

            //---------------------
            //response cleared pops
            response_multi_game_pops_cleared(p_pop_game,p_sprite,seq_num,pops_array,pops_num);

            if(result == S_FALSE)
            {
                //---------------------
                //game success
                response_multi_game_over_success(p_pop_game,p_sprite);
                response_multi_game_other_over_success(p_pop_game,p_sprite);

                return 1;
            }

        }//if pops_num > 0

        //---------------------
        //response scroe
        response_multi_game_score(p_pop_game,p_sprite);

        //---------------------
        //update bottom row index
        response_multi_game_bottom_row_index(p_pop_game,p_sprite);

        //---------------------
        //update props num
        response_multi_game_props_num_shoot(p_pop_game,p_sprite);
        response_multi_game_props_num_bomb(p_pop_game,p_sprite);
        response_multi_game_props_num_spider(p_pop_game,p_sprite);

        return 0;
    }//else if cmd == CMD_SHOOT_POS
    else if(cmd == CMD_REQUEST_USE_PROP)
    {
        DEBUG_LOG("MULTI GAME RECV PACKAGE: CMD_REQUEST_USE_PROP");

        if(data_len != 2)
        {
            return 0;
        }

        int pop_type = *data;
        int seq_num = *(data + 1);

        int allow_to_use = 0;

        if(pop_type != 1 && pop_type != 2 && pop_type != 3)
        {
            return 0;
        }

        //---------------------
        //attemp to use pop
        if(pop_type == 1)
        {
            //---------------------
            //attemp to use props shoot
            int result = p_pop_game->use_props_shoot();

            if(result == S_OK)
            {
                allow_to_use = 1;
            }

            //---------------------
            //response to user
            response_multi_game_request_use_props_ack(p_pop_game,p_sprite,pop_type,allow_to_use,seq_num);
        }
        else if(pop_type == 2)
        {
            //---------------------
            //attemp to use props bomb
            int result = p_pop_game->use_props_bomb();

            if(result == S_OK)
            {
                allow_to_use = 1;
            }

            //---------------------
            //response to user
            response_multi_game_request_use_props_ack(p_pop_game,p_sprite,pop_type,allow_to_use,seq_num);
        }
        else if(pop_type == 3)
        {
            //---------------------
            //attemp to user props spider
            int result = p_pop_game->use_props_spider();

            if(result == S_OK)
            {
                allow_to_use = 1;

                //---------------------
                //response to other user
                response_multi_game_other_use_spider(p_pop_game,p_sprite);
            }

            //---------------------
            //response to user
            response_multi_game_request_use_props_ack(p_pop_game,p_sprite,pop_type,allow_to_use,seq_num);
        }
        else
        {
        }

        //---------------------
        //update props num
        response_multi_game_props_num_shoot(p_pop_game,p_sprite);
        response_multi_game_props_num_bomb(p_pop_game,p_sprite);
        response_multi_game_props_num_spider(p_pop_game,p_sprite);

        return 0;

    }//else if cmd == CMD_REQUEST_USE_PROP
    else if(cmd == CMD_USER_SCREEN_STATUS)
    {
        DEBUG_LOG("MULTI GAME RECV PACKAGE: CMD_USER_SCREEN_STATUS");

        if(data_len > 1024 * 2)
        {
            return 0;
        }

        response_multi_game_other_screen_status(p_pop_game,p_sprite,(unsigned char*)data,data_len);

        return 0;

    }//else if cmd == CMD_USER_SCREEN_STATUS
    else if(cmd == CMD_REQUEST_ADD_POP)
    {
        if(data_len != 1)
        {
            return 0;
        }

        int pop_request_add_num = *(data);

        if(pop_request_add_num <= 0)
        {
            return 0;
        }

        //---------------------
        //user request add pop
        added_pop_info_t added_pop_info_array[200];
        int added_pop_num = 0;

        p_pop_game->add_pop(pop_request_add_num,added_pop_info_array,sizeof(added_pop_info_array)/sizeof(added_pop_info_t),&added_pop_num);

        response_multi_game_pop_added(p_pop_game,p_sprite,added_pop_info_array,added_pop_num);

        return 0;
    }
    else if(cmd == proto_player_leave)
    {
        //---------------------
        //user has exit
        single_pop_game_progress_t game_progress;
        p_pop_game->get_game_progress(&game_progress);

        if(game_progress == game_progress_going)
        {
            //---------------------
            //user abort game,game fail
            p_pop_game->end_game(false);//force to end,game fail

            response_multi_game_over_fail(p_pop_game,p_sprite);
            response_multi_game_other_over_fail(p_pop_game,p_sprite);
        }

        return 1;
    }
    else
    {
    }

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::setup_multi_game_movedown_timer(game_group* p_game_group)
{
    DEBUG_LOG("TIMER: setup_multi_game_movedown_timer");

    //---------------------
    //get expire time
    int expire_time = time(null) + MULTI_GAME_MOVEDOWN_TIMER_INTERVAL;

    //---------------------
    //add move down timer
    ADD_TIMER_EVENT(p_game_group,deal_multi_game_movedown_timer,this,expire_time);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::deal_multi_game_movedown_timer(void* p_data1,void* p_data2)
{
    DEBUG_LOG("TIMER: deal_multi_game_movedown_timer");

    game_group* p_game_group = (game_group*)p_data1;
    c_multi_pop_game_impl_t* p_multi_pop_game = (c_multi_pop_game_impl_t*)p_data2;

    assert(p_game_group != null);
    assert(p_multi_pop_game != null);
    assert(p_multi_pop_game->m_p_single_pop_game_first != null);
    assert(p_multi_pop_game->m_p_single_pop_game_second != null);

    //---------------------
    //move down
    {
        sprite* p_sprite = (sprite*)p_multi_pop_game->m_p_single_pop_game_first->get_user_data();

        int result = p_multi_pop_game->m_p_single_pop_game_first->move_downward();

        if(result == S_FALSE)
        {
            //---------------------
            //game is over
            p_multi_pop_game->response_multi_game_over_fail(p_multi_pop_game->m_p_single_pop_game_first,p_sprite);
            p_multi_pop_game->response_multi_game_other_over_fail(p_multi_pop_game->m_p_single_pop_game_first,p_sprite);

            return 1;
        }
        else
        {
            //---------------------
            //response to user
            p_multi_pop_game->response_multi_game_bottom_row_index(p_multi_pop_game->m_p_single_pop_game_first,p_sprite);
        }
    }

    {
        sprite* p_sprite = (sprite*)p_multi_pop_game->m_p_single_pop_game_second->get_user_data();

        int result = p_multi_pop_game->m_p_single_pop_game_second->move_downward();

        if(result == S_FALSE)
        {
            //---------------------
            //game is over
            p_multi_pop_game->response_multi_game_over_fail(p_multi_pop_game->m_p_single_pop_game_second,p_sprite);
            p_multi_pop_game->response_multi_game_other_over_fail(p_multi_pop_game->m_p_single_pop_game_second,p_sprite);

            return 1;
        }
        else
        {
            //---------------------
            //response to user
            p_multi_pop_game->response_multi_game_bottom_row_index(p_multi_pop_game->m_p_single_pop_game_second,p_sprite);
        }
    }

    //---------------------
    //continue to work
    int expire_time = time(null) + MULTI_GAME_MOVEDOWN_TIMER_INTERVAL;
    ADD_TIMER_EVENT(p_game_group,deal_multi_game_movedown_timer,p_multi_pop_game,expire_time);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::setup_multi_game_colorful_pop_timer(game_group* p_game_group)
{
    DEBUG_LOG("RESPONSE: setup_multi_game_colorful_pop_timer");

    //---------------------
    //generate colorful pop timer
    int expire_time = time(null) + GENERATE_COLORFUL_POP_TIMER_INTERVAL;
    ADD_TIMER_EVENT(p_game_group,deal_multi_game_generate_colorful_pop_timer,this,expire_time);

    //---------------------
    //clear colorful pop timer
    expire_time = time(null) + CLEAR_COLORFUL_POP_TIMER_INTERVAL;
    ADD_TIMER_EVENT(p_game_group,deal_multi_game_clear_colorful_pop_timer,this,expire_time);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::deal_multi_game_generate_colorful_pop_timer(void* p_data1,void* p_data2)
{
    DEBUG_LOG("TIMER: deal_multi_game_generate_colorful_pop_timer");

    game_group* p_game_group = (game_group*)p_data1;
    c_multi_pop_game_impl_t* p_multi_pop_game = (c_multi_pop_game_impl_t*)p_data2;

    assert(p_game_group != null);
    assert(p_multi_pop_game != null);
    assert(p_multi_pop_game->m_p_single_pop_game_first != null);
    assert(p_multi_pop_game->m_p_single_pop_game_second != null);

    //---------------------
    //generate colorful pop
    {
        sprite* p_sprite = (sprite*)p_multi_pop_game->m_p_single_pop_game_first->get_user_data();

        c_pop_t colorful_pop;
        int result = p_multi_pop_game->m_p_single_pop_game_first->generate_colorful_pop(COLORFUL_POP_TTL,&colorful_pop);

        if(result == S_OK)
        {
            //---------------------
            //successful generate a colorful pop
            int col = high_word(colorful_pop.get_pop_id());
            int row = low_word(colorful_pop.get_pop_id());

            p_multi_pop_game->response_multi_game_generate_colorful_pop(p_multi_pop_game->m_p_single_pop_game_first,p_sprite,col,row);
        }
    }

    {
        sprite* p_sprite = (sprite*)p_multi_pop_game->m_p_single_pop_game_second->get_user_data();

        c_pop_t colorful_pop;
        int result = p_multi_pop_game->m_p_single_pop_game_second->generate_colorful_pop(COLORFUL_POP_TTL,&colorful_pop);

        if(result == S_OK)
        {
            //---------------------
            //successful generate a colorful pop
            int col = high_word(colorful_pop.get_pop_id());
            int row = low_word(colorful_pop.get_pop_id());

            p_multi_pop_game->response_multi_game_generate_colorful_pop(p_multi_pop_game->m_p_single_pop_game_second,p_sprite,col,row);
        }
    }

    //---------------------
    //continue to work
    int expire_time = time(null) + GENERATE_COLORFUL_POP_TIMER_INTERVAL;
    ADD_TIMER_EVENT(p_game_group,deal_multi_game_generate_colorful_pop_timer,p_multi_pop_game,expire_time);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::deal_multi_game_clear_colorful_pop_timer(void* p_data1,void* p_data2)
{
    DEBUG_LOG("TIMER: deal_multi_game_clear_colorful_pop_timer");

    game_group* p_game_group = (game_group*)p_data1;
    c_multi_pop_game_impl_t* p_multi_pop_game = (c_multi_pop_game_impl_t*)p_data2;

    assert(p_game_group != null);
    assert(p_multi_pop_game != null);
    assert(p_multi_pop_game->m_p_single_pop_game_first != null);
    assert(p_multi_pop_game->m_p_single_pop_game_second != null);

    //---------------------
    //clear colorful pop
    {
        sprite* p_sprite = (sprite*)p_multi_pop_game->m_p_single_pop_game_first->get_user_data();

        c_pop_t colorful_pop;
        int result = p_multi_pop_game->m_p_single_pop_game_first->clear_colorful_pop(&colorful_pop);

        if(result == S_OK)
        {
            int col = high_word(colorful_pop.get_pop_id());
            int row = low_word(colorful_pop.get_pop_id());

            p_multi_pop_game->response_multi_game_clear_colorful_pop(p_multi_pop_game->m_p_single_pop_game_first,p_sprite,col,row);
        }
    }

    {
        sprite* p_sprite = (sprite*)p_multi_pop_game->m_p_single_pop_game_second->get_user_data();

        c_pop_t colorful_pop;
        int result = p_multi_pop_game->m_p_single_pop_game_second->clear_colorful_pop(&colorful_pop);

        if(result == S_OK)
        {
            int col = high_word(colorful_pop.get_pop_id());
            int row = low_word(colorful_pop.get_pop_id());

            p_multi_pop_game->response_multi_game_clear_colorful_pop(p_multi_pop_game->m_p_single_pop_game_second,p_sprite,col,row);
        }
    }

    //---------------------
    //continue to work
    int expire_time = time(null) + CLEAR_COLORFUL_POP_TIMER_INTERVAL;
    ADD_TIMER_EVENT(p_game_group,deal_multi_game_clear_colorful_pop_timer,p_multi_pop_game,expire_time);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_over_fail(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_multi_game_over_fail");

    assert(p_pop_game != null);

    //---------------------
    //response game bonus first (lose)
    response_multi_game_bonus(p_pop_game,p_sprite,0);

    single_pop_game_progress_t game_progress;
    p_pop_game->get_game_progress(&game_progress);
    assert(game_progress == game_progress_over_fail);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)0,pkg_index);

    init_proto_head(pkg,CMD_GAME_OVER_FAIL,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,0);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_over_success(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_multi_game_over_success");

    assert(p_pop_game != null);

    //---------------------
    //response game bonus first (win)
    response_multi_game_bonus(p_pop_game,p_sprite,1);

    single_pop_game_progress_t game_progress;
    p_pop_game->get_game_progress(&game_progress);
    assert(game_progress == game_progress_over_success || game_progress == game_progress_level_over);

    int pkg_index = sizeof(protocol_t);

    init_proto_head(pkg,CMD_GAME_OVER_SUCCESS,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,0);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_level_info(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_multi_game_level_info");

    assert(p_pop_game != null);

    int current_level = p_pop_game->get_current_level();
    int pops_num = (_MAX_POP_MAP_ROW_NUM - _INIT_START_ROW_INDEX) * _MAX_POP_MAP_COL_NUM;

    unsigned char map_info[1024 * 2];
    int map_info_data_len = 0;
    int result = p_pop_game->get_map_info(map_info,sizeof(map_info),&map_info_data_len);

    if(result != S_OK)
    {
        return result;
    }

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)current_level,pkg_index);
    ant::pack(pkg,(uint16_t)pops_num,pkg_index);

    memcpy(pkg + pkg_index,map_info,map_info_data_len);
    pkg_index += map_info_data_len;

    init_proto_head(pkg,CMD_NEW_LEVEL_INFO,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_current_pops_update(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_multi_game_current_pops_update");

    assert(p_pop_game != null);

    //---------------------
    //send current shoot pop
    c_pop_t current_shoot_pop;
    p_pop_game->get_current_shoot_pop(&current_shoot_pop);

    pop_type_t current_shoot_pop_type;
    current_shoot_pop.get_pop_type(&current_shoot_pop_type);

    int pop_type = -1;
    if(current_shoot_pop_type == pop_type_normal)
    {
        pop_type = 1;
    }
    else
    {
        pop_type = 0;
    }

    pop_color_t current_shoot_pop_color;
    current_shoot_pop.get_pop_color(&current_shoot_pop_color);

    int pop_color = (int)current_shoot_pop_color;

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)1,pkg_index);
    ant::pack(pkg,(uint8_t)pop_type,pkg_index);
    ant::pack(pkg,(uint8_t)pop_color,pkg_index);

    init_proto_head(pkg,CMD_SHOOT_POP_UPDATE,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    //---------------------
    //send current bakup pop
    c_pop_t current_bakup_pop;
    p_pop_game->get_current_bakup_pop(&current_bakup_pop);

    pop_type_t current_bakup_pop_type;
    current_bakup_pop.get_pop_type(&current_bakup_pop_type);

    if(current_bakup_pop_type == pop_type_normal)
    {
        pop_type = 1;
    }
    else
    {
        pop_type = 0;
    }

    pop_color_t current_bakup_pop_color;
    current_bakup_pop.get_pop_color(&current_bakup_pop_color);

    pop_color = (int)current_bakup_pop_color;

    pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)0,pkg_index);
    ant::pack(pkg,(uint8_t)pop_type,pkg_index);
    ant::pack(pkg,(uint8_t)pop_color,pkg_index);

    init_proto_head(pkg,CMD_SHOOT_POP_UPDATE,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_props_num_shoot(c_single_pop_game_t* p_pop_game,sprite* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_multi_game_props_num_shoot");

    assert(p_pop_game != null);

    int props_shoot_num = p_pop_game->get_props_shoot_num();

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)1,pkg_index);
    ant::pack(pkg,(uint8_t)props_shoot_num,pkg_index);

    init_proto_head(pkg,CMD_PROPS_NUM_UPDATE,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_props_num_bomb(c_single_pop_game_t* p_pop_game,sprite* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_multi_game_props_num_bomb");

    assert(p_pop_game != null);

    int props_bomb_num = p_pop_game->get_props_bomb_num();

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)2,pkg_index);
    ant::pack(pkg,(uint8_t)props_bomb_num,pkg_index);

    init_proto_head(pkg,CMD_PROPS_NUM_UPDATE,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_props_num_spider(c_single_pop_game_t* p_pop_game,sprite* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_multi_game_props_num_spider");

    assert(p_pop_game != null);

    int props_spider_num = p_pop_game->get_props_spider_num();

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)3,pkg_index);
    ant::pack(pkg,(uint8_t)props_spider_num,pkg_index);

    init_proto_head(pkg,CMD_PROPS_NUM_UPDATE,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_score(c_single_pop_game_t* p_pop_game,sprite* p_sprite)
{
    assert(p_pop_game != null);

    int game_score = p_pop_game->get_total_score();

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint32_t)game_score,pkg_index);

    init_proto_head(pkg,CMD_SCORE_UPDATE,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_pops_erased(c_single_pop_game_t* p_pop_game,sprite* p_sprite,int seq_num,unsigned short* pop_array,int pop_num)
{
    DEBUG_LOG("RESPONSE: response_multi_game_pops_erased");

    assert(p_pop_game != null);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)seq_num,pkg_index);
    ant::pack(pkg,(uint8_t)pop_num,pkg_index);

    for(int index = 0; index < pop_num; index++)
    {
       int col = high_word( *(pop_array + index) );
       int row = low_word( *(pop_array + index) );

       ant::pack(pkg,(uint8_t)col,pkg_index);
       ant::pack(pkg,(uint8_t)row,pkg_index);
    }

    init_proto_head(pkg,CMD_ERASE_POPS,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_pops_cleared(c_single_pop_game_t* p_pop_game,sprite* p_sprite,int seq_num,unsigned short* pop_array,int pop_num)
{
    assert(p_pop_game != null);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)seq_num,pkg_index);
    ant::pack(pkg,(uint8_t)pop_num,pkg_index);

    for(int index = 0; index < pop_num; index++)
    {
       int col = high_word( *(pop_array + index) );
       int row = low_word( *(pop_array + index) );

       ant::pack(pkg,(uint8_t)col,pkg_index);
       ant::pack(pkg,(uint8_t)row,pkg_index);
    }

    init_proto_head(pkg,CMD_CLEAR_POPS,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_bottom_row_index(c_single_pop_game_t* p_pop_game,sprite* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_multi_game_bottom_row_index");

    assert(p_pop_game != null);

    int bottom_row_index = p_pop_game->get_bottom_row_index();

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)bottom_row_index,pkg_index);

    init_proto_head(pkg,CMD_BOTTOM_ROW_UPDATE,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_request_use_props_ack(c_single_pop_game_t* p_pop_game,sprite* p_sprite,int props_type,int result,int seq_num)
{
    assert(p_pop_game != null);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)props_type,pkg_index);
    ant::pack(pkg,(uint8_t)result,pkg_index);
    ant::pack(pkg,(uint8_t)seq_num,pkg_index);

    init_proto_head(pkg,CMD_USE_PROP_ACK,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_generate_colorful_pop(c_single_pop_game_t* p_pop_game,sprite* p_sprite,int col,int row)
{
    DEBUG_LOG("RESPONSE: response_multi_game_generate_colorful_pop");

    assert(p_pop_game != null);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)col,pkg_index);
    ant::pack(pkg,(uint8_t)row,pkg_index);

    init_proto_head(pkg,CMD_GENERATE_COLORFUL_POP,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_clear_colorful_pop(c_single_pop_game_t* p_pop_game,sprite* p_sprite,int col,int row)
{
    DEBUG_LOG("RESPONSE: response_multi_game_clear_colorful_pop");

    assert(p_pop_game != null);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)col,pkg_index);
    ant::pack(pkg,(uint8_t)row,pkg_index);

    init_proto_head(pkg,CMD_CLEAR_COLORFUL_POP,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//
//important note: data send to user next to p_pop_game

int c_multi_pop_game_impl_t::response_multi_game_other_over_fail(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_multi_game_other_over_fail");

    assert(p_pop_game != null);

    if(p_pop_game == m_p_single_pop_game_first)
    {
        p_sprite = (sprite*)m_p_single_pop_game_second->get_user_data();
        m_p_single_pop_game_second->end_game(true);//force to end game

        //---------------------
        //response bonus (other fail,you win)
        response_multi_game_bonus(m_p_single_pop_game_second,p_sprite,1);
    }
    else
    {
        assert(p_pop_game == m_p_single_pop_game_second);
        p_sprite = (sprite*)m_p_single_pop_game_first->get_user_data();
        m_p_single_pop_game_first->end_game(true);//force to end game

        //---------------------
        //response bonus (other fail,you win)
        response_multi_game_bonus(m_p_single_pop_game_first,p_sprite,1);
    }

    int pkg_index = sizeof(protocol_t);

    init_proto_head(pkg,CMD_OTHER_GAME_OVER_FAIL,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,0);

    return 0;
}

//-----------------------------------//
//-----------------------------------//
//important note: data send to user next to p_pop_game

int c_multi_pop_game_impl_t::response_multi_game_other_over_success(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_multi_game_other_over_success");

    assert(p_pop_game != null);

    if(p_pop_game == m_p_single_pop_game_first)
    {
        p_sprite = (sprite*)m_p_single_pop_game_second->get_user_data();
        m_p_single_pop_game_second->end_game(false);//force to end game

        //---------------------
        //response bonus (other success,you lose)
        response_multi_game_bonus(m_p_single_pop_game_second,p_sprite,0);

    }
    else
    {
        assert(p_pop_game == m_p_single_pop_game_second);
        p_sprite = (sprite*)m_p_single_pop_game_first->get_user_data();
        m_p_single_pop_game_first->end_game(false);//force to end game

        //---------------------
        //response bonus (other success,you win)
        response_multi_game_bonus(m_p_single_pop_game_first,p_sprite,0);

    }

    int pkg_index = sizeof(protocol_t);

    init_proto_head(pkg,CMD_OTHER_GAME_OVER_SUCCESS,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,0);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_other_screen_status(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite,unsigned char* p_data,int data_len)
{
    DEBUG_LOG("RESPONSE: response_multi_game_other_screen_status");

    assert(p_pop_game != null);
    assert(data_len >= 0);

    int pkg_index = sizeof(protocol_t);
    memcpy(pkg + pkg_index,p_data,data_len);
    pkg_index += data_len;

    init_proto_head(pkg,CMD_OTHER_USER_SCREEN_STATUS,pkg_index);

    if(p_pop_game == m_p_single_pop_game_first)
    {
        p_sprite = (sprite*)m_p_single_pop_game_second->get_user_data();
    }
    else
    {
        assert(p_pop_game == m_p_single_pop_game_second);
        p_sprite = (sprite*)m_p_single_pop_game_first->get_user_data();
    }

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_other_use_spider(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_multi_game_other_use_spider");

    assert(p_pop_game != null);

    int pkg_index = sizeof(protocol_t);

    init_proto_head(pkg,CMD_OTHER_USE_SPIDER,pkg_index);

    if(p_pop_game == m_p_single_pop_game_first)
    {
        p_sprite = (sprite*)m_p_single_pop_game_second->get_user_data();
    }
    else
    {
        assert(p_pop_game == m_p_single_pop_game_second);
        p_sprite = (sprite*)m_p_single_pop_game_first->get_user_data();
    }

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_other_add_pop_notify(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite,int add_pop_num)
{
    DEBUG_LOG("RESPONSE: response_multi_game_other_use_spider");

    assert(p_pop_game != null);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)add_pop_num,pkg_index);

    init_proto_head(pkg,CMD_ADD_POP_NOTIFY,pkg_index);

    if(p_pop_game == m_p_single_pop_game_first)
    {
        p_sprite = (sprite*)m_p_single_pop_game_second->get_user_data();
    }
    else
    {
        assert(p_pop_game == m_p_single_pop_game_second);
        p_sprite = (sprite*)m_p_single_pop_game_first->get_user_data();
    }

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_pop_added(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite,added_pop_info_t* p_data,int data_len)
{
    DEBUG_LOG("RESPONSE: response_multi_game_pop_added");

    assert(p_pop_game != null);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)data_len,pkg_index);

    for(int index = 0; index < data_len; index++)
    {
        ant::pack(pkg,(uint8_t)(p_data + index)->col,pkg_index);
        ant::pack(pkg,(uint8_t)(p_data + index)->row,pkg_index);
        ant::pack(pkg,(uint8_t)(p_data + index)->pop_color,pkg_index);
    }

    init_proto_head(pkg,CMD_POP_ADDED,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_bonus(c_single_pop_game_t* p_pop_game,sprite* p_sprite,bool is_win)
{
    DEBUG_LOG("RESPONSE: response_multi_game_bonus");

    assert(p_pop_game != null);

    //--------------------
    //get pop game bonus,and submit to db
    pop_game_bonus_t pop_game_bonus;
    memset(&pop_game_bonus,0,sizeof(pop_game_bonus));
    p_pop_game->get_pop_game_bonus(&pop_game_bonus,is_win);

    game_score_t game_score;
    memset(&game_score,0,sizeof(game_score));

    pack_score_session(p_sprite,&game_score,p_sprite->group->game->id,p_pop_game->get_total_score());

    game_score.coins = pop_game_bonus.mole_coin;
    game_score.exp = pop_game_bonus.exp;
    game_score.strong = pop_game_bonus.strength;
    game_score.score = p_pop_game->get_total_score();

    //--------------------
    //determine whether should get props
    if(is_win && pop_game_bonus.mole_coin > 0)
    {
    	uint32_t db_buf[] = {0, 1, 0, 0, 99, 1351378, 1, 1};
		send_request_to_db(db_proto_modify_items, NULL, sizeof(db_buf), db_buf, p_sprite->id);
	
        srand(time(null));
        int random_num = rand();

        if(random_num % 50 == 0)
        {
            game_score.itmid = 12111;
            game_score.itmkind = 0;
            game_score.itm_max = 1;
        }
    }
        //} else {
        //    game_score.itmid = get_fire_medal(p_sprite, 0);
//
//------------------------------------------------------
//        else
//        {
//            game_score.itmid = get_medal(p_sprite);
//        }
//------------------------------------------------------
    //} else {
    //    game_score.itmid = get_fire_medal(p_sprite, 1);
    /*
	if(IS_ENABLE_YUANBAO(p_sprite)) {
		DEBUG_LOG("%d has enable yuanbao box", p_sprite->id);
		if (game_score.score < 201) {

		} else {
			game_score.itmid = get_yuanbao(p_sprite);
		}
	}
	if (game_score.itmid) {
		DEBUG_LOG("%d get item %d", p_sprite->id, game_score.itmid);
	}
	*/

    submit_game_score(p_sprite,&game_score);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_error(c_single_pop_game_t* p_pop_game,sprite* p_sprite,int cmd,int errno)
{
    DEBUG_LOG("RESPONSE: response_multi_game_error");

    return send_to_self_error(p_sprite,cmd,errno,1);
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_shoot_pop_array(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite)
{
    DEBUG_LOG("RESPONSE: response_multi_game_shoot_pop_array");

    assert(p_pop_game != null);

    //--------------------
    //get shoot pop array
    const int shoot_pop_num = 5;
    c_pop_t shoot_pop_array[shoot_pop_num];
    p_pop_game->get_shoot_pop_array(shoot_pop_array,shoot_pop_num);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)shoot_pop_num,pkg_index);

    for(int index = 0; index < shoot_pop_num; index++)
    {
        pop_color_t pop_color;
        shoot_pop_array[index].get_pop_color(&pop_color);

        ant::pack(pkg,(uint8_t)1,pkg_index);
        ant::pack(pkg,(uint8_t)pop_color,pkg_index);
    }

    init_proto_head(pkg,CMD_SHOOT_POP_ARRAY,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_multi_pop_game_impl_t::response_multi_game_tail_shoot_pop_update(c_single_pop_game_t* p_pop_game,sprite_t* p_sprite,c_pop_t& new_shoot_pop)
{
    DEBUG_LOG("RESPONSE: response_multi_game_tail_shoot_pop_update");

    assert(p_pop_game != null);

    pop_color_t pop_color;
    new_shoot_pop.get_pop_color(&pop_color);

    int pkg_index = sizeof(protocol_t);
    ant::pack(pkg,(uint8_t)1,pkg_index);
    ant::pack(pkg,(uint8_t)pop_color,pkg_index);

    init_proto_head(pkg,CMD_TAIL_SHOOT_POP_UPDATE,pkg_index);

    send_to_self(p_sprite,pkg,pkg_index,1);

    return 0;
}

//-----------------------------------//
//-----------------------------------//

