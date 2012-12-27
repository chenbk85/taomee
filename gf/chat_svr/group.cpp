#include <cerrno>
#include <cstdlib>
#include <string>
#include <vector>
#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/inet/byteswap.hpp>
#include <algorithm>
extern "C" 
{
#include <fcntl.h>
#include <libtaomee/timer.h>
#include <libtaomee/utils.h>
#include <libtaomee/conf_parser/config.h>
#include <async_serv/net_if.h>
#include  <libtaomee/project/stat_agent/msglog.h>
#include <libtaomee/project/utilities.h>
}
#include "chat.hpp"
#include "group.hpp"
#include "player.hpp"
using namespace std;
using namespace taomee;


void TalkGroup::send_to_group(void* pkg, uint32_t len) {
	DEBUG_LOG("SEND 2 GROUP %u", group_id());
	PlayerMap::iterator it = talk_obj_map.begin();
	PlayerMap::iterator end = talk_obj_map.end();
	for (; it!= end; ++it) {
		Player* p = it->second;
		send_to_player(p, pkg, len, 1);
	}
}


