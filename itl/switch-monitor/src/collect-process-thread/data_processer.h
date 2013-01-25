/**
 * =====================================================================================
 *       @file  data_processer.h
 *      @brief  
 *
 *  request the xml data from data source.then parse them ,and save them into haash tree
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
 
 
#ifndef DATA_PROCESSER_H
#define DATA_PROCESSER_H

#include <pthread.h>
#include "../lib/i_ring_queue.h"
#include "../lib/c_mysql_iface.h"
#include "../defines.h"
#include "../proto.h"

/**
*  @struct 用于报警的传参结构
*/
typedef struct {
    const   char          *cur_val;
    op_t                   op;
    const   char          *threshold_val;
} metric_alarm_info_t;

/**
*  @struct 状态转换表结构 
*/
typedef struct {
    const status_t      old_status;
    const state_t       cur_state;
    const status_t      ret_status;
    bool                flag;
} status_change_t;

class c_data_processer
{
public :
    c_data_processer();
    ~c_data_processer();
    /** 
     * @brief  初始化函数,要么init成功，要么失败会uninit已经init成功的变量
     * @param   metric_set     metric信息结构的vector
     * @param   switch_group   switch_group结构的指针
     * @param   p_if_alarm     交换机接口对应的报警信息结构的指针
     * @param   switch_table   switch的hahs表的指针
     * @param   p_config       config对象指针
     * @param   p_queue        队列指针
     * @return   0 success -1 failed
     */
    int init(metric_info_vec_t* metric_set, metric_alarm_vec_t *default_metric_alarm_set, metric_alarm_vec_t *specified_metric_alarm_set, switch_group_info_t *switch_group, if_alarm_map_t *p_if_alarm, hash_t *switch_table, const config_var_t *p_config, i_ring_queue*  p_queue);
    int uninit();
    int release();
protected:
    /** 
     * @brief   线程主函数
     * @param   p_data  用户数据
     * @return  NULL success UNNULL failed
     */
    static void* data_processer_main(void *p_data);

    /** 
     * @brief   收集一个交换机的所有接口的信息
     * @param   switch_info      switch_info_t结构的指针
     * @param   switch_data      switch_t结构的指针
     * @return  -1 = failed 0 successed
     */
    int collect_interface_info(const switch_info_t* switch_info, switch_t *switch_data);

    /** 
     * @brief   收集一个接口的信息
     * @param   ifs              switch_interface_info_t结构数组
     * @param   ifs_count        数组长度是个值结果参数
     * @param   switch_ip        switch ip
     * @param   switch_community switch community
     * @return  -1 = failed 0 successed
     */
    int get_interface_info(switch_interface_info_t *ifs, unsigned int *ifs_count, const char *switch_ip, const char *switch_community);

    /** 
     * @brief   收集一个接口的流量
     * @param   switch_info      switch_info_t结构的指针
     * @param   switch_data      switch_t结构的指针
     * @return  -1 = failed 0 successed
     */
    int collect_interface_data(const switch_info_t *switch_info, switch_t *switch_data);

    /** 
     * @brief   收集一个交换机的接口的状态
     * @param   if_info          switch_interface_info_t结构指针
     * @param   switch_ip        switch ip
     * @param   switch_community switch community
     * @return  -1 = failed 0 successed
     */
    int get_interface_status(switch_interface_info_t *if_table, unsigned int if_num, const char *switch_ip, 
            const char *switch_community);

    /** 
     * @brief   收集一个交换机的接口的出入流量
     * @param   if_info          switch_interface_info_t结构指针
     * @param   switch_ip        switch ip
     * @param   switch_community switch community
     * @return  -1 = failed 0 successed
     */
    int get_interface_in_out(switch_interface_info_t *if_table, unsigned int if_num, const char *switch_ip, 
            const char *switch_community);

    /** 
     * @brief   收集一个交换机的所有接口的数据
     * @param   metric         metric_info_t结构指针
     * @param   switch_info    switch_info_t结构的指针
     * @param   switch_data    switch_t结构的指针
     * @return  -1 = failed 0 successed
     */
    int  collect_metric_data(const metric_info_t* metric, const switch_info_t* switch_info,
            switch_t * switch_data);

    /** 
     * @brief   执行一个外部程序并获得输出结果
     * @param   exec_file_name      可执行文件名
     * @param   arg                 可选的参数
     * @param   timeout             超时值
     * @return  -1 = failed >0 successed
     */
    double plugins_func(const char *exec_file_name, const char * arg, unsigned int timeout);

    /** 
     * @brief   处理一个非接口类型metric的报警 
     * @param   metric         metric_info_t结构指针
     * @param   metric_val     metric的当前值
     * @param   switch_info    switch_info_t结构的指针
     * @param   switch_data    switch_t结构的指针
     * @return  void 
     */
    int do_metric_alarm(const metric_info_t *metric, double metric_val, const switch_info_t *switch_info,
            switch_t *switch_data);

    /** 
     * @brief   处理一个接口的流量报警
     * @param   switch_info      switch_info_t结构的指针
     * @param   switch_data      switch_t结构的指针
     * @return  -1 = failed 0 success
     */
    int handle_interface_alarm(const switch_info_t* switch_info, switch_t *switch_data);

    /** 
     * @brief   处理一个metric的报警 
     * @param   ip             switch ip
     * @param   alarm_info     alarm_info_t结构的指针
     * @param   metric_status  metric_status_info_t结构指针
     * @param   metric_name    metric name
     * @param   cur_val        metric的当前值
     * @return  0-success -1=failed 
     */
    int handler_alarm(const char *ip, const alarm_info_t *alarm_info, 
            metric_status_info_t *metric_status, const char *metric_name, double cur_val);

    /** 
     * @brief  根据前一个metric的状态和当前metric的警报状态产生新的状态
     * @param   old_status   metric的前一个状态(这是一个值-参数型的参数)
     * @param   cur_state    当前metric的警报状态
     * @param   flag         标志位表示探测次数是否达到探测上限
     * @return  void 
     */
    void status_change(status_t *old_status, const state_t cur_state, bool flag);

    /** 
     * @brief   取得当前的警报状态 
     * @param   wrn_val    warning value for a metric
     * @param   crtcl_val  critical value for a metric
     * @param   cur_val    current value of metric
     * @param   op         操作方式(=,<,>,>=,<=)
     * @return  state_t 
     */
    state_t get_state(const double wrn_val, const double crtcl_val, const double cur_val, op_t op);

    /** 
     * @brief   请求web服务器报警 
     * @param   switch_ip   switch ip
     * @param   metricname  metric name
     * @param   operation   操作(0=报警1=撤销报警)
     * @param   alarm_level 报警级别(warning or critical)
     * @param   alarm_info  metric_alarm_info_t
     * @return  void 
     */
    void report_alarm(const char *host_ip, const char *metricname, const int operation, 
            const char *alarm_level, const metric_alarm_info_t *alarm_info);
private:
    int                    m_inited;           /**<是否初始化标志*/
    hash_t                *m_p_root;           /**<保存交换机的hash表*/
    i_ring_queue          *m_p_queue;          /**<ring_queue对象指针*/
    switch_group_info_t   *m_p_switch_group;   /**<swtich_group_info_t对象指针*/
    metric_info_vec_t     *m_p_metric_set;     /**<metric_info_t结构动态数组指针*/
    metric_alarm_vec_t    *m_p_default_metric_alarm_set;     
    metric_alarm_vec_t    *m_p_specified_metric_alarm_set;  
    if_alarm_map_t        *m_p_if_alarm_map;   /**<alarm_info_t结构动态map指针*/
    pthread_t              m_pid;              /**<线程id*/
    unsigned int           m_collect_interval; /**<收集间隔*/
    bool                   m_stop;
};

/** 
 * @brief  创建对象实例
 * @param  pp_instance 对象指针的指针 
 * @return  0 success -1 failed 
 */
int create_data_processer_instance(c_data_processer  **pp_instance);


#endif   /**<metric_info_t结构动态数组指针*/
