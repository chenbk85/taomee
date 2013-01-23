/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file i_event.h
 * @author richard <richard@taomee.com>
 * @date 2011-06-13
 */

#ifndef I_EVENT_H_2011_06_13
#define I_EVENT_H_2011_06_13

class i_event
{
public:   
    typedef enum {
        EVENT_READ  = 1 << 0,
        EVENT_WRITE = 1 << 1,
        EVENT_RDWT  = EVENT_READ | EVENT_WRITE,
        EVENT_ERROR = 1 << 2
    } event_type_t;
    struct event_item_t;
    typedef void (*event_proc_t)(int fd, event_type_t event_type, void *p_user_data);

    virtual int init() = 0;
    
    virtual event_item_t * add(int fd, event_type_t event_type, 
                               event_proc_t event_proc, void *p_user_data) = 0;

    virtual int del(event_item_t *p_event_item) = 0;
    
    virtual int ctl(event_item_t *p_event_item, event_type_t event_type) = 0;

    virtual int check(int timeout) = 0;

    virtual int uninit() = 0;

    virtual int release() = 0;
};

i_event * create_event_instance();

#endif /* I_EVENT_H_2011_06_13 */
