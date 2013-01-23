#ifndef EXP2LEVEL_H_
#define EXP2LEVEL_H_

#include <vector>
#include <stdint.h>
#include <stdlib.h>

/*--------------------------------------------------------------------------------*/
enum exp2level_calc_type
{
    PLAYER_CALC_TYPE = 1,
    PET_CALC_TYPE = 2,
};



typedef struct exp2level_data
{
    exp2level_data()
    {
        exp = 0;
        level = 0;
    }
    uint32_t exp;
    uint32_t level;
}exp2level_data;


class base_exp2level_calculator
{
    public:
        base_exp2level_calculator(){}
        virtual ~base_exp2level_calculator(){}

        virtual bool init()
        {
            m_datas.clear();
            return true;
        }

        bool final()
        {
            m_datas.clear();    
            return true;
        }

        void add_exp2level_data(uint32_t level, uint32_t exp)
        {
            exp2level_data data;
            data.level = level;
            data.exp = exp;
            m_datas.push_back(data);
        }

        uint32_t calc_exp2level(uint32_t exp) const
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

        uint32_t get_level_exp(uint32_t level) const
        {
            for (uint32_t i = 0; i < m_datas.size(); i++)
            {
                if (level == m_datas[i].level)
                {
                    return m_datas[i].exp;
                }
            }
            return 0;
        }

        uint32_t get_max_level() const
        {
            return m_datas.size();
        }

        uint32_t get_max_exp() const
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



class player_exp2level: public base_exp2level_calculator
{
    public:
        bool init();
        /*
           {
           add_exp2level_data(1, 0);
           add_exp2level_data(2, 300);
           add_exp2level_data(3, 600);
           add_exp2level_data(4, 1000);
           add_exp2level_data(5, 1500);
           add_exp2level_data(6, 2100);
           add_exp2level_data(7, 2800);
           return true;
           }*/
};


class pet_exp2level : public base_exp2level_calculator
{
    public:

        pet_exp2level()
        {
            m_id = 0;
        }

        bool init(uint32_t id);

    public:

        uint32_t m_id;
};

// pet_exp2level_calc不用这个factory
class exp2level_factory
{
    private:
        exp2level_factory(){}
        ~exp2level_factory(){}
    public:
        base_exp2level_calculator* create_calculator(uint32_t calc_type);
        void                       destroy_calculator(base_exp2level_calculator* p_calc);
    public:
        base_exp2level_calculator* get_calculator(uint32_t calc_type);
    public:
        static exp2level_factory* getInstance();
};


#endif
