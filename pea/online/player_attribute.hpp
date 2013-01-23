#ifndef PLAYER_ATTRIBUTE_HPP_
#define PLAYER_ATTRIBUTE_HPP_

class player_t;

#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>

extern "C"
{
#include <libtaomee/list.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/project/types.h>
#include <async_serv/dll.h>
}

#include <map>
#include <set>
#include <list>

#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "proto.hpp"


using namespace std;
using namespace taomee;

bool calc_player_attr(player_t* p);




int notify_player_gold(player_t * p);



#endif
