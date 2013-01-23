/**
 * @file mulithomed_echo_serv.cpp
 * @brief 支持多端口多协议的echo服务器
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
#include "sock_acceptor.h"
#include "sock_stream.h"
#include "sock_dgram.h"
#include "inet_address.h"
#include "reactor.h"
#include "acceptor.hpp"
#include "config.h"

class mulithomed_echo_serv {
public:
    mulithomed_echo_serv(u_short local_port)
        : m_local_addr_(local_port),
          m_p_reator_(reactor::instance()),
          m_p_tcp_pool_(connections_pool<connection<sock_stream> >::instance()),
          m_p_udp_pool_(connections_pool<connection<sock_dgram> >::instance()),
          m_tcp_acceptor_(m_local_addr_),
          m_udp_acceptor_(m_local_addr_)
    {
        m_p_reator_->reactor_init();
    }

    ~mulithomed_echo_serv() {
        m_p_tcp_pool_->destroy_conns_pool();
        m_p_udp_pool_->destroy_conns_pool();

        m_p_reator_->reactor_fini();

        delete m_p_tcp_pool_;
        delete m_p_udp_pool_;
        delete m_p_reator_;
    }

    int start() {
        m_tcp_acceptor_.set_new_conn_callback(
                boost::bind(&mulithomed_echo_serv::on_new_conn<sock_stream>, this, _1));
        m_udp_acceptor_.set_new_conn_callback(
                boost::bind(&mulithomed_echo_serv::on_new_conn<sock_dgram>, this, _1));
        m_tcp_acceptor_.set_on_message_callback(
                boost::bind(&mulithomed_echo_serv::on_message_tcp, this, _1, _2, _3));
        m_udp_acceptor_.set_on_message_callback(
                boost::bind(&mulithomed_echo_serv::on_message_udp, this, _1, _2, _3));

        if (m_tcp_acceptor_.start() < 0) return -1;
        m_tcp_acceptor_.enable_accepting();

        if (m_udp_acceptor_.start() < 0) return -1;
        m_udp_acceptor_.enable_accepting();

        while (1) {
            m_p_reator_->reactor_wait(500);
        }

        return 0;
    }


private:
    template<typename ipc_stream_t>
    int on_new_conn(connection<ipc_stream_t>* p_conn) {
        return 0;
    }

    int on_message_tcp(connection<sock_stream>* p_conn, void* buf, int len) {
        std::cout << "recv_len " << len << std::endl;

        char* p_recv_data = new char[len + 1];
        memcpy(p_recv_data, buf, len);
        p_recv_data[len] = '\0';

        std::cout << "recv data: " << p_recv_data << std::endl;

        if (p_conn->send_data(buf, len) < 0) {
            return -1;
        }
        return 0;
    }

    int on_message_udp(connection<sock_dgram>* p_conn, void* buf, int len) {
        std::cout << "recv_len " << len << std::endl;

        char* p_recv_data = new char[len + 1];
        memcpy(p_recv_data, buf, len);
        p_recv_data[len] = '\0';

        std::cout << "recv data: " << p_recv_data << std::endl;

        delete p_recv_data;
        if (p_conn->send_data(p_conn->get_remote_addr(), buf, len) < 0) {
            return -1;
        }
        return 0;
    }

private:
    inet_address m_local_addr_;
    reactor* m_p_reator_;
    connections_pool<connection<sock_stream> >* m_p_tcp_pool_;
    connections_pool<connection<sock_dgram> >* m_p_udp_pool_;
    acceptor<connection<sock_stream>, sock_acceptor> m_tcp_acceptor_;
    acceptor<connection<sock_dgram>, sock_dgram> m_udp_acceptor_;
};


int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "usage: local_port" << std::endl;
        return 0;
    }

    mulithomed_echo_serv echo_serv(atoi(argv[1]));

    echo_serv.start();

    return 0;
}
