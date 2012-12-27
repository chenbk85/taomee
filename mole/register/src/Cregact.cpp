/*
 * =====================================================================================
 *
 *       Filename:  Cregact.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/20/2008 03:51:41 PM EST
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  spark (spark), zhezhide@163.com
 *        Company:  TAOMEE
 * 		
 * 		------------------------------------------------------------
 * 		view configure:
 * 			VIM:  set tabstop=4 
 * 		
 * =====================================================================================
 */

#include <cstring>
#include <ctime>

#include <iostream>
#include <cerrno>
extern "C" {
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/interprocess/filelock.h>
#include "../inc/benchapi.h"
}

#include "common.h"
#include "Cregact.h"
#include "pub.h"
#include "register.h"

extern int uid_idx_fd;
extern int unreg_id_fd;
extern int usr_id_fd;

Cregact::Cregact(void* usr_id_map, void* uid_idx_map)
{
	id_idx_ = reinterpret_cast<uint32_t*>(uid_idx_map);
	id_cnt_ = reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(uid_idx_map) + 4);
	ids_    = reinterpret_cast<uint32_t*>(usr_id_map);
}

int Cregact::create_user_id(uint32_t& user_id)
{
	int ret = lockf(uid_idx_fd, F_LOCK, 4);
	DEBUG_LOG("===create %d %d", ret, uid_idx_fd);

	uint32_t idx = (*id_idx_)++;

	//lockf(uid_idx_fd, F_ULOCK, 4);

	if (idx < *id_cnt_) {
		user_id = ids_[idx];

		DEBUG_LOG("GET USER ID SUCCESSFULLY\t[uid=%u idx=%d]", user_id, idx);
		lockf(uid_idx_fd, F_ULOCK, 4);
		return 0;
	}

	lockf(uid_idx_fd, F_ULOCK, 4);
	// lack of user id
	ALERT_LOG("Lack Of User ID\t[idx=%d total=%d]", idx, *id_cnt_);
	return -1;
}

int Cregact::cancel_user_id(uint32_t user_id)
{
	char buf[20];
	int  len = snprintf(buf, sizeof buf, "%u\n", user_id);

	write(unreg_id_fd, buf, len);

	DEBUG_LOG("SAVE UNREG USER ID\t[%u]", user_id);
	return 0;
}

int Cregact::gen_reg_act_code (uint32_t user_id, uint32_t time, uint32_t cnt, uint32_t rand, char *key, char *act)
{
	uint8_t buf1[16], buf2[16], buf3[33];

	memcpy (buf1, (uint8_t *)&rand+2, 2);
	memcpy (buf1+2, (uint8_t *)&cnt, 2);
	memcpy (buf1+4, (uint8_t *)&time+2, 2);
	memcpy (buf1+6, (uint8_t *)&rand, 2);
	memcpy (buf1+8, (uint8_t *)&user_id+2, 2);
	memcpy (buf1+10, (uint8_t *)&time, 2);
	memcpy (buf1+12, (uint8_t *)&user_id, 2);
	memcpy (buf1+14, (uint8_t *)&cnt+2, 2);

	//DES_n (key, (char *)buf1, (char *)buf2, 2);
	des_encrypt_n(key, (char *)buf1, (char *)buf2, 2);
	hex2str ((char *)buf2, 16, (char *)buf3);
	cpDecodeBase64 (buf3, (uint8_t *)act, 32); //32 ---> 24

	return (0);
}

/*
 * uesr_id	key	time key
 */

int Cregact::gen_mole_emissary_code (uint32_t user_id, char *act)
{
	uint8_t	buf1[16], buf2[16];
	uint32_t tm, rand;

	tm = time(NULL);
	rand = MOLE_EMISSARY_RAND;

	memcpy(buf1, (uint8_t *)&user_id, 4);
	memcpy(buf1+4, (uint8_t *)&rand, 4);
	memcpy(buf1+8, (uint8_t *)&tm, 4);
	memcpy(buf1+12, (uint8_t *)&rand, 4);

	//DES_n (MOLE_EMISSARY_KEY, (char *)buf1, (char *)buf2, 2);
	des_encrypt_n(MOLE_EMISSARY_KEY, (char *)buf1, (char *)buf2, 2);
	hex2str ((char *)buf2, 16, (char *)act);
	//cpDecodeBase64 (buf3, (uint8_t *)act, 32); //32 ---> 24

	return 0;
}

int Cregact::send_email(const char *email, const char *send_email_cmd, uint32_t user_id, const char *nick)
{
	char cmd[2 * SEND_EMAIL_CMD_LEN];
	char active_code[2*sizeof(STRU_ACTIVE_KEY)];

	memset(active_code, 0x00, sizeof(active_code)); //need !
	generate_active_code(user_id, active_code);

	char *p=strchr (nick, '>'); //add in 20080630 
	if (p) *p=0x00;

	snprintf(cmd, sizeof (cmd), "perl %s %64.64s %48.48s %u %s  & >/dev/null 2>&1 ", \
			send_email_cmd, email, active_code, user_id, nick);

#ifdef INNER_DEBUG
	LOG(7, "The active code of user id [%u] is [%48.48s]", user_id, active_code);
	LOG(7, "cmd[%s]", cmd);
#endif
	system(cmd);

	return (0);
}

int Cregact::send_mole_emissary_email(const char *email, const char *email_cmd, uint32_t user_id)
{
	char cmd[2 * SEND_EMAIL_CMD_LEN];
	char mole_emissary_code[32];

	memset(mole_emissary_code, 0x00, sizeof(mole_emissary_code)); //need !
	gen_mole_emissary_code (user_id, mole_emissary_code);

	snprintf(cmd, sizeof (cmd), "%s  %64.64s  %u %32.32s & >/dev/null 2>&1 ", \
				email_cmd, email, user_id, mole_emissary_code);

#ifdef INNER_DEBUG
	LOG(7, "The mole active code of user id [%u] is [%32.32s]", user_id, mole_emissary_code);
	LOG(7, "cmd[%s]", cmd);
#endif
	system(cmd);

	return (0);
}

int Cregact::generate_active_code(uint32_t user_id, char *active_code)   //test !
{
	STRU_ACTIVE_KEY ak;
	char tepCode[sizeof (STRU_ACTIVE_KEY)];

	ak.user_id=user_id;
	ak.pri_key1=FILLKEY1;
	ak.pri_key2=FILLKEY2;
	ak.pri_key3=FILLKEY3;
	ak.now=time(NULL);
	ak.now_next=ak.now+TIME_NEXT_SEP;

	//DES_n(ACT_KEY, (char*)&ak, tepCode, 3); //24 bytes
	des_encrypt_n(ACT_KEY, (char*)&ak, tepCode, 3); //24 bytes
	hex2str(tepCode, sizeof(STRU_ACTIVE_KEY), active_code);

	return (0);
}

int Cregact::verify_active_code (uint32_t user_id, char *active_code)
{
	STRU_ACTIVE_KEY ak;
	char buf1[sizeof (STRU_ACTIVE_KEY)];

	str2hex (active_code, 2*sizeof (buf1), buf1);
	//_DES_n (ACT_KEY, buf1, (char *)&ak, sizeof (buf1)/8);
	des_decrypt_n(ACT_KEY, buf1, (char *)&ak, sizeof (buf1)/8);

	//verify here
	if (ak.user_id != user_id) {
#ifdef INNER_DEBUG
		LOG (7, "user id error [%u] [%u]", ak.user_id, user_id);
#endif
		return (-1);
	}
	if (time (NULL) - ak.now > TIME_SEP_WEEK) {
#ifdef INNER_DEBUG
		LOG (7, "time is over [%u] [%u]", ak.now, time(NULL));
#endif
		return (-1);
	}
	if ((ak.pri_key1 != FILLKEY1) || (ak.pri_key2 != FILLKEY2) || (ak.pri_key3 != FILLKEY3)) {
#ifdef INNER_DEBUG
		LOG (7, "fill key is error");
#endif
		return (-1);
	}

	return (0);
}

