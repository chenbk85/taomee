/** 
 * ========================================================================
 * @file calculator.hpp
 * @brief 各种计算器
 * @version 1.0
 * @date 2012-02-20
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_CALCULATOR_H_2012_02_20
#define H_CALCULATOR_H_2012_02_20

#include <stdlib.h>
#include <stdint.h>
#include <map>
#include <vector>

#include <libtaomee++/random/random.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>



// 概率计算器
template < class key_t, class compare = std::less<key_t> >
class c_prob_calculator
{
    public:
        c_prob_calculator()
        {
            reset();
        }

        ~c_prob_calculator()
        {
            reset();
        }

        void insert(key_t key, uint32_t prob)
        {
            m_data.insert(std::pair<key_t, uint32_t>(key, prob));
            m_total_prob += prob;
        }

        void reset()
        {
            m_data.clear();
            m_total_prob = 0;
        }


        const key_t * calc() const
        {
            if (0 == m_total_prob)
            {
                return NULL;
            }

            uint32_t r = taomee::ranged_random(0, m_total_prob - 1);

            typename prob_map_t::const_iterator it = m_data.begin();
            while (it != m_data.end())
            {
                if (r <= it->second)
                {
                    return &(it->first);
                }

                r -= it->second;
                it++;
            }

            return NULL;
        }


    private:

        typedef std::map<key_t, uint32_t> prob_map_t;


    private:

        uint32_t m_total_prob;
        prob_map_t m_data;

};


#endif
