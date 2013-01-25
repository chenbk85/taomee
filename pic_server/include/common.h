#ifndef _COMMON_H
#define _COMMON_H

#define LLOCCODE_LEN			64
#define MAX_PATH_LEN			128
#define MAX_PIC_NUM_PER_ABLUM	1000

//#define MAX_FIEL_PATH 		256
#define MAX_THUMB_SIZE	2 * 1024 * 1024

#define MAX_THUMB_CNT_PER_PIC		4

enum _proto_ts_cmd {
	proto_ts_upload_thumb 				= 1001,
	proto_ts_cgi_get_thumbs				= 1002,
	proto_ts_cgi_get_thumb 				= 1003,
	proto_ts_change_thumb				= 1004,
	
	proto_ts_delete_thumbs	 			= 1012,
	proto_ts_delete_album			= 1013,
	
	proto_ts_trans_multi_get_thumbs		= 1015,
	proto_ts_trans_multi_add_thumbs		= 1016,
	proto_ts_trans_multi_del_thumbs		= 1017,
};

enum _proto_as_cmd {
	proto_as_delete_file 			= 3001,
	proto_as_delete_multi 			= 3002,
//	proto_as_transfer_file 			= 3003,
	proto_as_change_file_attr		= 3004,		
	proto_as_trans_multi_file 		= 3003,
};

enum _proto_fs_cmd {
	proto_fs_cgi_upload_file  	 	= 1,
	proto_fs_cgi_chg_thumb  	 	= 2,

	proto_fs_delete_file			= 1011,
	proto_fs_change_file_attr		=1014,
	imp_proto_upload_file 			= 2000,
	imp_proto_upload_logo 			= 2001,
};

#endif
