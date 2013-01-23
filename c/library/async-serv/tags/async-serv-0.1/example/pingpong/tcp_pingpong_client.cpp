/**
 * @file tcp_echo_client.cpp
 * @brief tcp pingpong服务客户端示例代码
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-24
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
#include "sock_connector.h"
#include "sock_stream.h"
#include "inet_address.h"
#include "reactor.h"
#include "connector.hpp"
#include "timer.h"


class tcp_echo_client {
public:
    tcp_echo_client(const std::string& serv_ip, u_short serv_port)
        : m_serv_addr_(serv_ip, serv_port),
          m_p_reator_(reactor::instance()),
          m_p_tcp_pool_(connections_pool<connection<sock_stream> >::instance()),
          m_tcp_conn_(m_serv_addr_),
          m_timer_(boost::bind(&tcp_echo_client::on_timer, this, _1),
                   NULL,
                   time(NULL) + 5,
                   TIMER_ONCE),
          m_loopout_(0),
          m_send_data_(0),
          m_recv_data_(0)
    {
        m_p_reator_->reactor_init();
        m_timer_.register_timer();
    }

    ~tcp_echo_client() {
        m_p_tcp_pool_->destroy_conns_pool();
        m_p_reator_->reactor_fini();

        delete m_p_tcp_pool_;
        delete m_p_reator_;
    }

    int start() {
        m_tcp_conn_.set_new_conn_callback(
                boost::bind(&tcp_echo_client::on_new_conn, this, _1));
        m_tcp_conn_.set_on_message_callback(
                boost::bind(&tcp_echo_client::on_message, this, _1, _2, _3));

        if (m_tcp_conn_.start() < 0) return -1;

        while (!m_loopout_) {
            m_p_reator_->reactor_wait(500);
        }

        return 0;
    }


private:
    int on_new_conn(connection<sock_stream>* p_conn) {
        static char send_bytes[1024 * 16] = {0};
        if (p_conn->send_data(send_bytes, sizeof(send_bytes)) < 0) {
            return -1;
        }
        m_send_data_ += sizeof(send_bytes);

        return 0;
    }

    int on_message(connection<sock_stream>* p_conn, void* buf, int len) {
        if (p_conn->send_data(buf, len) < 0) {
            return -1;
        }
        m_send_data_ += len;
        m_recv_data_ += len;

        return 0;
    }

    void on_timer(void* arg) {
        m_loopout_ = 1;
        std::cout << "total send data in 5 sec " << m_send_data_/1024/1024 << " MB" << std::endl;
        std::cout << "total  send throughput " << m_send_data_/1024/1024/5 << " MB/s" << std::endl;

        std::cout << "total recv data in 5 sec " << m_recv_data_/1024/1024 << " MB" << std::endl;
        std::cout << "total recv throughput " << m_recv_data_/1024/1024/5 << " MB/s" << std::endl;
    }

private:
    inet_address m_serv_addr_;
    reactor* m_p_reator_;
    connections_pool<connection<sock_stream> >* m_p_tcp_pool_;
    connector<connection<sock_stream>, sock_connector> m_tcp_conn_;
    timer m_timer_;
    int m_loopout_;
    uint64_t m_send_data_;
    uint64_t m_recv_data_;
};


int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cout << "usage: serv_ip serv_port" << std::endl;
        return 0;
    }

    tcp_echo_client echo_client(argv[1], atoi(argv[2]));

    echo_client.start();

    return 0;
}
