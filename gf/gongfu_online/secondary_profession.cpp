#include "secondary_profession.hpp"
#include "global_data.hpp"

/*----------------------------extern function----------------------------------------------*/
bool init_all_exp2level_calculator()
{
	//init_apothecary_exp2level_calculator();
	for(uint32_t i = begin_profession_type; i< end_profession_type; i++)
	{
		s_second_pro_calculator[i].init(i);	
	}
	return true;
}

/**
 *    @brief  init all the profession calculator 
 *    @param  profession type
 *    @return true sucess false otherwise fail
**/
bool final_all_exp2level_calculator()
{
	for(uint32_t i = begin_profession_type; i< end_profession_type; i++)
	{
		s_second_pro_calculator[i].final();	
	}
	return true;
}

/**
 *    @brief  init all the profession calculator 
 *    @param  profession type
 *    @return true sucess false otherwise fail
**/
base_exp2level_calculator* get_pothecary_exp2level_calc_by_type( profession_type type)
{
	if(!( type > begin_profession_type && type < end_profession_type)) return NULL;
	switch( type )
	{
		case apothecary_profession_type:
		case iron_man_profession_type:
		{
			return &s_second_pro_calculator[type];
		}
		break;

		default:
		{
			return NULL;
		}
	}
}



