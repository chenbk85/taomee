#ifndef __DISPATCH_HPP__
#define __DISPATCH_HPP__

#include "common.hpp"


#define MAX_CACHE_PKG		(50)

int dispatch(void *data, fdsession_t *fdsess, bool cache_flag);


#endif // __DISPATCH_HPP__
