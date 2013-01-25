#ifndef MC_PROTO_H_
#define MC_PROTO_H_

#include "global.h"

typedef struct reload_proto {
	uint16_t		online_id;
	uint32_t		type;
}__attribute__((packed)) reload_proto_t;

enum {
	reload_type_all		= 0,
	reload_type_item 	= 2,
	reload_type_cloth	= 3,
	reload_type_beast	= 4,
	reload_type_mail	= 5,
	reload_type_rand	= 6,
	reload_type_task    = 7,
	reload_beast_grp	= 8,
	reload_type_map		= 9,
	reload_skills		= 11,
	reload_holiday		= 12,
	reload_picsrv		= 13,
	reload_sysinfo		= 14,
	reload_shop_item	= 15,
	reload_shop_skill	= 16,
	reload_suit			= 17,
	reload_title		= 18,
	reload_init_prof	= 19,
	reload_exchanges	= 20,
	reload_commodity	= 21,
	reload_type_max
};

int handle_mc_return(reload_proto_t* pkg, int len);

#endif 

