#include "global.h"

fdsession_t*	all_fds[MAX_ONLINE_NUM] = {NULL};
GHashTable*		all_home = NULL;

list_head_t 	wait_head;
uint32_t 		next_dupid = 0;
GHashTable* 	all_player = NULL;
GHashTable* 	all_games = NULL;

