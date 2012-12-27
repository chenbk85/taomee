/*
 * =====================================================================================
 *
 *       Filename:  decorate.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/05/2011 14:13:55 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Saga (), saga@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#ifndef __DECORATE_HPP__
#define __DECORATE_HPP__
#include <string>
#include <map>

#include <libtaomee++/conf_parser/xmlparser.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>

#include "object.hpp"

using namespace taomee;

enum {
    decorate_snowman = 1,
    decorate_max_enum
};

#pragma pack(1)

struct decorate_t {
    uint32_t    id;
    uint32_t    lv;
    uint32_t    tm;
    uint32_t    status;
    //uint32_t    xpos;
    //uint32_t    ypos;
    decorate_t() {
        id  = 0;
        lv  = 0;
        tm  = 0;
        status = 0;
        //xpos    = 0;
        //ypos    = 0;
    }
};

struct db_get_decorate_t {
    uint32_t    cnt;
    decorate_t  decorate[];
};

struct db_add_decorate_lv_t {
    uint32_t    decorate_id;
    uint32_t    decorate_tm;
    uint32_t    decorate_lv;
    uint32_t    status;
    uint32_t    flag;
    uint32_t    itemcnt;
    uint32_t    itemarr[];
};

struct item_elem {
    uint32_t    itemid;
    uint32_t    itemcnt;
};

#pragma pack()

class HomeDecorate {
public:
    HomeDecorate();
    ~HomeDecorate();
public:
    decorate_t* get_decorate_by_id(uint32_t id) {
        std::map<uint32_t, decorate_t>::iterator it = decorate_map_.find(id);
        if (it != decorate_map_.end()) {
            return &(it->second);
        }
        return NULL;
    }

    void set_decorate_lv(uint32_t id, uint32_t lv) {
        std::map<uint32_t, decorate_t>::iterator it = decorate_map_.find(id);
        if (it != decorate_map_.end()) {
            it->second.lv = lv;
        }
    }

    void add_decorate_lv(uint32_t id, uint32_t add_lv) {
        std::map<uint32_t, decorate_t>::iterator it = decorate_map_.find(id);
        if (it != decorate_map_.end()) {
            it->second.lv += add_lv;
        }
    }

    uint32_t get_decorate_lv(uint32_t id) {
        std::map<uint32_t, decorate_t>::iterator it = decorate_map_.find(id);
        if (it != decorate_map_.end()) {
            return it->second.lv;
        }
        return 0;
    }

    void set_decorate_status(uint32_t id, uint32_t status) {
        std::map<uint32_t, decorate_t>::iterator it = decorate_map_.find(id);
        if (it != decorate_map_.end()) {
            it->second.status = status;
        }
    }

    uint32_t get_decorate_status(uint32_t id) {
        std::map<uint32_t, decorate_t>::iterator it = decorate_map_.find(id);
        if (it != decorate_map_.end()) {
            return it->second.status;
        }
        return 0;
    }
    void set_db_update_flag() {
        db_update_flag = true;
    }

    bool is_db_update() {
        return db_update_flag;
    }

    void add_decorate_to_map(decorate_t *pdata);

    int pack_all_decorate(void *buf);
private:
    std::map<uint32_t, decorate_t> decorate_map_;
    bool db_update_flag;
};



//-------------------------- CMD FUNCTION ------------------------------

int get_decorate_list_cmd(Player * p, uint8_t * body, uint32_t bodylen);
int set_decorate_cmd(Player * p, uint8_t * body, uint32_t bodylen);
int add_decorate_lv_cmd(Player * p, uint8_t * body, uint32_t bodylen);
int get_decorate_reward_cmd(Player * p, uint8_t * body, uint32_t bodylen);

//----------------------- CALLBACK FUNCTION ----------------------------

int db_get_decorate_list_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
int db_set_decorate_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
//

int db_get_decorate_list(Player* p, uint32_t uid, uint32_t utm);
int db_set_decorate(Player* p, uint32_t uid, uint32_t utm, uint32_t decorate_id);
int db_add_decorate_lv(Player* p, uint32_t decorate_id, uint32_t flag, uint32_t fillingin, uint32_t itemcnt, uint32_t *itemarr);



#endif



