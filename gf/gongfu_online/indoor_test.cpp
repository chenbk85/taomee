#include <libtaomee++/inet/pdumanip.hpp>

using namespace taomee;

#include "cli_proto.hpp"
#include "indoor_test.hpp"
#include "utils.hpp"
#include "player.hpp"
#include "battle.hpp"

//-----------------------------------------------------------
//-----------------------------------------------------------

int change_game_battle_value_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    uint8_t cmd_str[64] = {0};
    int idx = 0;
    //unpack(body, seq, idx);
    unpack(body, cmd_str, 64, idx);

    if ( !(p->battle_grp)) {
        return -1;
    }
    idx = sizeof(btl_proto_t);
	pack(btlpkgbuf, cmd_str, 64, idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_change_game_value);

	send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
    p->waitcmd = 0;
    return 0;
}
//--------------------------------------------------------------------

