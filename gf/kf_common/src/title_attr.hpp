#ifndef _TITLE_ATTR_HPP_
#define _TITLE_ATTR_HPP_

#include <stdint.h>
//#include <stdlib.h>
#include <map>

using namespace std;

typedef struct title_attr_data
{
	title_attr_data()
	{
		id = 0;
        strength = 0;
        agility  = 0;
        body     = 0;
        stamina  = 0;

        hit      = 0;
        dodge    = 0;
        crit     = 0;

        atk      = 0;
        def      = 0;
        hp       = 0;
        mp       = 0;
        addhp    = 0;
        addmp    = 0;
	}
	uint32_t id;
	uint32_t strength;
	uint32_t agility;
	uint32_t body;
	uint32_t stamina;
	uint32_t hit;
	uint32_t dodge;
    uint32_t crit;
	uint32_t atk;
	uint32_t def;
    uint32_t hp;
    uint32_t mp;
    uint32_t addhp;
    uint32_t addmp;
}title_attr_data;

class title_attr_data_mgr
{
public:
    title_attr_data_mgr(){}
    ~title_attr_data_mgr(){}
public:
    //title_attr_data_mgr* getInstance();

	bool init(const char* xml);
	bool final();

    bool is_title_id_exist(uint32_t id);
    bool add_title_attr_data(title_attr_data* p_data);

    title_attr_data * get_title_attr_data(uint32_t id);
private:
	std::map<uint32_t, title_attr_data*> datas;
};


#endif
