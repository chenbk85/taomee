/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file utils.h
 * @author tonyliu <tonyliu@taomee.com>
 * @date 2011-01-13
 */

#ifndef H_UTILS_2011_01_13_H
#define H_UTILS_2011_01_13_H

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

/**
 * @brief 运行指定目录下的start脚本
 * @param p_path 路径名
 * @return -1: failed, 0: succ
 */
int run_start_script(const char *p_path);

/**
 * @brief 获取服务运行状态
 * @param p_server_name 服务名
 * @param p_is_running 保存服务运行状态
 * @return -1: failed, 0: succ
 */
int check_server_running(const char *p_server_name, bool *p_is_running);

#endif

