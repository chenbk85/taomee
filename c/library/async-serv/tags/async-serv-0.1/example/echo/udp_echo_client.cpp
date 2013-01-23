/**
 * @file udp_echo_client.cpp
 * @brief udp echo客户端示例代码
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-25
 */
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <boost/bind.hpp>

#include "connection.hpp"
#include "connections_pool.hpp"
#include "inet_address.h"
#include "reactor.h"
#include "connector.hpp"
#include "sock_dgram.h"

class udp_echo_client {
public:
    udp_echo_client(const std::string& serv_ip, u_short serv_port)
        : m_serv_addr_(serv_ip, serv_port),
          m_p_reator_(reactor::instance()),
          m_p_udp_pool_(connections_pool<connection<sock_dgram> >::instance()),
          m_loopout(0)
    {
        m_p_reator_->reactor_init();
    }

    ~udp_echo_client() {
        m_p_udp_pool_->destroy_conns_pool();
        m_p_reator_->reactor_fini();

        delete m_p_udp_pool_;
        delete m_p_reator_;
    }

    int start() {
        m_udp_conn_.set_new_conn_callback(
                boost::bind(&udp_echo_client::on_new_conn, this, _1));
        m_udp_conn_.set_on_message_callback(
                boost::bind(&udp_echo_client::on_message, this, _1, _2, _3));

        if (m_udp_conn_.start() < 0) return -1;

        while (!m_loopout) {
            m_p_reator_->reactor_wait(500);
        }

        return 0;
    }


private:
    int on_new_conn(connection<sock_dgram>* p_conn) {
        char send_bytes[1024] = {0};
        int len = sprintf(send_bytes, "pid[%d] udp_echo_client send bytes.\r\n", getpid());
        if (p_conn->send_data(m_serv_addr_, send_bytes, len) < 0) {
            return -1;
        }

        return 0;
    }

    int on_message(connection<sock_dgram>* p_conn, void* buf, int len) {
        std::cout << "recv_len " << len << std::endl;

        char* p_recv_data = new char[len + 1];
        memcpy(p_recv_data, buf, len);
        p_recv_data[len] = '\0';

        std::cout << "recv data: " << p_recv_data << std::endl;

        m_loopout = 1;
        return 0;
    }
private:
    inet_address m_serv_addr_;
    reactor* m_p_reator_;
    connections_pool<connection<sock_dgram> >* m_p_udp_pool_;
    connector<connection<sock_dgram>, sock_dgram> m_udp_conn_;

    int m_loopout;

};


int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cout << "usage: serv_ip serv_port" << std::endl;
        return 0;
    }

    udp_echo_client echo_client(argv[1], atoi(argv[2]));

    echo_client.start();

    return 0;
}
