/*
 * =====================================================================================
 * 
 *	   Filename:  proto.h
 * 
 *	Description:  
 * 
 *		Version:  1.0
 *		Created:  2007年11月01日 14时10分21秒 CST
 *	   Revision:  none
 *	   Compiler:  gcc
 * 
 *		 Author:  xcwen (xcwen), xcwenn@gmail.com
 *		Company:  NULL
 * 
 * =====================================================================================
 */

#ifndef  PROTO_HEADER_INC
#define  PROTO_HEADER_INC
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>


#include <string.h>
#include "common.h"
#include "benchapi.h"
#include "proto.h"
#include "logproto.h"



//define userid type
typedef uint32_t userid_t;


typedef struct proto_header{
	uint32_t proto_length; 
	uint32_t proto_id; 
	uint16_t cmd_id; 
	int32_t result; 
	userid_t id;  /* userId */
	uint32_t role_tm;
} __attribute__((packed)) PROTO_HEADER;

#define PROTO_MAX_SIZE  						 	8192	
#define PROTO_HEADER_SIZE  							(sizeof (PROTO_HEADER))

#define RECVBUF_HEADER 	  							((PROTO_HEADER*)recvbuf)
#define SNDBUF_PRIVATE_POS  	  					(((char*)*sendbuf)+PROTO_HEADER_SIZE )
#define RCVBUF_PRIVATE_POS  	  					((char*)recvbuf +PROTO_HEADER_SIZE )

#define RECVBUF_USERID 	  							(RECVBUF_HEADER->id)
#define RECVBUF_ROLETM 	  							(RECVBUF_HEADER->role_tm)
#define RECVBUF_GROUPID 	  						(RECVBUF_HEADER->id)
#define RECVBUF_CMDID								(RECVBUF_HEADER->cmd_id)
#define USER_ID_FROM_RECVBUF  						RECVBUF_USERID
#define RECVLEN_FROM_RECVBUF 	  					(RECVBUF_HEADER->proto_length)
#define PROTOID_FROM_RECVBUF 	  					(RECVBUF_HEADER->proto_id)
#define PRI_IN_LEN									(RECVLEN_FROM_RECVBUF-PROTO_HEADER_SIZE)
#define PRI_IN_POS  								(typeof(p_in))(RCVBUF_PRIVATE_POS) 
#define PRI_SEND_IN_POS  							(typeof(in))(sendbuf+PROTO_HEADER_SIZE ) 
#define SET_STD_ERR_BUF(sendbuf,sndlen,rcvph,ret)  set_std_return(sendbuf,sndlen,rcvph,ret,0)

inline bool set_std_return(char **sendbuf, int * sndlen,
		PROTO_HEADER * rcvph,int ret, uint32_t private_size )
{
	PROTO_HEADER *ph;
	DEBUG_LOG("1.2 %ul",sndlen);
	*sndlen=PROTO_HEADER_SIZE+private_size;
	
	if (*sndlen>PROTO_MAX_SIZE){
		return false; 
	}
	 if (!(*sendbuf =  (char*)malloc(*sndlen))) {
		return false;
     }

	ph=(PROTO_HEADER*)*sendbuf;
	memcpy (ph,rcvph,PROTO_HEADER_SIZE);
	ph->proto_length=*sndlen;
	ph->result=ret;
	return true;
}
inline int set_proto_buf(char *sendbuf, short cmd_id, userid_t userid, uint32_t role_time,
	   char * pri_part, uint32_t pri_size )
{	
	PROTO_HEADER *ph=(PROTO_HEADER *)sendbuf ;
	ph->proto_length=PROTO_HEADER_SIZE+pri_size;
	ph->cmd_id= cmd_id;
	ph->proto_id=0x12345678;
	ph->id=userid;
	ph->result=0;
	ph->role_tm=role_time;
	memcpy((sendbuf+PROTO_HEADER_SIZE), pri_part,pri_size);
	return ph->proto_length;    
}


#endif   /* ----- #ifndef PROTO_HEADER_INC  ----- */

