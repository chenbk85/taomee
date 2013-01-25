#include <stdio.h>
#include <stdint.h>

#include "hash_table.h"

/**
 * @return 0: fail, >0: success
 */
size_t calc_hash_table_size(size_t node_size, uint32_t row_num, uint32_t *nodes_num)
{
	int i;
	uint32_t tot_node_count = 0;

	if (row_num > HASH_MAX_ROW) return 0;

	for (i = 0; i < row_num; i++) {
		tot_node_count += nodes_num[i];
	}
	return (node_size * tot_node_count);
}

int hash_table_init(struct hash_table_t *hash_table, void *table, size_t table_size, size_t node_size, uint32_t row_num, uint32_t *nodes_num, uint32_t *mods, int(*compare)(const void *key, const void *node))
{
	int i;
	void *row;
	uint32_t count;

#define CHECK_AND_SET(x) do { if (!(hash_table->x = x)) { printf("line: %d\n", __LINE__); return -1; } } while(0)
	CHECK_AND_SET(compare);
	CHECK_AND_SET(table);
	CHECK_AND_SET(node_size);
	CHECK_AND_SET(row_num);
#undef CHECK_AND_SET
	if (row_num > HASH_MAX_ROW) { return -1; }

#define CHECK_AND_SET(x) do { if (!(hash_table->x[i] = x[i])) { printf("line: %d\n", __LINE__); return -1; } } while(0)
	for (i = 0, count = 0; i < row_num; i++) {
		/* 确保 mods 有效 */
		if (mods[i] > nodes_num[i]) {
			return -1;
		}

		CHECK_AND_SET(nodes_num);
		CHECK_AND_SET(mods);

		hash_table->row_nodes_size[i] = node_size * nodes_num[i];
		if (i == 0) {
			row = hash_table->table;
		} else {
			row = hash_table->row_starts[i-1] + hash_table->row_nodes_size[i-1];
		}
		hash_table->row_starts[i] = row;
		count += nodes_num[i];
	}
#undef CHECK_AND_SET
	if (table_size != node_size * count ) { return -1; }
	hash_table->total_node_num = count;

	return 0;
}

void *hash_table_lookup_node(struct hash_table_t *hash_table, void *key, uint32_t short_key)
{
	int i;
	void *row, *node;
	size_t node_size = hash_table->node_size;

	for (i = 0; i < hash_table->row_num; i++) {
		row = hash_table->row_starts[i];
		node = row + node_size * (short_key % hash_table->mods[i]);
		if (hash_table->compare(key, node) == 0) { return node; }
	}

	return NULL;
}

void *hash_table_lookup_node_posinfo(struct hash_table_t *hash_table, void *key, uint32_t short_key, uint32_t *row_idx, uint32_t *col_idx)
{
	int i;
	uint32_t col;
	void *row, *node;
	size_t node_size = hash_table->node_size;

	for (i = 0; i < hash_table->row_num; i++) {
		row = hash_table->row_starts[i];
		col = short_key % hash_table->mods[i];
		node = row + node_size * col;
		if (hash_table->compare(key, node) == 0) {
			*row_idx = i;
			*col_idx = col;
			return node;
		}
	}

	return NULL;
}

void *hash_table_lookup_node_ex(struct hash_table_t *hash_table, void *empty_key, void *key, uint32_t short_key, int *exist)
{
	int i;
	void *row, *node, *empty_node = NULL, *exist_node = NULL;
	size_t node_size = hash_table->node_size;

	for (i = 0; i < hash_table->row_num; i++) {
		row = hash_table->row_starts[i];
		node = row + node_size * (short_key % hash_table->mods[i]);
		if (hash_table->compare(key, node) == 0) {
			exist_node = node;
			if (exist) {
				*exist = 1;
			}
			break;
		}

		if (empty_node == NULL && hash_table->compare(empty_key, node) == 0) {
			empty_node = node;
		}
	}

	return (exist_node ? exist_node : empty_node);
}

void *hash_table_lookup_node_ex_posinfo(struct hash_table_t *hash_table, void *empty_key, void *key, uint32_t short_key, int *exist, uint32_t *row_idx, uint32_t *col_idx)
{
	int i;
	uint32_t col, erow, ecol;
	void *row, *node, *empty_node = NULL, *exist_node = NULL;
	size_t node_size = hash_table->node_size;

	for (i = 0; i < hash_table->row_num; i++) {
		row = hash_table->row_starts[i];
		col = (short_key % hash_table->mods[i]);
		node = row + node_size * col;
		if (hash_table->compare(key, node) == 0) {
			exist_node = node;
			if (exist) {
				*exist = 1;
			}
			*row_idx = i;
			*col_idx = col;
			break;
		}

		if (empty_node == NULL && hash_table->compare(empty_key, node) == 0) {
			erow = i;
			ecol = col;
			empty_node = node;
		}
	}

	if (!exist_node) {
		*row_idx = erow;
		*col_idx = ecol;
	} 
	return (exist_node ? exist_node : empty_node);
}

void *hash_table_walk(struct hash_table_t *hash_table, uint32_t *row_idx, uint32_t *col_idx)
{
	void *node = hash_table_get_node(hash_table, *row_idx, *col_idx);

	if (node == NULL) { return NULL; }

	if (++ *col_idx >= hash_table->nodes_num[*row_idx]) {
		*col_idx = 0;
		++ *row_idx;
	}

	return node;
}

void *hash_table_get_node(struct hash_table_t *hash_table, uint32_t row_idx, uint32_t col_idx)
{
	void *node = NULL, *row;
	size_t node_size = hash_table->node_size;

	if (row_idx >= hash_table->row_num
		|| col_idx >= hash_table->nodes_num[row_idx]) {
		return NULL;
	}

	row = hash_table->row_starts[row_idx];
	node = row + node_size * col_idx;

	return node;
}
