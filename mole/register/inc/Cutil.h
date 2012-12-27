/*
 * =====================================================================================
 * 
 *       Filename:  Cutil.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  01/13/2008 02:28:35 PM EST
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

#include "global.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef  CUTIL_INC
#define  CUTIL_INC

typedef struct conf_stru{
	char *Key;
	char *Val;
	conf_stru *next;
}CONF_STRU, *CONF_STRU_P;

class Cutil{
	private:
		char *config_path_name; //a full path name
		CONF_STRU_P config;
	protected:
		char *get_config_str(const char *aKey) const;
		int init_config_stru(char *path_name);
		void destroy_config_stru();
	public:
		int read_config_file(char *path_name);  //if want to use this object second time
		char *get_config_strval(const char *aKey);
		int get_config_intval(const char *aKey);
		int print() const; //test
		Cutil();
		Cutil(char *path_name);
		~Cutil();
};
#endif   /* ----- #ifndef CUTIL_INC  ----- */

