/**
 * @file continuous_buffer.h
 * @brief 连续型的buffer类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-22
 */

#ifndef _H_CONTINUOUS_BUFFER_H_
#define _H_CONTINUOUS_BUFFER_H_

#include <string.h>
#include <stdlib.h>

#include "log.h"


/**
 * @brief 连续型的缓冲区
 */
class continuous_buffer {
public:
    /**
     * @brief 缓冲类构造函数
     */
    continuous_buffer()
        : m_alloc_buf_size_(1024),
          m_p_buffer_(NULL),
          m_rd_index_(0),
          m_wr_index_(0)
    {

    }

    /**
     * @brief 缓冲类析构函数
     */
    ~continuous_buffer() {
        if (m_p_buffer_) {
            continuous_buffer_fini();
        }
    }

    /**
     * @brief 缓冲类拷贝构造函数
     */
    continuous_buffer(const continuous_buffer& rhs);

    /**
     * @brief 缓冲类拷贝析构函数
     */
    continuous_buffer& operator = (const continuous_buffer& rhs);

    /**
     * @brief 缓冲类初始化函数
     * @return -1failed, 0success
     */
    int continuous_buffer_init();

    /**
     * @brief 缓冲类反初始化函数
     * @return -1failed, 0success
     */
    int continuous_buffer_fini();

    /**
     * @brief 获得可读区域长度
     */
    size_t readable_bytes() const {
        return m_wr_index_ - m_rd_index_;
    }

    /**
     * @brief 获得调整前可写区域的长度
     */
    size_t writeable_actual_bytes() const {
        return m_alloc_buf_size_ - m_wr_index_;
    }

    /**
     * @brief 获得可写区域长度，可写区域长度在不够的情况下会调整或再分配
     */
    size_t writeable_bytes() {
        make_space();
        append_buffer();
        return m_alloc_buf_size_ - m_wr_index_;
    }

    /**
     * @brief 获得可写区域首指针，可写区域指针在不够的情况下会调整或再分配
     */
    void* writeable_addr() {
        make_space();
        append_buffer();
        return (void*)(m_p_buffer_ + m_wr_index_);
    }

    /**
     * @brief 获得可读区域首指针
     */
    void* readable_addr() const {
        return (void*)(m_p_buffer_ + m_rd_index_);
    }

    /**
     * @brief 移动读指针
     */
    void move_readable(int len) {
        m_rd_index_ += len;
    }

    /**
     * @brief 移动写指针
     */
    void move_writeable(int len) {
        m_wr_index_ += len;
    }

    /**
     * @brief 调整或分配新区域，使得可写区域满足给定长度
     * @param len  所要求的给定长度
     * @return -1failed, 0success
     */
    int make_writeable_bytes(int len) {
        if (m_rd_index_ == m_wr_index_) {
            m_rd_index_ = m_wr_index_ = 0;
        } else if (m_rd_index_ < m_wr_index_) {
            ::memmove(m_p_buffer_, m_p_buffer_ + m_rd_index_, m_wr_index_ - m_rd_index_);
            m_wr_index_ -= m_rd_index_;
            m_rd_index_ = 0;
        }

        if ((int)writeable_actual_bytes() >= len) {
            return 0;
        }

        int n = (len - writeable_actual_bytes()) / m_alloc_buf_size_ + 1;
        m_alloc_buf_size_ *= (n * 2);

        m_p_buffer_ = (char*) ::realloc(m_p_buffer_, m_alloc_buf_size_);
        if (!m_p_buffer_) {
            m_alloc_buf_size_ /= (n * 2);
            ERROR_LOG("mem not enough to realloc");
            return -1;
        }

        return 0;
    }

    /**
     * @brief 缩小缓冲区
     */
    void resize() {
        m_alloc_buf_size_ = 1024;
        m_wr_index_ = m_rd_index_ = 0;
        m_p_buffer_ = (char*) ::realloc(m_p_buffer_, m_alloc_buf_size_);
    }

    /**
     * @brief 重置缓冲区
     */
    void reset() {
        m_wr_index_ = m_rd_index_ = 0;
    }

private:

    /**
     * @brief 调整缓冲区指针使获得更多写空间
     */
    void make_space() {
        if (m_rd_index_ >= m_alloc_buf_size_*3/4) {
            if (m_rd_index_ == m_wr_index_) {
                m_rd_index_ = m_wr_index_ = 0;
            } else if (m_rd_index_ < m_wr_index_) {
                ::memmove(m_p_buffer_, m_p_buffer_ + m_rd_index_, m_wr_index_ - m_rd_index_);
                m_wr_index_ -= m_rd_index_;
                m_rd_index_ = 0;
            }
        }
    }

    /**
     * @brief 扩大缓冲区大小使获得更多写空间
     */
    void append_buffer() {
        if (m_wr_index_ == m_alloc_buf_size_) {
            m_alloc_buf_size_ *= 2;
            m_p_buffer_ = (char*) ::realloc(m_p_buffer_, m_alloc_buf_size_);
            if (!m_p_buffer_) {
                m_alloc_buf_size_ /= 2;
                ERROR_LOG("mem not enough to realloc");
            }
        }
    }

private:
    /**<@当前所分配的缓冲区大小*/
    size_t m_alloc_buf_size_;

    /**<@缓冲区指针*/
    char* m_p_buffer_;

    /**<@缓冲区读索引*/
    size_t m_rd_index_;

    /**<@缓冲区写索引*/
    size_t m_wr_index_;
};
#endif
