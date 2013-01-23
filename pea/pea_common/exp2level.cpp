#include <map>
#include "exp2level.hpp"
#include "attr_config_data.hpp"
#include "pet_data.hpp"

using namespace std;

bool player_exp2level::init()
{
	const attr_config_data_map_t * p_map = attr_config_data_mgr::instance().get_object_map();
	if(p_map == NULL)return false;
	
	attr_config_data_map_t::const_iterator pItr = p_map->begin();
	for(; pItr != p_map->end(); ++pItr)
	{
		attr_config_data* p_data = pItr->second;
		add_exp2level_data(p_data->level_, p_data->exp_);	
	}
	return true;
}

bool pet_exp2level::init(uint32_t id)
{
    m_id = id;

    c_pet_config * p_pet = pet_config_mgr::instance().get_object(id);
    if (NULL == p_pet)
    {
        return false;
    }

    container_for_each(*p_pet, it)
    {
        pet_attr_config_t * p_data = it->second;
        add_exp2level_data(p_data->level_, p_data->exp_);
    }

    return true;
}


base_exp2level_calculator* exp2level_factory::create_calculator(uint32_t calc_type)
{
	base_exp2level_calculator* p_obj = NULL;
	
	switch(calc_type)
	{
		case PLAYER_CALC_TYPE:
			p_obj = new player_exp2level();
			p_obj->init();
            break;
        case PET_CALC_TYPE:
            p_obj = new pet_exp2level();
            p_obj->init();
            break;
        default:
            break;
	}
	return p_obj;
}

void exp2level_factory::destroy_calculator(base_exp2level_calculator* p_calc)
{
	delete p_calc;
}

base_exp2level_calculator* exp2level_factory::get_calculator(uint32_t calc_type)
{
	switch(calc_type)
	{
		case PLAYER_CALC_TYPE:
            {
                static player_exp2level obj;
                return &obj;
            }
            break;
        case PET_CALC_TYPE:
            {
                static pet_exp2level obj;
                return &obj;
            }
            break;
        default:
            return NULL;
	}
	return NULL;
}


exp2level_factory* exp2level_factory::getInstance()
{
	static exp2level_factory obj;
	return &obj;
}
