/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file event.h
 * @author richard <richard@taomee.com>
 * @date 2011-06-13
 */

#ifndef EVENT_H_2011_06_13
#define EVENT_H_2011_06_13

#include "list.h"
#include "i_event.h"

class c_event : public i_event
{
public:
    virtual int init();
    
    virtual event_item_t * add(int fd, event_type_t event_type, 
                               event_proc_t event_proc, void *p_user_data);

    virtual int del(event_item_t *p_event_item);
    
    virtual int ctl(event_item_t *p_event_item, event_type_t event_type);

    virtual int check(int timeout);

    virtual int uninit();

    virtual int release();

private:
    typedef struct {
        int fd;
        event_type_t event_type;
        event_proc_t event_proc;
        void *p_user_data;
        bool deleted;
        struct list_head list;
    } event_item_t;

    bool m_inited;
    struct list_head m_event_list;
};

#endif /* EVENT_H_2011_06_13 */
