#include "cli_proto.hpp"
#include "player.hpp"
#include "player_attribute.hpp"


bool calc_player_attr(player_t* p)
{
	
	return true;	
}

int notify_player_gold(player_t * p)
{
    cli_proto_obj_attr_chg_out out;
    out.user_id = p->id;
    obj_attr_chg change;
    change.obj_attr_type = OBJ_ATTR_GOLD;
    change.obj_attr_value = p->get_player_attr_value(OBJ_ATTR_GOLD);
    out.obj_attr_array.push_back(change);

    return send_to_player(p, &out, cli_proto_obj_attr_chg_cmd, 0);
}
