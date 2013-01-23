/**
 * @file vpipe_sockpair.h
 * @brief 类管道模型socketpair封装
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-19
 */
#ifndef _H_VPIPE_SOCKPAIR_H_
#define _H_VPIPE_SOCKPAIR_H_

#include "unistd.h"

#include "vpipe.h"

#define SOCKPAIR_TYPE_1   1
#define SOCKPAIR_TYPE_2   2

class vpipe_sockpair : public vpipe {
public:
    /**<@定义vpipe_sockpair标签*/
    typedef vpipe_sockpair_tag ipc_category;

    /**
     * @brief vpipe_sockpair构造函数
     */
    vpipe_sockpair()
        : m_sockpair_type_(0)
    {

    }
    /**
     * @brief vpipe_sockpair析构函数
     */
    ~vpipe_sockpair() { m_sockpair_type_ = 0; }

    /**
     * @brief 打开流管道
     * @return -1failed, 0success
     */
    int open();

    /**
     * @brief socketpair向对端发送数据
     * @param buf    所要发送的数据
     * @param len    所要发送数据的长度
     * @param flags  设置fd的属性
     * @return -1failed, 0对端关闭连接, >=0所发送数据的长度
     */
    ssize_t write(void* buf, size_t len, int flags = 0);

    /**
     * @brief socketpair从对端接收数据
     * @param buf   所要接收数据的缓冲区
     * @param len   所要接收数据的缓冲区长度
     * @param flags 设置fd的属性
     * @return -1failed, >=0所接收数据的长度
     */
    ssize_t read(void* buf, size_t len, int flags = 0);

    /**
     * @brief socketpair向对端scatter发送数据
     * @param iov          所发送数据的iov数组
     * @param iov_cnt      所发送iov数组的长度
     * @param flags        设置fd的标志位
     * @return -1failed, >=0所发送数据的长度
     */
    ssize_t writev(struct iovec* iov, int iov_cnt, int flags = 0);

    /**
     * @brief socketpair从对端gather接收数据
     * @param iov          所接收数据的iov数组
     * @param iov_cnt      iov数组的长度
     * @param flags        设置fd的标志位
     * @return -1failed, 0对端关闭连接, >=0所接收数据的长度
     */
    ssize_t readv(struct iovec* iov, int iov_cnt, int flags = 0);

    /**
     * @brief 设置soketpair的连接端，由调用者决定是连接端1还是2
     */
    void set_sockpair_type(int sockpair_type) {
        m_sockpair_type_ = sockpair_type;
    }

    const int get_fd() const {
        if (m_sockpair_type_ == SOCKPAIR_TYPE_1) {
            return vpipe::get_fd1();
        } else {
            return vpipe::get_fd2();
        }
    }

    /**
     * @brief 半关闭写连接
     * @return -1failed, 0success
     */
    int close_writer();

    /**
     * @brief 半关闭读连接
     * @return -1failed, 0success
     */
    int close_reader();

private:
    /**<@sockpair的类型，用来区别sockpair是连接端1还是连接端2*/
    int m_sockpair_type_;
};

#endif
