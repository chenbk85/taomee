/*
 * =====================================================================================
 *
 *       Filename:  mmap.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/25/2010 01:54:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  maxwell (zdl), maxwell@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef MMAP_H
#define MMAP_H

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <assert.h>
}

struct mmap_content{
	uint32_t index;
};

class Cmmap{
	private:
		struct mmap_content rec_content;
    	struct mmap_content *p_mapped_memory_addr;
	public:
		Cmmap();
		~Cmmap();
		uint32_t init();
		uint32_t get_index();
};

#endif
