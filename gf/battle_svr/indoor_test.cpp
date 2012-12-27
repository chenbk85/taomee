#include <map>

using namespace std;

extern "C" {
#include <libtaomee/log.h>
}
#include "cli_proto.hpp"
#include "player.hpp"
#include "battle.hpp"
#include "indoor_test.hpp"


int notify_client_btl_monsters_dead(Player* player)
{
    PlayerSet::iterator it = player->cur_map->monsters.begin(); //p->btl->stage()
    for (; it != player->cur_map->monsters.end(); ++it) {
        Player* monster = *it;
        if ( !(monster->is_dead())  && (monster->mon_info->type != 1) ) {
            monster->suicide();
        }
    }
    //player->btl->set_all_monsters_dead();
    return 0;
}

void change_player_attack(Player* p)
{
    p->atk += 10000;
}

int change_game_value_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
    uint32_t tmp = 0; //*reinterpret_cast<uint32_t*>(body);
    uint32_t idx = 0;
    uint32_t para[64] = {0};
    char* p_tmp = (char *)(body);
    //sscanf(p_tmp, "%s|%s")
    while (*p_tmp != '\0') {
        while ( is_digit(*p_tmp) ) {
            tmp = tmp * 10 + (*p_tmp++ - 0x30);
        }
        para[idx++] = tmp;
        tmp = 0;
        //if (*p_tmp == '|') {
        p_tmp++;
        //}
    }
    TRACE_LOG("=======DUMPING ===========");
    for (int i = 0; i < 9; i++) {
        TRACE_LOG("========= %u", para[i]);
    }
    switch (para[0]) {
        case 1:
            change_player_attack(p);
            break;
        case 2:
            notify_client_btl_monsters_dead(p);
            break;
        default:
            break;
    }
    p->waitcmd = 0;
    return 0;
}
