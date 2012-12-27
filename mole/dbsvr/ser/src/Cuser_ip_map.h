#ifndef CUSERMAP_H
#define CUSERMAP_H

#include "proto.h"
#include "common.h"
#include "benchapi.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

#include <vector>
#include <algorithm>
using namespace std;
#include <unistd.h>
#include <sys/mman.h>


template <class _Tp > 
class Tuser_ip_map{
	public:
		typedef _Tp  T_ITEM  ;

	private:	
		int fd;
		T_ITEM * p_start;	
		uint32_t max_userid;
		char   map_filename[255];
	protected:
		inline bool check_userid(userid_t userid){
			return userid<max_userid;
		}
		bool init () {	
			this->fd = open(map_filename,O_RDWR,0777);
			if (this->fd==-1)  return false;

			this->p_start=(T_ITEM *)mmap(NULL, sizeof(T_ITEM)*max_userid ,
					 PROT_READ|PROT_WRITE ,MAP_SHARED , this->fd,0 );
			if (this->p_start==(T_ITEM  *) -1)  return false;
			return true;
		}

	public:
			
		void set_info( const char * map_filename,uint32_t max_userid_value){
			this->max_userid=max_userid_value;
			strcpy (this->map_filename,map_filename);
		}

		Tuser_ip_map(){
			fd=-1;
			p_start=(T_ITEM  *)-1;
		}

		Tuser_ip_map(char * map_filename,uint32_t max_userid ){
			this->set_info(  map_filename,max_userid);
		}


		~Tuser_ip_map(){
			if (this->p_start!=(T_ITEM* ) -1  ){
				munmap(this->p_start ,sizeof(T_ITEM)*max_userid  );
				close(this->fd);
			}
		}

		//得到数据指针，可直接对map进行处理
		T_ITEM * get_data_point(userid_t userid ){
			if (this->p_start==(T_ITEM* ) -1  ){
				if(!this->init()){
					return NULL;	
				}
			}
			if (this->check_userid(userid)){
				 return this->p_start+userid;  
			}

			return NULL;
		}
};

#define GAME_MAX_COUNT 32
class Cuser_ip_map {
	public:
		Cuser_ip_map();
		Tuser_ip_map <struct stru_ip_item> ip_map [GAME_MAX_COUNT ];
		int add_ip(userid_t userid,uint32_t gameid , uint32_t ip  );

		int get_ip_item(userid_t userid,uint32_t gameid ,stru_ip_item  *p_out);
	private:
		
};

#endif
