#ifndef _TCPIP_H_
#define _TCPIP_H_
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
extern "C"{
//#include "libtaomee/log.h"
#include "log.h"
}
#include <unistd.h>


#define SUCC 0
#define  SYS_ERR 1001
#define  NET_ERR 1003
struct stru_ip_port {
    char addr[128]; /* ip or dns */
    uint32_t port;
};  
inline int unpack_addr(const char * addr,  stru_ip_port *p_out  )
{   
	if (addr==NULL ) return -1;
    const char * p=addr; 
    while (*p!='\0' && *p!=':') ++p;
 
    if (*p=='\0' ) return -1; 
    uint32_t len=p-addr;
     if (len>sizeof(p_out->addr)-1) return -1   ;
        
    strncpy(p_out->addr,  addr,len);
    p_out->addr[len]='\0';
    p_out->port=atol( p+1 );
    return 0;
}
inline int t_connect_to_svr(const char* ipaddr, uint16_t port)
{
        struct sockaddr_in peer;
        //int fd;

        bzero(&peer, sizeof (peer));
        peer.sin_family  = AF_INET;
        peer.sin_port    = htons(port);
        if (inet_pton (AF_INET, ipaddr, &peer.sin_addr) <= 0) {
        return -1;
    }

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == connect(sockfd, (const struct sockaddr *)&peer, sizeof(peer))) {
//        DEBUG_LOG("[tcpip.h] errno[%d]:%s",errno,strerror(errno));        
		return -1;
    }

        return sockfd;
}



int open_socket (const char *svrip, u_short svrport, uint32_t timeout=3 );
int open_socket_udp (const char* svrip, u_short svrport,struct  sockaddr_in *servaddr);
int net_io(int socketfd, const char *sndbuf, int sndlen, char** rcvbuf, int *rcvlen);
int net_io_no_return(int sockfd, const char *sndbuf, int sndlen );
class Cudp_sender{
	public:
		Cudp_sender(){ fd=-1;memset (&addr,0,sizeof(addr)); };
		Cudp_sender(const char *a_ipport ){
			struct stru_ip_port ip_port;
			if ( unpack_addr(a_ipport,&ip_port )==0){
				init( ip_port.addr ,ip_port.port);
			}else{
				fd=-1;memset (&addr,0,sizeof(addr)); 
			}	
		};
		Cudp_sender(char *a_ip,uint16_t a_port ){
			init( a_ip,a_port);
		};
inline		void init(char *a_ip , uint16_t a_port ){
			strcpy(this->ip,a_ip);
			this->port=a_port;
			this->fd= open_socket_udp(this->ip, this->port, &(this->addr));	
		}
	inline	bool send(char *buf, int len  ){
		    int sendlen=sendto(this->fd, buf, len, 0,
            (const struct sockaddr *)&(this->addr), sizeof(this->addr));	
			if (sendlen!=len){
				this->fd= open_socket_udp(this->ip, this->port, &(this->addr));	
				if (this->fd!=-1){
		    		sendlen=sendto(this->fd, buf, len, 0,
            			(const struct sockaddr *)&(this->addr), sizeof(this->addr));	
				}
			}
			return sendlen==len;
		}
		inline const char * get_ip(){
			return ip;
		}

	inline 		uint16_t	 get_port(){
			return  port;
		}


  private:
		char ip[32];
		uint16_t port;
		int fd;
		struct  sockaddr_in addr;	
};
class Ctcp{
	public:
		Ctcp(){ fd_=-1; };

		//"10.1.1.24:21001"
		Ctcp(const char *a_ipport  ,uint32_t send_recv_timeout=1, uint32_t reconnect_interval=10){
			struct stru_ip_port ip_port;
			if ( unpack_addr(a_ipport,&ip_port )==0){
				init( ip_port.addr ,ip_port.port,send_recv_timeout,reconnect_interval );
			}else{
				this->fd_=-1;
			}	
		};

		Ctcp(const char *ip,short port,uint32_t send_recv_timeout=1, uint32_t reconnect_interval=10 ){
			this->fd_=-1;
			init( ip,port,send_recv_timeout,reconnect_interval );
		};
		~Ctcp(){
			CGI_DEBUG_LOG("~Ctcp");
			close(this->fd_);
		}
		void init(const char *ip ,short port,uint32_t send_recv_timeout,uint32_t reconnect_interval ){
			if (ip!=NULL){
				strncpy(this->ip_,ip,sizeof(this->ip_));
			}else{
				memset(this->ip_,'\0',sizeof(this->ip_) );
			}
			this->ip_[sizeof(this->ip_)]='\0';
			this->port_=port;
			this->send_recv_timeout_=send_recv_timeout;
			this->reconnect_interval_=reconnect_interval;
			this->last_fail_connect_time_=0;
			this->reconnect();
		}

		int reconnect()
		{
			uint32_t now=time(NULL);
			if (this->fd_!=-1){
				close(this->fd_);
				this->fd_=-1;
			}

			if (now-this->last_fail_connect_time_<this->reconnect_interval_){
				//小于失败时间 间隔，直接返回
				return this->fd_;
			}

			this->fd_= t_connect_to_svr(this->ip_, this->port_ );	
			if (this->fd_==-1){//连接失败
				this->last_fail_connect_time_=time(NULL);
			}
			return this->fd_;
		}
			
		int do_net_io( const char *sndbuf, const int sndlen, char **rcvbuf, int  *rcvlen)
		{
			int ret;
			ret=net_io(this->fd_,sndbuf,sndlen,rcvbuf,rcvlen);
		//	DEBUG_LOG("[tcpip.h] fd[%d] ret [%d]",this->fd_,ret);
			if ( ret!=SUCC ){
		//		DEBUG_LOG("[tcpip.h] net error");
				if (this->reconnect()==-1){
		//			DEBUG_LOG("[tcpip.h] reconnect fail.fd[%d]",this->fd_);
					return NET_ERR;
				}else{
					ret = net_io(this->fd_,sndbuf,sndlen,rcvbuf,rcvlen);
		//			DEBUG_LOG("[tcpip.h] :ret [%d]",ret);
					return ret;
				}
			}	  
			return ret;
		}
		inline const char * get_ip(){
			return ip_;
		}

		inline 	uint16_t	get_port(){
			return  port_;
		}



  private:
		char ip_[16];
		short port_;
		int fd_;
		uint32_t send_recv_timeout_;//发送接收超时
		uint32_t reconnect_interval_;//重连时间间隔
		uint32_t last_fail_connect_time_;//最后一次连接失败的时间
};


/**
 * 得到网卡ip
 * ip_type :0x01 内网 
 * ip_type :0x02 外网 
 * */
const char * get_ip_ex (const int ip_type );

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

inline uint32_t ip2long(const char *ip) {
	return inet_addr(ip);
}

inline char * long2ip(uint32_t ip_int ) {
	struct in_addr in;
	in.s_addr=ip_int;
	return inet_ntoa( in) ;
}



#endif 

