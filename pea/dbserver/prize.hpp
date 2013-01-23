#ifndef PRIZE_H
#define PRIZE_H

#include "CtableRoute100x10.h"
#include "proto.hpp"
#include "benchapi.h"

class pea_prize: public CtableRoute100x10
{
public:
	pea_prize(mysql_interface* db);	
	int add( db_user_id_t db_user_id,  uint32_t prize_id, int32_t add_time);
	int del( db_user_id_t db_user_id,  uint32_t prize_id);
	int get_prize_list( db_user_id_t db_user_id, std::vector<db_prize_t> &prize_list);
};


#endif
