/*
 * =====================================================================================
 * 
 *	   Filename:  proto.h
 * 
 *	Description:  作为网络协议的头，此处定义协议头相关的宏与处理函数
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
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
//define userid type
typedef uint32_t userid_t;


typedef struct proto_header{
	uint32_t proto_length;  ///< 包的全部长度
	uint32_t proto_id; ///< 协议版本号
	uint16_t cmd_id; ///< 命令编号
	int32_t result; 
	userid_t id;  /**< 用户编号 */
} __attribute__((packed)) PROTO_HEADER;

#define PROTO_MAX_LEN 	8192*4 
#define PROTO_HEADER_SIZE  							(sizeof (PROTO_HEADER))

#define RECVBUF_HEADER 	  							((PROTO_HEADER*)recvbuf)
#define SNDBUF_PRIVATE_POS  	  					(((char*)*sendbuf)+PROTO_HEADER_SIZE )
#define RCVBUF_PRIVATE_POS  	  					(recvbuf +PROTO_HEADER_SIZE )

#define RECVBUF_USERID 	  							(RECVBUF_HEADER->id)
#define RECVBUF_GROUPID 	  						(RECVBUF_HEADER->id)
#define USER_ID_FROM_RECVBUF  						RECVBUF_USERID
#define RECVLEN_FROM_RECVBUF 	  					(RECVBUF_HEADER->proto_length)
#define PRI_IN_LEN									(RECVLEN_FROM_RECVBUF-PROTO_HEADER_SIZE)
#define PRI_IN_POS  								(typeof(p_in))(RCVBUF_PRIVATE_POS) 
#define PRI_SEND_IN_POS  							(typeof(in))(sendbuf+PROTO_HEADER_SIZE ) 
#define SET_STD_ERR_BUF(sendbuf,sndlen,rcvph,ret)  set_std_return(sendbuf,sndlen,rcvph,ret,0)

/**
 * @brief 准备需要返回的头
 * @details 它只是把收到的头再发回去
 * @param PROTO_HEADER* rcvph 收到的头
 */
inline bool set_std_return(char **sendbuf, int * sndlen,
		PROTO_HEADER * rcvph,int ret, uint32_t private_size )
{
	PROTO_HEADER *ph;
	*sndlen=PROTO_HEADER_SIZE+private_size;
	if (*sndlen>PROTO_MAX_LEN ){
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

//由上面函数准备好的头，把返回的数据附加到头之后
inline int set_proto_buf(char *sendbuf, short cmd_id, userid_t userid,
	   char * pri_part, uint32_t pri_size )
{	
	PROTO_HEADER *ph=(PROTO_HEADER *)sendbuf ;
	ph->proto_length=PROTO_HEADER_SIZE+pri_size;
	ph->cmd_id= cmd_id;
	ph->proto_id=0x12345678;
	ph->id=userid;
	ph->result=0;
	memcpy((sendbuf+PROTO_HEADER_SIZE), pri_part,pri_size);
	return ph->proto_length;    
}


#endif   /* ----- #ifndef PROTO_HEADER_INC  ----- */

