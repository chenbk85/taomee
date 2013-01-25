#ifndef _EOORO_NBR_H_
#define _EOORO_NBR_H_


enum _error_no {
	err_thumbserv_net_err		= 10001,		
	err_adminserv_busy		= 10002,

	err_system_fatal_err		= 10010,		

	err_src_file_not_exist		= 10101,		
	err_thumb_file_not_exist	= 10102,		
	err_file_not_pic		= 10103,		
	err_file_too_large		= 10106,
	err_cgi_para_err  		= 10110,
	err_invalid_file  		= 10111,
	err_upload_busy_now  		= 10112,
	err_invalid_para 		= 10113,
	err_del_too_many_once 		= 10114,
	err_chg_logo_to_quickly		= 10115,
	err_llocc_fault			= 10116,
	err_cannot_create_image		= 10117,
	err_cannot_save_file		= 10118,
	err_write_llocc_db_err		= 10119,
};

enum del_file_ret {
	del_file_succ			= 11000,
	del_llocc_err			= 11001,
	del_fs_timeout			= 11002,
	del_file_not_exist		= 11003,
};


#endif
