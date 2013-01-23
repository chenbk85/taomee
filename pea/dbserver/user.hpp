#ifndef _PEA_USER_H_
#define _PEA_USER_H_

#include "CtableRoute100x10.h"
#include "proto.hpp"
#include "benchapi.h"
#include <vector>
#define OFFLINE_MSG_LEN 2048 


class pea_user : public CtableRoute100x10
{
    public:
        pea_user(mysql_interface* db);
        int query_user_info(uint32_t user_id, uint32_t role_tm, uint32_t server_id, db_proto_get_player_out *p_out) ;
        int save_user_info(uint32_t user_id, uint32_t role_tm, uint32_t server_id, db_player_info* info);
        int query_role_list(uint32_t user_id, uint32_t server_id, std::vector<login_player_info_t> & vec);
        int create_role(db_user_id_t * db_user, const char * name, model_info_t * p_model);

        int update_gold(db_user_id_t * db_user_id, uint32_t new_gold);
        int change_gold(db_user_id_t * db_user_id, int32_t gold_change);

        int change_exp(db_user_id_t * db_user_id, int32_t exp_change);

        int level_up(db_user_id_t * db_user_id);

        int save_login_time(db_user_id_t * db_user_id, uint32_t last_login_tm, uint32_t last_off_line_tm);

		int forbid_friends_me(uid_role_t &user,  uint32_t server_id, uint32_t flag_forbid);
		
		int get_user_info(uid_role_t &user, uint32_t server_id, db_proto_get_user_info_out *p_out);

		int get_user_nick(uid_role_t &user, uint32_t server_id, char *buf, uint32_t buf_len);

		int check_user_exist(uid_role_t &user, uint32_t server_id, db_proto_check_user_exist_out *p_out);

		int get_offline_msg(db_user_id_t * p_db_user_id, msg_list_t *p_msg );
		int update_offline_msg(db_user_id_t * p_db_user_id, msg_list_t *p_msg );
};

#endif
