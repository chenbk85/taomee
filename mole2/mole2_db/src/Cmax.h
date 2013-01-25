#ifndef  CMAX_INC
#define  CMAX_INC

#include "CtableRoute.h"
#include "proto.h"
#include "db_error.h"
#include "Ccommon.h"


#define IS_TYPE_MAX_2(type)      ((type) & 0x80000000)

class Cmax:public CtableRoute
{
private:
	int insert(uint32_t userid, uint32_t type,uint32_t id,int count);
	int set(uint32_t userid, uint32_t type,uint32_t id,uint32_t count,uint32_t day);
public:
	Cmax(mysql_interface * db);

	int get(uint32_t userid, uint32_t type,uint32_t id,uint32_t *count,uint32_t *day);
	int add(uint32_t userid, uint32_t type,uint32_t id,int count,uint32_t *cur_cnt);
	int del(uint32_t userid, uint32_t type,uint32_t id);

};

#endif

