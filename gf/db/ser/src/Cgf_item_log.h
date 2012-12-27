#ifndef  GF_CGF_ITEM_LOG_H
#define    GF_CGF_ITEM_LOG_H

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_item_log : public CtableRoute100x10{
	public:
		Cgf_item_log(mysql_interface * db ); 

		int insert_use_item_log(userid_t userid, uint32_t role_regtime, uint32_t itemid, uint32_t count);
};

#endif

