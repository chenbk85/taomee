/** 
 * ========================================================================
 * @file npc_shop.hpp
 * @brief 
 * @version 1.0
 * @date 2012-05-28
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_NPC_SHOP_H_2012_05_28
#define H_NPC_SHOP_H_2012_05_28


#include "pea_common.hpp"
#include "proto.hpp"



#define MAX_NPC_SHOP_ITEM_COUNT     100


struct npc_shop_t
{
    npc_shop_t()
    {
        items.clear();
    }

    std::vector<uint32_t> items;
};


class c_npc_shop_data_mgr : public c_object_container<uint32_t, npc_shop_t>
{

};


typedef singleton_default<c_npc_shop_data_mgr> npc_shop_data_mgr;


int init_npc_shop_config_data(xmlNodePtr root);



#endif
