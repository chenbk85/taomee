#ifndef _CGF_AP_TOPLIST_H_
#define _CGF_AP_TOPLIST_H_

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_ap_toplist: public Ctable
{
public:
	Cgf_ap_toplist(mysql_interface * db );
	int get_ap_toplist(uint32_t begin_index, uint32_t end_index, gf_get_ap_toplist_out_element** pdata, uint32_t* count);
};





#endif
