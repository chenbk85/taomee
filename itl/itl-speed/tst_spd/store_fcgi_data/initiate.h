/*
 * initiate.h
 *
 * Created on:	2011-8-4
 * Author:		Singku
 * Paltform:		Linux Fedora Core 8 x86-32
 *	Compiler:		GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef INITIATE_H_
#define INITIATE_H_


extern int initiate();
extern int my_mkdir(const char *path);
extern int data_forward;
extern int forward_fd;
extern char forward_ip[16];
extern uint16_t forward_port;

#endif /* INITIATE_H_ */
