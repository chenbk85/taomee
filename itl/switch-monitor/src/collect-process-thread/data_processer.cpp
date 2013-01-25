/**
 * =====================================================================================
 *       @file  data_processer.cpp
 *      @brief  
 *       收集数据然后处理报警
 *
 *   @internal
 *     Created  2010-10-18 11:13:42
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  mason, mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include "../lib/log.h"
#include "../lib/utils.h"
#include "../lib/check_hostalive.h"
#include "../lib/http_transfer.h"
#include "../lib/snmplib.h"
#include "data_processer.h"

/** 
 * @brief  创建对象实例
 * @param  pp_instance 对象指针的指针 
 * @return  0 success -1 failed 
 */
int create_data_processer_instance(c_data_processer  **pp_instance)
{
    if(NULL == pp_instance) {   
        ERROR_LOG("pp_instance is null.");
        return -1; 
    }   

    *pp_instance = new (std::nothrow)c_data_processer();
    if(*pp_instance == NULL) {   
        ERROR_LOG("new failed.");
        return -1; 
    }   

    return 0;
}

/** 
 * @brief  构造函数
 * @param   
 * @return   
 */
c_data_processer::c_data_processer()
{
    m_inited = 0;
    m_stop = false; 
    m_p_root = NULL;
    m_p_queue = NULL;
    m_p_switch_group = NULL;
    m_p_metric_set = NULL;
    m_p_default_metric_alarm_set = NULL;
    m_p_specified_metric_alarm_set = NULL;
    m_collect_interval = 0;
    m_p_if_alarm_map = NULL;
    m_pid = 0;
}

/** 
 * @brief  析构函数
 * @param   
 * @return   
 */
c_data_processer::~c_data_processer()
{
    uninit();
}

/** 
 * @brief  释放对象函数与create_instance对应
 * @param   
 * @return   
 */
int c_data_processer::release()
{
    delete this;
    return 0;
}

/** 
 * @brief  反初始化
 * @param   
 * @return   
 */
int c_data_processer::uninit()
{
    if(!m_inited)
    {
        return -1;
    }

    assert(m_pid != 0);
    m_stop = true;
    pthread_join(m_pid, NULL);
    m_stop = false;
    m_pid  = 0;
    m_collect_interval = 0;
    m_p_queue = NULL;
    m_p_root = NULL;
    m_p_switch_group = NULL;
    m_p_if_alarm_map = NULL;
    m_p_metric_set = NULL;
    m_p_specified_metric_alarm_set = NULL;
    m_p_default_metric_alarm_set = NULL;
    m_inited = 0;

    return 0;
}

/** 
 * @brief  初始化函数,要么init成功，要么失败会uninit已经init成功的变量
 * @param   metric_set     metric信息结构的vector
 * @param   switch_group   switch_group结构的指针
 * @param   p_if_alarm     交换机接口对应的报警信息结构的指针
 * @param   switch_table   switch的hahs表的指针
 * @param   p_config       config对象指针
 * @param   p_queue        队列指针
 * @return  0 success -1 failed
 */
int c_data_processer::init(
        metric_info_vec_t* metric_set,
        metric_alarm_vec_t *default_alarm_conf,
        metric_alarm_vec_t *special_alarm_conf,
        switch_group_info_t *switch_group,
        if_alarm_map_t *p_if_alarm,
        hash_t *switch_table,
        const config_var_t *p_config,
        i_ring_queue* p_queue)
{
    if(m_inited) {
        ERROR_LOG("ERROR: c_data_processer been inited.");
        return -1;
    }

    if(NULL == metric_set || NULL == switch_group || NULL == switch_table
            || NULL == default_alarm_conf || NULL == special_alarm_conf
            || NULL == p_config || NULL == p_queue || NULL == p_if_alarm) {
        ERROR_LOG("ERROR: paraments cannot be NULL.");
        return -1;
    }

    m_p_root  = switch_table;
    m_p_queue = p_queue;
    m_p_switch_group = switch_group;
    DEBUG_LOG("Switch number:[%u], pid[%u].", m_p_switch_group->switch_num, getpid());
    for(unsigned index = 0; index < m_p_switch_group->switch_num; index++) {
        DEBUG_LOG("  |____Switch[%uth-%u]ip:[%s]community:[%s]type:[%u]collect_count:[%u].",
                index + 1,
                m_p_switch_group->switch_table[index].id,
                m_p_switch_group->switch_table[index].ip,
                m_p_switch_group->switch_table[index].community,
                m_p_switch_group->switch_table[index].type,
                m_p_switch_group->switch_table[index].collect_count);
    }
    m_p_metric_set = metric_set;
    m_p_default_metric_alarm_set = default_alarm_conf;
    m_p_specified_metric_alarm_set = special_alarm_conf;
    m_p_if_alarm_map = p_if_alarm;
    m_collect_interval = p_config->collect_interval;

    //if(m_collect_interval <= 0 || m_collect_interval > 200)
    //{
    //    DEBUG_LOG("The collect interval is wrong,use default 60.");
    //    m_collect_interval = 60;
    //}

    int ret = pthread_create(&m_pid, NULL, data_processer_main, this);
    if(0 == ret) {
        m_inited = 1;
        return 0;
    } else {
        m_stop = false;
        m_pid  = 0;
        m_p_queue = NULL;
        m_p_root = NULL;
        m_p_switch_group = NULL;
        m_p_metric_set = NULL;
        m_p_specified_metric_alarm_set = NULL;
        m_p_default_metric_alarm_set = NULL;
        m_p_if_alarm_map = NULL;
        m_collect_interval = 0;
        return -1;
    }
}

/** 
 * @brief   线程主函数
 * @param   p_data  用户数据
 * @return  NULL success UNNULL failed
 */
void* c_data_processer::data_processer_main(void  *p_data)
{
    assert(NULL != p_data);
    c_data_processer     *processer = (c_data_processer*)p_data;
    switch_group_info_t  *switch_group = processer->m_p_switch_group;
    metric_info_vec_t    *metric_set = processer->m_p_metric_set;
    hash_t               *switch_table = processer->m_p_root;
    unsigned int         start = 0, interval = 0;

    unsigned int tid = pthread_self();
    DEBUG_LOG("Processor thread[%u] enter main while loop.", tid);
    while(!processer->m_stop) {
        start = time(NULL);
        unsigned int unconn_cnt = 0;
        for(unsigned int i = 0; i < switch_group->switch_num; i++) {
            if (switch_group->switch_table[i].unconnect) {//之前连续10次连接不上则跳过
                unconn_cnt++;
                continue;
            }
            datum_t *switch_data = NULL;
            datum_t  switch_key = {(void*)(switch_group->switch_table[i].ip),
                strlen(switch_group->switch_table[i].ip) + 1};
            datum_t  switch_val = {NULL, 0};
            //缓存当前的switch的信息
            switch_t switch_tmp;
            int new_hash_for_metrics = 0, new_hash_for_metrics_status = 0;

            //如果存在switch信息
            if((switch_data = hash_lookup(&switch_key, switch_table)) != NULL) {
                //把数据拷贝出来
                memcpy(&switch_tmp, switch_data->data, sizeof(switch_tmp));
                datum_free(switch_data);
            } else {
                memset(&switch_tmp, 0, sizeof(switch_tmp));
                switch_tmp.dmax = DEFAULT_DMAX;
                switch_tmp.id = switch_group->switch_table[i].id;
            }

            //创建metrics的hash table
            if(NULL == switch_tmp.metrics) {
                if((switch_tmp.metrics = hash_create(DEFAULT_METRICSIZE)) == NULL) {
                    ERROR_LOG("Tid[%u] create metris hash table for switch[%s] failed.",
                            tid, switch_group->switch_table[i].ip);
                    continue;
                }
                new_hash_for_metrics = 1;
            }

            //创建metrics_status的hash table
            if(NULL == switch_tmp.metrics_status) {
                if((switch_tmp.metrics_status = hash_create(128)) == NULL) {
                    ERROR_LOG("Tid[%u] create metircs_status hash table for switch[%s] failed.",
                            tid, switch_group->switch_table[i].ip);
                    if(new_hash_for_metrics) {
                        hash_destroy(switch_tmp.metrics);
                    }
                    continue;
                }
                new_hash_for_metrics_status = 1;
            }

            //if(check_hostalive(switch_group->switch_table[i].ip, 10) != STATE_OK)
            //{
            //    switch_group->switch_table[i].down = 1;
            //    continue;
            //}
            //else if(switch_group->switch_table[i].down == 1)
            //{
            //    switch_group->switch_table[i].down = 0;
            //}

            //如果是初始启动或者达到了收集间隔,或者是上次收集不成功
            ++switch_group->switch_table[i].collect_count;
            if(switch_tmp.if_num == 0 || switch_group->switch_table[i].collect_count >= DEFAULT_COLLECT_IF_INFO_COUNT) {
                //收集本交换机的接口的信息
                if(0 != processer->collect_interface_info(&(switch_group->switch_table[i]), &switch_tmp)) {
                    ERROR_LOG("Tid[%u] collect interface info of switch[%s] failed[%uth].",
                            tid, switch_group->switch_table[i].ip, switch_group->switch_table[i].collect_count);
                    if(new_hash_for_metrics) {
                        hash_destroy(switch_tmp.metrics);
                    }
                    if(new_hash_for_metrics_status) {
                        hash_destroy(switch_tmp.metrics_status);
                    }
                    if (switch_tmp.if_num == 0 && switch_group->switch_table[i].collect_count >= MAX_UNCONNECT_TIMES) {
                        DEBUG_LOG("Tid[%u]: cannot connect switch[%s] times[%u] continuously, so remove it from collection",
                                tid, switch_group->switch_table[i].ip, switch_group->switch_table[i].collect_count);
                        switch_group->switch_table[i].unconnect = true;
                    }
                    continue;
                }
                switch_group->switch_table[i].unconnect = false;
                switch_group->switch_table[i].collect_count = 0;
            }

            //收集本交换机的接口的出入流量
            if(0 != processer->collect_interface_data(&(switch_group->switch_table[i]), &switch_tmp)) {
                ERROR_LOG("Tid[%u] collect interface data of switch[%s] failed.", tid, switch_group->switch_table[i].ip);
            }

            //收集本交换机的基本项metric的数据
            metric_info_vec_t::iterator itm = metric_set->begin();
            for(; itm != metric_set->end(); itm++) {
                if(processer->collect_metric_data(&(*itm), &(switch_group->switch_table[i]), &switch_tmp) != 0) {
                    ERROR_LOG("Tid[%u] collect metric[%s] data of switch[%s] failed.", 
                            tid, (*itm).metric_name, switch_group->switch_table[i].ip);
                }
            }

            //更新接受数据时间
            switch_tmp.recv_time = time(0);
            switch_val.data = (void*)&switch_tmp;
            switch_val.size = sizeof(switch_tmp);
            //更新switch信息
            if(hash_insert(&switch_key, &switch_val, switch_table) == NULL) {
                ERROR_LOG("Tid[%u] update switch[%s] data into hash failed.", tid, switch_group->switch_table[i].ip);
            }
        }
        interval = time(NULL) - start;
        if(interval < processer->m_collect_interval) {
            unsigned int sec = 0;
            while(sec++ < (processer->m_collect_interval - interval) && !processer->m_stop) {
                sleep(1);
            }
        } else {
            DEBUG_LOG("Tid[%u] collect data interval[%u] >= m_collect_interval[%u].",
                    tid, interval, processer->m_collect_interval);
        }

        if (unconn_cnt >= switch_group->switch_num) {
            DEBUG_LOG("Switch unconnect count[%d] reach group_max_num, so exit the thread[%u].", unconn_cnt, tid);
            break;
        }
    }
    DEBUG_LOG("Tid[%u] exit main while loop of data_processer_thread.", tid);
    return NULL;
}

/** 
 * @brief   收集一个交换机的所有接口的信息
 * @param   switch_info      switch_info_t结构的指针
 * @param   switch_data      switch_t结构的指针
 * @return  -1 = failed 0 successed
 */
int c_data_processer::collect_interface_info(const switch_info_t *switch_info, switch_t *switch_data)
{
    if(NULL == switch_info || NULL == switch_data) {
        ERROR_LOG("ERROR: parameter cannot be NULL.");
        return -1;
    }

    //值结果参数,传入最大个数，传出实际个数
    unsigned int ifs_count = sizeof(switch_data->if_table) / sizeof(switch_data->if_table[0]);
    if(0 != get_interface_info(switch_data->if_table, &ifs_count, switch_info->ip, switch_info->community)) {
        ERROR_LOG("Get interface info of switch[%s] failed.", switch_info->ip);
        return -1;
    }
    DEBUG_LOG("Get interface info of switch[%s] success if_count[%u].", switch_info->ip, ifs_count);
    switch_data->if_num = ifs_count;
    return 0;
}

/** 
 * @brief   收集一个交换机的接口的信息
 * @param   ifs              switch_interface_info_t结构数组
 * @param   ifs_count        数组长度是个值结果参数
 * @param   switch_ip        switch ip
 * @param   switch_community switch community
 * @return  -1 = failed 0 successed
 */
int c_data_processer::get_interface_info(
        switch_interface_info_t *ifs,
        unsigned int *ifs_count,
        const char *switch_ip,
        const char *switch_community) 
{
    if(NULL == ifs || NULL == ifs_count || NULL == switch_ip || NULL == switch_community) {
        ERROR_LOG("ERROR: parameter cannot be NULL.");
        return -1;
    }

    snmp_val_t    val_list_if_idx[MAX_IF_NUM];
    snmp_val_t    val_list_if_info[3];
    unsigned int  val_list_len = sizeof(val_list_if_idx) / sizeof(val_list_if_idx[0]);
    char          buf[1024] = {0};//for store string field
    unsigned int  max_ifs_count = *ifs_count;
    unsigned int  i = 0, k = 0;
    memset(val_list_if_idx, 0, sizeof(val_list_if_idx));

    if(simple_snmp_walk(switch_ip, switch_community, IFTABLE_OID, DEFAULT_TIME_OUT, 0,
                val_list_if_idx, &val_list_len, buf, sizeof(buf)) != 0) {
        ERROR_LOG("Get the switch [%s]'s interfacc ifIdexs failed.", switch_ip);
        return -1;
    }

    unsigned count = val_list_len;
    for(i = 0; i < count && k < max_ifs_count; i++) {
        if(val_list_if_idx[i].val_type != VAL_TYPE_INT32 || val_list_if_idx[i].val.int32_val <= 0) {
            continue;
        }

        val_list_len = sizeof(val_list_if_info) / sizeof(val_list_if_info[0]);
        memset(val_list_if_info, 0, sizeof(val_list_if_info));
        char desc_oid[256] = {0};
        char type_oid[256] = {0};
        char speed_oid[256] = {0};
        const char *info_oids[3] = {desc_oid, type_oid, speed_oid};
        snprintf(desc_oid, sizeof(desc_oid) - 1, "%s.%d", IFDESC_OID, val_list_if_idx[i].val.int32_val);
        snprintf(type_oid, sizeof(type_oid) - 1, "%s.%d", IFTYPE_OID, val_list_if_idx[i].val.int32_val);
        snprintf(speed_oid, sizeof(speed_oid) - 1, "%s.%d", IFSPEED_OID, val_list_if_idx[i].val.int32_val);

        if(simple_snmp_get(switch_ip, switch_community, info_oids,
                    sizeof(info_oids) / sizeof(info_oids[0]), DEFAULT_TIME_OUT, 0,
                    val_list_if_info, &val_list_len, buf, sizeof(buf)) != 0) {
            ERROR_LOG("Get the switch [%s]'s interfacc [%d]'s info[desc, type, speed] failed.", 
                    switch_ip, val_list_if_idx[i].val.int32_val);
            continue;
        }

        //DEBUG_LOG("Get the switch [%s]'s interfacc [%d]'s info success val list len = [%u].", 
        //switch_ip, val_list_if_idx[i].val.int32_val, val_list_len);

        if(val_list_len != 3)//三个oid应该返回三个value
        {
            ERROR_LOG("Not all the interface info gruped.");
            continue;
        }

        ifs[k].idx = (unsigned int)(val_list_if_idx[i].val.int32_val);
        if(val_list_if_info[0].val_type == VAL_TYPE_STR && val_list_if_info[0].val.str_val != NULL) {
            //去掉类型前缀
            char *pos = val_list_if_info[0].val.str_val;
            char *endline = NULL;
            size_t type_len = strlen("STRING: \"");
            if(!strncasecmp(pos, "STRING: \"", type_len)) {
                pos += type_len;
            }
            if((endline = rindex(pos, '\"')) != NULL) {
                *endline = '\0';
            }
            strncpy(ifs[k].desc, pos, sizeof(ifs[k].desc) - 1);
        }
        if(val_list_if_info[1].val_type == VAL_TYPE_INT32 && val_list_if_info[1].val.int32_val > 0) {
            ifs[k].type = (unsigned int)val_list_if_info[1].val.int32_val;
        }
        if(val_list_if_info[2].val_type == VAL_TYPE_UINT32 && val_list_if_info[2].val.uint32_val != 0) {
            ifs[k].speed = val_list_if_info[2].val.uint32_val / BITS_PER_MBIT;
        }
        k++;
    }
    *ifs_count = k;
    return 0;
}

/** 
 * @brief   收集一个接口的出入流量
 * @param   switch_info      switch_info_t结构的指针
 * @param   switch_data      switch_t结构的指针
 * @return  -1 = failed 0 successed
 */
int c_data_processer::collect_interface_data(const switch_info_t* switch_info, switch_t *switch_data)
{
    if(NULL == switch_info || NULL == switch_data) {
        ERROR_LOG("ERROR: parameter cannot be NULL.");
        return -1;
    }
    if(0 == switch_data->if_num) {
        ERROR_LOG("Switch[%s] interface number is zero.", switch_info->ip);
        return 0;
    }

    switch_interface_info_t *if_table_begin = switch_data->if_table; 
    ///一次处理50个接口
    unsigned int i = 0;
    unsigned int group_count = (switch_data->if_num % MAX_IF_NUM_PER_GROUP) == 0 ?
        (switch_data->if_num / MAX_IF_NUM_PER_GROUP) :
        ((unsigned int)(switch_data->if_num / MAX_IF_NUM_PER_GROUP) + 1);

    while(i < group_count) {
        unsigned int if_num = i == (group_count - 1) ?
            (switch_data->if_num - MAX_IF_NUM_PER_GROUP * (group_count - 1)) : MAX_IF_NUM_PER_GROUP;
        if(0 != get_interface_status(if_table_begin, if_num,  switch_info->ip, switch_info->community)) {
            ERROR_LOG("Get interface status of switch[%s] failed.", switch_info->ip);
            return -1;
        }

        if(0 != get_interface_in_out(if_table_begin, if_num, switch_info->ip, switch_info->community)) {
            ERROR_LOG("Get interface in&out bits of switch[%s] failed.", switch_info->ip);
            return -1;
        }

        if_table_begin += MAX_IF_NUM_PER_GROUP;
        i++;
    }

    if(0 != handle_interface_alarm(switch_info, switch_data)) {
        ERROR_LOG("Handle interface alarm of switch[%s] failed.", switch_info->ip);
    }

    return 0;
}

/** 
 * @brief   处理一个接口的流量报警
 * @param   switch_info      switch_info_t结构的指针
 * @param   switch_data      switch_t结构的指针
 * @param   idx              在if_table数组中的下标
 * @return  -1 = failed 0 successed
 */
int c_data_processer::handle_interface_alarm(const switch_info_t* switch_info, switch_t *switch_data)
{
    if(switch_info == NULL || switch_data == NULL) {
        return -1;
    }

    if(!switch_data->metrics_status) {
        return -1;
    }

    for(unsigned int idx = 0; idx < switch_data->if_num; idx++) {
        if(switch_data->if_table[idx].status == 2 || switch_data->if_table[idx].speed == 0 ||
                switch_data->if_table[idx].type == 1)
            continue; 
        char interface_metric_name[MAX_NAME_SIZE] = {0};
        alarm_info_t *alarm_info = NULL;
        if_alarm_map_t::iterator it= m_p_if_alarm_map->find(switch_data->if_table[idx].speed);
        if(it != m_p_if_alarm_map->end()) {
            alarm_info = &(it->second);
        }
        if(alarm_info == NULL) {
            ERROR_LOG("No alarm info for the interfcae with speed %u.", switch_data->if_table[idx].speed);
            continue;
            //return -1;
        }

        metric_status_info_t tmp_metric_status; 
        datum_t *metric_status = NULL; 

        snprintf(interface_metric_name, MAX_NAME_SIZE, "if_%u_in", switch_data->if_table[idx].idx);
        datum_t metric_status_key = {(void*)(interface_metric_name), strlen(interface_metric_name) + 1};
        metric_status = hash_lookup(&metric_status_key, switch_data->metrics_status);
        if(metric_status == NULL) {
            memset(&tmp_metric_status, 0, sizeof(tmp_metric_status));
            tmp_metric_status.cur_atc = 1;
        }
        else {
            memcpy(&tmp_metric_status, metric_status->data, metric_status->size);
            datum_free(metric_status);
        }

        if(handler_alarm(switch_info->ip, alarm_info, &tmp_metric_status, 
                    interface_metric_name, switch_data->if_table[idx].in_bits_per_sec) == 0) {
            tmp_metric_status.last_alarm = time(0);
            datum_t metric_status_val = {(void*)(&tmp_metric_status), sizeof(tmp_metric_status)};
            if(hash_insert(&metric_status_key, &metric_status_val, switch_data->metrics_status) == NULL) {
                ERROR_LOG("Insert %s metric status info into metrics_status hash failed.",
                        interface_metric_name);
            }
        }

        snprintf(interface_metric_name, MAX_NAME_SIZE, "if_%u_out", switch_data->if_table[idx].idx);
        metric_status_key.data = (void*)(interface_metric_name);
        metric_status_key.size = strlen(interface_metric_name) + 1;
        metric_status = hash_lookup(&metric_status_key, switch_data->metrics_status);
        if(metric_status == NULL)
        {
            memset(&tmp_metric_status, 0, sizeof(tmp_metric_status));
            tmp_metric_status.cur_atc = 1;
        }
        else
        {
            memcpy(&tmp_metric_status, metric_status->data, metric_status->size);
            datum_free(metric_status);
        }
        if(handler_alarm(switch_info->ip, alarm_info, &tmp_metric_status, 
                    interface_metric_name, switch_data->if_table[idx].out_bits_per_sec) == 0)
        {
            tmp_metric_status.last_alarm = time(0);
            datum_t metric_status_val = {(void*)(&tmp_metric_status), sizeof(tmp_metric_status)};
            if(hash_insert(&metric_status_key, &metric_status_val, switch_data->metrics_status) == NULL)
            {
                ERROR_LOG("Insert %s metric status info into metrics_status hash failed.",
                        interface_metric_name);
            }
        }
    }
    return 0;
}

/** 
 * @brief   收集一个交换机的接口的状态
 * @param   if_info          switch_interface_info_t结构指针
 * @param   switch_ip        switch ip
 * @param   switch_community switch community
 * @return  -1 = failed 0 successed
 */
int c_data_processer::get_interface_status(
        switch_interface_info_t *if_table,
        unsigned int if_num,
        const char *switch_ip,
        const char *switch_community) 
{
    if(NULL == if_table || 0 == if_num || NULL == switch_ip || NULL == switch_community) {
        ERROR_LOG("ERROR: parameter wrong.");
        return -1;
    }

    const char   *if_status_oids[MAX_IF_NUM] = {NULL};
    char          oids[MAX_IF_NUM][256] = {{0}};//保存每个接口的status的oid
    snmp_val_t    val_list_if_status[MAX_IF_NUM];
    unsigned int  val_list_len = sizeof(val_list_if_status) / sizeof(val_list_if_status[0]);
    char          buf[1] = {0};
    unsigned int  i;

    for(i = 0; i < if_num; i++) {
        snprintf(oids[i], 255, "%s.%u", IFSTATUS_OID, if_table[i].idx);
        if_status_oids[i] = oids[i];
    }
    memset(val_list_if_status, 0, sizeof(val_list_if_status));
    if(0 != simple_snmp_get(switch_ip, switch_community, if_status_oids, if_num, 
                DEFAULT_TIME_OUT, 0, val_list_if_status, &val_list_len, buf, sizeof(buf))) {
        ERROR_LOG("Get the switch[%s] interface status failed.", switch_ip);
        return -1;
    }
    if(val_list_len != if_num) {
        ERROR_LOG("Not all interface status of switch[%s] gruped.", switch_ip);
        return -1;
    }

    for(i = 0; i < val_list_len; i++) {
        if(val_list_if_status[i].val_type != VAL_TYPE_INT32 || val_list_if_status[i].val.int32_val <= 0) {
            continue;
        }

        if(1 == val_list_if_status[i].val.int32_val) {
            if_table[i].status = 1;
        } else {
            if_table[i].status = 2;
        }
    }

    return 0;
}

/** 
 * @brief   收集一个交换机的接口的出入流量
 * @param   if_info          switch_interface_info_t结构指针
 * @param   switch_ip        switch ip
 * @param   switch_community switch community
 * @return  -1 = failed 0 successed
 */
int c_data_processer::get_interface_in_out(
        switch_interface_info_t *if_table,
        unsigned int if_num,
        const char *switch_ip,
        const char *switch_community) 
{
    if(NULL == if_table || 0 == if_num || NULL == switch_ip || NULL == switch_community) {
        ERROR_LOG("ERROR: parameter wrong.");
        return -1;
    }

    const char *if_io_oids[MAX_IF_NUM] = {NULL};
    char oids[MAX_IF_NUM][256] = {{0}};//保存每个接口的io的oid
    unsigned int real_if_count = 0;            //真正需要监控流量的接口的个数
    snmp_val_t val_list_if_io[MAX_IF_NUM];
    unsigned int val_list_len = sizeof(val_list_if_io) / sizeof(val_list_if_io[0]);
    char buf[1] = {0};//因没有字符串，其长度只取1
    unsigned int i, k;

    for(i = 0; i < if_num; i++) {
        //如果这个接口down了,或者是null(1)端口,或者速度为0(很诡异)
        if(if_table[i].status == 2 || if_table[i].speed == 0 || if_table[i].type == 1) {
            continue; 
        }
        snprintf(oids[real_if_count], 255, "%s.%u", IFINOCTER_OID, if_table[i].idx);
        if_io_oids[real_if_count] = oids[real_if_count];
        real_if_count++;
    }

    if(0 != simple_snmp_get(switch_ip, switch_community, if_io_oids, real_if_count, 
                DEFAULT_TIME_OUT, 0, val_list_if_io, &val_list_len, buf, sizeof(buf))) {
        ERROR_LOG("Get the interface of switch[%s] in octer failed.", switch_ip);
        return -1;
    }
    if(val_list_len != real_if_count) {
        ERROR_LOG("Not all the interface of switch[%s] in octer gruped.", switch_ip);
        return -1;
    }

    unsigned int now = time(NULL);
    for(i = 0, k = 0; i < if_num && k < val_list_len; i++) {
        if(if_table[i].status == 2 || if_table[i].speed == 0 || if_table[i].type == 1) {
            continue; 
        }
        uint32_t in_bytes = 0; 
        if(val_list_if_io[k].val_type != VAL_TYPE_UINT32 || val_list_if_io[k].val.uint32_val == 0) {
            in_bytes = 0;
        } else {
            in_bytes = val_list_if_io[k].val.uint32_val;
        }
        k++;

        if_table[i].in_bits_per_sec = in_bytes;
        //if(in_bytes >= if_table[i].last_in_bytes) {
        //    if_table[i].in_bits_per_sec = 8 * (in_bytes - if_table[i].last_in_bytes) / 
        //        (now > if_table[i].last_in_report ? (now - if_table[i].last_in_report) : m_collect_interval);
        //} else {
        //    if_table[i].in_bits_per_sec = 8 * (in_bytes + (MAX_UINT32 - if_table[i].last_in_bytes)) / 
        //        (now > if_table[i].last_in_report ? (now - if_table[i].last_in_report) : m_collect_interval);
        //} 

        //if (in_bytes > 0 && in_bytes < SWITCH_MAX_IO_STREAM) {
        //long diff_bytes = in_bytes - if_table[i].last_in_bytes;
        //if (in_bytes > 0 && diff_bytes < if_table[i].speed) {
        if (in_bytes > 0) {
            if_table[i].last_in_bytes = in_bytes;
            if_table[i].last_in_report = now;
        }
        else {
            ERROR_LOG("IN_BYTES WRONG: if_idx[%u], in_bytes[%u], speed[%u], val_type[0X%X], value[%u]", 
                    if_table[i].idx, in_bytes, if_table[i].speed,
                    val_list_if_io[k].val_type, val_list_if_io[k].val.uint32_val);
        }
    }

    val_list_len = sizeof(val_list_if_io) / sizeof(val_list_if_io[0]);
    memset(val_list_if_io, 0, sizeof(val_list_if_io));
    real_if_count = 0;
    for(i = 0; i < if_num; i++) {
        if(if_table[i].status == 2 || if_table[i].speed == 0 || if_table[i].type == 1) {
            continue; 
        }
        snprintf(oids[real_if_count], 255, "%s.%u", IFOUTOCTER_OID, if_table[i].idx);
        if_io_oids[real_if_count] = oids[real_if_count];
        real_if_count++;
    }

    if(0 != simple_snmp_get(switch_ip, switch_community, if_io_oids, real_if_count, 
                DEFAULT_TIME_OUT, 0, val_list_if_io, &val_list_len, buf, sizeof(buf))) {
        ERROR_LOG("Get the interface of switch[%s] out octer failed.", switch_ip);
        return -1;
    }
    if(val_list_len != real_if_count) {
        ERROR_LOG("Not all the interface of switch[%s] out octer gruped.", switch_ip);
        return -1;
    }
    now = time(NULL);
    for(i = 0, k = 0; i < if_num && k < val_list_len; i++) {
        if(if_table[i].status == 2 || if_table[i].speed == 0 || if_table[i].type == 1) {
            continue; 
        }
        uint32_t out_bytes = 0; 
        if(val_list_if_io[k].val_type != VAL_TYPE_UINT32 || val_list_if_io[k].val.uint32_val == 0) {
            out_bytes = 0;
        } else {
            out_bytes = val_list_if_io[k].val.uint32_val;
        }
        k++;

        if_table[i].out_bits_per_sec = out_bytes;
        //if(out_bytes >= if_table[i].last_out_bytes) {
        //    if_table[i].out_bits_per_sec = 8 * (out_bytes - if_table[i].last_out_bytes) / 
        //        (now > if_table[i].last_out_report ? (now - if_table[i].last_out_report) : m_collect_interval);
        //} else {
        //    if_table[i].out_bits_per_sec = 8 * (out_bytes + (MAX_UINT32 - if_table[i].last_out_bytes)) / 
        //        (now > if_table[i].last_out_report ? (now - if_table[i].last_out_report) : m_collect_interval);
        //} 
        //if (out_bytes > 0 && out_bytes < SWITCH_MAX_IO_STREAM) {
        //long diff_bytes = out_bytes - if_table[i].last_out_bytes;
        //if (out_bytes > 0 && diff_bytes < if_table[i].speed) {
        if (out_bytes > 0) {
            if_table[i].last_out_bytes = out_bytes;
            if_table[i].last_out_report = now;
        }
        else {
            ERROR_LOG("OUT_BYTES WRONG: if_idx[%u], out_bytes[%u], speed[%u], val_type[0X%X], value[%u]", 
                    if_table[i].idx, out_bytes, if_table[i].speed,
                    val_list_if_io[k].val_type, val_list_if_io[k].val.uint32_val);
        }
    }

    return 0;
}

/** 
 * @brief   收集一个交换机的一个metric的数据
 * @param   metric         metric_info_t结构指针
 * @param   switch_info    switch_info_t结构的指针
 * @param   switch_data    switch_t结构的指针
 * @return  -1 = failed 0 successed
 */
int c_data_processer::collect_metric_data(const metric_info_t* metric, const switch_info_t* switch_info, switch_t* switch_data)
{
    if(metric == NULL || switch_info == NULL || switch_data == NULL)
        return -1;

    snmp_val_t    val_list_metric[1];
    unsigned int  val_list_len = sizeof(val_list_metric) / sizeof(val_list_metric[0]);
    char          buf[1] = {0};
    const char   *metric_oid[1];
    const char   *oid = NULL;
    datum_t       metric_key = {NULL, 0};
    datum_t       metric_val = {NULL, 0};

    for(unsigned int i = 0;
            i < sizeof(switch_metric_oid_table) / sizeof(switch_metric_oid_table[0]); i++)
    {
        if(switch_info->type == switch_metric_oid_table[i].type &&
                !strcasecmp(metric->metric_name, switch_metric_oid_table[i].metric_name))
            oid = switch_metric_oid_table[i].oid;
    }
    if(oid == NULL)
    {
        ERROR_LOG("There are no oid info of metric %s for %s switch.", 
                metric->metric_name, switch_info->type == 1 ? "cisco" : "h3c");
        return -1;
    }

    metric_oid[0] = oid;
    if(simple_snmp_get(switch_info->ip, switch_info->community, metric_oid, sizeof(metric_oid) / sizeof(metric_oid[0]),
                DEFAULT_TIME_OUT, 0, val_list_metric, &val_list_len, buf, sizeof(buf)) != 0)
    {
        ERROR_LOG("Get the switch [%s]'s metric [%s]'s info failed.", 
                switch_info->ip, metric->metric_name);
        return -1;
    }
    if(val_list_len < 1)
    {
        ERROR_LOG("No data grub from the switch [%s]'s metric [%s].", 
                switch_info->ip, metric->metric_name);
        return -1;
    }
    if(val_list_metric[0].val_type != VAL_TYPE_INT32 || val_list_metric[0].val.int32_val <= 0)
    {
        ERROR_LOG("Data grub from the switch [%s]'s metric [%s] is wrong.", 
                switch_info->ip, metric->metric_name);
        return -1;
    }

    metric_t metric_data = {0, {0}, DEFAULT_DMAX, "both"};
    metric_data.val.d = val_list_metric[0].val.int32_val;
    metric_data.recv_time = time(0);//拉取的数据正确才更新接收时间
    metric_key.data = (void*)metric->metric_name;
    metric_key.size = strlen(metric->metric_name) + 1;
    metric_val.data = (void*)(&metric_data);
    metric_val.size = sizeof(metric_data);

    //更新metric的值
    if(hash_insert(&metric_key, &metric_val, switch_data->metrics) == NULL)
    {
        ERROR_LOG("Insert %s's metric data to hash table for switch %s failed.", 
                metric->metric_name, switch_info->ip);
        return -1;
    }

    //处理这个metric的报警
    if(do_metric_alarm(metric, metric_data.val.d, switch_info, switch_data) != 0)
    {
        ERROR_LOG("Handle %s's metric alarm for switch %s failed.", metric->metric_name, switch_info->ip);
    }
    return 0;
}

/** 
 * @brief   处理一个非接口类型metric的报警 
 * @param   metric         metric_info_t结构指针
 * @param   metric_val     metric的当前值
 * @param   switch_info    switch_info_t结构的指针
 * @param   switch_data    switch_t结构的指针
 * @return  void 
 */
int c_data_processer::do_metric_alarm(const metric_info_t *metric, double metric_val, const switch_info_t *switch_info, switch_t *switch_data)
{
    if(metric == NULL || switch_info == NULL || switch_data == NULL)
    {
        return -1;
    }

    datum_t *metric_status_data = NULL;
    datum_t  metric_key = {(void*)metric->metric_name, strlen(metric->metric_name) + 1};
    metric_status_info_t metric_status_tmp;

    if(!switch_data->metrics_status)
    {
        ERROR_LOG("metircs_status hahs talbe for switch %s is null.", switch_info->ip);
        return -1;
    }

    if((metric_status_data = hash_lookup(&metric_key, switch_data->metrics_status)) == NULL)
    {
        memset(&metric_status_tmp, 0, sizeof(metric_status_tmp));
        metric_status_tmp.cur_atc = 1;
    }
    else
    {
        memcpy(&metric_status_tmp, metric_status_data->data, sizeof(metric_status_tmp));
        datum_free(metric_status_data);
    }

    alarm_info_t *alarm_info = NULL;
    metric_alarm_vec_t::iterator mai = m_p_specified_metric_alarm_set->begin();
    for(; mai != m_p_specified_metric_alarm_set->end(); mai++)
    {
        if((*mai).switch_id == switch_info->id && !strcmp((*mai).metric_name, metric->metric_name))
        {
            alarm_info = &((*mai).alarm_info);
        }
    }
    if(alarm_info == NULL)
    {
        metric_alarm_vec_t::iterator mai = m_p_default_metric_alarm_set->begin();
        for(; mai != m_p_default_metric_alarm_set->end(); mai++)
        {
            if((*mai).switch_id == 0 && !strcmp((*mai).metric_name, metric->metric_name))
            {
                alarm_info = &((*mai).alarm_info);
            }
        }
    }

    if(handler_alarm(switch_info->ip, alarm_info, &metric_status_tmp, 
                metric->metric_name, metric_val) == 0)
    {
        metric_status_tmp.last_alarm = time(0);
        datum_t metric_status_val = {(void*)&metric_status_tmp, sizeof(metric_status_tmp)};
        if(hash_insert(&metric_key, &metric_status_val, switch_data->metrics_status) == NULL)
        {
            ERROR_LOG("Update the metric_status info of metric %s in switch %s failed.", 
                    metric->metric_name, switch_info->ip);
            return -1;
        }
        return 0;
    }
    return -1;
}

/** 
 * @brief   处理一个metric的报警 
 * @param   ip             switch ip
 * @param   alarm_info     alarm_info_t结构的指针
 * @param   metric_status  metric_status_info_t结构指针
 * @param   metric_name    metric name
 * @param   cur_val        metric的当前值
 * @return  void 
 */
int c_data_processer::handler_alarm(const char *ip, const alarm_info_t *alarm_info, metric_status_info_t *metric_status, const char *metric_name, double cur_val)
{
    return 0;//TODO
    if(ip == NULL || alarm_info == NULL || metric_status == NULL || metric_name == NULL) {
        return -1;
    }

    //first of all add 1 to the check_count
    metric_status->check_count++;        

    //每拉取固定的"次数间隔"，那么探测次数(atc)加1，然后将拉取计数置0
    if(metric_status->check_count % (metric_status->is_normal ? 
                alarm_info->retry_interval : alarm_info->normal_interval) == 0)        {
        metric_status->cur_atc = 
            metric_status->cur_atc == alarm_info->max_atc ? alarm_info->max_atc : metric_status->cur_atc + 1;
        metric_status->check_count = 0;
    } else {
        return 0;
    }

    state_t  metric_state = STATE_O; //-1 = unknown 0 = ok, 1 = warning, 2 = critical

    //获得当前的值的警报状态
    metric_state = get_state(alarm_info->warning_val, alarm_info->critical_val, cur_val, alarm_info->op);

    status_t prev_status = metric_status->cur_status;

    //第一个参数是个值结果参数
    status_change(&(metric_status->cur_status), metric_state, 
            metric_status->cur_atc == alarm_info->max_atc);

    switch(metric_status->cur_status) {
    case  STATUS_OK:
    {
        if(prev_status == STATUS_SW || prev_status == STATUS_SC) {
            metric_status->is_normal = 0;
            metric_status->check_count = 0;
        }

        if(prev_status == STATUS_HW || prev_status == STATUS_HC) {
            //消除警报
            report_alarm(ip, metric_name, 1, NULL, NULL);
        }

        metric_status->cur_atc = 1;
        break;
    }  
    case  STATUS_SW:
    {
        if(metric_status->is_normal == 0) {
            //改变探测频度继续尝试
            metric_status->is_normal = 1;
            metric_status->check_count = 0;
        } 
        break;
    }
    case  STATUS_HW:
    {
        //如果上次也是HW状态那么已经报过警了，就歇一会吧
        if(prev_status == STATUS_HW) {
            //break;
        }

        if(prev_status == STATUS_HC) {
            //撤销上次的HC报警，然后报HW
            //report_alarm(ip, metric_name, 1, NULL, NULL);
        }

        if(prev_status == STATUS_SW || prev_status == STATUS_SC) {
            //改变探测频度继续尝试
            metric_status->is_normal = 0;
            metric_status->check_count = 0;
        }

        metric_alarm_info_t ma;
        char  val_str[MAX_STR_LEN] = {'\0'};
        char  t_val_str[MAX_STR_LEN] = {'\0'};
        snprintf(t_val_str, sizeof(t_val_str) - 1, "%0.2f", cur_val);
        snprintf(val_str, sizeof(val_str) - 1, "%0.2f", alarm_info->warning_val);

        ma.cur_val = t_val_str;
        ma.op = alarm_info->op;
        ma.threshold_val = val_str;
        report_alarm(ip, metric_name, 0, "warning", &ma);
        break;
    }
    case  STATUS_SC:
    {
        if(metric_status->is_normal == 0) {
            //改变探测频度继续尝试
            metric_status->is_normal = 1;
            metric_status->check_count = 0;
        }
        break;
    }
    case  STATUS_HC:
    {
        //如果上次也是HC状态那么已经报过警了，就歇一会吧
        if(prev_status == STATUS_HC) {
            //break;
        }

        if(prev_status == STATUS_HW) {
            //撤销上次的HW报警，然后报HC
            //report_alarm(ip, metric_name, 1, NULL, NULL);
        }

        if(prev_status == STATUS_SW || prev_status == STATUS_SC) {
            //改变探测频度继续尝试
            metric_status->is_normal = 0;
            metric_status->check_count = 0;
        }

        metric_alarm_info_t ma;
        char val_str[MAX_STR_LEN] = {'\0'};
        char t_val_str[MAX_STR_LEN] = {'\0'};
        snprintf(t_val_str, sizeof(t_val_str) - 1, "%0.2f", cur_val);
        snprintf(val_str, sizeof(val_str) - 1, "%0.2f", alarm_info->critical_val);

        ma.cur_val = t_val_str;
        ma.op = alarm_info->op;
        ma.threshold_val = val_str;
        report_alarm(ip, metric_name, 0, "critical", &ma);
        break;
    }
    default:
        break;
    }

    return 0;
}

/** 
 * @brief 根据前一个metric的状态和当前metric的警报状态产生新的状态
 * @param   old_status   metric的前一个状态(这是一个值-参数型的参数)
 * @param   cur_state    当前metric的警报状态
 * @param   flag         标志位表示探测次数是否达到上限
 * @return  void 
 */
void c_data_processer::status_change(status_t *old_status, const state_t cur_state, bool flag)
{
    static status_change_t status_table[]=
    {
        {STATUS_OK, STATE_O, STATUS_OK, false},
        {STATUS_OK, STATE_O, STATUS_OK, true},

        {STATUS_OK, STATE_W, STATUS_SW, false},
        {STATUS_OK, STATE_W, STATUS_SW, true},

        {STATUS_OK, STATE_C, STATUS_SC, false},
        {STATUS_OK, STATE_C, STATUS_SC, true},

        {STATUS_OK, STATE_U, STATUS_SC, false},//新增unknown状态
        {STATUS_OK, STATE_U, STATUS_SC, true},//新增unknown状态

        {STATUS_SW, STATE_O, STATUS_OK, false},
        {STATUS_SW, STATE_O, STATUS_OK, true},

        //这里有两种转换状态(第一种当cur_atc<max_atc发生,第二种当cur_atc=max_atc发生)
        {STATUS_SW, STATE_W, STATUS_SW, false},
        {STATUS_SW, STATE_W, STATUS_HW, true},

        //这里有两种转换状态(第一种当cur_atc<max_atc发生,第二种当cur_atc=max_atc发生)
        {STATUS_SW, STATE_C, STATUS_SC, false},
        {STATUS_SW, STATE_C, STATUS_HC, true},

        {STATUS_SW, STATE_U, STATUS_SC, false},//新增unknown状态
        {STATUS_SW, STATE_U, STATUS_HC, true}, //新增unknown状态

        {STATUS_SC, STATE_O, STATUS_OK, false},
        {STATUS_SC, STATE_O, STATUS_OK, true},

        //这里有两种转换状态(第一种当cur_atc<max_atc发生,第二种当cur_atc=max_atc发生)
        {STATUS_SC, STATE_W, STATUS_SW, false},
        {STATUS_SC, STATE_W, STATUS_HC, true},
        //这里有两种转换状态(第一种当cur_atc<max_atc发生,第二种当cur_atc=max_atc发生)
        {STATUS_SC, STATE_C, STATUS_SC, false},
        {STATUS_SC, STATE_C, STATUS_HC, true},

        {STATUS_SC, STATE_U, STATUS_SC, false},//新增unknown状态
        {STATUS_SC, STATE_U, STATUS_HC, true}, //新增unknown状态

        {STATUS_HW, STATE_O, STATUS_OK, false},
        {STATUS_HW, STATE_O, STATUS_OK, true},

        {STATUS_HW, STATE_W, STATUS_HW, false},
        {STATUS_HW, STATE_W, STATUS_HW, true},

        {STATUS_HW, STATE_C, STATUS_HC, false},
        {STATUS_HW, STATE_C, STATUS_HC, true},

        {STATUS_HW, STATE_U, STATUS_HC, false},//新增unknown状态
        {STATUS_HW, STATE_U, STATUS_HC, true},//新增unknown状态

        {STATUS_HC, STATE_O, STATUS_OK, false},
        {STATUS_HC, STATE_O, STATUS_OK, true},

        {STATUS_HC, STATE_W, STATUS_HW, false},
        {STATUS_HC, STATE_W, STATUS_HW, true},

        {STATUS_HC, STATE_C, STATUS_HC, false},
        {STATUS_HC, STATE_C, STATUS_HC, true},

        {STATUS_HC, STATE_U, STATUS_HC, false},//新增unknown状态
        {STATUS_HC, STATE_U, STATUS_HC, true}//新增unknown状态
    };

    for(unsigned int i = 0; i < sizeof(status_table) / sizeof(status_table[0]); i++)
    {
        if(*old_status == status_table[i].old_status && 
                cur_state == status_table[i].cur_state && 
                flag == status_table[i].flag)
        {
            *old_status = status_table[i].ret_status;
            break;
        }
    }
    return;
}

/** 
 * @brief   取得当前的警报状态 
 * @param   wrn_val    warning value for a metric
 * @param   crtcl_val  critical value for a metric
 * @param   cur_val    current value for of metric
 * @param   op         操作方式(=,<,>,>=,<=)
 * @return  state_t 
 */
state_t c_data_processer::get_state(const double wrn_val, const double crtcl_val, const double cur_val, op_t op)
{
    state_t   ret;

    //如果当前值为-1则是Unknown状态
    if(cur_val > -1.00000000001 && cur_val < -0.99999999999)
    {
        return STATE_U;
    }

    switch(op)
    {
        case OP_EQ:
            {
                if(cur_val > wrn_val - 0.0000000001 && cur_val < wrn_val + 0.0000000001 )
                {
                    ret = STATE_W; 
                }
                else if(cur_val > crtcl_val - 0.0000000001 && cur_val < crtcl_val + 0.0000000001 )
                {
                    ret = STATE_C; 
                }
                else
                {
                    ret = STATE_O;
                }
                break;
            }
        case OP_GT:
            {
                if(cur_val > wrn_val && cur_val <= crtcl_val)
                {
                    ret = STATE_W; 
                }
                else if(cur_val > crtcl_val)
                {
                    ret = STATE_C; 
                }
                else
                {
                    ret = STATE_O;
                }
                break;
            }
        case OP_LT:
            {
                if(cur_val < crtcl_val)
                {
                    ret = STATE_C; 
                }
                else if(cur_val >= crtcl_val && cur_val < wrn_val)
                {
                    ret = STATE_W; 
                }
                else
                {
                    ret = STATE_O;
                }
                break;
            }
        case OP_GE:
            {
                if(cur_val >= wrn_val && cur_val < crtcl_val)
                {
                    ret = STATE_W; 
                }
                else if(cur_val >= crtcl_val)
                {
                    ret = STATE_C; 
                }
                else
                {
                    ret = STATE_O;
                }
                break;
            }
        case OP_LE:
            {
                if(cur_val <= crtcl_val)
                {
                    ret = STATE_C; 
                }
                else if(cur_val > crtcl_val && cur_val <= wrn_val)
                {
                    ret = STATE_W; 
                }
                else
                {
                    ret = STATE_O;
                }
                break;
            }
        default :
            {
                ret = STATE_U;
                break;
            }
    }
    return ret;
}

/** 
 * @brief   请求服务器报警 
 * @param   switch_ip   switch ip
 * @param   metricname  metric name
 * @param   operation   操作(0=报警1=撤销报警)
 * @param   alarm_level 报警级别(warning or critical)
 * @param   alarm_info  metric_alarm_info_t
 * @return  void 
 */
void c_data_processer::report_alarm(const char *switch_ip, const char *metricname, const int operation,
 const char *alarm_level, const metric_alarm_info_t *alarm_info)
{
    if(switch_ip == NULL || metricname == NULL || (operation != 1 && operation != 0)) {
        return;
    }

    if(operation == 0 && (alarm_level == NULL || alarm_info == NULL)) {
        return;
    }

    char  post_data[MAX_STR_LEN] = {'\0'};
    unsigned short data_len = 0;
    int ret = 0;
    //撤销报警
    if(operation == 1) {
        snprintf(post_data + sizeof(uint16_t), sizeof(post_data) - sizeof(uint16_t) - 1, 
                "cmd=20008&switch_info=%s&metric_info=%s&is_mute=0", switch_ip, metricname);
    } else {
        snprintf(post_data + sizeof(uint16_t), sizeof(post_data) - sizeof(uint16_t) - 1, 
                "cmd=20007&time=%lu&switch_info=%s&metric_info=%s;%s;%s;%s&alarm_info=%s",
                time(0), switch_ip, metricname, alarm_info->cur_val, alarm_info->threshold_val,
                op_to_cstr(alarm_info->op), alarm_level);
    }
    data_len = strlen(post_data + sizeof(uint16_t)) + sizeof(uint16_t);
    memcpy(post_data, &data_len, sizeof(data_len));
    ret = m_p_queue->push_data(post_data, data_len, 1);
    DEBUG_LOG("push data ret:[%d] error str:[%s]", ret, m_p_queue->get_last_errstr());

    return;
}
