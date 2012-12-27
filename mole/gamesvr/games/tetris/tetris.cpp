/**
 * =====================================================================================
 *       @file  tetris.cpp
 *      @brief  俄罗斯方块游戏的服务器端的实现
 *
 *   @internal
 *     Created  2008年10月22日 13时51分11秒
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee .Inc
 *   Copyright  Copyright (c) 2008, aceway
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
extern "C" {
#include <gameserv/timer.h>
#include <gameserv/dbproxy.h>

}

#include "tetris.hpp"

//=====================================================================================
//
//                        Public Methods
//
//=====================================================================================

/**
 * @brief  构造函数，需要用一个game_group_t结构体指针初始化
 * @param  game_group_t* grp, 游戏组结构体指针
 * @return None
 */
Tetris::Tetris(game_group_t* grp)
{
    game_group_ = grp;
    DEBUG_LOG("%s: TETRIS CONSTRUCTOR!",game_group_->game->name);
}

/**
 * @brief  析构函数
 * @param  None
 * @return None
 */
Tetris::~Tetris()
{
    DEBUG_LOG("%s: TETRIS DESTRUCTOR!",game_group_->game->name);
}


/**
 * @brief  提供给上层调用的三个接口函数之一. 以初始化游戏数据
 * @param  sprite_t* p, 玩家结构体指针
 * @return void
 */
void Tetris::init(sprite_t* player)
{
    memset(game_score_, 0, sizeof(game_score_t)*2);
    memset(game_sessions_, 0, sizeof(suser_session)*2);
    tetris_start_time = get_now_tv()->tv_sec + game_group_->game->timeout;

    bready_to_transdata = false;
    bhas_end_game = false;

    winning_player_pos_ = 0;
    uiplayer_ready_count = 0;
    for(int i = 0; i < game_group_->count; ++i)
    {
       game_group_->players[i]->pos_id = i%2;
    }
}

/**
 * @brief  提供给上层调用的三个接口函数之一。上层通知本模块处理相关事件的接口
 * @param  sprite_t* player, 玩家结构体指针
 * @param  int cmd, 通讯协议命令号
 * @param  const uint8_t body[], 消息包体指针
 * @param  int len, 消息包体的长度
 * @return int 类型错误码, -1踢掉发送此消息的用户；0正常继续游戏；其他参见结构体game_end_reason_t
 */
inline int Tetris::handle_data(sprite_t* player, int cmd, const uint8_t body[], int body_len)
{
//    DEBUG_LOG("%s: TETRIS HANDLE DATE", game_group_->game->name );
    MOD_EVENT_EXPIRE_TIME(player, on_timer_expire, get_now_tv()->tv_sec + 8);
    int err = -1;
    switch (cmd) {
        case proto_player_leave:
            err = game_player_leave(player, body, body_len);
            break;
        case etetris_game_new_bout:
            err = tetris_new_bout(player, body, body_len);
            break;
        case etetris_trans_data:
            err = tetris_trans_data(player, body, body_len);
            break;
        case etetris_game_end_bout:
            err = tetris_end_bout(player, body, body_len);
            break;
        default:
            ERROR_RETURN(("%s: Unrecognized Command ID %d from uid=%u",
                          game_group_->game->name, cmd, player->id), -1);
            break;
    }
    return err;
}

/**
 * @brief 提供给上层调用的三个接口函数之一。游戏结束时清理资源
 * @param  void
 * @return void
 */
inline void Tetris::destroy(void)
{
}

/**
 * @brief   处理用户游戏超时，处理方式，直接结束用户的游戏
 * @param  void* data
 * @return 枚举game_end_reason_t 类型值, 结束游戏码
 */
inline int Tetris::handle_timeout(void* data)
{
    DEBUG_LOG("%s: TETRIS HANDLE TIMEOUT", game_group_->game->name );
    if(game_group_->count > 1)
    {
        //find winner
        int i = 0;
        sprite_t* winner_player = game_group_->players[0];
        for(i = 0; i < game_group_->count; ++i)
        {
            if(game_group_->players[i]->score > winner_player->score)
            {
                winner_player = game_group_->players[i];
                winning_player_pos_ = winner_player->pos_id;
            }
        }
        if(!bhas_end_game)
        {
            bhas_end_game = true;
            if(game_group_->players[0]->score > game_group_->players[1]->score)
            {
                winning_player_pos_ = game_group_->players[0]->pos_id;
            }
            else
            {
                winning_player_pos_ = game_group_->players[1]->pos_id;
            }

            calculate_score(escore_timeout);
            notify_player_win(winner_player);
            distribute_score();
        }
    }
    else
    {
        if(!bhas_end_game)
        {
            bhas_end_game = true;
            calculate_score(escore_timeout);
            notify_player_win(game_group_->players[0]);
            distribute_score();
        }
    }
    return  GER_end_of_game;
}

//=====================================================================================
//
//                      Private Methods
//
//=====================================================================================
/**
 * @brief  玩家离开了游戏，要清理本地资源
 * @param  sprite_t* player, 玩家结构体指针
 * @param  const uint8_t body[], 玩家发送来的数据体
 * @param   int body_len, 数据体长度
 * @return
 */
inline int Tetris::game_player_leave(sprite_t* player, const uint8_t body[], int body_len)
{
    assert(player != NULL);
    assert(body != NULL);

  //  DEBUG_LOG("game_player_leave(), DATA BODY LEN :[%d].", body_len);
  //  DEBUG_LOG("THIS TIME PLAYER ID:[%u], GROUP USER COUNT:[%u].", player->id, game_group_->count);

    if(game_group_->count > 1)
    {
        for(int i = 0; i < game_group_->count; ++i)
        {
            if(game_group_->players[i]->id != player->id)
            {
                if(!bhas_end_game)
                {
                    bhas_end_game = true;
                    winning_player_pos_ = game_group_->players[i]->pos_id;
                    calculate_score(escore_normal);
                    notify_player_win(game_group_->players[i]);
                    distribute_score();
                }
                break;
            }
        }
    }
    else
    {
        if(!bhas_end_game)
        {
            bhas_end_game = true;
            calculate_score(escore_normal);
            notify_player_win(game_group_->players[0]);
            distribute_score();
        }
    }
    return GER_player_offline;
}

/**
 * @brief  开始游戏, 当进入游戏的玩家满了后，通知所有人可以开始玩了
 * @param  sprite_t* player, 玩家结构体指针
 * @param  const uint8_t body[], 玩家发送来的数据体
 * @param   int body_len, 数据体长度
 * @return void
 */
inline int Tetris::tetris_new_bout(sprite_t* player, const uint8_t body[], int body_len)
{
    assert(player != NULL);
    assert(body != NULL);

//    DEBUG_LOG("tetris_new_bout(), DATA BODY LEN :[%d].", body_len);
//    DEBUG_LOG("THIS TIME PLAYER ID:[%u], GROUP NEED USER COUNT:[%u].", player->id, game_group_->count);

    if(!check_player_session(player, body, body_len)) return -1;

    uiplayer_ready_count++;
    //DEBUG_LOG("ROUP HAS USER COUNT:[%u].", uiplayer_ready_count);
    if(game_group_->count == uiplayer_ready_count)
    {
        bready_to_transdata = true;;
        notify_player_game_ready();
        add_grp_timers(game_group_, get_now_tv()->tv_sec + 30);
        ADD_TIMER_EVENT(game_group_, on_game_timer_expire, (void*)1, get_now_tv()->tv_sec + game_group_->game->timeout -100);
        tetris_start_time = get_now_tv()->tv_sec;
       // DEBUG_LOG("ROUP HAS USER COUNT:[%u].", uiplayer_ready_count);
        return 0;
    }
    else if(game_group_->count > uiplayer_ready_count)
    {
        bready_to_transdata = false;
        return 0;
    }
    else//(game_group_->count < uiplayer_ready_count)
    {
        uiplayer_ready_count--;
        return -1;
    }
}
/**
 * @brief  游戏过程中一个游戏用户提交了数据，要转发通知到其它各个用户
 * @param  sprite_t* player, 玩家结构体指针
 * @param  const uint8_t body[], 玩家发送来的数据体
 * @param   int body_len, 数据体长度
 * @return void
 */
inline int Tetris::tetris_trans_data(sprite_t* player, const uint8_t body[], int body_len)
{
    assert(player != NULL);
    assert(body != NULL);

//    DEBUG_LOG("tetris_trans_data(), PLAYER ID:[%u], DATA BODY LEN = %d.", player->id, body_len);
//    DEBUG_LOG("THIS TIME PLAYER ID:[%u], GROUP USER COUNT:[%u].", player->id, game_group_->count);

    if(!check_player_session(player, body, body_len)) return -1;
    if(!get_client_submit_result(player, body, body_len)) return -1;

    if(IS_GAME_PLAYER(player))
    {
        if(bready_to_transdata && game_group_->count > 1)
        {
            int i = sizeof(protocol_t);
            ant::pack(pkg, body+8, body_len-8, i); //4字节uint32_t的session, 4字节uint32_t的score
            init_proto_head(pkg, etetris_trans_data, i);
            send_to_group_except_self(game_group_, player->id, pkg, i);
        }
        else
        {
        }
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * @brief  游戏结束了，记录用户提交上来的数据（输赢，分数）
 * @param  sprite_t* player, 玩家结构体指针
 * @param  const uint8_t body[], 玩家发送来的数据体
 * @param  int body_len, 数据体长度
 * @return int类型
 */
inline int Tetris::tetris_end_bout(sprite_t* player, const uint8_t body[], int body_len)
{
    assert(player != NULL);
    assert(body != NULL);

//    DEBUG_LOG("tetris_end_bout(), player id:[%u], data body len = %d.", player->id, body_len);
//    DEBUG_LOG("this time player id:[%u], group user count:[%u].", player->id, game_group_->count);

    if(!check_player_session(player, body, body_len)) return -1;

    if(game_group_->count > 1)
    {
        for(int i = 0; i < game_group_->count; ++i)
        {
            if(game_group_->players[i]->id != player->id)
            {
                if(!bhas_end_game)
                {
                    bhas_end_game = true;
                    winning_player_pos_ = game_group_->players[i]->pos_id;
                    calculate_score(escore_normal);
                    notify_player_win(game_group_->players[i]);
                    distribute_score();
                }
                break;
            }
        }
    }
    else
    {
        if(!bhas_end_game)
        {
            bhas_end_game = true;
            calculate_score(escore_normal);
            notify_player_win(game_group_->players[0]);
            distribute_score();
        }
    }
    return GER_end_of_game;
}

inline void Tetris::notify_player_win(sprite_t* winner_player)
{
    int index = 0;
    //notify client who win
    if(game_group_->count > 1)
    {
        for(index = 0; index < game_group_->count; ++index)
        {
            uint8_t win_lost = 0;
            sprite_t* tmp_player = game_group_->players[index];
            if(tmp_player->id == winner_player->id)
            {
                win_lost = 1;
				uint32_t db_buf[] = {0, 1, 0, 0, 99, 1351376, 1, 1};
				send_request_to_db(db_proto_modify_items, NULL, sizeof(db_buf), db_buf, winner_player->id);
            }
            else
            {
                win_lost = 0;
            }
            memset(pkg, 0, 4096);
            int i = sizeof(protocol_t);
            ant::pack(pkg, &win_lost, 1, i);
            init_proto_head(pkg, etetris_game_end_bout, i);
            send_to_self(tmp_player, pkg, i, 0);
        }
    }else
    {
            uint8_t win_lost = 0;
            if(winner_player->score > 16000)
            {
                win_lost = 1;
            }
            else
            {
                win_lost = 0;
            }
            memset(pkg, 0, 4096);
            int i = sizeof(protocol_t);
            ant::pack(pkg, &win_lost, 1, i);
            init_proto_head(pkg, etetris_game_end_bout, i);
            send_to_self(winner_player, pkg, i, 0);
    }
}

/**
 * @brief  通知加入本游戏的玩家，所有游戏成员都准备好了，可以开始游戏
 * @param  void
 * @return void
 */
inline void Tetris::notify_player_game_ready(void)
{
//    DEBUG_LOG("%s: notify client play game ready! user count %d, need count %d",
//                game_group_->game->name, game_group_->count, game_group_->game->players);
    int i = sizeof(protocol_t);
    init_proto_head(pkg, etetris_game_new_bout, i);
    send_to_players(game_group_, pkg, i);
}

/**
 * @brief  检验用户此次的session号是否合法，并更新记录的session号，以用于下次检验
 * @param  sprite_t* player, 用户结构体指针
 * @param  const uint8_t body[], 包含sesseion号的数据，网络接收的原始数据
 * @param  int body_len, 数据包长度
 * @return 检验的成功与否 true 或 false.
 */
inline bool Tetris::check_player_session(sprite_t* player, const uint8_t body[], int body_len)
{
    if((uint32_t)body_len < sizeof(uint32_t)) return false;

    uint32_t tmp_session = *((uint32_t*)body);
    uint32_t local_session = ntohl(tmp_session);

    //第一次添加用户session
    int session_count = 2 > game_group_->game->players ? game_group_->game->players : 2;
    int i = 0;
    for ( i = 0; i < session_count; i += 1 )
    {
        if(game_sessions_[i].user_id == 0)
       {
            game_sessions_[i].user_id = player->id;
            game_sessions_[i].session_id = local_session;
//            DEBUG_LOG("check_player_session(), user [%d] first time check true.", player->id);
            return true;
        }
    }

    //第一次以后就开始检验session
    for ( i = 0; i < session_count; i += 1 )
    {
        if(game_sessions_[i].user_id == player->id)
        {
            if(game_sessions_[i].session_id == (local_session -1))
            {
                game_sessions_[i].session_id++;
//                DEBUG_LOG("check_player_session(), user [%d] check true.", player->id);
                return true;
            }
            else
            {
                DEBUG_LOG("TETRIS, check_player_session(), USER [%d] CHECK FALSE.", player->id);
                DEBUG_LOG("OLD SESSION [%d], NEW SESSION [%d].",
                        game_sessions_[0].session_id, local_session);
                return false;
            }
        }
    }
    DEBUG_LOG("TETRIS, check_player_session(), USER [%d] CHECK FALSE. NO THIS USER SESSION HISTROY",
                player->id);
    return false;
}

/**
 * @brief  解析玩家提交上来的游戏结果，分数
 * @param  sprite_t* player, 玩家结构体指针
 * @param  const uint8_t body[], 玩家发送来的数据体
 * @param  int body_len, 数据体长度
 * @return bool类型，数据不够，不可解析返回false, 否则返回true;
 */
inline bool Tetris::get_client_submit_result(sprite_t* player, const uint8_t body[], int body_len)
{
    if(body_len < 8)
    {
        ERROR_LOG("TETRIS, PARSE CLIENT SUBMIT SCORE. PLAYER ID:[%u], DATA BODY LEN = %d.",
                  player->id, body_len);
        ERROR_LOG("TETRIS, PARSE CLIENT SUBMIT SCORE ERROR, BODY LEN WRONG. NEED AT LEAST LEN :[8]");
        return false;
    }

    uint32_t tmp_score = 0;
    uint32_t score = 0;
    memcpy(&tmp_score, body+4, sizeof(tmp_score));
    score = ntohl(tmp_score);

    for(int i = 0; i < game_group_->count; ++i)
    {
        sprite_t* tmp_player = game_group_->players[i];
        if(tmp_player->id == player->id)
        {
            if(score > 800)
            {
                DEBUG_LOG("TETRIS, USER [%d] SUBMIT SCORE NOT RIGHT!!!!! SUBMIT SCORE IS: [%d].", tmp_player->id, score);
                return false;
            }
            if(score > 0)
            {
                DEBUG_LOG("TETRIS, USER [%d] SUBMIT SCORE, SUBMIT SCORE IS: [%d].", tmp_player->id, score);
            }
            tmp_player->score += score;
        }
    }
    return true;
}

/**
 * @brief  游戏完毕，根据情形因子计算各类玩家获得的分数, 此处玩家分为赢家和输家， 可以括展平手，
 *         看家等
 * @param escore_factor efactor, 计算分数时用的因子
 * @return void
 */
inline void Tetris::calculate_score(escore_factor efactor)
{
    //策划安给的分数计算方式过于简单，目前下面个种方式都是一样的算法:w
    switch(efactor)
    {
    case escore_normal:
        calc_normal_score();
        break;
    case escore_timeout:
        calc_timeout_score();
        break;
    default:
        calc_default_score();
        break;
    }
}

/**
 * @brief  游戏正常结束计算各类玩家的分数
 * @param  void
 * @return void
 */
inline void Tetris::calc_normal_score(void)
{
    //0 下标存的是赢家获得分数， 1下标存的是输家获得分数，

    //赢家是输家 分数的 2 倍
    //可优化，玩的时间越长分数越高
    //以及算上其它个人属性
    //find the winner
    int index = 0;
    int32_t loster_score = 0;
    int32_t winner_score = 0;
    for(index = 0; index < game_group_->count; ++index)
    {
        sprite_t* tmp_player = game_group_->players[index];
        if(tmp_player->pos_id == winning_player_pos_)
        {
            winner_score = tmp_player->score;
        }
        else
        {
            loster_score = tmp_player->score;
        }
    }

    if((get_now_tv()->tv_sec - tetris_start_time) < 30)
    {
        if(game_group_->count == 1)
        {
            game_score_[0].score  = winner_score;
            game_score_[0].coins  = 0;
            game_score_[0].exp    = 0;
            game_score_[0].lovely = 0;
        }
        else
        {
            game_score_[0].score  = winner_score;
            game_score_[0].coins  = 0;
            game_score_[0].exp    = 0;
            game_score_[0].lovely = 0;

            game_score_[1].score  = 0;
            game_score_[1].coins  = 0;
            game_score_[1].exp    = 0;
            game_score_[1].lovely = 0;
        }
        return;
    }
    if(game_group_->count == 1)
    {
        winner_score = winner_score > game_group_->game->score ? game_group_->game->score : winner_score;

        float percent = (float)winner_score / 16000;
        if(percent < 0.2) percent = 0.2;
        if(percent > 1.0) percent = 1.0;

        game_score_[0].score  = winner_score;
        if( game_score_[0].score  > 200)
        {
            game_score_[0].coins  = (int)(game_group_->game->yxb * percent);
            game_score_[0].exp    = (int)(game_group_->game->exp * percent) ;
            game_score_[0].lovely = (int)(game_group_->game->lovely * percent);
        }
        else
        {
            game_score_[0].coins  = 0;
            game_score_[0].exp    = 0;
            game_score_[0].lovely = 0;
        }
    }
    else
    {
        winner_score = winner_score > game_group_->game->score?game_group_->game->score:winner_score;

        float percent = (float)(get_now_tv()->tv_sec - game_group_->start) / 150;
        if(percent < 0.9) percent = 0.9;
        if(percent > 1.0) percent = 1.0;

        game_score_[0].score  = winner_score;
        if( game_score_[0].score  > 200)
        {
            game_score_[0].coins  = (int)(game_group_->game->yxb * percent);
            game_score_[0].exp    = (int)(game_group_->game->exp * percent) ;
            game_score_[0].lovely = (int)(game_group_->game->lovely * percent);

            game_score_[1].score  = loster_score;
            game_score_[1].coins  = game_score_[0].coins/2;
            game_score_[1].exp    = game_score_[0].exp/2;
            game_score_[1].lovely = game_score_[0].lovely/2;
        }
        else
        {
            game_score_[0].coins  = 0;
            game_score_[0].exp    = 0;
            game_score_[0].lovely = 0;

            game_score_[1].coins  = 0;
            game_score_[1].exp    = 0;
            game_score_[1].lovely = 0;
        }
    }

}

/**
 * @brief  游戏超时计算各类玩家的分数
 * @param  void
 * @return void
 */
inline void Tetris::calc_timeout_score(void)
{
    //0 下标存的是赢家获得分数， 1下标存的是输家获得分数，

    //大家可能获得相同分数，但概率上先入游戏的分数高些
    calc_normal_score();
}

/**
 * @brief  游戏默认的（其它的）方式计算各类玩家的分数
 * @param  void
 * @return void
 */
inline void Tetris::calc_default_score(void)
{
    //0 下标存的是赢家获得分数， 1下标存的是输家获得分数，

    //大家分数概率上相同
    calc_normal_score();
}

/**
 * @brief  给玩家分发分数，经验值
 * @param  void
 * @return void
 */
inline void Tetris::distribute_score(void)
{
//    DEBUG_LOG("distribute_score(), winner pos id: %d.", winning_player_pos_);
    if(game_group_->count > 1)
    {
        for(int i = 0; i < game_group_->count; ++i)
        {
            sprite_t* player = game_group_->players[i];
            int score_index = 0;
//            DEBUG_LOG("distribute_score(), player pos id: %d.", player->pos_id);
            if(player->pos_id == winning_player_pos_)
            {
                score_index = 0;  //赢家类型
                game_score_[score_index].rank = 1;
                //game_score_[score_index].itmid = get_fire_medal(player, 0);
            }
            else
            {
                score_index = 1;  //输家类型
                game_score_[score_index].rank = 2;
                //if(player->waitcmd != proto_player_leave)
                //    game_score_[score_index].itmid = get_fire_medal(player, 1);
            }

            pack_score_session(player, &(game_score_[score_index]), 34, game_score_[score_index].score);
            submit_game_score(player, &(game_score_[score_index]));
        }
    }
    else
    {
        sprite_t* player = game_group_->players[0];
//        DEBUG_LOG("distribute_score(), player id: %d.", player->id);
        pack_score_session(player, &(game_score_[0]), 33, game_score_[0].score);
        submit_game_score(player, &(game_score_[0]));
    }
}

