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
//define userid type
typedef uint32_t userid_t;

struct stru_db_cache_state {
	bool transaction_open_flag;//事务是否开启
}; 
extern stru_db_cache_state g_db_cache_state;//全局的状态

typedef struct proto_header{
	uint32_t proto_length; //报文总长度
	uint32_t proto_id;//序列号，需要原样返回
	uint16_t cmd_id; //命令号
	int32_t result; //返回值
	userid_t id;  /*一般是米米号*/
#ifdef  GF_PROTO_HEADER 
	uint32_t role_tm;
#endif

} __attribute__((packed)) PROTO_HEADER;
typedef struct proto_header proto_header_t;  

//在cmdid 中数据库识别标志
#define RROTO_ROUTE_FIELD               0xFE00 //使用前7个bit

//是否需要保存交易报文
//有涉及到更新数据库数据的协议，都要保存 
#define NEED_UPDATE                         0x0100

#define PROTO_MAX_SIZE  						 	81920
#define PROTO_HEADER_SIZE  							(sizeof (PROTO_HEADER))

#define RECVBUF_HEADER 	  							((PROTO_HEADER*)recvbuf)
#define SNDBUF_PRIVATE_POS  	  					(((char*)*sendbuf)+PROTO_HEADER_SIZE )
#define RCVBUF_PRIVATE_POS  	  					((char*)recvbuf +PROTO_HEADER_SIZE )

#define RECVBUF_USERID 	  							(RECVBUF_HEADER->id)
#define RECVBUF_GROUPID 	  						(RECVBUF_HEADER->id)
#define RECVBUF_CMDID								(RECVBUF_HEADER->cmd_id)
#define USER_ID_FROM_RECVBUF  						RECVBUF_USERID
#define RECVLEN_FROM_RECVBUF 	  					(RECVBUF_HEADER->proto_length)
#define PROTOID_FROM_RECVBUF 	  					(RECVBUF_HEADER->proto_id)
#define PRI_IN_LEN									(RECVLEN_FROM_RECVBUF-PROTO_HEADER_SIZE)
#define PRI_IN_POS  								(typeof(p_in))(RCVBUF_PRIVATE_POS) 
#define PRI_SEND_IN_POS  							(typeof(in))(sendbuf+PROTO_HEADER_SIZE ) 
#define SET_STD_ERR_BUF(sendbuf,sndlen,rcvph,ret)  set_std_return(sendbuf,sndlen,rcvph,ret,0)
inline bool set_std_return_p(char *sendbuf, int * sndlen,
        PROTO_HEADER * rcvph,uint32_t protoid ,int ret, uint32_t private_size )
{
    PROTO_HEADER *ph;
    *sndlen=PROTO_HEADER_SIZE+private_size;
    if (*sndlen>81920000){
        return false; 
    }
    ph=(PROTO_HEADER*)sendbuf;
    memcpy (ph,rcvph,PROTO_HEADER_SIZE);
    ph->proto_length=*sndlen;
    ph->proto_id=protoid;
    ph->result=ret;
    return true;
}

inline void set_db_proto_buf( char* send_buf,uint16_t cmdid ,uint32_t userid, uint32_t private_size )
{
	PROTO_HEADER *ph=(PROTO_HEADER*)(send_buf);
	memset(ph,0,PROTO_HEADER_SIZE);
	ph->proto_length=PROTO_HEADER_SIZE+private_size ;
	ph->cmd_id=cmdid ;
	ph->id=userid;
}

inline bool set_std_return(char **sendbuf, int * sndlen,
		PROTO_HEADER * rcvph,int ret, uint32_t private_size ,
		uint32_t proto_header_len =PROTO_HEADER_SIZE  )
{
	PROTO_HEADER *ph;
	*sndlen=proto_header_len +private_size;
	if (*sndlen>81920000){
		return false; 
	}
	 if (!(*sendbuf =  (char*)malloc(*sndlen))) {
		return false;
     }

	ph=(PROTO_HEADER*)*sendbuf;
	memcpy (ph,rcvph,proto_header_len );
	ph->proto_length=*sndlen;
	ph->result=ret;
	return true;
}
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
inline int set_proto_header(char *sendbuf,  uint16_t cmd_id,uint32_t proto_id,  
		userid_t userid, uint32_t pri_size=0 ,int result=0 )
{	
	PROTO_HEADER *ph=(PROTO_HEADER *)sendbuf ;
	ph->proto_length=PROTO_HEADER_SIZE+pri_size;
	ph->cmd_id= cmd_id;
	ph->proto_id=proto_id;
	ph->id=userid;
	ph->result=0;
	return ph->proto_length;    
}




#endif   /* ----- #ifndef PROTO_HEADER_INC  ----- */

