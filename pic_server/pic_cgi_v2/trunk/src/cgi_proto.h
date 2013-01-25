#ifndef  CGI_PROTO_H
#define  CGI_PROTO_H
#include <arpa/inet.h>
#include <stdint.h>
#include "common.h"

#define	pic_max_len	(5*1024*1024)
#define read_pic_len	8
#define	thumb_max_len	(128*1024)
#define	TIME_LEN	16	//上传时间长度
#define	CNT_MAX 4       //最多只能生成4个缩略图
#define	SUCC    0
#define	FAIL	-1
#define	KEY "!tA:mEv,"
#define	log_size    104857600
#define	FILE_LEN	256
#define	FILE_ROOT_PATH "/opt/taomee/file_root/"



enum _error_cgi{
	unknown_err = 4000,
	session_key_err = 4001,
	thumb_cnt_err = 4002,
	retrieve_filename_err = 4003,
	image_size_err = 4004,
	open_file_err = 4005,
	image_type_err = 4006,
	net_err	= 4007,
	read_file_err = 4008,
	hostid_err = 4009,
	timeout_err = 4010,
	invalid_file = 4011,
};



enum ImageType{
	jpg = 1,
	png = 2,
	gif = 3,
};

typedef uint32_t userid_t;

#define PROTO_H_SIZE    (sizeof(protocol_t))

//缩略图的参数
typedef struct _thumb_arg{
	uint32_t    W;
	uint32_t    H;
	uint32_t    start_x;
	uint32_t    start_y;
	uint32_t    thumb_x;
	uint32_t    thumb_y;
}__attribute__((packed))thumb_arg_t;


//--------------------------------------------upload image------------------------------------------

//上传图片时fileserver返回值
typedef struct _recv_fileserver{
	uint32_t	hostid;		
	uint32_t 	albumid;
	uint32_t	photoid;
	char    lloccode[LLOCCODE_LEN +1];	//原图标识码
	uint32_t	len;					//文件长度
}__attribute__((packed))recv_fileserver_t;

//上传图片时cgi发送给fileserver数据
typedef struct _snd_fileserer{
	uint32_t	channel;
	uint32_t	albumid;
	uint32_t    type;
	uint32_t    branchtype;
	uint32_t    width_limit;
	uint32_t    height_limit;
	char	filename[FILE_LEN+1];
	char	filepath[MAX_PATH_LEN+1];
}__attribute__((packed))snd_fileserer_t;

//上传图片时，cgi从webclient收到的数据
typedef struct _recv_webclient{
	userid_t    userid;
	uint32_t	channel;
	uint32_t	albumid;
	uint32_t    ip;
	uint32_t    time;
	uint32_t    width_limit;
	uint32_t    height_limit;
}__attribute__((packed))recv_webclient_t;



//--------------------------------------------upload logo------------------------------------------


//上传头像图片时cgi请求fileserver数据
typedef struct _logo_snd_fileserver{
	uint32_t	channel;
	uint32_t    type;
	uint32_t    branchtype;
	uint32_t    width_limit;
	uint32_t    height_limit;
	uint32_t    cnt;
	thumb_arg_t thumb_arg[CNT_MAX];
	char    filepath[MAX_PATH_LEN+1];
}__attribute__((packed))logo_snd_fileserver_t;

//上传头像时，cgi从webclient收到数据
typedef struct _logo_recv_webclient{
	userid_t    userid;
	uint32_t	channel;
	uint32_t    ip;
	uint32_t    time;
	uint32_t    width_limit;
	uint32_t    height_limit;
	uint32_t	cnt;
	thumb_arg_t	thumb_arg[CNT_MAX];
	char    filepath[MAX_PATH_LEN+1];
}__attribute__((packed))logo_recv_webclient_t;

//上传头像时，fileserver返回包
typedef struct _logo_recv_fileServ{
	uint32_t	hostid;
	char	old_lloccode[LLOCCODE_LEN]; 
	uint32_t	Thumbcnt;
	char	new_lloccodes[CNT_MAX*LLOCCODE_LEN+1];
}__attribute__((packed))logo_recv_fileServ_t;
#endif

