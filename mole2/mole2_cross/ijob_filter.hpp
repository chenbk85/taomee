#ifndef __I_TAOMEE_JOB_FILTER_H__
#define __I_TAOMEE_JOB_FILTER_H__
#include "ijob_object.hpp"
BEGIN_NAME_SPACE(CROSS_DISPATCHER)
class IJobFilter : public IJobObject
{
public:
	virtual ~IJobFilter(){}
public:
	virtual int DoFilter(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess) = 0;
};
END_NAME_SPACE()
#endif

