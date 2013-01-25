#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "common.h"
#include "struct.h"
#include "shm_keys.h"
#include "shm.h"
#include "util.h"
#include "hash_table.h"

#include "svc_hash_table.h"


/* common config */
static uint32_t _nodes_num[] = {
	25000, 25000, 25000, 25000, 25000, 25000, 25000, 25000, 25000, 25000,
	25000, 25000, 25000, 25000, 25000, 25000, 25000, 25000, 25000, 25000,
};
static uint32_t _mods[] = {
	24989, 24979, 24977, 24971, 24967, 24953, 24943, 24923, 24919, 24917, 
	24907, 24889, 24877, 24859, 24851, 24847, 24841, 24821, 24809, 24799,
};
COMPILE_ASSERT(ARRAY_SIZE(_nodes_num) <= MAX_NODES_NUM_HTAB_SIZE);
COMPILE_ASSERT(ARRAY_SIZE(_nodes_num) == ARRAY_SIZE(_mods));
static uint32_t _row_num = ARRAY_SIZE(_nodes_num);

/* seer's big hash-table config */
static uint32_t _seer_nodes_num[] = {
	30000, 30000, 30000, 30000, 30000, 30000, 30000, 30000, 30000, 30000,
	30000, 30000, 30000, 30000, 30000, 30000, 30000, 30000, 30000, 30000,
};
static uint32_t _seer_mods[] = {
	29989, 29983, 29959, 29947, 29927, 29921, 29917, 29881, 29879, 29873, 
	29867, 29863, 29851, 29837, 29833, 29819, 29803, 29789, 29761, 29759,
};
COMPILE_ASSERT(ARRAY_SIZE(_seer_nodes_num) <= MAX_NODES_NUM_HTAB_SIZE);
COMPILE_ASSERT(ARRAY_SIZE(_seer_nodes_num) == ARRAY_SIZE(_seer_mods));
static uint32_t _seer_row_num = ARRAY_SIZE(_seer_nodes_num);



size_t get_svc_hash_table_size(uint32_t svc_gameid, size_t node_size,
		uint32_t *row_num, uint32_t **nodes_num, uint32_t **mods)
{
	switch (svc_gameid) {
	case SVC_GAMEID_SEER:
		*row_num = _seer_row_num;
		*nodes_num = _seer_nodes_num;
		*mods = _seer_mods;
		break;

	default:
		*row_num = _row_num;
		*nodes_num = _nodes_num;
		*mods = _mods;
		break;
	}

	return calc_hash_table_size(node_size, *row_num, *nodes_num);
}
