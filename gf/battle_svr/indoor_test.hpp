#ifndef __INDOOR_TEST__
#define __INDOOR_TEST__

#include "player.hpp"

inline bool is_digit(char c)
{
    if (c > 0x39 || c < 0x30) {
        return false;
    }
    return true;
}

int change_game_value_cmd(Player* p, uint8_t* body, uint32_t bodylen);

int notify_client_btl_monsters_dead(Player* player);

#endif
