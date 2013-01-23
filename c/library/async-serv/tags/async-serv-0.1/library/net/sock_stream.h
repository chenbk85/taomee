/**
 * @file sock_stream.h
 * @brief sock连接类，封装基本socket操作
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-05
 */
#ifndef _H_SOCK_STREAM_H_
#define _H_SOCK_STREAM_H_

#include <unistd.h>

#include "sock.h"

class sock_stream : public sock {
public:
    typedef sock_stream_tag ipc_category;

    sock_stream() { }
    sock_stream(int sock_fd) { set_fd(sock_fd); }
    ~sock_stream() { }

    /**
     * @brief 向fd发送数据
     * @param buf   所要发送的数据buf
     * @param len   所要发送的数据buf的长度
     * @param flags 设置fd的标志，默认是0
     * @return -1发送失败; len所发送数据的大小
     */
    ssize_t write(void* buf, size_t len, int flags = 0);

    /**
     * @brief 从fd接收数据
     * @param buf   接收数据buf
     * @param len   接收数据buf的长度
     * @param flags 设置fd的标志，默认是0
     * @return -1接收失败; 0对端关闭连接; len所接收数据的大小
     */
    ssize_t read(void* buf, size_t len, int flags = 0);

    /**
     * @brief 向fd scatter发送数据
     * @param iov       所要发送的数据iov结构体
     * @param iov_cnt   所要发送的数据iov结构体个数
     * @param flags     设置fd的标志，默认是0
     * @return -1发送失败; len所发送数据的大小
     */
    ssize_t writev(struct iovec* iov, int iov_cnt, int flags = 0);

    /**
     * @brief 从fd gather接收数据
     * @param iov       接收数据iov结构体
     * @param iov_cnt   接收数据iov结构体个数
     * @param flags     设置fd的标志，默认是0
     * @return -1接收失败; -2对端关闭连接; len所接收数据的大小
     */
    ssize_t readv(struct iovec* iov, int iov_cnt, int flags = 0);

    /**
     * @brief   半关闭写连接
     * @return  -1failed, 0success
     */
    int close_writer();

    /**
     * @brief   半关闭读连接
     * @return  -1failed, 0success
     */
    int close_reader();

};

#endif

