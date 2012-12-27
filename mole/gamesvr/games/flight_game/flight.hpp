/**
 * =====================================================================================
 *       @file  flight.h
 *      @brief  飞行棋头文件
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  03/19/2009 05:21:59 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
 *
 *     @author  sylar (), sylar@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef FLIGHT_GAME_H
#define FLIGHT_GAME_H
extern "C" {
#include  <libtaomee/log.h>
#include  <gameserv/game.h>
}
#include <vector>
#include "player.hpp"
#include "../mpog.hpp"
#define GRIDS_TOTAL         97
#define GRID_AIRSHED        77
#define GRID_AIRPORT_START  81
#define GRID_BIG_FLY_START  18
#define GRID_BIG_FLY_FIN    30
#define GRID_LANDING        50
#define GRID_RUNWAY_START   53
#define GRID_RUNWAY_FIN     58
#define GRID_START          1
#define AIRLINE_LOOP_GRIDS  52
#define OFFSET_NORMAL(pos) ((pos)*13)
#define OFFSET_AIRPORT(pos) ((pos)*5)
#define OFFSET_RUNWAY(pos) ((pos)*6)
#define PATH_POINT_END      0xFF
#define NO_DATA             0xFF
#define PATH_MSG_HEAD_LEN   15
#define ITEM_ATK_MSG_HEAD   16
#define PLANE_NUM_CAN_WIN   4

enum _is_all_move {
    single_move,
    all_move,
};

enum _cmd {
    GAME_READY  = 33000,
    ROLL        = 33001,
    ITEM        = 33002,
    MOVE        = 33003,
    GAME_WIN    = 33004,
    GAME_SYN    = 33005,
    MAP_UPDATE  = 33006,
};

enum event_t {
    event_none,
    event_wait,
    event_start,
    event_finished,
    event_kill,
    event_normal_fly_start,
    event_normal_fly_end,
    event_big_fly_start,
    event_big_fly_end,
    event_rain,
    event_storm,
    event_add_item,
    event_overlap,
};
typedef struct _weather_info
{
    int type;
    int grid_id;
    _weather_info(){
        type = weather_none;
        grid_id = EMPTY;
    }
}weather_info_t;

typedef struct _item_info
{
    int type;
    int grid_id;
    _item_info(){
        type = item_none;
        grid_id = EMPTY;
    }
}item_info_t;

typedef struct _path_info
{
    int grid_id;
    int event_cnt;
    int event_id[10];
    _path_info(){
        grid_id = 0;
        event_cnt = 0;
    }
}path_info_t;

/**
 * @class Flight
 * @brief 处理各种游戏信息
 */
class Flight: public mpog
{
    public:
        Flight(game_group_t *grp);

		void init(sprite_t* p) {
        }
		int  handle_db_return(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code) {
            return 0;
        }
		int  handle_timeout(void* data) {
            return 0;
        }
		int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);

    private:
        int handle_action(int grid_id, int steps);
        int handle_item(int item_id, int grid_id);

        bool is_my_turn(sprite_t *p);

        void grid_move(int target, int start);
        int grid_attack(int target);
        void grid_clear(int target);
        void grid_reset(int target);


        void notify_roll_point(int point, uint32_t uid);
        void notify_use_item(uint32_t owner, int item_id, uint32_t target, int grid_id);
        void notify_player_action(uint32_t uid);
        void notify_player_win(sprite_t *p);
        void notify_game_start();
        void notify_game_syn(uint32_t uid);

        void notify_player_leave(uint32_t uid, uint32_t next_uid);
        void notify_player_end(sprite_t *p);
        void map_update();
        void game_update();
        void delete_player(sprite_t *p);

        game_group_t *m_grp;
        int owner;
        int players_cnt;
        int bout_cnt;
        weather_info_t m_weather;
        item_info_t m_item[3];
        Player m_player[4];
        std::vector<Player> player;
        // map[0] no used;
        grid_t map[GRIDS_TOTAL];
        path_info_t path[16];
        int path_cnt;
        int msg_len;
        int winner_cnt;
        bool is_end;
        bool is_all_move;
        static int bonus_point[AIRLINE_LOOP_GRIDS + 1];
};

#endif
