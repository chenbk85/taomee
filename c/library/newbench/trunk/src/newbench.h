/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file newbench.h
 * @author richard <richard@taomee.com>
 * @date 2011-08-01
 */

#ifndef NEWBENCH_H_2011_08_01
#define NEWBENCH_H_2011_08_01

#include "i_net_io_server.h"
#include "i_ini_file.h"
#include "util.h"

extern i_ini_file *g_p_ini;
extern bench_config_t g_bench_conf;
extern i_net_io_notifier *g_p_net_io_notifier;

///控制标志位
enum {
    DAT_BLOCK,
    BROADCAST_BLOCK,
    PAD_BLOCK,
    FIN_BLOCK
};

/**
 * shm_block_t - share memory block infomation
 * @blk_id - block id
 * @length - total block length,including data
 * @type - socket type (%SOCK_STREAM, %SOCK_DGRAM, etc)
 * @skinfo - socket info
 * @accept_fd - accept socket id
 * @data - receive or send data
 */
typedef struct shm_block {
    int32_t length;
    int32_t blk_id;
    int8_t type;
    skinfo_t skinfo;
    char data[0];
}__attribute__((packed)) shm_block_t;

#endif /* NEWBENCH_H_2011_08_01 */
