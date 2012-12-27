#ifndef _CHECK_SHOP_H_
#define _CHECK_SHOP_H_

#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <vector>
#include <algorithm>
#include "utils.hpp"


typedef struct shop_data
{
	shop_data()
	{
		shop_id = 0;
	}
	
	bool is_item_exist(uint32_t item_id)
	{
		std::vector<uint32_t>::iterator pItr = std::find(items.begin(), items.end(),  item_id);
		return pItr != items.end();
	}
	
	uint32_t shop_id;
	std::vector<uint32_t> items;
}shop_data;


class shop_mgr
{
public:
	bool init(const char* xml);
	bool final();
	bool add_shop_data(shop_data* data);
	shop_data* get_shop_data_by_id(uint32_t shop_id);
	bool is_shop_data_exist(uint32_t shop_id);
private:	
	std::map<uint32_t,  shop_data*> m_datas;
};

bool check_npc_shop(const char* xml_name);
bool check_items_price(char* items_str);
bool check_npc_item(uint32_t npc_id, uint32_t item_id);


#endif
