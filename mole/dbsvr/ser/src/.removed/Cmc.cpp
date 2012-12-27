/*
 * =====================================================================================
 *
 *       Filename:  Cmc.cpp
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
#include "Cmc.h"
#include "proto.h"
#include "benchapi.h"
#include <stdio.h>
Cmc::Cmc (bool enabled )
{

	this->enabled=enabled;	
	if (enabled) {
		this->mc= mc_new();
		mc_server_add4(mc, "localhost:11211");
		#define CACHE_CMDID_BIND(cmd1,cmd2) \
		this->cache_cmdid_map.insert(CACHE_CMDID_MAP::value_type (cmd1,cmd2 ) );

		//home 
		CACHE_CMDID_BIND(USER_GET_HOME_CMD , USER_GET_HOME_CMD  );
		CACHE_CMDID_BIND(USER_SET_HOME_ATTIRELIST_CMD, USER_GET_HOME_CMD  );
		CACHE_CMDID_BIND(USER_SET_HOMEMAP_CMD , USER_GET_HOME_CMD  );
	}else{
		this->mc=NULL;
	}
	//this->cache_cmdid_map.equal_range();
}  /* -----  end of method Cmc::Cmc  (constructor)  ----- */

Cmc::~Cmc ()
{
	if (this->mc) free(this->mc);
}  /* -----  end of method Cmc::Cmc  (constructor)  ----- */

void Cmc::setkey(char *recvbuf)
{
	sprintf(key,"%X%d",
		this->cache_cmdid_map.at(((PROTO_HEADER*)recvbuf)->cmd_id),
			((PROTO_HEADER*)recvbuf)->id);
	this->keylen=strlen(key);
}

bool Cmc::add( char *recvbuf,  char **sendbuf, int *sndlen )
{
	this->setkey(recvbuf);
	return !bool(mc_set(this->mc,this->key,keylen, 
			*sendbuf, *sndlen,0,0 ));
}

bool Cmc::del( char *recvbuf)
{
	this->setkey(recvbuf);
	return  !bool(mc_delete(this->mc,this->key,keylen,0));
}

bool Cmc::get( char *recvbuf,  char **sendbuf, int *sndlen )
{
	this->setkey(recvbuf);
	*sendbuf = (char* )mc_aget2(this->mc, (char*)(&key), 
			keylen, (size_t *)sndlen);
	if (*sendbuf !=NULL){
		((PROTO_HEADER*)*sendbuf)->proto_id=((PROTO_HEADER*)recvbuf)->proto_id; 
	}
	return bool(*sendbuf);//NULL return false
}

