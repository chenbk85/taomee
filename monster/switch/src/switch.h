/*
 * =====================================================================================
 *
 *       Filename:  switch.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月11日 15时45分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_SWITCH_H_20110711
#define H_SWITCH_H_20110711

#include <async_serv/dll.h>
#include <async_serv/net_if.h>

/**
 * @brief init_service 异步框架要求实现的接口之一
 *
 * @param isparent
 *
 * @return
 */
extern "C"  int init_service(int isparent);

/**
 * @brief fini_service 异步框架要求实现的接口之一
 *
 * @param isparent
 *
 * @return
 */
extern "C" int fini_service(int isparent);

/**
 * @brief proc_events 异步框架要求实现的接口之一
 */
extern "C" void proc_events();

/**
 * @brief get_pkg_len 异步框架要求实现的接口之一
 *
 * @param fd
 * @param pkg
 * @param pkglen
 * @param isparent
 *
 * @return
 */
extern "C" int get_pkg_len(int fd, const void* pkg, int pkglen, int isparent);

/**
 * @brief proc_pkg_from_client 异步框架要求实现的接口之一
 *
 * @param data
 * @param len
 * @param fdsess
 *
 * @return
 */
extern "C" int proc_pkg_from_client(void *data, int len, fdsession_t *fdsess);

/**
 * @brief proc_pkg_from_serv 异步框架要求实现的接口之一
 *
 * @param fd
 * @param data
 * @param len
 *
 * @return
 */
extern "C" void proc_pkg_from_serv(int fd, void *data, int len);

/**
 * @brief on_client_conn_closed 异步框架要求实现的接口之一
 *
 * @param fd
 */
extern "C" void on_client_conn_closed(int fd);

/**
 * @brief on_fd_closed 异步框架要求实现的接口之一
 *
 * @param fd
 */
extern "C" void on_fd_closed(int fd);


/**
 * @brief dispatch 应用so自己实现的函数，分发协议包，调用不同的处理函数
 *
 * @param p_data    接收到数据包
 * @param len       接收到的数据包的长度
 * @param fdsess    对断连接信息
 *
 * @return 0:success -1:failed
 */
int dispatch(void *p_data, int len, fdsession_t *fdsess);

#endif //H_SWITCH_H_20110711
