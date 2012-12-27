///----------------------------------//
//-----------------------------------//

#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "my_errno.h"
#include "pop.h"

///----------------------------------//
//-----------------------------------//

#define null 0

///----------------------------------//
//-----------------------------------//

unsigned char high_word(unsigned short word)
{
    return ( word & 0xFF00 ) >> 8 ;
}

///----------------------------------//
//-----------------------------------//

unsigned char low_word(unsigned short word)
{
    return ( word & 0x00FF );
}

///----------------------------------//
//-----------------------------------//

unsigned short make_word(unsigned char high_word,unsigned char low_word)
{
    unsigned short temp_word = high_word;
    temp_word = temp_word << 8;

    return temp_word + low_word;
}

///----------------------------------//
//-----------------------------------//

c_pop_t::c_pop_t()
{
    reset_pop_data();
}

///----------------------------------//
//-----------------------------------//

c_pop_t::~c_pop_t()
{
}

///----------------------------------//
//-----------------------------------//

int c_pop_t::reset_pop_data()
{
    m_pop_id = INVALID_POP_ID;
    m_pop_color = pop_color_unknown;
    m_pop_type = pop_type_unknown;
    m_pop_status = pop_status_null;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//

int c_pop_t::set_pop_color(pop_color_t pop_color)
{
    m_pop_color = pop_color;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//
//note: the type of pop may be changed to color during game

int c_pop_t::set_pop_type(pop_type_t pop_type)
{
    m_pop_type = pop_type;

    return S_OK;
}

///----------------------------------//
//-----------------------------------//

int c_pop_t::set_pop_status(pop_status_t pop_status)
{
    m_pop_status = pop_status;

    return S_OK;
}

//-----------------------------------//
//-----------------------------------//
//note: the pos of pop is unchangeable during game

int c_pop_t::set_pop_pos(pop_pos_t& pop_pos)
{
    if(m_pop_id == INVALID_POP_ID)
    {
        m_pop_id = make_word(pop_pos.x,pop_pos.y);

        return S_OK;
    }
    else
    {
        return E_LOGIC;
    }
}

//-----------------------------------//
//-----------------------------------//

int c_pop_t::get_pop_color(pop_color_t* p_pop_color)
{
    //--------------------
    //param check
    if(null == p_pop_color)
    {
        return E_INVALIDARG;
    }

    *p_pop_color = m_pop_color;

    return S_OK;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_t::get_pop_type(pop_type_t* p_pop_type)
{
    //--------------------
    //param check
    if(null == p_pop_type)
    {
        return E_INVALIDARG;
    }

    *p_pop_type = m_pop_type;

    return S_OK;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_t::get_pop_status(pop_status_t* p_pop_status)
{
    //--------------------
    //param check
    if(null == p_pop_status)
    {
        return E_INVALIDARG;
    }

    *p_pop_status = m_pop_status;

    return S_OK;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_t::get_pop_pos(pop_pos_t* p_pop_pos)
{
    //--------------------
    //param check
    if(null == p_pop_pos)
    {
        return E_INVALIDARG;
    }

    p_pop_pos->x = high_word(m_pop_id);
    p_pop_pos->y = low_word(m_pop_id);

    return S_OK;
}

//-----------------------------------//
//-----------------------------------//

unsigned short c_pop_t::get_pop_id()
{
    return m_pop_id;
}

//-----------------------------------//
//-----------------------------------//

bool c_pop_t::is_same_color(c_pop_t& pop)
{
    pop_color_t pop_color = pop_color_unknown;
    pop.get_pop_color(&pop_color);

    if(pop_color == m_pop_color)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------//
//-----------------------------------//

bool c_pop_t::is_colorful_pop()
{
    if(m_pop_type == pop_type_colorful)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------//
//-----------------------------------//

bool c_pop_t::is_present()
{
    if(m_pop_status == pop_status_present)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------//
//-----------------------------------//

bool c_pop_t::is_neighbor(c_pop_t& pop)
{
    return false;// not impl
}

//-----------------------------------//
//-----------------------------------//

int c_pop_t::generate_random_color(pop_color_t* p_pop_color)
{
    //--------------------
    //param check
    if(null == p_pop_color)
    {
        return E_INVALIDARG;
    }

    //--------------------
    //get a random num
    timeval current_time;
    gettimeofday(&current_time,null);

    srand(current_time.tv_sec * 1000 + current_time.tv_usec);

    int rand_num = rand();

    if(rand_num % 4 == 0)
    {
        *p_pop_color = pop_color_red;
    }
    else if(rand_num % 4 == 1)
    {
        *p_pop_color = pop_color_blue;
    }
    else if(rand_num % 4 == 2)
    {
        *p_pop_color = pop_color_yellow;
    }
    //else if(rand_num % 4 == 3)
    //{
    //    *p_pop_color = pop_color_orange;
    //}
    else if(rand_num % 4 == 3)
    {
        *p_pop_color = pop_color_green;
    }
    else
    {
        assert(false);
    }

    return S_OK;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_t::set_user_data(int user_data)
{
    m_user_data = user_data;

    return 0;
}

//-----------------------------------//
//-----------------------------------//

int c_pop_t::get_user_data()
{
    return m_user_data;
}

//-----------------------------------//
//-----------------------------------//
