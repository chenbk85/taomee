extern "C"
{
#include <glib.h>
#include <assert.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>
}

#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include <libtaomee++/random/random.hpp>

#include "player_attribute.hpp"
#include "player.hpp"
#include "cli_proto.hpp"

/*
void notify_player_attr_change(Player* p, uint64_t type, int clear_waitcmd)
{
	uint32_t type_count = 0;
	int idx =sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, p->id, idx);
	int idx2 = idx;
	pack(pkgbuf, type_count, idx);

	if(check_type(type, obj_hp_type))
	{
		type_count++;
		pack(pkgbuf, (uint32_t)(obj_hp_type), idx);
		pack(pkgbuf, p->life, idx);	
	}

	pack(pkgbuf, type_count, idx2);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_obj_attr_chg_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	send_to_player(p, pkgbuf, idx, clear_waitcmd);	
}
*/
