/**
 * =====================================================================================
 *       @file  tetris.hpp
 *      @brief  俄罗斯方块游戏的服务器端
 *
 *   @internal
 *     Created  2008年10月22日 13时52分04秒
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee .Inc
 *   Copyright  Copyright (c) 2008, aceway
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef TM_TETRIS_ACEWAY
#define TM_TETRIS_ACEWAY

#include <cstdlib>
#include <ant/inet/pdumanip.hpp>
#include <ant/random/random.hpp>
extern "C" {
#include <gameserv/proto.h>
}
#include <libtaomee/log.h>
#include "../mpog.hpp"

/**
 * @class Tetris
 * @brief 实现俄罗斯方块服务端。
 *        上层已经初始化了游戏组结构体，玩家结构体;
 *        类内部可以添加超时处理函数；
 *        类内部处理上层传递过来的协议级消息；
 */
class Tetris : public mpog{
     public:
         Tetris(game_group_t* grp);
         ~Tetris();
         void init(sprite_t* p);
         int  handle_data(sprite_t* player, int cmd, const uint8_t body[], int body_len);
         void destroy(void);

         int  handle_timeout(void* data);
     private:
         /** 为协议消息定义的枚举类型 */
         typedef enum __etetris_cmd_id{
             etetris_game_new_bout = 32000, /**< @brief 一个用户请求开始俄罗斯方块游戏 */
             etetris_trans_data    = 32001, /**< @brief 用户请求转发数据 */
             etetris_game_end_bout = 32002 /**< @brief 用户发送过来的俄罗斯方块游戏结束 */
         }etetris_cmd_id;

         /** 为计算分数定义的枚举类型 */
         typedef enum __escore_factor{
             escore_normal = 0, /**< @brief 正常玩完游戏，分出胜负 */
             escore_timeout /**< @brief 超时，没有分出胜负 */
         }escore_factor;

         /** 为记录每个用户的session信息定义的结构体 */
         typedef struct __suser_session{
             uint32_t user_id; /**< @brief 次session号所属用户的ID，即其米米号 */
             uint32_t session_id; /**< @brief sesseion号 */
         }suser_session;

         inline int game_player_leave(sprite_t* player, const uint8_t body[], int body_len);

         inline int tetris_new_bout(sprite_t* plyaer, const uint8_t body[], int body_len);
         inline int tetris_trans_data(sprite_t* player, const uint8_t body[], int body_len);
         inline int tetris_end_bout(sprite_t* player, const uint8_t body[], int body_len);

         inline void notify_player_game_ready(void);
         inline void notify_player_win(sprite_t* winner_player);

         inline bool check_player_session(sprite_t* player, const uint8_t body[], int body_len);
         inline bool get_client_submit_result(sprite_t* player, const uint8_t body[], int body_len);

         inline void calculate_score(escore_factor efactor);
         inline void calc_normal_score(void); /**< @brief  正常玩完游戏,分出胜负，计算分数 */
         inline void calc_timeout_score(void); /**< @brief  游戏超时，未分出胜负，计算分数 */
         inline void calc_default_score(void); /**< @brief 其它情况时，计算分数 */
         inline void distribute_score(void); /**< @brief 给用户分发分数 */

     private:
         uint8_t winning_player_pos_; /**< @brief 赢家的pos_id */
         //0 for players belong to player 2, 1 for players belong to player 1
         game_group_t* game_group_; /**< @brief 构造函数参数中记录的游戏组指针 */
         game_score_t game_score_[2]; /**< @brief 分数的种类, 目前两种：赢家，输家, 看客不给分数 */
         //0 下标存的是赢家获得分数， 1下标存的是输家获得分数，
         //3 下标存的是平手获得的分数，4下标是看家获得的分数......

         suser_session game_sessions_[2]; /**< @brief 记录游戏玩家每次的session号 */
         bool bready_to_transdata;   /**< @brief 判断玩家是否都准备好了可以接收转发数据 */
         uint32_t  uiplayer_ready_count;
         bool bhas_end_game;
         int32_t  tetris_start_time;
 };
#endif //TM_TETRIS_ACEWAY
