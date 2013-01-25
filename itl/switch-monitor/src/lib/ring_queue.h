/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file ring_queue.h
 * @author xml <xml@taomee.com>
 * @author richard <richard@taomee.com>
 * @date 2010-03-23
 */

#ifndef RING_QUEUE_H_2010_03_23
#define RING_QUEUE_H_2010_03_23

#include "i_ring_queue.h"

/**
 * ring queue implementation for binary data
 */
class c_ring_queue : public i_ring_queue
{
    public:
        c_ring_queue();
        virtual ~c_ring_queue();
        virtual int init(int buffer_len);
        virtual int push_data(const char *p_data,int data_len,int is_atomic);
        virtual int pop_data(char *p_recv_buffer, int buffer_len, int timeout = 0);
        virtual int pop_data_dummy(char *p_recv_buffer, int buffer_len, int timeout = 0);
        virtual int get_buffer_len();
        virtual int get_data_len();
        virtual int get_empty_buffer_len();
        virtual int get_last_errno();
        virtual const char * get_last_errstr();
        virtual int uninit();
        virtual int release();

    protected:
        typedef struct {
            char* p_buffer;                          /**< buffer to simulate ring queue */
            int buffer_len;                          /**< length of whole buffer */
            int read_index;                          /**< read index,start from zero */
            int write_index;                         /**< write index,start from zero */
        } ring_queue_meta_t;

        int m_inited;                                /**< module status */
        int m_last_errno;                            /**< last error code */
        ring_queue_meta_t *m_p_meta;
};

#endif //RING_QUEUE_H_2010_03_23

