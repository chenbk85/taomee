/**
 * @file ipc_traits.hpp
 * @brief 定义ipc类型的traits
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-19
 */
#ifndef _H_IPC_TRAITS_HPP_
#define _H_IPC_TRAITS_HPP_

struct ipc_tag { };

struct sock_tag : public ipc_tag { };
struct sock_stream_tag : public sock_tag { };
struct sock_acceptor_tag : public sock_tag { };
struct sock_connector_tag : public sock_tag { };
struct sock_dgram_tag : public sock_tag { };

struct vpipe_tag : public ipc_tag { };
struct vpipe_sockpair_tag : public vpipe_tag { };

template<typename ipc_t>
struct ipc_traits {
    typedef typename ipc_t::ipc_category ipc_category;
};

#endif
