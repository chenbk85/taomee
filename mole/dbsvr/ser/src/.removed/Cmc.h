/*
 * =====================================================================================
 * 
 *       Filename:  Cmc.h
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

#ifndef  CMC_INC
#define  CMC_INC
#include <map>
#include <string.h>
#include <stdint.h>
#include <memcache.h>

typedef struct cache_key {
	uint16_t	cmdid;
	uint32_t	userid;
} __attribute__((packed)) CACHE_KEY ;

using namespace std;
typedef map<uint16_t, uint16_t>  CACHE_CMDID_MAP;
class Cmc
{

  public:
	char key[30];
	int keylen;

    /* ====================  LIFECYCLE   ========================================= */
	Cmc (bool enabled );
    ~Cmc ();  /* constructor */
  bool get( char *recvbuf,  char **sendbuf, int *sndlen );
  bool add( char *recvbuf,  char **sendbuf, int *sndlen );
  bool del( char *recvbuf);
  inline bool isEnabled(){ return this->enabled; }
	
  protected:
  	void setkey(char *recvbuf);
	bool enabled;
  private:
	map<uint16_t, uint16_t> cache_cmdid_map;
	struct memcache *mc;
}; /* -----  end of class  Cmc  ----- */

#endif   /* ----- #ifndef CMC_INC  ----- */

