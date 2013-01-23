/**
 * @file chargen_serv.cpp
 * @brief 字符串发生器服务端
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-25
 */

#include <unistd.h>
#include <time.h>

#include <iostream>
#include <string>

#include <boost/bind.hpp>

#include "connection.hpp"
#include "connections_pool.hpp"
#include "acceptor.hpp"
#include "reactor.h"
#include "sock_acceptor.h"
#include "sock_stream.h"
#include "inet_address.h"
#include "timer.h"

static int g_index = 0;
static uint64_t g_total_data = 0;

class chargen_serv {
public:
    chargen_serv(u_short local_port)
        : m_local_addr_(local_port),
          m_p_reator_(reactor::instance()),
          m_p_tcp_pool_(connections_pool<connection<sock_stream> >::instance()),
          m_tcp_acceptor_(m_local_addr_),
          m_timer_(boost::bind(&chargen_serv::on_timer, this, _1),
                   NULL,
                   time(NULL) + 10,
                   TIMER_REPEAT,
                   1)
    {

        m_p_reator_->reactor_init();

        m_timer_.register_timer();

        for (int i = 33; i < 127; i++) {
            m_line_.push_back((char)i);
        }
        m_line_ += m_line_;

        for (size_t i = 0; i < 127 - 33; i++) {
            m_message_ += m_line_.substr(i, 72) + '\n';
        }
    }

    ~chargen_serv() {
        m_p_tcp_pool_->destroy_conns_pool();

        m_p_reator_->reactor_fini();

        delete m_p_tcp_pool_;
        delete m_p_reator_;
    }

    int start() {
        m_tcp_acceptor_.set_new_conn_callback(
                boost::bind(&chargen_serv::on_new_conn, this, _1));
        m_tcp_acceptor_.set_on_write_comple_callback(
                boost::bind(&chargen_serv::on_write_comple, this, _1));

        if (m_tcp_acceptor_.start() < 0) return -1;
        m_tcp_acceptor_.enable_accepting();

        while (1) {
            m_p_reator_->reactor_wait(500);
        }

        return 0;
    }

private:
    int on_new_conn(connection<sock_stream>* p_conn) {

        if (p_conn->send_data((void*)m_message_.c_str(), m_message_.length()) < 0)
            return -1;
        g_total_data += m_message_.length();

        return 0;
    }

    int on_write_comple(connection<sock_stream>* p_conn) {

       if (p_conn->send_data((void*)m_message_.c_str(), m_message_.length()) < 0) {
           return -1;
       }

       g_total_data += m_message_.length();

       return 0;
    }

    void on_timer(void* arg){
        double data_in_mb = g_total_data/1024/1024;

        std::cout << "total data send: " << data_in_mb << " MB" << std::endl;
    }

private:
    inet_address m_local_addr_;
    reactor* m_p_reator_;
    connections_pool<connection<sock_stream> >* m_p_tcp_pool_;
    acceptor<connection<sock_stream>, sock_acceptor> m_tcp_acceptor_;
    timer m_timer_;

    std::string m_line_;
    std::string m_message_;

};

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "usage: local_port" << std::endl;
        return 0;
    }

    chargen_serv serv(atoi(argv[1]));

    serv.start();

    return 0;
}
