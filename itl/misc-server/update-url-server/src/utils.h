/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file utils.h
 * @author richard <richard@taomee.com>
 * @date 2010-03-09
 */

#ifndef UTILS_H_2010_03_09
#define UTILS_H_2010_03_09

#include <stdint.h>

/**
 * @brief 为指定的信号安装信号处理函数
 * @param sig 要处理的信号
 * @param signal_handler 信号处理函数
 * @return 成功返回0，失败返回-1
 */
int mysignal(int sig, void(*signal_handler)(int));

/**
 * @brief 判断程序是否已经有实例正在运行
 * @return 如果已经有实例正在运行返回1，否则返回0，出错时返回-1
 */
int already_running(); 

/**
 * @brief 初始化设置proc标题
 * @param argc main函数的第一个参数
 * @param argv main函数的第二个参数
 * @return 无
 */
void init_proc_title(int argc, char *argv[]);

/**
 * @brief 设置proc标题
 * @param fmt 标题格式
 * @param ... 可变参数
 * @return 无
 */
void set_proc_title(const char *fmt, ...);

/**
 * @brief 反初始化设置proc标题
 * @return 无
 */
void uninit_proc_title();

#endif //UTILS_H_2010_03_09

