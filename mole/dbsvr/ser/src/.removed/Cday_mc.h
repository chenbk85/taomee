/*
 * =====================================================================================
 * 
 *       Filename:  Cday_mc.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年03月24日 11时40分12秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 *      ------------------------------------------------------------
 *      view configure:
 *          VIM:  set tabstop=4  
 * 
 * =====================================================================================
 */

#ifndef  CDAY_MC
#define  CDAY_MC
#include <map>
#include <string.h>
#include <stdint.h>
#include <memcache.h>

typedef struct  	key_type_user{
	uint32_t	type;
	uint32_t	userid;
} __attribute__((packed))	KEY_TYPE_USER;

class Cday_mc
{

  public:
    /* ====================  LIFECYCLE   ========================================= */
	Cday_mc ( char *  mc_server   );
    ~Cday_mc ();  /* constructor */
	int get_value(KEY_TYPE_USER *key , uint32_t *p_value );
	int add_value(KEY_TYPE_USER *key , uint32_t maxvalue  );
  protected:
	bool get( uint32_t *p_value);
	int set( uint32_t *p_value);
	void setkey(KEY_TYPE_USER *p_key);
  private:
	char key_str[30];
	uint32_t keylen;
	struct memcache *mc;
}; /* -----  end of class  Cday_mc  ----- */

#endif   /* ----- #ifndef CDAY_MC  ----- */

