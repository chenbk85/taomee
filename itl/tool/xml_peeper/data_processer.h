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

#include <stdint.h>
#include "./net_client_impl.h"

const unsigned int MAX_BUF_SIZE = 8 * 1024 * 1024;

/*
 * @struct 上层head发送的命令的格式
 */
typedef struct 
{
    uint32_t msg_len;
    uint32_t version;
    uint32_t msg_id;
    //unsigned long msg_len;
    //unsigned long version;
    //unsigned long msg_id;
    char     cmd[0];
}__attribute__((__packed__)) oa_cmd_t;

#define GET_SUMMARY_INFO 1000
#define GET_GRID_INFO 1001
#define GET_CLUSTER_INFO 1002
#define GET_HOST_INFO 1003


class c_data_processer
{
    public :
        c_data_processer();
        ~c_data_processer();
        int init();
        int uninit();
        int release();
        /** 
         * @brief  线程主函数
         * @param   p_data  用户数据
         * @return   NULL success UNNULL failed
         */
        void* data_processer_main(const int server_ip, const int server_port, 
const char *xml_path, unsigned int cmd_id);
    private:
        int                  m_inited;     /**<@是否初始化标志*/
        c_net_client_impl   *m_ds_conn;    /**<@和本数据源建立的连接*/
};

#endif
