#ifndef  GF_CGF_AMB_H
#define    GF_CGF_AMB_H
#include "CtableRoute100.h"
#include "proto.h"
#include "benchapi.h"


class Cgf_amb:public CtableRoute100{
	public:
		Cgf_amb(mysql_interface * db ); 
		int set_amb_info(uint32_t userid, uint32_t parentuid);
		int get_amb_info(uint32_t userid, gf_get_amb_info_out* p_out);
		int del_amb_info(uint32_t userid);
};
#endif 

