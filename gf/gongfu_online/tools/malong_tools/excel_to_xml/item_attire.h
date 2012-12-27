#ifndef _ITEM_ATTIRE_H_
#define _ITEM_ATTIRE_H_

#include <vector>
#include <map>
using namespace std;



typedef struct item_attire_data
{
	item_attire_data()
	{
		memset(this, 0, sizeof(item_attire_data));
		trade_ability = 3;
		vip_trade_ability = 3;
		un_storage = 1;
		strcpy(descipt, "empty descript");
		strcpy(name, "empty name");
	}
	int  id; 
	char name[1024];
	int  droplv;
	int  quality_level;
	int  equip_part;
	int  price;
	int  sell_price;
	int  repair_price;
	int  uselv;
	int  strength;
	int  agility;
	int  body_quality;
	int  stamina;
	char atk[1024];
	int  def;
	int  duration;
	int  hit;
	int  dodge;
	int  crit;
	int  hp;
	int  mp;
	int  add_hp;
	int  add_mp;
	int  slot;
	int  trade_ability;		
	int  vip_trade_ability; 
	int  trade_able;
	int  dailyid;
	int  setid;
	int  exploit_value;
	int  honor_level;
	int  life_time;
	int  vip_only;
	int  decompose;
	int  shop;
	int  un_storage;
	int  res_id;
	char descipt[1024];
}item_attire_data;

typedef struct cat_items
{
	cat_items()
	{
		cat_id = 0;
		db_cat_id = 0;
		memset(name, sizeof(name), 0);
		max = 0;
	}
	bool is_item_id_exist(int id)
	{
		return item_map.find(id) != item_map.end();
	}
	bool add_item( item_attire_data& data)
	{
		if(is_item_id_exist(data.id))return false;
		item_map[data.id] = data;
		return true;
	}
	int cat_id;
	int db_cat_id;
	char name[1024];
	int max;
	map<int, item_attire_data>  item_map;
}cat_items;


class cat_items_mgr
{
public:
	bool is_cat_id_exist(int id)
	{
		return cat_items_map.find(id) != cat_items_map.end();
	}
	bool add_cat_items(cat_items& cat)
	{
		if(is_cat_id_exist(cat.cat_id))return false;
		cat_items_map[cat.cat_id] = cat;
		return true;
	}
	cat_items* get_cat_items_by_id(int cat_id)
	{
		std::map<int, cat_items>::iterator pItr = cat_items_map.find(cat_id);
		if(pItr == cat_items_map.end())return NULL;
		return &(pItr->second);
	}
public:
	std::map<int, cat_items> cat_items_map;
};


bool init_xml_file(const char* xml_name, cat_items_mgr& mgr);
bool final_xml_file(const char* xml_name, cat_items_mgr& mgr);

bool init_excel_file(const char* excel_name, std::vector<item_attire_data>& items);
bool final_excel_file(const char* excel_name);



bool merge_data(cat_items_mgr& dest, cat_items_mgr& mgr);

bool merge_data(cat_items_mgr& dest, std::vector<item_attire_data>& items);


#endif