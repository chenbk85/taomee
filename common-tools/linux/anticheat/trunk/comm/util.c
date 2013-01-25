#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

#include "util.h"

char *skip_blank(char *str)
{
	while(*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n') str++;
	return str;
}

int get_local_eth_ip(const char *eth, struct in_addr *addr)
{
/*
	   struct ifreq {
		   char ifr_name[IFNAMSIZ]; // Interface name
		   union {
			   struct sockaddr ifr_addr;
			   struct sockaddr ifr_dstaddr;
			   struct sockaddr ifr_broadaddr;
			   struct sockaddr ifr_netmask;
			   struct sockaddr ifr_hwaddr;
			   short	       ifr_flags;
			   int	    	   ifr_ifindex;
			   int	     	   ifr_metric;
			   int	      	   ifr_mtu;
			   struct ifmap    ifr_map;
			   char	      	   ifr_slave[IFNAMSIZ];
			   char	       	   ifr_newname[IFNAMSIZ];
			   char *	       ifr_data;
		   };
	   };

	   struct ifconf {
			int		     	  ifc_len; // size of buffer
			union {
			   char			* ifc_buf; // buffer address
			   struct ifreq * ifc_req; // array of structures
	   		};
	   };
 */

	int fd;
	struct sockaddr_in *sin;
	struct ifreq ifr;

	if ((fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket: ");
		return -1;
	}

	/* Get IP Address */
	strncpy(ifr.ifr_name, eth, IF_NAMESIZE);
	ifr.ifr_name[IFNAMSIZ - 1]='\0';

	if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) {
		perror("ioctl: ");
		close(fd);
		return -1;
	}
	close(fd);

	sin = (struct sockaddr_in *)(&ifr.ifr_addr);
	memcpy(addr, &(sin->sin_addr), sizeof(*addr));

	return 0;
}

int get_local_eth_ipstr(const char *eth, char *ipbuf)
{
	struct sockaddr_in sin;

	if (get_local_eth_ip(eth, &(sin.sin_addr)) == -1) {
		return -1;
	}

	if (inet_ntop(AF_INET, &(sin.sin_addr), ipbuf, INET_ADDRSTRLEN) == NULL) {
		perror("inet_ntop: ");
		return -1;
	}

	return 0;
}

int set_tcp_socket_nodely(int sock)
{
	int ret;
	int flag = 1;

	ret = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
	if (ret == -1) {
		fprintf(stderr, "ISSUE: Failed to set socket nodely: fd=%d, err(%d): %s\n",
				sock, errno, strerror(errno));
		return -1;
	}
	return 0;
}

int set_socket_rwmem(int sock, size_t size)
{
	/* 不管能不能设置, 都返回成功 */
	setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const void *)&size, sizeof (int));
	setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const void *)&size, sizeof (int));

	return 0;
}

int set_socket_nonblock(int sock)
{
	int flag;

	flag = fcntl(sock, F_GETFL, 0);
	flag |= O_NONBLOCK;
	flag |= O_NDELAY;
	fcntl(sock, F_SETFL, flag);
	return 0;
}

void set_socket_reuseaddr(int sock)
{
	int flag = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
}

/* return: 0: ok, -1: failed */
int set_rlimit(int max_open_files)
{
/*
   struct rlimit {
		rlim_t rlim_cur;  // Soft limit
		rlim_t rlim_max;  // Hard limit (ceiling for rlim_cur)
   };
*/

	int r;
	struct rlimit rlim;

	/* max open files */
	rlim.rlim_cur = rlim.rlim_max = max_open_files;
	r = setrlimit(RLIMIT_NOFILE, &rlim);
	if (r == -1) {
		fprintf(stderr, "ERROR: Failed to set RLIMIT_NOFILE to %d, err(%d): %s\n",
				max_open_files, errno, strerror(errno));
		return -1;
	}

	rlim.rlim_cur = rlim.rlim_max = RLIM_INFINITY;
	r = setrlimit(RLIMIT_CORE, &rlim);
	if (r == -1) {
		fprintf(stderr, "ISSUE: Failed to set RLIMIT_CORE, err(%d): %s\n",
				errno, strerror(errno));
	}

	return 0;
}
