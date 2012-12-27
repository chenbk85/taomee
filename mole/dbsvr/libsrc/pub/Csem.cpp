/*
 * =====================================================================================
 *
 *       Filename:  Csem.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年12月29日 09时23分54秒 CST
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

#include "Csem.h"
#define IFLAGS (IPC_CREAT | IPC_EXCL)

static  struct sembuf stru_P={ 0,-1, 0 };		
static  struct sembuf stru_V={ 0, 1, 0 };		
/*
 *--------------------------------------------------------------------------------------
 *       Class:  Csem
 *      Method:  Csem
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

Csem::Csem ()  /* constructor */
{
}  /* -----  end of method Csem::Csem  (constructor)  ----- */


bool Csem::init(key_t key)
{
	//init sed	
  	if ((semid = semget (key , 1 , 0600 | IFLAGS )) < 0){
		//existed for get
		if ((semid = semget (key , 1 , 0600 | IPC_CREAT )) < 0)
			return(false);
			
	}else{ //create  
		//init value
	}

	this->setvalue( 1 );

	return true;
}

Csem::~Csem ()
{
}

bool Csem::setvalue( int value )
{
	if (semctl (semid,0, SETVAL, value ) < 0){
		return(false);
	}else{
		return true;
	}
}	

bool Csem::remove()
{
	if (semctl (semid, 0,IPC_RMID, NULL) <0 ){
		return(false);
	}else{
		return true;
	}
}
bool Csem::lock()
{
	if (semop(semid,&stru_P,1)==-1)
		return(false);
	return true;
}

int Csem::getvalue()
{
	return( semctl(this->semid ,0, GETVAL, NULL));
}

bool Csem::unlock()
{
	if (semop(semid,&stru_V,1)==-1){
	 	return(false);
	}
	return true;
		
}
