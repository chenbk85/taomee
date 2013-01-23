/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file event.cpp
 * @author richard <richard@taomee.com>
 * @date 2011-06-13
 */

#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#include "event.h"

i_event * create_event_instance()
{
    return new (std::nothrow)c_event();
}

int c_event::init()
{
    if (m_inited) {
        return -1;
    }

    INIT_LIST_HEAD(&m_event_list);

    m_inited = true;

    return 0;
}
    
i_event::event_item_t * c_event::add(int fd, event_type_t event_type, 
                                     event_proc_t event_proc, void *p_user_data)
{
    event_item_t *p_event_item = (event_item_t *)malloc(sizeof(event_item_t));
    if (p_event_item == NULL) {
        return NULL;
    }

    p_event_item->fd = fd;
    p_event_item->event_type = event_type;
    p_event_item->event_proc = event_proc;
    p_event_item->p_user_data = p_user_data;
    p_event_item->deleted = false;

    list_add(&p_event_item->list, &m_event_list);

    return (i_event::event_item_t *)p_event_item;
}

int c_event::del(i_event::event_item_t *p_event_item)
{
    if (p_event_item == NULL) {
        return -1;
    }

    ((event_item_t *)p_event_item)->deleted = true;

    return 0;
}

int c_event::ctl(i_event::event_item_t *p_event_item, event_type_t event_type)
{
    if (p_event_item == NULL) {
        return -1;
    }

    ((event_item_t *)p_event_item)->event_type = event_type;

    return 0;
}

int c_event::check(int timeout)
{
    event_item_t *p_event_item = NULL;
    event_item_t *pTmp = NULL;

    list_for_each_entry_safe(p_event_item, pTmp, &m_event_list, list) {
        if (p_event_item->deleted) {
            list_del(&((event_item_t *)p_event_item)->list);
            free(p_event_item);
        }
    }

    fd_set readSet;
    fd_set writeSet;
    fd_set errorSet;

    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    FD_ZERO(&errorSet);

    int max_fd = 0;
    list_for_each_entry(p_event_item, &m_event_list, list) {
        if (p_event_item->event_type & EVENT_READ) {
            FD_SET(p_event_item->fd, &readSet);
        }
        if (p_event_item->event_type & EVENT_WRITE) {
            FD_SET(p_event_item->fd, &writeSet);
        }
        FD_SET(p_event_item->fd, &errorSet);
        if (max_fd < p_event_item->fd) {
            max_fd = p_event_item->fd;
        }
    }   

    timeval tv = {0};
    tv.tv_sec = timeout;

    int rv = select(max_fd + 1, &readSet, &writeSet, &errorSet, &tv);

    if (rv == -1) {
        fprintf(stderr, "ERROR: select: %s\n", strerror(errno));
        return -1;
    } else if (rv == 0) {
        return 0;
    }

    list_for_each_entry_safe(p_event_item, pTmp, &m_event_list, list) {
        if (!p_event_item->deleted && FD_ISSET(p_event_item->fd, &readSet)) {
            p_event_item->event_proc(p_event_item->fd, EVENT_READ, p_event_item->p_user_data);
        }
        if (!p_event_item->deleted && FD_ISSET(p_event_item->fd, &writeSet)) {
            p_event_item->event_proc(p_event_item->fd, EVENT_WRITE, p_event_item->p_user_data);
        }
        if (!p_event_item->deleted && FD_ISSET(p_event_item->fd, &errorSet)) {
            p_event_item->event_proc(p_event_item->fd, EVENT_ERROR, p_event_item->p_user_data);
        }
    }   

    return 0;
}

int c_event::uninit()
{
    event_item_t *p_event_item = NULL;
    event_item_t *pTmp = NULL;
    list_for_each_entry_safe(p_event_item, pTmp, &m_event_list, list) {
        list_del(&((event_item_t *)p_event_item)->list);
        free(p_event_item);
        p_event_item = NULL;
    }

    return 0;
}

int c_event::release()
{
    delete this;

    return 0;
}
