#ifndef __I_TAOMEE_JOB_OBJECT_H__
#define __I_TAOMEE_JOB_OBJECT_H__
#include "cross_comm.hpp"
BEGIN_NAME_SPACE(CROSS_DISPATCHER)

class IJobObject
{
public:
	virtual ~IJobObject(){}
public:
	/*!
	  * @Function:Release
	  * @Description: release all source that hold by object
	  * @Param:none
	  * @return:void
	  */
	virtual void Release() = 0;

	/*!
	  * @Function:GetObjId
	  * @Description: get object's id
	  * @Param:none
	  * @return:const char*
	  */
	virtual const char* GetObjId() const = 0;
};

END_NAME_SPACE()
#endif