/*
 * =====================================================================================
 *       Filename:  Cutil.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/13/2008 02:47:50 PM EST
 *       Revision:  none
 *       Compiler:  gcc
 *        Company:  TAOMEE
 * 		
 * 		------------------------------------------------------------
 * 		view configure:
 * 			VIM:  set tabstop=4 
 * 		
 * =====================================================================================
 */



#include "Cutil.h"
//#include "cpub.h"
#include "pub.h"
#include <errno.h>
#include <iostream>
using std::cerr;
using std::endl;



Cutil::Cutil()
{
	config_path_name=NULL;
	config = NULL;
}

Cutil::Cutil(char *path_name)
{
	init_config_stru(path_name);
}

int Cutil::init_config_stru( char *path_name)
{
	FILE *fp;
	int len;
	char *alTmp;
	char alBuf[1024], aKey[256], aVal[256];
	CONF_STRU_P p, q;

	config = new CONF_STRU();
	p = config;
	if ((fp = fopen(path_name, "r")) == NULL) {
		cerr<<"Open the config file ["<<path_name<<"] error : "<<strerror(errno)<<endl;
		return (-1);
	}

	while (fgets(alBuf, sizeof(alBuf), fp) != NULL){
		Trim(alBuf);
		if (strlen(alBuf) == 0 || alBuf[0] == '#') continue;
		if ((alTmp=strchr(alBuf, '#'))!=NULL) *alTmp = 0x00; //add in 20080304 by spark
		sscanf(alBuf,"%s %s",aKey,aVal);

		q = new CONF_STRU();
		len = strlen(aKey)+1;
		q->Key = new char[len];  
		memcpy(q->Key, aKey, len-1);
		*(q->Key+len-1)=0x00;
		len = strlen(aVal)+1;
		q->Val = new char[len];
		memcpy(q->Val, aVal, len-1);
		*(q->Val+len-1) = 0x00;
		q->next = NULL;
		p->next = q;
		p=q;
	}
	return (0);
}

int Cutil::read_config_file(char *path_name)
{
	destroy_config_stru();
	return (this->init_config_stru(path_name));
}


Cutil::~Cutil()
{
		destroy_config_stru();
}

void Cutil::destroy_config_stru()
{
		if(config) {
			CONF_STRU_P p, q;
			p = config;
			while(p) {
				q = p;
				p = q->next;
				delete q->Key;
				delete q->Val;
				delete q;
		   	}
		}
}


char *Cutil::get_config_str(const char *aKey) const
{
	CONF_STRU_P p;

	if (config)
		p = config->next;
	
	while(p) {
		if (strcmp(p->Key, aKey))
			p=p->next;
		else
			break;
	}
	if (p) {
		return (p->Val);
	}

	return (NULL);
}

char *Cutil::get_config_strval(const char *aKey)
{
			return (get_config_str(aKey));
}

int Cutil::get_config_intval(const char *aKey)
{
			return (atoi(get_config_str(aKey)));
}

int Cutil::print() const
{
	CONF_STRU_P p;

	if (config) p = config->next;

	while(p != NULL) {
		p = p->next;
	}

	return (0);
}

