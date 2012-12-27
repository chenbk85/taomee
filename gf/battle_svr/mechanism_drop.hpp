#ifndef _MECHANISM_DROP_H_
#define _MECHANISM_DROP_H_

extern "C" {
#include <stdint.h>
}

#include <map>
#include <vector>
#include <libtaomee++/conf_parser/xmlparser.hpp>

#include "object.hpp"
#include "fwd_decl.hpp"

using namespace taomee;
using namespace std;


typedef struct mechanism_item_drop_t
{
	mechanism_item_drop_t()
	{
		item_id = 0;
		begin_odds = 0;
		end_odds = 0;
	}
	
	uint32_t item_id;
	uint32_t begin_odds;
	uint32_t end_odds;
}mechanism_item_drop_t;


typedef struct mechanism_monster_drop_t
{
	mechanism_monster_drop_t()
	{
		monster_id = 0;
		begin_odds = 0;
		end_odds = 0;
	}
	uint32_t monster_id;
	uint32_t begin_odds;
	uint32_t end_odds;
}mechanism_monster_drop_t;


typedef struct mechanism_drop_data
{
	mechanism_drop_data()
	{
		drop_id = 0;
		repeat_count = 0;
	}
	int drop_id;
	uint32_t repeat_count;
	std::vector<mechanism_item_drop_t> item_drops;
	std::vector<mechanism_monster_drop_t> monster_drops;
}mechanism_drop_data;



class mechanism_drop_mgr
{
private:
	mechanism_drop_mgr(){}
public:
	static mechanism_drop_mgr* getInstance();
public:
	bool init(const char* xml_name);
	bool final();
	mechanism_drop_data* get_mechanism_drop_by_id(uint32_t id);
	bool add_mechanism_drop_data(mechanism_drop_data* p);
	bool is_mechanism_drop_id_exist(uint32_t id);
private:
	std::map<uint32_t, mechanism_drop_data*> datas;
};


#endif
