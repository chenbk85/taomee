#ifndef SECONDARY_PROFESSION_H_
#define SECONDARY_PROFESSION_H_

#include <vector>
#include <stdint.h>
//secondary profession type
enum profession_type
{
	begin_profession_type = 0,
	apothecary_profession_type = 1,
	iron_man_profession_type = 2,
	end_profession_type
};

/*--------------------------------------------------------------------------------*/

typedef struct exp2level_data
{
	exp2level_data()
	{
		exp = 0;
		level = 0;
	}
	//profession exp value
	uint32_t exp;
	//profession level value
	uint32_t level;
}exp2level_data;


//this class is base class for calculator
class base_exp2level_calculator
{
public:
	//constructor
	base_exp2level_calculator(){}
	//destructor
	~base_exp2level_calculator(){}
	bool init(uint32_t pro_type)
	{
		switch(pro_type)
		{
			case apothecary_profession_type:
			{
				add_exp2level_data(1, 0);
				add_exp2level_data(2, 300);
				add_exp2level_data(3, 600);
				add_exp2level_data(4, 1000);
				add_exp2level_data(5, 1500);
				add_exp2level_data(6, 2100);
				add_exp2level_data(7, 2800);
			}
			break;

			case iron_man_profession_type:
			{
				add_exp2level_data(1, 0);
				add_exp2level_data(2, 300);
				add_exp2level_data(3, 600);
				add_exp2level_data(4, 1000);
				add_exp2level_data(5, 1500);
				add_exp2level_data(6, 2100);
				add_exp2level_data(7, 2800);
			}
			break;
		}
		return true;
	}
	bool final()
	{
		m_datas.clear();    
		return true;
	}
	// add exp2level data to calculator
	void add_exp2level_data(uint32_t level, uint32_t exp)
	{
		exp2level_data data;
		data.level = level;
		data.exp = exp;
		m_datas.push_back(data);
	}
	//calc exp to level  return the total exp 
	uint32_t calc_exp2level(uint32_t exp)
	{
		if( exp == 0)return 1;
		
		if(m_datas.size() == 0){
			return 0;
		}
		uint32_t max_level = m_datas.size(); 
		for(uint32_t i =0; i< max_level; i++)
		{
			if( exp < m_datas[i].exp)
			{
				return m_datas[i].level;
			}
		}
		return m_datas[ max_level - 1 ].level;
	}
	//get max level limit  of calculator
	uint32_t   get_max_level()
	{
		return m_datas.size();
	}
	//get max exp limit of calculator
	uint32_t   get_max_exp()
	{
		if(m_datas.size() == 0){
			return 0;
		}
		uint32_t max_level = m_datas.size();
		return m_datas[max_level - 1].exp;
	}
protected:	
	std::vector<exp2level_data> m_datas;
};

/**
 *  *    @brief  get exp2level calculator by one profession type
 *   *    @param  profession type
 *    *    @return point sucess otherwise null 
 *    **/
base_exp2level_calculator* get_pothecary_exp2level_calc_by_type( profession_type type);

/**
 *  *   @brief  init all the profession calculator 
 *   *   @param  profession type
 *    *   @return true sucess false otherwise fail
 *    **/
bool init_all_exp2level_calculator();


/**
 *  *   @brief  init all the profession calculator 
 *   *   @param  profession type
 *    *   @return true sucess false otherwise fail
 *    **/
bool final_all_exp2level_calculator();


/*---------------------------------------------------------------------*/
/*
class apothecary_exp2level: public base_exp2level_calculator
{
public:
	//overwrite this function of parent class 
	bool init()
	{
		add_exp2level_data(1, 0);
		add_exp2level_data(2, 300);
		add_exp2level_data(3, 600);
		add_exp2level_data(4, 1000);
		add_exp2level_data(5, 1500);
		add_exp2level_data(6, 2100);
		add_exp2level_data(7, 2800);
		return true;
	}
	//overwrite this function of parent class
	bool final()
	{
		m_datas.clear();	
		return true;
	}
};
*/

/*------------------------------------------------------------------------*/
//the secondary profession data class 
class secondary_profession_data
{
public:
	//constructor
	secondary_profession_data()
	{
		m_exp = 0;
		m_level = 0;
		m_type  = begin_profession_type;
		m_pCalc = 0;
	}
	//destructor
	~secondary_profession_data()
	{
		m_exp = 0;
		m_level = 0;
		m_type = begin_profession_type;
		m_pCalc = 0;
	}
public:
	// set profession data 
	void      set_profession_data(uint32_t level, uint32_t exp, profession_type type)
	{
		m_level = level;
		m_exp = exp;
		m_type = type;
		m_pCalc = get_pothecary_exp2level_calc_by_type(type); 
	}
	// get current level of profession
	uint32_t  get_profession_level()
	{
		return m_level;
	}
	// get current exp of profession
	uint32_t  get_profession_exp()
	{
		return m_exp;
	}
public:
	// calc level from exp 
	uint32_t  calc_level_by_exp()
	{
		m_level = m_pCalc->calc_exp2level(m_exp);
		return m_level;
	}
	// add exp for profession,  [para1: exp] which you need to add,  [para2: fact_add_exp] the count of exp you real add 
	uint32_t  add_profession_exp(uint32_t exp,  uint32_t& fact_add_exp)
	{
		uint32_t old_exp = m_exp;
		m_exp+= exp;
		if(m_exp >= m_pCalc->get_max_exp())
		{
			m_exp = m_pCalc->get_max_exp();
		}
		calc_level_by_exp();
		fact_add_exp = m_exp - old_exp;

		return m_exp;
	}
	// set profession exp and calc the level from exp
	uint32_t set_profession_exp(uint32_t exp)
	{	
		m_exp = exp;
		if(m_exp >= m_pCalc->get_max_exp())
        {
			m_exp = m_pCalc->get_max_exp();				        
		}
        calc_level_by_exp();
        return m_exp;	
	}

public:
	// current exp of profession
	uint32_t   				m_exp;
	// current level of profession
	uint32_t   				m_level;
	// current profession type
	profession_type 		m_type;
	// current calc 
	base_exp2level_calculator *m_pCalc;
};





#endif
