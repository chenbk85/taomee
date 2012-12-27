//-----------------------------------//
//-----------------------------------//

#ifndef _POP_H_MINGLIN_XUU_
#define _POP_H_MINGLIN_XUU_

///----------------------------------//
//-----------------------------------//

#define INVALID_POP_ID  0xF0F0

///----------------------------------//
//-----------------------------------//
//note:unless the pop_type is pop_type_normal or pop_type_colorful,the pop_color is no use!

typedef enum {
    pop_color_red = 1,
    pop_color_blue = 2,
    pop_color_yellow = 3,
    //pop_color_orange = 4,
    pop_color_green = 5,
    pop_color_unknown = 6,
} pop_color_t;

//-----------------------------------//
//-----------------------------------//
//warning: unless the pop_status = pop_status_present,the pop_type is no use!

typedef enum {
    pop_type_normal = 1,
    pop_type_colorful = 2,
    pop_type_bomb = 3,
    pop_type_unknown = 4,
} pop_type_t;

//-----------------------------------//
//-----------------------------------//

typedef enum {
    pop_status_null = 1,
    pop_status_present = 2,
    pop_status_erased = 3,
    pop_status_for_shoot = 4,
} pop_status_t;

//-----------------------------------//
//-----------------------------------//

typedef struct {
    unsigned char x;
    unsigned char y;
} pop_pos_t;

//-----------------------------------//
//-----------------------------------//

class c_pop_t
{
public:
    c_pop_t();
    ~c_pop_t();

    int set_pop_color(pop_color_t pop_color);
    int set_pop_type(pop_type_t pop_type);
    int set_pop_status(pop_status_t pop_status);
    int set_pop_pos(pop_pos_t& pop_pos);

    int get_pop_color(pop_color_t* p_pop_color);
    int get_pop_type(pop_type_t* p_pop_type);
    int get_pop_status(pop_status_t* p_pop_status);
    int get_pop_pos(pop_pos_t* p_pop_pos);

    unsigned short get_pop_id();

    bool is_same_color(c_pop_t& pop);
    bool is_colorful_pop();
    bool is_present();

    bool is_neighbor(c_pop_t& pop);

    int reset_pop_data();

    int set_user_data(int user_data);
    int get_user_data();

    static int generate_random_color(pop_color_t* p_pop_color);

private:
    unsigned short m_pop_id;   //if pop is normal pop or colorful pop,high word of pop id is col,low word is row

    pop_color_t m_pop_color;
    pop_type_t m_pop_type;
    pop_status_t m_pop_status;

    int m_user_data;
};

//-----------------------------------//
//-----------------------------------//

unsigned char high_word(unsigned short word);
unsigned char low_word(unsigned short word);
unsigned short make_word(unsigned char high_word,unsigned char low_word);

//-----------------------------------//
//-----------------------------------//

#endif//_POP_H_MINGLIN_XUU_

//-----------------------------------//
//-----------------------------------//
