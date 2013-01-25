#ifndef __JOB_FACTORY_H__
#define __JOB_FACTORY_H__
#include "cross_comm.hpp"
BEGIN_NAME_SPACE(CROSS_DISPATCHER)

IJobDispatcher* CreateJobDispatcher();

END_NAME_SPACE()
#endif