#ifndef DB_FRIENDS_HPP
#define DB_FRIENDS_HPP

#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "dbproxy.hpp"

class player_t;

int db_add_friend(player_t* p, uint32_t user_id, uint32_t role_tm);

int db_del_friend(player_t* p, uint32_t user_id, uint32_t role_tm);

int db_add_to_blacklist(player_t *p, uint32_t user_id, uint32_t role_tm);

int db_del_from_blacklist(player_t *p, uint32_t user_id, uint32_t role_tm);

int db_forbid_friends_me(player_t *p, uint32_t flag_forbid);

int db_get_user_info(player_t *p, uint32_t user_id, uint32_t role_tm);

int db_get_friends_list(player_t *p);

int db_get_blacklist(player_t *p);

int db_get_union_list(player_t *p);

int db_check_user_exist(player_t *p, uint32_t checked_id, uint32_t role_tm);

#endif
