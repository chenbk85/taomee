/**
 *============================================================
 *  @file      safe_trade.hpp
 *  @brief    declare class SafeTrade
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef __SAFE_TRADE_HPP__
#define __SAFE_TRADE_HPP__

#include <algorithm>
#include <list>
#include <map>
using namespace std;

//#include <boost/pool/object_pool.hpp>
//#include <libtaomee++/event/eventable_obj.hpp>
//#include <libtaomee++/event/event_mgr.hpp>

#include <libtaomee++/inet/byteswap.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>

extern "C" {
#include <assert.h>
#include <glib.h>

#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>

#include <async_serv/net_if.h>
}

//#include "mempool/mempool.hpp"
//#include "moving_object.hpp"

#include "utils.hpp"
#include "dbproxy.hpp"

extern uint8_t trpkgbuf[1<<21];
const uint16_t max_safe_trade_item_cnt = 10;                                               


#pragma pack(1) 

enum trade_step_t{
    step_enter_room = 1,
    step_set_item   = 2,
    step_agree_trade    = 3,
    step_delete_item    = 4,
    step_add_item       = 5,
    step_max_limit      = 6,
};

struct trade_item_t
{
    uint32_t    item_id;
    uint32_t    itemcnt;
};
struct trade_attire_t
{
    uint32_t    attire_id;
    uint32_t    unique_id;
    uint32_t    item_lv;
};

struct trader_info_t
{                                                                                          
    trader_info_t() {
        userid = 0;
        role_tm = 0;
        trade_step = 0;
        xiaomee = 0;
        item_cnt = 0;
        memset(&fdsess, 0x00, sizeof(fdsession_t));
        memset(item_array, 0x00, max_safe_trade_item_cnt * sizeof(trade_item_t));
        memset(attire_array, 0x00, max_safe_trade_item_cnt * sizeof(trade_attire_t));
    }
    uint32_t    userid;
    uint32_t    role_tm;
    fdsession_t fdsess;
    uint32_t    trade_step; //1-->进入交易房间 2-->确定 3-->同意 4-->删除物品完成
    uint32_t    xiaomee; 
    uint32_t    item_cnt;
    trade_item_t item_array[max_safe_trade_item_cnt];
    uint32_t    attire_cnt;
    trade_attire_t attire_array[max_safe_trade_item_cnt];
}; 


struct trade_room_t 
{
    trade_room_t() {
        trade_id = 0;
        for (uint32_t i = 0; i < 2; i++) {
            trader_info_t player;
            trader[i] = player;
        }
    }
    uint32_t    trade_id; 
    trader_info_t trader[2];   
};                                                                                         

struct set_item_t {
    uint32_t    room_id;
    uint32_t    userid;
    uint32_t    role_tm;
    uint32_t    action;
    uint32_t    xiaomee;
    uint32_t    itemcnt;
    uint32_t    attirecnt;
    uint8_t     items[];
};


struct db_safe_trade_item_t {
    uint32_t    trade_id;
    uint32_t    action;
    uint32_t    xiaomee;
    uint32_t    item_cnt;
    uint32_t    attire_cnt;
    uint8_t     items[];
};

#pragma pack()                                                                    

                                                              
class SafeTrade {

public:
    SafeTrade() 
    {
        init();
    }


    ~SafeTrade() 
    {
        final();
    }

public:

    bool init() 
    {
        for (uint32_t i = 1; i < 0x000fffe ; i++) {
            trade_id_list_.push_back(i);
        }
        TRACE_LOG("init trade_id_list size:%u", (uint32_t)trade_id_list_.size());
        return true;
    }
    bool final()
    {
        trade_id_list_.clear();
        trade_map_.clear();
        return true;
    }

    trade_room_t* get_trade_room_info(uint32_t room_id)
    { 		
        std::map<uint32_t, trade_room_t>::iterator it = trade_map_.find(room_id);
        if (it == trade_map_.end()) {
            TRACE_LOG("not found %u %u %u", get_server_id(), room_id, (uint32_t)trade_map_.size());
            return NULL;
        } 

        return &(it->second);
    }

    bool add_trade_room(trade_room_t* p_trade)
    { 
        trade_map_.insert(std::map<uint32_t, trade_room_t>::value_type(p_trade->trade_id, *p_trade)); 
        TRACE_LOG("add_trade_room %u %u %u", get_server_id(), p_trade->trade_id, (uint32_t)trade_map_.size());
        return true;
    }

    void del_trade_room(uint32_t room_id)
    { 
        TRACE_LOG("del_trade_room %u", room_id);
        std::map<uint32_t, trade_room_t>::iterator it = trade_map_.find(room_id);
        if (it != trade_map_.end()) {
            trade_map_.erase(it);
        }
    }

    void del_trade_room_by_fd(int fd)
    {
        TRACE_LOG("del_trade_room_by_fd %u", fd);
        std::map<uint32_t, trade_room_t>::iterator it = trade_map_.begin();
        uint32_t flag = 0;
        for (; it != trade_map_.end(); ) {
            flag = 0;
            for (uint32_t i = 0; i < 2; i++) {
                if (it->second.trader[i].fdsess.fd == fd) {
                    flag = 1;
                    break;
                }
            }

            if (flag) {
                TRACE_LOG("del_trade_room_by_fd %d %u", fd, it->first);
                release_trade_id(it->first);
                trade_map_.erase(it++);
            } else {
                ++it;
            }
        }
    }

    uint32_t produce_trade_id()
    {
        std::list<uint32_t>::iterator it = trade_id_list_.begin();
        if (trade_id_list_.end() == it) {
            return 0;
        }

        uint32_t id = trade_id_list_.front();

        trade_id_list_.pop_front();

        TRACE_LOG("produce trade id:%u ", id);

        return id;
    }

    void release_trade_id(uint32_t id)
    {
        trade_id_list_.push_back(id);
        TRACE_LOG("release trade id:%u", id);
    }

private:
private:
    std::list<uint32_t> trade_id_list_;
    std::map<uint32_t, trade_room_t> trade_map_;
};

SafeTrade* get_safe_trade_mgr();

void clear_safe_trade_room(int fd = -1);

void handle_safe_trade_dispatch(fdsession_t* fdsess, uint32_t cmd, uint8_t* body, uint32_t bodylen);

/**
 * @brief create safe trade room
 * @param
 * @return
 */
int safe_trade_create_room_cmd(fdsession_t* fdsess, uint8_t* body, uint32_t bodylen);


/**
 * @brief enter safe trade room
 * @param
 * @return
 */
int safe_trade_join_room_cmd(fdsession_t* fdsess, uint8_t* body, uint32_t bodylen);

/**
 * @brief cancel safe trade room
 * @param
 * @return
 */
int safe_trade_cancel_room_cmd(fdsession_t* fdsess, uint8_t* body, uint32_t bodylen);

int safe_trade_set_item_cmd(fdsession_t* fdsess, uint8_t* body, uint32_t bodylen);


int safe_trade_action_agree_cmd(fdsession_t* fdsess, uint8_t* body, uint32_t bodylen);


int handle_safe_trade_db_dispatch(db_proto_t* dbpkg, uint32_t pkglen);

/**
 * @brief 
 * @param action: 1-->del, 2-->add;
 */
int db_safe_trade_item(uint32_t trade_id, uint32_t userid, uint32_t role_tm, uint32_t action); 

/**
 * @brief player safe trade del/add db callback
 * @return 0 on success, -1 on error
 */
int db_safe_trade_item_callback(trader_info_t *trader, void* body, uint32_t bodylen, uint32_t ret);

#endif

