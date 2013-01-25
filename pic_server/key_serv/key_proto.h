/*
 * =====================================================================================
 *
 *       Filename:  proto.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/25/2010 05:31:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  maxwell (zdl), maxwell@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef PROTO_H
#define PROTO_H
typedef uint32_t userid_t;

typedef struct protocol {
	uint32_t    len;
	uint16_t    cmd;
	userid_t    id;
	uint32_t    seq;
	uint32_t    ret;
	uint8_t     body[];
} __attribute__((packed)) PROTO_HEADER;

enum proto_cmd {
	proto_create_album      = 1001,
	proto_upload_pic        = 1002,
	proto_get_thumb         = 1003,
	proto_get_image         = 1004,
	proto_del_image         = 1011,
	proto_change_albumid    = 1012,
};

enum {
	err_get_too_many_keys	= 1,
};

//是否需要保存交易报文
//有涉及到更新数据库数据的协议，都要保存 
#define NEED_UPDATE                         0x0100

#define MAX_COUNT_DAY						100
#define PROTO_MAX_SIZE                              81920
#define PROTO_HEADER_SIZE                           (sizeof (PROTO_HEADER))

#define RECVBUF_HEADER                              ((PROTO_HEADER*)recvbuf)
#define SNDBUF_PRIVATE_POS                          (((char*)*sendbuf)+PROTO_HEADER_SIZE )
#define RCVBUF_PRIVATE_POS                          ((char*)recvbuf +PROTO_HEADER_SIZE )

#define RECVBUF_USERID                              (RECVBUF_HEADER->id)
#define RECVBUF_GROUPID                             (RECVBUF_HEADER->id)
#define RECVBUF_CMDID                               (RECVBUF_HEADER->cmd)
#define USER_ID_FROM_RECVBUF                        RECVBUF_USERID
#define RECVLEN_FROM_RECVBUF                        (RECVBUF_HEADER->len)

#define PRI_IN_LEN                                  (RECVLEN_FROM_RECVBUF-PROTO_HEADER_SIZE)
#define PRI_IN_POS                                  (typeof(p_in))(RCVBUF_PRIVATE_POS) 
#define PRI_SEND_IN_POS                             (typeof(in))(sendbuf+PROTO_HEADER_SIZE ) 
#define SET_STD_ERR_BUF(sendbuf,sndlen,rcvph,ret)  set_std_return(sendbuf,sndlen,rcvph,ret,0)

#define SEND_PRI_IN_POS     (typeof(p_in))(this->sendbuf+PROTO_HEADER_SIZE) 
#define PH_RESULT(buf )     (((PROTO_HEADER* )buf)->ret )

#define PH_LEN(buf )    (((PROTO_HEADER* )buf)->len)
#define PH_ID(buf)          (((PROTO_HEADER* )buf)->id)
#define PH_CMD(buf)         (((PROTO_HEADER* )buf)->cmd)

struct stru_back_index{
	uint32_t index;
}__attribute__((packed));

#endif
