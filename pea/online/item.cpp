#include <libtaomee++/inet/pdumanip.hpp>
extern "C"
{
#include <arpa/inet.h>
#include <libtaomee/timer.h>
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/project/utilities.h>
}

#include "proto.hpp"
#include "cli_proto.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "item.hpp"
#include "item_bag.hpp"
#include "db_item.hpp"

using namespace taomee;

int cli_proto_get_items(DEFAULT_ARG)
{
	return send_player_items(p);
}

int cli_proto_add_item( DEFAULT_ARG )
{
	//only for test this code
	
	std::vector<db_add_item_request_t> rsp;

	bool ret = parse_db_add_item_request(p, 123, 100, 1340174320, rsp);
	
	if(!ret)return 0;
    
	return db_add_item(p, rsp);	
}

int cli_proto_del_item( DEFAULT_ARG )
{
	// only for test this code
	std::vector<db_del_item_request_t> rsp;

	bool ret = parse_db_del_item_request(p, 123, 2,  rsp);

	if(!ret)return 0;
	
	return db_del_item(p, rsp);
}


int send_player_items(player_t* p)
{
	uint32_t  count = 0;
	int idx = sizeof(cli_proto_t);
	int idx2 = idx;

	pack(pkgbuf, count, idx);

	for( uint32_t i =0; i< p->bag->max_grid_count; i++)
	{
		item* p_item = p->bag->item_grid[i];
		if(p_item == NULL)continue;
		
		pack(pkgbuf, p_item->id_, idx);
		pack(pkgbuf, p_item->p_data->item_id_, idx);
		pack(pkgbuf, p_item->cur_heap_count_, idx);
		pack(pkgbuf, p_item->bag_index_, idx);
		pack(pkgbuf, p_item->get_time_, idx);
        pack(pkgbuf, p_item->get_expire_time(), idx);
		count++;
	}
	
	pack(pkgbuf, count, idx2);

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, (char*)pkgbuf, idx, 1);
}

int notify_player_add_item(player_t* p, db_add_item_reply_t* reply)
{
	int idx = sizeof(cli_proto_t);
	
	pack(pkgbuf, reply->id, idx);
	pack(pkgbuf, reply->grid_index, idx);
	pack(pkgbuf, reply->item_id, idx);
	pack(pkgbuf, reply->item_count, idx);
	pack(pkgbuf, reply->get_time, idx);

	init_cli_proto_head(pkgbuf, p, cli_proto_add_item_cmd, idx);
	return send_to_player(p, (char*)pkgbuf, idx, 0);
}

int notify_player_del_item(player_t* p, db_del_item_reply_t* reply)
{
	int idx = sizeof(cli_proto_t);

	pack(pkgbuf, reply->grid_index, idx);
	pack(pkgbuf, reply->item_id, idx);
	pack(pkgbuf, reply->item_count, idx);
	
	init_cli_proto_head(pkgbuf, p, cli_proto_del_item_cmd, idx);
	return send_to_player(p, (char*)pkgbuf, idx, 0);
}
