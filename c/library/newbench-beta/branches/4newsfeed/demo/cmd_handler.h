#ifndef __DEMO_COMMAND_HANDLER_H__
#define __DEMO_COMMAND_HANDLER_H__

#include "newbench.h"

int dispatch(int cmd,
             int user_id,
             const char *p_proto_body,
             int body_len,
             char *p_outbuf,
             int *p_outbuf_len);

#endif //!__DEMO_COMMAND_HANDLER_H__
