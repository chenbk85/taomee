/**
 * @file proto.h
 * @brief demo
 * @author baron baron@taomee.com
 * @version 1.0
 * @date 2011-04-25
 */
#ifndef __DEMO_PROTO_H__
#define __DEMO_PROTO_H__

#include <sys/types.h>

#define PROTO_MAX_LENGTH 8 * 1024 * 1024

#define PROTO_ERROR_COMMON 1

#define PROTO_CMD_GETFILE 0x1001

typedef struct {
    int16_t cmd;
    int16_t status;
    int32_t length;
    int32_t user_id;
}__attribute__((packed)) proto_header_t;

typedef struct {
    char name[0];
}__attribute__((packed)) proto_request_body_t;

typedef struct {
    char content[0];
}__attribute__((packed)) proto_ack_body_t;

#endif //!__DEMO_PROTO_H__
