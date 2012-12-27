#ifndef _COMPOSE_H_
#define _COMPOSE_H_

#include<string>
#include<vector>
#include<map>
using namespace std;


typedef struct input_item
{
	input_item()
	{
		item_id = 0;
		item_count = 0;
	}
	unsigned int item_id;
	unsigned int item_count;
}input_item;

typedef struct output_item
{
	output_item()
	{
		item_id = 0;
	}
	unsigned int item_id;
	string odds;
}output_item;

typedef struct method_data
{
	method_data()
	{
		method_id = 0;
		coins = 0;
		need_viatality = 0;
		add_pro_exp = 0;
		need_pro_lv = 0;
		role_type = 0;
		broadcast = 0;
		memset(name, 0, sizeof(name));
	}
	unsigned int method_id;
	unsigned int coins;
	unsigned need_viatality;
	unsigned add_pro_exp;
	unsigned need_pro_lv;
	unsigned broadcast;
	char     name[1024];
	unsigned role_type;
	vector<input_item> ins;
	vector<output_item> outs;
}method_data;

class method_mgr
{
public:
	bool init_from_excel(const char* excel);
	bool final_to_xml(const char* xml);
public:
	bool add_data(method_data& data);
private:
	map<unsigned int, method_data> m_maps;
};


















#endif