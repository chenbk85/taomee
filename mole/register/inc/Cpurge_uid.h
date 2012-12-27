/*
 * =====================================================================================
 * 
 *       Filename:  Cpurge_uid.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  10/10/2009 10:55:20 AM CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  Miller (Miller), nsnwz@126.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CPURGE_UID_INC
#define  CPURGE_UID_INC
	int del_used_id(void *uid_idx_map, int usr_id_fd, void *usr_id_map);

	int cat_unreg_to_id(int unreg_fd, int id_fd);

	int update_idx(void *uid_idx_fd, int usr_id_fd);

	int copy_file(int fd, char *dst);

	int get_date(time_t t); 
#endif   /* ----- #ifndef CPURGE_UID_INC  ----- */

