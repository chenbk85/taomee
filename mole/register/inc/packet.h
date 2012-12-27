/*
 * =====================================================================================
 * 
 *       Filename:  packet.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  02/20/2008 05:58:23 PM EST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  spark (spark), zhezhide@163.com
 *        Company:  TAOMEE
 *
 *      ------------------------------------------------------------
 *      view configure:
 *          VIM:  set tabstop=4  
 * 
 * =====================================================================================
 */

#include <stdint.h>
#include <sys/types.h>

#ifndef  PACKET_INC
#define  PACKET_INC

#define VERSION 1


//the size of each field in protocol
#define USER_ID_SIZE    4
#define NICK_SIZE       16
#define PASSWD_SIZE     32
#define EMAIL_SIZE      64
#define SESSION_SIZE    24
#define ACT_CODE_SIZE   24
#define USER_ID_CNT_SIZE 4  //for 
#define SEND_EMAIL_CMD_LEN   512

#define ACTIVE_CODE_SIZE   8 

//extended register information 
#define	TEL_SIZE	16
#define PROV_SIZE	2
#define CITY_SIZE	2
#define ADR_SIZE	64
#define SIG_SIZE	128

typedef struct proto_head {
	uint32_t len;
	uint8_t  version;
	uint32_t cmd_id;
	uint32_t userid;
	int32_t result;
} __attribute__((packed)) PROTO_HEAD, *PROTO_HEAD_PTR;

#define PROTO_HEAD_SIZE     (sizeof(PROTO_HEAD))

#endif   /* ----- #ifndef PACKET_INC  ----- */

