/**
 * =====================================================================================
 *       @file  data_processer.cpp
 *      @brief  
 *
 *  request the xml data from data source.then parse them ,and save them into hash tree
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
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "data_processer.h"


/**<@保存接收的xml数据*/
char  buf[MAX_BUF_SIZE] = {0};


/** 
 * @brief  构造函数
 * @param   
 * @return   
 */
c_data_processer::c_data_processer()
{
    m_inited = 0;
    m_ds_conn = NULL;
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

int c_data_processer::release()
{
    delete this;
    return 0;
}
int c_data_processer::uninit()
{
    if(!m_inited)
    {
        return -1;
    }

    if(m_ds_conn != NULL)
    {
        m_ds_conn->uninit();
        m_ds_conn->release();
        m_ds_conn = NULL;
    }

    m_inited = 0;

    return 0;
}

/** 
 * @brief  初始化函数,要么init成功，要么失败会uninit已经init成功的变量
 * @return   0 success -1 failed
 */
int c_data_processer::init()
{
    if(m_inited)
    {
        return -1;
    }

    if(0 != create_net_client_instance(&m_ds_conn))
    {
        return -1;
    }
    m_inited = 1;

    return 0;
}

/** 
 * @brief  线程主函数
 * @param   p_data  用户数据
 * @return   NULL success UNNULL failed
 */
void* c_data_processer::data_processer_main(const int server_ip, const int server_port, 
const char *xml_path, unsigned int cmd_id)
{
    c_net_client_impl    *ds_conn = m_ds_conn;

    if(0 !=  ds_conn->init(server_ip, server_port, 10))
    {
        printf("net connect failed\n");
        return NULL;
    }

    int   recv_byte  = 0;
    int   recv_sum   = 0;
    char *write_pos  = buf;
    int   remain_len = MAX_BUF_SIZE;

    while(true)
    {
        int           flag = 0;
        oa_cmd_t      cmd;
        cmd.msg_len = sizeof(cmd);
        cmd.version = 1;
        cmd.msg_id  = cmd_id;

        if(xml_path != NULL)
        {
            cmd.msg_len += strlen(xml_path);
            flag = 1;
        }

        ds_conn->send_data((char*)&cmd, sizeof(cmd));

        if(flag)
            ds_conn->send_data((char*)xml_path, strlen(xml_path));

        if(ds_conn->do_io() < 0)
        {
            printf("do_io() error,net connect error.\n");
            goto error_exit;
        }
        recv_byte = ds_conn->recv_data(write_pos, remain_len);
        while(recv_byte >= 0)
        {
            write_pos += recv_byte;
            remain_len -= recv_byte;
            recv_sum += recv_byte;
            int ret = ds_conn->do_io();
            if(ret == -2 )
            {	
                goto done;
            }
            else if(ret == -1)
            {
                printf("do_io() error,net connect error.\n");
                goto error_exit;
            }
            recv_byte = ds_conn->recv_data(write_pos, remain_len);
        }
        if(recv_byte < 0)
        {
            printf("xml buffer has no enough space\n");
            goto error_exit;
        }
    }
done:
    buf[recv_sum] = '\0';
    printf(buf);

error_exit: 
    ds_conn->uninit();
    return NULL;
}

