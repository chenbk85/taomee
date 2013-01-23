/**
 * @file sock_dgram.h
 * @brief udp socket类封装
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-09
 */

#ifndef _H_SOCK_DGRAM_H_
#define _H_SOCK_DGRAM_H_

#include "inet_address.h"
#include "sock.h"

class inet_address;

class sock_dgram : public sock {
public:
    typedef sock_dgram_tag ipc_category;

    sock_dgram() { }
    sock_dgram(const inet_address& local_addr) : m_local_addr_(local_addr) { }
    ~sock_dgram() { }

    /**
     * @brief udp接收器打开被动监听
     * @return -1failed 0success
     */
    int open_listening();

    /**
     * @brief udp连接器创建主动套接字
     * @return -1failed, 0success
     */
    int open_connecting();

    /**
     * @brief udp向对端发送数据
     * @param remote_addr  对端地址
     * @param buf          所要发送的数据
     * @param len          所要发送数据的长度
     * @param flags        设置fd的标志位
     * @return -1failed  >=0所发送数据的长度
     */
    ssize_t write(const inet_address& remote_addr, void* buf, size_t len, int flags = 0);

    /**
     * @brief udp从对端接收数据
     * @param remote_addr   所收到数据的对端地址
     * @param buf           所要接收数据的缓冲区
     * @param len           所要接收数据的缓冲区长度
     * @param flags         设置fd的标志位
     * @return -1failed, >=0所接收数据的长度
     */
    ssize_t read(inet_address& remote_addr, void* buf, size_t len, int flags = 0);

    /**
     * @brief udp向对端scatter发送数据
     * @param remote_addr  对端地址
     * @param iov          所发送数据的iov数组
     * @param iov_cnt      所发送iov数组的长度
     * @param flags        设置fd的标志位
     * @return -1failed, >=0所发送数据的长度
     */
    ssize_t writev(const inet_address& remote_addr, struct iovec* iov, int iov_cnt, int flags = 0);

    /**
     * @brief udp从对端gather接收数据
     * @param remote_addr  所接收数据的对端地址
     * @param iov          所接收数据的iov数组
     * @param iov_cnt      iov数组的长度
     * @param flags        设置fd的标志位
     * @return -1failed, >=0所接收数据的长度
     */
    ssize_t readv(inet_address& remote_addr, struct iovec* iov, int iov_cnt, int flags = 0);

    /**
     * @brief 设置本地连接地址
     * @param local_addr  本地连接地址
     * @return NULL
     */
    void set_local_addr(const inet_address& local_addr) { m_local_addr_ = local_addr; }

private:
    inet_address m_local_addr_;
};

#endif

