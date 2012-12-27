/**
 *============================================================
 *  @file      master_prentice.hpp
 *  @brief    common operations such as walk, talk...
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef __KF_MASTER_PRENTICE_HPP__
#define __KF_MASTER_PRENTICE_HPP__

#include <libtaomee++/inet/pdumanip.hpp>

//#include "cli_proto.hpp"
//#include "fwd_decl.hpp"
extern "C" {
#include <libtaomee/dataformatter/bin_str.h>
}

#include <vector>

using namespace std;

#pragma pack(1)

struct mp_master_t {
    uint32_t    master_id;
    uint32_t    master_tm;
	uint32_t 	join_tm;
};

struct mp_prentice_t {
    uint32_t    uid;
    uint32_t    roletm;
    uint32_t    lv;
    uint32_t    tm;
    uint32_t    grade;
};

//typedef vector<mp_prentice_t> mp_vector;

struct db_get_master_t {
    uint32_t    cnt;
    mp_master_t master[];
};

struct db_get_prentice_t {
    uint32_t    cnt;
    mp_prentice_t prentice[];
};


struct db_master_add_prentice_t {
    uint32_t    action; //0 fail
    mp_prentice_t prentice;
};

struct db_master_del_prentice_t {
    uint32_t    prentice_id;
    uint32_t    prentice_tm;
};

#pragma pack()

//---------------------inline ----------------
//----------------------------------------------------------------
//----------------------------------------------------------------

int invite_prentice_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
int reply_invite_prentice_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int kick_prentice_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int get_master_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int get_prentice_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

//-------------------------------------------------------------------------------
// Callbacks: process package from db and send package to client
//-------------------------------------------------------------------------------
/**
  * @brief callback for handling friend adding action returned from dbproxy
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */  
int db_master_add_prentice_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int db_get_master_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
int db_get_prentice_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

//---------------------------------------------------------------
// little function 
//---------------------------------------------------------------
bool is_player_have_master(player_t* p);

int db_master_add_prentice(player_t* prentice, uint32_t inviter, uint32_t role_tm);

int db_set_prentice_grade(player_t* prentice);

int db_prentice_graduate(player_t* prentice);

bool is_can_get_prentice_reward(player_t* master, uint32_t prentice_id, uint32_t prentice_tm, uint32_t reward_id);

void db_master_set_prentice_grade(player_t* master, uint32_t prentice_id, uint32_t prentice_tm, uint32_t reward_id);

#endif
