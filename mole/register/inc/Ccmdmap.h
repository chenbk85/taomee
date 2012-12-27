/*
 * =====================================================================================
 * 
 *       Filename:  Ccmdmap.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  01/13/2008 09:28:54 AM EST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *        Company:  TAOMEE
 *
 *      ------------------------------------------------------------
 *      view configure:
 *          VIM:  set tabstop=4  
 * 
 * =====================================================================================
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#ifndef _CMDMAP_H_
#define _CMDMAP_H_

class Cregdeal;  //cannot include Cregdeal head file here, only use the pointer of the type !

typedef int (Cregdeal::*P_DEALFUN)(char *recvbuf, int rcvlen,char **sendbuf, int *sndlen);

class Ccmdmap {
		private:
			uint32_t cmd_id;
			P_DEALFUN p_dealfun;
		public:
			inline void set(uint32_t cmd_id, P_DEALFUN p_dealfun) {
				this->cmd_id = cmd_id;
				this->p_dealfun = p_dealfun;
			}
			inline uint32_t get_cmd_id() {
				return (this->cmd_id);
			}
			inline P_DEALFUN get_fun() {
				return (this->p_dealfun);
			}
			inline bool operator==(const Ccmdmap & other) {
				if (this->cmd_id == other.cmd_id) {
					return (true);
				}else {
					return (false);
				}
			}
			inline bool operator>(const Ccmdmap & other) {
				if (this->cmd_id > other.cmd_id) {
					return (true);
				}else {
					return (false);
				}
			}
			inline bool operator<(const Ccmdmap & other) {
				if (this->cmd_id < other.cmd_id) {
					return (true);
				}else {
					return (false);
				}
			}

};
#endif   /* ----- #ifndef CCMDMAP_INC  ----- */

