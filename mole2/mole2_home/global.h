#ifndef MOLE2_GLOBAL_C
#define MOLE2_GLOBAL_C


#include <time.h>
#include <stdint.h>
#include <glib.h>

extern "C"{
#include <libtaomee/timer.h>
#include <libtaomee/list.h>
#include <async_serv/net_if.h>
}

#include "onlinehome.h"
#include "pvp.h"
#include "market.h"

#define	MAX_ONLINE_NUM	 5001

#define LO32(x)	(uint32_t)(x)
#define HI32(x)	(uint32_t)((x) >> 32)
extern GHashTable*	all_home;
extern fdsession_t*	all_fds[MAX_ONLINE_NUM] ;

extern list_head_t	wait_head;
extern uint32_t 	next_dupid;
extern GHashTable* 	all_player;
extern GHashTable* 	all_games;

extern Cpvp  g_pvp[2];
extern Cmarket g_market;
enum{
	cli_err_user_offline        = 200003,
};
#endif
