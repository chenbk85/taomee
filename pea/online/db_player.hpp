#ifndef DB_PLAYER_HPP
#define DB_PLAYER_HPP

#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "dbproxy.hpp"

class player_t;

int db_get_player(player_t* p);

int db_save_player(player_t* p);

int db_get_extra_info(player_t * p);

int db_save_extra_info(player_t * p);

int db_save_login_time(player_t * p);

int db_update_gold(const player_t *p, uint32_t new_gold);

int db_change_exp(const player_t *p, int32_t exp_change);

int db_change_gold(const player_t *p, int32_t gold_change);

#endif
