/** 
 * ========================================================================
 * @file node.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-03
 * Modify $Date: 2012-10-19 14:27:52 +0800 (五, 19 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_NODE_H_2012_07_03
#define H_NODE_H_2012_07_03



#include <stdint.h>
#include "itl_common.h"
#include "file.h"

// 跟head交互前需要验证该版本号
#define HEAD_VERSION    2

// 跟control交互前需要验证该版本号
#define CONTROL_VERSION    1

extern uint32_t g_node_id;
extern char g_server_tag[SERVER_TAG_LEN];
extern char g_node_ip_str[IP_STRING_LEN];
extern uint32_t g_node_ip;
extern uint32_t g_start_timestamp;

extern const char * g_os;
extern const char * g_auto_update_url;
extern const char * g_metric_so_dir;

extern c_file * g_server_bin;
extern c_file * g_node_bin;
extern c_file * g_bench_conf;
extern c_file * g_work_conf;
extern c_file * g_restart_script;
extern c_file * g_control_script;

extern bool g_auto_update_flag;
extern bool g_collect_flag;

typedef int (*check_file_func_t)(const char * file);


enum
{
    PROC_PROXY = 0,
    PROC_COLLECT,
    PROC_COMMAND,
    PROC_MYSQL,
};

// 获取node的ip
int get_node_ip();

// 定时检查node的ip
int monitor_node_ip(void * owner, void * data);


// 重启itl node
int restart_node();



/** 
 * @brief 从远程web服务器上下载文件
 * 
 * @param dst 下载文件的本地名称
 * @param src url
 * @param md5 NULL表示不检查md5
 * 
 * @return NODE_SUCC成功，其他失败
 */
int download_file(const char * dst, const char * src, const char * md5 = NULL);


/** 
 * @brief 上传文件
 * 
 * @param src 本地文件
 * @param dst 远程路径，包括协议和文件名
 * 
 * @return 0成功，其他失败
 */
int upload_file(const char * src, const char * dst);

/** 
 * @brief 下载metric so到配置预设的文件夹
 * 
 * @param filename metric_so的文件名，不带路径
 * 
 * @return 0成功，-1失败
 */
int download_metric_so(const char * filename);

// 从src处下载文件，校验md5，用cb检查文件，并链接为dst
int download_check_link(const char * dst, const char * src, const char * md5, check_file_func_t cb = NULL);

/** 
 * @brief 将src链接为dst
 * 如果dst存在，并且是symbolic link，则会：
 *  替换dst的symbolic link为src
 *  如果替换失败，保持原来dst的symbolic link
 * 
 * @param src
 * @param dst
 * 
 * @return 0成功，-1失败
 */
int link_file(const char * src, const char * dst);


bool check_update();



int init_check_update();
int fini_check_update();


int check_so(const char * new_so);

int do_command(const char * cmd);

#endif
