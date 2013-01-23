#include "item_bag.hpp"
#include "player.hpp"

item_bag::item_bag(uint32_t max_count)
{
	max_grid_count = max_count;

	item_grid = new item_ptr[max_grid_count + 1];

	for(uint32_t i =0; i< max_grid_count + 1; i++)
	{
		item_grid[ i ] = NULL;
	}
}

item_bag::~item_bag()
{
	delete [] item_grid;
	item_grid = NULL;
	max_grid_count = 0;
}

bool item_bag::init_bag()
{
    return true;
}

bool item_bag::final_bag()
{
    for(uint32_t i = 0; i <= max_grid_count; i++)
    {
        item_ptr p_item = item_grid[i];
        if(p_item)
        {
            item_factory::get_instance()->destroy_item(p_item);
            p_item = NULL;
        }
    }	
    return true;
}

int32_t item_bag::find_empty_bag_index(uint32_t begin_index)
{
	if( !check_valid_grid_index(begin_index))return INVALID_BAG_INDEX;
	
	for( uint32_t i = begin_index; i< max_grid_count; i++)
	{
		if(item_grid[i] == NULL)return i;	
	}
	return INVALID_BAG_INDEX;
}

bool   item_bag::check_bag_index_empty(uint32_t index)
{
	if( index >=1 && index <= max_grid_count && item_grid[index] == NULL)return true;
	return false;
}


item_ptr item_bag::get_item_by_grid(uint32_t grid_index)
{
	if(  !(grid_index >= 1 && grid_index < max_grid_count) )return NULL;
	return item_grid[grid_index];
}

bool item_bag::add_item_by_grid( db_add_item_reply_t* reply)
{
	if( !check_valid_grid_index( reply->grid_index) ) return false;

	item* p_item = get_item_by_grid(reply->grid_index);

	if(p_item == NULL)
	{
		item* new_item = item_factory::get_instance()->create_item(reply->id, reply->item_id, reply->item_count, reply->grid_index, reply->get_time, reply->expire_time);
		set_grid_item(new_item, new_item->grid_index());
	}
	else
	{
		p_item->inc_cur_heap_count(reply->item_count);	
	}
	return true;
}

bool item_bag::del_item_by_grid( db_del_item_reply_t* reply)
{
	if( !check_valid_grid_index( reply->grid_index) ) return false;

	item* p_item = get_item_by_grid(reply->grid_index);

	if(p_item == NULL)return false;

	if(p_item->get_cur_heap_count() == reply->item_count)
	{
		p_item->clear_to_item();
        item_grid[p_item->bag_index_] = NULL;
		item_factory::get_instance()->destroy_item(p_item);
		p_item = NULL;
	}
	else
	{
		p_item->dec_cur_heap_count(reply->item_count);
	}
	return true;
}


bool item_bag::set_grid_item(item* p_item, uint32_t grid_index)
{
	if( !(grid_index >=1 && grid_index <  max_grid_count) )return false;
	item_grid[grid_index] = p_item;

	if(p_item == NULL)return true;

	p_item->set_grid_index(grid_index);
	
	item* last_item = get_last_item(p_item->item_id());
	if(last_item)
	{
		last_item->to_next(p_item);
		p_item->to_prev(last_item);
	}
	return true;
}

item_ptr  item_bag::get_first_item(uint32_t item_id)
{
	for(uint32_t i = 1; i < max_grid_count; i++)
	{
		item* p_item = item_grid[i];
		if(p_item == NULL)continue;
		if(p_item->item_id() != item_id)continue;
		if(p_item->p_prev_item != NULL)continue;
		return p_item;
	}
	return NULL;
}

item_ptr item_bag::get_next_item(item* p_item)
{
	if(p_item)return p_item->p_next_item;
	return NULL;
}

item_ptr item_bag::get_last_item(uint32_t item_id)
{
	item* p_item = get_first_item(item_id);
	if(p_item == NULL)return NULL;
	while(p_item)
	{
		if(p_item->p_next_item)
		{
			return p_item;
		}
		p_item = p_item->p_next_item;
	}
	return NULL;
}

bool item_bag::check_valid_grid_index(uint32_t grid_index)
{
	if( !(grid_index >=1 && grid_index <  max_grid_count) )return false;
	return true;
}

uint32_t item_bag::get_item_count(uint32_t item_id)
{
	item* p_item = get_first_item(item_id);
	uint32_t item_count = 0;
	while(p_item)
	{
		item_count += p_item->get_cur_heap_count();
		p_item = get_next_item(p_item);
	}
	return item_count;
}

uint32_t item_bag::get_empty_grid_count()
{
	uint32_t count = 0;
	for(uint32_t i = 1; i < max_grid_count; i++)
	{
		if(item_grid[i] == NULL)count++;
	}
	return count;
}

int c_redeem_bag::add_item(item_ptr p_item)
{
    item_ptr new_item = 
        item_factory::get_instance()->create_item(
                p_item->id_, 
                p_item->item_id(), 
                p_item->get_cur_heap_count(),
                p_item->grid_index(),
                p_item->get_time(),
                p_item->get_expire_time());

    item_grid.push_back(new_item);
    return 0;
}

int c_redeem_bag::del_item(uint32_t index)
{
    if (index >= get_grid_count())
    {
        return -1;
    }

    item_ptr p_item = item_grid[index];
    p_item->clear_to_item();
    item_factory::get_instance()->destroy_item(p_item);
    item_grid.erase(item_grid.begin() + index);
    return 0;
}




bool init_player_item_bag(player_t* p, uint32_t max_grid_count)
{
	p->bag = new item_bag(max_grid_count);
	p->bag->init_bag();

    p->redeem_bag = new c_redeem_bag;
    p->redeem_bag->init();

	return true;	
}

bool final_player_item_bag(player_t* p)
{
	if (NULL != p->bag)
	{
        p->bag->final_bag();
        delete p->bag;
        p->bag = NULL;

    }

    if (NULL != p->redeem_bag)
    {
        p->redeem_bag->uninit();
        delete p->redeem_bag;
        p->redeem_bag = NULL;
    }
    return true;
}
