/*
 * =====================================================================================
 * 
 *       Filename:  Cattire_conf.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年01月14日 09时53分27秒 CST
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

#ifndef  CATTIRE_CONF_INC
#define  CATTIRE_CONF_INC

#define ATTIRE_INFO_MAXNUM 10000
#define SHM_SIZE 	(sizeof(ATTIRE_INFO)* 10000+4) 
/*
 * =====================================================================================
 *        Class:  Cattire_conf
 *  Description:  
 * =====================================================================================
 */
#include <stdint.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "common.h"


typedef struct attire_info{
	uint32_t id; 
	uint32_t price; 
	inline bool operator == ( const struct attire_info & other  ) 
	{
		return this->id==other.id ;
	}

	inline  bool operator <(const  struct attire_info & other  ) const {

		return this->id<other.id ;

	}
	inline  bool operator >(const struct attire_info  & other  ) const {
		return this->id>other.id ;
	}


} __attribute__((packed)) ATTIRE_INFO;

class Cattire_conf
{

  public:

    /* ====================  LIFECYCLE   ========================================= */

    Cattire_conf (int ashm_key);  /* constructor */
	int get_price( uint32_t id, uint32_t * p_price );
  protected:

  private:
	int shmid;
	//uint32_t * p_count ;
	//ATTIRE_INFO  * p_attire_info ;
	void * shm_pos; 

}; /* -----  end of class  Cattire_conf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Cattire_conf
 *      Method:  Cattire_conf
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
#endif   /* ----- #ifndef CATTIRE_CONF_INC  ----- */

