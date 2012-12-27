/*
 * =====================================================================================
 *
 *       Filename:  mail.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  04/06/2010 09:03:21 AM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  kain (kain), kain@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#ifndef MOLE_MAIL_H
#define MOLE_MAIL_H

typedef struct {
	uint32_t	mail_id;
	int			mail_bit;
	int			date_start;
	int			date_end;
	char		send_by[32];
	char		msg_fmt[256 + 16];
} mail_t;

int load_mail_conf(const char *file);
int send_mail_to_self(sprite_t *p, int check, int mail_id, ...);

#endif   /* ----- #ifndef MOLE_MAIL_H  ----- */

