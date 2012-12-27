/**
 *============================================================
 *  @file      item.hpp
 *  @brief    item related functions are declared here.
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFTRD_ITEM_HPP_
#define KFTRD_ITEM_HPP_

#include "fwd_decl.hpp"
#include <kf/item_impl.hpp>
#include "cli_proto.hpp"


//--------------------------------------------------------------------------------
// struct
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// global 
//--------------------------------------------------------------------------------

/*! manage all kinds of items */
extern GfItemManager* items;

void save_sell_goods_log(Player* p, uint32_t shop_start_tm, uint32_t type, uint32_t itemid, uint32_t uniquekey, uint32_t cnt, uint32_t price);

#endif //KFTRD_ITEM_HPP_