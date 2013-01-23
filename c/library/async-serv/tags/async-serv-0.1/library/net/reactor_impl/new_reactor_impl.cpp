/**
 * @file new_reactor_impl.cpp
 * @brief
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-11
 */
#include <stdlib.h>

#include <new>

#include "../reactor_impl.h"
#include "epoll_reactor_impl.h"
#include "select_reactor_impl.h"

reactor_impl* reactor_impl::new_reactor_impl()
{
    if (::getenv("USE_SELECT_REACTOR")) {
        return new (std::nothrow) select_reactor_impl;
    } else {
        return new (std::nothrow) epoll_reactor_impl;
    }
}
