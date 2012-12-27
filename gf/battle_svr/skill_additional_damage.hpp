#ifndef _SKILL_ADDITIONAL_DAMAGE_H_
#define _SKILL_ADDITIONAL_DAMAGE_H_


#include<vector>

extern "C" {
#include <stdint.h>
}

#include <libtaomee++/conf_parser/xmlparser.hpp>
#include "fwd_decl.hpp"

using namespace std;

#define RABBIT ("conf/rabbit_skill_add_damage.xml")
#define MONKEY ("conf/monkey_skill_add_damage.xml")
#define PANDA  ("conf/panda_skill_add_damage.xml")
#define DROGON ("conf/drogon_skill_add_damage.xml")


typedef struct additional_data
{
	additional_data()
	{
		memset(damage, 0, sizeof(damage));
	}
	uint32_t damage[5];
}additional_data;


class skill_add_damage_mgr
{
private:
	skill_add_damage_mgr();
	~skill_add_damage_mgr();
public:
	static skill_add_damage_mgr*  getInstance();
public:
	bool init(const char* xml_name);
	bool init();
	bool final();
	std::vector<additional_data>* get_data_by_name(const char* name);
	std::vector<additional_data>* get_data_by_role_type(uint32_t role_type);

private:
	std::vector<additional_data>  m_rabbit_data;
	std::vector<additional_data>  m_panda_data;
	std::vector<additional_data>  m_monkey_data;
	std::vector<additional_data>  m_drogon_data;
};

uint32_t get_addition_damage_by_index( uint32_t role_type,  uint32_t add_type,  uint32_t add_start, uint32_t add_step, uint32_t skill_lv);

#endif
