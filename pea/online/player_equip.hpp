#ifndef PLAYER_EQUIP_HPP
#define PLAYER_EQUIP_HPP

#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "db_item.hpp"
#include "item_bag.hpp"

class player_t;

int cli_proto_get_equips(DEFAULT_ARG);

int send_player_equips(player_t* p);

int cli_proto_wear_equip(DEFAULT_ARG);

int cli_proto_remove_equip(DEFAULT_ARG);

int cli_proto_batch_equips_opt(DEFAULT_ARG);


#endif
