/**
 * @file vpipe.h
 * @brief 类管道类型的fd封装类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-19
 */

#ifndef _H_VPIPE_H_
#define _H_VPIPE_H_

#include <boost/noncopyable.hpp>

#include "ipc_traits.hpp"

#define INVALID_VPIPE_HANDLER -1

class vpipe : boost::noncopyable {
public:
    /**<@定义vpipe标签*/
    typedef vpipe_tag ipc_category;

    /**
     * @brief 设置fd为非阻塞fd
     * @return -1failed, 0success
     */
    int set_nonblocking();

    /**
     * @brief 关闭连接两端的fd
     * @return NULL
     */
    void close();

    /**
     * @brief 关闭连接端1的fd
     * @return NULL
     */
    void close_fd1();

    /**
     * @brief 关闭连接端2的fd
     * @return NULL
     */
    void close_fd2();

    /**
     * @brief 获得连接两端的fd并填入数组
     * @param fd    放置fd的数组
     * @return NULL
     */
    void get_fd_all(int fd[2]) {
        fd[0] = m_vpipe_fd_[0];
        fd[1] = m_vpipe_fd_[1];
    }

    /**
     * @brief 获取连接端1的fd
     * @return 连接端1的fd
     */
    const int get_fd1() const { return m_vpipe_fd_[0]; }

    /**
     * @brief 获取连接端2的fd
     * @return 连接端2的fd
     */
    const int get_fd2() const { return m_vpipe_fd_[1]; }

    /**
     * @brief 设置连接两端的fd
     * @param fd    fd数组
     * @return NULL
     */
    void set_fd(int fd[2]) {
        m_vpipe_fd_[0] = fd[0];
        m_vpipe_fd_[1] = fd[1];
    }

    /**
     * @brief 设置连接端1的fd
     * @return NULL
     */
    void set_fd1(int fd) { m_vpipe_fd_[0] = fd; }

    /**
     * @brief 设置连接端2的fd
     * @return NULL
     */
    void set_fd2(int fd) { m_vpipe_fd_[1] = fd; }

protected:
    vpipe() { m_vpipe_fd_[0] = m_vpipe_fd_[1] = -1; }
    ~vpipe();

private:
    /**<@vpipe fd*/
    int m_vpipe_fd_[2];
};

#endif
