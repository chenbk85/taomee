#ifndef SWITCH_DISPATCH_HPP_
#define SWITCH_DISPATCH_HPP_
#ifdef __cplusplus
extern "C"
{
#endif

#include <async_serv/dll.h>

#ifdef __cplusplus
}
#endif

void init_funcs();

int  dispatch(void* data, fdsession_t* fdsess);

#endif // SWITCH_DISPATCH_HPP_

