#ifndef __SVC_HASH_TABLE_H__
#define __SVC_HASH_TABLE_H__


#include "svc.h"

/* hash-table 最多级别数量 */
#define MAX_NODES_NUM_HTAB_SIZE			(100)

size_t get_svc_hash_table_size(uint32_t svc_gameid, size_t node_size,
		uint32_t *row_num, uint32_t **nodes_num, uint32_t **mods);


#endif /* __SVC_HASH_TABLE_H__ */
