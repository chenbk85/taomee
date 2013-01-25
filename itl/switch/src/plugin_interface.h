/** 
 * ========================================================================
 * @file plugin_interface.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-04
 * Modify $Date: 2012-07-12 18:37:53 +0800 (Thu, 12 Jul 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_PLUGIN_INTERFACE_H
#define H_PLUGIN_INTERFACE_H

#include "async_server.h"


extern "C"
{

    /** 
     * @brief 初始化main/work/conn进程
     * 启动服务程序后，主进程、工作进程和网络进程都会调用
     * 非必须实现
     * 
     * @param type 
     * PROC_MAIN 主进程
     * PROC_WORK 工作进程
     * PROC_CONN 网络进程
     * 
     * @return 0成功，-1失败
     */
    int plugin_init(int type);


    /** 
     * @brief 反初始化main/work/conn进程
     * 停止服务程序时，网络进程、工作进程和主进程都会调用
     * 非必须实现
     * 
     * @param type 同plugin_ini
     * 
     * @return 0成功，-1失败
     */
    int plugin_fini(int type);


    /** 
     * @brief 定时器，一旦间隔超过1s，就会被调用一次
     * 只会在工作进程中调用
     * 非必须实现
     */
    void time_event();


    /** 
     * @brief 判断来自客户端报文的长度
     * 必须实现
     * 
     * @param buf 报文
     * @param len 表示buf中数据长度
     * 
     * @return 判断成功则返回报文的长度，-1判断失败，0继续接收数据
     */
    int get_pkg_len_cli(const char * buf, uint32_t len);


    /** 
     * @brief 判断来自服务端报文的长度
     * 必须实现
     * 
     * @param buf 报文
     * @param len 表示buf中数据长度
     * 
     * @return 判断成功则返回报文的长度，-1判断失败，0继续接收数据
     */
    int get_pkg_len_ser(const char * buf, uint32_t len);

    
    /** 
     * @brief 检查连接上来的客户端的ip和port
     * 非必须实现
     * 
     * @param ip 客户算的ip
     * @param port 客户端的port
     * 
     * @return -1则关闭该连接，其他不做处理
     */
    int check_open_cli(uint32_t ip, uint16_t port);


    /** 
     * @brief 给客户端发过来的报文选择对应的工作进程
     * 非必须实现
     * 
     * @param fd 客户端fd
     * @param buf 报文
     * @param len 报文长度，此时已经由get_pkg_len_cli得到了报文的长度
     * @param ip 客户端ip
     * @param work_num 工作进程数量
     * 
     * @return -1由框架自动选择，[0, work_num)表示指定的工作进程，其他将产生错误日志，并丢弃报文
     */
    int select_channel(int fd, const char * buf, uint32_t len, uint32_t ip, uint32_t work_num);


    /** 
     * @brief 来自客户端的报文发给相应工作进程后调用
     * 非必须实现
     * 
     * @param fd 客户端fd
     * @param buf 报文
     * @param len 报文长度
     * @param flag 1已经成功发给工作进程，0失败
     * 
     * @return -1则断开连接
     */
    int shmq_pushed(int fd, const char * buf, uint32_t len, int flag);


    /** 
     * @brief 处理来自服务端的报文
     * 必须实现
     * 
     * @param fd 服务端连接的fd
     * @param buf 报文
     * @param len 报文长度
     * get_pkg_len_cli中保证了报文长度的正确性，这里不需要再判断
     */
    void proc_pkg_cli(int fd, const char * buf, uint32_t len);


    /** 
     * @brief 处理来自服务端的报文
     * 必须实现
     * 
     * @param fd 服务端连接的fd
     * @param buf 报文
     * @param len 报文长度
     * get_pkg_len_ser中保证了报文长度的正确性，这里不需要再判断
     */
    void proc_pkg_ser(int fd, const char * buf, uint32_t len);


    /** 
     * @brief 客户端有新的连接进来时调用
     * 非必须实现
     * 
     * @param fd 进来的新客户端fd
     */
    void link_up_cli(int fd, uint32_t ip);
    

    /** 
     * @brief 客户端连接断开时调用
     * 非必须实现
     * 
     * @param fd 断开的客户端fd
     */
    void link_down_cli(int fd);


    /** 
     * @brief 服务端连接断开时调用
     * 非必须实现
     * 
     * @param fd 断开的服务端fd
     */
    void link_down_ser(int fd);

}


#endif
