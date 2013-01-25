/** 
 * ========================================================================
 * @file define.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-05
 * Modify $Date: 2012-10-31 11:28:59 +0800 (三, 31 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_DEFINE_H_2012_07_05
#define H_DEFINE_H_2012_07_05


#include <arpa/inet.h>
#include <limits.h>


#define FD_CLOSE(a) if ((a) >= 0) {close(a);a = -1;}


// 尝试连接服务器的间隔
// 即如果没有连上，在5秒后尝试再次连接
#define CONNECT_TO_SERVER_INTERVAL (5)

// itl_node没有获取到相关配置时，定时找db拉取
#define GET_NODE_CONFIG_INTERVAL (5 * 60)

// itl_node监视内网ip变更的间隔
#define MONITOR_NODE_IP_INTERVAL (30)

// 自动检查itl node文件更新的间隔
#define ITL_NODE_CHECK_UPDATE_INTERVAL (60)

// sql语句长度
#define SQL_BUFF_LEN (1024 * 1024)

// ip字符串的长度
#define IP_STRING_LEN (INET_ADDRSTRLEN)

// md5长度
#define MD5_LEN (32)

// server_tag的最大长度
#define SERVER_TAG_LEN (32)

// 机器所属项目名称
#define PROJECT_NAME_LEN (64)

// 配置文件中key的最大长度
#define MAX_CONFIG_KEY_LEN (64)

// metric名字的最大长度
#define MAX_METRIC_NAME_LEN (64)

// metric单位的最大长度
#define MAX_METRIC_UNIT_LEN (16)

// metric中fmt的最大长度
#define MAX_METRIC_FMT_LEN (16)

// 传给metric采集函数的参数的buf大小
#define MAX_METRIC_ARG_LEN (128)

// 报文的最大长度
#define PKG_BUF_SIZE (8192 * 1024)

// 告警联系人列表长度
#define CONTACT_LIST_LEN (256)

// 默认采集间隔
// 如果配置中的采集间隔为0
// 则改成默认值，防止出现问题
#define DEFAULT_COLLECT_INTERVAL (20)

// 定时获取配置更新命令
#define GET_CONFIG_UPDATE_INTERVAL (30)







///1:服务器硬件 2:交换机 3:mysql
enum {
    ITL_TYPE_ALL = 0,
    ITL_TYPE_SERVER = 1,
    ITL_TYPE_SWITCH,
    ITL_TYPE_MYSQL,
};

// head处理类型
enum
{
    METRIC_TYPE_BEGIN = 0,
    METRIC_TYPE_ALARM = 1,
    METRIC_TYPE_RRD = 1<<1,
    METRIC_TYPE_MYSQL = 1<<2,
    METRIC_TYPE_END
};


//网络类型
enum {
    NET_INSIDE_TYPE = 1,
    NET_OUTSIDE_TYPE = 2
};



///比较操作符枚举
enum 
{
    OP_BEGIN = 0,
    OP_GE,
    OP_LE,
    OP_GT,
    OP_LT,
    OP_EQ, 
    OP_END
};

enum
{
    ALARM_NORMAL = 0,
    ALARM_WARN = 1,
    ALARM_CRITICAL = 1<<1,
    ALARM_HOSTDOWN = 1<<2,
    ALARM_UPFAILED = 1<<3

};

// 告警方式
enum
{
    ALARM_WAY_MOBILE = 1,
    ALARM_WAY_EMAIL = 1<<1,
    ALARM_WAY_RTX = 1<<2,
};

// 告警方式
enum
{
    SWITCH_TYPE_DELL    = 1,
    SWITCH_TYPE_IBM     = 2,
    SWITCH_TYPE_NSFOCUS = 3,
    SWITCH_TYPE_CISCO   = 4,
    SWITCH_TYPE_H3C     = 5
};

// 服务器/交换机状态
enum
{
    STATUS_HOST_UP = 0,
    STATUS_HOST_DOWN = 3,
    STATUS_HOST_UNKOWN = 2,
    STATUS_NODE_DOWN = 1,
};

// 节点类型
enum
{
    NODE_TYPE_HOST = 1,
    NODE_TYPE_SWITCH = 2,
};





#endif /* H_DEFINE_H_2012_07_05 */
