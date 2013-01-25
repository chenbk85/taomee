/**
 * =====================================================================================
 *       @file  define.h
 *      @brief  
 *    some macro defines
 *
 *   @internal
 *     Created  09/02/2010 03:08:06 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  mason, mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef DEFINES_H
#define DEFINES_H

#include <limits.h>
#include <stdint.h>
#include <vector>
#include <map>

const char  *const VERSION               = "1.0.5";             /**<@VERSION */

const unsigned int REINIT_INTERVAL       = 300;                 /**<@从数据库更新配置的间隔*/
const unsigned int DEFAULT_COLLECT_IF_INFO_COUNT = 480;         /**<隔多少次重新去拉取switch的interface信息*/
const unsigned int MAX_UNCONNECT_TIMES   = 10;                  /**<最大可允许连接不上的次数*/
const unsigned int MAX_STR_LEN           = 1024;                 
const unsigned int MAX_URL_LEN           = 256;                 /**<@最大的URL长度*/
const unsigned int DEFAULT_DMAX          = 3600;                //数据生存期默认一天
const unsigned int MAX_IF_NUM            = 256;                 //一个交换机的最大接口个数
const unsigned int DEFAULT_TIME_OUT      = 2;                  //默认超时值
const unsigned int BITS_PER_MBIT         = 1000000;             //兆比特
const unsigned int MAX_UINT32            = 0xffffffffU;         //32位无符号整数的最大值
const unsigned int MAX_NAME_SIZE         = 128;                 /**<@最大的名字长度*/
const unsigned int MAX_IF_NUM_PER_GROUP  = 50;                 

const unsigned char SWITCH_CONFIG_UPDATE = 0x01;
const unsigned char METRIC_CONFIG_UPDATE = 0x02;
const unsigned char METRIC_ALARM_STRATEGY_CONFIG_UPDATE = 0x04;
const unsigned char INTERFACE_ALARM_STRATEGY_CONFIG_UPDATE = 0x08;

const unsigned int MAX_SWITCH_NUM_PER_GROUP = 20;

const unsigned int SWITCH_MAX_IO_STREAM = 1000000000;//1G

//根switch hash表的缺省大小
#define DEFAULT_SWITCHSIZE 128

//metric 的hash表的缺省大小
#define DEFAULT_METRICSIZE 10

//比较操作符枚举
typedef enum {
    OP_UNKNOW = 0,
    OP_EQ, 
    OP_GT,
    OP_LT,
    OP_GE,
    OP_LE 
} op_t;

//metric增长率(斜率)
typedef enum {
    SLOPE_ZERO = 0,
    SLOPE_POSITIVE,
    SLOPE_NEGATIVE,
    SLOPE_BOTH,
    SLOPE_UNSPECIFIED
} slope_t;

//警报等级
typedef enum {
    STATUS_OK = 0,
    STATUS_SW,
    STATUS_HW,
    STATUS_SC,
    STATUS_HC
} status_t;

//metric警报级别,四种基本状态unknown, ok，warning，critical
typedef enum {
    STATE_U = -1,
    STATE_O = 0,
    STATE_W,
    STATE_C
} state_t;

#endif /* DEFINES_H*/
