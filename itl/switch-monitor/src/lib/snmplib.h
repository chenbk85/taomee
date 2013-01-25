/**
author : mason@taomee.com
**/

#ifndef  SNMPLIB_H_2011726153039
#define  SNMPLIB_H_2011726153039

#include "./utils.h"
const char *const IFTABLE_OID    = ".1.3.6.1.2.1.2.2.1.1";
const char *const IFDESC_OID     = ".1.3.6.1.2.1.2.2.1.2";
const char *const IFTYPE_OID     = ".1.3.6.1.2.1.2.2.1.3";
const char *const IFSPEED_OID    = ".1.3.6.1.2.1.2.2.1.5";
const char *const IFSTATUS_OID   = ".1.3.6.1.2.1.2.2.1.8";
const char *const IFINOCTER_OID  = ".1.3.6.1.2.1.2.2.1.10";
const char *const IFOUTOCTER_OID = ".1.3.6.1.2.1.2.2.1.16";

typedef struct {
    unsigned int type;       //switch type id 1= cisco 2 =h3c
    const char *metric_name; //交换机的metric的名字
    const char *oid;         //mib里对应的oid
} switch_metric_oid_t;

static switch_metric_oid_t switch_metric_oid_table[]=
{
    {1, "switch_cpu_used", ".1.3.6.1.4.1.9.2.1.57.0"},
    {1, "switch_mem_free", ".1.3.6.1.4.1.9.2.1.8.0"},
    {2, "switch_cpu_used", ".1.3.6.1.4.1.2021.11.11.0"},
    {2, "switch_mem_free", ".1.3.6.1.4.1.2021.4.11.0"}
};

typedef union {
    int int32_val;
    unsigned int uint32_val;
    long long int64_val;
    unsigned long long uint64_val;
    double float_val;
    char *str_val;
} snmp_val;

//值类型:所有原子数值类型都定义了相应的类型,其余的全部以字符串类型处理
enum {
    VAL_TYPE_INT32 = 1, 
    VAL_TYPE_UINT32,
    VAL_TYPE_INT64,
    VAL_TYPE_UINT64, 
    VAL_TYPE_FLOAT,
    VAL_TYPE_STR
};

typedef struct {
    int val_type;
    snmp_val val;
} snmp_val_t;

/**
 * @brief   简单的snmpwalk函数
 * @param   ip            交换机ip
 * @param   community     交换机团体
 * @param   root_oid      遍历的oid根
 * @param   timeout       超时值
 * @param   retry         重试次数
 * @param   val_list      值列表数组
 * @param   val_list_len  值列表数组长度:值结果参数，传入最大个数传出实际个数
 * @param   buf           缓冲区 外部分配的用于保存值列表中的字符串类型的数据部分
 * @param   buf_len       缓冲区长度
 * @return  -1 = failed 0 = success
 */
int simple_snmp_walk(const char *ip, const char *community, const char *root_oid, unsigned int timeout, unsigned retry, snmp_val_t *val_list, unsigned int *val_list_len, char *buf, unsigned int buf_len);

/**
 * @brief   简单的snmpget函数
 * @param   ip            交换机ip
 * @param   community     交换机团体
 * @param   oids          遍历的所有oid
 * @param   oid_num       遍历的所有oid的个数
 * @param   timeout       超时值
 * @param   retry         重试次数
 * @param   val_list      值列表数组
 * @param   val_list_len  值列表数组长度:值结果参数，传入最大个数传出实际个数
 * @param   buf           缓冲区 外部分配的用于保存值列表中的字符串类型的数据部分
 * @param   buf_len       缓冲区长度
 * @return  -1 = failed 0 = success
 */
int simple_snmp_get(const char *ip, const char *community, const char **oids, unsigned int oid_num, unsigned int timeout, unsigned int retry, snmp_val_t *val_list, unsigned int *val_list_len, char *buf, unsigned int buf_len);

#endif
