/** 
 * ========================================================================
 * @file metrics.cpp
 * @brief 
 * @author tonyliu
 * @version 1.0.0
 * @date 2012-08-20
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <map>
#include <string>
#include <assert.h>

extern "C"
{
#include <libtaomee/log.h>
}

#include "itl_util.h"
#include "container.h"
#include "c_itl_snmp.h"
#include "metrics.h"

using namespace std;

//================================常量定义================================
const char *const OID_IFTABLE    = ".1.3.6.1.2.1.2.2.1.1";
const char *const OID_IFDESC     = ".1.3.6.1.2.1.2.2.1.2";
const char *const OID_IFTYPE     = ".1.3.6.1.2.1.2.2.1.3";
const char *const OID_IFSPEED    = ".1.3.6.1.2.1.2.2.1.5";
const char *const OID_IFSTATUS   = ".1.3.6.1.2.1.2.2.1.8";
const char *const OID_IFINOCTER  = ".1.3.6.1.2.1.2.2.1.10";
const char *const OID_IFOUTOCTER = ".1.3.6.1.2.1.2.2.1.16";

#define SWITCH_INFO_OID_NUM 4
enum {
    SWITCH_IF_STATUS = 0,
    SWITCH_IF_TYPE,
    SWITCH_IF_SPEED,
    SWITCH_IF_DESC,
};

typedef enum {
    SWITCH_METRIC_MIN = -1,
    SWITCH_CPU_USED = 0,
    SWITCH_MEM_FREE,
    SWITCH_IF_INFO,
    SWITCH_IF_IN,
    SWITCH_IF_OUT,
    SWITCH_METRIC_MAX,
} switch_metric_t;

typedef struct {
    unsigned int switch_type;       //交换机类型：1-DELL，2-IBM，3-NSFOCUS，4-H3C，5-Cisco
    switch_metric_t metric_type;//交换机的metric的名字
    const char *oid;         //mib里对应的oid
} switch_metric_oid_t;

static switch_metric_oid_t metric_oid_table[] =
{
    {4, SWITCH_CPU_USED, ".1.3.6.1.4.1.2021.11.11.0"},
    {4, SWITCH_MEM_FREE, ".1.3.6.1.4.1.2021.4.11.0"},
    {5, SWITCH_CPU_USED, ".1.3.6.1.4.1.9.2.1.57.0"},
    {5, SWITCH_MEM_FREE, ".1.3.6.1.4.1.9.2.1.8.0"}
};

static const char *g_metric_names[] = 
    {"switch_cpu_used", "switch_mem_free", "switch_if_info", "switch_if_in", "switch_if_out"};

#define CHECK_VALUE_TYPE(p_snmp_val, val_type) \
    do {\
        if (p_snmp_val->val_type != val_type) {\
            ERROR_LOG();\
        }\
    } while (false)

#define GET_METRIC_OID(switch_type, metric_type, if_num, oid_name) \
    do {\
        switch (metric_type) {\
        case SWITCH_IF_IN:\
            snprintf(g_oid_name, sizeof(g_oid_name), "%s.%u", OID_IFINOCTER, if_num);\
            oid_name = g_oid_name;\
            break;\
        case SWITCH_IF_OUT:\
            snprintf(g_oid_name, sizeof(g_oid_name), "%s.%u", OID_IFOUTOCTER, if_num);\
            oid_name = g_oid_name;\
            break;\
        case SWITCH_CPU_USED:\
        case SWITCH_MEM_FREE:\
        {\
            for (uint32_t i = 0; i < sizeof(metric_oid_table) / sizeof(switch_metric_oid_t); i++)\
            {\
                if (metric_oid_table[i].switch_type == switch_type && metric_oid_table[i].metric_type == metric_type)\
                {\
                    oid_name = metric_oid_table[i].oid;\
                    break;\
                }\
            }\
            break;\
        }\
        }\
    } while (false)


#define GET_SNMP_VALUE(p_snmp_value, val, switch_ip, metric_name, if_num) \
    do {\
        switch ((p_snmp_value)->val_type)\
        {\
            case VAL_TYPE_INT32:\
                val.value_int = (p_snmp_value)->val.int32_val;\
                INFO_LOG("switch[%s] metric[%s] arg[%u] value_int: %d",\
                        switch_ip, metric_name, if_num, val.value_int);\
                break;\
            case VAL_TYPE_UINT32:\
                val.value_uint = (p_snmp_value)->val.uint32_val;\
                INFO_LOG("switch[%s] metric[%s] arg[%u] value_uint: %u",\
                        switch_ip, metric_name, if_num, val.value_uint);\
                break;\
            case VAL_TYPE_INT64:\
                val.value_ll = (p_snmp_value)->val.int64_val;\
                INFO_LOG("switch[%s] metric[%s] arg[%u] value_ll: %lld",\
                        switch_ip, metric_name, if_num, val.value_ll);\
                break;\
            case VAL_TYPE_UINT64:\
                val.value_ull = (p_snmp_value)->val.uint64_val;\
                INFO_LOG("switch[%s] metric[%s] arg[%u] value_ull: %llu",\
                        switch_ip, metric_name, if_num, val.value_ull);\
                break;\
            case VAL_TYPE_FLOAT:\
                val.value_f = (p_snmp_value)->val.float_val;\
                INFO_LOG("switch[%s] metric[%s] arg[%u] value_f: %.2f",\
                        switch_ip, metric_name, if_num, val.value_f);\
                break;\
            case VAL_TYPE_STRING:\
                STRNCPY(val.value_str, (p_snmp_value)->val.str_val, sizeof(val.value_str));\
                INFO_LOG("switch[%s] metric[%s] arg[%u] value_str: %s",\
                        switch_ip, metric_name, if_num, val.value_str);\
                break;\
        }\
    } while (false)

//========================================================================


static map<string, c_itl_snmp *> g_switch_snmp_map;
char g_oid_name[SWITCH_MAX_OID_LEN] = {0};


int free_switch_snmp_map();
c_itl_snmp * get_switch_handler(const char *p_ip, const char *p_community);
int get_single_oid_metric_value(int metric_type, const void *arg, value_t & val);

int metric_init()
{
    free_switch_snmp_map();
    return 0;
}

int metric_fini()
{
    free_switch_snmp_map();
    return 0;
}


int switch_cpu_used_func(const void *arg, value_t & val)
{
    return get_single_oid_metric_value(SWITCH_CPU_USED, arg, val);
}


int switch_mem_free_func(const void *arg, value_t & val)
{
    return get_single_oid_metric_value(SWITCH_MEM_FREE, arg, val);
}


int switch_interface_in_func(const void *arg, value_t & val)
{
    return get_single_oid_metric_value(SWITCH_IF_IN, arg, val);
}


int switch_interface_out_func(const void *arg, value_t & val)
{
    return get_single_oid_metric_value(SWITCH_IF_OUT, arg, val);
}


int switch_interface_info_func(const void *arg, value_t & val)
{
    val.value_str[0] = '\0';

    //获取交换机连接句柄
    const switch_base_info_t *p_base_info = reinterpret_cast<const switch_base_info_t *>(arg);
    const char *p_ip = p_base_info->ip;
    const char *p_community = p_base_info->community;
    c_itl_snmp *p_snmp = get_switch_handler(p_ip, p_community);
    if (NULL == p_snmp)
    {
        return -1;
    }

    //获取交换端口号
    const char *p_oid_name = OID_IFTABLE;
    vector<snmp_value_t> if_index_vec;
    if (0 != p_snmp->simple_snmp_walk(p_oid_name, if_index_vec))
    {
        ERROR_LOG("get switch[%s] ifIndex failed", p_ip);
        return -1;
    }

    //获取各端口基本信息: status、speed、type、desc
    char desc_oid[SWITCH_MAX_OID_LEN] = {0};
    char type_oid[SWITCH_MAX_OID_LEN] = {0};
    char speed_oid[SWITCH_MAX_OID_LEN] = {0};
    char status_oid[SWITCH_MAX_OID_LEN] = {0};
    const char *info_oids[SWITCH_INFO_OID_NUM] = {status_oid, type_oid, speed_oid, desc_oid};
    vector<snmp_value_t> if_info_vec;
    int if_index = 0;
    snmp_value_t *p_if_status = NULL;
    snmp_value_t *p_if_type = NULL;
    snmp_value_t *p_if_speed = NULL;
    snmp_value_t *p_if_desc = NULL;
    char *write_pos = val.value_str;
    int left_len = sizeof(val.value_str);
    vector_for_each (if_index_vec, if_it)
    {
        snmp_value_t *p_snmp_val = &(*if_it);
        if (p_snmp_val->val_type != VAL_TYPE_INT32 || p_snmp_val->val.int32_val <= 0)
        {
            continue;
        }

        if_index = p_snmp_val->val.int32_val;
        snprintf(status_oid, sizeof(status_oid), "%s.%d", OID_IFSTATUS, if_index);
        snprintf(type_oid, sizeof(type_oid), "%s.%d", OID_IFTYPE, if_index);
        snprintf(speed_oid, sizeof(speed_oid), "%s.%d", OID_IFSPEED, if_index);
        snprintf(desc_oid, sizeof(desc_oid), "%s.%d", OID_IFDESC, if_index);

        if_info_vec.clear();
        if (0 != p_snmp->simple_snmp_get(info_oids, SWITCH_INFO_OID_NUM, if_info_vec))
        {
            ERROR_LOG("get switch[%s] ifIndex[%d] info(desc/type/speed/status) failed", p_ip, if_index);
            continue;
        }
        if (if_info_vec.size() != SWITCH_INFO_OID_NUM)
        {
            ERROR_LOG("switch[%s] ifIndex[%d]: just get %zu of info(desc/type/speed/status)",
                    p_ip, if_index, if_info_vec.size());
            continue;
        }

        p_if_status = &if_info_vec[SWITCH_IF_STATUS];
        p_if_type = &if_info_vec[SWITCH_IF_TYPE];
        p_if_speed = &if_info_vec[SWITCH_IF_SPEED];
        p_if_desc = &if_info_vec[SWITCH_IF_DESC];
        if (p_if_status->val_type != VAL_TYPE_INT32 || p_if_status->val.int32_val <= 0
                || p_if_type->val_type != VAL_TYPE_INT32 || p_if_type->val.int32_val <= 0
                || p_if_speed->val_type != VAL_TYPE_UINT32 || p_if_speed->val.uint32_val == 0
                || p_if_desc->val_type != VAL_TYPE_STRING)
        {
            ERROR_LOG("switch[%s] ifIndex[%d]: if_info wrong type or value", p_ip, if_index);
            ERROR_LOG(">>>if_status val_type: %d, value: %d", p_if_status->val_type, p_if_status->val.int32_val);
            ERROR_LOG(">>>if_type val_type: %d, value: %d", p_if_type->val_type, p_if_type->val.int32_val);
            ERROR_LOG(">>>if_speed val_type: %d, value: %u", p_if_speed->val_type, p_if_speed->val.uint32_val);
            ERROR_LOG(">>>if_desc val_type: %d, value: %s", p_if_desc->val_type, p_if_desc->val.str_val);
            continue;
        }

        char *desc_start = strstr(p_if_desc->val.str_val, "STRING: \"");
        if (NULL != desc_start)
        {
            desc_start += strlen("STRING: \"");
        }
        else
        {
            desc_start = p_if_desc->val.str_val;
        }
        char *desc_end = rindex(desc_start, '"');
        if (NULL != desc_end)
        {
            *desc_end = '\0';
        }

        INFO_LOG("switch[%s] ifIndex[%d]: status[%d], type[%d], speed[%u], desc[%s]",
                p_ip, if_index, p_if_status->val.int32_val, p_if_type->val.int32_val,
                p_if_speed->val.uint32_val, desc_start);
        //格式: if_num,if_status,if_type,if_speed,if_desc; 
        int write_len = snprintf(write_pos, left_len, "%d,%d,%d,%u,%s;", if_index,
                p_if_status->val.int32_val, p_if_type->val.int32_val,
                p_if_speed->val.uint32_val / IF_SPEED_BASE, desc_start);
        write_pos += write_len;
        left_len -= write_len;
        if (left_len <= 0)
        {
            ERROR_LOG("value_t string length[%zu] is not enougth.", sizeof(val.value_str));
            break;
        }
    }
    INFO_LOG("switch[%s] info: %s", p_ip, val.value_str);
    if_info_vec.clear();
    if_index_vec.clear();

    return 0;
}

//==========================公用函数==========================
int free_switch_snmp_map()
{
    c_itl_snmp *p_snmp = NULL;
    map<string, c_itl_snmp *>::iterator snmp_it = g_switch_snmp_map.begin();
    while (snmp_it != g_switch_snmp_map.end())
    {
        p_snmp = snmp_it->second;
        if (NULL != p_snmp)
        {
            p_snmp->uninit();
            p_snmp = NULL;
        }
        ++snmp_it;
    }
    g_switch_snmp_map.clear();
    
    return 0;
}

c_itl_snmp * get_switch_handler(const char *p_ip, const char *p_community)
{
    map<string, c_itl_snmp *>::iterator snmp_it;
    snmp_it = g_switch_snmp_map.find(p_ip);
    if (snmp_it != g_switch_snmp_map.end())
    {
        return snmp_it->second;
    }
    c_itl_snmp *p_snmp = new c_itl_snmp;
    if (NULL == p_snmp)
    {
        return NULL;
    }
    if (0 != p_snmp->init(p_ip, p_community, SWITCH_SNMP_DEFAULT_TIMEOUT, SWITCH_SNMP_DEFAULT_RETRY))
    {
        ERROR_LOG("p_snmp->init() failed.");
        delete p_snmp;
        return NULL;
    }
    g_switch_snmp_map.insert(pair<string, c_itl_snmp *>(p_ip, p_snmp));

    return p_snmp;
}


int get_single_oid_metric_value(int metric_type, const void *arg, value_t & val)
{
    assert(metric_type > SWITCH_METRIC_MIN && metric_type < SWITCH_METRIC_MAX);
    val.value_f = 0;

    //获取交换机连接句柄
    const switch_base_info_t *p_base_info = reinterpret_cast<const switch_base_info_t *>(arg);
    const char *p_ip = p_base_info->ip;
    const char *p_community = p_base_info->community;
    c_itl_snmp *p_snmp = get_switch_handler(p_ip, p_community);
    if (NULL == p_snmp)
    {
        return -1;
    }

    //获取采集metric_oid列表
    const char *p_oid_name = NULL;
    const char *metric_name = g_metric_names[metric_type];
    unsigned int switch_type = p_base_info->switch_type;
    unsigned int if_num = p_base_info->if_num;
    GET_METRIC_OID(switch_type, metric_type, if_num, p_oid_name);
    if (NULL == p_oid_name)
    {
        ERROR_LOG("cannot support type[%u] switch[%s] metric[%s]", switch_type, p_ip, metric_name);
        return -1;
    }
    //DEBUG_LOG("metric[%s] arg[%u] oid: %s", metric_name, if_num, p_oid_name);

    //do collect
    vector<snmp_value_t> value_vector;
    if (0 != p_snmp->simple_snmp_get(&p_oid_name, 1, value_vector))
    {
        ERROR_LOG("get type[%u] switch[%s] metric[%s] value failed", switch_type, p_ip, metric_name);
        return -1;
    }
    if (value_vector.size() > 0)
    {
        snmp_value_t *p_snmp_val = &value_vector[0];
        GET_SNMP_VALUE(p_snmp_val, val, p_ip, metric_name, if_num);
        value_vector.clear();
    }

    return 0;
}
