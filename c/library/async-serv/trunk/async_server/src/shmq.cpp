#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>

#include "log.h"
#include "shmq.h"

struct shm_queue_mgr_t g_shm_queue_mgr;

int shmq_push(struct shm_queue_t *sq, struct shm_block_t *sb, const uint8_t *buf)
{
    uint32_t total_len = sb->len + sizeof(struct shm_block_t);
    uint32_t w;
    uint32_t r;
    uint32_t s;
    uint32_t pos;

    w = sq->head->w_pos;
    r = sq->head->r_pos;
    s = w >= r ? sq->size - w + r : r - w;
    if (s < total_len + 1) {
        ERROR_LOG("recv queue is full");
        return 1;
    }

    pos = w;
    /*
     * write buf head.
     */
    if (pos + sizeof(struct shm_block_t) <= sq->size) {
        memcpy(sq->base + pos, (uint8_t *)sb, sizeof(struct shm_block_t));
        pos += sizeof(struct shm_block_t);
    } else {
        uint32_t n = sq->size - pos;
        memcpy(sq->base + pos, (uint8_t *)sb, n);
        memcpy(sq->base, (uint8_t *)sb + n, sizeof(struct shm_block_t) - n);
        pos = sizeof(struct shm_block_t) - n;
    }
    /*
     * write buf.
     */
    if (pos + sb->len <= sq->size) {
        memcpy(sq->base + pos, buf, sb->len);
        pos += sb->len;
    } else {
        uint32_t n = sq->size - pos;
        memcpy(sq->base + pos, buf, n);
        memcpy(sq->base, buf + n, sb->len - n);
        pos = sb->len - n;
    }

    sq->head->w_pos = pos;
    return 0;
}

int shmq_pull(struct shm_queue_t *sq, struct shm_block_t *sb, uint8_t **buf)
{
    uint32_t w;
    uint32_t r;
    uint32_t s;
    uint32_t pos;

    w = sq->head->w_pos;
    r = sq->head->r_pos;
    s = w >= r ? w - r : sq->size - r + w;
    if (s == 0)
        return 1;

    pos = r;
    /*
     * read head.
     */
    if (pos + sizeof(struct shm_block_t) <= sq->size) {
        memcpy((uint8_t *)sb, sq->base + pos, sizeof(struct shm_block_t));
        pos += sizeof(struct shm_block_t);
    } else {
        uint32_t n = sq->size - pos;
        memcpy((uint8_t *)sb, sq->base + pos, n);
        memcpy((uint8_t *)sb + n, sq->base, sizeof(struct shm_block_t) - n);
        pos = sizeof(struct shm_block_t) - n;
    }

    if (sb->len > 0) {
        if (sb->len > sq->pull_len) {
            uint8_t *tmp = (uint8_t *)realloc(sq->pull_buf, sb->len);
            if (tmp == NULL) { // memory empty
                ERROR_LOG("memory empty realloc");
                if (pos + sb->len <= sq->size) {
                    pos += sb->len;
                } else {
                    uint32_t n = sq->size - pos;
                    pos = sb->len - n;
                }
                sq->head->r_pos = pos;
                return -1;
            }
            sq->pull_buf = tmp;
            sq->pull_len = sb->len;
        }
        *buf = sq->pull_buf;
        /*
         * read buf.
         */
        if (pos + sb->len <= sq->size) {
            memcpy(*buf,  sq->base + pos, sb->len);
            pos += sb->len;
        } else {
            uint32_t n = sq->size - pos;
            memcpy(*buf, sq->base + pos, n);
            memcpy(*buf + n, sq->base, sb->len - n);
            pos = sb->len - n;
        }
    }

    sq->head->r_pos = pos;
    return 0;
}

bool shmq_init(uint32_t channel, uint32_t size)
{
    if (size < sizeof(shm_head_t))
        BOOT_LOG(-1, "create %u shm queue channel size:%u", channel, size);

    int ret = 0;
    g_shm_queue_mgr.channel_num = channel;
    g_shm_queue_mgr.recv_queue = new shm_queue_t[channel];
    g_shm_queue_mgr.send_queue = new shm_queue_t[channel];

    for (uint32_t i = 0; i < channel; ++i) {
        uint8_t *p;

        /* recv queue */
        p = (uint8_t *)mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
        if (p == MAP_FAILED) {
            ret = -1;
            break;
        }

        g_shm_queue_mgr.recv_queue[i].base = p + sizeof(struct shm_head_t);
        g_shm_queue_mgr.recv_queue[i].size = size - sizeof(struct shm_head_t);
        g_shm_queue_mgr.recv_queue[i].pull_buf = (uint8_t *)malloc(8192);
        g_shm_queue_mgr.recv_queue[i].pull_len = 8192;
        g_shm_queue_mgr.recv_queue[i].head = (struct shm_head_t *)p;
        g_shm_queue_mgr.recv_queue[i].head->inited = 0;
        g_shm_queue_mgr.recv_queue[i].head->w_pos = 0;
        g_shm_queue_mgr.recv_queue[i].head->r_pos = 0;
        if (pipe(g_shm_queue_mgr.recv_queue[i].pipe) == -1) {
            ret = -1;
            break;
        }

        /* send queue */
        p = (uint8_t *)mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
        if (p == MAP_FAILED) {
            ret = -1;
            break;
        }

        g_shm_queue_mgr.send_queue[i].base = p + sizeof(struct shm_head_t);
        g_shm_queue_mgr.send_queue[i].size = size - sizeof(struct shm_head_t);
        g_shm_queue_mgr.send_queue[i].pull_buf = (uint8_t *)malloc(8192);
        g_shm_queue_mgr.send_queue[i].pull_len = 8192;
        g_shm_queue_mgr.send_queue[i].head = (struct shm_head_t *)p;
        g_shm_queue_mgr.send_queue[i].head->inited = 0;
        g_shm_queue_mgr.send_queue[i].head->w_pos = 0;
        g_shm_queue_mgr.send_queue[i].head->r_pos = 0;
        if (pipe(g_shm_queue_mgr.send_queue[i].pipe) == -1) {
            ret = -1;
            break;
        }
    }

    BOOT_LOG(ret, "create %u shm queue channel size:%u", channel, size);
}

void shmq_fini()
{
    for (uint32_t i = 0; i < g_shm_queue_mgr.channel_num; ++i) {
        free(g_shm_queue_mgr.recv_queue[i].pull_buf);
        munmap(g_shm_queue_mgr.recv_queue[i].base - sizeof(struct shm_head_t), 
               g_shm_queue_mgr.recv_queue[i].size + sizeof(struct shm_head_t));
        close(g_shm_queue_mgr.recv_queue[i].pipe[0]);
        close(g_shm_queue_mgr.recv_queue[i].pipe[1]);

        free(g_shm_queue_mgr.send_queue[i].pull_buf);
        munmap(g_shm_queue_mgr.send_queue[i].base - sizeof(struct shm_head_t), 
               g_shm_queue_mgr.send_queue[i].size + sizeof(struct shm_head_t));
        close(g_shm_queue_mgr.send_queue[i].pipe[0]);
        close(g_shm_queue_mgr.send_queue[i].pipe[1]);
    }
}

int recv_push(uint32_t channel, struct shm_block_t *sb, const uint8_t *buf, bool safe)
{
    int ret = pruge_recv(channel);

    if (sb->len >= g_shm_queue_mgr.recv_queue[channel].size)
        return -1;

    if (ret == 0) {
        if (shmq_push(&g_shm_queue_mgr.recv_queue[channel], sb, buf) == 0) {
            write_pipe(g_shm_queue_mgr.recv_queue[channel].pipe[1]);
            return 0;
        }
    }

    if (!safe)
        return ret;

    shm_block_t *tmp = (shm_block_t *)malloc(sizeof(struct shm_block_t) + sb->len);
    if (!tmp)
        return -1;

    memcpy(tmp, sb, sizeof(struct shm_block_t));
    memcpy(tmp->buf, buf, tmp->len);
    g_shm_queue_mgr.recv_queue[channel].push_list.push_back(tmp);

    return 0;
}

int recv_pull(uint32_t channel, struct shm_block_t *sb, uint8_t **buf)
{
    return shmq_pull(&g_shm_queue_mgr.recv_queue[channel], sb, buf); 
}

int send_push(uint32_t channel, struct shm_block_t *sb, const uint8_t *buf, bool safe)
{
    int ret = pruge_send(channel);

    if (sb->len >= g_shm_queue_mgr.send_queue[channel].size)
        return -1;

    if (ret == 0) {
        if (shmq_push(&g_shm_queue_mgr.send_queue[channel], sb, buf) == 0) {
            write_pipe(g_shm_queue_mgr.send_queue[channel].pipe[1]);
            return 0;
        }
    }

    if (!safe)
        return ret;

    shm_block_t *tmp = (shm_block_t *)malloc(sizeof(struct shm_block_t) + sb->len);
    if (!tmp)
        return -1;

    memcpy(tmp, sb, sizeof(struct shm_block_t));
    memcpy(tmp->buf, buf, tmp->len);
    g_shm_queue_mgr.send_queue[channel].push_list.push_back(tmp);

    return 0;
}

int send_pull(uint32_t channel, struct shm_block_t *sb, uint8_t **buf)
{
    return shmq_pull(&g_shm_queue_mgr.send_queue[channel], sb, buf); 
}

int pruge_recv(uint32_t channel)
{
    while (!g_shm_queue_mgr.recv_queue[channel].push_list.empty()) {
        struct shm_block_t *tmp_sb = g_shm_queue_mgr.recv_queue[channel].push_list.front();
        if (shmq_push(&g_shm_queue_mgr.recv_queue[channel], tmp_sb, tmp_sb->buf) == 0) {
            write_pipe(g_shm_queue_mgr.recv_queue[channel].pipe[1]);
            g_shm_queue_mgr.recv_queue[channel].push_list.pop_front();
            free(tmp_sb);
        } else {
            return -1;
        }
    }

    return 0;
}

int pruge_send(uint32_t channel)
{
    while (!g_shm_queue_mgr.send_queue[channel].push_list.empty()) {
        struct shm_block_t *tmp_sb = g_shm_queue_mgr.send_queue[channel].push_list.front();
        if (shmq_push(&g_shm_queue_mgr.send_queue[channel], tmp_sb, tmp_sb->buf) == 0) {
            write_pipe(g_shm_queue_mgr.send_queue[channel].pipe[1]);
            g_shm_queue_mgr.send_queue[channel].push_list.pop_front();
            free(tmp_sb);
        } else {
            return -1;
        }
    }

    return 0;
}

void write_pipe(int fd)
{
    uint8_t trash[1];
    write(fd, trash, sizeof(trash));
}

void pruge_pipe(int fd)
{
    uint8_t trash[1024];
    read(fd, trash, sizeof(trash));
}
