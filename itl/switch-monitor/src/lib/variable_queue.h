/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file variable_queue.h
 * @author xml <xml@taomee.com>
 * @date 2010-03-22
 */

#ifndef ROUTED_QUEUE_H_2010_03_22
#define ROUTED_QUEUE_H_2010_03_22

#include "ring_queue.h"

class c_variable_queue : public c_ring_queue
{
    public:
        c_variable_queue(int len);
        virtual ~c_variable_queue();
        virtual int init(int buffer_len);
        virtual int push_data(const char *p_data, int data_len, int is_atomic);
        virtual int pop_data(char *p_recv_buffer, int buffer_len, int timeout = 0);
        virtual int pop_data_dummy(char *p_recv_buffer, int buffer_len, int timeout = 0);

    private:
        int m_len;
};

#endif //ROUTED_QUEUE_H_2010_03_22

