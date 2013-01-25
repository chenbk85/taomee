/*
 * =====================================================================================
 *
 *       Filename:  test.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/25/2010 01:12:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  maxwell (zdl), maxwell@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef _TEST_H_
#define _TEST_H_
extern "C"{     
                
     
#include <iter_serv/dll.h>
 
int  init_service();
int  fini_service();
void proc_events();
 
int  get_pkg_len(int fd, const void* pkg, int pkglen);
int on_pkg_received(int sockfd, void* pkg, int pkglen);    

}
#endif

