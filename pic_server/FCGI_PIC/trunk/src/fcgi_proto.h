#ifndef  FCGI_PROTO_H
#define  FCGI_PROTO_H
#include <arpa/inet.h>
#define pic_max_len	(2*1024*1024)
#define Lloccode_Len    64 //原图标识码长度
#define PATH_MAX_LEN	128 //原图路径
#define CNT_MAX 4       //最多只能生成4个缩略图
#define SUCC    0
#define FAIL	-1
#define KEY "!tA:mEv,"
//#define FCGI_MAX_COUNT	100
#define log_size	104857600
#define JPG	1
#define PNG	2
#define GIF	3

#define FILE_ROOT_PATH "/opt/taomee/file_root/"

enum _error_nbr{
	del_file_para_err = 1,
	host_id_err = 2,
	file_not_exist_err = 3,
};

enum proto_cmd {
	proto_change_thumb		= 1005,
	proto_create_album      = 1001,
	proto_upload_pic        = 1002,
	proto_get_thumb         = 1003,
	proto_get_image         = 1004,
	proto_del_image         = 1011,
	proto_change_albumid    = 1012,
};

typedef uint32_t userid_t;


#define PROTO_H_SIZE    (sizeof(protocol_t))
//缩略图的参数
struct thumb_arg{
	uint32_t    W;
	uint32_t    H;
	uint32_t    start_x;
	uint32_t    start_y;
	uint32_t    thumb_x;
	uint32_t    thumb_y;
}__attribute__((packed));
//修改缩略图时FCGI从webclient收到的数据
struct fcgi_recv_webclient{
	userid_t    userid;
	uint32_t    key;
	uint32_t	ip;
	uint32_t	time;
	char	lloccode[Lloccode_Len+1];
	uint32_t	cnt;
	struct thumb_arg thumb_arg[CNT_MAX];
}__attribute__((packed));
//修改缩略图时FCGI从fileserver收到的数据
struct fcgi_recv_fileser{
	uint32_t	key;
	char	lloccode[Lloccode_Len+1];
	uint32_t	cnt;
	uint32_t    Thumbid[CNT_MAX];
}__attribute__((packed));	


//浏览缩略图时，从缩略图服务器收到的应答包
struct recv_thumb_pkg{
	int thumb_len;
	char thumb_data[];
}__attribute__((packed));
//浏览缩略图时，发送给缩略图服务器的请求包
typedef struct snd_thumb_pkg{
	uint32_t	key;
	uint32_t	thumb_id;
}__attribute__((packed)) stru_snd_thumb;

#define HeaderContentType(type) \
		printf("Content-type: %s\r\n", type);
#define HeaderLastModified(time) \
		printf("Last-Modified: %s\r\nCache-Control: max-age=94608000\r\n", time);
#define HeaderStatus(status,statusMessage)	\
		printf("Status: %d %s\r\n", status, statusMessage);
#define HeaderContentDisposition(filename)	\
		printf("Content-Disposition: inline; filename=%s\r\n",filename);
#define HeaderEnd \
		printf("\r\n");

#endif
