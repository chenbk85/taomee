#ifndef _EOORO_NBR_H_
#define _EOORO_NBR_H_


enum _error_no {
	err_thumbserv_net_err		= 10001,		
	err_adminserv_busy		= 10002,
	err_fileserv_net_err		= 10003,
	err_system_fatal_err		= 10010,		

	err_src_file_not_exist		= 10101,		
	err_thumb_file_not_exist	= 10102,		
	err_file_not_pic		= 10103,		
	err_trans_too_much_file		= 10104,		
	err_album_too_much_thumbs	= 10105,		
	err_file_too_large		= 10106,
	err_cgi_para_err  		= 10110,
	err_invalid_file  		= 10111,
	err_upload_busy_now  		= 10112,
};

#endif
