/** 
 * ========================================================================
 * @file npc_shop.cpp
 * @brief 
 * @author TAOMEE
 * @version 1.0
 * @date 2012-05-28
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */



#include "npc_shop.hpp"
#include "player.hpp"
#include "item_bag.hpp"
#include "cli_proto.hpp"




using namespace std;
using namespace taomee;



int init_npc_shop_config_data(xmlNodePtr root)
{
    xmlnode_for_each(root, node_1)
    {
        if (is_xmlnode_name(node_1, "shop"))
        {
            uint32_t shop_id = 0;
            get_xml_prop_def(shop_id, node_1, "id", 0);

            npc_shop_t * p_data = new npc_shop_t;
            if (NULL == p_data)
            {
                return -1;
            }

            uint32_t item_id[MAX_NPC_SHOP_ITEM_COUNT] = {0};
            size_t count = get_xml_prop_arr_def(item_id, node_1, "items", 0);

            for (uint32_t i = 0; i < count; i++)
            {
                // 过滤掉不存在item_data的item_id
                item_data * p_item_data = item_data_mgr::get_instance()->get_item_data_by_id(item_id[i]);
                if (NULL == p_item_data)
                {
                    continue;
                }

                p_data->items.push_back(item_id[i]);
            }

            npc_shop_data_mgr::instance().insert_object(shop_id, p_data);

        }

    }
    return 0;
}



int cli_proto_get_npc_shop(DEFAULT_ARG)
{
    cli_proto_get_npc_shop_in * p_in = P_IN;
    cli_proto_get_npc_shop_out * p_out = P_OUT;

    uint32_t shop_id = p_in->shop_id;

    npc_shop_t * p_data = npc_shop_data_mgr::instance().get_object(shop_id);
    if (NULL == p_data)
    {
        ERROR_TLOG("user: %u, shop %u not found", p->id, shop_id);
        send_error_to_player(p, ONLINE_ERR_XML);
        return 0;
    }


    vector_for_each(p_data->items, it)
    {
        uint32_t item_id = *it;


        item_data * p_item_data = item_data_mgr::get_instance()->get_item_data_by_id(item_id);
        if (NULL == p_item_data)
        {
            continue;
        }

        npc_shop_item_info_t info;
        info.item_id = item_id;
        info.buy_price = p_item_data->buy_price_;

        p_out->npc_shop_items.push_back(info);

    }

    return send_to_player(p, p_out, p->waitcmd, 1);
}


int cli_proto_buy_npc_shop_item(DEFAULT_ARG)
{
    if (!p->check_module(MODULE_BAG))
    {

        send_error_to_player(p, ONLINE_ERR_MODULE);
        return 0;
    }

    cli_proto_buy_npc_shop_item_in * p_in = P_IN;
    uint32_t shop_id = p_in->shop_id;
    uint32_t item_id = p_in->item_id;
    uint32_t item_count = p_in->item_count;

    npc_shop_t * p_data = npc_shop_data_mgr::instance().get_object(shop_id);
    if (NULL == p_data)
    {
        ERROR_TLOG("user: %u, shop %u not found", p->id, shop_id);
        send_error_to_player(p, ONLINE_ERR_XML);
        return -1;
    }


    // 先判断商店里面有木有这个东西在卖
    vector_for_each(p_data->items, it)
    {
        if (item_id == *it)
        {
            // 在卖
            item_data * p_item_data = item_data_mgr::get_instance()->get_item_data_by_id(item_id);
            if (NULL == p_item_data)
            {
                ERROR_TLOG("user: %u, item data %u not found", p->id, item_id);
                send_error_to_player(p, ONLINE_ERR_XML);
                return 0;
            }

            if (p->get_player_attr_value(OBJ_ATTR_GOLD) < p_item_data->buy_price_)
            {
                ERROR_TLOG("user: %u, gold %u < %u", p->id, p->get_player_attr_value(OBJ_ATTR_GOLD), p_item_data->buy_price_);
                send_error_to_player(p, ONLINE_ERR_ITEM_LACK);
                return 0;
            }


            db_proto_buy_npc_shop_item_in out;
            p->export_db_user_id(&out.db_user_id);
            if (!parse_db_add_item_request(p, item_id, item_count, 0, out.add_items))
            {
                return send_error_to_player(p, ONLINE_ERR_BAG_FULL);
            }

            out.gold = p_item_data->buy_price_ * item_count;

            return send_to_db(p, db_proto_buy_npc_shop_item_cmd, &out);
        }

    }


    return send_error_to_player(p, ONLINE_ERR_INVALID_PARA);
}


int cli_proto_npc_item(DEFAULT_ARG)
{
    if (!p->check_module(MODULE_BAG))
    {

        send_error_to_player(p, ONLINE_ERR_MODULE);
        return 0;
    }
    cli_proto_npc_item_in * p_in = P_IN;
    uint32_t grid_index = p_in->grid_index;
    uint32_t item_count = p_in->item_count;

    item_ptr p_item = p->bag->get_item_by_grid(grid_index);
    if (NULL == p_item)
    {
        ERROR_TLOG("user: %u, item not found, grid_index: %u", p->id, grid_index);
        send_error_to_player(p, ONLINE_ERR_INVALID_PARA);
        return 0;
    }
    
    // TODO 判断这个物品是否能卖

    if (item_count > p_item->get_cur_heap_count())
    {
        ERROR_TLOG("user: %u, npc item %u, heap count %u < %u", p->id, p_item->item_id(), p_item->get_cur_heap_count(), item_count);
        send_error_to_player(p, ONLINE_ERR_INVALID_PARA);
        return 0;
    }

    db_proto_npc_item_in out;
    p->export_db_user_id(&out.db_user_id);
    // 不能用parse_db_del_item_request
    // 因为要求删指定grid_index的item
    db_del_item_request_t req;
    req.grid_index = grid_index;
    req.item_id = p_item->item_id();
    req.item_count = item_count;
    out.del_items.push_back(req);

    out.gold = p_item->get_item_data()->sell_price_ * item_count;

    return send_to_db(p, db_proto_npc_item_cmd, &out);
}



int cli_proto_get_redeem_shop(DEFAULT_ARG)
{
    if (!p->check_module(MODULE_BAG))
    {

        send_error_to_player(p, ONLINE_ERR_MODULE);
        return 0;
    }

    cli_proto_get_redeem_shop_out * p_out = P_OUT;

    vector_for_each(p->redeem_bag->item_grid, it)
    {
        item_ptr p_item = *it;
        redeem_shop_item_info_t info;
        info.item_id = p_item->item_id();
        info.item_count = p_item->get_cur_heap_count();
        info.buy_price = p_item->get_item_data()->sell_price_ * info.item_count;

        p_out->redeem_shop_items.push_back(info);
    }
    return send_to_player(p, p_out, p->waitcmd, 1);
}



int cli_proto_redeem_item(DEFAULT_ARG)
{
    if (!p->check_module(MODULE_BAG))
    {

        send_error_to_player(p, ONLINE_ERR_MODULE);
        return 0;
    }
    cli_proto_redeem_item_in * p_in = P_IN;
    uint32_t redeem_index = p_in->redeem_index;


    if (redeem_index >= p->redeem_bag->get_grid_count())
    {
        send_error_to_player(p, ONLINE_ERR_INVALID_PARA);
        return 0;
    }

    item_ptr p_item = p->redeem_bag->get_item(redeem_index);
    uint32_t item_id = p_item->item_id();
    uint32_t item_count = p_item->get_cur_heap_count();
    uint32_t price = p_item->get_item_data()->sell_price_ * item_count;

    if (p->get_player_attr_value(OBJ_ATTR_GOLD) < price)
    {
        ERROR_TLOG("user: %u, gold %u < %u", p->id, p->get_player_attr_value(OBJ_ATTR_GOLD), price);
        send_error_to_player(p, ONLINE_ERR_ITEM_LACK);
        return 0;
    }

    db_proto_redeem_item_in out;
    p->export_db_user_id(&out.db_user_id);
    if (!parse_db_add_item_request(p, item_id, item_count, p_item->get_expire_time(), out.add_items))
    {
        return send_error_to_player(p, ONLINE_ERR_BAG_FULL);
    }

    out.gold = price;
    out.redeem_index = redeem_index;

    return send_to_db(p, db_proto_redeem_item_cmd, &out);

}
