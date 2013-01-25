#ifndef __I_TAOMEE_JOB_DISPATCHER_H__
#define __I_TAOMEE_JOB_DISPATCHER_H__
#include "ijob_object.hpp"
BEGIN_NAME_SPACE(CROSS_DISPATCHER)
class IJobDispatcher : public IJobObject
{
public:
	virtual ~IJobDispatcher(){}
public:
	virtual int Dispatch(uint16_t cmd, void* lparam, void* wparam) = 0;
};
END_NAME_SPACE()

#define BEGIN_JOB_DISPATCHER_MAP() int Dispatch(uint16_t cmd, void* lparam, void* wparam){\
	mole2cross_proto_t* pkg = NULL;\
	CONVERT_PTR_TO_PTR(lparam, pkg);\
	DEBUG_LOG("dispatch command %u", cmd);\
	switch(cmd){\

#define REGISTER_JOB_DISPATCH_HANDLE(cmdid, call_back_function) case cmdid: {fdsession_t* fdsess = NULL; CONVERT_PTR_TO_PTR(wparam, fdsess); \
																	DEBUG_LOG("dispatch proto command %u pkg_len=%u head_len=%u", cmd, pkg->len, sizeof(mole2cross_proto_t));\
																	return call_back_function(pkg, pkg->len - sizeof(mole2cross_proto_t), fdsess);} break;

#define REGISTER_JOB_TIMOUT_HANDLE(cmdid, call_back_function) case cmdid: return call_back_function(lparam, wparam);

#define REGISTER_JOB_EVENT_HANDLE(cmdid, call_back_function) case cmdid: return call_back_function(lparam, wparam);

#define REGISTER_JOB_UNKNOW_EVENT_HANDLE(call_back_function) default: return call_back_function(cmdid, lparam, wparam);

#define END_JOB_DISPATCHER_MAP() }ERROR_LOG("commandid %u is not support", cmd); return -1;}
#endif