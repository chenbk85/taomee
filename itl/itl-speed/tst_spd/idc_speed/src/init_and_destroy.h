/*
 * initiate.h
 *
 *  Created on:	2011-7-4
 *  Author:		singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef INITIATE_H_
#define INITIATE_H_

extern int initiate();
extern void destroy();
extern int collect_load_config();
extern int send_load_config();
extern void get_time_string();


#endif /* INITIATE_H_ */
