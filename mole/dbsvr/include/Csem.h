/*
 * =====================================================================================
 * 
 *       Filename:  Csem.h
 * 
 *    Description: 
 * 
 *        Version:  1.0
 *        Created:  2007年12月29日 09时27分18秒 CST
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

#ifndef  CSEM_INC
#define  CSEM_INC
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

/*
 * =====================================================================================
 *        Class:  Csem
 *  Description:  
 * =====================================================================================
 */

class Csem
{

  public:
    Csem ();  /* constructor */
    ~Csem (); 
	bool lock();
	bool unlock();
	bool setvalue( int value);  
	bool remove();  
	int  getvalue();  
	bool init(key_t key);
  protected:
  private:
	int semid;
}; /* -----  end of class  Csem  ----- */

#endif   /* ----- #ifndef CSEM_INC  ----- */

