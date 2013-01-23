/*
 * =====================================================================================
 *
 *
 *	   Filename:  route_func.cpp
 *
 *	Description:
 *
 *		Version:  1.0
 *		Created:  2009 
 *	   Revision:  none
 *	   Compiler:  gcc
 *
 *		 Author:  jim(jim), jim@taomee.com
 *		Company:  TAOMEE
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <string.h>
#include "Croute_func.hpp"
#include "benchapi.h"
#include "proto.hpp"
#include <limits.h>
#include <vector>
#include <libtaomee/time/time.h>
#include <libtaomee/bitmanip/bitmanip.h>
#include <libtaomee++/inet/pdumanip.hpp>



using namespace std;
using namespace taomee;


class Croute_func;

#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid, name, c_in, c_out, md5_tag, bind_bitmap)     \
    {cmdid, &Croute_func::name, new (c_in), new (c_out), md5_tag, bind_bitmap},

bind_proto_cmd_t g_db_cmd_list[] =
{
#include "proto/pea_db_bind.h"
#include "proto/pea_db_bind_online.h"
#include "proto/pea_db_bind_login.h"
#include "proto/pea_db_bind_battle.h"
};

#undef BIND_PROTO_CMD





// db的协议
c_cmd_map g_db_cmd_map(g_db_cmd_list, array_elem_num(g_db_cmd_list));

//----------------------------------------------------------------------



const bind_proto_cmd_t * find_db_cmd_bind(uint16_t cmd)
{
    return g_db_cmd_map.find_cmd(cmd);
}



/*
 *--------------------------------------------------------------------------------------
 *	   Class:  Croute_func
 *	  Method:  Croute_func
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */


Croute_func::Croute_func (mysql_interface * db) : Cfunc_route_base(db),
    m_cmd_map(g_db_cmd_list, array_elem_num(g_db_cmd_list)),
    handle_user(db), 
    handle_pet(db), 
    handle_pet_gift(db), 
    handle_item(db), 
    handle_mail(db),
    handle_extra_info(db),
	handle_friends(db),
	handle_task(db),
    handle_prize(db)
{
    db->set_is_log_debug_sql(config_get_intval("IS_LOG_DEBUG_SQL", 1));
    DEBUG_LOG("%lu", PROTO_HEADER_SIZE); 
    this->db=db;
}  


int Croute_func::db_proto_get_player(DEAL_FUNC_ARG )
{
    db_proto_get_player_in* p_in = P_IN;
    db_proto_get_player_out*  p_out = P_OUT;

    ret = handle_user.query_user_info(p_in->user_id, p_in->role_tm, p_in->server_id, p_out);
    if(ret != SUCC)
    {
        STD_ROLLBACK();
        return ret;
    }

	db_user_id_t db_user;
	db_user.user_id = p_in->user_id;
	db_user.role_tm = p_in->role_tm;
	db_user.server_id = p_in->server_id;
	msg_list_t tmp_msg_list;
	tmp_msg_list.msg_list.clear();
	handle_user.update_offline_msg(&db_user, &tmp_msg_list);
    return ret;
}

int Croute_func::db_proto_save_player(DEAL_FUNC_ARG)
{
    db_proto_save_player_in* p_in = P_IN;

    ret = handle_user.save_user_info(p_in->player_info.user_id, 
            p_in->player_info.role_tm, 
            p_in->player_info.server_id, 
            &p_in->player_info
            );
    if(ret != SUCC)
    {
        STD_ROLLBACK();
        return ret;
    }
    return ret;
}

int Croute_func::db_proto_get_role_list(DEAL_FUNC_ARG)
{
    db_proto_get_role_list_in * p_in = P_IN;
    db_proto_get_role_list_out * p_out = P_OUT;

    ret = handle_user.query_role_list(p_in->user_id, p_in->server_id, p_out->login_player);
    if (SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }

    for (uint32_t i = 0; i < p_out->login_player.size(); i++)
    {
        login_player_info_t * p_player = &(p_out->login_player[i]);
        db_user_id_t db_user;
        db_user.user_id = p_player->user_id;
        db_user.role_tm = p_player->role_tm;
        db_user.server_id = p_player->server_id;
        vector<db_equip_info_t> item_vec;

        handle_item.get_player_equips(&db_user, item_vec);

        for (uint32_t j = 0; j < item_vec.size(); j++)
        {
            simple_equip_info_t e;
            e.equip_id = item_vec[i].item_id;

            p_player->equip_info.push_back(e);
        }
    }

    return ret;
}


int Croute_func::db_proto_create_role(DEAL_FUNC_ARG)
{
    db_proto_create_role_in * p_in = P_IN;
    db_proto_create_role_out * p_out = P_OUT;

    std::vector<login_player_info_t> vec;
    ret = handle_user.query_role_list(p_in->user_id, p_in->server_id, vec);
    if (SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }

    if (!vec.empty())
    {
        return DB_ERR_EXIST;
    }

    uint32_t cur_time = time(NULL);
    p_out->db_user_id.server_id = p_in->server_id;
    p_out->db_user_id.user_id = p_in->user_id;
    p_out->db_user_id.role_tm = cur_time;

    ret = handle_user.create_role(&p_out->db_user_id, p_in->nick, &p_in->model);
    if (SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }

#ifdef CREATE_ROLE_WITH_DATA
    // 建立帐号时添加一些数据

    handle_pet.do_add_pet(&p_out->db_user_id, 10, p_in->nick, PET_STATUS_FIGHT);
    for (uint32_t i = 11; i < 18; i++)
    {
        handle_pet.do_add_pet(&p_out->db_user_id, i, p_in->nick, PET_STATUS_BAG);
    }

    db_add_item_request_t req;
    db_add_item_reply_t reply;
    req.get_time = cur_time;
    req.expire_time = 0;
    req.grid_index = 0;
    req.item_count = 999;

    req.grid_index++;
    req.item_id = 10029;
    handle_item.add_item(&p_out->db_user_id, &req, &reply);

    req.grid_index++;
    req.item_id = 10030;
    handle_item.add_item(&p_out->db_user_id, &req, &reply);

    req.grid_index++;
    req.item_id = 10031;
    handle_item.add_item(&p_out->db_user_id, &req, &reply);

    req.grid_index++;
    req.item_id = 10001;
    handle_item.add_item(&p_out->db_user_id, &req, &reply);

    req.item_count = 1;
    for (uint32_t i = 20007; i < 20018; i++)
    {
        req.grid_index++;
        req.item_id = i;
        handle_item.add_item(&p_out->db_user_id, &req, &reply);
    }

#endif


    return ret;
}

int Croute_func::db_proto_update_gold(DEAL_FUNC_ARG)
{
    db_proto_update_gold_in *p_in = P_IN;
    ret = handle_user.update_gold(&p_in->db_user_id, p_in->new_gold);
    if (SUCC != ret){
        STD_ROLLBACK();
        return ret;
    }
    return ret;
}

int Croute_func::db_proto_change_gold(DEAL_FUNC_ARG)
{
    db_proto_change_gold_in *p_in = P_IN;
    ret = handle_user.change_gold(&p_in->db_user_id, p_in->gold_change);
    if (SUCC != ret){
        STD_ROLLBACK();
        return ret;
    }
    return ret;
}

int Croute_func::db_proto_change_exp(DEAL_FUNC_ARG)
{
    db_proto_change_exp_in *p_in = P_IN;
    ret = handle_user.change_exp(&p_in->db_user_id, p_in->exp_change);
    if (SUCC != ret){
        STD_ROLLBACK();
        return ret;
    }
    return ret;
}

int Croute_func::db_proto_level_up(DEAL_FUNC_ARG)
{
    db_proto_level_up_in *p_in = P_IN;
    ret = handle_user.level_up(&p_in->db_user_id);
    if (SUCC != ret){
        STD_ROLLBACK();
        return ret;
    }
    return ret;
}

int Croute_func::db_proto_save_login_time(DEAL_FUNC_ARG)
{
    db_proto_save_login_time_in * p_in = P_IN;
    uint32_t last_login_tm = p_in->last_login_tm;
    uint32_t last_off_line_tm = p_in->last_off_line_tm;
    ret = handle_user.save_login_time(&p_in->db_user_id, last_login_tm, last_off_line_tm);
    if (SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }

    return ret;
}

int Croute_func::db_proto_get_pet(DEAL_FUNC_ARG)
{
    db_proto_get_pet_in * p_in = P_IN;
    db_proto_get_pet_out * p_out = P_OUT;

    p_out->init();

    db_user_id_t * db_user_id = &p_in->db_user_id;
    ret = handle_pet.query_pet(db_user_id, p_out->db_pet_info);
    if (SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }

    // 精灵天赋
    for (uint32_t i = 0; i < p_out->db_pet_info.size(); i++)
    {
        ret = handle_pet_gift.query_pet_gift(db_user_id, p_out->db_pet_info[i].pet_no, p_out->db_pet_info[i].gift);
        if (SUCC != ret)
        {
            STD_ROLLBACK();
            return ret;
        }
    }
    return ret;
}

int Croute_func::db_proto_get_fight_pet(DEAL_FUNC_ARG)
{
    ret = handle_pet.query_fight_pet(c_in, c_out);
    if (SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }
    return ret;
}

int Croute_func::db_proto_set_pet_status(DEAL_FUNC_ARG)
{
    db_proto_set_pet_status_in * p_in = P_IN;
    db_proto_set_pet_status_out * p_out = P_OUT;

    db_user_id_t * p_db_user_id = &(p_in->db_user_id);

    ret = SUCC;

    for (uint32_t i = 0; i < p_in->change_list.size(); i++)
    {
        db_change_pet_status_t * p_change = &(p_in->change_list[i]);
        if (PET_STATUS_FIGHT == p_change->status)
        {
            ret = handle_pet.set_fight_pet(p_db_user_id, p_change->des_pet_no, p_change->src_pet_no);

            if (SUCC != ret)
            {
                STD_ROLLBACK();
                return ret;

            }

        }
        else if (PET_STATUS_ASSIST == p_change->status)
        {

            ret = handle_pet.set_assist_pet(p_db_user_id, p_change->des_pet_no, p_change->src_pet_no);

            if (SUCC != ret)
            {
                STD_ROLLBACK();
                return ret;

            }


        }

        p_out->change_list.push_back(*p_change);
    }

    return ret;
}



int Croute_func::db_proto_set_pet_iq(DEAL_FUNC_ARG)
{
    db_proto_set_pet_iq_in * p_in = P_IN;
    db_proto_set_pet_iq_out * p_out = P_OUT;

    db_user_id_t * db_user_id = &p_in->db_user_id;

    uint32_t pet_no = p_in->pet_no;
    uint32_t iq = p_in->iq;

    // items
    for (uint32_t i = 0; i < p_in->del_items.size(); i++)
    {
        db_del_item_request_t * del_info = &p_in->del_items[i];
        ret = handle_item.del_item(db_user_id, del_info);
        if (SUCC != ret)
        {
            STD_ROLLBACK();
            return ret;
        }

        db_del_item_reply_t reply;
        reply.grid_index = del_info->grid_index;
        reply.item_id = del_info->item_id;
        reply.item_count = del_info->item_count;

        p_out->del_items.push_back(reply);
    }

    ret = handle_pet.set_pet_iq(db_user_id, pet_no, iq);
    if (SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }

    p_out->pet_no = pet_no;
    p_out->iq = iq;

    return ret;
}


int Croute_func::db_proto_try_pet_train(DEAL_FUNC_ARG)
{
    db_proto_try_pet_train_in * p_in = P_IN;

    db_proto_try_pet_train_out * p_out = P_OUT;


    db_user_id_t * db_user_id = &p_in->db_user_id;


    // gold
    ret = handle_user.change_gold(db_user_id, -1 * (int)p_in->gold);
    if (SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }


    // del item
    for (uint32_t i = 0; i < p_in->del_items.size(); i++)
    {
        db_del_item_request_t * del_info = &p_in->del_items[i];
        ret = handle_item.del_item(db_user_id, del_info);
        if (SUCC != ret)
        {
            STD_ROLLBACK();
            return ret;
        }

        db_del_item_reply_t reply;
        reply.grid_index = del_info->grid_index;
        reply.item_id = del_info->item_id;
        reply.item_count = del_info->item_count;

        p_out->del_items.push_back(reply);
    }


    ret = handle_pet.try_pet_train(db_user_id, p_in->pet_no, p_in->train_value);
    if (SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }


    p_out->pet_no = p_in->pet_no;
    p_out->gold = p_in->gold;
    for (uint32_t i = 0; i < PET_TRAIN_ATTR_NUM; i++)
    {
        p_out->train_value[i] = p_in->train_value[i];
    }

    return ret;
}


int Croute_func::db_proto_set_pet_train(DEAL_FUNC_ARG)
{
    db_proto_set_pet_train_in * p_in = P_IN;

    db_user_id_t * p_user_id = &p_in->db_user_id;
    uint32_t pet_no = p_in->pet_no;
    uint32_t set_flag = p_in->set_flag;

    if (0 == set_flag)
    {
        ret = handle_pet.cancel_pet_train(p_user_id, pet_no);
        if (SUCC != ret)
        {
            STD_ROLLBACK();
            return ret;
        }

    }
    else
    {

        ret = handle_pet.save_pet_train(p_user_id, pet_no);
        if (SUCC != ret)
        {
            STD_ROLLBACK();
            return ret;
        }
    }

    db_proto_set_pet_train_out * p_out = P_OUT;
    p_out->pet_no = pet_no;
    p_out->set_flag = set_flag;
    // uint32_t try_attr[PET_TRAIN_ATTR_NUM] = {0};
    // handle_pet.get_pet_train_attr(p_user_id, pet_no, p_out->train_attr, try_attr);

    return ret;
}



int Croute_func::db_proto_update_pet_gift(DEAL_FUNC_ARG)
{
    db_proto_update_pet_gift_in * p_in = P_IN;
    db_proto_update_pet_gift_out * p_out = P_OUT;

    db_user_id_t * db_user_id = &p_in->db_user_id;
    uint32_t pet_no = p_in->pet_no;

    // del item
    for (uint32_t i = 0; i < p_in->del_items.size(); i++)
    {
        db_del_item_request_t * del_info = &p_in->del_items[i];
        ret = handle_item.del_item(db_user_id, del_info);
        if (SUCC != ret)
        {
            STD_ROLLBACK();
            return ret;
        }

        db_del_item_reply_t reply;
        reply.grid_index = del_info->grid_index;
        reply.item_id = del_info->item_id;
        reply.item_count = del_info->item_count;

        p_out->del_items.push_back(reply);
    }


    // update extra info
    ret = handle_extra_info.update_extra_info(db_user_id, p_in->extra_info);
    if (SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }
    for (uint32_t i = 0; i < p_in->extra_info.size(); i++)
    {
        p_out->extra_info.push_back(p_in->extra_info[i]);
    }
    

    for (uint32_t i = 0; i < p_in->update_gift.size(); i++)
    {
        ret = handle_pet_gift.update_pet_gift(db_user_id, pet_no, &p_in->update_gift[i]);
        if (SUCC != ret)
        {
            STD_ROLLBACK();
            return ret;
        }

        p_out->update_gift.push_back(p_in->update_gift[i]);
    }

    p_out->pet_no = pet_no;
    return ret;
}

int Croute_func::db_proto_pet_merge(DEAL_FUNC_ARG)
{
    db_proto_pet_merge_in * p_in = P_IN;
    db_proto_pet_merge_out * p_out = P_OUT;
    p_out->init();

    uint32_t base_pet_no = p_in->base_pet_no;
    uint32_t base_pet_level = p_in->base_pet_level;
    uint32_t base_pet_exp = p_in->base_pet_exp;
    db_user_id_t * db_user_id = &p_in->db_user_id;

    // del item
    for (uint32_t i = 0; i < p_in->del_items.size(); i++)
    {
        db_del_item_request_t * del_info = &p_in->del_items[i];
        ret = handle_item.del_item(db_user_id, del_info);
        if (SUCC != ret)
        {
            STD_ROLLBACK();
            return ret;
        }

        db_del_item_reply_t reply;
        reply.grid_index = del_info->grid_index;
        reply.item_id = del_info->item_id;
        reply.item_count = del_info->item_count;

        p_out->del_items.push_back(reply);
    }


    // add item
    for (uint32_t i = 0; i < p_in->add_items.size(); i++)
    {
        db_add_item_request_t * add_info = &p_in->add_items[i];
        db_add_item_reply_t reply_info;
        ret = handle_item.add_item(db_user_id, add_info, &reply_info);
        if (SUCC != ret)
        {
            STD_ROLLBACK();
            return ret;
        }


        p_out->add_items.push_back(reply_info);
    }

    // del pet
    for (uint32_t i = 0; i < p_in->del_pet_no.size(); i++)
    {
        uint32_t del_pet_no = p_in->del_pet_no[i];
        ret = handle_pet.do_del_pet(db_user_id, del_pet_no);
        if (SUCC != ret)
        {
            STD_ROLLBACK();
            return ret;
        }

        p_out->del_pet_no.push_back(del_pet_no);
    }


    // base pet
    ret = handle_pet.do_set_pet_level_exp(db_user_id, base_pet_no, base_pet_level, base_pet_exp);
    if (SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }


    p_out->base_pet_level = base_pet_level;
    p_out->base_pet_no = base_pet_no;
    p_out->base_pet_exp = base_pet_exp;

    return ret;
}


int Croute_func::db_proto_get_player_items(DEAL_FUNC_ARG)
{	
    ret = handle_item.query_player_items(c_in, c_out);
    if(SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }
    return ret;	
}

int Croute_func::db_proto_get_player_equips(DEAL_FUNC_ARG)
{
    ret = handle_item.query_player_equips(c_in, c_out);
    if(SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }
    return ret;
}

int Croute_func::db_proto_add_item(DEAL_FUNC_ARG)
{
    ret = handle_item.add_item(c_in, c_out);
    if(SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }
    return ret;	
}

int Croute_func::db_proto_del_item(DEAL_FUNC_ARG)
{
    db_proto_del_item_in * p_in = P_IN;
    db_proto_del_item_out * p_out = P_OUT;

    uint32_t ret = 0;

    for(uint32_t i=0; i< p_in->del_items.size(); i++)
    {
        ret = handle_item.del_item(&p_in->db_user_id, &p_in->del_items[i]);
        if (SUCC != ret)
        {
            STD_ROLLBACK();
            return ret;
        }

        db_del_item_reply_t reply;
        reply.grid_index = p_in->del_items[i].grid_index;
        reply.item_id = p_in->del_items[i].item_id;
        reply.item_count = p_in->del_items[i].item_count;

        p_out->del_items.push_back(reply);
    }
    return ret;
}

int Croute_func::db_proto_split_item(DEAL_FUNC_ARG)
{
    ret = handle_item.split_item(c_in, c_out);
    if(SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }
    return ret;
}



int Croute_func::db_proto_buy_npc_shop_item(DEAL_FUNC_ARG)
{
    db_proto_buy_npc_shop_item_in * p_in = P_IN;
    db_proto_buy_npc_shop_item_out * p_out = P_OUT;


    db_user_id_t * db_user_id = &p_in->db_user_id;


    // gold
    ret = handle_user.change_gold(db_user_id, -1 * (int)p_in->gold);
    if (SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }


    // add item
    for (uint32_t i = 0; i < p_in->add_items.size(); i++)
    {
        db_add_item_request_t * add_info = &p_in->add_items[i];
        db_add_item_reply_t reply_info;
        ret = handle_item.add_item(db_user_id, add_info, &reply_info);
        if (SUCC != ret)
        {
            STD_ROLLBACK();
            return ret;
        }


        p_out->add_items.push_back(reply_info);
    }


    p_out->gold = p_in->gold;

    return ret;
}



int Croute_func::db_proto_npc_item(DEAL_FUNC_ARG)
{
    db_proto_npc_item_in * p_in = P_IN;
    db_proto_npc_item_out * p_out = P_OUT;


    db_user_id_t * db_user_id = &p_in->db_user_id;


    // gold
    ret = handle_user.change_gold(db_user_id, p_in->gold);
    if (SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }


    // del item
    for (uint32_t i = 0; i < p_in->del_items.size(); i++)
    {
        db_del_item_request_t * del_info = &p_in->del_items[i];
        ret = handle_item.del_item(db_user_id, del_info);
        if (SUCC != ret)
        {
            STD_ROLLBACK();
            return ret;
        }

        db_del_item_reply_t reply;
        reply.grid_index = del_info->grid_index;
        reply.item_id = del_info->item_id;
        reply.item_count = del_info->item_count;

        p_out->del_items.push_back(reply);
    }


    p_out->gold = p_in->gold;

    return ret;
}

int Croute_func::db_proto_redeem_item(DEAL_FUNC_ARG)
{
    db_proto_redeem_item_in * p_in = P_IN;
    db_proto_redeem_item_out * p_out = P_OUT;


    db_user_id_t * db_user_id = &p_in->db_user_id;


    // gold
    ret = handle_user.change_gold(db_user_id, -1 * (int)p_in->gold);
    if (SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }


    // add item
    for (uint32_t i = 0; i < p_in->add_items.size(); i++)
    {
        db_add_item_request_t * add_info = &p_in->add_items[i];
        db_add_item_reply_t reply_info;
        ret = handle_item.add_item(db_user_id, add_info, &reply_info);
        if (SUCC != ret)
        {
            STD_ROLLBACK();
            return ret;
        }


        p_out->add_items.push_back(reply_info);
    }


    p_out->redeem_index = p_in->redeem_index;
    p_out->gold = p_in->gold;

    return ret;
}


int Croute_func::db_proto_wear_equip(DEAL_FUNC_ARG)
{
    ret = handle_item.wear_equip(c_in, c_out);
    if(SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }
    return ret;
}

int Croute_func::db_proto_remove_equip(DEAL_FUNC_ARG)
{
    ret = handle_item.remove_equip(c_in, c_out);
    if(SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }
    return ret;
}


int Croute_func::db_proto_save_extra_info(DEAL_FUNC_ARG)
{
    db_proto_save_extra_info_in * p_in = P_IN;

    db_user_id_t * db_user_id = &p_in->db_user_id;
    ret = handle_extra_info.update_extra_info(db_user_id, p_in->extra_info);
    if (SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }

    db_proto_save_extra_info_out * p_out = P_OUT;
    
    for (uint32_t i = 0; i < p_in->extra_info.size(); i++)
    {
        p_out->extra_info.push_back(p_in->extra_info[i]);

    }


    return ret;
}


int Croute_func::db_proto_get_extra_info(DEAL_FUNC_ARG)
{
    db_proto_get_extra_info_in * p_in = P_IN;
    db_user_id_t * db_user_id = &p_in->db_user_id;
    ret = handle_extra_info.query_extra_info(db_user_id, p_in->query_info);
    if (SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }

    db_proto_get_extra_info_out * p_out = P_OUT;
    for (uint32_t i = 0; i < p_in->query_info.size(); i++)
    {
        p_out->query_info.push_back(p_in->query_info[i]);
    }
    return ret;
}

int Croute_func::db_proto_batch_equips_opt(DEAL_FUNC_ARG)
{
    ret = handle_item.batch_equips_opt(c_in, c_out);
    if(SUCC != ret)
    {
        STD_ROLLBACK();
        return ret;
    }
    return ret;	
}

int Croute_func::db_proto_mail_head_list(DEAL_FUNC_ARG)
{
	ret = handle_mail.mail_head_list(c_in, c_out);
	if(SUCC != ret)
	{
		STD_ROLLBACK();
		return ret;
	}
	return ret;	
}

int Croute_func::db_proto_mail_body(DEAL_FUNC_ARG)
{
	db_proto_mail_body_in* p_in = P_IN;
	
	ret = handle_mail.query_mail_body(c_in, c_out);
	if(SUCC != ret)
	{
		STD_ROLLBACK();
		return ret;
	}
	ret = handle_mail.set_mail_read(&p_in->db_user_id, p_in->mail_id);
	if(SUCC != ret)
	{
		STD_ROLLBACK();
		return ret;
	}
	return ret;
}

int Croute_func::db_proto_del_mail(DEAL_FUNC_ARG)
{
	db_proto_del_mail_in* p_in = P_IN;

	ret = handle_mail.del_mail(c_in, c_out);
	if(SUCC != ret)
	{
		STD_ROLLBACK();
		return ret;
	}
	return ret;
}

int Croute_func::db_proto_take_mail_enclosure(DEAL_FUNC_ARG)
{
	db_proto_take_mail_enclosure_in* p_in = P_IN;
	db_proto_take_mail_enclosure_out* p_out = P_OUT;
	
	uint32_t ret = 0;

	for(uint32_t i =0; i< p_in->add_items.size(); i++)
	{
		uint32_t cur_grid_item_count = 0;
		uint32_t auto_increment_id = 0;

		db_add_item_request_t* info = &p_in->add_items[i];
		ret = handle_item.get_cur_grid_item_count(&p_in->db_user_id, info->grid_index, &cur_grid_item_count);
		if(ret != DB_ERR_NOERROR)
		{
			STD_ROLLBACK();
			return ret;
		}

		if(cur_grid_item_count == 0)
		{
			ret = handle_item.insert_item_count( &p_in->db_user_id, info->grid_index, info->item_id, info->item_count, info->get_time, info->expire_time,  &auto_increment_id);
		}
		else
		{
			ret = handle_item.inc_item_count( &p_in->db_user_id, info->grid_index, info->item_id, info->item_count);
		}
		
		if(ret != DB_ERR_NOERROR)
		{
			STD_ROLLBACK();
			return ret;
		}

		db_add_item_reply_t reply_info;
		reply_info.id = auto_increment_id;
		reply_info.grid_index = info->grid_index;
		reply_info.item_id  = info->item_id;
		reply_info.item_count = info->item_count;
		reply_info.get_time = info->get_time;
		reply_info.expire_time = info->expire_time;

		p_out->add_items.push_back(reply_info);
	}

	ret = handle_mail.clear_mail_enclosure(&p_in->db_user_id, p_in->mail_id);
	if( ret != DB_ERR_NOERROR)
	{
		STD_ROLLBACK();
		return ret;
	}

	p_out->mail_id = p_in->mail_id;
	p_out->add_values = p_in->add_values;
	return ret;
}

int Croute_func::db_proto_send_mail(DEAL_FUNC_ARG)
{
	db_proto_send_mail_in* p_in = P_IN;	
	db_proto_send_mail_out* p_out = P_OUT;	
	
	uint32_t mail_count = 0;
	uint32_t mail_id = 0;
	uint32_t cur_time = (uint32_t)time(NULL);

	ret = handle_mail.get_total_mail_count(p_in->receive_id, p_in->receive_role_tm, p_in->db_user_id.server_id,  &mail_count);
	if(ret != DB_ERR_NOERROR)
	{
		STD_ROLLBACK();
		return ret;
	}
	
	if(mail_count > MAX_MAIL_COUNT)
	{
		STD_ROLLBACK();
		return DB_ERR_MAIL_FULL;
	}

	ret = handle_mail.send_mail(&p_in->db_user_id, p_in->sender_nick, 
								 p_in->receive_id, p_in->receive_role_tm, 
								 p_in->mail_title, p_in->mail_content, cur_time, &mail_id);
	if(ret != DB_ERR_NOERROR)
	{
		STD_ROLLBACK();
		return ret;
	}
	
	p_out->mail_head.mail_id = mail_id;
	p_out->mail_head.mail_time = cur_time;
	p_out->mail_head.mail_state = 2;
	p_out->mail_head.mail_templet = 2;
	p_out->mail_head.mail_type = 2;
	p_out->mail_head.sender_id = p_in->db_user_id.user_id;
	p_out->mail_head.sender_role_tm  = p_in->db_user_id.role_tm;
	strcpy((char*)p_out->mail_head.sender_nick, (char*)p_in->sender_nick);
	strcpy((char*)p_out->mail_head.mail_title, (char*)p_in->mail_title);
	p_out->recevie_id = p_in->receive_id;

	return DB_ERR_NOERROR;
}

int Croute_func::db_proto_add_friend(DEAL_FUNC_ARG)
{
	db_proto_add_friend_in* p_in = P_IN;
	db_proto_add_friend_out *p_out = P_OUT;
    ret = handle_friends.add_friend(p_in->user, p_in->ufriend, p_in->server_id);
    if(ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
	p_out->ufriend = p_in->ufriend;
    return ret;
}

int Croute_func::db_proto_del_friend(DEAL_FUNC_ARG)
{
	db_proto_del_friend_in* p_in = P_IN;
	db_proto_del_friend_out *p_out = P_OUT;
    ret = handle_friends.del_friend(p_in->user, p_in->ufriend, p_in->server_id);
    if(ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
	p_out->ufriend = p_in->ufriend;
    return ret;
}

int Croute_func::db_proto_add_to_blacklist(DEAL_FUNC_ARG)
{
	db_proto_add_to_blacklist_in* p_in = P_IN;
	db_proto_add_to_blacklist_out *p_out = P_OUT;
    ret = handle_friends.add_to_blacklist(p_in->user, p_in->blacked_user, p_in->server_id);
    if(ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
	p_out->blacked_user = p_in->blacked_user;
    return ret;
}

int Croute_func::db_proto_del_from_blacklist(DEAL_FUNC_ARG)
{
	db_proto_del_from_blacklist_in* p_in = P_IN;
	db_proto_del_from_blacklist_out *p_out = P_OUT;
    ret = handle_friends.del_from_blacklist(p_in->user, p_in->unblacked_user, p_in->server_id);
    if(ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
	p_out->unblacked_user = p_in->unblacked_user;
    return ret;
}

int Croute_func::db_proto_forbid_friends_me(DEAL_FUNC_ARG)
{
	db_proto_forbid_friends_me_in* p_in = P_IN;
	db_proto_forbid_friends_me_out *p_out = P_OUT;

    ret = handle_user.forbid_friends_me(p_in->user, p_in->server_id, p_in->flag_forbid);
	p_out->flag_forbid = p_in->flag_forbid;
    if(ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
    return ret;
}

int Croute_func::db_proto_get_user_info(DEAL_FUNC_ARG)
{
	db_proto_get_user_info_in* p_in = P_IN;
	db_proto_get_user_info_out* p_out = P_OUT;

    ret = handle_user.get_user_info(p_in->user, p_in->server_id, p_out);
    if(ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
    return ret;
}

int Croute_func::db_proto_get_friends_list(DEAL_FUNC_ARG)
{
	db_proto_get_friends_list_in* p_in = P_IN;
	db_proto_get_friends_list_out* p_out = P_OUT;

    ret = handle_friends.get_friends_list(p_in->user, p_in->server_id, p_out->user_list);
    if(ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
    return ret;
}


int Croute_func::db_proto_get_blacklist(DEAL_FUNC_ARG)
{
	db_proto_get_blacklist_in* p_in = P_IN;
	db_proto_get_blacklist_out* p_out = P_OUT;

    ret = handle_friends.get_blacklist(p_in->user, p_in->server_id, p_out->blacklist);
    if(ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
    return ret;
}

/* 以后也许可以用到 现在没有协议
int Croute_func::db_proto_get_user_nick(DEAL_FUNC_ARG)
*/

//查询用户是否存在 返回包0不存在 1存在
int Croute_func::db_proto_check_user_exist(DEAL_FUNC_ARG)
{
	db_proto_check_user_exist_in *p_in = P_IN;
	db_proto_check_user_exist_out *p_out = P_OUT;

	ret = handle_user.check_user_exist(p_in->user, p_in->server_id, p_out);
	if (ret != SUCC) {//不为SUCC有可能查询成功，但没有找到记录
		p_out->flag_exist = 0; //不存在
	} else {
		p_out->flag_exist = 1;
	}

	return 0;
}

int Croute_func::db_proto_get_union_list(DEAL_FUNC_ARG)
{
	db_proto_get_union_list_in *p_in = P_IN;
	db_proto_get_union_list_out *p_out = P_OUT;
	return 0;
}


/* ----------------CODE FOR db_proto_obtain_task  ---------*/
// ./Croute_func.cpp

/*接任务*/
int Croute_func::db_proto_obtain_task(DEAL_FUNC_ARG){
	db_proto_obtain_task_in* p_in=P_IN;
	db_proto_obtain_task_out* p_out=P_OUT;
	DEBUG_LOG("11111111==========111111111111");
	ret=this->handle_task.add(p_in->db_user_id,p_in->tskid, sizeof(p_in->buff), p_in->buff);
	p_out->tskid=p_in->tskid;
    memcpy(p_out->buff, p_in->buff, sizeof(p_out->buff));
	return ret;
}

/* ----------------CODE FOR db_proto_cancel_task  ---------*/
// ./Croute_func.cpp

/*取消任务*/
int Croute_func::db_proto_cancel_task(DEAL_FUNC_ARG){
	db_proto_cancel_task_in* p_in=P_IN;
	db_proto_cancel_task_out* p_out=P_OUT;

	ret=this->handle_task.del(p_in->db_user_id,p_in->tskid);

	p_out->tskid=p_in->tskid;

	return ret;
}

/* ----------------CODE FOR db_proto_setbuff_task  ---------*/
// ./Croute_func.cpp

/*设置任务buf*/
int Croute_func::db_proto_setbuff_task(DEAL_FUNC_ARG){
	db_proto_setbuff_task_in* p_in=P_IN;
	db_proto_setbuff_task_out* p_out=P_OUT;

	ret=this->handle_task.update_buf(p_in->db_user_id,p_in->tskid,sizeof( p_in->buff) ,p_in->buff );
	p_out->tskid=p_in->tskid;

	return ret;
}

/* ----------------CODE FOR db_proto_finish_task  ---------*/
// ./Croute_func.cpp

/*完成任务*/
int Croute_func::db_proto_finish_task(DEAL_FUNC_ARG){
	db_proto_finish_task_in* p_in=P_IN;
	db_proto_finish_task_out* p_out=P_OUT;

	ret=this->handle_task.update(p_in->db_user_id,p_in->tskid, sizeof(p_in->buff), p_in->buff,1);

	p_out->tskid=p_in->tskid;
    memcpy(p_out->buff, p_in->buff, sizeof(p_out->buff));

	return ret;
}

/* ----------------CODE FOR db_proto_get_task_flag_list  ---------*/
// ./Croute_func.cpp

/*拉取任务状态列表*/
int Croute_func::db_proto_get_task_flag_list(DEAL_FUNC_ARG){
	db_proto_get_task_flag_list_in* p_in=P_IN;
	db_proto_get_task_flag_list_out* p_out=P_OUT;


	ret=this->handle_task.get_flag_list(p_in->db_user_id,p_in->begin_tskid,p_in->end_tskid,
			p_out->task_flag_list );


	return ret;
}

/* ----------------CODE FOR db_proto_get_task_full_list  ---------*/
// ./Croute_func.cpp

/*拉取任务全量列表*/
int Croute_func::db_proto_get_task_full_list(DEAL_FUNC_ARG){
	db_proto_get_task_full_list_in* p_in=P_IN;
	db_proto_get_task_full_list_out* p_out=P_OUT;

	ret=this->handle_task.get_full_list(p_in->db_user_id,p_in->begin_tskid,p_in->end_tskid,
			p_out->task_full_list );

	return ret;
}

/*online发送离线消息到DB*/
int Croute_func::db_proto_send_offline_msg(DEAL_FUNC_ARG){
	db_proto_send_offline_msg_in* p_in=P_IN;
	msg_list_t offline_msg_list;

	ret=this->handle_user.get_offline_msg( &(p_in->db_user_id ), &offline_msg_list );
	if( ret != SUCC) return 0;

	offline_msg_list.msg_list.push_back(p_in->msg);
	ret=this->handle_user.update_offline_msg( &(p_in->db_user_id ), &offline_msg_list );

	return ret;
}

int Croute_func::db_proto_add_prize(DEAL_FUNC_ARG)
{
    db_proto_add_prize_in *p_in = P_IN;
    db_proto_add_prize_out *p_out = P_OUT;
    p_out->prize_id = p_in->prize_id;
    p_out->add_time = p_out->add_time;

    ret = this->handle_prize.add(p_in->db_user_id, p_in->prize_id, p_in->add_time);
    return ret;
}

int Croute_func::db_proto_del_prize(DEAL_FUNC_ARG)
{
    db_proto_del_prize_in *p_in = P_IN;
    db_proto_del_prize_out *p_out = P_OUT;
    p_out->prize_id = p_in->prize_id;

    ret = this->handle_prize.del(p_in->db_user_id, p_in->prize_id);
    return ret;
}

int Croute_func::db_proto_get_prize_list(DEAL_FUNC_ARG)
{
    db_proto_get_prize_list_in *p_in = P_IN;
    db_proto_get_prize_list_out *p_out = P_OUT;
    ret = this->handle_prize.get_prize_list(p_in->db_user_id, p_out->prize_list);
    return ret;
}

// ../../gen_proto/proto/pea_db_db_src.cpp  
