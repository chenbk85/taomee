/**
 *============================================================
 *  @file        net_if.h
 *  @brief      Essential net interface to deal with network
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef ITERATIVE_SERVER_NET_INTERFACE_H_
#define ITERATIVE_SERVER_NET_INTERFACE_H_

#include <arpa/inet.h>

/**
  * @brief Connect to a given server.
  *
  * @param const char* ipaddr,  ip address of the server to connect to.
  * @param in_addr_t port,  port of the server to connect to.
  * @param int bufsz,  size of the buffer to hold the sending data.
  * @param int timeout, interrupt the connecting attempt after timeout secs.
  *
  * @return int, the connected socket fd, -1 on error.
  */
int connect_to_svr(const char* ipaddr, in_addr_t port, int bufsz, int timeout);

/**
  * @brief Create a udp socket to the given server.
  *
  * @param struct sockaddr_in* addr,  it will be initialized base on the given ip and port.
  * @param const char* ip,  ip address of the server.
  * @param in_port_t port,  port of the server.
  *
  * @return int, the created udp socket fd, -1 on error.
  */
int create_udp_socket(struct sockaddr_in* addr, const char* ip, in_port_t port);

/**
  * @brief Send data to the given tcp socket fd
  *
  * @param int fd,  socket fd to send data to.
  * @param const void* data,  data to be sent to the given fd.
  * @param uint32_t len,  length of the data to be sent.
  *
  * @return int, 0 on sucess, -1 on failure.
  */
int net_send(int fd, const void* data, uint32_t len);

#endif // ITERATIVE_SERVER_NET_INTERFACE_H_

