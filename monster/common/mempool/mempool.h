/*
 * =====================================================================================
 *
 *       Filename:  mempool.h
 *
 *    Description:  内存池
 *
 *        Version:  1.0
 *        Created:  2010年12月03日 11时24分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jerryshao (邵赛赛), jerryshao@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */

#ifndef _H_MEMPOOL_H_
#define _H_MEMPOOL_H_

#ifdef _cplusplus
extern "C"
{
#endif

#include <unistd.h>
#include "libtaomee/list.h"

typedef struct mempool
{
    size_t item_size;
    size_t alloc_size;
    size_t total_alloc_item;

    char *reuse_ptr;
    char *alloc_ptr;
    size_t alloc_pool_pos;

    int ref_count;

} mempool_t;

extern mempool_t* mempool_create(size_t item_size, size_t alloc_size);
extern void* mempool_alloc(mempool_t *pool);
extern void* mempool_calloc(mempool_t *pool);
extern int mempool_inc_refcount(mempool_t *pool);
extern int mempool_free(mempool_t* pool, void *data);
extern int mempool_destroy(mempool_t *pool);


typedef struct variable_mempool_entry
{
    size_t item_size;
    mempool_t *pool;

    struct list_head list_entry;
} variable_mempool_entry_t;

typedef struct variable_mempool
{
    unsigned int table_size;
    unsigned int table_count;

    variable_mempool_entry_t **mempool_table;

    int multiple;
} variable_mempool_t;

extern variable_mempool_t* var_mempool_create(unsigned int reg_table_size, unsigned int multiple);
extern void* var_mempool_alloc(variable_mempool_t* var_mempool, unsigned int item_size);
extern void* var_mempool_calloc(variable_mempool_t* var_mempool, unsigned int item_size);
extern int var_mempool_free(variable_mempool_t* var_mempool, void *data, unsigned int data_size);
extern int var_mempool_destroy(variable_mempool_t* var_mempool);

#ifdef _cplusplus
}
#endif

#endif
