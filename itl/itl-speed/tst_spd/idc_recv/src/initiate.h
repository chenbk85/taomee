/*
 * initiate.h
 *
 *  Created on:	2011-7-7
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef INITIATE_H_
#define INITIATE_H_

/*
 * @typedef time_string_t
 * @brief 时间字符串结构,用于日志记录的时间，
 */
typedef struct time_string{
	char	year[5];
	char	month[3];
	char	day[3];
	char	hour[3];
	char	min[3];
	char	secs[3];
	char	result_string[24];	//存储分钟文件名
	char	fullstring[24];		//错误日志中的详细时间
}__attribute__((packed)) time_string_t;

//全局的timestring
extern  time_string_t timestring;

/*
 * @brief 得到当前运行时时间，以便打印错误日志时记录时间
 */
extern void get_time_string();

/*
 * @brief 初始化，包括设置本次循环时间，改变工作目录，加载日志文件(每次循环都加载,以便修改配置文件后不需要重启),打开日志
 * @prame 执行路径  char *path
 */
extern int initiate(char *path);

/*
*  @brief 销毁内存函数,关闭日志
*/
extern void destroy();

#endif /* INITIATE_H_ */
