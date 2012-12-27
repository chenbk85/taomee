/*
 * =====================================================================================
 *
 *       Filename:  mempool.c
 *
 *    Description:  内存池
 *
 *        Version:  1.0
 *        Created:  2010年12月03日 11时44分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jerryshao (邵赛赛), jerryshao@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "mempool.h"

#define WORD_SIZE (sizeof(void*))
#define DEFAULT_ALLOC_SIZE 100

static mempool_t* var_mempool_get(variable_mempool_t* pool, size_t item_size);
static mempool_t* var_mempool_put(variable_mempool_t* pool, size_t item_size, mempool_t* sub_pool);


mempool_t* mempool_create(size_t item_size, size_t alloc_size)
{
    mempool_t *p = (mempool_t*)calloc(1, sizeof(mempool_t));
    if(p == NULL) return NULL;

    if(item_size < WORD_SIZE) item_size = WORD_SIZE;
    if(item_size%WORD_SIZE)
        item_size = item_size + WORD_SIZE - item_size % WORD_SIZE;

    p->item_size = item_size;
    int pagesize = getpagesize();
    if(alloc_size <= item_size * DEFAULT_ALLOC_SIZE + WORD_SIZE)
        alloc_size = item_size * DEFAULT_ALLOC_SIZE + WORD_SIZE;
    if(alloc_size <= DEFAULT_ALLOC_SIZE * (size_t)pagesize)
        alloc_size = DEFAULT_ALLOC_SIZE * pagesize;

    if(alloc_size%pagesize)
        alloc_size = alloc_size + pagesize - alloc_size % pagesize;

    p->alloc_size = alloc_size;

    p->alloc_ptr = (char*)malloc(alloc_size);
    if(p->alloc_ptr == NULL)
    {
        free(p);
        return NULL;
    }

    p->total_alloc_item = (alloc_size - WORD_SIZE) / item_size;

    *((void**)p->alloc_ptr) = NULL;

    return p;
}

void* mempool_alloc(mempool_t *pool)
{
    if(!pool)
    {
        return NULL;
    }

    void *p = NULL;
    if(pool->reuse_ptr)
    {
        p = pool->reuse_ptr;
        pool->reuse_ptr = *(char **)p;
    }
    else
    {
        if(pool->alloc_pool_pos == pool->total_alloc_item)
        {/* malloc的内存已经分配满了,需要再分配 */
            p = malloc(pool->alloc_size);
            if(p == NULL) return NULL;

            *(void**)p = pool->alloc_ptr;
            pool->alloc_ptr = (char *)p;

            pool->alloc_pool_pos = 0;
        }

        p = pool->alloc_ptr + WORD_SIZE + pool->item_size * pool->alloc_pool_pos++;


    }

    mempool_inc_refcount(pool);
    return p;

}

void* mempool_calloc(mempool_t *pool)
{
    if(!pool)
    {
        return NULL;
    }

    void *p = mempool_alloc(pool);
    if(p)
    {
        memset(p, 0, sizeof(pool->item_size));
    }

    return p;
}

int mempool_inc_refcount(mempool_t *pool)
{
    if(!pool)
    {
        return -1;
    }

    pool->ref_count++;

    return 0;
}

int mempool_free(mempool_t *pool, void* data)
{
    if(!pool)
    {
        return -1;
    }

    *(void**)data = pool->reuse_ptr;
    pool->reuse_ptr = (char *)data;
    pool->ref_count--;

    return 0;
}

int mempool_destroy(mempool_t *pool)
{
    if(!pool)
        return -1;

    if(pool->ref_count <= 0)
    {
        void *p = NULL;
        while((p = pool->alloc_ptr))
        {
            pool->alloc_ptr = *((char**)pool->alloc_ptr);
            free(p);
        }

        free(pool);
        pool = NULL;
    }

    return 0;
}


/* *************variable mempool************** */
static mempool_t* var_mempool_get(variable_mempool_t *pool, size_t item_size)
{
    if(!pool || item_size == 0)
    {
        return NULL;
    }

    variable_mempool_entry_t *p_entry = pool->mempool_table[item_size % pool->table_size];

    mempool_t *p_pool = NULL;
    if(p_entry)
    {
        variable_mempool_entry_t *entry = NULL;
        list_for_each_entry(entry, &(p_entry->list_entry), list_entry){
            if(entry->item_size == item_size)
            {
                p_pool = entry->pool;
                break;
            }
        }
    }

    return p_pool;
}

static mempool_t* var_mempool_put(variable_mempool_t* pool, size_t item_size, mempool_t* sub_pool)
{
    if(!pool || !sub_pool)
        return NULL;

    variable_mempool_entry_t **pp_entry = &(pool->mempool_table[item_size % pool->table_size]);
    if(*pp_entry)
    {
        variable_mempool_entry_t *tmp_entry = NULL;
        list_for_each_entry(tmp_entry, &((*pp_entry)->list_entry), list_entry){
            if(tmp_entry->item_size == item_size)
            {
                tmp_entry->pool = sub_pool;
                return sub_pool;
            }
        }
    }
    else
    {
        *pp_entry = (variable_mempool_entry_t *)
            calloc(1, sizeof(variable_mempool_entry_t));
        if(!(*pp_entry)) return NULL;
        INIT_LIST_HEAD(&((*pp_entry)->list_entry));
    }

    /* 链表没有节点或是链表中没有匹配的节点，需要新增节点 */
    variable_mempool_entry_t *node =
        (variable_mempool_entry_t *)
        calloc(1, sizeof(variable_mempool_entry_t));
    if(!node)
    {
        free(*pp_entry);
        return NULL;
    }
    list_add(&(node->list_entry), &((*pp_entry)->list_entry));
    node->item_size = item_size;
    node->pool = sub_pool;

    return sub_pool;
}


variable_mempool_t* var_mempool_create(unsigned int reg_table_size, unsigned int multiple)
{
    if(!reg_table_size || !multiple)
        return NULL;

    variable_mempool_t *pool = (variable_mempool_t *)
        calloc(1, sizeof(variable_mempool_t));
    if(!pool)
        return NULL;

    pool->table_size = reg_table_size;
    pool->multiple = multiple;

    pool->mempool_table = (variable_mempool_entry_t **)
        calloc(reg_table_size, sizeof(variable_mempool_entry_t*));
    if(!pool->mempool_table)
    {
        free(pool);
        return NULL;
    }

    return pool;
}

void* var_mempool_alloc(variable_mempool_t* var_mempool, unsigned int item_size)
{
    if(!var_mempool)
        return NULL;

    mempool_t *mempool = NULL;

    if(item_size%WORD_SIZE)
    {
        item_size += item_size - item_size % WORD_SIZE;
    }

    if((mempool = var_mempool_get(var_mempool, item_size)))
        return mempool_alloc(mempool);

    mempool = mempool_create(item_size, var_mempool->multiple);
    var_mempool_put(var_mempool, item_size, mempool);
    return mempool_alloc(mempool);
}

void* var_mempool_calloc(variable_mempool_t* var_mempool, unsigned int item_size)
{
    if(!var_mempool) return NULL;

    void *p = var_mempool_alloc(var_mempool, item_size);
    if(p)
    {
        memset(p, 0, sizeof(item_size));
    }

    return p;
}

int var_mempool_free(variable_mempool_t* var_mempool, void* data, unsigned int data_size)
{
    if(!var_mempool)
    {
        return -1;
    }

    mempool_t *mempool = var_mempool_get(var_mempool, data_size);
    if(mempool)
    {
        return mempool_free(mempool, data);
    }
    else
    {
        return -1;
    }
}

int var_mempool_destroy(variable_mempool_t *var_mempool)
{
    if(!var_mempool)
        return -1;

    unsigned int i = 0;
    for(i = 0; i < var_mempool->table_size; i++)
    {
        variable_mempool_entry_t *p_entry = var_mempool->mempool_table[i];
        if(p_entry)
        {
            struct list_head *l = NULL;
            list_for_each(l, &(p_entry->list_entry)){
                variable_mempool_entry_t *t = list_entry(l, variable_mempool_entry_t, list_entry);

                mempool_destroy(t->pool);
                struct list_head tmp = *l;
                list_del(l);
                free(t);
                t = NULL;
                l = &tmp;
            }
            free(p_entry);
            p_entry = NULL;
        }
    }
    free(var_mempool->mempool_table);
    var_mempool->mempool_table = NULL;

    free(var_mempool);
    var_mempool = NULL;

    return 0;

}
