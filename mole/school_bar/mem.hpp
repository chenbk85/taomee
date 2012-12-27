// =====================================================================================
// 
//       Filename:  mem.hpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  07/07/2010 04:23:27 PM CST
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  xcwen (xcwen), jim@taomee.com
//        Company:  TAOMEE
// 
// =====================================================================================

#ifndef  MEM_INC
#define  MEM_INC
extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/timer.h>
#include <async_serv/net_if.h>
}

enum {
	data_size_128 = 0,
	data_size_256,
	data_size_512,
	data_size_1024,
	data_size_2048,
	max_data_size,
};

typedef struct mem_block {
	int type;
	int size;
	uint32_t uid;
	uint32_t cmd;
	list_head_t list;
	uint8_t *data;
} mem_block_t;

struct mem_heap {
	list_head_t head;
	int block_num;
};

class Mem {

	public:
		Mem()
		{

		}
		~Mem()
		{

		}

		int mem_init();
		/*从未使用链表中得到空闲的对象*/
		struct mem_block *get_free_block(int len);
		/*从已使用链表中得到已经使用的对象*/
		struct mem_block *get_used_block();
		/*释放多余的空闲链表，以节约内存*/
		int release_surplus_free_block();
		/*从新保存到使用队列中*/
		void back_to_used_heap(mem_block_t *ptr);
		/*保存数据到文件中*/
		void save_used_heap();
	private:
		struct mem_block *mem_block_new(int len);

		int release_mem_block(struct mem_heap *header);

		struct mem_block *get_free_block_from_free_heap(int len);

		struct mem_block *get_used_block_from_used_heap();

		int get_malloc_size(int len, int *type);
	private:
		/*保存空链表的最小数目*/
		int min_block_num;
		
		/*空闲链表和使用中链表的数目比率*/
		int free_div_used;

		/*保存拥有数据的链表*/
		struct mem_heap used_block;

		/*保存空链表*/
		struct mem_heap free_block[max_data_size];
};

#endif   // ----- #ifndef MEM_INC  ----- 

