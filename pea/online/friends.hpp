#ifndef _FRIENDS_HPP_
#define _FRIENDS_HPP_

#include "proto.hpp"

class player_t;
int do_add_friend(player_t *p, uint32_t user_id, uint32_t role_tm, uint32_t forbid_friends_me);

int do_add_to_blacklist(player_t *p, uint32_t user_id, uint32_t role_tm);

#endif /*_FRIENDS_HPP_ END*/
