#ifndef SWITCH_DISPATCH_HPP_
#define SWITCH_DISPATCH_HPP_

extern "C" {
#include <async_serv/dll.h>
}
#include "cross_inc.hpp"

int dispatch(IJobDispatcher* dispatcher, void* data, fdsession_t* fdsess);

#endif // SWITCH_DISPATCH_HPP_

