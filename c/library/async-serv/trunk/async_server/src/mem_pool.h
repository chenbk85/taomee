#ifndef ASYNC_SERVER_MEM_POOL_H
#define ASYNC_SERVER_MEM_POOL_H

#include <stdint.h>
#include <stdlib.h>

#define BUF_LEN (8192 - sizeof(struct buffer_t))

struct buffer_t {
    uint32_t dat_len;
    uint8_t buf[];
};

static inline struct buffer_t *get_buffer()
{
    struct buffer_t *buffer = (struct buffer_t *)malloc(8192);
    if (!buffer)
        return NULL;

    return buffer;
}

static inline void del_buffer(struct buffer_t *buffer)
{
    free(buffer);
}

#endif
