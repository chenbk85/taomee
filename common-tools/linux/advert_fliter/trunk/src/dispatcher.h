#ifndef SWITCH_DISPATCH_HPP_
#define SWITCH_DISPATCH_HPP_

extern "C" {
#include <async_serv/async_serv.h>
}

void init_funcs();
void try_free_msg_timely();

int  dispatch(void* data, fdsession_t* fdsess);

#endif // SWITCH_DISPATCH_HPP_

