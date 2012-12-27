#ifndef CUSERLOGIN_H
#define CUSERLOGIN_H

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

template <class _Tp > 
class Citemlist{
	public:
		typedef _Tp  T_ITEM  ;

	private:	
		vector< T_ITEM > v_list;
		int fd;
		int opt_date;
		char filename_fix[256];
		size_t write_count_once;
	public:
//		typedef typename vector<T_ITEM >::iterator  iterator;
		Citemlist(){
			this->write_count_once= 4096/sizeof(T_ITEM)-1 ;
			this->fd=-1;
			opt_date=0;
			this->filename_fix[0]='\0';
			write_count_once=200;
		}

		void set_filename_fix(char * value) {
			strcpy(this->filename_fix,value );
		}

		void write_file(){
			int now=get_date(time(NULL));
			if (this->v_list.size()==0){
				return;				
			}
			char filename[256];
			if(this->fd==-1 || opt_date!=now ){
				close(this->fd);
				sprintf(filename,"%s_%08d",this->filename_fix ,now );
				this->fd = open(filename,O_CREAT|O_APPEND|O_RDWR,0777);
				if (this->fd==-1){
					ERROR_LOG("write file err:%s",filename );
					return;
				}
			}			
			//write
			if (write(fd,&(this->v_list[0]),
						this->v_list.size()* sizeof(this->v_list[0]))==-1 ){
				close(fd);
				this->fd=-1;
			}
			this->v_list.clear();	
		}

		void add(const T_ITEM &item){
			this->v_list.push_back(item);	
			if (this->v_list.size()>this->write_count_once){//写到文件中
				this->write_file();
			}
		}
};



#define FD_MAX 100
class Clogin_info{
	public:
		Clogin_info();
			
		int add(userid_t userid,  ff_login_add_in *p_in );

		int get_list(userid_t userid, ff_login_get_list_in*p_in,
	 		ff_login_get_list_out_item	**pp_out_item,uint32_t *p_out_count);
	private:
		int last_write_time;
		int write_step_time;//每次写文件最长的时间间隔.
		Citemlist< stru_login_info > itemlist[FD_MAX];
		int get_read_fd(int index, uint32_t login_date);
};

#endif
