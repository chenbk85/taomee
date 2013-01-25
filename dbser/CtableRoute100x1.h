#ifndef _CTABLEROUTE100X1_INC_
#define _CTABLEROUTE100X1_INC_

#include "CtableRoute.h"

class CtableRoute100x1 : public CtableRoute
{
protected:
	virtual char* get_table_name(uint32_t id);
public:
	virtual ~CtableRoute100x1(void){}
	CtableRoute100x1(mysql_interface* db, const char* db_name_pre,const char* table_name_pre,const char* id_name,const char *key2_name="");
};

#endif
