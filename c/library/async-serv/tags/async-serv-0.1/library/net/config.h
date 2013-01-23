/**
 * @file config.h
 * @brief
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-10
 */
#ifndef _H_CONFIG_H_
#define _H_CONFIG_H_

#include <stdint.h>
#include <netinet/in.h>

#define TYPE_STREAM         2
#define TYPE_DGRAM          4
#define TYPE_SOCKPAIR       6

#define NULL_MASK       0
#define READ_MASK       (1 << 0)
#define WRITE_MASK      (1 << 1)
#define ERROR_MASK      (1 << 2)
#define RWE_MASK        (READ_MASK | WRITE_MASK | ERROR_MASK)
#define ONESHOT_MASK    (1 << 3)

#define TIMER_ONCE          (1 << 0)
#define TIMER_REPEAT        (1 << 1)
#define TIMER_REPEAT_EXACT  (1 << 2)

#define CONN_COMPL           0
#define IS_CONNECTING        1
#define CONN_TEMP_UNAVAIL    2
#define CONN_ERR             -1

#define ACCEPT_ERR      -1
#define ACCEPT_COMPL     0
#define ACCEPT_CONTINUE  1
#define ACCEPT_EMFILE    2
#define ACCEPT_EINTR     3

#define IS_LISTENING     1

typedef void* timer_id_t;

#endif
