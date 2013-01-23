#ifndef PLAYER_ATTRIBUTE_HPP_
#define PLAYER_ATTRIBUTE_HPP_
#include <boost/pool/object_pool.hpp>
#include <libtaomee++/event/eventable_obj.hpp>
#include <libtaomee++/event/event_mgr.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <libtaomee++/memory/mempool.hpp>
#include "pea_common.hpp"
using namespace std;
using namespace taomee;

#define select_type(x)    	   (1<<x)                
#define check_type(num, x)     ( num & (1<<x))  //判断num第X位是否为1

class Player;






#endif
