#ifndef __UTIL_H__
#define __UTIL_H__

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>

char *skip_blank(char *str);
int get_local_eth_ip(const char *eth, struct in_addr *addr);
int get_local_eth_ipstr(const char *eth, char *ipbuf);
int set_tcp_socket_nodely(int sock);
int set_socket_rwmem(int sock, size_t size);
int set_socket_nonblock(int sock);
void set_socket_reuseaddr(int sock);
int set_rlimit(int max_open_files);


#endif /* __UTIL_H__ */
