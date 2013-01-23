#include <algorithm>
#include "item.hpp"
#include "pea_common.hpp"

pea_item::pea_item(mysql_interface* db)
	:CtableRoute100x10(db, "pea", "pea_item", "user_id")
{
		
}
    
int pea_item::query_player_items(Cmessage * c_in, Cmessage * c_out)
{
	db_proto_get_player_items_in * p_in = P_IN;
	db_proto_get_player_items_out * p_out = P_OUT;

	uint32_t user_id = p_in->db_user_id.user_id;
	uint32_t role_tm = p_in->db_user_id.role_tm;
	uint32_t server_id = p_in->db_user_id.server_id;


	GEN_SQLSTR(sqlstr, "select id, grid_index, item_id, item_count, get_time, expire_time from %s where user_id = %u and role_tm = %u and server_id = %u and !(grid_index >= %u and grid_index <= %u)", get_table_name(user_id), 
		 user_id, 
		 role_tm, 
		 server_id,
		 head_equip_pos,
		 suit_equip_pos
		 );

	STD_QUERY_WHILE_BEGIN_NEW(sqlstr, p_out->items);
		INT_CPY_NEXT_FIELD(item.id);
		INT_CPY_NEXT_FIELD(item.grid_index);
		INT_CPY_NEXT_FIELD(item.item_id);
		INT_CPY_NEXT_FIELD(item.item_count);
		INT_CPY_NEXT_FIELD(item.get_time);
		INT_CPY_NEXT_FIELD(item.expire_time);
	STD_QUERY_WHILE_END_NEW();
}

int pea_item::query_player_equips(Cmessage * c_in, Cmessage * c_out)
{
	db_proto_get_player_equips_in * p_in = P_IN;
	db_proto_get_player_equips_out * p_out = P_OUT;

    return get_player_equips(&p_in->db_user_id, p_out->equips);
}


int pea_item::get_player_equips(db_user_id_t * db_user, vector<db_equip_info_t> & item_vec)
{
	GEN_SQLSTR(sqlstr, "select id, grid_index, item_id, item_count, get_time, expire_time, hide from %s where user_id = %u and role_tm = %u and server_id = %u and grid_index >= %u and grid_index <= %u", 
            get_table_name(db_user->user_id), 
            db_user->user_id, 
            db_user->role_tm, 
            db_user->server_id,
            head_equip_pos,
            suit_equip_pos
            );

    STD_QUERY_WHILE_BEGIN_NEW(sqlstr, item_vec);
        INT_CPY_NEXT_FIELD(item.id);
        INT_CPY_NEXT_FIELD(item.grid_index);
        INT_CPY_NEXT_FIELD(item.item_id);
        INT_CPY_NEXT_FIELD(item.item_count);
        INT_CPY_NEXT_FIELD(item.get_time);
		INT_CPY_NEXT_FIELD(item.expire_time);
		INT_CPY_NEXT_FIELD(item.hide);
    STD_QUERY_WHILE_END_NEW();
}

int pea_item::add_item(Cmessage * c_in, Cmessage * c_out)
{
    db_proto_add_item_in * p_in = P_IN;
    db_proto_add_item_out * p_out = P_OUT;

    uint32_t ret = 0;

    for(uint32_t i =0; i< p_in->add_items.size(); i++)
    {
        uint32_t cur_grid_item_count = 0;
        uint32_t auto_increment_id = 0;

        db_add_item_request_t* info = &p_in->add_items[i];	
        ret = get_cur_grid_item_count(&p_in->db_user_id, info->grid_index, &cur_grid_item_count);
        if(ret != DB_ERR_NOERROR)return ret;

        if(cur_grid_item_count == 0)
        {
            ret = insert_item_count( &p_in->db_user_id, info->grid_index, info->item_id, info->item_count, info->get_time, info->expire_time,  &auto_increment_id);
        }
        else
        {
            ret = inc_item_count( &p_in->db_user_id, info->grid_index, info->item_id, info->item_count);	
        }
        if(ret != DB_ERR_NOERROR)return ret;

        db_add_item_reply_t reply_info;
        reply_info.id = auto_increment_id;
        reply_info.grid_index = info->grid_index;
        reply_info.item_id  = info->item_id;
        reply_info.item_count = info->item_count;
        reply_info.get_time = info->get_time;
		reply_info.expire_time = info->expire_time;

        p_out->add_items.push_back(reply_info);
    }
    return 0;
}

int pea_item::add_item(db_user_id_t * db_user_id, std::vector<db_add_item_request_t>& request)
{
	uint32_t ret = DB_ERR_NOERROR;
	for(uint32_t i=0; i< request.size(); i++)
	{
		db_add_item_request_t* info = &request[i];
		ret = add_item(db_user_id, info);
		if(ret != DB_ERR_NOERROR)return ret;
	}
	return DB_ERR_NOERROR;
}

int pea_item::add_item(db_user_id_t * db_user_id, db_add_item_request_t * info, db_add_item_reply_t * reply)
{
    uint32_t ret = 0;
    uint32_t cur_grid_item_count = 0;
    uint32_t auto_increment_id = 0;

    ret = get_cur_grid_item_count(db_user_id, info->grid_index, &cur_grid_item_count);
    if(ret != DB_ERR_NOERROR)return ret;

    if(cur_grid_item_count == 0)
    {
        ret = insert_item_count(db_user_id, info->grid_index, info->item_id, info->item_count, info->get_time, info->expire_time,  &auto_increment_id);
    }
    else
    {
        ret = inc_item_count(db_user_id, info->grid_index, info->item_id, info->item_count);	
    }

    if (DB_ERR_NOERROR != ret)
    {
        return ret;
    }

    reply->id = auto_increment_id;
    reply->grid_index = info->grid_index;
    reply->item_id  = info->item_id;
    reply->item_count = info->item_count;
    reply->get_time = info->get_time;
    reply->expire_time = info->expire_time;


    return ret;
}


int pea_item::del_item(db_user_id_t * db_user_id, db_del_item_request_t * info)
{
    uint32_t ret = 0;
    uint32_t cur_grid_item_count = 0;

    ret = get_cur_grid_item_count(db_user_id, info->grid_index,  &cur_grid_item_count);	
    if(ret != DB_ERR_NOERROR)return ret;

    if( cur_grid_item_count < info->item_count) return DB_ERR_NOT_ENOUGH_COUNT;

    if( cur_grid_item_count == info->item_count)
    {
        ret = del_grid_item(db_user_id, info->grid_index, info->item_id);	
    }
    else
    {
        ret = dec_item_count(db_user_id, info->grid_index, info->item_id, info->item_count);
    }

    return ret;
}

int pea_item::split_item(Cmessage* c_in, Cmessage* c_out)
{
    db_proto_split_item_in * p_in = P_IN;
    db_proto_split_item_out * p_out = P_OUT;

    uint32_t cur_grid_item_count = 0;
    uint32_t other_grid_item_count = 0;
    uint32_t auto_increment_id = 0;

    uint32_t ret = get_cur_grid_item_count(&p_in->db_user_id, p_in->grid_index,  &cur_grid_item_count);
    if(ret != DB_ERR_NOERROR)return ret;

    ret = get_cur_grid_item_count(&p_in->db_user_id, p_in->other_grid_index, &other_grid_item_count);
    if(ret != DB_ERR_NOERROR)return ret;


    if( cur_grid_item_count < p_in->item_count ) return DB_ERR_NOT_ENOUGH_COUNT;

    if( cur_grid_item_count == p_in->item_count)
    {
        ret = del_grid_item(&p_in->db_user_id, p_in->grid_index, p_in->item_id);		
    }
    else
    {
        ret = dec_item_count(&p_in->db_user_id, p_in->grid_index, p_in->item_id, p_in->item_count);
    }

    if(ret != DB_ERR_NOERROR)return ret;

    if( other_grid_item_count == 0)
    {
        ret = insert_item_count( &p_in->db_user_id, p_in->grid_index, p_in->item_id, p_in->item_count, p_in->get_time, p_in->expire_time,  &auto_increment_id);
    }
    else
    {
        ret = update_item_count( &p_in->db_user_id, p_in->grid_index, p_in->item_id, p_in->item_count);
    }
    if(ret != DB_ERR_NOERROR)return ret;

    p_out->grid_index = p_in->grid_index;
    p_out->item_id = p_in->item_id;
    p_out->item_count = p_in->item_count;
    p_out->get_time = p_in->get_time;
    p_out->expire_time = p_in->expire_time;
    p_out->other_grid_index = p_in->other_grid_index;
    p_out->id = auto_increment_id;
    return 0;
}

int pea_item::wear_equip(Cmessage* c_in, Cmessage* c_out)
{
    db_proto_wear_equip_in* p_in = P_IN;	
    db_proto_wear_equip_out* p_out = P_OUT;

    uint32_t temp_grid = 10000000;

    uint32_t ret = update_item_grid_index(&p_in->db_user_id, p_in->bag_index, temp_grid, 0);
    if(ret != DB_ERR_NOERROR)return ret;

    ret = update_item_grid_index(&p_in->db_user_id, p_in->body_index, p_in->bag_index, 0);
    if(ret != DB_ERR_NOERROR)return ret;

    ret = update_item_grid_index(&p_in->db_user_id,  temp_grid, p_in->body_index, p_in->hide);
    if(ret != DB_ERR_NOERROR)return ret;


    p_out->bag_index = p_in->bag_index;
    p_out->body_index = p_in->body_index;
    return 0;
}

int pea_item::remove_equip(Cmessage* c_in, Cmessage* c_out)
{
    db_proto_remove_equip_in* p_in = P_IN;
    db_proto_remove_equip_out* p_out = P_OUT;

    uint32_t ret = update_item_grid_index(&p_in->db_user_id, p_in->equip_pos, p_in->bag_empty_index, 0);
    if(ret != DB_ERR_NOERROR)return ret;

    p_out->equip_pos = p_in->equip_pos;
    p_out->bag_empty_index = p_in->bag_empty_index;
    return 0;
}

int pea_item::batch_equips_opt(Cmessage* c_in, Cmessage* c_out)
{
    db_proto_batch_equips_opt_in* p_in = P_IN;
    db_proto_batch_equips_opt_out* p_out = P_OUT;
    uint32_t ret = 0;

    for(uint32_t i =0; i< p_in->equip_opts.size(); i++)
    {
        equip_change_t* opt = &p_in->equip_opts[i];	
		if (opt->empty_bag_index == 0) {//包裹index为0表示不换装只隐藏
			set_equip_hide(&p_in->db_user_id, opt->body_index, opt->hide);
			continue;
		}

        uint32_t body_count = 0;
        uint32_t bag_count = 0; 
        ret = get_cur_grid_item_count( &p_in->db_user_id,  opt->body_index, &body_count);
        if(ret != SUCC)return ret;

        ret = get_cur_grid_item_count( &p_in->db_user_id,  opt->empty_bag_index, &bag_count);
        if(ret != DB_ERR_NOERROR)return ret;

        if(body_count == 0 && bag_count ==1)//穿上衣服
        {
            ret = update_item_grid_index(&p_in->db_user_id,  opt->empty_bag_index, opt->body_index, opt->hide);
            if(ret != DB_ERR_NOERROR)return ret;
        }
        if(body_count ==1 && bag_count == 0)//脱下衣服	
        {
            ret = update_item_grid_index(&p_in->db_user_id,  opt->body_index, opt->empty_bag_index, 0);
            if(ret != DB_ERR_NOERROR)return ret;
        }
        if(body_count == 1 && bag_count == 1)//交换衣服
        {
            uint32_t temp_grid = 10000000;

            ret = update_item_grid_index(&p_in->db_user_id, opt->empty_bag_index, temp_grid, 0);
            if(ret != DB_ERR_NOERROR)return ret;

            ret = update_item_grid_index(&p_in->db_user_id, opt->body_index, opt->empty_bag_index, 0);
            if(ret != DB_ERR_NOERROR)return ret;

            ret = update_item_grid_index(&p_in->db_user_id,  temp_grid, opt->body_index, opt->hide);
            if(ret != DB_ERR_NOERROR)return ret;
        }
    }
    p_out->equip_opts = p_in->equip_opts;
    return 0;
}




/*---------------------------------------------------------------------------------*/
int pea_item::set_equip_hide(db_user_id_t * db_user, uint32_t grid_index, uint32_t hide)
{
	GEN_SQLSTR(sqlstr, "update %s set hide = %u where user_id = %u and role_tm = %u and server_id = %u and grid_index = %u",
            get_table_name(db_user->user_id), hide, db_user->user_id, db_user->role_tm, db_user->server_id, grid_index);	
    return exec_update_sql(sqlstr, DB_ERR_NOERROR);
}

int pea_item::update_item_grid_index(db_user_id_t* db_user, uint32_t cur_item_index, uint32_t new_item_index, uint32_t hide)
{
    GEN_SQLSTR(sqlstr, "update %s set grid_index = %u, hide = %u where user_id = %u and role_tm = %u and server_id = %u and grid_index = %u",
            get_table_name(db_user->user_id), new_item_index, hide, db_user->user_id, db_user->role_tm, db_user->server_id, cur_item_index);	

    return exec_update_sql(sqlstr, DB_ERR_NOERROR);
}



int pea_item::get_cur_grid_item_count(db_user_id_t* db_user, uint32_t cur_item_index, uint32_t* item_count)
{
    GEN_SQLSTR(sqlstr, "select item_count from %s where user_id = %u and role_tm = %u and server_id = %u and grid_index = %u limit 1", get_table_name(db_user->user_id), db_user->user_id, db_user->role_tm, db_user->server_id, cur_item_index);	

    STD_QUERY_ONE_BEGIN(sqlstr, DB_ERR_NOERROR)
        INT_CPY_NEXT_FIELD(*item_count);
    STD_QUERY_ONE_END();
}

int pea_item::get_item_count(db_user_id_t* db_user, uint32_t item_id, uint32_t* item_count)
{
    GEN_SQLSTR(sqlstr, "select sum(item_count) as total from %s where user_id = %u and role_tm = %u and server_id = %u and item_id = %u  limit 1", get_table_name(db_user->user_id), db_user->user_id, db_user->role_tm, db_user->server_id, item_id);

    STD_QUERY_ONE_BEGIN(sqlstr, DB_ERR_NOERROR)
        INT_CPY_NEXT_FIELD(*item_count);
    STD_QUERY_ONE_END();
}

int pea_item::insert_item_count(db_user_id_t* db_user, uint32_t cur_item_index, uint32_t item_id, uint32_t item_count, uint32_t get_time, uint32_t expire_time, uint32_t* last_insert_id)
{
    GEN_SQLSTR(sqlstr, "insert into %s (user_id, role_tm, server_id, grid_index, item_id, item_count, get_time, expire_time) values(%u, %u, %u, %u, %u, %u, %u, %u)", get_table_name(db_user->user_id), db_user->user_id, db_user->role_tm, db_user->server_id, cur_item_index, item_id, item_count, get_time, expire_time);

    return exec_insert_sql_get_auto_increment_id(sqlstr, DB_ERR_EXIST, last_insert_id);
}

int pea_item::update_item_count(db_user_id_t* db_user, uint32_t cur_item_index, uint32_t item_id, uint32_t item_count)
{
    GEN_SQLSTR(sqlstr, "update %s set item_count = %u where user_id = %u and role_tm = %u and server_id = %u and item_id = %u  and grid_index = %u limit 1 ", get_table_name(db_user->user_id), item_count, db_user->user_id, db_user->role_tm, db_user->server_id, item_id, cur_item_index);

    return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);	
}

int pea_item::inc_item_count(db_user_id_t* db_user, uint32_t cur_item_index, uint32_t item_id, uint32_t item_count)
{
    GEN_SQLSTR(sqlstr, "update %s set item_count = item_count +  %u where user_id = %u and role_tm = %u and server_id = %u and item_id = %u and grid_index = %u limit 1 ", get_table_name(db_user->user_id), item_count, db_user->user_id, db_user->role_tm, db_user->server_id, item_id, cur_item_index);

    return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);
}

int pea_item::dec_item_count(db_user_id_t* db_user, uint32_t cur_item_index, uint32_t item_id, uint32_t item_count)
{
    GEN_SQLSTR(sqlstr, "update %s set item_count = item_count - %u where user_id = %u and role_tm = %u and server_id = %u and item_id = %u and grid_index = %u limit 1 ", get_table_name(db_user->user_id), item_count, db_user->user_id, db_user->role_tm, db_user->server_id, item_id, cur_item_index);	

    return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);
}

int pea_item::del_grid_item(db_user_id_t* db_user, uint32_t cur_item_index, uint32_t item_id)
{
    GEN_SQLSTR(sqlstr, "delete from %s where  user_id = %u and role_tm = %u and server_id = %u and item_id = %u and grid_index = %u", get_table_name(db_user->user_id), db_user->user_id, db_user->role_tm, db_user->server_id, item_id, cur_item_index);

    return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);
}

