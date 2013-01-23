/**
 * @file continuous_buffer.cpp
 * @brief 连续型的buffer类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-22
 */
#include <errno.h>

#include "continuous_buffer.h"
#include "inet_address.h"
#include "config.h"

continuous_buffer::continuous_buffer(const continuous_buffer& rhs)
{
    m_alloc_buf_size_ = rhs.m_alloc_buf_size_;

    m_rd_index_ = rhs.m_rd_index_;
    m_wr_index_ = rhs.m_wr_index_;

    if (rhs.m_p_buffer_) {
        char* tmp = (char*) ::malloc(m_alloc_buf_size_);
        if (tmp) {
            ::memcpy(tmp, rhs.m_p_buffer_, m_alloc_buf_size_);
        }
        if (m_p_buffer_) {
            ::free(m_p_buffer_);
            m_p_buffer_ = NULL;
        }
        m_p_buffer_ = tmp;
    } else {
        if (m_p_buffer_) {
            ::free(m_p_buffer_);
            m_p_buffer_ = NULL;
        }
    }

}

continuous_buffer& continuous_buffer::operator = (const continuous_buffer& rhs)
{
    m_alloc_buf_size_ = rhs.m_alloc_buf_size_;

    m_rd_index_ = rhs.m_rd_index_;
    m_wr_index_ = rhs.m_wr_index_;

    if (rhs.m_p_buffer_) {
        char* tmp = (char*) ::malloc(m_alloc_buf_size_);
        if (tmp) {
            ::memcpy(tmp, rhs.m_p_buffer_, m_alloc_buf_size_);
        }
        if (m_p_buffer_) {
            ::free(m_p_buffer_);
            m_p_buffer_ = NULL;
        }
        m_p_buffer_ = tmp;
    } else {
        if (m_p_buffer_) {
            ::free(m_p_buffer_);
            m_p_buffer_ = NULL;
        }
    }

    return *this;
}

int continuous_buffer::continuous_buffer_init()
{
    if ((m_p_buffer_ = (char*) ::malloc(m_alloc_buf_size_)) == NULL) {
        ERROR_LOG("malloc failed, err: %s", ::strerror(errno));
        m_p_buffer_ = NULL;
        return -1;
    }

    m_rd_index_ = m_wr_index_ = 0;

    return 0;
}

int continuous_buffer::continuous_buffer_fini()
{
    if (m_p_buffer_) {
        ::free(m_p_buffer_);
        m_p_buffer_ = NULL;
    }

    m_rd_index_ = m_wr_index_ = 0;
    m_alloc_buf_size_ = 0;

    return 0;
}

