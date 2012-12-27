#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "Clogin_info.h"
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "msglog.h"
#include <errno.h>

Clogin_info::Clogin_info(){
	this->last_write_time=time(NULL);
	this->write_step_time=300;
	char filename[255];
	for(int i=0;i<FD_MAX;i++){//处理
		sprintf(filename,"../login_info/login_%02d",i);
		this->itemlist[i].set_filename_fix(filename );
	}
}

int Clogin_info::add(userid_t userid,  ff_login_add_in *p_in )
{
	int	index=userid%FD_MAX;
	stru_login_info item;	
	item.userid=userid;
	item.loginflag=p_in->loginflag;
	item.onlineid=p_in->onlineid;
	item.logintime=p_in->logintime;
	this->itemlist[index].add(item);
	if(time(NULL)-this->last_write_time>this->write_step_time ){
		for(int i=0;i<FD_MAX;i++ )	{
			this->itemlist[i].write_file();
		}
		this->last_write_time=time(NULL);
	}
	return SUCC;
}

int Clogin_info::get_list(userid_t userid, ff_login_get_list_in*p_in,
	 ff_login_get_list_out_item	**pp_out_item,uint32_t *p_out_count)
{
	
	int ret=SUCC;
	int fd_index = userid%FD_MAX;
	uint32_t read_max_count=300;//最多读取300个
	int read_len_once=sizeof(typeof(**pp_out_item))*read_max_count;

	//打开读文件
	int read_fd = get_read_fd(fd_index,p_in->login_date);
	if(read_fd==-1){
        DEBUG_LOG("open read file error=[%m]!" );
		return NO_DEFINE_ERR;
	}
    
	//分配内存
	if((*pp_out_item=(typeof(*pp_out_item))malloc(read_len_once ))==NULL){
		DEBUG_LOG("malloc failed while reading from file!");
		return SYS_ERR;
	}

	char read_buff[4096*2];
    ff_login_get_list_out_item *p_item=(ff_login_get_list_out_item *)read_buff;
	*p_out_count=0;
	//先将数据读入缓存
	int read_len,read_count;
	do{
		DEBUG_LOG("========2==1=%d",read_len_once  );
		read_len= read(read_fd,read_buff,read_len_once );
		DEBUG_LOG("=====3333==");
		read_count=read_len/sizeof(typeof(**pp_out_item));
		DEBUG_LOG("read info %d,%d,%u,%u",read_len,read_count,p_item[0].userid,userid);
			
		for (int i=0;i<read_count;i++){
			if (p_item[i].userid==userid){
				if(*p_out_count<read_max_count){
					(*pp_out_item)[*p_out_count]=p_item[i];	
					(*p_out_count)++;
				}else{
					break;
				}	
			}
		}
	} while(read_count > 0  );

	close(read_fd);
	return ret;
}

int Clogin_info::get_read_fd(int index, uint32_t login_date){
	int read_fd;
	char filename[50];
	sprintf(filename,"../login_info/login_%02d_%08d",index, login_date);
	read_fd = open(filename,O_RDONLY);
	return read_fd;
}
