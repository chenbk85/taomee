/**
 *====================================================================
 *  @file        		mole_bank.h
 *  @brief     		This file is to include the function prototypes and macros.
 *
 *  compiler   		gcc  4.1.2
 *  platform   		Debian GNU/LINUX
 *  author      		Kal, Develop Group 1
 *  create date	2009.2.26
 *  copyright  		TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *====================================================================
*/

#ifndef MOLE_BANK_H
#define MOLE_BANK_H


typedef struct deposit {
	uint32_t deposit_bean;
	uint32_t drawout;
	uint32_t deposit_time;
	uint32_t deposit_interval;
} __attribute__((packed)) deposit_t;

int get_deposit_info_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int deposit_bean_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int draw_bean_cmd(sprite_t* p, const uint8_t *body, int bodylen);


int get_deposit_info_callback(sprite_t *p, uint32_t id, char *buf, int len);
int deposit_bean_callback(sprite_t *p, uint32_t id, char *buf, int len);
int draw_bean_callback(sprite_t *p, uint32_t id, char *buf, int len);

#endif

