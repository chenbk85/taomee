/** 
 * ========================================================================
 * @file db_pet.cpp
 * @brief 
 * @version 1.0
 * @date 2012-04-11
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include "db_pet.hpp"
#include "pet.hpp"
#include "player.hpp"
#include "cli_proto.hpp"
#include "map.hpp"
#include "login.hpp"
#include "player_attribute.hpp"


int db_get_pet(player_t * p)
{
    db_proto_get_pet_in in;
    p->export_db_user_id(&in.db_user_id);

    return send_to_db(p, db_proto_get_pet_cmd, &in);
}



int db_proto_get_pet_callback(DEFAULT_ARG)
{
    db_proto_get_pet_out * p_in = P_IN;

    // uint32_t fight_pet_index = 0;
    for (uint32_t i = 0; i < p_in->db_pet_info.size(); )
    {
        db_pet_info_t * p_db_pet_info = &(p_in->db_pet_info[i]);
        // uint32_t pet_no = p_db_pet_info->pet_no;
        // uint32_t pet_id = p_db_pet_info->pet_id;
        // uint32_t level = p_db_pet_info->level;

        c_pet * p_pet = new c_pet(p);
        if (NULL == p_pet)
        {
            send_error_to_player(p, ONLINE_ERR_SYSTEM_FAULT);
            return 0;
        }


        if (0 != p_pet->init(p_db_pet_info))
        {
            send_error_to_player(p, ONLINE_ERR_DB);
            delete p_pet;
            return 0;
        }

        if (p_db_pet_info->status == PET_STATUS_FIGHT)
        {
            // fight_pet_index = i;
            set_fight_pet(p, p_pet);
        }
        else if (p_db_pet_info->status == PET_STATUS_ASSIST)
        {
            add_assist_pet(p, p_pet);

        }

        add_player_pet(p, p_pet);
        i++;
    }

    calc_assist_attr(p);


    p->set_module(MODULE_PET);

    // byte_array_t ba;

    // ba.write_uint32(fight_pet_index);
    // ba.write_uint32(p_in->db_pet_info.size());
    // for (uint32_t i = 0; i < p_in->db_pet_info.size(); i++)
    // {
        // p_in->db_pet_info[i].write_to_buf(ba);
    // }

    // cli_proto_get_bag_pet_out out;
    // out.read_from_buf_ex(ba);

    
	return process_login(p);
}



int db_set_fight_pet(player_t * p, uint32_t des_pet_no, uint32_t src_pet_no)
{
    db_proto_set_pet_status_in in;

    p->export_db_user_id(&in.db_user_id);
    db_change_pet_status_t change;
    change.status = PET_STATUS_FIGHT;
    change.src_pet_no = src_pet_no;
    change.des_pet_no = des_pet_no;

    in.change_list.push_back(change);

    return send_to_db(p, db_proto_set_pet_status_cmd, &in);
}


int db_set_assist_pet(player_t * p, uint32_t des_pet_no, uint32_t src_pet_no)
{
    db_proto_set_pet_status_in in;
    p->export_db_user_id(&in.db_user_id);

    db_change_pet_status_t change;
    if (is_fight_pet(p, des_pet_no))
    {
        // 主、辅对换
        if (0 == src_pet_no)
        {
            // 主精灵不能为空
            ERROR_TLOG("user: %u, new fight pet_no: %u", p->id, src_pet_no);
            return send_error_to_player(p, ONLINE_ERR_PET_NO);
        }

        change.status = PET_STATUS_FIGHT;
        change.src_pet_no = des_pet_no;
        change.des_pet_no = src_pet_no;
        in.change_list.push_back(change);

        change.status = PET_STATUS_ASSIST;
        change.src_pet_no = 0;
        change.des_pet_no = des_pet_no;
        in.change_list.push_back(change);
    }
    else
    {
        // 跟背包里的换
        change.status = PET_STATUS_ASSIST;
        change.src_pet_no = src_pet_no;
        change.des_pet_no = des_pet_no;
        in.change_list.push_back(change);
    }





    return send_to_db(p, db_proto_set_pet_status_cmd, &in);

}

int db_proto_set_pet_status_callback(DEFAULT_ARG)
{

    INFO_TLOG("user: %u, [set pet status] enter", p->id);
    print_pet_log(p);

    db_proto_set_pet_status_out * p_in = P_IN;


    for (uint32_t i = 0; i < p_in->change_list.size(); i++)
    {
        db_change_pet_status_t * p_change = &(p_in->change_list[i]);
        if (PET_STATUS_FIGHT == p_change->status)
        {
            c_pet * p_src_pet = find_player_pet(p, p_change->src_pet_no);
            if (NULL != p_src_pet)
            {
                p_src_pet->m_status = PET_STATUS_BAG;
            }
            c_pet * p_des_pet = find_player_pet(p, p_change->des_pet_no);
            if (NULL != p_src_pet)
            {
                p_des_pet->m_status = PET_STATUS_FIGHT;
                del_assist_pet(p, p_des_pet);
				set_fight_pet(p, p_des_pet);
            }

            p->clear_waitcmd();

            broadcast_player_fight_pet_change(p);
        }
        else if (PET_STATUS_ASSIST == p_change->status)
        {
            c_pet * p_src_pet = find_player_pet(p, p_change->src_pet_no);
            if (NULL != p_src_pet)
            {
                del_assist_pet(p, p_src_pet);
                p_src_pet->m_status = PET_STATUS_BAG;
            }

            c_pet * p_des_pet = find_player_pet(p, p_change->des_pet_no);
            if (NULL != p_des_pet)
            {
                add_assist_pet(p, p_des_pet);
                p_des_pet->m_status = PET_STATUS_ASSIST;
            }

            cli_proto_set_assist_pet_out out;
            pack_assist_pet_info(p, &out);

            send_to_player(p, &out, cli_proto_set_assist_pet_cmd, 1);
        }

    }


    INFO_TLOG("user: %u, [set pet status] leave", p->id);
    print_pet_log(p);

    return 0;
}


int db_set_pet_iq(player_t * p, uint32_t pet_no, uint32_t iq, pet_iq_data_t * p_data)
{
    db_proto_set_pet_iq_in in;
    p->export_db_user_id(&in.db_user_id);

    in.pet_no = pet_no;
    in.iq = iq;

    if (NULL != p_data)
    {
        if (!parse_db_del_item_request(p, p_data->item_id, p_data->item_num, in.del_items))
        {
            send_error_to_player(p, ONLINE_ERR_ITEM_LACK);
            return -1;
        }
    }

    return send_to_db(p, db_proto_set_pet_iq_cmd, &in);
}



int db_proto_set_pet_iq_callback(DEFAULT_ARG)
{
    db_proto_set_pet_iq_out * p_in = P_IN;

    uint32_t pet_no = p_in->pet_no;

    c_pet * p_pet = find_player_pet(p, pet_no);
    if (NULL == p_pet)
    {
        return -1;
    }

    // 扣减

    for (uint32_t i = 0; i < p_in->del_items.size(); i++)
    {
		db_del_item_reply_t* reply = &p_in->del_items[i];
		p->bag->del_item_by_grid(reply);
		notify_player_del_item(p, reply);
    }

    p_pet->m_iq = p_in->iq;

    // 回包
    cli_proto_upgrade_pet_iq_out * p_out = PROTO_OUT(p->bind_cmd->p_out);
    p_out->iq = p_pet->m_iq;
    p_pet->m_rand_calc->get_max_value_list(p_pet->m_iq, p_out->rand_attr_max);


    return send_to_player(p, p_out, p->waitcmd, 1);
}


int db_try_pet_train(player_t * p, uint32_t pet_no, uint32_t value_count, uint32_t * train_value, pet_train_consume_data_t * p_consume_data)
{
    db_proto_try_pet_train_in in;

    p->export_db_user_id(&in.db_user_id);

    in.pet_no = pet_no;
    for (uint32_t i = 0; i < value_count; i++)
    {
        in.train_value[i] = train_value[i];
    }

    in.gold = p_consume_data->gold;

    if (NULL != p_consume_data)
    {
        if (!parse_db_del_item_request(p, p_consume_data->item_id, p_consume_data->item_num, in.del_items))
        {
            send_error_to_player(p, ONLINE_ERR_PET_TRAIN);
            return -1;
        }
    }

    return send_to_db(p, db_proto_try_pet_train_cmd, &in);
}

int db_proto_try_pet_train_callback(DEFAULT_ARG)
{
    db_proto_try_pet_train_out * p_in = P_IN;

    uint32_t pet_no = p_in->pet_no;

    c_pet * p_pet = find_player_pet(p, pet_no);
    if (NULL == p_pet)
    {
        return -1;
    }

    // 扣减
    p->add_player_attr(OBJ_ATTR_GOLD, -p_in->gold);
    notify_player_gold(p);

    for (uint32_t i = 0; i < p_in->del_items.size(); i++)
    {
		db_del_item_reply_t* reply = &p_in->del_items[i];
		p->bag->del_item_by_grid(reply);
		notify_player_del_item(p, reply);
    }


    // 回包
    cli_proto_try_pet_train_out * p_out = PROTO_OUT(p->bind_cmd->p_out);

    p_out->pet_no = pet_no;
    for (uint32_t i = 0; i < PET_TRAIN_ATTR_NUM; i++)
    {
        p_pet->m_try_train_value[i] = p_in->train_value[i];
        p_out->try_train_attr[i] = p_in->train_value[i];
    }


    return send_to_player(p, p_out, p->waitcmd, 1);

}


int db_set_pet_train(player_t * p, uint32_t pet_no, uint32_t set_flag)
{
    db_proto_set_pet_train_in in;
    p->export_db_user_id(&in.db_user_id);
    in.pet_no = pet_no;
    in.set_flag = set_flag;

    return send_to_db(p, db_proto_set_pet_train_cmd, &in);
}


int db_proto_set_pet_train_callback(DEFAULT_ARG)
{
    db_proto_set_pet_train_out * p_in = P_IN;

    uint32_t pet_no = p_in->pet_no;
    uint32_t set_flag = p_in->set_flag;

    c_pet * p_pet = find_player_pet(p, pet_no);
    if (NULL == p_pet)
    {
        return -1;
    }

    if (0 == set_flag)
    {
        p_pet->cancel_train_grow();
    }
    else 
    {
        p_pet->confirm_train_grow();
    }


    cli_proto_set_pet_train_out * p_out = PROTO_OUT(p->bind_cmd->p_out);
    p_out->pet_no = pet_no;
    p_out->set_flag = set_flag;
    for (uint32_t i = 0; i < PET_TRAIN_ATTR_NUM; i++)
    {
        p_out->train_attr[i] = p_pet->m_train_value[i];
    }

    return send_to_player(p, p_out, p->waitcmd, 1);
}


int db_update_pet_gift(player_t * p, uint32_t pet_no, pet_gift_consume_data_t * p_consume_data, db_extra_info_t * p_extra_info, std::vector<db_update_pet_gift_t> & update_gift)
{
    db_proto_update_pet_gift_in in;
    p->export_db_user_id(&in.db_user_id);
    in.pet_no = pet_no;
    if (NULL != p_consume_data)
    {
        if (!parse_db_del_item_request(p, p_consume_data->item_id, p_consume_data->item_num, in.del_items))
        {
            send_error_to_player(p, ONLINE_ERR_PET_GIFT);
            return -1;
        }
    }

    if (NULL != p_extra_info)
    {
        in.extra_info.push_back(*p_extra_info);
    }
    in.update_gift = update_gift;
    return send_to_db(p, db_proto_update_pet_gift_cmd, &in);
}


int db_proto_update_pet_gift_callback(DEFAULT_ARG)
{
    db_proto_update_pet_gift_out * p_in = P_IN;

    uint32_t pet_no = p_in->pet_no;


    c_pet * p_pet = find_player_pet(p, pet_no);
    if (NULL == p_pet)
    {
        return -1;
    }


    // 扣减
    for (uint32_t i = 0; i < p_in->del_items.size(); i++)
    {
		db_del_item_reply_t* reply = &p_in->del_items[i];
		p->bag->del_item_by_grid(reply);
		notify_player_del_item(p, reply);
    }

    // 扣减免费次数
    if (!p_in->extra_info.empty())
    {
        for (uint32_t i = 0; i < p_in->extra_info.size(); i++)
        {
            db_extra_info_t * p_extra = &p_in->extra_info[i];
            uint32_t * p_value = p->extra_info->id2p(p_extra->info_id);
            if (NULL != p_value)
            {
                *p_value = p_extra->info_value;
            }
            else
            {
                p_in->extra_info.erase(p_in->extra_info.begin() + i);
            }
        }
        notify_extra_info(p, p_in->extra_info);
    }

    // 更新天赋
    for (uint32_t i = 0; i < p_in->update_gift.size(); i++)
    {
        p_pet->m_gift.update(&p_in->update_gift[i]);

    }

    p_pet->calc_gift_attr();
    p_pet->calc_attr();

    // 回包

    if (p->waitcmd == cli_proto_rand_pet_gift_cmd)
    {
        cli_proto_rand_pet_gift_out * p_out = PROTO_OUT(p->bind_cmd->p_out);
        p_pet->export_info(&p_out->pet_info);
        // as要求返回为这个包
        return send_to_player(p, p_out, cli_proto_get_pet_detail_cmd, 1);
    }
    else if (p->waitcmd == cli_proto_upgrade_pet_gift_cmd)
    {
        cli_proto_upgrade_pet_gift_out * p_out = PROTO_OUT(p->bind_cmd->p_out);
        if (p_in->update_gift.size() > 0)
        {
            p_out->upgrade_gift_no = p_in->update_gift[0].gift_no;
        }
        p_pet->export_info(&p_out->pet_info);
        return send_to_player(p, p_out, p->waitcmd, 1);
    }

    return 0;
}



int db_pet_merge(
        player_t * p,
        uint32_t base_pet_no,
        uint32_t base_pet_level,
        uint32_t base_pet_exp,
        std::vector<uint32_t> & del_pet_no,
        std::vector<db_del_item_request_t> & del_items,
        std::vector<db_add_item_request_t> & add_items)
{
    db_proto_pet_merge_in in;

    p->export_db_user_id(&in.db_user_id);
    in.base_pet_no = base_pet_no;
    in.base_pet_level = base_pet_level;
    in.base_pet_exp = base_pet_exp;
    in.del_pet_no = del_pet_no;
    in.del_items = del_items;
    in.add_items = add_items;

    return send_to_db(p, db_proto_pet_merge_cmd, &in);
}


int db_proto_pet_merge_callback(DEFAULT_ARG)
{
    db_proto_pet_merge_out * p_in = P_IN;
    cli_proto_pet_merge_out * p_out = PROTO_OUT(p->bind_cmd->p_out);

    uint32_t base_pet_no = p_in->base_pet_no;
    uint32_t base_pet_level = p_in->base_pet_level;
    uint32_t base_pet_exp = p_in->base_pet_exp;


    // 扣减
    for (uint32_t i = 0; i < p_in->del_items.size(); i++)
    {
		db_del_item_reply_t* reply = &p_in->del_items[i];
		p->bag->del_item_by_grid(reply);
		notify_player_del_item(p, reply);
    }

    // 添加
    for (uint32_t i = 0; i < p_in->add_items.size(); i++)
    {
		db_add_item_reply_t* reply = &p_in->add_items[i];
		p->bag->add_item_by_grid(reply);
		notify_player_add_item(p, reply);


        // 解析到应答包的补偿物品
        uint8_t flag = 0;
        for (uint32_t j = 0; j < p_out->compensate_items.size(); j++)
        {
            if (p_out->compensate_items[i].item_id == reply->item_id)
            {
                p_out->compensate_items[i].item_count += reply->item_count;
                flag = 1;
                break;
            }
        }

        if (!flag)
        {
            simple_item_info_t item_info;
            item_info.item_id = reply->item_id;
            item_info.item_count = reply->item_count;
            p_out->compensate_items.push_back(item_info);
        }
    }

    // 参与幻化的精灵
    for (uint32_t i = 0; i < p_in->del_pet_no.size(); i++)
    {
        uint32_t del_pet_no = p_in->del_pet_no[i];
        c_pet * p_del_pet = find_player_pet(p, del_pet_no);
        if (NULL == p_del_pet)
        {
            continue;
        }


        if (is_assist_pet(p, del_pet_no))
        {
            // 参与幻化的是辅助精灵
            del_assist_pet(p, p_del_pet);
        
        }

        del_player_pet(p, p_del_pet);

        p_del_pet->uninit();
        delete p_del_pet;

    }


    // 幻化的目标精灵
    c_pet * p_pet = find_player_pet(p, base_pet_no);
    if (NULL == p_pet)
    {
        ERROR_TLOG("user: %u, merge target pet %u not found", p->id, base_pet_no);
        send_error_to_player(p, ONLINE_ERR_PET_NOT_FOUND);
        return 0;
    }




    if (base_pet_level == p_pet->m_level)
    {
        // 幻化失败
        p_out->flag = 0;
        p_pet->export_info(&p_out->pet_info);

    }
    else
    {
        // 幻化成功
        p_pet->m_exp = base_pet_exp;
        p_pet->level_up(base_pet_level);

        p_out->flag = 1;
        p_pet->export_info(&p_out->pet_info);
    }

    return send_to_player(p, p_out, p->waitcmd, 1);
}
