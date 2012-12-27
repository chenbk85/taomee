#ifndef  GF_CGF_GET_SHOPITEM_LOG_H
#define    GF_CGF_GET_SHOPITEM_LOG_H

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_get_shopitem_log : public CtableRoute100x10{
	public:
		Cgf_get_shopitem_log(mysql_interface * db ); 

		int insert_item_log(userid_t userid, uint32_t role_regtime, uint32_t itemid, uint32_t count);
};

#endif

