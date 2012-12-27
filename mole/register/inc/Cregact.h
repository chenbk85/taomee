/*
 * =====================================================================================
 * 
 *       Filename:  Cregact.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  02/20/2008 03:21:01 PM EST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  spark (spark), zhezhide@163.com
 *        Company:  TAOMEE
 *
 *      ------------------------------------------------------------
 *      view configure:
 *          VIM:  set tabstop=4  
 * 
 * =====================================================================================
 */

#include <stdint.h>
#include <sys/types.h>
#include <arpa/inet.h>

#ifndef  CREGACT_INC
#define  CREGACT_INC

/*
 * this class offers the common operations for all users
 * so should supply some pre-data for the proceed
 */

class Cregact {
private:
	uint32_t*        id_idx_;
	const uint32_t*  id_cnt_;
	const uint32_t*  ids_;
protected:
	int generate_active_code (uint32_t user_id, char *active_code);
	int verify_active_code (uint32_t user_id, char *active_code);
	int gen_reg_act_code (uint32_t user_id, uint32_t time, uint32_t cnt, uint32_t rand, char *key, char *act);

	int gen_mole_emissary_code (uint32_t user_id, char *act); //24 bytes
public:
	int create_user_id (uint32_t &user_id);
	int cancel_user_id (uint32_t user_id);

	int send_email (const char *email,const char *send_email_cmd,  uint32_t user_id, const char *nick);
	int send_mole_emissary_email (const char *email, const char *snd_email, uint32_t user_id);

	Cregact(void* usr_id_map, void* uid_idx_map);
};
#endif   /* ----- #ifndef CREGACT_INC  ----- */

