/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file test_client.cpp
 * @author richard <richard@taomee.com>
 * @date 2011-07-14
 */

#include <stdio.h>
#include <stdint.h>

#include "i_net_client.h"

int main()
{
    i_net_client *p_net_client = create_net_client_instance();
    if (p_net_client == NULL) {
        fprintf(stderr, "ERROR: create_net_client_instance\n");
        return -1;
    }

    if (p_net_client->init("127.0.0.1", 4444, 4) != 0) {
        fprintf(stderr, "ERROR: p_net_client->init\n");
        return -1;
    }

    struct timeval timeout = {1, 0};

    if (p_net_client->open_conn(&timeout) != 0) {
        fprintf(stderr, "ERROR: p_net_client->open_conn\n");
        return -1;
    }

    char send_buff[8172] = {0};
    char recv_buff[8172] = {0};

    *(uint32_t *)send_buff = 5000;

    for (int i = 0; i != 100; ++i) {
        for (int j = 0; j < i; ++j) {
            send_buff[sizeof(uint32_t) + j] = 'a';
        }
        fprintf(stdout, "send: %s\n", send_buff + sizeof(uint32_t));
        struct timeval timeout = {10, 0};
        if (p_net_client->send_rqst(send_buff, recv_buff, sizeof(recv_buff), &timeout) != 0) {
            fprintf(stderr, "ERROR: p_net_client->open_conn\n");
            return -1;
        }
        fprintf(stdout, "recv: %s\n", recv_buff + sizeof(uint32_t));
    }
    
    if (p_net_client->close_conn() != 0) {
        fprintf(stderr, "ERROR: p_net_client->open_conn\n");
        return -1;
    }
    
    if (p_net_client->uninit() != 0) {
        fprintf(stderr, "ERROR: p_net_client->open_conn\n");
        return -1;
    }
    
    if (p_net_client->release() != 0) {
        fprintf(stderr, "ERROR: p_net_client->open_conn\n");
        return -1;
    }

    return 0;
}
