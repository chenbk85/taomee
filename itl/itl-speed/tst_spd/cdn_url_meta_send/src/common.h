/*
 * common.h 全局结构体定义
 *
 *  Created on:	2011-7-4
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef COMMON_H_
#define COMMON_H_

#define	MAX_POINT		20

// needed for uintxx_t
#include <stdint.h>

/*
 * @enum number_related
 * @brief 将需要用的宏定义的数值用枚举元素来代替
 */
enum number_related{
	REALLY_UPDATE_FOLDER	= 0,
	TEST_FOLDER_EXISTENCE	= 1,
	ALIGN_LEN				= 8,
	PASSWD_LEN				= 16,
	FOLDER_NAME_LEN			= 256,
	FILE_NAME_LEN			= 256,
	FULL_PATH_LEN			= 1024,
	TEXT_LINE_LEN			= 4096,
	NET_BUFFER_LEN			= 1024000,
};

extern char 	data_store_path[FULL_PATH_LEN];	//数据存储路径
extern char start_folder[FOLDER_NAME_LEN];
extern char start_filename[FILE_NAME_LEN];
extern char stop_folder[FOLDER_NAME_LEN];
extern char stop_filename[FILE_NAME_LEN];
extern char current_folder[FOLDER_NAME_LEN];
extern char current_filename[FILE_NAME_LEN];

extern char *net_buffer;
extern char *encrypt_net_buffer;

#endif /* COMMON_H_ */
