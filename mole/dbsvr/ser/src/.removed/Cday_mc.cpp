/*
 * =====================================================================================
 *
 *       Filename:  Cday_mc.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2008年03月24日 11时40分11秒 CST
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
#include "Cday_mc.h"
#include "proto.h"
#include "benchapi.h"
#include <stdio.h>
#include <errno.h>
#include "common.h" 
Cday_mc::Cday_mc (char * mc_server)
{
	this->mc= mc_new();
	mc_server_add4(mc, mc_server );
	//this->cache_cmdid_map.equal_range();
}  /* -----  end of method Cday_mc::Cday_mc  (constructor)  ----- */

Cday_mc::~Cday_mc ()
{
	if (this->mc) free(this->mc);
}  /* -----  end of method Cday_mc::Cday_mc  (constructor)  ----- */

void Cday_mc::setkey(KEY_TYPE_USER *p_key)
{
	sprintf(key_str,"%d:%d",p_key->userid,p_key->type);
	this->keylen=strlen(key_str);
	//memcpy(key_str,p_key,sizeof(*p_key) );
	//this->keylen=sizeof(*p_key);
}

bool Cday_mc::get( uint32_t *p_value)
{
	char * sendbuf;
	sendbuf = (char* )mc_aget(this->mc,this->key_str, this->keylen );
	if (errno==103){//没有连接
		mc_server_activate_all(mc);	
		sendbuf = (char* )mc_aget(this->mc,this->key_str, this->keylen );
	}
	if (sendbuf !=NULL){
		*p_value=*((uint32_t*)(sendbuf));
		free(sendbuf);
	}else{
		*p_value=0;
	}
	return true;
}


int Cday_mc::set(uint32_t *p_value)
{
	DEBUG_LOG("SETMC-%s-%u",this->key_str,*p_value );
	return mc_set(this->mc,this->key_str,this->keylen,
			p_value , sizeof(*p_value) ,0,0 );
}

int Cday_mc::add_value(KEY_TYPE_USER *key , uint32_t maxvalue  )
{
	uint32_t	value;
	this->setkey(key);
	this->get(&value);			
	value++;
	if (value>maxvalue){
		DEBUG_LOG("value:%u;maxvalue:%u",value,maxvalue );
		return EMAIL_EXISTED_ERR;
	}
	//DEBUG_LOG("mc==%d",value);
	int ret=this->set(&value);
	if (ret!=0){
		DEBUG_LOG("==add err:%d",ret);
	}
	return SUCC;
}

int Cday_mc::get_value(KEY_TYPE_USER *key , uint32_t *p_value )
{
	this->setkey(key);
	this->get(p_value);			
	return SUCC;
}
