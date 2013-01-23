/**
 * @file event_handler.h
 * @brief 事件回调基类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-03
 */
#ifndef _H_EVENT_HANDLER_H_
#define _H_EVENT_HANDLER_H_

#include "log.h"

class event_handler {
public:
    event_handler() { }
    virtual ~event_handler() { }

    /**
     * @brief 读事件回调函数，在读事件响应时调用此函数
     * @return 0success; -1failed,若返回-1反应器会取消注册事件
     */
    virtual int handle_read() {
        return 0;
    }

   /**
     * @brief 写事件回调函数，在写事件响应时调用此函数
     * @return 0success; -1failed,若返回-1反应器会取消注册事件
     */
   virtual int handle_write() {
        return 0;
    }

    /**
     * @brief 错误事件回调函数，在错误事件响应时调用此函数
     * @return 0success; -1failed, 此事件回调函数返回后反应器会取消注册事件
     */
   virtual int handle_error() {
        return 0;
    }

   /**
     * @brief 定时事件回调函数，在定时器到期时调用此函数
     * @return 0success; -1failed
     */
   virtual int handle_timer() {
        return 0;
    }

   /**
    * @brief 获取该事件所关联的fd
    * @return -1failed, >=0 fd值
    */
    virtual int get_fd() {
        return 0;
    }

} __attribute__((aligned(sizeof(long))));

#endif
