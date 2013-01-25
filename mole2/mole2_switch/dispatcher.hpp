#ifndef SWITCH_DISPATCH_HPP_
#define SWITCH_DISPATCH_HPP_

extern "C" {
#include <async_serv/dll.h>
}

void init_funcs();

int  dispatch(void* data, fdsession_t* fdsess);
int	 dispatch_udp(void* data, int fd);

#endif // SWITCH_DISPATCH_HPP_

