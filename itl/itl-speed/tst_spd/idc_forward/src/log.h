/*
 * log.h
 *
 *  Created on:	2011-7-4
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef LOG_H_
#define LOG_H_

//日志文件的打开与关闭
enum log{
	size_per_log		= 8192,
};

/*
 * @brief 打开日志文件
 */
extern int open_log_file();

/*
 * @brief 关闭日志文件
 */
extern void close_log_file();

/*
 * @brief 写格式化日志
 * @param format, several args
 */

extern void write_log(const char *format, ...) __attribute__((format(printf, 1, 2)));

#endif /* LOG_H_ */
