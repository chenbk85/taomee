/** 
 * ========================================================================
 * @file container.hpp
 * @brief 
 * @version 1.0
 * @date 2012-03-19
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_CONTAINER_H_2012_03_19
#define H_CONTAINER_H_2012_03_19


#include <stdlib.h>
#include <stdint.h>

#include <set>
#include <vector>
#include <map>
#include <libtaomee++/utils/noncopyable.hpp>




// 类似于ObjectRegistry的一个容器
template<typename key_t, typename value_t>
class c_object_container : public taomee::Noncopyable
{
    public:

        typedef std::map < key_t, value_t *> map_t;
        typedef typename map_t::const_iterator const_iterator_t;
        typedef typename map_t::iterator iterator_t;

    public:

        typedef typename map_t::size_type size_type;
        typedef int (*value_func_t)(value_t * p_value, void * param);


    public:

        // virtual c_object_container()
        // {
            // m_data.clear();
        // }

        virtual ~c_object_container()
        {
            iterator_t it = m_data.begin();
            while (it != m_data.end())
            {
                if (NULL != it->second)
                {
                    delete it->second;
                }
                it++;
            }

            m_data.clear();
        }

        // 若回调函数cb为NULL，则调用delete，销毁value对象
        virtual void uninit(value_func_t cb = NULL, void * param = NULL)
        {
            iterator_t it = m_data.begin();
            while (it != m_data.end())
            {
                if (NULL != it->second)
                {
                    if (NULL != cb)
                    {
                        cb(it->second, param);
                    }
                    else
                    {
                        delete it->second;
                    }
                }
                it++;
            }

            m_data.clear();

        }

        map_t * get_object_map()
        {
            return &m_data;
        }

        bool insert_object(const key_t & key, value_t * obj, bool override = false)
        {
            iterator_t it = m_data.find(key);
            if (it != m_data.end())
            {
                if (!override)
                {
                    return false;
                }
                delete it->second;
            }

            m_data[key] = obj;
            return true;
        }

        const value_t * get_const_object(const key_t & key) const
        {
            const_iterator_t it = m_data.find(key);
            if (it != m_data.end())
            {
                return it->second;
            }
            else
            {
                return NULL;
            }

        }


        value_t * get_object(const key_t & key)
        {
            iterator_t it = m_data.find(key);
            if (it != m_data.end())
            {
                return it->second;
            }
            else
            {
                return NULL;
            }

        }

        const key_t get_max_key()
        {

            typename map_t::const_reverse_iterator it = m_data.rbegin();
            if (it == m_data.rend())
            {
                return 0;
            }

            return (it->first);
        }

        // 若回调函数cb为NULL，则调用delete，销毁value对象
        virtual void remove_object(const key_t & key, value_func_t cb = NULL, void * param = NULL)
        {
            iterator_t it = m_data.find(key);
            if (it != m_data.end())
            {
                if (NULL != it->second)
                {
                    if (NULL != cb)
                    {
                        cb(it->second, param);
                    }
                    else
                    {
                        delete it->second;
                    }

                }

                m_data.erase(it);
            }
        }


        void destroy_object(const key_t & key)
        {
            iterator_t it = m_data.find(key);
            if (it != m_data.end())
            {
                if (it->second != NULL)
                {
                    delete it->second;
                }

                m_data.erase(it);
            }
        }


        bool has_object(const key_t & key) const
        {
            return m_data.find(key) != m_data.end();
        }


        size_type size() const
        {
            return m_data.size();
        }

        iterator_t begin()
        {
            return m_data.begin();
        }

        iterator_t end()
        {
            return m_data.end();
        }

        inline void next(iterator_t & it)
        {
            it++;
        }

    private:

        map_t m_data;
};


#define container_for_each(container, it) \
        for (typeof((container).begin()) it = (container).begin(); it != (container).end(); (container).next(it))



#define vector_for_each(container, it) \
        for (typeof((container).begin()) it = (container).begin(); it != (container).end(); it++)


#define vector_while_each(container, it) \
        for (typeof((container).begin()) it = (container).begin(); it != (container).end(); )


#define map_for_each(container, it) \
        for (typeof((container).begin()) it = (container).begin(); it != (container).end(); it++)


#endif
