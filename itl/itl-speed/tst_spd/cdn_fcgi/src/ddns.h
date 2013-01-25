/*
 * ddns.h
 *
 *  Created on:	2011-7-6
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef DDNS_H_
#define DDNS_H_

/*
 * int getaddrinfo(	const char *restrict host,
 * 							const char *restirct service,
 * 							const struct addrinfo *restrict hint,
 * 							struct addrinfo **restrict res
 * 						);
 * struct addrinfo{
 * 		int	 				ai_flags;
 * 		int					ai_family;
 * 		int					ai_socktype;
 * 		int					ai_protocol;
 * 		socklen_t			ai_addrlen;
 * 		struct sockaddr	*ai_addr;
 * 		char				*canonname;
 * 		struct addrinfo	*ai_next;
 * };
 */

/*
 * @brief 利用getaddrinfo() 解析DNS地址并返回IP，可能返回多个IP
 * @param domainname 待解析的域名
 * @return 地址链表
 */

extern int dns_resolve(const char *domainname);

#endif /* DDNS_H_ */
