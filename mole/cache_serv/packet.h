/**
 * ====================================================================
 *  @file       packet.h
 *
 *  @brief      定义包头结构
 *
 *  platform   Debian 4.1.1-12 
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * ====================================================================
 */



#ifndef  PACKET_H_
#define  PACKET_H_

extern "C" {
#include <stdint.h>
}



/**
 * @brief 包头结构体
 */
typedef struct proto_head {
	/*the packet length*/
    uint32_t len; 
    uint8_t version;
    uint32_t cmd_id;
    uint32_t user_id;
    int32_t result;
} __attribute__((packed)) proto_head_t, *proto_head_ptr_t;

/*包头的长度*/
#define PROTO_HEAD_SIZE (sizeof(proto_head_t))

#endif   /* PACKET_H_ */

