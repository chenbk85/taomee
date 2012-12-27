/**
 * =====================================================================================
 *       @file  player.hpp
 *      @brief  玩家信息类头文件
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  03/23/2009 11:08:16 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
 *
 *     @author  sylar (), sylar@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef PLAYER_HPP
#define PLAYER_HPP
extern "C" {
#include  <libtaomee/log.h>
#include  <gameserv/game.h>
}
#define ITEM_KIND_NUM 5
#define PLANE_START 77

enum _player{
    EMPTY   = 0xFF,
    Player1 = 0,
    Player2 = 1,
    Player3 = 2,
    Player4 = 3,
};

enum _itm{
    item_none,
    item_shield,
    item_get_six,
    item_twice_roll,
    item_block,
    item_attack,
};

typedef enum _status{
    status_none,
    status_shield,
    status_get_six,
    status_last_roll,
    status_twice_roll,
    status_twice_action,
    status_normal_roll,
    status_blocked,
}status_t;

enum _weather{
    weather_none,
    rain,
    storm,
};

enum grid_status{
    grid_free,
    grid_unready,
    grid_shielded,
    grid_finished,
};

typedef struct _grid
{
    int item;
    int plane_sn[4];
    int cnt;
    uint32_t owner;
    int status;
    int weather;
    _grid() {
        item = item_none;
        weather = weather_none;
        status = grid_free;
        cnt = 0;
        owner = EMPTY;
        plane_sn[0] = EMPTY;
        plane_sn[1] = EMPTY;
        plane_sn[2] = EMPTY;
        plane_sn[3] = EMPTY;
    }
}grid_t;
class Player
{
    public:
        Player();
        void setup(sprite_t *player, int pos);
        int get_pos() { return m_pos;}
        const uint32_t id() { return p->id;}
        int use_power();
        //frozen
        bool is_block() { return (status == status_blocked)?true:false; }

        void set_can_continue() { is_continue = 1;}
        void set_discontinue() { is_continue = 0;}
        bool can_continue();

        //status
        void set_status(status_t status_now) { status = status_now;
        //    DEBUG_LOG("SET STATUS %d %d", p->id, status_now);
        }
        void add_item(int item_id) { item_cnt[item_id - 1]++; }
        int use_item(int item_id);

        bool bout_finished();
        int plane_finished_num() { return plane_finished_cnt; }
        bool no_plane_can_move();
        bool can_roll_again() { return can_roll_more; }

        void plane_finished() {
            plane_finished_cnt++;
            //DEBUG_LOG("plane finished [%d %d]", p->id, plane_finished_cnt);
        }
        void plane_move(int plane_sn, int pos) { plane_pos[plane_sn] = pos; }
        void set_can_use_item() { can_use_item = true;}
        int get_plane_pos(int sn) { return plane_pos[sn];}
        sprite_t* get_sprite() {return p;}
        void reset();
        //roll
        int roll();
    private:
        sprite_t *p;
        int m_pos; //0-3
        int item_cnt[ITEM_KIND_NUM];
        int plane_pos[4];
        int plane_finished_cnt;
        int roll_point;
        status_t status;
        int is_continue;
        bool can_use_item;
        bool can_roll_more;
};
#endif
