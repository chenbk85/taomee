/** 
 * ========================================================================
 * @file c_itl_snmp.h
 * @brief 
 * @author tonyliu
 * @version 1.0.0
 * @date 2012-08-07
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */
#ifndef  H_C_ITL_SNMP_H
#define  H_C_ITL_SNMP_H

#include <vector>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#define ITL_SNMP_MAX_BUFF_LEN 1024*8
#define ITL_SNMP_DEFAULT_TIMEOUT 4

typedef union {
    int int32_val;
    unsigned int uint32_val;
    long long int64_val;
    unsigned long long uint64_val;
    double float_val;
    char *str_val;
} snmp_value_u;

/**值类型:所有原子数值类型都定义了相应的类型,其余的全部以字符串类型处理*/
enum {
    VAL_TYPE_INT32 = 1, 
    VAL_TYPE_UINT32,
    VAL_TYPE_INT64,
    VAL_TYPE_UINT64, 
    VAL_TYPE_FLOAT,
    VAL_TYPE_STRING
};

typedef struct {
    int val_type;
    snmp_value_u val;
} snmp_value_t;


class c_itl_snmp
{
public:
    c_itl_snmp();
    ~c_itl_snmp();

    /** 
     * @brief 初始化函数
     * 
     * @param p_ip: 机器IP
     * @param p_community: 访问机器的团提名
     * @param timeout: 超时时长，单位: 秒
     * @param retry_times: 失败最大尝试次数
     * 
     * @return 0:succ, -1:failed
     */
    int init(const char *p_ip, const char *p_community, int timeout, int retry_times);

    int uninit();

    /** 
     * @brief 获取oid列表中对应的值
     * 
     * @param oids: 待查询的oid数组
     * @param oid_num: oids数组大小
     * @param value_vector: 存在值的vector
     * 
     * @return 0:succ, -1:failed
     */
    int simple_snmp_get(const char **oids, unsigned int oid_num, std::vector<snmp_value_t> &value_vector);
    //int single_snmp_get(const char *oid, snmp_value_t &snmp_value);

    /** 
     * @brief 获取root_oid_name下面的所有字oid信息
     * 
     * @param root_oid_name: 跟oid名称
     * @param value_vector: 待返回值的vector
     * 
     * @return 0:succ, -1:failed
     */
    int simple_snmp_walk(const char *root_oid_name, std::vector<snmp_value_t> &value_vector);


private:
    bool m_inited;
    uint32_t m_timeout_count;//超时次数
    time_t m_last_timeout;//最后一次超时时间
    netsnmp_session m_snmp_sess;
    void *m_p_sess_handle;
    char m_buff[ITL_SNMP_MAX_BUFF_LEN];
};

#endif
