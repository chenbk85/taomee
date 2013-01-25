/**
 * =====================================================================================
 *       @file  proto.h
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  09/13/2010 04:30:22 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  mason(张龙龙), mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef PROTO_H
#define PROTO_H

#include <sys/types.h>
#include <stdint.h>
#include <pthread.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include "lib/hash.h"
#include "defines.h"

//配置项的结构
typedef struct {
    //[log]
    unsigned int   log_count;                                            
    char           log_dir[PATH_MAX];  
    unsigned int   log_lvl;  
    char           log_prefix[MAX_STR_LEN]; 
    unsigned int   log_size;

    unsigned int   flush_interval;    //写rrd和清理的间隔
    unsigned int   alarm_interval;    //报警间隔
    unsigned int   collect_interval;  //收集间隔
    unsigned int   queue_len;         //队列长度
    unsigned int   collect_thread_num;//收集线程个数

    char           alarm_server_url[MAX_URL_LEN];         //报警服务器url
    char           rrd_rootdir[PATH_MAX];                 //rrd根路径
} config_var_t;

//数据库信息
typedef struct {
    char           db_host[16];
    unsigned short db_port;
    char           db_name[256];
    char           db_user[256];
    char           db_pass[256];
} db_info_t;

//switch 信息
typedef struct {
    unsigned int id;//数据库内的switch_id
    char ip[16];
    char community[128];
    unsigned int type;//交换机厂家,1:Cisco 2:H3C
    unsigned int collect_count;
    unsigned int down;//1=down 0=up
    bool   unconnect;//连接状态，当collect_count达到一定次数，且获取不到数据的情况下标记为true
} switch_info_t;

//switch group信息
typedef struct {
    unsigned int   switch_num;
    switch_info_t  switch_table[MAX_SWITCH_NUM_PER_GROUP];
} switch_group_info_t;

//switch_interface 信息
typedef struct {
    char               desc[64];
    unsigned int       idx; 
    unsigned int       type;              //type 请参见mib标准
    unsigned int       speed;             //单位 Mbit
    unsigned int       status;            // up=1 or down=2
    uint32_t           last_in_bytes;     //last in bytes
    unsigned long      in_bits_per_sec;   // in bits per second
    uint32_t           last_out_bytes;    //last out bytes
    unsigned long      out_bits_per_sec;  // out bits per second
    unsigned int       last_in_report;    //最近一次拉取in的时间
    unsigned int       last_out_report;   //最近一次拉取out的时间
} switch_interface_info_t;


//alarm 元信息
typedef struct {
    double         warning_val;               //警戒阀值
    double         critical_val;              //严重警戒阀值
    op_t           op;                        //阀值算子
    unsigned int   normal_interval;           //正常探测间隔
    unsigned int   retry_interval;            //非正常探测间隔
    unsigned int   max_atc;                   //最大探测次数
} alarm_info_t;

//metric 信息
typedef struct {
    //属性信息
    char           metric_name[MAX_NAME_SIZE]; //metic名称
    unsigned int   metric_type;               //metric类型 1:写rrd,2:报警,3:即写rrd又报警
    char           arg[MAX_URL_LEN];          //参数
} metric_info_t;

//metric_alarm 信息
typedef struct {
    unsigned int   switch_id;                 //交换机id
    char           metric_name[MAX_NAME_SIZE]; //metic名称
    //报警相关信息
    alarm_info_t   alarm_info;
} metric_alarm_t;

typedef std::vector<switch_group_info_t> switch_group_vec_t;
typedef std::vector<metric_info_t> metric_info_vec_t;
typedef std::vector<metric_alarm_t> metric_alarm_vec_t;
typedef std::map<unsigned int, alarm_info_t> if_alarm_map_t;//if_speed 和alarm_info之间的map


//metric数据
typedef union {
    double d;
    int    str;
} metric_val_t;

//switch结构
typedef struct {
    time_t         recv_time;      //接到这个switch信息的时间
    hash_t        *metrics;        //保存switch下的所有metric的hash表
    hash_t        *metrics_status; //保存switch下的所有metric的报警状态的hash表
    unsigned int   id;             //switch在数据库中的id
    unsigned int   dmax;           //dmax 数据生存期
    unsigned int   if_num;         //交换机接口个数
    switch_interface_info_t    if_table[MAX_IF_NUM]; //interface info table
} switch_t;


//metric警报状态结构
typedef struct {
    //alarm info
    unsigned int               cur_atc;      //current attempt count
    unsigned int               is_normal;    //is use the normal attempt interval(0 = yes,1=no)
    unsigned int               check_count;  //the check count 其实就是拉取数据次数的计数
    status_t                   cur_status;   //current metric status
    unsigned int               last_alarm;   //最后报警时间用于清理
} metric_status_info_t;

//metric_t结构
typedef struct {
    time_t           recv_time;  //收到数据的时间
    metric_val_t     val;        //metric数据
    unsigned int     dmax;       //数据生存期
    char             slope[32];  //值增长类型 defualt:both
} metric_t;

#endif  
