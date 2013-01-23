/**
 * @file cmd_handler.cpp
 * @brief demo
 * @author baron baron@taomee.com
 * @version 1.0
 * @date 2011-04-26
 */
#include "cmd_handler.h"
#include "log.h"
#include "proto.h"

#include <string.h>
#include <string>
#include <stdio.h>

static int cmd_getfile_(char *filename, char *p_outbuf, int *p_outbuf_len)
{
    std::string path("./res/");
    path.append(filename);
    FILE *p_file = fopen(path.c_str(), "r");

    if (NULL == p_file)
    {
        return -1;
    }

    int length = fread(p_outbuf, 1, *p_outbuf_len, p_file);

    if (length <= 0)
    {
        fclose(p_file);
        return PROTO_ERROR_COMMON;
    }

    *p_outbuf_len = length;
    fclose(p_file);
    return 0;
}

int dispatch(int cmd,
             int user_id,
             const char *p_proto_body,
             int body_len,
             char *p_outbuf,
             int *p_outbuf_len)
{
    static int user_id_ = -1;

    if (-1 == user_id_)
    {
        user_id_ = user_id;
    }

    if (user_id_ != user_id)
    {
        ERROR_LOG("----------------user_id: %d != %d", user_id, user_id_);
    }

    switch (cmd)
    {
        case PROTO_CMD_GETFILE:
        {
            static char filename[128];
            const proto_request_body_t *p_body
                = reinterpret_cast<const proto_request_body_t *>(p_proto_body);
            strncpy(filename, p_body->name, body_len);
            filename[body_len] = '\0';

            DEBUG_LOG("user[%d] request: get file[%s][length:%d]",
                        user_id, filename, *p_outbuf_len);

            return cmd_getfile_(filename, p_outbuf, p_outbuf_len);
        }
        default:
            ERROR_LOG("user[%d] request: invalid cmd", user_id);
            return 1;
    }
}
