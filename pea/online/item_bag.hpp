#ifndef ITEM_BAG_HPP_
#define ITEM_BAG_HPP_

#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "item.hpp"
#include "proto.hpp"

typedef struct player_t player_t;

#define DEFAULT_BAG_GRID_COUNT (100)
#define INVALID_BAG_INDEX      (-1)

/////////////////// 玩家的物品包裹 ///////////////////////


struct item_bag
{
	item_bag(uint32_t max_count);
	
	~item_bag();

	bool init_bag();

	bool final_bag();

	int32_t find_empty_bag_index(uint32_t begin_index = 1);

	bool   check_bag_index_empty(uint32_t index);

	bool check_del_item(uint32_t item_id, uint32_t item_count)
	{
		uint32_t count = get_item_count(item_id);
		if(count >= item_count )return true;
		return false;
	}

	bool del_item(uint32_t item_id, uint32_t item_count)
	{
		if( !check_del_item(item_id, item_count))return false;
		
		uint32_t total_del_count = item_count;

		while(total_del_count)
		{
			item* p_item = get_first_item(item_id);	
			uint32_t cur_heap_count = p_item->get_cur_heap_count();	

			if(cur_heap_count > item_count)
			{
				p_item->dec_cur_heap_count(item_count);
				total_del_count -= item_count;
			}
			else
			{
				p_item->dec_cur_heap_count(item_count);
				total_del_count -= cur_heap_count;	

				p_item->clear_to_item();
				item_grid[p_item->bag_index_] = NULL;
				item_factory::get_instance()->destroy_item(p_item);
				p_item = NULL;
			}	
		}
		return true;
	}




	bool   check_add_item(uint32_t item_id, uint32_t item_count)
	{
		item_data* p_data = item_data_mgr::get_instance()->get_item_data_by_id(item_id);
		item* p_item = 	get_first_item(item_id);

		int32_t remain_count = item_count;
		uint32_t need_grid_count = 0;
		if(p_item == NULL)
		{
			need_grid_count = item_count / p_data->max_heap_num_  + (item_count % p_data->max_heap_num_ ? 1:0 );
		}
		else
		{
			while(p_item)
			{
				uint32_t count = p_item->get_cur_remain_heap_count();
				remain_count -= count; 
				p_item = get_next_item(p_item);	
				if(remain_count < 0)
				{
					remain_count = 0;
					break;
				}
			}
			need_grid_count = remain_count / p_data->max_heap_num_  + (remain_count % p_data->max_heap_num_ ? 1:0 );
		}
		return need_grid_count >= get_empty_grid_count();
	}

	bool check_add_item_to_empty_grid(uint32_t item_id, uint32_t item_count)
	{
		item_data* p_data = item_data_mgr::get_instance()->get_item_data_by_id(item_id);
		uint32_t need_grid_count = 0;
		need_grid_count = item_count / p_data->max_heap_num_  + (item_count % p_data->max_heap_num_ ? 1:0 );
		return need_grid_count >= get_empty_grid_count();
	}

	bool add_item_to_empty_grid(uint32_t item_id, uint32_t item_count)
	{
		item_data* p_data = item_data_mgr::get_instance()->get_item_data_by_id(item_id);
		uint32_t remain_count = item_count;
		item* p_last_item = NULL;

		while( remain_count )
		{
			int32_t  index = find_empty_bag_index();
			uint32_t cur_heap_count = remain_count / p_data->max_heap_num() ? p_data->max_heap_num() : remain_count;
			
			item* p_item = item_factory::get_instance()->create_item(0, item_id, cur_heap_count, index, 0, 0);
			item_grid[index]  = p_item;
			if(p_last_item)
			{
				p_last_item->to_next(p_item);
				p_item->to_prev(p_last_item);
				p_last_item = p_item;
			}
			else
			{
				p_last_item = p_item;
			}
			remain_count -= p_data->max_heap_num();
		}
		return true;
	}

	bool add_item(uint32_t item_id, uint32_t item_count)
	{
		if( !check_add_item(item_id, item_count))return false;

		uint32_t remain_count = item_count;	

		item* p_item =  get_first_item(item_id);

		if(p_item == NULL)
		{
			return add_item_to_empty_grid(item_id, item_count);
		}
		else
		{
			while(p_item)
			{
				uint32_t remain_heap_count = p_item->get_cur_remain_heap_count();
				if(remain_heap_count == 0)continue;
				
				if(remain_heap_count <= remain_count)
				{
					p_item->inc_cur_heap_count( remain_heap_count);	
					remain_count -= remain_heap_count;
				}
				else
				{
					p_item->inc_cur_heap_count(remain_count);	
					remain_count = 0;
					break;
				}	
				p_item = get_next_item(p_item);
			}
			if(remain_count > 0)
			{
				return add_item_to_empty_grid(item_id, remain_count);
			}	
		}
		return true;	
	}


	uint32_t  get_item_count(uint32_t item_id);

	uint32_t  get_empty_grid_count();

	item_ptr  get_first_item(uint32_t item_id);

	item_ptr get_next_item(item* p_item);

	item_ptr get_last_item(uint32_t item_id);
	
	item_ptr get_item_by_grid(uint32_t grid_index);

	bool check_valid_grid_index(uint32_t grid_index);

	bool add_item_by_grid( db_add_item_reply_t* reply);

	bool del_item_by_grid( db_del_item_reply_t* reply);

	bool set_grid_item(item* p_item, uint32_t grid_index);
	
	item_ptr*   item_grid;
	uint32_t    max_grid_count;
};


bool init_player_item_bag(player_t* p, uint32_t max_grid_count);

bool final_player_item_bag(player_t* p);
	







/////////////////// 玩家的回收包裹 ///////////////////

struct c_redeem_bag
{
    c_redeem_bag()
    {
        init();
    }

    ~c_redeem_bag()
    {
        uninit();
    }


    int init()
    {
        item_grid.clear();
        return 0;
    }

    int uninit()
    {
        vector_for_each(item_grid, it)
        {
            item_ptr p_item = *it;
            p_item->clear_to_item();
            item_factory::get_instance()->destroy_item(p_item);
        }

        item_grid.clear();
        return 0;
    }

    uint32_t get_grid_count()
    {
        return item_grid.size();
    }

    // p_item可以在调用完成之后释放
    int add_item(item_ptr p_item);

    item_ptr get_item(uint32_t index)
    {
        if (index >= get_grid_count())
        {
            return NULL;
        }

        return item_grid[index];
    }

    int del_item(uint32_t index);



    std::vector<item_ptr> item_grid;
};


#endif
