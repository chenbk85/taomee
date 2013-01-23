#include <algorithm>
extern "C" {
#include <libtaomee/log.h>
}
#include "user.hpp"


using namespace std;

pea_user::pea_user(mysql_interface* db)
    :CtableRoute100x10(db, "pea", "pea_user", "user_id")
{


}

int pea_user::query_user_info(uint32_t user_id, uint32_t role_tm, uint32_t server_id, db_proto_get_player_out *p_out)
{
    GEN_SQLSTR(sqlstr, "select user_id, role_tm, nick, eye_model, resource_id, server_id, last_login_tm, last_off_line_tm, exp, level, max_bag_grid_count, map_id, map_x, map_y, gold, forbid_friends_me, offline_msg from %s where user_id = %u and role_tm = %u and server_id = %u limit 1", 
            get_table_name(user_id), user_id, role_tm, server_id);	

	db_player_info *info = &(p_out->player_info);
    // init没有把nick清零
	char buf[OFFLINE_MSG_LEN];
    memset(info->nick, 0, sizeof(info->nick));
    STD_QUERY_ONE_BEGIN(sqlstr, DB_ERR_NOT_EXIST)
        INT_CPY_NEXT_FIELD(info->user_id);
        INT_CPY_NEXT_FIELD(info->role_tm); 
        BIN_CPY_NEXT_FIELD(info->nick, sizeof(info->nick));
        INT_CPY_NEXT_FIELD(info->model.eye_model);
        INT_CPY_NEXT_FIELD(info->model.resource_id);
        INT_CPY_NEXT_FIELD(info->server_id); 
        INT_CPY_NEXT_FIELD(info->last_login_tm);
        INT_CPY_NEXT_FIELD(info->last_off_line_tm);
        INT_CPY_NEXT_FIELD(info->exp);
        INT_CPY_NEXT_FIELD(info->level);
        INT_CPY_NEXT_FIELD(info->max_bag_grid_count);
        INT_CPY_NEXT_FIELD(info->map_id);
        INT_CPY_NEXT_FIELD(info->map_x);
        INT_CPY_NEXT_FIELD(info->map_y);
        INT_CPY_NEXT_FIELD(info->gold);
		INT_CPY_NEXT_FIELD(info->forbid_friends_me);
		BIN_CPY_NEXT_FIELD(buf, sizeof(buf));
		p_out->offline_msg_list.read_from_buf_ex(buf,sizeof(buf));
    STD_QUERY_ONE_END()
}

int pea_user::save_user_info(uint32_t user_id, uint32_t role_tm, uint32_t server_id, db_player_info* info)
{
    GEN_SQLSTR(sqlstr, "update %s set last_login_tm = %u, last_off_line_tm = %u, exp = %u, level = %u, max_bag_grid_count = %u, map_id = %u, map_x = %u, map_y = %u, gold  = %u where user_id = %u and role_tm = %u and server_id = %u",
            get_table_name(user_id), 
            info->last_login_tm, 
            info->last_off_line_tm,
            info->exp, 
            info->level, 
            info->max_bag_grid_count, 
            info->map_id, 
            info->map_x, 
            info->map_y, 
            info->gold, 
            info->user_id, 
            info->role_tm,
            info->server_id
            );
    return exec_update_sql(sqlstr, SUCC);
}



int pea_user::query_role_list(uint32_t user_id, uint32_t server_id, vector<login_player_info_t> & vec)
{
    GEN_SQLSTR(sqlstr, "SELECT user_id, role_tm, nick, eye_model, resource_id, server_id, level FROM %s WHERE user_id = %u AND server_id = %u",
            get_table_name(user_id),
            user_id,
            server_id);

    STD_QUERY_WHILE_BEGIN_NEW(sqlstr, vec)
        memset(item.nick, 0, sizeof(item.nick));
        INT_CPY_NEXT_FIELD(item.user_id);
        INT_CPY_NEXT_FIELD(item.role_tm);
        BIN_CPY_NEXT_FIELD(item.nick, sizeof(item.nick));
        INT_CPY_NEXT_FIELD(item.model.eye_model);
        INT_CPY_NEXT_FIELD(item.model.resource_id);
        INT_CPY_NEXT_FIELD(item.server_id);
        INT_CPY_NEXT_FIELD(item.level);
    STD_QUERY_WHILE_END_NEW()
}


int pea_user::create_role(db_user_id_t * db_user_id, const char * name, model_info_t * p_model)
{
    GEN_SQLSTR(sqlstr, "INSERT IGNORE INTO %s (user_id, role_tm, nick, eye_model, resource_id, server_id,offline_msg ) VALUES(%u, %u, \'%s\', %u, %u, %u,0x00000000 )", 
            get_table_name(db_user_id->user_id),
            db_user_id->user_id,
            db_user_id->role_tm,
            name,
            p_model->eye_model,
            p_model->resource_id,
            db_user_id->server_id
            );
    return exec_insert_sql(sqlstr, SUCC);
}

int pea_user::update_gold(db_user_id_t * db_user_id, uint32_t new_gold)
{
    GEN_SQLSTR(sqlstr, "UPDATE %s SET gold = %u WHERE user_id = %u AND role_tm = %u AND server_id = %u",
            get_table_name(db_user_id->user_id),
            new_gold,
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id);
    return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);
}

int pea_user::change_gold(db_user_id_t * db_user_id, int32_t gold_change)
{
    GEN_SQLSTR(sqlstr, "UPDATE %s SET gold = gold + (%d) WHERE user_id = %u AND role_tm = %u AND server_id = %u",
            get_table_name(db_user_id->user_id),
            gold_change,
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id);
    return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);
}

int pea_user::change_exp(db_user_id_t * db_user_id, int32_t exp_change)
{
    GEN_SQLSTR(sqlstr, "UPDATE %s SET exp = exp + (%d) WHERE user_id = %u AND role_tm = %u AND server_id = %u",
            get_table_name(db_user_id->user_id),
            exp_change,
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id);
    return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);
}

int pea_user::level_up(db_user_id_t * db_user_id)
{
    GEN_SQLSTR(sqlstr, "UPDATE %s SET level = level + 1 WHERE user_id = %u AND role_tm = %u AND server_id = %u",
            get_table_name(db_user_id->user_id),
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id);
    return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);
}

int pea_user::save_login_time(db_user_id_t * db_user_id, uint32_t last_login_tm, uint32_t last_off_line_tm)
{
    if (last_login_tm && last_off_line_tm)
    {
        GEN_SQLSTR(sqlstr, "UPDATE %s SET last_login_tm = %u, last_off_line_tm = %u WHERE user_id = %u AND role_tm = %u AND server_id = %u",
                get_table_name(db_user_id->user_id),
                last_login_tm, 
                last_off_line_tm,
                db_user_id->user_id,
                db_user_id->role_tm,
                db_user_id->server_id);
    }
    else if (last_login_tm)
    {
        GEN_SQLSTR(sqlstr, "UPDATE %s SET last_login_tm = %u WHERE user_id = %u AND role_tm = %u AND server_id = %u",
                get_table_name(db_user_id->user_id),
                last_login_tm, 
                db_user_id->user_id,
                db_user_id->role_tm,
                db_user_id->server_id);
    }
    else if (last_off_line_tm)
    {
        GEN_SQLSTR(sqlstr, "UPDATE %s SET last_off_line_tm = %u WHERE user_id = %u AND role_tm = %u AND server_id = %u",
                get_table_name(db_user_id->user_id),
                last_off_line_tm, 
                db_user_id->user_id,
                db_user_id->role_tm,
                db_user_id->server_id);
    }
    else
    {
        return SUCC;
    }

    return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);
}

int pea_user::forbid_friends_me(uid_role_t &user, uint32_t server_id, uint32_t flag_forbid)
{
	GEN_SQLSTR(sqlstr, "update %s set forbid_friends_me = %u where user_id = %u and role_tm = %u and server_id = %u",
			get_table_name(user.user_id), 
			flag_forbid,
			user.user_id,
			user.role_tm,
			server_id);
	return exec_update_sql(sqlstr, SUCC);
}

int pea_user::get_user_info(uid_role_t &user, uint32_t server_id, db_proto_get_user_info_out *p_out)
{
	GEN_SQLSTR(sqlstr, "select user_id, nick, level, exp, role_tm from %s where user_id = %u and role_tm = %u and server_id = %u",
			get_table_name(user.user_id),
			user.user_id,
			user.role_tm,
			server_id);
	STD_QUERY_ONE_BEGIN(sqlstr, DB_ERR_NOT_EXIST) {
        memset(p_out->nick, 0, sizeof(p_out->nick));
		INT_CPY_NEXT_FIELD(p_out->user.user_id);
		BIN_CPY_NEXT_FIELD(p_out->nick, sizeof(p_out->nick));
		INT_CPY_NEXT_FIELD(p_out->level);
		INT_CPY_NEXT_FIELD(p_out->exp);
		INT_CPY_NEXT_FIELD(p_out->user.role_tm);
	}
	STD_QUERY_ONE_END()
}

int pea_user::get_user_nick(uid_role_t &user, uint32_t server_id, char *buf, uint32_t buf_len)
{
	GEN_SQLSTR(sqlstr, "select nick from %s where user_id = %u and role_tm = %u and server_id = %u",
			get_table_name(user.user_id),
			user.user_id,
			user.role_tm,
			server_id);
	STD_QUERY_ONE_BEGIN(sqlstr, DB_ERR_NOT_EXIST) 
		BIN_CPY_NEXT_FIELD(buf, buf_len);
	STD_QUERY_ONE_END()
}

int pea_user::check_user_exist(uid_role_t &user, uint32_t server_id, db_proto_check_user_exist_out *p_out)
{
	GEN_SQLSTR(sqlstr, "select user_id, role_tm, forbid_friends_me from %s where user_id = %u and role_tm = %u and server_id = %u limit 1",
			get_table_name(user.user_id),
			user.user_id,
			user.role_tm,
			server_id);
	STD_QUERY_ONE_BEGIN(sqlstr, DB_ERR_NOT_EXIST);
		INT_CPY_NEXT_FIELD(p_out->user.user_id);
		INT_CPY_NEXT_FIELD(p_out->user.role_tm);
		INT_CPY_NEXT_FIELD(p_out->forbid_friends_me);
	STD_QUERY_ONE_END();
}

int pea_user::get_offline_msg(db_user_id_t * p_db_user_id, msg_list_t *p_msg )
{

	GEN_SQLSTR(sqlstr, "select offline_msg from %s where user_id = %u and role_tm = %u and server_id = %u  ",
			get_table_name(p_db_user_id->user_id),
			p_db_user_id->user_id,
			p_db_user_id->role_tm,
			p_db_user_id->server_id);
	char buf[ OFFLINE_MSG_LEN ] ={0,0,0,0,0,0};
	STD_QUERY_ONE_BEGIN(sqlstr, DB_ERR_NOT_EXIST);
		BIN_CPY_NEXT_FIELD(buf, sizeof(buf));
		p_msg->read_from_buf_ex(buf,sizeof(buf));
	STD_QUERY_ONE_END();
}

int pea_user::update_offline_msg(db_user_id_t * p_db_user_id, msg_list_t *p_msg )
{

	char buf[ OFFLINE_MSG_LEN ] ={0,0,0,0,0,0};
	char buf_mysql[mysql_str_len(sizeof(buf) )] ={0,0,0,0,0,0};
	
	uint32_t  real_buf_len;
	//set to buf
	bool ret=p_msg->write_to_buf_ex(buf,sizeof( buf), &real_buf_len  );
	if (!ret ) {
		DEBUG_LOG("rest space not enough[offline msg]");
		return -1;
	}
    set_mysql_string(buf_mysql, buf , real_buf_len );

    GEN_SQLSTR(sqlstr, "UPDATE %s SET offline_msg ='%s' WHERE user_id = %u AND role_tm = %u AND server_id = %u",
            get_table_name(p_db_user_id->user_id),
            buf_mysql,
            p_db_user_id->user_id,
            p_db_user_id->role_tm,
            p_db_user_id->server_id);
    return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);
}
