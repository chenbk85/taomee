/*
 * =====================================================================================
 *
 *       Filename:  mmap.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/25/2010 05:09:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  maxwell (zdl), maxwell@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include"mmap.h"
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#define SIZE 1

Cmmap::Cmmap(){
}

uint32_t Cmmap::init(){
	
	int fd;
	fd = open("records", O_RDWR);
    if(ftruncate(fd, SIZE*sizeof(*p_mapped_memory_addr)) == -1){
		ERROR_LOG("mmap ftruncate err...");
	}
    //DEBUG_LOG("===fd:%d", fd);
    this->p_mapped_memory_addr = (struct mmap_content *)mmap(0, SIZE*sizeof(*p_mapped_memory_addr), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(MAP_FAILED == p_mapped_memory_addr){
        ERROR_LOG("mmap get err...");
        return 1;
    }
	return 0;
}

Cmmap::~Cmmap(){
    //将修改写回映射文件中(采用异步写方式MS_ASYNC)(同步写:MS_SYNC)
    msync((void *)p_mapped_memory_addr, SIZE * sizeof(*p_mapped_memory_addr), MS_SYNC);	
    //释放内存段
    munmap((void *)p_mapped_memory_addr, SIZE * sizeof(*p_mapped_memory_addr));
}

uint32_t Cmmap::get_index(){
	uint32_t res;
	res = ++p_mapped_memory_addr[0].index;
	return res;
}





