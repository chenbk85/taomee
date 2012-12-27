#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "Cuser_ip_map.h"
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "msglog.h"
#include <errno.h>

Cuser_ip_map::Cuser_ip_map(){
	char map_filename[255];
	for (int i=0;i<GAME_MAX_COUNT ;i++ ){
		sprintf(map_filename, "../ip_info/ip.info_%02d",i)	;
		this->ip_map[i].set_info(map_filename ,200000000 );
	}

}

int Cuser_ip_map::add_ip(userid_t userid,uint32_t gameid , uint32_t ip  )
{
	stru_ip_item *p_item;		 
	if (gameid>=GAME_MAX_COUNT) 
		return NO_DEFINE_ERR;
	if ((p_item= this->ip_map[gameid].get_data_point(userid))!=NULL){
		p_item->add_ip(ip );
		return SUCC;
	}
	return NO_DEFINE_ERR;
}
int Cuser_ip_map::get_ip_item(userid_t userid,uint32_t gameid ,stru_ip_item  *p_out)
{
	stru_ip_item *p_item;		 
	if (gameid>=GAME_MAX_COUNT) 
		return NO_DEFINE_ERR;
	if ((p_item= this->ip_map[gameid].get_data_point(userid))!=NULL){
		*p_out=*p_item;
		return SUCC;
	}
	return NO_DEFINE_ERR;
}
