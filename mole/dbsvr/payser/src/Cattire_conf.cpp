/*
 * =====================================================================================
 *
 *       Filename:  Cattire_conf.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2008年01月14日 09时53分26秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 		
 * 		------------------------------------------------------------
 * 		view configure:
 * 			VIM:  set tabstop=4 
 * 		
 * =====================================================================================
 */

#include <algorithm>
using namespace std;
#include "Cattire_conf.h"
#include "benchapi.h"

Cattire_conf::Cattire_conf (int ashm_key)
{
	if((this->shmid = shmget( ashm_key,SHM_SIZE , IPC_CREAT | 0660 )) != -1)
	{
		this->shm_pos=shmat(shmid , 0, 0);
	}
	/*
	ATTIRE_INFO* a=(ATTIRE_INFO * )((char *)shm_pos+4);
	printf("count:%u \n",*((uint32_t*)shm_pos) );
	for (int i=0; (uint32_t)i< *((uint32_t*)shm_pos);i++){
		printf("item[%d]:%u %u \n",i,(a+i)->id, (a+i)->price );
	}
	*/

	
}  /* -----  end of method Cattire_conf::Cattire_conf  (constructor)  ----- */

int Cattire_conf::get_price( uint32_t id, uint32_t * p_price )
{
	uint32_t count; 
	ATTIRE_INFO  * p_attire_info;
	ATTIRE_INFO   a_info;
	pair< ATTIRE_INFO  * , ATTIRE_INFO  *	> range;
	
	if (this->shmid== -1){
		return SYS_ERR;
	}
	a_info.id=id;
	a_info.price=0;
	count=*((uint32_t*)this->shm_pos);
	p_attire_info=(ATTIRE_INFO*)((char *)(this->shm_pos)+4 );

	//二分查找
	range=equal_range(p_attire_info,p_attire_info+count , a_info  );
	if (range.first->id==a_info.id ) {
		*p_price=range.first->price;			
		return SUCC;
	}else 
		return   PAY_ATTITE_ID_NOFIND_ERR ;

}

