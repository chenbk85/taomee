#include <cerrno>
#include <cstdlib>
#include <cstring>

#include <vector>

using namespace std;
#include <libtaomee++/inet/pdumanip.hpp>

using namespace taomee;

extern "C" {
#include <fcntl.h>
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
}

#include "utils.hpp"
#include "proto.hpp"
#include <map>
#include <set>

#include "cross_comm.hpp"
#include "proto.hpp"
#include "cjob_dispatcher.hpp"
BEGIN_NAME_SPACE(CROSS_DISPATCHER)
IJobDispatcher* CreateJobDispatcher()
{
	return new CJobDispatcher();
}	
END_NAME_SPACE()


