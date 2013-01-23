#ifndef ITEM_HPP_
#define ITEM_HPP_


#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "db_item.hpp"
#include "item_bag.hpp"

struct player_t;

int cli_proto_get_items(DEFAULT_ARG);

int send_player_items(player_t* p);

int notify_player_add_item(player_t* p, db_add_item_reply_t* reply);

int notify_player_del_item(player_t* p, db_del_item_reply_t* reply);

int cli_proto_add_item( DEFAULT_ARG );

int cli_proto_del_item( DEFAULT_ARG );








#endif
