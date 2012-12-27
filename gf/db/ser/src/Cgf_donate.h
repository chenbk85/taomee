#ifndef _CGF_DONATE_H_
#define _CGF_DONATE_H_

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_donate : public Ctable
{
public:
	Cgf_donate( mysql_interface *db );
	int get_red_black_count(uint32_t* red,  uint32_t* black);
	int add_red(uint32_t add = 1);
	int add_black(uint32_t add = 1);
};

#endif
